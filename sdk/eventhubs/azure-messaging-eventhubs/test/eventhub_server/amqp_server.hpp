// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Fake out the declarations for the AMQP library so that we can use the mock server.
// The AMQP functionality enabling the mock server is conditionaled on the TESTING_BUILD define.
// The Mock Server also needs to be named Azure::Core::Amqp::Tests::AmqpServerMock to be used.
#define TESTING_BUILD 1
#include <azure/core/amqp/claims_based_security.hpp>
#include <azure/core/amqp/connection.hpp>
#include <azure/core/amqp/message_receiver.hpp>
#include <azure/core/amqp/message_sender.hpp>
#include <azure/core/amqp/models/amqp_error.hpp>
#include <azure/core/amqp/models/amqp_message.hpp>
#include <azure/core/amqp/models/amqp_protocol.hpp>
#include <azure/core/amqp/models/message_source.hpp>
#include <azure/core/amqp/models/message_target.hpp>
#include <azure/core/amqp/models/messaging_values.hpp>
#include <azure/core/amqp/network/amqp_header_detect_transport.hpp>
#include <azure/core/amqp/network/socket_listener.hpp>
#include <azure/core/amqp/session.hpp>
#include <azure/core/internal/diagnostics/log.hpp>

#include <memory>

namespace Azure { namespace Core { namespace Amqp { namespace Tests {

  extern uint16_t FindAvailableSocket();
  namespace MessageTests {

    class NamespaceHandler {
    protected:
      std::unique_ptr<Azure::Core::Amqp::_internal::MessageReceiver> m_messageReceiver;
      std::unique_ptr<Azure::Core::Amqp::_internal::MessageSender> m_messageSender;
      Azure::Core::Amqp::Common::_internal::AsyncOperationQueue<
          Azure::Core::Amqp::Models::AmqpMessage>
          m_incomingMessageQueue;

      Azure::Core::Amqp::Common::_internal::AsyncOperationQueue<
          Azure::Core::Amqp::Models::AmqpMessage>
          m_outgoingMessageQueue;

      void QueueMessageForProcessing(Azure::Core::Amqp::Models::AmqpMessage const& message)
      {
        m_incomingMessageQueue.CompleteOperation(message);
      }

    public:
      virtual ~NamespaceHandler() = default;

      virtual void ProcessMessages(Azure::Core::Context const& context)
      {
        if (context.IsCancelled())
        {
          return;
        }
        if (m_messageReceiver)
        {
          while (!m_incomingMessageQueue.Empty())
          {
            std::unique_ptr<std::tuple<Azure::Core::Amqp::Models::AmqpMessage>> incomingMessage
                = m_incomingMessageQueue.WaitForPolledResult(context);
            if (incomingMessage)
            {
              ProcessMessage(std::get<0>(*incomingMessage), context);
            }
          }
        }
        if (m_messageSender)
        {
          // Drain the outgoing message queue of messages on this namespace.
          while (!m_outgoingMessageQueue.Empty())
          {
            std::unique_ptr<std::tuple<Azure::Core::Amqp::Models::AmqpMessage>> outgoingMessage
                = m_outgoingMessageQueue.WaitForPolledResult(context);
            if (outgoingMessage)
            {
              m_messageSender->Send(std::get<0>(*outgoingMessage));
            }
          }
        }
      }

      void SetMessageReceiver(
          std::unique_ptr<Azure::Core::Amqp::_internal::MessageReceiver>&& receiver)
      {
        m_messageReceiver = std::move(receiver);
      }

      void SetMessageSender(std::unique_ptr<Azure::Core::Amqp::_internal::MessageSender>&& sender)
      {
        m_messageSender = std::move(sender);
      }

      /** The OnMessageReceived callback is called when a message is received on the
       * messageReceiver. The callback should return an AmqpValue that will be sent back to the
       * client. This gives the server the ability to set the disposition of an incoming message if
       * it is considered invalid.
       */
      virtual Models::AmqpValue ValidateIncomingMessage(Models::AmqpMessage const& message) = 0;

      /** The ProcessMessage method is called when a message is received on the messageReceiver.
       * This method should process the message and send a response back to the client.
       */
      virtual void ProcessMessage(Models::AmqpMessage const& message, Context const& context) = 0;
    };

