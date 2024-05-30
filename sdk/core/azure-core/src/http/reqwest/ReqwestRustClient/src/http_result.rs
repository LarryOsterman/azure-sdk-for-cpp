// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// cspell: words reqwest repr staticlib dylib

use crate::{rust_error::RustError, RustRawResponse};
use azure_core::Result;

/// An HttpResult is a dummy type whose purpose is to wrap a Result<RawResponse> type
/// when presented to C++ code.
pub struct RustHttpResult(pub(crate) Result<RustRawResponse>);

impl RustHttpResult {
    pub(crate) fn new(result: Result<RustRawResponse>) -> Self {
        RustHttpResult(result)
    }
}

pub(crate) fn result_from_http_result<'a>(
    http_result: *const RustHttpResult,
) -> &'a Result<RustRawResponse> {
    let http_result = unsafe { http_result.as_ref().unwrap() };
    &http_result.0
}

#[no_mangle]
pub extern "C" fn result_has_error(result: *const RustHttpResult) -> bool {
    let result = result_from_http_result(result);
    result.is_err()
}

#[no_mangle]
pub extern "C" fn result_extract_error(result: *mut RustHttpResult) -> *mut RustError {
    let result = unsafe { Box::from_raw(result).0 };
    if let Err(err) = result {
        Box::into_raw(Box::new(RustError::new(err)))
    } else {
        std::ptr::null_mut()
    }
}
#[no_mangle]
pub extern "C" fn result_extract_response(result: *mut RustHttpResult) -> *mut RustRawResponse {
    let result = unsafe { Box::from_raw(result).0 };
    if let Ok(response) = result {
        Box::into_raw(Box::new(response))
    } else {
        std::ptr::null_mut()
    }
}

#[no_mangle]
pub extern "C" fn result_delete(result: *mut RustHttpResult) {
    unsafe {
        let _ = Box::from_raw(result);
    }
}
