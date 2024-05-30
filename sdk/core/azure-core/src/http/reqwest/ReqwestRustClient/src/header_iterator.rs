// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// cspell: words reqwest repr staticlib dylib

use reqwest::header::{HeaderValue, HeaderMap};
use std::mem;

use crate::http_header::RustHttpHeader;

pub struct RustHeaderIterator<'a> {
    iterator: reqwest::header::Iter<'a, HeaderValue>,
}

impl<'a> RustHeaderIterator<'a> {
    pub(crate) fn new(headers: &'a HeaderMap) -> Self {
        RustHeaderIterator {
            iterator: headers.iter(),
        }
    }

    pub(crate) fn get_next(&mut self) -> Option<(&str, &str)> {
        self.iterator
            .next()
            .map(|(header, value)| (header.as_str(), value.to_str().unwrap()))
    }
}

#[no_mangle]
pub extern "C" fn header_iterator_next<'a>(iterator: *mut RustHeaderIterator<'a>) -> *mut RustHttpHeader {
    let iterator = unsafe { &mut *iterator };
    let (h, v) = iterator
        .get_next()
        .map(|(header, value)| (header.to_string(), value.to_string()))
        .unwrap_or((String::new(), String::new()));

    if h.is_empty() && v.is_empty() {
        return std::ptr::null_mut();
    }
    Box::into_raw(Box::new(RustHttpHeader::new(h, v)))
}

#[no_mangle]
pub extern "C" fn header_iterator_delete<'a>(iterator: *mut RustHeaderIterator<'a>) {
    unsafe {
        mem::drop(Box::from_raw(iterator));
    }
}