    class CbsNamespaceHandler : public NamespaceHandler {
      bool m_forceCbsError{false};

    public:
      CbsNamespaceHandler() = default;
      CbsNamespaceHandler(bool forceCbsError) : m_forceCbsError(forceCbsError) {}

    private:
      virtual Models::AmqpValue ValidateIncomingMessage(const Models::AmqpMessage& message) override
      {
        if (IsValidCbsMessage(message))
        {
          // Queue the message to be processed by the ProcessMessage method.
          QueueMessageForProcessing(message);
          return Models::_internal::Messaging::DeliveryAccepted();
        }
        return Models::_internal::Messaging::DeliveryRejected(
            Models::_internal::AmqpErrorCondition::InvalidField.ToString(),
            "The CBS message was not valid for this namespace.");
      }

      bool IsValidCbsMessage(Models::AmqpMessage const& message)
      {
        if (!message.ApplicationProperties.empty())
        {
          Models::AmqpValue operation = message.ApplicationProperties.at("operation");
          Models::AmqpValue type = message.ApplicationProperties.at("type");

          // If we're processing a put-token message, then we should get a "type" and "name"
          // value.

          if (operation.GetType() != Models::AmqpValueType::String)
          {
            return false;
          }
          if (static_cast<std::string>(operation) == "put-token")
          {
            // The body of a put-token operation MUST be an AMQP AmqpValue.
            if (message.BodyType != Models::MessageBodyType::Value)
            {
              return false;
            }
            return true;
          }
          else if (static_cast<std::string>(operation) == "delete-token")
          {
            return true;
          }
        }
        return false;
      }
      void ProcessMessage(Models::AmqpMessage const& message, Context const& context) override
      {
        Models::AmqpValue operation = message.ApplicationProperties.at("operation");
        Models::AmqpValue type = message.ApplicationProperties.at("type");
        Models::AmqpValue name = message.ApplicationProperties.at("name");
        // If we're processing a put-token message, then we should get a "type" and "name"
        // value.
        if (static_cast<std::string>(operation) == "put-token")
        {
          // Respond to the operation.
          Models::AmqpMessage response;
          Models::MessageProperties responseProperties;

          // Management specification section 3.2: The correlation-id of the response message
          // MUST be the correlation-id from the request message (if present), else the
          // message-id from the request message.
          Azure::Nullable<Models::AmqpValue> requestCorrelationId
              = message.Properties.CorrelationId;
          if (!message.Properties.CorrelationId.HasValue())
          {
            requestCorrelationId = message.Properties.MessageId.Value();
          }
          response.Properties.CorrelationId = requestCorrelationId;

          // Populate the response application properties.

          if (m_forceCbsError)
          {
            response.ApplicationProperties["status-code"] = 500;
            response.ApplicationProperties["status-description"] = "Internal Server Error";
          }
          else
          {
            response.ApplicationProperties["status-code"] = 200;
            response.ApplicationProperties["status-description"] = "OK-put";
          }

          response.SetBody(Models::AmqpValue());

          // Set the response body type to an empty AMQP value.
          if (context.IsCancelled())
          {
            return;
          }

          // Queue the response to be sent.
          m_outgoingMessageQueue.CompleteOperation(response);
        }
        else if (static_cast<std::string>(operation) == "delete-token")
        {
          Models::AmqpMessage response;
          Models::MessageProperties responseProperties;

          // Management specification section 3.2: The correlation-id of the response message
          // MUST be the correlation-id from the request message (if present), else the
          // message-id from the request message.
          Azure::Nullable<Models::AmqpValue> requestCorrelationId
              = message.Properties.CorrelationId;
          if (!message.Properties.CorrelationId.HasValue())
          {
            requestCorrelationId = message.Properties.MessageId;
          }
          response.Properties.CorrelationId = requestCorrelationId;
          response.ApplicationProperties["status-code"] = 200;
          response.ApplicationProperties["status-description"] = "OK-delete";

          response.SetBody(Models::AmqpValue());

          // Set the response body type to an empty AMQP value.
          if (context.IsCancelled())
          {
            return;
          }

          m_outgoingMessageQueue.CompleteOperation(response);
        }
      }
    };

