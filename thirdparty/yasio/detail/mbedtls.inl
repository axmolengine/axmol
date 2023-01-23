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

#ifndef YASIO__MBEDTLS_INL
#define YASIO__MBEDTLS_INL

#if YASIO_SSL_BACKEND == 2

YASIO__DECL ssl_ctx_st* yssl_ctx_new(const yssl_options& opts)
{
  auto ctx = new ssl_ctx_st();
  ::mbedtls_ctr_drbg_init(&ctx->ctr_drbg);
  ::mbedtls_entropy_init(&ctx->entropy);
  ::mbedtls_ssl_config_init(&ctx->conf);
  ::mbedtls_x509_crt_init(&ctx->cert);
  ::mbedtls_pk_init(&ctx->pkey);

  do
  {
    int ret = 0;
    // ssl role
    if ((ret = ::mbedtls_ssl_config_defaults(&ctx->conf, opts.client ? MBEDTLS_SSL_IS_CLIENT : MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM,
                                             MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
      YASIO_LOG("mbedtls_ssl_config_defaults fail with ret=%d", ret);

    // rgn engine
    cxx17::string_view pers = opts.client ? cxx17::string_view{YASIO_SSL_PIN, YASIO_SSL_PIN_LEN} : cxx17::string_view{YASIO_SSL_PON, YASIO_SSL_PON_LEN};
    ret                     = ::mbedtls_ctr_drbg_seed(&ctx->ctr_drbg, ::mbedtls_entropy_func, &ctx->entropy, (const unsigned char*)pers.data(), pers.length());
    if (ret != 0)
    {
      YASIO_LOG("mbedtls_ctr_drbg_seed fail with ret=%d", ret);
      break;
    }
    ::mbedtls_ssl_conf_rng(&ctx->conf, ::mbedtls_ctr_drbg_random, &ctx->ctr_drbg);

    // --- load cert
    int authmode = MBEDTLS_SSL_VERIFY_OPTIONAL;
    if (yasio__valid_str(opts.crtfile_)) // the cafile_ must be full path
    {
      int fail_count = 0;
      yssl_splitpath(opts.crtfile_, [&](char* first, char* last) {
        yssl_split_term null_term(last);

        if ((ret = ::mbedtls_x509_crt_parse_file(&ctx->cert, first)) != 0)
        {
          ++fail_count;
          YASIO_LOG("mbedtls_x509_crt_parse_file with ret=-0x%x", (unsigned int)-ret);
        }

        return !!ret;
      });
      if (!fail_count)
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
    }

    if (opts.client)
    {
      ::mbedtls_ssl_conf_authmode(&ctx->conf, authmode);
      ::mbedtls_ssl_conf_ca_chain(&ctx->conf, &ctx->cert, nullptr);
    }
    else
    {
      // --- load server private key
      if (yasio__valid_str(opts.keyfile_))
      {
#  if MBEDTLS_VERSION_MAJOR >= 3
        if (::mbedtls_pk_parse_keyfile(&ctx->pkey, opts.keyfile_, nullptr, mbedtls_ctr_drbg_random, &ctx->ctr_drbg) != 0)
#  else
        if (::mbedtls_pk_parse_keyfile(&ctx->pkey, opts.keyfile_, nullptr) != 0)
#  endif
        {
          YASIO_LOG("mbedtls_x509_crt_parse_file with ret=-0x%x", (unsigned int)-ret);
          break;
        }
      }
      ::mbedtls_ssl_conf_ca_chain(&ctx->conf, ctx->cert.next, nullptr);
      if ((ret = mbedtls_ssl_conf_own_cert(&ctx->conf, &ctx->cert, &ctx->pkey)) != 0)
      {
        YASIO_LOG("mbedtls_ssl_conf_own_cert with ret=-0x%x", (unsigned int)-ret);
        break;
      }
    }

  } while (false);

  return ctx;
}

YASIO__DECL void yssl_ctx_free(ssl_ctx_st*& ctx)
{
  if (!ctx)
    return;
  ::mbedtls_pk_free(&ctx->pkey);
  ::mbedtls_x509_crt_free(&ctx->cert);
  ::mbedtls_ssl_config_free(&ctx->conf);
  ::mbedtls_entropy_free(&ctx->entropy);
  ::mbedtls_ctr_drbg_free(&ctx->ctr_drbg);

  delete ctx;
  ctx = nullptr;
}

YASIO__DECL ssl_st* yssl_new(ssl_ctx_st* ctx, int fd, const char* hostname, bool client)
{
  auto ssl = new ssl_st();
  ::mbedtls_ssl_init(ssl);
  ::mbedtls_ssl_setup(ssl, &ctx->conf);

  // ssl_set_fd
  ssl->bio.fd = fd;
  ::mbedtls_ssl_set_bio(ssl, &ssl->bio, ::mbedtls_net_send, ::mbedtls_net_recv, nullptr /*  rev_timeout() */);
  if (client)
    ::mbedtls_ssl_set_hostname(ssl, hostname);
  return ssl;
}
YASIO__DECL void yssl_shutdown(ssl_st*& ssl)
{
  ::mbedtls_ssl_close_notify(ssl);
  ::mbedtls_ssl_free(ssl);
  delete ssl;
  ssl = nullptr;
}
YASIO__DECL int yssl_do_handshake(ssl_st* ssl, int& err)
{
  int ret = ::mbedtls_ssl_handshake_step(ssl);
  switch (ret)
  {
    case 0:
      if (ssl->state == MBEDTLS_SSL_HANDSHAKE_OVER)
        return 0;
      err = EWOULDBLOCK;
      ret = -1;
      break;
    case MBEDTLS_ERR_SSL_WANT_READ:
    case MBEDTLS_ERR_SSL_WANT_WRITE:
      err = EWOULDBLOCK;
      break; // Nothing need to do
    default:
      err = yasio::errc::ssl_handshake_failed;
  }
  return ret;
}
const char* yssl_strerror(ssl_st* ssl, int sslerr, char* buf, size_t buflen)
{
  int n = snprintf(buf, buflen, "error:%d:", sslerr);
  ::mbedtls_strerror(sslerr, buf + n, buflen - n);
  return buf;
}
YASIO__DECL int yssl_write(ssl_st* ssl, const void* data, size_t len, int& err)
{
  int n = ::mbedtls_ssl_write(ssl, static_cast<const uint8_t*>(data), len);
  if (n > 0)
    return n;
  switch (n)
  {
    case MBEDTLS_ERR_SSL_WANT_READ:
    case MBEDTLS_ERR_SSL_WANT_WRITE:
      err = EWOULDBLOCK;
      break;
    default:
      err = yasio::errc::ssl_write_failed;
  }
  return -1;
}
YASIO__DECL int yssl_read(ssl_st* ssl, void* data, size_t len, int& err)
{
  int n = ::mbedtls_ssl_read(ssl, static_cast<uint8_t*>(data), len);
  if (n > 0)
    return n;
  switch (n)
  {
    case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: // n=0, the upper caller will regards as eof
      n = 0;
    case 0:
      err = yasio::xxsocket::get_last_errno();
      ::mbedtls_ssl_close_notify(ssl);
      break;
    case MBEDTLS_ERR_SSL_WANT_READ:
    case MBEDTLS_ERR_SSL_WANT_WRITE:
      err = EWOULDBLOCK;
      break;
    default:
      err = yasio::errc::ssl_read_failed;
  }
  return n;
}

#endif

#endif
