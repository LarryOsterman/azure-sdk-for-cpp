// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

// cspell: words reqwest

#pragma once

#include "rust_http.h"

#include <azure/core/http/http.hpp>
#include <azure/core/http/transport.hpp>
#include <azure/core/internal/unique_handle.hpp>

#include <iostream>
#include <iterator>
#include <memory>

// RAII wrappers for rust interoperability types.
namespace Azure { namespace Core { namespace _detail {
  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::RuntimeContext>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RuntimeContext,
        Azure::Core::Http::RustTransport::_detail::RustInterop::runtime_context_delete>;
  };
  template <>
  struct UniqueHandleHelper<
      Azure::Core::Http::RustTransport::_detail::RustInterop::RustHeaderIterator>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RustHeaderIterator,
        Azure::Core::Http::RustTransport::_detail::RustInterop::header_iterator_delete>;
  };
  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::RustRequest>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RustRequest,
        Azure::Core::Http::RustTransport::_detail::RustInterop::request_delete>;
  };

  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::ReqwestWrapper>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::ReqwestWrapper,
        Azure::Core::Http::RustTransport::_detail::RustInterop::reqwest_wrapper_delete>;
  };

  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::RustHttpResult>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RustHttpResult,
        Azure::Core::Http::RustTransport::_detail::RustInterop::result_delete>;
  };

  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::RustHttpHeader>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RustHttpHeader,
        Azure::Core::Http::RustTransport::_detail::RustInterop::http_header_delete>;
  };

  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::RustByteArray>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RustByteArray,
        Azure::Core::Http::RustTransport::_detail::RustInterop::byte_array_delete>;
  };

  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::RustError>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RustError,
        Azure::Core::Http::RustTransport::_detail::RustInterop::rust_error_delete>;
  };

  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::RustBodyStream>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RustBodyStream,
        Azure::Core::Http::RustTransport::_detail::RustInterop::rust_body_stream_delete>;
  };

  template <>
  struct UniqueHandleHelper<Azure::Core::Http::RustTransport::_detail::RustInterop::RustRawResponse>
  {
    using type = Azure::Core::_internal::BasicUniqueHandle<
        Azure::Core::Http::RustTransport::_detail::RustInterop::RustRawResponse,
        Azure::Core::Http::RustTransport::_detail::RustInterop::raw_response_delete>;
  };

}}} // namespace Azure::Core::_detail

namespace Azure { namespace Core { namespace Http { namespace RustTransport { namespace _detail {
  using AutoRuntimeContext = Azure::Core::_internal::UniqueHandle<RustInterop::RuntimeContext>;
  using AutoRustHeaderIterator
      = Azure::Core::_internal::UniqueHandle<RustInterop::RustHeaderIterator>;
  using AutoRustHttpHeader = Azure::Core::_internal::UniqueHandle<RustInterop::RustHttpHeader>;
  using AutoRequest = Azure::Core::_internal::UniqueHandle<RustInterop::RustRequest>;
  using AutoRustHttpResult = Azure::Core::_internal::UniqueHandle<RustInterop::RustHttpResult>;
  using AutoReqwestWrapper = Azure::Core::_internal::UniqueHandle<RustInterop::ReqwestWrapper>;
  using AutoRustByteArray = Azure::Core::_internal::UniqueHandle<RustInterop::RustByteArray>;
  using AutoRustBodyStream = Azure::Core::_internal::UniqueHandle<RustInterop::RustBodyStream>;
  using AutoRustError = Azure::Core::_internal::UniqueHandle<RustInterop::RustError>;
  using AutoRustRawResponse = Azure::Core::_internal::UniqueHandle<RustInterop::RustRawResponse>;

  class RustString {
  public:
    RustString(int8_t* ptr) : ptr(ptr) {}
    ~RustString() { RustInterop::rust_string_delete(ptr); }
    RustString(const RustString&) = default;
    RustString& operator=(const RustString&) = default;
    RustString(RustString&&) = default;
    RustString& operator=(RustString&&) = default;
    operator const char*() const { return reinterpret_cast<const char*>(ptr); }

    friend std::ostream& operator<<(std::ostream& stream, const RustString& instance)
    {
      return stream << instance.ptr;
    }

  private:
    int8_t* ptr;
  };

  namespace RustInterop {
    // The CppStream is a declared but undefined C++ type that is used to pass the C++ stream
    // to rust. Implement this type in the C++ code to pass the stream to rust, enabling
    // cancellation support.
    struct CppStream : public Azure::Core::IO::BodyStream
    {
    public:
      CppStream(Azure::Core::IO::BodyStream* stream, Azure::Core::Context context)
          : m_stream(stream), m_context{context}
      {
      }
      Azure::Core::Context const& Context() const { return m_context; }