    class AmqpServerMock : public Network::_internal::SocketListenerEvents {
      struct MessageLinkComponents
      {
        void Reset()
        {
          if (m_linkSender)
          {
            m_linkSender.reset();
          }
          if (m_linkReceiver)
          {
            m_linkReceiver.reset();
          }
        }
        std::unique_ptr<_internal::MessageSender> m_linkSender;
        std::unique_ptr<_internal::MessageReceiver> m_linkReceiver;
        Common::_internal::AsyncOperationQueue<std::unique_ptr<Models::AmqpMessage>> m_messageQueue;
        Common::_internal::AsyncOperationQueue<bool> m_messageReceiverPresentQueue;
        Common::_internal::AsyncOperationQueue<bool> m_messageSenderPresentQueue;
      };

    public:
      AmqpServerMock() { m_testPort = FindAvailableSocket(); }
      AmqpServerMock(uint16_t listeningPort) : m_testPort{listeningPort} {}

      bool WaitForConnection(
          Network::_internal::SocketListener const& listener,
          Azure::Core::Context const& context = {})
      {
        auto result = m_connectionQueue.WaitForPolledResult(context, listener);
        return result != nullptr;
      }

      uint16_t GetPort() const { return m_testPort; }
      Azure::Core::Context& GetListenerContext() { return m_listenerContext; }

      class ServerConnection : public _internal::ConnectionEvents,
                               public _internal::SessionEvents,
                               public _internal::MessageReceiverEvents,
                               public _internal::MessageSenderEvents {

      public:
        ServerConnection(Network::_internal::Transport const& transport, AmqpServerMock* parentMock)
            : m_parentServer{parentMock}, m_connectionValid{true}
        {
          _internal::ConnectionOptions options;
          options.ContainerId = "connectionId";
          options.EnableTrace = true;
          m_connection = std::make_shared<_internal::Connection>(transport, options, this);
        }

        ServerConnection(ServerConnection&& other) = default;

        virtual void Poll() const {}

        void StartProcessing()
        {
          std::mutex threadRunningMutex;
          std::condition_variable threadStarted;
          bool running = false;

          // Start a thread waiting on incoming connections.
          m_connectionThread = std::thread([this, &threadStarted, &running]() {
            Azure::Core::Diagnostics::_internal::Log::Stream(
                Azure::Core::Diagnostics::Logger::Level::Informational)
                << "Start connection Listener ";
            m_connection->Listen();
            running = true;
            threadStarted.notify_one();

            while (!m_parentServer->GetListenerContext().IsCancelled())
            {
              std::this_thread::yield();
              while (m_connectionValid && !m_parentServer->GetListenerContext().IsCancelled())
              {
                std::this_thread::yield();

                m_connection->Poll();
                for (const auto& val : m_linkMessageQueues)
                {
                  // if (!val.second.LinkReceiver)
                  //{
                  //   Azure::Core::Diagnostics::_internal::Log::Stream(
                  //       Azure::Core::Diagnostics::Logger::Level::Informational)
                  //       << "Wait for message receiver for " << val.first;

                  //  if (!WaitForMessageReceiver(val.first, m_listenerContext))
                  //  {
                  //    Azure::Core::Diagnostics::_internal::Log::Stream(
                  //        Azure::Core::Diagnostics::Logger::Level::Informational)
                  //        << "Message Receiver failed, cancelling thread.";
                  //    return;
                  //  }
                  //}
                  // if (!val.second.LinkSender)
                  //{
                  //  Azure::Core::Diagnostics::_internal::Log::Stream(
                  //      Azure::Core::Diagnostics::Logger::Level::Informational)
                  //      << "Wait for message sender for " << val.first;
                  //  if (!WaitForMessageSender(val.first, m_listenerContext))
                  //  {
                  //    Azure::Core::Diagnostics::_internal::Log::Stream(
                  //        Azure::Core::Diagnostics::Logger::Level::Informational)
                  //        << "Message sender failed, cancelling thread.";
                  //    return;
                  //  }
                  //}
                  ProcessMessageOnNode(val.first, val.second);
                }
                for (auto const& ns : m_parentServer->m_namespaceHandlers)
                {
                  ns.second->ProcessMessages(m_parentServer->GetListenerContext());
                }
              }
            }
          });

          // Wait until our running thread is actually listening before we return.
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Wait 10 seconds for connection listener to start.";
          std::unique_lock<std::mutex> waitForThreadStart(threadRunningMutex);
          threadStarted.wait_until(
              waitForThreadStart,
              std::chrono::system_clock::now() + std::chrono::seconds(10),
              [&running]() { return running == true; });
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Listener running.";
        }
        void StopProcessing()
        {
          if (m_connectionThread.joinable())
          {
            m_connectionThread.join();
          }
          for (auto& val : m_linkMessageQueues)
          {
            val.second.Reset();
          }
          if (m_session)
          {
            m_session.reset();
          }
          if (m_connection)
          {
            m_connection.reset();
          }
        }
        virtual void OnConnectionStateChanged(
            _internal::Connection const&,
            _internal::ConnectionState newState,
            _internal::ConnectionState oldState) override
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Connection State changed. Old state: " << ConnectionStateToString(oldState)
              << " New state: " << ConnectionStateToString(newState);
          if (newState == _internal::ConnectionState::End
              || newState == _internal::ConnectionState::Error)
          {
            // If the connection is closed, then we should close the connection.
            m_connectionValid = false;
          }
        }

