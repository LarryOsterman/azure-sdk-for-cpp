// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#if ENABLE_RUST_AMQP
#include "../src/amqp/private/unique_handle.hpp"
#include "rust_amqp_wrapper.h"

#include <azure/core/context.hpp>

#include <memory>

namespace Azure { namespace Core { namespace Amqp { namespace _detail {

  using RustRuntimeContext = RustInterop::RuntimeContext;
  using RustCallContext = RustInterop::RustCallContext;
//  using RustAmqpError = RustInterop::RustError;

  template <> struct UniqueHandleHelper<RustRuntimeContext>
  {
    static void FreeRuntimeContext(RustRuntimeContext* obj);

    using type = Core::_internal::BasicUniqueHandle<RustRuntimeContext, FreeRuntimeContext>;
  };
  template <> struct UniqueHandleHelper<RustCallContext>
  {
    static void FreeCallContext(RustCallContext* obj);

    using type = Core::_internal::BasicUniqueHandle<RustCallContext, FreeCallContext>;
  };
//  template <> struct UniqueHandleHelper<RustAmqpError>
//  {
//    static void FreeRustError(RustAmqpError* obj);//

//    using type = Core::_internal::BasicUniqueHandle<RustAmqpError, FreeRustError>;
//  };

}}}} // namespace Azure::Core::Amqp::_detail

namespace Azure { namespace Core { namespace Amqp { namespace Common { namespace _detail {

  using UniqueRustRuntimeContext = Azure::Core::Amqp::_detail::UniqueHandleHelper<
      Azure::Core::Amqp::_detail::RustRuntimeContext>::type;

  using UniqueRustCallContext = Azure::Core::Amqp::_detail::UniqueHandleHelper<
      Azure::Core::Amqp::_detail::RustCallContext>::type;
//  using UniqueRustError = Azure::Core::Amqp::_detail::UniqueHandleHelper<
//      Azure::Core::Amqp::_detail::RustAmqpError>::type;

  /**
   * @brief Represents the an implementation of the rust multithreaded runtime.
   *
   * Needed to implement blocking Rust API calls.
   */
  class RustRuntimeContext final {

    UniqueRustRuntimeContext m_runtimeContext;

  public:
    RustRuntimeContext()
        : m_runtimeContext{Azure::Core::Amqp::_detail::RustInterop::runtime_context_new()}
    {
    }

    Azure::Core::Amqp::_detail::RustRuntimeContext* GetRuntimeContext() const
    {
      return m_runtimeContext.get();
    }
  };

  class CallContext final {
  public:
    /** Construct a new CallContext object.
     *
     * @param runtimeContext - pointer to the Rust runtime for this process.
     * @param context - Azure Context for this operation.
     *
     * @note This class does *NOT* take ownership of the runtime - that is because the lifetime of
     * all CallContext objects MUST be shorter than the lifetime of the GlobalState object which
     * actually holds the RustRuntimeContext.
     *
     */

    CallContext(
        Azure::Core::Amqp::_detail::RustInterop::RuntimeContext* runtimeContext,
        Azure::Core::Context const& context)
        : m_callContext{Azure::Core::Amqp::_detail::RustInterop::call_context_new(runtimeContext)},
          m_context(context)
    {
    }

    Azure::Core::Amqp::_detail::RustCallContext* GetCallContext() const
    {
      return m_callContext.get();
    }

    std::string GetError() const
    {
//      UniqueRustError error{call_context_get_error(GetCallContext())};
      auto err = call_context_get_error(GetCallContext());
      if (err)
      {
//        auto err{rust_error_get_message(error.get())};
        std::string errorString{err};
        Azure::Core::Amqp::_detail::RustInterop::rust_string_delete(err);
        return errorString;
      }
      else
      {
        return "No current Error.";
      }
    }

  private:
    UniqueRustCallContext m_callContext;
    Azure::Core::Context m_context;
  };

}}}}} // namespace Azure::Core::Amqp::Common::_detail
#endif // ENABLE_RUST_AMQP
