# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
#
# Creates custom targets for documentation generation if
# BUILD_DOCUMENTATION == YES
# Usage: generate_documentation(azure-core 1.0.0-preview.1)
# Requires: Doxygen
# Target name in the form of ${PROJECT_NAME}-docs (e.g. azure-core-docs)
# Note that PROJECT_NAME is also the directory containing the package.
function(generate_documentation PROJECT_NAME PROJECT_VERSION)
    if(BUILD_DOCUMENTATION)
        find_package(Doxygen 1.9.7 REQUIRED doxygen)

        set(DOXYGEN_PROJECT_NAME ${PROJECT_NAME})
        set(DOXYGEN_PROJECT_NUMBER ${PROJECT_VERSION})

        # Doc assets are needed from the top-level SDK directory.
        # CMAKE_SOURCE_DIR will not point there if building from a subproject
        # like sdk/core/azure-core, so use PROJECT_SOURCE_DIR instead.
        # Subprojects are assumed to always be 3 levels down from the root.
        set(SDK_ROOT_DIR ${PROJECT_SOURCE_DIR}/../../..)

        set(DOXYGEN_OUTPUT_DIRECTORY docs)
        set(DOXYGEN_LAYOUT_FILE ${SDK_ROOT_DIR}/eng/docs/api/assets/DoxygenLayout.xml)
        set(DOXYGEN_RECURSIVE YES)
        if (MSVC)
          set(DOXYGEN_WARN_FORMAT "$file($line) : $text")
        endif()
        set(DOXYGEN_WARN_AS_ERROR FAIL_ON_WARNINGS)
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ./README.md)
        # Setting the INLINE_SOURCES tag to YES will include the body of functions,
        # classes and enums directly into the documentation.
        set(DOXYGEN_INLINE_SOURCES NO)
        set(DOXYGEN_MARKDOWN_ID_STYLE GITHUB)
        # Skip generating docs for json, test, samples, and private files.
        set(DOXYGEN_EXCLUDE_PATTERNS
            json.hpp
            package_version.hpp
            dll_import_export.hpp
            apiview.hpp
            rtti.hpp
            test
            */out/*
            */build/*
            */samples/*
            */vendor/*
            )
        # Skip documenting internal and private symbols (all from ::_detail/_::internal namespaces)
        set(DOXYGEN_EXCLUDE_SYMBOLS _detail _internal)
        set(DOXYGEN_IGNORE_PREFIX 
            az_
            AZ_
        )
        set(DOXYGEN_HTML_HEADER ${SDK_ROOT_DIR}/eng/docs/api/assets/header.html)
        set(DOXYGEN_HTML_FOOTER ${SDK_ROOT_DIR}/eng/docs/api/assets/footer.html)
        set(DOXYGEN_HTML_STYLESHEET ${SDK_ROOT_DIR}/eng/docs/api/assets/style.css)
        set(DOXYGEN_PROJECT_LOGO ${SDK_ROOT_DIR}/eng/common/docgeneration/assets/logo.svg)

        set(DOXYGEN_GENERATE_XML YES)
        set(DOXYGEN_GENERATE_LATEX NO)
        # Use MathJax instead of latex to render formulas
        set(DOXYGEN_USE_MATHJAX YES)

        set(DOXYGEN_REPEAT_BRIEF NO)

        doxygen_add_docs(${PROJECT_NAME}-docs
            ./inc ./README.md
            ALL
            COMMENT "Generate documentation for ${PROJECT_NAME} with Doxygen Version ${DOXYGEN_VERSION}")
    endif()
endfunction()