        virtual bool OnNewEndpoint(
            _internal::Connection const& connection,
            _internal::Endpoint& endpoint) override
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "OnNewEndpoint - Incoming endpoint created, create session.";
          _internal::SessionOptions options;
          options.InitialIncomingWindowSize = 10000;

          m_session = std::make_shared<_internal::Session>(
              connection.CreateSession(endpoint, options, this));
          m_session->Begin();
          return true;
        }
        virtual void OnIOError(_internal::Connection const&) override
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "On I/O Error - connection closed.";
          m_connectionValid = false;
        }

        // Inherited via Session
        virtual bool OnLinkAttached(
            _internal::Session const& session,
            _internal::LinkEndpoint& newLinkInstance,
            std::string const& name,
            _internal::SessionRole role,
            Models::AmqpValue const& source,
            Models::AmqpValue const& target,
            Models::AmqpValue const&) override
        {
          Models::_internal::MessageSource msgSource(source);
          Models::_internal::MessageTarget msgTarget(target);

          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "OnLinkAttached. Source: " << msgSource << " Target: " << msgTarget;
          std::string targetAddress = static_cast<std::string>(msgTarget.GetAddress());
          std::string sourceAddress = static_cast<std::string>(msgSource.GetAddress());

          // If the incoming role is receiver, then we want to create a sender to talk to it.
          // Similarly, if the incoming role is sender, we want to create a receiver to receive
          // from it.
          if (role == _internal::SessionRole::Receiver)
          {

            _internal::MessageSenderOptions senderOptions;
            senderOptions.EnableTrace = true;
            senderOptions.Name = name;
            senderOptions.MessageSource = msgSource;
            senderOptions.InitialDeliveryCount = 0;

            // If there's a handler registered for this source address, then create and register a
            // message sender for that namespace, otherwise hook it up to the generic sender
            // queue.
            if (m_parentServer->m_namespaceHandlers.find(sourceAddress)
                != m_parentServer->m_namespaceHandlers.end())
            {
              auto messageSender = std::make_unique<_internal::MessageSender>(
                  session.CreateMessageSender(newLinkInstance, targetAddress, senderOptions, this));
              messageSender->Open();
              m_parentServer->m_namespaceHandlers[sourceAddress]->SetMessageSender(
                  std::move(messageSender));
            }
            else
            {
              MessageLinkComponents& linkComponents = m_linkMessageQueues[static_cast<std::string>(
                  senderOptions.MessageSource.GetAddress())];

              if (!linkComponents.m_linkSender)
              {
                linkComponents.m_linkSender
                    = std::make_unique<_internal::MessageSender>(session.CreateMessageSender(
                        newLinkInstance, targetAddress, senderOptions, this));
                linkComponents.m_linkSender->Open();
                linkComponents.m_messageSenderPresentQueue.CompleteOperation(true);
              }
            }
          }
          else if (role == _internal::SessionRole::Sender)
          {
            _internal::MessageReceiverOptions receiverOptions;
            receiverOptions.EnableTrace = true;
            receiverOptions.Name = name;
            receiverOptions.MessageTarget = msgTarget;
            receiverOptions.InitialDeliveryCount = 0;
            if (m_parentServer->m_namespaceHandlers.find(sourceAddress)
                != m_parentServer->m_namespaceHandlers.end())
            {
              auto messageReceiver
                  = std::make_unique<_internal::MessageReceiver>(session.CreateMessageReceiver(
                      newLinkInstance, sourceAddress, receiverOptions, this));
              messageReceiver->Open();
              m_parentServer->m_namespaceHandlers[sourceAddress]->SetMessageReceiver(
                  std::move(messageReceiver));
            }
            else
            {
              MessageLinkComponents& linkComponents = m_linkMessageQueues[static_cast<std::string>(
                  receiverOptions.MessageTarget.GetAddress())];
              if (!linkComponents.m_linkReceiver)
              {
                linkComponents.m_linkReceiver
                    = std::make_unique<_internal::MessageReceiver>(session.CreateMessageReceiver(
                        newLinkInstance, sourceAddress, receiverOptions, this));

                linkComponents.m_linkReceiver->Open();
              }
              linkComponents.m_messageReceiverPresentQueue.CompleteOperation(true);
            }
          }

          return true;
        }

