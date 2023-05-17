// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-Licence-Identifier: MIT

#include "azure/core/amqp/session.hpp"
#include "azure/core/amqp/connection.hpp"
#include "azure/core/amqp/link.hpp"
#include "private/connection_impl.hpp"
#include "private/session_impl.hpp"

#include <azure_uamqp_c/session.h>

void Azure::Core::_internal::UniqueHandleHelper<SESSION_INSTANCE_TAG>::FreeAmqpSession(
    SESSION_HANDLE value)
{
  session_destroy(value);
}

namespace Azure { namespace Core { namespace Amqp {
  namespace _internal {
    Endpoint::~Endpoint()
    {
      if (m_endpoint)
      {
        connection_destroy_endpoint(m_endpoint);
      }
    }

    Session::Session(
        Connection const& parentConnection,
        Endpoint& newEndpoint,
        SessionOptions const& options,
        SessionEvents* eventHandler)
        : m_impl{std::make_shared<Azure::Core::Amqp::_detail::SessionImpl>(
            _detail::ConnectionFactory::GetImpl(parentConnection),
            newEndpoint,
            options,
            eventHandler)}
    {
    }

    Session::Session(
        Connection const& parentConnection,
        SessionOptions const& options,
        SessionEvents* eventHandler)
        : m_impl{std::make_shared<Azure::Core::Amqp::_detail::SessionImpl>(
            _detail::ConnectionFactory::GetImpl(parentConnection),
            options,
            eventHandler)}
    {
    }

    Session::~Session() noexcept {}

    uint32_t Session::GetIncomingWindow() const { return m_impl->GetIncomingWindow(); }
    uint32_t Session::GetOutgoingWindow() const { return m_impl->GetOutgoingWindow(); }
    uint32_t Session::GetHandleMax() const { return m_impl->GetHandleMax(); }

    void Session::Begin() { m_impl->Begin(); }
    void Session::End(std::string const& condition_value, std::string const& description)
    {
      m_impl->End(condition_value, description);
    }
  } // namespace _internal

  namespace _detail {

    SessionImpl::~SessionImpl() noexcept {}

    SessionImpl::SessionImpl(
        std::shared_ptr<_detail::ConnectionImpl> connection,
        _internal::Endpoint& endpoint,
        _internal::SessionOptions const& options,
        _internal::SessionEvents* eventHandler)
        : m_connectionToPoll(connection), m_session{session_create_from_endpoint(
                                              *connection,
                                              endpoint.Release(),
                                              SessionImpl::OnLinkAttachedFn,
                                              this)},
          m_options{options}, m_eventHandler{eventHandler}
    {
      if (options.MaximumLinkCount.HasValue())
      {
        if (session_set_handle_max(m_session.get(), options.MaximumLinkCount.Value()))
        {
          throw std::runtime_error("Could not set handle max."); // LCOV_EXCL_LINE
        }
      }
      if (options.InitialIncomingWindowSize.HasValue())
      {
        if (session_set_incoming_window(m_session.get(), options.InitialIncomingWindowSize.Value()))
        {
          throw std::runtime_error("Could not set incoming window"); // LCOV_EXCL_LINE
        }
      }
      if (options.InitialOutgoingWindowSize.HasValue())
      {
        if (session_set_outgoing_window(m_session.get(), options.InitialOutgoingWindowSize.Value()))
        {
          throw std::runtime_error("Could not set outgoing window"); // LCOV_EXCL_LINE
        }
      }
    }

    SessionImpl::SessionImpl(
        std::shared_ptr<_detail::ConnectionImpl> connection,
        _internal::SessionOptions const& options,
        _internal::SessionEvents* eventHandler)
        : m_connectionToPoll(connection),
          m_session{session_create(*connection, SessionImpl::OnLinkAttachedFn, this)},
          m_options{options}, m_eventHandler{eventHandler}

    {
      if (options.MaximumLinkCount.HasValue())
      {
        if (session_set_handle_max(m_session.get(), options.MaximumLinkCount.Value()))
        {
          throw std::runtime_error("Could not set handle max."); // LCOV_EXCL_LINE
        }
      }
      if (options.InitialIncomingWindowSize.HasValue())
      {
        if (session_set_incoming_window(m_session.get(), options.InitialIncomingWindowSize.Value()))
        {
          throw std::runtime_error("Could not set incoming window"); // LCOV_EXCL_LINE
        }
      }
      if (options.InitialOutgoingWindowSize.HasValue())
      {
        if (session_set_outgoing_window(m_session.get(), options.InitialOutgoingWindowSize.Value()))
        {
          throw std::runtime_error("Could not set outgoing window"); // LCOV_EXCL_LINE
        }
      }
    }

    uint32_t SessionImpl::GetIncomingWindow()
    {
      uint32_t window;
      if (session_get_incoming_window(m_session.get(), &window))
      {
        throw std::runtime_error("Could not get incoming window"); // LCOV_EXCL_LINE
      }
      return window;
    }

    uint32_t SessionImpl::GetOutgoingWindow()
    {
      uint32_t window;
      if (session_get_outgoing_window(m_session.get(), &window))
      {
        throw std::runtime_error("Could not get outgoing window"); // LCOV_EXCL_LINE
      }
      return window;
    }

    uint32_t SessionImpl::GetHandleMax()
    {
      uint32_t max;
      if (session_get_handle_max(m_session.get(), &max))
      {
        throw std::runtime_error("Could not get handle max."); // LCOV_EXCL_LINE
      }
      return max;
    }

    void SessionImpl::Begin()
    {
      if (session_begin(m_session.get()))
      {
        throw std::runtime_error("Could not begin session"); // LCOV_EXCL_LINE
      }
    }
    void SessionImpl::End(const std::string& condition, const std::string& description)
    {
      // When we end the session, it clears all the links, so we need to ensure that the
      //    m_newLinkAttachedQueue.Clear();
      if (session_end(
              m_session.get(),
              condition.empty() ? nullptr : condition.c_str(),
              description.empty() ? nullptr : description.c_str()))
      {
        throw std::runtime_error("Could not begin session"); // LCOV_EXCL_LINE
      }
    }

    bool SessionImpl::OnLinkAttachedFn(
        void* context,
        LINK_ENDPOINT_INSTANCE_TAG* newLinkEndpoint,
        const char* name,
        bool role,
        AMQP_VALUE_DATA_TAG* source,
        AMQP_VALUE_DATA_TAG* target,
        AMQP_VALUE_DATA_TAG* properties)
    {
      SessionImpl* session = static_cast<SessionImpl*>(context);
      _internal::LinkEndpoint linkEndpoint(newLinkEndpoint);
      if (session->m_eventHandler)
      {
        return session->m_eventHandler->OnLinkAttached(
            Azure::Core::Amqp::_detail::SessionFactory::CreateFromInternal(
                session->shared_from_this()),
            linkEndpoint,
            name,
            role == role_receiver ? Azure::Core::Amqp::_internal::SessionRole::Receiver
                                  : Azure::Core::Amqp::_internal::SessionRole::Sender,
            source,
            target,
            properties);
      }
      else
      {
        // Even if we don't have any actions to take, if we return false, the connection will be
        // aborted.
        return true;
      }
      static_cast<void>(role);
    }
  } // namespace _detail
}}} // namespace Azure::Core::Amqp