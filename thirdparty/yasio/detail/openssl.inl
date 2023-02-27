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

#ifndef YASIO__OPENSSL_INL
#define YASIO__OPENSSL_INL

#if YASIO_SSL_BACKEND == 1 // OpenSSL

// The ssl error mask (1 << 31), a little hack, but works
#  define YSSL_ERR_MASK 0x80000000

YASIO__DECL ssl_ctx_st* yssl_ctx_new(const yssl_options& opts)
{
  auto ctx = ::SSL_CTX_new(opts.client ? ::SSLv23_client_method() : SSLv23_server_method());

  auto mode = SSL_CTX_get_mode(ctx);
#  if defined(SSL_MODE_RELEASE_BUFFERS)
  mode |= SSL_MODE_RELEASE_BUFFERS;
#  endif

  ::SSL_CTX_set_mode(ctx, SSL_MODE_ENABLE_PARTIAL_WRITE | mode);

  if (opts.client)
  {
    int fail_count = -1;
    if (yasio__valid_str(opts.crtfile_))
    {
      fail_count = 0;
      yssl_splitpath(opts.crtfile_, [&](char* first, char* last) {
        yssl_split_term null_term(last);

#  if defined(OPENSSL_VERSION_MAJOR) && (OPENSSL_VERSION_MAJOR >= 3)
        /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
        bool ok = ::SSL_CTX_load_verify_file(ctx, first) == 1;
#  else
        bool ok = ::SSL_CTX_load_verify_locations(ctx, first, nullptr) == 1;
#  endif
        if (!ok)
        {
          ++fail_count;
          YASIO_LOG("[global] load ca certifaction file failed!");
        }

        return !ok;
      });
    }
    if (!fail_count)
    {
      ::SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, ::SSL_CTX_get_verify_callback(ctx));
#  if OPENSSL_VERSION_NUMBER >= 0x10101000L
      ::SSL_CTX_set_post_handshake_auth(ctx, 1);
#  endif
#  if defined(X509_V_FLAG_PARTIAL_CHAIN)
      /* Have intermediate certificates in the trust store be treated as
        trust-anchors, in the same way as self-signed root CA certificates
        are. This allows users to verify servers using the intermediate cert
        only, instead of needing the whole chain. */
      ::X509_STORE_set_flags(::SSL_CTX_get_cert_store(ctx), X509_V_FLAG_PARTIAL_CHAIN);
#  endif
    }
    else
      ::SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);
  }
  else
  {
    if (yasio__valid_str(opts.crtfile_) && ::SSL_CTX_use_certificate_file(ctx, opts.crtfile_, SSL_FILETYPE_PEM) <= 0)
      YASIO_LOG("[gobal] load server cert file failed!");

    if (yasio__valid_str(opts.keyfile_) && ::SSL_CTX_use_PrivateKey_file(ctx, opts.keyfile_, SSL_FILETYPE_PEM) <= 0)
      YASIO_LOG("[gobal] load server private key file failed!");
  }

  return ctx;
}

YASIO__DECL void yssl_ctx_free(ssl_ctx_st*& ctx)
{
  ::SSL_CTX_free((SSL_CTX*)ctx);
  ctx = nullptr;
}
YASIO__DECL ssl_st* yssl_new(ssl_ctx_st* ctx, int fd, const char* hostname, bool client)
{
  auto ssl = ::SSL_new(ctx);
  ::SSL_set_fd(ssl, fd);
  if (client)
  {
    ::SSL_set_connect_state(ssl);
    ::SSL_set_tlsext_host_name(ssl, hostname);
  }
  else
    ::SSL_set_accept_state(ssl);
  return ssl;
}
YASIO__DECL void yssl_shutdown(ssl_st*& ssl)
{
  ::SSL_shutdown(ssl);
  ::SSL_free(ssl);
  ssl = nullptr;
}
YASIO__DECL int yssl_do_handshake(ssl_st* ssl, int& err)
{
  ERR_clear_error();
  int ret = ::SSL_do_handshake(ssl);
  if (ret == 1) // handshake succeed
    return 0;

  int sslerr = ::SSL_get_error(ssl, ret);
  /*
  When using a non-blocking socket, nothing is to be done, but select() can be used to check for
  the required condition: https://www.openssl.org/docs/manmaster/man3/SSL_do_handshake.html
  */
  if (sslerr == SSL_ERROR_WANT_READ || sslerr == SSL_ERROR_WANT_WRITE)
  {
    err = EWOULDBLOCK;
    return -1;
  }

  /* ssl handshake fail */
  err = yasio::errc::ssl_handshake_failed; // emit ssl handshake failed, continue handle close flow
  return (sslerr != SSL_ERROR_SYSCALL) ? static_cast<int>(ERR_get_error() | YSSL_ERR_MASK) : yasio::xxsocket::get_last_errno();
}
YASIO__DECL const char* yssl_strerror(ssl_st* ssl, int sslerr, char* buf, size_t buflen)
{
  if (yasio__testbits(sslerr, YSSL_ERR_MASK))
    ::ERR_error_string_n((unsigned int)sslerr & ~YSSL_ERR_MASK, buf, buflen);
  else
    yasio::xxsocket::strerror_r(sslerr, buf, buflen);
  return buf;
}
YASIO__DECL int yssl_write(ssl_st* ssl, const void* data, size_t len, int& err)
{
  ERR_clear_error();
  int n = ::SSL_write(ssl, data, static_cast<int>(len));
  if (n > 0)
    return n;

  int sslerr = ::SSL_get_error(ssl, n);
  if (sslerr == SSL_ERROR_ZERO_RETURN)
  { // SSL_ERROR_SYSCALL
    err = yasio::xxsocket::get_last_errno();
    return 0;
  }
  switch (sslerr)
  {
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
      err = EWOULDBLOCK;
      break;
    default:
      err = yasio::errc::ssl_write_failed;
  }
  return -1;
}
YASIO__DECL int yssl_read(ssl_st* ssl, void* data, size_t len, int& err)
{
  ERR_clear_error();
  int n = ::SSL_read(ssl, data, static_cast<int>(len));
  if (n > 0)
    return n;
  int sslerr = ::SSL_get_error(ssl, n);
  switch (sslerr)
  {
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
      /* The operation did not complete; the same TLS/SSL I/O function
          should be called again later. This is basically an EWOULDBLOCK
          equivalent. */
      err = EWOULDBLOCK;
      break;
    default:
      err = sslerr == SSL_ERROR_SYSCALL ? yasio::xxsocket::get_last_errno() : yasio::errc::ssl_read_failed;
      if (sslerr == SSL_ERROR_ZERO_RETURN || ERR_peek_error() == 0) // peer closed connection in SSL handshake
        return 0;
  }
  return -1;
}
#endif

#endif