        // Inherited via MessageReceiverEvents
        void OnMessageReceiverStateChanged(
            _internal::MessageReceiver const&,
            _internal::MessageReceiverState newState,
            _internal::MessageReceiverState oldState) override
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Message Receiver State changed. Old state: " << ReceiverStateToString(oldState)
              << " New state: " << ReceiverStateToString(newState);
        }

        Models::AmqpValue OnMessageReceived(
            _internal::MessageReceiver const& receiver,
            Models::AmqpMessage const& message) override
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Received a message " << message;
          if (m_parentServer->m_namespaceHandlers.find(receiver.GetSourceName())
              != m_parentServer->m_namespaceHandlers.end())
          {
            // If there's a handler registered for this source address, then pass the message to it
            // to validate the incoming message. If the message is not valid, the handler will
            // return an appropriate messag disposition to send to the sender. If the message is
            // valid, it will equeue the incoming message for later processing.
            return m_parentServer->m_namespaceHandlers[receiver.GetSourceName()]
                ->ValidateIncomingMessage(message);
          }
          m_linkMessageQueues[receiver.GetSourceName()].m_messageQueue.CompleteOperation(
              std::make_unique<Models::AmqpMessage>(message));
          return Models::_internal::Messaging::DeliveryAccepted();
        }

        // Inherited via MessageSenderEvents
        void OnMessageSenderStateChanged(
            _internal::MessageSender const&,
            _internal::MessageSenderState newState,
            _internal::MessageSenderState oldState) override
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Message Sender State changed. Old state: " << SenderStateToString(oldState)
              << " New state: " << SenderStateToString(newState);
        }

