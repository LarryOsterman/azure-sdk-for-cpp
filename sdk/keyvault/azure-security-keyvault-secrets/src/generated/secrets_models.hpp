// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See License.txt in the project root for license information.
// Code generated by Microsoft (R) TypeSpec Code Generator.
// Changes may cause incorrect behavior and will be lost if the code is regenerated.

#pragma once

#include "azure/keyvault/secrets/dll_import_export.hpp"

#include <azure/core/datetime.hpp>
#include <azure/core/internal/extendable_enumeration.hpp>
#include <azure/core/nullable.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace Azure { namespace Security { namespace KeyVault { namespace Secrets { namespace _detail {
  namespace Models {
    /**
     * @brief Reflects the deletion recovery level currently in effect for secrets in the current
     * vault. If it contains 'Purgeable', the secret can be permanently deleted by a privileged
     * user; otherwise, only the system can purge the secret, at the end of the retention interval.
     *
     */
    class DeletionRecoveryLevel final
        : public Core::_internal::ExtendableEnumeration<DeletionRecoveryLevel> {
    public:
      /**
       * @brief Constructs a new DeletionRecoveryLevel instance.
       *
       **/
      DeletionRecoveryLevel() = default;

      /**
       * @brief Constructs a new DeletionRecoveryLevel instance from a string.
       * @param deletionRecoveryLevel String value to construct the new instance from.
       *
       **/
      explicit DeletionRecoveryLevel(std::string deletionRecoveryLevel)
          : ExtendableEnumeration(std::move(deletionRecoveryLevel))
      {
      }

      /// Denotes a vault state in which deletion is an irreversible operation, without the
      /// possibility for recovery. This level corresponds to no protection being available against
      /// a Delete operation; the data is irretrievably lost upon accepting a Delete operation at
      /// the entity level or higher (vault, resource group, subscription etc.)
      AZ_SECURITY_KEYVAULT_SECRETS_DLLEXPORT static const DeletionRecoveryLevel Purgeable;

      /// Denotes a vault state in which deletion is recoverable, and which also permits immediate
      /// and permanent deletion (i.e. purge). This level guarantees the recoverability of the
      /// deleted entity during the retention interval (90 days), unless a Purge operation is
      /// requested, or the subscription is cancelled. System wil permanently delete it after 90
      /// days, if not recovered
      AZ_SECURITY_KEYVAULT_SECRETS_DLLEXPORT static const DeletionRecoveryLevel
          RecoverablePurgeable;

      /// Denotes a vault state in which deletion is recoverable without the possibility for
      /// immediate and permanent deletion (i.e. purge). This level guarantees the recoverability of
      /// the deleted entity during the retention interval (90 days) and while the subscription is
      /// still available. System wil permanently delete it after 90 days, if not recovered
      AZ_SECURITY_KEYVAULT_SECRETS_DLLEXPORT static const DeletionRecoveryLevel Recoverable;

      /// Denotes a vault and subscription state in which deletion is recoverable within retention
      /// interval (90 days), immediate and permanent deletion (i.e. purge) is not permitted, and in
      /// which the subscription itself  cannot be permanently canceled. System wil permanently
      /// delete it after 90 days, if not recovered
      AZ_SECURITY_KEYVAULT_SECRETS_DLLEXPORT static const DeletionRecoveryLevel
          RecoverableProtectedSubscription;

      /// Denotes a vault state in which deletion is recoverable, and which also permits immediate
      /// and permanent deletion (i.e. purge when 7 <= SoftDeleteRetentionInDays < 90). This level
      /// guarantees the recoverability of the deleted entity during the retention interval, unless
      /// a Purge operation is requested, or the subscription is cancelled.
      AZ_SECURITY_KEYVAULT_SECRETS_DLLEXPORT static const DeletionRecoveryLevel
          CustomizedRecoverablePurgeable;

      /// Denotes a vault state in which deletion is recoverable without the possibility for
      /// immediate and permanent deletion (i.e. purge when 7 <= SoftDeleteRetentionInDays <
      /// 90).This level guarantees the recoverability of the deleted entity during the retention
      /// interval and while the subscription is still available.
      AZ_SECURITY_KEYVAULT_SECRETS_DLLEXPORT static const DeletionRecoveryLevel
          CustomizedRecoverable;

      /// Denotes a vault and subscription state in which deletion is recoverable, immediate and
      /// permanent deletion (i.e. purge) is not permitted, and in which the subscription itself
      /// cannot be permanently canceled when 7 <= SoftDeleteRetentionInDays < 90. This level
      /// guarantees the recoverability of the deleted entity during the retention interval, and
      /// also reflects the fact that the subscription itself cannot be cancelled.
      AZ_SECURITY_KEYVAULT_SECRETS_DLLEXPORT static const DeletionRecoveryLevel
          CustomizedRecoverableProtectedSubscription;
    };

    /**
     * @brief The secret management attributes.
     *
     */
    struct SecretAttributes final
    {
      /// Determines whether the object is enabled.
      Nullable<bool> Enabled;

      /// Not before date in UTC.
      Nullable<DateTime> NotBefore;

      /// Expiry date in UTC.
      Nullable<DateTime> Expires;

      /// [out] Creation time in UTC.
      Nullable<DateTime> Created;

      /// [out] Last updated time in UTC.
      Nullable<DateTime> Updated;

      /// [out] softDelete data retention days. Value should be >=7 and <=90 when softDelete
      /// enabled, otherwise 0.
      Nullable<std::int32_t> RecoverableDays;

      /// [out] Reflects the deletion recovery level currently in effect for secrets in the current
      /// vault. If it contains 'Purgeable', the secret can be permanently deleted by a privileged
      /// user; otherwise, only the system can purge the secret, at the end of the retention
      /// interval.
      Nullable<DeletionRecoveryLevel> RecoveryLevel;
    };

    /**
     * @brief A secret consisting of a value, id and its attributes.
     *
     */
    struct SecretBundle final
    {
      /// The secret value.
      Nullable<std::string> Value;

      /// The secret id.
      Nullable<std::string> Id;

      /// The content type of the secret.
      Nullable<std::string> ContentType;

      /// The secret management attributes.
      Nullable<SecretAttributes> Attributes;

      /// Application specific metadata in the form of key-value pairs.
      Nullable<std::map<std::string, std::string>> Tags;

      /// [out] If this is a secret backing a KV certificate, then this field specifies the
      /// corresponding key backing the KV certificate.
      Nullable<std::string> Kid;

      /// [out] True if the secret's lifetime is managed by key vault. If this is a secret backing a
      /// certificate, then managed will be true.
      Nullable<bool> Managed;
    };

    /**
     * @brief The secret set parameters.
     *
     */
    struct SecretSetParameters final
    {
      /// The value of the secret.
      std::string Value;

      /// Application specific metadata in the form of key-value pairs.
      Nullable<std::map<std::string, std::string>> Tags;

      /// Type of the secret value such as a password.
      Nullable<std::string> ContentType;

      /// The secret management attributes.
      Nullable<Models::SecretAttributes> SecretAttributes;
    };

    /**
     * @brief A Deleted Secret consisting of its previous id, attributes and its tags, as well as
     * information on when it will be purged.
     *
     */
    struct DeletedSecretBundle final
    {
      /// The secret value.
      Nullable<std::string> Value;

      /// The secret id.
      Nullable<std::string> Id;

      /// The content type of the secret.
      Nullable<std::string> ContentType;

      /// The secret management attributes.
      Nullable<SecretAttributes> Attributes;

      /// Application specific metadata in the form of key-value pairs.
      Nullable<std::map<std::string, std::string>> Tags;

      /// [out] If this is a secret backing a KV certificate, then this field specifies the
      /// corresponding key backing the KV certificate.
      Nullable<std::string> Kid;

      /// [out] True if the secret's lifetime is managed by key vault. If this is a secret backing a
      /// certificate, then managed will be true.
      Nullable<bool> Managed;

      /// The url of the recovery object, used to identify and recover the deleted secret.
      Nullable<std::string> RecoveryId;

      /// [out] The time when the secret is scheduled to be purged, in UTC
      Nullable<DateTime> ScheduledPurgeDate;

      /// [out] The time when the secret was deleted, in UTC
      Nullable<DateTime> DeletedDate;
    };

    /**
     * @brief The secret update parameters.
     *
     */
    struct SecretUpdateParameters final
    {
      /// Type of the secret value such as a password.
      Nullable<std::string> ContentType;

      /// The secret management attributes.
      Nullable<Models::SecretAttributes> SecretAttributes;

      /// Application specific metadata in the form of key-value pairs.
      Nullable<std::map<std::string, std::string>> Tags;
    };

    /**
     * @brief The secret item containing secret metadata.
     *
     */
    struct SecretItem final
    {
      /// Secret identifier.
      Nullable<std::string> Id;

      /// The secret management attributes.
      Nullable<SecretAttributes> Attributes;

      /// Application specific metadata in the form of key-value pairs.
      Nullable<std::map<std::string, std::string>> Tags;

      /// Type of the secret value such as a password.
      Nullable<std::string> ContentType;

      /// [out] True if the secret's lifetime is managed by key vault. If this is a key backing a
      /// certificate, then managed will be true.
      Nullable<bool> Managed;
    };

    /**
     * @brief The deleted secret item containing metadata about the deleted secret.
     *
     */
    struct DeletedSecretItem final
    {
      /// Secret identifier.
      Nullable<std::string> Id;

      /// The secret management attributes.
      Nullable<SecretAttributes> Attributes;

      /// Application specific metadata in the form of key-value pairs.
      Nullable<std::map<std::string, std::string>> Tags;

      /// Type of the secret value such as a password.
      Nullable<std::string> ContentType;

      /// [out] True if the secret's lifetime is managed by key vault. If this is a key backing a
      /// certificate, then managed will be true.
      Nullable<bool> Managed;

      /// The url of the recovery object, used to identify and recover the deleted secret.
      Nullable<std::string> RecoveryId;

      /// [out] The time when the secret is scheduled to be purged, in UTC
      Nullable<DateTime> ScheduledPurgeDate;

      /// [out] The time when the secret was deleted, in UTC
      Nullable<DateTime> DeletedDate;
    };

    /**
     * @brief PurgeDeletedSecret operation result.
     *
     */
    struct PurgeDeletedSecretResult final
    {
    };

    /**
     * @brief The backup secret result, containing the backup blob.
     *
     */
    struct BackupSecretResult final
    {
      /// [out] The backup blob containing the backed up secret.
      Nullable<std::vector<std::uint8_t>> Value;
    };

    /**
     * @brief The secret restore parameters.
     *
     */
    struct SecretRestoreParameters final
    {
      /// The backup blob associated with a secret bundle.
      std::vector<std::uint8_t> SecretBundleBackup;
    };
}}}}}} // namespace Azure::Security::KeyVault::Secrets::_detail::Models