    private:
      // Inherited via BodyStream
      size_t OnRead(uint8_t* buffer, size_t count, Azure::Core::Context const& context) override;
      int64_t Length() const override;

      Azure::Core::IO::BodyStream* m_stream;
      Azure::Core::Context m_context;
    };
  } // namespace RustInterop

  class RustHttpHeader {
  public:
    RustHttpHeader() : m_header{} {}
    RustHttpHeader(RustHttpHeader&& other) = default;
    RustHttpHeader& operator=(RustHttpHeader&& other) = default;
    RustHttpHeader(const RustHttpHeader& other) = delete; // Added copy constructor
    RustHttpHeader& operator=(const RustHttpHeader& other)
        = delete; // Added copy assignment operator
    ~RustHttpHeader() = default; // Added destructor

    RustHttpHeader(_detail::AutoRustHttpHeader&& rustHeader) : m_header{std::move(rustHeader)} {}
    std::string HeaderName() const
    {
      return std::string{
          _detail::RustString{_detail::RustInterop::http_header_get_name(m_header.get())}};
    }

    std::string HeaderValue() const
    {
      return std::string{
          _detail::RustString{_detail::RustInterop::http_header_get_value(m_header.get())}};
    }

    bool operator==(const RustHttpHeader& that) const
    {
      // If they're the same header, we're done.
      if (that.m_header == this->m_header)
      {
        return true;
      }

      // If one is empty and the other is not, they're not equal.
      if ((that.m_header && !this->m_header) || (!that.m_header && this->m_header))
      {
        return false;
      }

      // Check the name and value for equality.
      if (HeaderName() != that.HeaderName() || HeaderValue() != that.HeaderValue())
      {
        return false;
      }
      else
      {
        return true;
      }
    }

  private:
    _detail::AutoRustHttpHeader m_header;
  };

  class RustHeaderIterator {
  public:
    explicit RustHeaderIterator(_detail::AutoRustHeaderIterator&& headerIterator)
        : m_iterator{std::move(headerIterator)}
    {
    }

    ~RustHeaderIterator() = default;
    RustHeaderIterator(RustHeaderIterator&& that) = default;
    RustHeaderIterator& operator=(RustHeaderIterator&& that) = default;
    RustHeaderIterator& operator=(const RustHeaderIterator& that) = delete;
    RustHeaderIterator(const RustHeaderIterator& that) = delete;

    class iterator {
    public:
      iterator(_detail::AutoRustHeaderIterator&& it) : m_it{std::move(it)}
      {
        if (m_it)
        {
          m_current = RustHttpHeader{
              _detail::AutoRustHttpHeader{_detail::RustInterop::header_iterator_next(m_it.get())}};
        }
      }
      ~iterator() = default;
      iterator(iterator&& that) = default;
      iterator& operator=(iterator&& that) = default;
      iterator(const iterator& that) = delete;
      iterator& operator=(const iterator& that) = delete;

      using iterator_category = std::input_iterator_tag;
      using value_type = RustHttpHeader;
      using pointer = const RustHttpHeader*;
      using reference = const RustHttpHeader&;
      using difference_type = std::ptrdiff_t;

      reference operator*() const { return m_current; }
      iterator& operator++()
      {
        // Advance the iterator.
        if (m_it)
        {
          m_current = RustHttpHeader{
              _detail::AutoRustHttpHeader{_detail::RustInterop::header_iterator_next(m_it.get())}};
        }
        return *this;
      }
      bool operator==(const iterator& that) const { return that.m_current == m_current; }
      bool operator!=(const iterator& that) const { return !(this->m_current == that.m_current); }

    private:
      RustHttpHeader m_current;
      _detail::AutoRustHeaderIterator m_it;
    };
    iterator begin() { return iterator{std::move(m_iterator)}; }
    iterator end() { return iterator{nullptr}; }

  private:
    _detail::AutoRustHeaderIterator m_iterator;
  };

  class RustHttpRequest {
  public:
    RustHttpRequest(Azure::Core::Http::Request& request, const Azure::Core::Context& requestContext)
        : m_rustRequest(_detail::RustInterop::request_new())
    {
      _detail::RustInterop::request_set_method(
          m_rustRequest.get(), request.GetMethod().ToString().c_str());
      if (!_detail::RustInterop::request_set_url(
              m_rustRequest.get(), request.GetUrl().GetAbsoluteUrl().c_str()))
      {
        throw Azure::Core::Http::TransportException(
            "Could not set URL in RUST request, possibly due to invalid UTF-8 encoding.");
      }

      for (auto const& header : request.GetHeaders())
      {
        _detail::RustInterop::request_set_header(
            m_rustRequest.get(), header.first.c_str(), header.second.c_str());
      }
      auto stream = request.GetBodyStream();

      if (stream)
      {
        m_stream = std::make_unique<_detail::RustInterop::CppStream>(stream, requestContext);
        _detail::RustInterop::request_set_body(m_rustRequest.get(), m_stream.get());
      }
    }