        void OnMessageSenderDisconnected(Models::_internal::AmqpError const& error) override
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Message Sender Disconnected: Error: " << error;
        }
        virtual void OnMessageReceiverDisconnected(
            Models::_internal::AmqpError const& error) override
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Message receiver disconnected: " << error << std::endl;
        }
        _internal::MessageReceiver* GetMessageReceiver(
            std::string const& nodeName,
            Azure::Core::Context const& context = {})
        {
          if (m_linkMessageQueues.find(nodeName) == m_linkMessageQueues.end())
          {
            return nullptr;
          }
          if (m_linkMessageQueues[nodeName].m_linkReceiver == nullptr)
          {
            auto result
                = m_linkMessageQueues[nodeName].m_messageReceiverPresentQueue.WaitForPolledResult(
                    context, *this, *m_connection);
            if (!result)
            {
              return nullptr;
            }
          }
          return m_linkMessageQueues[nodeName].m_linkReceiver.get();
        }
        _internal::MessageSender* GetMessageSender(
            std::string const& nodeName,
            Azure::Core::Context const& context = {})
        {
          if (m_linkMessageQueues.find(nodeName) == m_linkMessageQueues.end())
          {
            return nullptr;
          }
          if (m_linkMessageQueues[nodeName].m_linkSender == nullptr)
          {
            auto result
                = m_linkMessageQueues[nodeName].m_messageSenderPresentQueue.WaitForPolledResult(
                    context, *this, *m_connection);
            if (!result)
            {
              return nullptr;
            }
          }
          return m_linkMessageQueues[nodeName].m_linkSender.get();
        }
        std::unique_ptr<Models::AmqpMessage> WaitForMessage(std::string const& nodeName)
        {
            if (m_linkMessageQueues[nodeName].m_messageQueue.Empty())
            {
            return nullptr;
          }
          // Poll for completion on both the mock server and the connection, that ensures that
          // we can implement unsolicited sends from the Poll function.
          try
          {
            auto result = m_linkMessageQueues[nodeName].m_messageQueue.WaitForPolledResult(
                m_parentServer->m_listenerContext, *m_connection, *this);
            if (result)
            {
              return std::move(std::get<0>(*result));
            }
            else
            {
              return nullptr;
            }
          }
          catch (std::runtime_error const& ex)
          {
            Azure::Core::Diagnostics::_internal::Log::Stream(
                Azure::Core::Diagnostics::Logger::Level::Warning)
                << "Exception thrown waiting for incoming message: " << ex.what();
            return nullptr;
          }
        }

        /** @brief Override for non CBS message receive operations which allows a specialization
         * to customize the behavior for received messages.
         */
        virtual void MessageReceived(
            std::string const&,
            MessageLinkComponents const&,
            Models::AmqpMessage const&) const {};

        void ProcessMessageOnNode(
            std::string const& nodeName,
            MessageLinkComponents const& linkComponents)
        {
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Wait for incoming message.";
          auto message = WaitForMessage(nodeName);
          if (!message)
          {
            Azure::Core::Diagnostics::_internal::Log::Stream(
                Azure::Core::Diagnostics::Logger::Level::Informational)
                << "No message, canceling thread";
          }
          else
          {
            Azure::Core::Diagnostics::_internal::Log::Stream(
                Azure::Core::Diagnostics::Logger::Level::Informational)
                << "Received message: " << *message;
            // if (nodeName == "$cbs" && IsCbsMessage(*message))
            //{
            //   ProcessCbsMessage(nodeName, *message);
            // }
            // else
            {
              MessageReceived(nodeName, linkComponents, *message);
            }
          }
        }

      private:
        AmqpServerMock* m_parentServer;
        std::thread m_connectionThread;

        std::shared_ptr<_internal::Connection> m_connection;
        bool m_connectionValid{false};
        std::shared_ptr<_internal::Session> m_session;
        // For each incoming message source, we create a queue of messages intended for that
        // message source.
        //
        // Each message queue is keyed by the message-id.
        std::map<std::string, MessageLinkComponents> m_linkMessageQueues;

        const char* SenderStateToString(_internal::MessageSenderState state)
        {
          // Return the stringized version of the values in the MessageSenderState enumeration
          switch (state)
          {
            case _internal::MessageSenderState::Invalid:
              return "Invalid";
            case _internal::MessageSenderState::Idle:
              return "Idle";
            case _internal::MessageSenderState::Opening:
              return "Opening";
            case _internal::MessageSenderState::Open:
              return "Open";
            case _internal::MessageSenderState::Closing:
              return "Closing";
            case _internal::MessageSenderState::Error:
              return "Error";
          }
          throw std::runtime_error("Unknown sender state");
        }

        const char* ConnectionStateToString(_internal::ConnectionState state)
        {
          switch (state)
          {
            case _internal::ConnectionState::Start:
              return "Start";
            case _internal::ConnectionState::HeaderReceived:
              return "HeaderReceived";
            case _internal::ConnectionState::HeaderSent:
              return "HeaderSent";
            case _internal::ConnectionState::HeaderExchanged:
              return "HeaderExchanged";
            case _internal::ConnectionState::OpenPipe:
              return "OpenPipe";
            case _internal::ConnectionState::OcPipe:
              return "OcPipe";
            case _internal::ConnectionState::OpenReceived:
              return "OpenReceived";
            case _internal::ConnectionState::OpenSent:
              return "OpenSent";
            case _internal::ConnectionState::ClosePipe:
              return "ClosePipe";
            case _internal::ConnectionState::Opened:
              return "Opened";
            case _internal::ConnectionState::CloseReceived:
              return "CloseReceived";
            case _internal::ConnectionState::CloseSent:
              return "CloseSent";
            case _internal::ConnectionState::Discarding:
              return "Discarding";
            case _internal::ConnectionState::End:
              return "End";
            case _internal::ConnectionState::Error:
              return "Error";
          }
          throw std::runtime_error("Unknown connection state");
        }

        const char* ReceiverStateToString(_internal::MessageReceiverState state)
        {
          switch (state)
          {
            case _internal::MessageReceiverState::Invalid:
              return "Invalid";
            case _internal::MessageReceiverState::Idle:
              return "Idle";
            case _internal::MessageReceiverState::Opening:
              return "Opening";
            case _internal::MessageReceiverState::Open:
              return "Open";
            case _internal::MessageReceiverState::Closing:
              return "Closing";
            case _internal::MessageReceiverState::Error:
              return "Error";
          }
          throw std::runtime_error("Unknown receiver state");
        }
      };

      virtual void OnSocketAccepted(
          std::shared_ptr<Network::_internal::Transport> transport) override
      {
        Azure::Core::Diagnostics::_internal::Log::Stream(
            Azure::Core::Diagnostics::Logger::Level::Informational)
            << "OnSocketAccepted - Socket connection received.";
        auto amqpTransport{
            Network::_internal::AmqpHeaderDetectTransportFactory::Create(transport, nullptr)};

        auto serverConnection{std::make_unique<ServerConnection>(amqpTransport, this)};
        serverConnection->StartProcessing();
        m_connections.push_back(std::move(serverConnection));
        m_connectionQueue.CompleteOperation(true);
      }

      void StartListening()
      {
        // Start the mock AMQP server which will be used to receive the connect open.
        // Ensure that the thread is started before we start using the message sender.
        std::mutex threadRunningMutex;
        std::condition_variable threadStarted;
        bool running = false;

        // Start a thread waiting on incoming connections.
        m_serverThread = std::thread([this, &threadStarted, &running]() {
          Network::_internal::SocketListener listener(GetPort(), this);
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Start test listener on port " << GetPort();
          listener.Start();
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "listener started";
          running = true;
          threadStarted.notify_one();

          while (!m_listenerContext.IsCancelled())
          {
            std::this_thread::yield();
            Azure::Core::Diagnostics::_internal::Log::Stream(
                Azure::Core::Diagnostics::Logger::Level::Informational)
                << "Wait for connection on listener.";

            if (!WaitForConnection(listener, m_listenerContext))
            {
              Azure::Core::Diagnostics::_internal::Log::Stream(
                  Azure::Core::Diagnostics::Logger::Level::Informational)
                  << "Wait for Connection failed, cancelling thread.";
              return;
            }
          }
          listener.Stop();
        });

        // Wait until our running thread is actually listening before we return.
        Azure::Core::Diagnostics::_internal::Log::Stream(
            Azure::Core::Diagnostics::Logger::Level::Informational)
            << "Wait 10 seconds for listener to start.";
        std::unique_lock<std::mutex> waitForThreadStart(threadRunningMutex);
        threadStarted.wait_until(
            waitForThreadStart,
            std::chrono::system_clock::now() + std::chrono::seconds(10),
            [&running]() { return running == true; });
        Azure::Core::Diagnostics::_internal::Log::Stream(
            Azure::Core::Diagnostics::Logger::Level::Informational)
            << "Listener running.";
      }

      void StopListening()
      {
        Azure::Core::Diagnostics::_internal::Log::Stream(
            Azure::Core::Diagnostics::Logger::Level::Informational)
            << "Stop listening";
        // Cancel the listener context, which will cause any WaitForXxx calls to exit.
        m_listenerContext.Cancel();

        // And stop all the connections.
        for (auto& connection : m_connections)
        {
          connection->StopProcessing();
        }
        m_serverThread.join();
      }

      void RegisterNamespace(
          std::string const& connectionNamespace,
          std::shared_ptr<NamespaceHandler> handler)
      {
        m_namespaceHandlers[connectionNamespace] = handler;
      }

      void ForceCbsError(bool forceError) { m_forceCbsError = forceError; }

    private:
      std::list<std::unique_ptr<ServerConnection>> m_connections;
      std::map<std::string, std::shared_ptr<NamespaceHandler>> m_namespaceHandlers;

      Common::_internal::AsyncOperationQueue<bool> m_connectionQueue;

      std::thread m_serverThread;
      std::uint16_t m_testPort;
      bool m_forceCbsError{false};

    protected:
      Azure::Core::Context m_listenerContext; // Used to cancel the listener if necessary.
    };
  } // namespace MessageTests
}}}} // namespace Azure::Core::Amqp::Tests
