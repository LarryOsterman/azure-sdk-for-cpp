// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "amqp_server.hpp"

#include <azure/core/amqp/common/global_state.hpp>

#include <iostream>
#include <random>

#if defined(AZ_PLATFORM_POSIX)
#include <poll.h> // for poll()

#include <netinet/in.h> // for sockaddr_in
#include <sys/socket.h> // for socket shutdown
#elif defined(AZ_PLATFORM_WINDOWS)
#include <winsock2.h> // for WSAPoll();
#ifdef max
#undef max
#endif
#endif // AZ_PLATFORM_POSIX/AZ_PLATFORM_WINDOWS
namespace Azure { namespace Core { namespace Amqp { namespace Tests {
  uint16_t FindAvailableSocket()
  {
    // Ensure that the global state for the AMQP stack is initialized. Normally this is done by
    // the network facing objects, but this is called before those objects are initialized.
    //
    // This may hide bugs in some of the global objects, but it is needed to ensure that the
    // port we choose for the tests is available.
    {
      auto instance = Azure::Core::Amqp::Common::_detail::GlobalStateHolder::GlobalStateInstance();
      (void)instance;
    }

    std::random_device dev;
    int count = 0;
    while (count < 20)
    {
      uint16_t testPort;
      // Make absolutely sure that we don't accidentally use the TLS port.
      do
      {
        testPort = dev() % 1000 + 5000;
      } while (testPort == 5671);

      Azure::Core::Diagnostics::_internal::Log::Stream(
          Azure::Core::Diagnostics::Logger::Level::Informational)
          << "Trying Test port: " << testPort;

      auto sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (sock != -1)
      {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(testPort);

        auto bindResult = bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        // We were able to bind to the port, so it's available.
#if defined(AZ_PLATFORM_WINDOWS)
        closesocket(sock);
#else
        close(sock);
#endif
        if (bindResult != -1)
        {
          return testPort;
        }
        else
        {
#if defined(AZ_PLATFORM_WINDOWS)
          auto err = WSAGetLastError();
#else
          auto err = errno;
#endif
          Azure::Core::Diagnostics::_internal::Log::Stream(
              Azure::Core::Diagnostics::Logger::Level::Informational)
              << "Error " << std::to_string(err) << " binding to socket.";
        }
      }
      else
      {
#if defined(AZ_PLATFORM_WINDOWS)
        auto err = WSAGetLastError();
#else
        auto err = errno;
#endif
        Azure::Core::Diagnostics::_internal::Log::Stream(
            Azure::Core::Diagnostics::Logger::Level::Informational)
            << "Error " << std::to_string(err) << " opening port.";
      }
      count += 1;
    }
    throw std::runtime_error("Could not find available test port.");
  }
}}}} // namespace Azure::Core::Amqp::Tests

namespace Azure { namespace Core { namespace Amqp { namespace Tests { namespace MessageTests {
    class EventHubManagementHandler : public NamespaceHandler {
    public:
      EventHubManagementHandler() : NamespaceHandler() {}

      ~EventHubManagementHandler() override {}

      virtual Models::AmqpValue ValidateIncomingMessage(const Models::AmqpMessage& message) override
      {
        //        if (IsValid(message))
        {
          // Queue the message to be processed by the ProcessMessage method.
          QueueMessageForProcessing(message);
          return Models::_internal::Messaging::DeliveryAccepted();
        }
        //        return Models::_internal::Messaging::DeliveryRejected(
        //            Models::_internal::AmqpErrorCondition::InvalidField.ToString(),
        //            "The CBS message was not valid for this namespace.");
      }
      void ProcessMessage(Models::AmqpMessage const& message, Context const& context) override
      {
        message;
        context;
      }
    };
}}}}} // namespace Azure::Core::Amqp::Tests::MessageTests

int main()
{
Azure::Core::Amqp::Tests::MessageTests::AmqpServerMock server(5000);

server.RegisterNamespace(
    "$cbs", std::make_shared<Azure::Core::Amqp::Tests::MessageTests::CbsNamespaceHandler>());

server.RegisterNamespace(
    "$management",
    std::make_shared<Azure::Core::Amqp::Tests::MessageTests::EventHubManagementHandler>());

server.StartListening();

std::string input;
std::cin >> input;

server.StopListening();

return 0;
}