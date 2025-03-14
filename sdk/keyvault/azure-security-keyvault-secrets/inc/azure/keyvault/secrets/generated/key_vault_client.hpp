// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See License.txt in the project root for license information.
// Code generated by Microsoft (R) TypeSpec Code Generator.
// Changes may cause incorrect behavior and will be lost if the code is regenerated.

#pragma once

#include "azure/keyvault/secrets/generated/key_vault_client_options.hpp"
#include "azure/keyvault/secrets/generated/key_vault_client_paged_responses.hpp"
#include "azure/keyvault/secrets/generated/models/generated_models.hpp"

#include <azure/core/context.hpp>
#include <azure/core/credentials/credentials.hpp>
#include <azure/core/datetime.hpp>
#include <azure/core/internal/extendable_enumeration.hpp>
#include <azure/core/internal/http/pipeline.hpp>
#include <azure/core/nullable.hpp>
#include <azure/core/paged_response.hpp>
#include <azure/core/response.hpp>
#include <azure/core/url.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Azure { namespace Security { namespace KeyVault { namespace Secrets { namespace _detail {
  /**
   * @brief The key vault client performs cryptographic key operations and vault operations against
   * the Key Vault service.
   *
   */
  class KeyVaultClient final {
  public:
    /**
     * @brief Constructs the KeyVaultClient.
     * @param url The URL address where the client will send the requests to.
     * @param credential Credential to authenticate with the service.
     * @param options Optional parameters.
     *
     */
    explicit KeyVaultClient(
        const std::string& url,
        const std::shared_ptr<const Core::Credentials::TokenCredential>& credential,
        const KeyVaultClientOptions& options = {});

    /**
     * @brief Gets the KeyVaultClient URL endpoint.
     * @return The KeyVaultClient's URL endpoint.
     *
     */
    std::string GetUrl() const;

    /**
     * @brief The SET operation adds a secret to the Azure Key Vault. If the named secret already
     * exists, Azure Key Vault creates a new version of that secret. This operation requires the
     * secrets/set permission.
     * @param secretName The name of the secret. The value you provide may be copied globally for
     * the purpose of running the service. The value provided should not include personally
     * identifiable or sensitive information.
     * @param parameters The parameters for setting the secret.
     * @param context The context for the operation can be used for request cancellation.
     * @return A secret consisting of a value, id and its attributes.
     *
     */
    Response<Models::SecretBundle> SetSecret(
        const std::string& secretName,
        const Models::SecretSetParameters& parameters,
        const Core::Context& context = {}) const;

    /**
     * @brief The DELETE operation applies to any secret stored in Azure Key Vault. DELETE cannot be
     * applied to an individual version of a secret. This operation requires the secrets/delete
     * permission.
     * @param secretName The name of the secret.
     * @param context The context for the operation can be used for request cancellation.
     * @return A Deleted Secret consisting of its previous id, attributes and its tags, as well as
     * information on when it will be purged.
     *
     */
    Response<Models::DeletedSecretBundle> DeleteSecret(
        const std::string& secretName,
        const Core::Context& context = {}) const;

    /**
     * @brief The UPDATE operation changes specified attributes of an existing stored secret.
     * Attributes that are not specified in the request are left unchanged. The value of a secret
     * itself cannot be changed. This operation requires the secrets/set permission.
     * @param secretName The name of the secret.
     * @param secretVersion The version of the secret.
     * @param parameters The parameters for update secret operation.
     * @param context The context for the operation can be used for request cancellation.
     * @return A secret consisting of a value, id and its attributes.
     *
     */
    Response<Models::SecretBundle> UpdateSecret(
        const std::string& secretName,
        const std::string& secretVersion,
        const Models::SecretUpdateParameters& parameters,
        const Core::Context& context = {}) const;

    /**
     * @brief The GET operation is applicable to any secret stored in Azure Key Vault. This
     * operation requires the secrets/get permission.
     * @param secretName The name of the secret.
     * @param secretVersion The version of the secret. This URI fragment is optional. If not
     * specified, the latest version of the secret is returned.
     * @param context The context for the operation can be used for request cancellation.
     * @return A secret consisting of a value, id and its attributes.
     *
     */
    Response<Models::SecretBundle> GetSecret(
        const std::string& secretName,
        const std::string& secretVersion,
        const Core::Context& context = {}) const;

    /**
     * @brief The Get Secrets operation is applicable to the entire vault. However, only the base
     * secret identifier and its attributes are provided in the response. Individual secret versions
     * are not listed in the response. This operation requires the secrets/list permission.
     * @param options Optional parameters.
     * @param context The context for the operation can be used for request cancellation.
     * @return The secret list result.
     *
     */
    GetSecretsPagedResponse GetSecrets(
        const KeyVaultClientGetSecretsOptions& options = {},
        const Core::Context& context = {}) const;

    /**
     * @brief The full secret identifier and attributes are provided in the response. No values are
     * returned for the secrets. This operations requires the secrets/list permission.
     * @param secretName The name of the secret.
     * @param options Optional parameters.
     * @param context The context for the operation can be used for request cancellation.
     * @return The secret list result.
     *
     */
    GetSecretVersionsPagedResponse GetSecretVersions(
        const std::string& secretName,
        const KeyVaultClientGetSecretVersionsOptions& options = {},
        const Core::Context& context = {}) const;

    /**
     * @brief The Get Deleted Secrets operation returns the secrets that have been deleted for a
     * vault enabled for soft-delete. This operation requires the secrets/list permission.
     * @param options Optional parameters.
     * @param context The context for the operation can be used for request cancellation.
     * @return The deleted secret list result
     *
     */
    GetDeletedSecretsPagedResponse GetDeletedSecrets(
        const KeyVaultClientGetDeletedSecretsOptions& options = {},
        const Core::Context& context = {}) const;

    /**
     * @brief The Get Deleted Secret operation returns the specified deleted secret along with its
     * attributes. This operation requires the secrets/get permission.
     * @param secretName The name of the secret.
     * @param context The context for the operation can be used for request cancellation.
     * @return A Deleted Secret consisting of its previous id, attributes and its tags, as well as
     * information on when it will be purged.
     *
     */
    Response<Models::DeletedSecretBundle> GetDeletedSecret(
        const std::string& secretName,
        const Core::Context& context = {}) const;

    /**
     * @brief The purge deleted secret operation removes the secret permanently, without the
     * possibility of recovery. This operation can only be enabled on a soft-delete enabled vault.
     * This operation requires the secrets/purge permission.
     * @param secretName The name of the secret.
     * @param context The context for the operation can be used for request cancellation.
     * @return Operation result.
     *
     */
    Response<Models::PurgeDeletedSecretResult> PurgeDeletedSecret(
        const std::string& secretName,
        const Core::Context& context = {}) const;

    /**
     * @brief Recovers the deleted secret in the specified vault. This operation can only be
     * performed on a soft-delete enabled vault. This operation requires the secrets/recover
     * permission.
     * @param secretName The name of the deleted secret.
     * @param context The context for the operation can be used for request cancellation.
     * @return A secret consisting of a value, id and its attributes.
     *
     */
    Response<Models::SecretBundle> RecoverDeletedSecret(
        const std::string& secretName,
        const Core::Context& context = {}) const;

    /**
     * @brief Requests that a backup of the specified secret be downloaded to the client. All
     * versions of the secret will be downloaded. This operation requires the secrets/backup
     * permission.
     * @param secretName The name of the secret.
     * @param context The context for the operation can be used for request cancellation.
     * @return The backup secret result, containing the backup blob.
     *
     */
    Response<Models::BackupSecretResult> BackupSecret(
        const std::string& secretName,
        const Core::Context& context = {}) const;

    /**
     * @brief Restores a backed up secret, and all its versions, to a vault. This operation requires
     * the secrets/restore permission.
     * @param parameters The parameters to restore the secret.
     * @param context The context for the operation can be used for request cancellation.
     * @return A secret consisting of a value, id and its attributes.
     *
     */
    Response<Models::SecretBundle> RestoreSecret(
        const Models::SecretRestoreParameters& parameters,
        const Core::Context& context = {}) const;

  private:
    std::shared_ptr<Core::Http::_internal::HttpPipeline> m_pipeline;
    Core::Url m_url;
    std::string m_apiVersion;
  };
}}}}} // namespace Azure::Security::KeyVault::Secrets::_detail
