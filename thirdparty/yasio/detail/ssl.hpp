//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2023 HALX99

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

#include "yasio/detail/config.hpp"

#if YASIO_SSL_BACKEND == 1 // OpenSSL
#  include <openssl/bio.h>
#  include <openssl/ssl.h>
#  include <openssl/err.h>
#elif YASIO_SSL_BACKEND == 2 // mbedtls
#  define MBEDTLS_ALLOW_PRIVATE_ACCESS
#  include "mbedtls/net_sockets.h"
#  include "mbedtls/debug.h"
#  include "mbedtls/ssl.h"
#  include "mbedtls/entropy.h"
#  include "mbedtls/ctr_drbg.h"
#  include "mbedtls/error.h"
#  include "mbedtls/version.h"
struct ssl_ctx_st {
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_entropy_context entropy;
  mbedtls_x509_crt cert;
  mbedtls_pk_context pkey;
  mbedtls_ssl_config conf;
};
struct ssl_st : public mbedtls_ssl_context {
  mbedtls_net_context bio;
};
#endif

#if defined(YASIO_SSL_BACKEND)
struct yssl_options {
  char* crtfile_;
  char* keyfile_;
  bool client;
};

YASIO__DECL ssl_ctx_st* yssl_ctx_new(const yssl_options& opts);
YASIO__DECL void yssl_ctx_free(ssl_ctx_st*& ctx);

YASIO__DECL ssl_st* yssl_new(ssl_ctx_st* ctx, int fd, const char* hostname, bool client);
YASIO__DECL void yssl_shutdown(ssl_st*&);

/**
* @returns
*   0: succeed
*   other: use yssl_strerror(ret & YSSL_ERROR_MASK, ...) get error message
*    - err can bb
       - EWOULDBLOCK: status ok, repeat call next time
*      - yasio::errc::ssl_handshake_failed: failed
*/
YASIO__DECL int yssl_do_handshake(ssl_st* ssl, int& err);
YASIO__DECL const char* yssl_strerror(ssl_st* ssl, int sslerr, char* buf, size_t buflen);

YASIO__DECL int yssl_write(ssl_st* ssl, const void* data, size_t len, int& err);
YASIO__DECL int yssl_read(ssl_st* ssl, void* data, size_t len, int& err);
#endif

///////////////////////////////////////////////////////////////////
// --- Implement common yasio ssl api with different ssl backends

#define yasio__valid_str(cstr) (cstr && *cstr)
#define yasio__c_str(str) (!str.empty() ? &str.front() : nullptr)

/* private use for split cert files */
template <typename _Fty>
inline bool yssl_splitpath(char* str, _Fty&& func)
{
  auto _Start  = str; // the start of every string
  auto _Ptr    = str; // source string iterator
  bool aborted = false;
  while ((_Ptr = strchr(_Ptr, ',')))
  {
    if (_Start <= _Ptr)
    {
      if ((aborted = func(_Start, _Ptr)))
        break;
    }
    _Start = _Ptr + 1;
    ++_Ptr;
  }

  if (!aborted)
    aborted = func(_Start, nullptr); // last one
  return aborted;
}

struct yssl_split_term {
  yssl_split_term(char* end)
  {
    if (end) {
      this->val_ = *end;
      *end = '\0';
      this->end_ = end;
    }
  }
  ~yssl_split_term()
  {
    if (this->end_)
      *this->end_ = this->val_;
  }
private:
  char* end_ = nullptr;
  char val_ = '\0';
};

#if YASIO_SSL_BACKEND == 1 // openssl
#  include "yasio/detail/openssl.inl"
#elif YASIO_SSL_BACKEND == 2 // mbedtls
#  include "yasio/detail/mbedtls.inl"
#else
#  error "yasio - Unsupported ssl backend provided!"
#endif

#endif
