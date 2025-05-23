# Azure Identity client library for C++

The Azure Identity library provides Microsoft Entra ID ([formerly Azure Active Directory](https://learn.microsoft.com/entra/fundamentals/new-name)) token authentication support across the Azure SDK. It provides a set of `TokenCredential` implementations which can be used to construct Azure SDK clients which support Microsoft Entra token authentication.
This library follows the [Azure SDK Design Guidelines for C++][azure_sdk_cpp_development_guidelines].

[Source code][source] | [Package (vcpkg)][identity_vcpkg] | [API reference documentation][doxygen] | [Microsoft Entra ID documentation][meid_doc] | [Samples][samples]

## Getting started

### Prerequisites
- [vcpkg](https://learn.microsoft.com/vcpkg/get_started/overview) for package acquisition and dependency management
- [CMake](https://cmake.org/download/) for project build
- An [Azure subscription][azure_sub]
- The [Azure CLI][azure_cli] can also be useful for authenticating in a development environment, creating accounts, and managing account roles.

### Install the package
The easiest way to acquire the C++ SDK is leveraging the vcpkg package manager and CMake. See the corresponding [Azure SDK for C++ readme section][azsdk_vcpkg_install]. We'll use vcpkg in manifest mode. To start a vcpkg project in manifest mode use the following command at the root of your project: 

```batch
vcpkg new --application
```

To install the Azure Identity package via vcpkg:
To add the Azure Identity package to your vcpkg enter the following command:

```batch
vcpkg add port azure-identity-cpp
```

Then, add the following in your CMake file:

```CMake
find_package(azure-identity-cpp CONFIG REQUIRED)
target_link_libraries(<your project name> PRIVATE Azure::azure-identity)
```

Remember to set `CMAKE_TOOLCHAIN_FILE` to the path to `vcpkg.cmake` either by adding the following to your `CMakeLists.txt` file before your project statement:

```CMake
set(CMAKE_TOOLCHAIN_FILE "vcpkg-root/scripts/buildsystems/vcpkg.cmake")
```

Or by specifying it in your CMake commands with the `-DCMAKE_TOOLCHAIN_FILE` argument.

There is more than one way to acquire and install this library. Check out [our samples on different ways to set up your Azure C++ project][project_set_up_examples].

### Authenticate the client

When debugging and executing code locally it is typical for a developer to use their own account for authenticating calls to Azure services. There are several developer tools which can be used to perform this authentication in your development environment.

#### Authenticate via the Azure CLI

Developers can use the [Azure CLI][azure_cli] to authenticate. Applications using the `DefaultAzureCredential` or the `AzureCliCredential` can then use this account to authenticate calls in their application when running locally.

To authenticate with the [Azure CLI][azure_cli], users can run the command `az login`. For users running on a system with a default web browser, the Azure CLI will launch the browser to authenticate the user.

## Key concepts
### Credentials

A credential is a class which contains or can obtain the data needed for a service client to authenticate requests. Service clients across Azure SDK accept credentials when they are constructed, and service clients use those credentials to authenticate requests to the service.

The Azure Identity library focuses on OAuth authentication with Microsoft Entra ID, and it offers a variety of credential classes capable of acquiring a Microsoft Entra token to authenticate service requests. All of the credential classes in this library are implementations of the `TokenCredential` abstract class in [azure-core][azure_core_library], and any of them can be used by to construct service clients capable of authenticating with a `TokenCredential`.

See [Credential Classes](#credential-classes) for a complete listing of available credential types.

### DefaultAzureCredential

`DefaultAzureCredential` simplifies authentication while developing apps that deploy to Azure by combining credentials used in Azure hosting environments with credentials used in local development. For more information, see [DefaultAzureCredential overview](https://aka.ms/azsdk/cpp/identity/credential-chains#defaultazurecredential-overview).

## Examples

See the [code samples](https://github.com/Azure/azure-sdk-for-cpp/tree/main/sdk/identity/azure-identity/samples).

## Chained Token Credential

`ChainedTokenCredential` allows users to set up custom authentication flow consisting of multiple credentials.

An example below demonstrates using `ChainedTokenCredential` which will attempt to authenticate using `EnvironmentCredential`, and fall back to authenticate using `ManagedIdentityCredential`.

```cpp
// A configuration demonstrated below would authenticate using EnvironmentCredential if it is
// available, and if it is not available, would fall back to use AzureCliCredential, and then to
// ManagedIdentityCredential.
auto chainedTokenCredential = std::make_shared<Azure::Identity::ChainedTokenCredential>(
    Azure::Identity::ChainedTokenCredential::Sources{
        std::make_shared<Azure::Identity::EnvironmentCredential>(),
        std::make_shared<Azure::Identity::AzureCliCredential>(),
        std::make_shared<Azure::Identity::ManagedIdentityCredential>()});

Azure::Service::Client azureServiceClient("serviceUrl", chainedTokenCredential);
```

## Managed Identity Support

The [Managed identity authentication](https://learn.microsoft.com/entra/identity/managed-identities-azure-resources/overview) is supported via the `ManagedIdentityCredential` for the following Azure Services:
* [Azure App Service and Azure Functions](https://learn.microsoft.com/azure/app-service/overview-managed-identity)
* [Azure Cloud Shell](https://learn.microsoft.com/azure/cloud-shell/msi-authorization)
* [Azure Arc](https://learn.microsoft.com/azure/azure-arc/servers/managed-identity-authentication)
* [Azure Virtual Machines](https://learn.microsoft.com/entra/identity/managed-identities-azure-resources/how-to-use-vm-token)

### Specify a user-assigned managed identity with `ManagedIdentityCredential`

Many Azure hosts allow the assignment of a user-assigned managed identity. The following examples demonstrate configuring `ManagedIdentityCredential` to authenticate a user-assigned managed identity when deployed to an Azure host. The sample code uses the credential to authenticate a `BlobClient` from the [Azure::Storage::Blobs][blobs_client_library] client library. It also demonstrates how you can specify a user-assigned managed identity either by a client ID, resource ID, or an object ID.

#### Client ID

To use a client ID, create a `ManagedIdentityId` with `ManagedIdentityIdKind::ClientId`, set that as the `IdentityId` in the `ManagedIdentityCredentialOptions`, and pass that to the `ManagedIdentityCredential` constructor. For example:

<!-- @insert_snippet: UserAssignedManagedIdentityViaClientId -->
```cpp
// When deployed to an Azure host, ManagedIdentityCredential will authenticate the specified
// user-assigned managed identity.

std::string userAssignedClientId = "<your managed identity client ID>";
ManagedIdentityCredentialOptions options;
options.IdentityId = ManagedIdentityId::FromUserAssignedClientId(userAssignedClientId);

auto credential = std::make_shared<ManagedIdentityCredential>(options);
auto blobClient = BlobClient(blobUrl, credential);
```

#### Resource ID

Similarly, to use a resource ID, create a `ManagedIdentityId` with `ManagedIdentityIdKind::ResourceId`, set that as the `IdentityId` in the `ManagedIdentityCredentialOptions`, and pass that to the `ManagedIdentityCredential` constructor. The resource ID takes the form `/subscriptions/{subscriptionId}/resourceGroups/{resourceGroupName}/providers/Microsoft.ManagedIdentity/userAssignedIdentities/{identityName}`. Because resource IDs can be built by convention, they can be more convenient when there are a large number of user-assigned managed identities in your environment. For example:

<!-- @insert_snippet: UserAssignedManagedIdentityViaResourceId -->
```cpp
std::string userAssignedResourceId = "/subscriptions/<your managed identity resource ID>";
ManagedIdentityCredentialOptions options;
options.IdentityId = ManagedIdentityId::FromUserAssignedResourceId(
    Azure::Core::ResourceIdentifier(userAssignedResourceId));

auto credential = std::make_shared<ManagedIdentityCredential>(options);
auto blobClient = BlobClient(blobUrl, credential);
```

#### Object ID

Similarly, to use an object ID, create a `ManagedIdentityId` with `ManagedIdentityIdKind::ObjectId`, set that as the `IdentityId` in the `ManagedIdentityCredentialOptions`, and pass that to the `ManagedIdentityCredential` constructor. For example:

<!-- @insert_snippet: UserAssignedManagedIdentityViaObjectId -->
```cpp
std::string userAssignedObjectId = "<your managed identity object ID>";
ManagedIdentityCredentialOptions options;
options.IdentityId = ManagedIdentityId::FromUserAssignedObjectId(userAssignedObjectId);

auto credential = std::make_shared<ManagedIdentityCredential>(options);
auto blobClient = BlobClient(blobUrl, credential);
```

### Specify a system-assigned managed identity with `ManagedIdentityCredential`

You can express your intent to use a system-assigned managed identity, explicitly, by creating a `ManagedIdentityId` with `ManagedIdentityIdKind::SystemAssigned` and an empty ID value, set that as the `IdentityId` in the `ManagedIdentityCredentialOptions`, and pass that to the `ManagedIdentityCredential` constructor. For example: 

<!-- @insert_snippet: SystemAssignedManagedIdentity -->
```cpp
ManagedIdentityCredentialOptions options;
options.IdentityId = ManagedIdentityId::SystemAssigned();

auto credential = std::make_shared<ManagedIdentityCredential>(options);
auto blobClient = BlobClient(blobUrl, credential);
```

An alternative way to specify a system-assigned managed identity, implicitly, is by calling the default `ManagedIdentityCredential` constructor. For example:

<!-- @insert_snippet: SystemAssignedManagedIdentityBrief -->
```cpp
auto credential = std::make_shared<ManagedIdentityCredential>();
auto blobClient = BlobClient(blobUrl, credential);
```

## Environment Variables

`DefaultAzureCredential` and `EnvironmentCredential` can be configured with environment variables. Each type of authentication requires values for specific variables:

### Service principal with secret

|Variable name|Value
|-|-
|`AZURE_TENANT_ID`|ID of the application's Microsoft Entra tenant
|`AZURE_CLIENT_ID`|ID of a Microsoft Entra application
|`AZURE_CLIENT_SECRET`|one of the application's client secrets
|`AZURE_AUTHORITY_HOST`|(optional) [authentication authority URL](https://learn.microsoft.com/entra/identity-platform/authentication-national-cloud)

### Service principal with certificate

|variable name|Value
|-|-
|`AZURE_CLIENT_ID`|ID of a Microsoft Entra application
|`AZURE_TENANT_ID`|ID of the application's Microsoft Entra tenant
|`AZURE_CLIENT_CERTIFICATE_PATH`|path to a PFX or PEM-encoded certificate file including private key
|`AZURE_AUTHORITY_HOST`|(optional) [authentication authority URL](https://learn.microsoft.com/entra/identity-platform/authentication-national-cloud)

Configuration is attempted in the above order. For example, if values for a client secret and certificate are both present, the client secret will be used.

## Credential classes

### Credential chains

|Credential | Usage
|-|-
|`DefaultAzureCredential`|Provides a simplified authentication experience to quickly start developing applications run in Azure.
|`ChainedTokenCredential`|Allows users to define custom authentication flows composing multiple credentials.

### Authenticate Azure-hosted applications

|Credential | Usage
|-|-
|`ManagedIdentityCredential`|Authenticates the managed identity of an Azure resource.
|`EnvironmentCredential`|Authenticates a service principal or user via credential information specified in environment variables.
|`WorkloadIdentityCredential`|Authenticate a workload identity on Kubernetes.

### Authenticate service principals

|Credential | Usage
|-|-
|`AzurePipelinesCredential`|Supports [Microsoft Entra Workload ID](https://learn.microsoft.com/azure/devops/pipelines/release/configure-workload-identity?view=azure-devops) on Azure Pipelines.
|`ClientAssertionCredential`|Authenticates a service principal using a signed client assertion.
|`ClientSecretCredential`|Authenticates a service principal [using a secret](https://learn.microsoft.com/entra/identity-platform/app-objects-and-service-principals).
|`ClientCertificateCredential`|Authenticates a service principal [using a certificate](https://learn.microsoft.com/entra/identity-platform/app-objects-and-service-principals).

### Authenticate via development tools

|Credential | Usage
|-|-
|`AzureCliCredential`|Authenticates in a development environment [with the Azure CLI](https://learn.microsoft.com/cli/azure/authenticate-azure-cli).

## Troubleshooting

1. Azure Identity [SDK log messages](https://github.com/Azure/azure-sdk-for-cpp/tree/main/sdk/core/azure-core#sdk-log-messages) contain disgnostic information, and start with "`Identity: `".
1. Credentials raise exceptions either when they fail to authenticate or cannot execute authentication. When a credential fails to authenticate, an `AuthenticationException` is thrown. The exception has the `what()` function that provides more information about the failure.

## Contributing
For details on contributing to this repository, see the [contributing guide][azure_sdk_for_cpp_contributing].

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit the [Contributor License Agreement](https://cla.microsoft.com).

When you submit a pull request, a CLA-bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

### Additional Helpful Links for Contributors
Many people all over the world have helped make this project better.  You'll want to check out:

* [What are some good first issues for new contributors to the repo?](https://github.com/azure/azure-sdk-for-cpp/issues?q=is%3Aopen+is%3Aissue+label%3A%22up+for+grabs%22)
* [How to build and test your change][azure_sdk_for_cpp_contributing_developer_guide]
* [How you can make a change happen!][azure_sdk_for_cpp_contributing_pull_requests]
* Frequently Asked Questions (FAQ) and Conceptual Topics in the detailed [Azure SDK for C++ wiki](https://github.com/azure/azure-sdk-for-cpp/wiki).

<!-- ### Community-->
### Reporting security issues and security bugs

Security issues and bugs should be reported privately, via email, to the Microsoft Security Response Center (MSRC) <secure@microsoft.com>. You should receive a response within 24 hours. If for some reason you do not, please follow up via email to ensure we received your original message. Further information, including the MSRC PGP key, can be found in the [Security TechCenter](https://www.microsoft.com/msrc/faqs-report-an-issue).

### License

Azure SDK for C++ is licensed under the [MIT](https://github.com/Azure/azure-sdk-for-cpp/blob/main/LICENSE.txt) license.

<!-- LINKS -->
[azure_cli]: https://learn.microsoft.com/cli/azure
[azure_sub]: https://azure.microsoft.com/free/
[azsdk_vcpkg_install]: https://github.com/Azure/azure-sdk-for-cpp#download--install-the-sdk
[azure_sdk_for_cpp_contributing]: https://github.com/Azure/azure-sdk-for-cpp/blob/main/CONTRIBUTING.md
[azure_sdk_for_cpp_contributing_developer_guide]: https://github.com/Azure/azure-sdk-for-cpp/blob/main/CONTRIBUTING.md#developer-guide
[azure_sdk_for_cpp_contributing_pull_requests]: https://github.com/Azure/azure-sdk-for-cpp/blob/main/CONTRIBUTING.md#pull-requests
[azure_sdk_cpp_development_guidelines]: https://azure.github.io/azure-sdk/cpp_introduction.html
[source]: https://github.com/Azure/azure-sdk-for-cpp/tree/main/sdk/identity/azure-identity
[samples]: https://github.com/Azure/azure-sdk-for-cpp/tree/main/sdk/identity/azure-identity/samples
[blobs_client_library]: https://github.com/Azure/azure-sdk-for-cpp/tree/main/sdk/storage/azure-storage-blobs
[meid_doc]: https://learn.microsoft.com/entra/identity/
[azure_core_library]: https://github.com/Azure/azure-sdk-for-cpp/tree/main/sdk/core
[doxygen]: https://azure.github.io/azure-sdk-for-cpp/
[identity_vcpkg]: https://vcpkg.io/en/package/azure-identity-cpp
[project_set_up_examples]: https://github.com/Azure/azure-sdk-for-cpp/tree/main/samples/integration