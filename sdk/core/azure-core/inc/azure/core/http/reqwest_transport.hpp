// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license
// information.

// cspell: words reqwest

#pragma once

#include <azure/core/http/http.hpp>
#include <azure/core/http/transport.hpp>
#include <azure/core/internal/unique_handle.hpp>

#include <iostream>
#include <iterator>
#include <memory>

namespace Azure { namespace Core { namespace Http { namespace RustTransport {

  namespace _detail {
    class ReqwestHttpTransportImpl;
  }

  class ReqwestHttpTransport final : public Azure::Core::Http::HttpTransport {
  public:
    ReqwestHttpTransport();
    virtual ~ReqwestHttpTransport();

    // Inherited via HttpTransport
    std::unique_ptr<Azure::Core::Http::RawResponse> Send(
        Azure::Core::Http::Request& request,
        Azure::Core::Context const& context) override;

  private:
      std::unique_ptr<_detail::ReqwestHttpTransportImpl> m_pImpl;
  };

}}}} // namespace Azure::Core::Http::RustTransport
