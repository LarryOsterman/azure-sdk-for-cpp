// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

#include "azure/core/http/reqwest_transport.hpp"

#include <iostream>

#include <gtest/gtest.h>

using namespace Azure::Core::Http::RustTransport;

TEST(RustTransport, GetSimple)
{
  Azure::Core::Http::Request request(
      Azure::Core::Http::HttpMethod::Get,
      Azure::Core::Url("https://jsonplaceholder.typicode.com/todos/1"));
  request.SetHeader("Content-Type", "application/json");

  Azure::Core::Http::RustTransport::RustHttpTransport transport;

  auto response = transport.Send(request, Azure::Core::Context());

  EXPECT_EQ(response->GetStatusCode(), Azure::Core::Http::HttpStatusCode::Ok);
  EXPECT_EQ(response->GetReasonPhrase(), "OK");
  EXPECT_EQ(response->GetHeaders().at("content-type"), "application/json; charset=utf-8");
  GTEST_LOG_(INFO) << "Second send.";
  response = transport.Send(request, Azure::Core::Context());
  EXPECT_EQ(response->GetStatusCode(), Azure::Core::Http::HttpStatusCode::Ok);
  EXPECT_EQ(response->GetReasonPhrase(), "OK");
  EXPECT_EQ(response->GetHeaders().at("content-type"), "application/json; charset=utf-8");
}

TEST(RustTransport, MicrosoftRead)
{
  Azure::Core::Http::Request request(
      Azure::Core::Http::HttpMethod::Get,
      Azure::Core::Url("https://www.microsoft.com/index.htm"), false);

    Azure::Core::Http::RustTransport::RustHttpTransport transport;

  auto response = transport.Send(request, Azure::Core::Context());

  EXPECT_EQ(response->GetStatusCode(), Azure::Core::Http::HttpStatusCode::Ok);
  auto stream = response->ExtractBodyStream();
  EXPECT_EQ(response->GetReasonPhrase(), "OK");
  EXPECT_EQ(response->GetHeaders().at("content-type"), "text/html");
  std::vector<uint8_t> buffer(1024);
  do
  {
    size_t read = stream->Read(&buffer[0], buffer.size(), Azure::Core::Context());
    if (read == 0)
    {
      break;
    }
  } while (true);

  GTEST_LOG_(INFO) << "Second send.";
  response = transport.Send(request, Azure::Core::Context());
  GTEST_LOG_(INFO) << "Send done.";
}