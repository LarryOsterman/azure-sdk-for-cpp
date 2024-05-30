// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
cspell: words reqwest repr staticlib dylib azsdk
*/

use bytes::Bytes;
use futures::TryStreamExt;
use reqwest::Version;

use crate::byte_array::RustByteArray;
use crate::header_iterator::RustHeaderIterator;
use crate::runtime_context::{runtime_context_from_ptr_mut, RuntimeContext};
use crate::rust_body_stream::RustBodyStream;
use azure_core::Result;

#[derive(Debug)]
pub struct RustRawResponse {
    response: reqwest::Response,
}

//impl Drop for RustRawResponse {
//	fn drop(&mut self) {
//		println!("Dropping RustRawResponse");
//	}
//}

impl RustRawResponse {
    pub(crate) fn new(response: reqwest::Response) -> Self {
        RustRawResponse { response }
    }

    pub(crate) fn get_status_code(&self) -> u16 {
        self.response.status().into()
    }

    fn get_version(&self) -> (i16, i16) {
        match self.response.version() {
            Version::HTTP_09 => (0, 9),
            Version::HTTP_10 => (1, 0),
            Version::HTTP_11 => (1, 1),
            Version::HTTP_2 => (2, 0),
            Version::HTTP_3 => (3, 0),
            _ => (0, 0),
        }
    }

    fn get_header_iterator(&self) -> RustHeaderIterator {
        RustHeaderIterator::new(&self.response.headers())
    }

    async fn get_body_chunk(&mut self) -> Result<Option<Bytes>> {
        let bytes = self
            .response
            .chunk()
            .await
            .map_err(|err| azure_core::Error::new(azure_core::error::ErrorKind::Other, err))?;
        Ok(bytes)
    }

    fn get_content_length(&self) -> u64 {
        self.response.content_length().unwrap_or(0xffffffffffffffff)
    }

    fn get_body_as_stream(self) -> *mut RustBodyStream {
        let body_stream = self
            .response
            .bytes_stream()
            .map_err(|err| azure_core::Error::new(azure_core::error::ErrorKind::Other, err));
        let stream = RustBodyStream::new(body_stream);
        Box::into_raw(Box::new(stream))
    }

    //    extern fn extract_body_stream(&mut self) -> azure_core::Result<dyn Stream<Item=azure_core::Result<Bytes>>>;
}

#[no_mangle]
pub extern "C" fn raw_response_get_header_iterator<'a>(
    response: *const RustRawResponse,
) -> *mut RustHeaderIterator<'a> {
    let response = unsafe { &*response };
    Box::into_raw(Box::new(response.get_header_iterator()))
}

#[no_mangle]
pub extern "C" fn raw_response_get_status(response: *const RustRawResponse) -> u16 {
    let response = unsafe { &*response };
    response.get_status_code()
}

#[repr(C)]
pub struct HttpVersion {
    major: i16,
    minor: i16,
}

#[no_mangle]
pub extern "C" fn raw_response_get_version(response: *const RustRawResponse) -> HttpVersion {
    let response = unsafe { &*response };
    let (major, minor) = response.get_version();
    HttpVersion { major, minor }
}

#[no_mangle]
pub extern "C" fn raw_response_get_content_length(response: *const RustRawResponse) -> u64 {
    let response = unsafe { &*response };
    response.get_content_length()
}

#[no_mangle]
pub extern "C" fn raw_response_extract_body_stream(
    response: *mut RustRawResponse,
) -> *mut RustBodyStream {
    unsafe {
        let response = Box::from_raw(response);
        // Note that get_body_as_stream() will consume the raw_response, so it will be dropped when it returns.
        return response.get_body_as_stream();
    }
}

#[no_mangle]
pub extern "C" fn raw_response_get_body_bytes(
    runtime_context: *mut RuntimeContext,
    response: *mut RustRawResponse,
) -> *mut RustByteArray {
    let response = unsafe { &mut *response };
    let runtime_context = runtime_context_from_ptr_mut(runtime_context);
    let bytes = runtime_context
        .runtime()
        .block_on(response.get_body_chunk());
    match bytes {
        Ok(bytes) => {
            //                    println!("Success reading chunk.");
            if let Some(bytes) = bytes {
                //                        println!("Chunk has data");
                let bytes = Box::new(RustByteArray::new(bytes.to_vec()));
                Box::into_raw(bytes)
            } else {
                std::ptr::null_mut()
            }
        }
        Err(err) => {
            println!("Error reading stream: {:?}", err);
            runtime_context.set_error(err);

            std::ptr::null_mut()
        }
    }
}

