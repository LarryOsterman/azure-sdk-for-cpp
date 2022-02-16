// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <stdexcept>

namespace Azure { namespace Security { namespace Attestation { namespace _detail {

  struct BCryptException : public std::runtime_error
  {
    BCryptException(std::string const& what);
  };
}}}} // namespace Azure::Security::Attestation::_detail
