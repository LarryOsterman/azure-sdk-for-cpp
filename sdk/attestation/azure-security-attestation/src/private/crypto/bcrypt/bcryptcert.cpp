// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT
/**
 * @brief Attestation client model support classes and functions.
 *
 * This file contains private classes used to support public model types.
 *
 */
#include "bcryptcert.hpp"
#include "../inc/crypto.hpp"
#include "bcryptkeys.hpp"
#include <azure/core/base64.hpp>
#include <azure/core/internal/json/json.hpp>
#include <ctime>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// cspell::words BCrypt X509 BCryptX509
namespace Azure { namespace Security { namespace Attestation { namespace _detail {
  std::unique_ptr<Cryptography::X509Certificate> BCryptX509Certificate::Import(std::string const&)
  {
    throw std::runtime_error("Not implemented");
  }
  std::string BCryptX509Certificate::ExportAsPEM() const
  {
    throw std::runtime_error("Not implemented");
  }
  std::unique_ptr<Cryptography::X509Certificate> BCryptX509Certificate::CreateFromPrivateKey(
      std::unique_ptr<Cryptography::AsymmetricKey> const&,
      std::string const&)

  {
    throw std::runtime_error("Not implemented");
  }
  std::unique_ptr<Cryptography::AsymmetricKey> BCryptX509Certificate::GetPublicKey() const
  {
    throw std::runtime_error("Not implemented");
  }

}}}} // namespace Azure::Security::Attestation::_detail
