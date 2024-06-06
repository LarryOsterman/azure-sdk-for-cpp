// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

#include "azure/core/http/reqwest_transport.hpp"

#include <utility>

namespace Azure { namespace Core { namespace Http { namespace RustTransport {

  std::string RustHttpResponse::ReasonPhrase() const
  {
    switch (StatusCode())
    {
      case HttpStatusCode::Ok:
        return "OK";
      case HttpStatusCode::NotFound:
        return "Not Found";
      default:
        return "Unknown";
    }
  }
  size_t RustTransport::RustBodyStream::OnRead(
      uint8_t* buffer,
      size_t count,
      Azure::Core::Context const&)
  {
    if (count == 0 || m_isEof)
    {
      return 0;
    }

    if (m_remainingBytes == 0)
    {
      m_currentBytes = _detail::AutoRustByteArray{_detail::RustInterop::rust_body_stream_read_bytes(
          m_rustStream.get(), m_runtimeContext.get())};
      // result_get_body_bytes returns nullptr if the stream is fully consumed.
      if (!m_currentBytes)
      {
        _detail::AutoRustError rustError{_detail::RustInterop::runtime_context_get_error(m_runtimeContext.get())};
        _detail::RustString error{_detail::RustInterop::rust_error_get_message(rustError.get())};
        throw std::runtime_error(error);
      }
      m_remainingBytes = _detail::RustInterop::byte_array_len(m_currentBytes.get());
      m_currentOffset = 0;
    }

    auto readSize = (std::min)(count, m_remainingBytes);
    std::memcpy(
        buffer,
        std::next(_detail::RustInterop::byte_array_as_ptr(m_currentBytes.get()), m_currentOffset),
        readSize);
    m_remainingBytes -= readSize;
    m_currentOffset += readSize;
    return readSize;
  }

  namespace _detail {
    namespace RustInterop {
      extern "C" uintptr_t stream_get_body_bytes(
          CppStream* stream,
          uint8_t* buffer,
          uintptr_t max_size)
      {
        return stream->Read(buffer, max_size /*, cppStream->Context()*/);
      }
    } // namespace RustInterop

    size_t RustInterop::CppStream::OnRead(
        uint8_t* buffer,
        size_t count,
        Azure::Core::Context const& context)
    {
      return m_stream->Read(buffer, count, context);
    }

    int64_t RustInterop::CppStream::Length() const { return m_stream->Length(); }
  } // namespace _detail

  struct Initializer
  {

    Initializer() { _detail::RustInterop::initialize(); }
  };

  static Initializer initializer{};

}}}} // namespace Azure::Core::Http::RustTransport
