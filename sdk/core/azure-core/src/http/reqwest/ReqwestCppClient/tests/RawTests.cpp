// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

#include "azure/core/http/reqwest_transport.hpp"
#include "../ReqwestCppClient/src/private/reqwest_http_transport_private.hpp"
#include <iostream>

#include <gtest/gtest.h>

using namespace Azure::Core::Http::RustTransport;

TEST(RustHttp, RawCreateHeader)
{
  auto request = _detail::AutoRequest(_detail::RustInterop::request_new());

  _detail::RustInterop::request_set_header(request.get(), "Content-Type", "application/json");
  _detail::RustInterop::request_set_method(
      request.get(), Azure::Core::Http::HttpMethod::Get.ToString().c_str());

  EXPECT_STREQ(
      Azure::Core::Http::HttpMethod::Get.ToString().c_str(),
      _detail::RustInterop::request_get_method(request.get()));

  _detail::RustInterop::request_set_url(
      request.get(), "https://jsonplaceholder.typicode.com/todos/1");
  EXPECT_STREQ(
      "https://jsonplaceholder.typicode.com/todos/1",
      _detail::RustInterop::request_get_url(request.get()));

  {
    auto header_iterator
        = _detail::AutoRustHeaderIterator(_detail::RustInterop::request_get_headers(request.get()));
    while (auto header = _detail::AutoRustHttpHeader(
               _detail::RustInterop::header_iterator_next(header_iterator.get())))
    {
      auto name = _detail::RustString(_detail::RustInterop::http_header_get_name(header.get()));
      auto value = _detail::RustString(_detail::RustInterop::http_header_get_value(header.get()));
      std::cout << "Request Header: " << name << " = " << value << std::endl;
    }
  }
}

TEST(RustRawTests, RustRawHttpGetSimple)
{
  auto runtime = _detail::AutoRuntimeContext(_detail::RustInterop::runtime_context_new());

  auto request = _detail::AutoRequest(_detail::RustInterop::request_new());

  _detail::RustInterop::request_set_header(request.get(), "Content-Type", "application/json");
  _detail::RustInterop::request_set_method(request.get(), "GET");
  _detail::RustInterop::request_set_url(
      request.get(), "https://jsonplaceholder.typicode.com/todos/1");

  //	request_send(request);
  auto client = _detail::AutoReqwestWrapper(_detail::RustInterop::reqwest_wrapper_new());

  auto result = _detail::AutoRustHttpResult{
      _detail::RustInterop::reqwest_wrapper_send(runtime.get(), client.get(), request.get())};
  if (_detail::RustInterop::result_has_error(result.get()))
  {
    std::cout << "Error.";
  }
  ASSERT_FALSE(_detail::RustInterop::result_has_error(result.get()));

  auto rawResponse
      = _detail::AutoRustRawResponse{_detail::RustInterop::result_extract_response(result.release())};

  std::cout << "Http Request completed, result = "
            << _detail::RustInterop::raw_response_get_status(rawResponse.get()) << std::endl;

  auto version = _detail::RustInterop::raw_response_get_version(rawResponse.get());
  std::cout << "Version major: " << version.major << " minor: " << version.minor << std::endl;
}

TEST(RustRawTestsHttp, RustHttpGetMicrosoftComReadHeaders)
{
  auto runtime = _detail::AutoRuntimeContext(_detail::RustInterop::runtime_context_new());

  auto request = _detail::AutoRequest(_detail::RustInterop::request_new());

  _detail::RustInterop::request_set_header(request.get(), "Content-Type", "application/json");
  _detail::RustInterop::request_set_method(request.get(), "GET");

  //	request_send(request);
  auto client = _detail::AutoReqwestWrapper(_detail::RustInterop::reqwest_wrapper_new());

  _detail::RustInterop::request_set_url(request.get(), "https://www.microsoft.com/index.htm");
  auto result = _detail::AutoRustHttpResult{
      _detail::RustInterop::reqwest_wrapper_send(runtime.get(), client.get(), request.get())};

  ASSERT_FALSE(_detail::RustInterop::result_has_error(result.get()));

  auto rawResponse
      = _detail::AutoRustRawResponse{_detail::RustInterop::result_extract_response(result.release())};

  std::cout << "Http Request 2 completed, result = "
            << _detail::RustInterop::raw_response_get_status(rawResponse.get()) << std::endl;

  auto version = _detail::RustInterop::raw_response_get_version(rawResponse.get());
  std::cout << "Version major: " << version.major << " minor: " << version.minor << std::endl;

  auto iterator = _detail::AutoRustHeaderIterator{
      _detail::RustInterop::raw_response_get_header_iterator(rawResponse.get())};

  while (true)
  {
    auto header
        = _detail::AutoRustHttpHeader{_detail::RustInterop::header_iterator_next(iterator.get())};
    if (!header)
    {
      break;
    }
    auto hName = _detail::RustString(_detail::RustInterop::http_header_get_name(header.get()));
    auto hValue = _detail::RustString(_detail::RustInterop::http_header_get_value(header.get()));
    std::cout << "Header: " << hName << " = " << hValue << std::endl;
  }
}

TEST(RustHttp, RustHttpGetMicrosoftComGetBody1)
{
  auto runtime = _detail::AutoRuntimeContext(_detail::RustInterop::runtime_context_new());

  auto request = _detail::AutoRequest(_detail::RustInterop::request_new());

  _detail::RustInterop::request_set_header(request.get(), "Content-Type", "application/json");
  _detail::RustInterop::request_set_method(request.get(), "GET");

  //	request_send(request);
  auto client = _detail::AutoReqwestWrapper(_detail::RustInterop::reqwest_wrapper_new());

  _detail::RustInterop::request_set_url(request.get(), "https://www.microsoft.com/index.htm");
  auto result = _detail::AutoRustHttpResult{
      _detail::RustInterop::reqwest_wrapper_send(runtime.get(), client.get(), request.get())};

  ASSERT_FALSE(_detail::RustInterop::result_has_error(result.get()));

  auto response
      = _detail::AutoRustRawResponse{_detail::RustInterop::result_extract_response(result.release())};

  std::cout << "Http Request 2 completed, result = "
            << _detail::RustInterop::raw_response_get_status(response.get()) << std::endl;

  std::cout << "Body length: "
            << _detail::RustInterop::raw_response_get_content_length(response.get()) << std::endl;
  std::vector<std::uint8_t> body;
  _detail::AutoRustByteArray bodyBytes;
  do
  {
    bodyBytes = _detail::AutoRustByteArray{
        _detail::RustInterop::raw_response_get_body_bytes(runtime.get(), response.get())};
    if (bodyBytes)
    {
      std::cout << "Body size: " << body.size()
                << ", new chunk length: " << _detail::RustInterop::byte_array_len(bodyBytes.get())
                << std::endl;
      body.reserve(body.size() + _detail::RustInterop::byte_array_len(bodyBytes.get()));
      body.insert(
          body.end(),
          _detail::RustInterop::byte_array_as_ptr(bodyBytes.get()),
          _detail::RustInterop::byte_array_as_ptr(bodyBytes.get())
              + _detail::RustInterop::byte_array_len(bodyBytes.get()));
    }
  } while (bodyBytes);
  std::cout << "At end, body size: " << body.size() << " capacity: " << body.capacity()
            << std::endl;
}
