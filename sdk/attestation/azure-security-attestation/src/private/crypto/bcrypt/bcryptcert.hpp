// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

/**
 * @brief Attestation client model support classes and functions.
 *
 * This file contains private classes used to support public model types.
 *
 */

#pragma once

#include "../inc/crypto.hpp"
#include "bcrypt_helpers.hpp"
#include <Windows.h>
#include <azure/core/base64.hpp>
#include <azure/core/internal/json/json.hpp>
#include <bcrypt.h>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// cspell::words BCrypt X509 BCryptX509

namespace Azure { namespace Security { namespace Attestation { namespace _detail {

  /** Represents an X509 Certificate.
   *
   */
  class BCryptX509Certificate final : public Cryptography::X509Certificate {
    friend class Cryptography;

  private:
    BCryptX509Certificate() = default;

  private:
    //      BCryptX509Certificate(_details::BCrypt_x509&& x509)
    //          : X509Certificate(), m_certificate(std::move(x509))
    //      {
    //      }

  protected:
    static std::unique_ptr<Cryptography::X509Certificate> CreateFromPrivateKey(
        std::unique_ptr<Cryptography::AsymmetricKey> const& key,
        std::string const& subjectName);

  public:
    virtual std::unique_ptr<Cryptography::AsymmetricKey> GetPublicKey() const override;
    virtual std::string ExportAsPEM() const override;
    std::string GetSubjectName() const override { throw std::runtime_error("Not implemented"); }

    std::string GetIssuerName() const override { throw std::runtime_error("Not implemented"); }

    static std::unique_ptr<X509Certificate> Import(std::string const& pemEncodedKey);
  };

}}}} // namespace Azure::Security::Attestation::_detail
