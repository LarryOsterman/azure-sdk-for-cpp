// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// cspell: words reqwest repr staticlib dylib

use std::mem;
use std::ffi::CString;

pub struct RustHttpHeader {
    name: String,
    value: String,
}

impl RustHttpHeader {
    pub(crate) fn new(name: impl Into<String>, value: impl Into<String>) -> Self {
        RustHttpHeader {
            name: name.into(),
            value: value.into(),
        }
    }

    fn get_name(&self) -> &[u8] {
        &self.name.as_bytes()
    }
    fn get_value(&self) -> &[u8] {
        &self.value.as_bytes()
    }
}

#[no_mangle]
pub extern "C" fn rust_string_delete(rust_string: *mut i8) {
    unsafe {
        mem::drop(CString::from_raw(rust_string));
    }
}

#[no_mangle]
pub extern "C" fn http_header_get_name(header: *const RustHttpHeader) -> *mut i8 {
    unsafe {
        let header = &*header;
        let name = header.get_name();
        let c_str = CString::new(name).unwrap();
        c_str.into_raw()
    }
}

#[no_mangle]
pub extern "C" fn http_header_get_value(header: *const RustHttpHeader) -> *mut i8 {
    unsafe {
        let header = &*header;
        let value = header.get_value();
        let c_str = CString::new(value).unwrap();
        c_str.into_raw()
    }
}

#[no_mangle]
pub extern "C" fn http_header_delete(header: *mut RustHttpHeader) {
    unsafe {
        mem::drop(Box::from_raw(header));
    }
}
