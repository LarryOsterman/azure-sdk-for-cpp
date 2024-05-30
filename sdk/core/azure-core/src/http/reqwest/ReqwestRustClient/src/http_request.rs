// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// cspell: words reqwest repr staticlib dylib

use std::{ffi::CString, mem};

use azure_core::{error::ErrorKind, Error, Result};
use reqwest::{
    header::{HeaderMap, HeaderName, HeaderValue, IntoHeaderName},
    Body, Method,
};
use std::ffi::{c_char, CStr};
use url::Url;

use crate::{
    cpp_body_stream::{CppBodyStream, CppStream},
    header_iterator::RustHeaderIterator,
    runtime_context::{runtime_context_from_ptr, RuntimeContext},
    RustRawResponse,
};

#[derive(Debug)]
pub struct RustRequest {
    method: String,
    url: String,

    //    headers: HashMap<String, String>,
    headers: HeaderMap,
    body: Option<CppBodyStream>,
}

impl RustRequest {
    fn new() -> Self {
        RustRequest {
            method: String::new(),
            url: String::new(),
            headers: HeaderMap::new(),
            body: None,
        }
    }

    pub(crate) fn build_reqwest(&mut self) -> Result<reqwest::Request> {
        let method = Method::from_bytes(self.method.as_bytes())
            .map_err(|err| azure_core::Error::new(ErrorKind::DataConversion, err))?;
        let url = Url::parse(&self.url).map_err(Error::from)?;

        let mut reqwest_request = reqwest::Request::new(method, url);
        for (name, value) in &self.headers {
            reqwest_request
                .headers_mut()
                .append(name.clone(), value.clone());
        }

        if let Some(body) = self.body {
            reqwest_request.body_mut().replace(Body::wrap_stream(body));
        }
        Ok(reqwest_request)
    }
    fn set_url(&mut self, url: &str) {
        self.url = url.to_string();
    }
    fn set_method(&mut self, method: &str) {
        self.method = method.to_string();
    }
    fn set_header(&mut self, name: impl IntoHeaderName, value: impl Into<HeaderValue>) {
        self.headers.insert(name, value.into());
    }
    fn remove_header(&mut self, name: &str) {
        self.headers.remove(name);
    }

    fn get_header_iterator(&self) -> RustHeaderIterator {
        RustHeaderIterator::new(&self.headers)
    }

    fn set_body(&mut self, body: *mut CppStream) {
        self.body = Some(CppBodyStream(body));
    }

    // fn get_header(&self, name: impl AsHeaderName + Clone) -> Option<String> {
    //     let name_clone = name.clone();
    //     if self.headers.contains_key(name_clone) {
    //         return Some(self.headers[name].to_str().unwrap().to_string());
    //     }
    //     None
    // }
    // fn get_body_stream(&self) -> &dyn Stream<Item = Result<Vec<u8>>> {
    //     todo!();
    // }
    // fn get_method(&self) -> reqwest::Method {
    //     todo!();
    // }
    // fn should_buffer_response(&self) -> bool {
    //     todo!();
    // }
    fn get_url(&self) -> &str {
        self.url.as_str()
    }
}

unsafe fn str_from_raw_parts<'a>(ptr: *const c_char) -> &'a str {
    CStr::from_ptr(ptr).to_str().unwrap()
}

//    pub fn request_new() -> Request;
//    pub fn request_delete(request: Request);
#[no_mangle]
pub extern "C" fn request_set_header(
    request: *mut RustRequest,
    name: *const c_char,
    value: *const c_char,
) {
    let request = unsafe { &mut *request };
    let name = unsafe { str_from_raw_parts(name) };
    let value = unsafe { str_from_raw_parts(value) };
    request.set_header(
        HeaderName::from_bytes(name.as_bytes()).unwrap(),
        HeaderValue::from_str(value).unwrap(),
    );
}

#[no_mangle]
pub extern "C" fn request_set_body(request: *mut RustRequest, body: *mut CppStream) {
    let request = unsafe { &mut *request };
    request.set_body(body);
}

#[no_mangle]
pub extern "C" fn request_delete_header(request: *mut RustRequest, name: *const c_char) {
    let request = unsafe { &mut *request };
    let name = unsafe { str_from_raw_parts(name) };
    request.remove_header(name);
}

#[no_mangle]
pub extern "C" fn request_get_headers<'a>(
    request: *const RustRequest,
) -> *mut RustHeaderIterator<'a> {
    let request = unsafe { &*request };
    let iterator = request.get_header_iterator();
    Box::into_raw(Box::new(iterator))
}

#[no_mangle]
pub extern "C" fn request_get_method(request: *const RustRequest) -> *const c_char {
    let request = unsafe { &*request };
    let method = request.method.as_str();

    CString::new(method).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn request_set_url(request: *mut RustRequest, url: *const c_char) -> bool {
    let request = unsafe { &mut *request };
    let url = unsafe { CStr::from_ptr(url) }.to_str();
    if let Ok(url) = url {
        request.set_url(url);
        true
    } else {
        false
    }
}

#[no_mangle]
pub extern "C" fn request_get_url(request: *mut RustRequest) -> *const c_char {
    let request = unsafe { &mut *request };
    CString::new(request.get_url()).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn request_set_method(request: *mut RustRequest, method: *const c_char) {
    let request = unsafe { &mut *request };
    let method = unsafe { str_from_raw_parts(method) };
    request.set_method(method);
}

#[no_mangle]
pub extern "C" fn request_new() -> *mut RustRequest {
    Box::into_raw(Box::new(RustRequest::new()))
}

#[no_mangle]
pub unsafe extern "C" fn request_delete(request: *mut RustRequest) {
    mem::drop(Box::from_raw(request));
}

#[no_mangle]
pub extern "C" fn request_send<'a>(
    runtime_context: *const RuntimeContext,
    request: *mut RustRequest,
) -> *mut RustRawResponse {
    let request = unsafe { &mut *request };
    let reqwest_request = request.build_reqwest().unwrap();
    let client = reqwest::Client::new();

    let reqwest_response = runtime_context_from_ptr(runtime_context)
        .runtime()
        .block_on(client.execute(reqwest_request))
        .unwrap();
    let raw_response = RustRawResponse::new(reqwest_response);
    Box::into_raw(Box::new(raw_response))
}

