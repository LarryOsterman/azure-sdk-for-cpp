if(EXISTS "G:/Az/LarryO/azure-sdk-for-cpp/sdk/attestation/azure-security-attestation/test/azure-attestation-test[1]_tests.cmake")
  include("G:/Az/LarryO/azure-sdk-for-cpp/sdk/attestation/azure-security-attestation/test/azure-attestation-test[1]_tests.cmake")
else()
  add_test(azure-attestation-test_NOT_BUILT azure-attestation-test_NOT_BUILT)
endif()
