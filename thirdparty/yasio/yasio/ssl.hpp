//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2024 HALX99

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef YASIO__SSL_HPP
#define YASIO__SSL_HPP

#include "yasio/config.hpp"

#if YASIO_SSL_BACKEND == 1 // OpenSSL
#  include <openssl/bio.h>
#  include <openssl/ssl.h>
#  include <openssl/err.h>
typedef struct ssl_ctx_st yssl_ctx_st;
struct yssl_st {
  ssl_st* session;
  int fd;
  BIO_METHOD* bmth;
};
#  define yssl_unwrap(ssl) ssl->session

#elif YASIO_SSL_BACKEND == 2 // mbedtls
#  define MBEDTLS_ALLOW_PRIVATE_ACCESS
#  include "mbedtls/net_sockets.h"
#  include "mbedtls/debug.h"
#  include "mbedtls/ssl.h"
#  include "mbedtls/entropy.h"
#  include "mbedtls/ctr_drbg.h"
#  include "mbedtls/error.h"
#  include "mbedtls/version.h"
typedef struct ssl_ctx_st {
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_entropy_context entropy;
  mbedtls_x509_crt cert;
  mbedtls_pk_context pkey;
  mbedtls_ssl_config conf;
} yssl_ctx_st;
struct yssl_st : public mbedtls_ssl_context {
  mbedtls_net_context bio;
};
#endif

#if defined(YASIO_SSL_BACKEND)
struct yssl_options {
  char* crtfile_;
  char* keyfile_;
  bool client;
};

YASIO__DECL yssl_ctx_st* yssl_ctx_new(const yssl_options& opts);
YASIO__DECL void yssl_ctx_free(yssl_ctx_st*& ctx);

YASIO__DECL yssl_st* yssl_new(yssl_ctx_st* ctx, int fd, const char* hostname, bool client);
YASIO__DECL void yssl_shutdown(yssl_st*&, bool writable);

/**
* @returns
*   0: succeed
*   other: use yssl_strerror(ret & YSSL_ERROR_MASK, ...) get error message
*    - err can bb
       - EWOULDBLOCK: status ok, repeat call next time
*      - yasio::errc::ssl_handshake_failed: failed
*/
YASIO__DECL int yssl_do_handshake(yssl_st* ssl, int& err);
YASIO__DECL const char* yssl_strerror(yssl_st* ssl, int sslerr, char* buf, size_t buflen);

YASIO__DECL int yssl_write(yssl_st* ssl, const void* data, size_t len, int& err);
YASIO__DECL int yssl_read(yssl_st* ssl, void* data, size_t len, int& err);
#endif

///////////////////////////////////////////////////////////////////
// --- Implement common yasio ssl api with different ssl backends

#define yasio__valid_str(cstr) (cstr && *cstr)
#define yasio__c_str(str) (!str.empty() ? &str.front() : nullptr)

#if YASIO_SSL_BACKEND == 1 // openssl
#  include "yasio/impl/openssl.hpp"
#elif YASIO_SSL_BACKEND == 2 // mbedtls
#  include "yasio/impl/mbedtls.hpp"
#else
#  error "yasio - Unsupported ssl backend provided!"
#endif

#endif
