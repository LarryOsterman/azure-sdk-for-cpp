// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

#include "reqwest_transport.hpp"

#include <iostream>

#include <gtest/gtest.h>

using namespace Azure::Core::Http::RustTransport;

TEST(RustHttpRequest, RawCreateRequest)
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

TEST(RustHttpRequest, CreateRequestSimple)
{
  auto request = Azure::Core::Http::RustTransport::RustHttpRequest();

  request.SetMethod(Azure::Core::Http::HttpMethod::Get);
  request.SetHeader("Test1", "Value1");
  request.SetHeader("Test2", "Value2");
  request.SetUrl(Azure::Core::Url("https://jsonplaceholder.typicode.com/todos/1"));
}

TEST(RustHttpRequest, EnumerateHeaders)
{
  auto request = Azure::Core::Http::RustTransport::RustHttpRequest();

  request.SetMethod(Azure::Core::Http::HttpMethod::Get);
  request.SetHeader("Test1", "Value1");
  request.SetHeader("Test2", "Value2");

  for (const auto& header : request.Headers())
  {
    std::cout << "Request Header: " << header.HeaderName() << " = " << header.HeaderValue()
              << std::endl;
  }

  for (auto it = request.Headers().begin(); it != request.Headers().end(); ++it)
  {
    std::cout << "Request Header: " << (*it).HeaderName() << " = " << (*it).HeaderValue()
              << std::endl;
  }

  std::vector<std::pair<std::string, std::string>> headers;
  for (const auto& header : request.Headers())
  {
    headers.push_back(std::make_pair(header.HeaderName(), header.HeaderValue()));
  }
  // Note that the Rust transport normalizes headers to their lower case value.
  ASSERT_EQ(headers.size(), 2);
  EXPECT_STREQ(headers[0].first.c_str(), "test1");
  EXPECT_STREQ(headers[0].second.c_str(), "Value1");
  EXPECT_STREQ(headers[1].first.c_str(), "test2");
  EXPECT_STREQ(headers[1].second.c_str(), "Value2");
}

TEST(RustHttpRequest, FromAzureRequest)
{
  Azure::Core::Http::Request request(
      Azure::Core::Http::HttpMethod::Get,
      Azure::Core::Url("https://jsonplaceholder.typicode.com/todos/1"),
      true);

  RustHttpRequest rustRequest{request, Azure::Core::Context{}};

  auto rustRequestPtr = rustRequest.GetRustRequest();
  EXPECT_STREQ(request.GetMethod().ToString().c_str(), request_get_method(rustRequestPtr));
  EXPECT_STREQ(request.GetUrl().GetAbsoluteUrl().c_str(), request_get_url(rustRequestPtr));
}
