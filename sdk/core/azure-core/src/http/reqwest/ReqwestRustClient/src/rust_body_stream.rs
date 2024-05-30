// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*
cspell: words reqwest repr staticlib dylib brotli gzip
*/

use core::pin::pin;

use crate::{
    byte_array::RustByteArray,
    runtime_context::{runtime_context_from_ptr_mut, RuntimeContext},
};
use azure_core::Error;
use bytes::Bytes;
use futures::StreamExt;
use futures_core::Stream;
use log::trace;

pub struct RustBodyStream {
    stream: Box<dyn Stream<Item = Result<Bytes, Error>> + Unpin>,
}

impl<'a> Drop for RustBodyStream {
    fn drop(&mut self) {
        println!("Dropping RustBodyStream");
    }
}

impl<'a> RustBodyStream {
    pub fn new(stream: impl Stream<Item = Result<Bytes, Error>> + Unpin + 'static) -> Self {
        RustBodyStream {
            stream: Box::new(stream),
        }
    }

    pub async fn read_stream_data(&mut self) -> Result<Bytes, Error> {
        match pin!(self.stream.next()).await {
            Some(Ok(bytes)) => Ok(bytes),
            Some(Err(err)) => Err(err),
            None => Ok(Bytes::new()),
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_body_stream_read_bytes(
    stream: *mut RustBodyStream,
    runtime_context: *mut RuntimeContext,
) -> *mut RustByteArray {
    let runtime_context = runtime_context_from_ptr_mut(runtime_context);
    trace!("RustBodyStream ReadBytes. Context: {:?}", runtime_context);
    let stream = unsafe { &mut *stream };

    let data = runtime_context
        .runtime()
        .block_on(stream.read_stream_data());
    match data {
        Ok(data) => {
            let data = Box::new(RustByteArray::new(data.to_vec()));
            Box::into_raw(data)
        }
        Err(err) => {
            println!("Error reading stream: {:?}", err);
            // Tunnel the error to the C code via the runtime_context.
            runtime_context.set_error(err);
            std::ptr::null_mut()
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_body_stream_delete(stream: *mut RustBodyStream) {
    trace!("Deleting RustBodyStream");
    unsafe {
        let _ = Box::from_raw(stream);
    }
}
