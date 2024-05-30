// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// cspell: words reqwest repr staticlib dylib

pub struct RustByteArray {
    bytes: Vec<u8>,
}

impl RustByteArray {
    pub fn new(bytes: Vec<u8>) -> Self {
        RustByteArray { bytes }
    }

    pub fn as_ptr(&self) -> *const u8 {
        self.bytes.as_ptr()
    }

    pub fn len(&self) -> usize {
        self.bytes.len()
    }
}

#[no_mangle]
pub extern "C" fn byte_array_as_ptr(byte_array: *const RustByteArray) -> *const u8 {
    let byte_array = unsafe { &*byte_array };
    byte_array.as_ptr()
}

#[no_mangle]
pub extern "C" fn byte_array_len(byte_array: *const RustByteArray) -> usize {
    let byte_array = unsafe { &*byte_array };
    byte_array.len()
}

#[no_mangle]
pub extern "C" fn byte_array_delete(byte_array: *mut RustByteArray) {
    unsafe {
        let _ = Box::from_raw(byte_array);
    }
}