#[no_mangle]
pub extern "C" fn raw_response_delete(raw_response: *mut RustRawResponse) {
    unsafe {
        let _ = Box::from_raw(raw_response);
    }
}

#[cfg(test)]
mod tests {
    use std::ffi::CString;

    use crate::{
        http_request::{
            request_delete, request_new, request_set_header, request_set_method, request_set_url,
        },
        http_result::{result_extract_response, result_has_error},
        reqwest_wrapper_delete, reqwest_wrapper_new, reqwest_wrapper_send,
        runtime_context::runtime_context_new,
    };

    use super::*;

    #[tokio::test]
    async fn http_request() {
        {
            let client = reqwest::Client::new();
            let response = client.get("https://microsoft.com/").send().await;
            assert!(response.is_ok());
            let mut raw_response = RustRawResponse::new(response.ok().unwrap());
            let version = raw_response.get_version();
            println!("Version: {:?}", version);
            let status_code = raw_response.get_status_code();
            println!("Status code: {}", status_code);
            let mut header_iterator = raw_response.get_header_iterator();
            while let Some(h) = header_iterator.get_next() {
                println!("{}: {}", h.0, h.1);
            }
            let bytes = raw_response.get_body_chunk().await.unwrap().unwrap();
            println!("Body: {:?}", bytes.len());
        }
    }

    #[test]
    fn http_request_sync() {
        let client = reqwest::Client::new();
        let response = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap()
            .block_on(client.get("https://microsoft.com/").send());

        assert!(response.is_ok());
        let mut raw_response = RustRawResponse::new(response.ok().unwrap());
        let version = raw_response.get_version();
        println!("Version: {:?}", version);
        let status_code = raw_response.get_status_code();
        println!("Status code: {}", status_code);
        let mut header_iterator = raw_response.get_header_iterator();
        while let Some(h) = header_iterator.get_next() {
            println!("{}: {}", h.0, h.1);
        }
        let bytes = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap()
            .block_on(raw_response.get_body_chunk())
            .unwrap()
            .unwrap();
        println!("Body: {:?}", bytes.len());
    }

    #[test]
    fn http_request_sync2() {
        let runtime = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();
        let client = reqwest::Client::new();
        let response = runtime.block_on(client.get("https://microsoft.com/").send());

        assert!(response.is_ok());
        let mut raw_response = RustRawResponse::new(response.ok().unwrap());
        let version = raw_response.get_version();
        println!("Version: {:?}", version);
        let status_code = raw_response.get_status_code();
        println!("Status code: {}", status_code);
        let mut header_iterator = raw_response.get_header_iterator();
        while let Some(h) = header_iterator.get_next() {
            println!("{}: {}", h.0, h.1);
        }
        let bytes = runtime
            .block_on(raw_response.get_body_chunk())
            .unwrap()
            .unwrap();
        println!("Body: {:?}", bytes.len());
    }

    #[test]
    fn http_request_sync3() {
        let runtime = runtime_context_new();

        let cpp_http = reqwest_wrapper_new();
        let request = request_new();
        let method = CString::new(String::from("GET")).unwrap();
        request_set_method(request, method.as_ptr());
        let url =
            CString::new(String::from("https://azuresdkforcpp.azurewebsites.net/get")).unwrap();
        request_set_url(request, url.as_ptr());
        let user_agent = CString::new(String::from("user-agent")).unwrap();
        let user_agent_value = CString::new(String::from("azsdk-cpp-TransportTest/X.X (Windows 10 Enterprise 6.3 22631 22621.1.amd64fre.ni_release.220506-1250)")).unwrap();
        request_set_header(request, user_agent.as_ptr(), user_agent_value.as_ptr());

        let x_ms_client_request_id = CString::new(String::from("x-ms-client-request-id")).unwrap();
        let x_ms_client_request_id_value =
            CString::new(String::from("99acf84d-bc6d-43b6-b892-9fc5d14c8bb5")).unwrap();
        request_set_header(
            request,
            x_ms_client_request_id.as_ptr(),
            x_ms_client_request_id_value.as_ptr(),
        );
        let result = reqwest_wrapper_send(runtime, cpp_http, request);

        assert!(!result_has_error(result));

        let raw_response = result_extract_response(result);

        assert!(raw_response_get_content_length(raw_response) > 0);

        raw_response_delete(raw_response);
        unsafe {
            request_delete(request);
        }
        unsafe {
            reqwest_wrapper_delete(cpp_http);
        }
    }
}
