// Copyright (c) Microsoft Corp. All Rights Reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// cspell: words reqwest repr staticlib dylib

use std::mem;

use azure_core::{error::ErrorKind, Error, Result};

use crate::rust_error::RustError;
#[derive(Debug)]
pub struct RuntimeContext {
    runtime: tokio::runtime::Runtime,
    error: Option<Error>,
}

impl RuntimeContext {
    pub fn new() -> Result<Self> {
        Ok(Self {
            //            runtime: match tokio::runtime::Builder::new_current_thread()
            runtime: match tokio::runtime::Builder::new_multi_thread()
                .enable_all()
                .build()
                .map_err(|err| Error::new(ErrorKind::Other, err))
            {
                Ok(it) => it,
                Err(err) => return Err(err),
            },
            error: None,
        })
    }

    pub fn set_error(&mut self, error: Error) {
        self.error = Some(error);
    }

    pub fn runtime(&self) -> &tokio::runtime::Runtime {
        &self.runtime
    }
}

#[no_mangle]
pub extern "C" fn runtime_context_new() -> *mut RuntimeContext {
    match RuntimeContext::new() {
        Ok(ctx) => Box::into_raw(Box::new(ctx)),
        Err(_) => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub extern "C" fn runtime_context_get_error(ctx: *const RuntimeContext) -> *mut RustError {
    let ctx = unsafe { &*ctx };
    match &ctx.error {
        Some(err) => {
            let msg = format!("{:?}", err);
            Box::into_raw(Box::new(RustError::new(Error::message(
                err.kind().clone(),
                msg,
            )))).cast()
        }
        None => std::ptr::null::<RustError>().cast_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn runtime_context_delete(ctx: *mut RuntimeContext) {
    mem::drop(Box::from_raw(ctx))
}

pub(crate) fn runtime_context_from_ptr<'a>(ctx: *const RuntimeContext) -> &'a RuntimeContext {
    unsafe { &*ctx }
}
pub(crate) fn runtime_context_from_ptr_mut<'a>(ctx: *mut RuntimeContext) -> &'a mut RuntimeContext {
    unsafe { &mut *ctx }
}
