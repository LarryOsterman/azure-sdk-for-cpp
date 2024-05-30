// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
cspell: words reqwest repr staticlib dylib brotli gzip
*/

#![crate_type = "staticlib"]

use azure_core::{error::ErrorKind, Error, Result};
use http_result::RustHttpResult;
use log::trace;
use std::mem;

mod byte_array;
mod cpp_body_stream;
mod header_iterator;
mod http_header;
mod http_request;
mod http_result;
mod raw_response;
mod runtime_context;
mod rust_body_stream;
mod rust_error;
use crate::http_request::RustRequest;
use crate::raw_response::RustRawResponse;
use crate::runtime_context::{runtime_context_from_ptr, RuntimeContext};

pub struct ReqwestWrapper {
    client: reqwest::Client,
}

// impl Drop for ReqwestWrapper {
//     fn drop(&mut self) {
//         println!("Dropping ReqwestWrapper");
//     }
// }

impl ReqwestWrapper {
    pub fn new() -> Self {
        Self {
            client: reqwest::Client::builder()
                //                .no_brotli()
                //                .no_gzip()
                //                .no_deflate()
                .build()
                .unwrap(),
        }
    }

    async fn send(&self, request: &mut RustRequest) -> Result<RustRawResponse> {
        trace!("Sending request: {:?}", request);
        let reqwest_request = request.build_reqwest()?;

        trace!("Request built, executing.");
        let reqwest_response = self
            .client
            .execute(reqwest_request)
            .await
            .map_err(|err| Error::new(ErrorKind::Other, err))?;
        trace!("Response received: {:?}", reqwest_response);
        return Ok(RustRawResponse::new(reqwest_response));
    }
}

#[no_mangle]
pub extern "C" fn reqwest_wrapper_new() -> *mut ReqwestWrapper {
    Box::into_raw(Box::new(ReqwestWrapper::new()))
}

#[no_mangle]
pub unsafe extern "C" fn reqwest_wrapper_delete(http_client: *mut ReqwestWrapper) {
    mem::drop(Box::from_raw(http_client))
}

#[no_mangle]
pub extern "C" fn reqwest_wrapper_send<'a>(
    runtime: *const RuntimeContext,
    http_client: *const ReqwestWrapper,
    request: *mut RustRequest,
) -> *mut RustHttpResult {
    trace!(
        "Sending request on context {:?} with client {:?} and request {:?}",
        runtime_context_from_ptr(runtime),
        http_client,
        request
    );
    let client = unsafe { &(*http_client) };
    let request = unsafe { &mut *request };

    let raw_response = runtime_context_from_ptr(runtime)
        .runtime()
        .block_on(client.send(request));
    Box::into_raw(Box::new(RustHttpResult::new(raw_response)))
}

#[no_mangle]
pub extern "C" fn initialize() {
    env_logger::init();
}

#[cfg(test)]
mod tests {
    //    use super::*;

    #[test]
    fn it_works() {
        {}
    }
}
