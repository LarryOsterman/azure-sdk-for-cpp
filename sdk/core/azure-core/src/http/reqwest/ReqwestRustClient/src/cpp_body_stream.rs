// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
cspell: words reqwest repr staticlib dylib brotli gzip nonoverlapping ctypes
*/

use futures_core::Stream;
use std::cell::UnsafeCell;
use std::marker::PhantomPinned;

/// Marker type for an Azure::Core::IO::BodyStream type. Treated as an opaque type by Rust.
pub struct CppStream {
    _data: UnsafeCell<[u8; 0]>, // Opt out of the immutability guarantee - a reference to CppStream may point to data that is being mutated.
    _marker: PhantomPinned, // Tag that this type can never be moved.
}

/// Rust wrapper around a C++ stream that implements the `Stream` trait.
#[derive(Debug, Copy, Clone)]
pub struct CppBodyStream(pub *mut CppStream);

unsafe impl Send for CppBodyStream {}
unsafe impl Sync for CppBodyStream {}

extern "C" {

    /// Get the next chunk of bytes from the stream.
    /// Wraps the Azure::Core::IO::BodyStream::Read method.
    #[allow(improper_ctypes)]
    pub fn stream_get_body_bytes(stream: *mut CppStream, buffer: *mut u8, max_size: usize)
        -> usize;
}

const STREAM_READ_SIZE: usize = 1024;
impl Stream for CppBodyStream {
    type Item = Result<Vec<u8>, azure_core::Error>;

    fn poll_next(
        self: std::pin::Pin<&mut Self>,
        _cx: &mut std::task::Context<'_>,
    ) -> std::task::Poll<Option<Self::Item>> {
        let mut data = Vec::with_capacity(STREAM_READ_SIZE);
        let bytes_read =
            unsafe { stream_get_body_bytes(self.0, data.as_mut_ptr(), STREAM_READ_SIZE) };
        if bytes_read == 0 {
            return std::task::Poll::Ready(None);
        }
        unsafe {
            data.set_len(bytes_read);
        }

        std::task::Poll::Ready(Some(Ok(data)))
    }
}