    RustHttpRequest() : m_rustRequest(_detail::RustInterop::request_new()) {}

    void SetMethod(Azure::Core::Http::HttpMethod const& method)
    {
      _detail::RustInterop::request_set_method(m_rustRequest.get(), method.ToString().c_str());
    }

    void SetHeader(std::string const& name, std::string const& value)
    {
      _detail::RustInterop::request_set_header(m_rustRequest.get(), name.c_str(), value.c_str());
    }

    void SetUrl(Azure::Core::Url const& url)
    {
      _detail::RustInterop::request_set_url(m_rustRequest.get(), url.GetAbsoluteUrl().c_str());
    }

    _detail::RustInterop::RustRequest* GetRustRequest() const { return m_rustRequest.get(); }

    RustHeaderIterator Headers()
    {
      return RustHeaderIterator{_detail::AutoRustHeaderIterator{
          _detail::RustInterop::request_get_headers(m_rustRequest.get())}};
    }

  private:
    _detail::AutoRequest m_rustRequest;
    std::unique_ptr<_detail::RustInterop::CppStream> m_stream;
  };

  class RustBodyStream : public Azure::Core::IO::BodyStream {

  public:
    RustBodyStream(
        _detail::AutoRuntimeContext&& runtimeContext,
        _detail::AutoRustBodyStream&& bodyStream,
        size_t contentLength)
        : m_runtimeContext{std::move(runtimeContext)}, m_rustStream{std::move(bodyStream)},
          m_contentLength{contentLength}
    {
    }

  private:
    // Inherited via BodyStream
    std::size_t OnRead(uint8_t* buffer, size_t count, Azure::Core::Context const& context) override;
    std::int64_t Length() const override { return static_cast<std::int64_t>(m_contentLength); }

    bool m_isEof{false};
    //    RustHttpResult m_result;
    _detail::AutoRuntimeContext m_runtimeContext;
    _detail::AutoRustBodyStream m_rustStream;

    _detail::AutoRustByteArray m_currentBytes;
    size_t m_remainingBytes{};
    size_t m_currentOffset{};

    size_t m_contentLength{0};
  };

  class RustHttpResponse {
  public:
    /**
     * Constructs a new RustHttpResult from an interoperability HttpResult.
     */
    RustHttpResponse(_detail::AutoRustRawResponse&& response)
        : m_response{std::move(response)},
          m_contentLength{_detail::RustInterop::raw_response_get_content_length(m_response.get())}
    {
    }

    RustHttpResponse(RustHttpResponse&& other) = default;

    Azure::Core::Http::HttpStatusCode StatusCode() const
    {
      auto statusCode = _detail::RustInterop::raw_response_get_status(m_response.get());
      return static_cast<Azure::Core::Http::HttpStatusCode>(statusCode);
    }

    std::string ReasonPhrase() const;

    RustHeaderIterator Headers() const
    {
      return RustHeaderIterator{_detail::AutoRustHeaderIterator{
          _detail::RustInterop::raw_response_get_header_iterator(m_response.get())}};
    }

    std::pair<std::uint32_t, std::uint32_t> Version() const
    {
      _detail::RustInterop::HttpVersion version
          = _detail::RustInterop::raw_response_get_version(m_response.get());
      return std::make_pair(version.major, version.minor);
    }

    std::uint64_t ContentLength() const { return m_contentLength; }

    std::unique_ptr<RustBodyStream> CreateBodyStream(_detail::AutoRuntimeContext&& context)
    {
      auto contentLength = _detail::RustInterop::raw_response_get_content_length(m_response.get());
      return std::make_unique<RustBodyStream>(
          std::move(context),
          _detail::AutoRustBodyStream{
              _detail::RustInterop::raw_response_extract_body_stream(m_response.release())},
          contentLength);
    }

  private:
    _detail::AutoRustRawResponse m_response;
    uint64_t m_contentLength{0};
  };

  class ReqwestHttpTransportImpl : public Azure::Core::Http::HttpTransport {
  public:
    ReqwestHttpTransportImpl();
    virtual ~ReqwestHttpTransportImpl() {}

    // Inherited via HttpTransport
    std::unique_ptr<Azure::Core::Http::RawResponse> Send(
        Azure::Core::Http::Request& request,
        Azure::Core::Context const& context) override;

  private:
    AutoReqwestWrapper m_rustReqwestWrapper;
  };
}}}}} // namespace Azure::Core::Http::RustTransport::_detail