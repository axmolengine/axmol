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
#ifndef YASIO__INET_COMPAT_INL
#define YASIO__INET_COMPAT_INL

// !!!Don't include this file directly, it's used for internal.

// from glibc
#ifdef SPRINTF_CHAR
#  define SPRINTF(x) strlen(sprintf /**/ x)
#else
#  ifndef SPRINTF
#    define SPRINTF(x) (/*(size_t)*/ sprintf x)
#  endif
#endif

/*
 * Define constants based on RFC 883, RFC 1034, RFC 1035
 */
#define NS_PACKETSZ 512   /*%< default UDP packet size */
#define NS_MAXDNAME 1025  /*%< maximum domain name */
#define NS_MAXMSG 65535   /*%< maximum message size */
#define NS_MAXCDNAME 255  /*%< maximum compressed domain name */
#define NS_MAXLABEL 63    /*%< maximum length of domain label */
#define NS_HFIXEDSZ 12    /*%< #/bytes of fixed data in header */
#define NS_QFIXEDSZ 4     /*%< #/bytes of fixed data in query */
#define NS_RRFIXEDSZ 10   /*%< #/bytes of fixed data in r record */
#define NS_INT32SZ 4      /*%< #/bytes of data in a u_int32_t */
#define NS_INT16SZ 2      /*%< #/bytes of data in a u_int16_t */
#define NS_INT8SZ 1       /*%< #/bytes of data in a u_int8_t */
#define NS_INADDRSZ 4     /*%< IPv4 T_A */
#define NS_IN6ADDRSZ 16   /*%< IPv6 T_AAAA */
#define NS_CMPRSFLGS 0xc0 /*%< Flag bits indicating name compression. */
#define NS_DEFAULTPORT 53 /*%< For both TCP and UDP. */

/////////////////// inet_ntop //////////////////
/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */

static const char* inet_ntop4(const u_char* src, char* dst, size_t size);
static const char* inet_ntop6(const u_char* src, char* dst, size_t size);

/* char *
 * inet_ntop(af, src, dst, size)
 *	convert a network format address to presentation format.
 * return:
 *	pointer to presentation format address (`dst'), or nullptr (see errno).
 * author:
 *	Paul Vixie, 1996.
 */
const char* inet_ntop(int af, const void* src, char* dst, size_t size)
{
  switch (af)
  {
    case AF_INET:
      return (inet_ntop4((const u_char*)src, dst, size));
    case AF_INET6:
      return (inet_ntop6((const u_char*)src, dst, size));
    default:
      errno = EAFNOSUPPORT;
      return (nullptr);
  }
  /* NOTREACHED */
}

/* const char *
 * inet_ntop4(src, dst, size)
 *	format an IPv4 address
 * return:
 *	`dst' (as a const)
 * notes:
 *	(1) uses no statics
 *	(2) takes a u_char* not an in_addr as input
 * author:
 *	Paul Vixie, 1996.
 */
static const char* inet_ntop4(const u_char* src, char* dst, size_t size)
{
  char fmt[] = "%u.%u.%u.%u";
  char tmp[sizeof "255.255.255.255"];

  if (SPRINTF((tmp, fmt, src[0], src[1], src[2], src[3])) >= static_cast<int>(size))
  {
    errno = (ENOSPC);
    return (nullptr);
  }
  return strcpy(dst, tmp);
}

/* const char *
 * inet_ntop6(src, dst, size)
 *	convert IPv6 binary address into presentation (printable) format
 * author:
 *	Paul Vixie, 1996.
 */
static const char* inet_ntop6(const u_char* src, char* dst, size_t size)
{
  /*
   * Note that int32_t and int16_t need only be "at least" large enough
   * to contain a value of the specified size.  On some systems, like
   * Crays, there is no such thing as an integer variable with 16 bits.
   * Keep this in mind if you think this function should have been coded
   * to use pointer overlays.  All the world's not a VAX.
   */
  char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
  struct {
    int base, len;
  } best, cur;
  u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
  int i;

  /*
   * Preprocess:
   *	Copy the input (bytewise) array into a wordwise array.
   *	Find the longest run of 0x00's in src[] for :: shorthanding.
   */
  memset(words, '\0', sizeof words);
  for (i = 0; i < NS_IN6ADDRSZ; i += 2)
    words[i / 2] = (src[i] << 8) | src[i + 1];
  best.base = -1;
  cur.base  = -1;
  best.len  = 0;
  cur.len   = 0;
  for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++)
  {
    if (words[i] == 0)
    {
      if (cur.base == -1)
      {
        cur.base = i;
        cur.len  = 1;
      }
      else
        cur.len++;
    }
    else
    {
      if (cur.base != -1)
      {
        if (best.base == -1 || cur.len > best.len)
          best = cur;
        cur.base = -1;
      }
    }
  }
  if (cur.base != -1)
  {
    if (best.base == -1 || cur.len > best.len)
      best = cur;
  }
  if (best.base != -1 && best.len < 2)
    best.base = -1;

  /*
   * Format the result.
   */
  tp = tmp;
  for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++)
  {
    /* Are we inside the best run of 0x00's? */
    if (best.base != -1 && i >= best.base && i < (best.base + best.len))
    {
      if (i == best.base)
        *tp++ = ':';
      continue;
    }
    /* Are we following an initial run of 0x00s or any real hex? */
    if (i != 0)
      *tp++ = ':';
    /* Is this address an encapsulated IPv4? */
    if (i == 6 && best.base == 0 && (best.len == 6 || (best.len == 5 && words[5] == 0xffff)))
    {
      if (!inet_ntop4(src + 12, tp, static_cast<size_t>(sizeof tmp - (tp - tmp))))
        return (nullptr);
      tp += strlen(tp);
      break;
    }
    tp += SPRINTF((tp, "%x", words[i]));
  }
  /* Was it a trailing run of 0x00's? */
  if (best.base != -1 && (best.base + best.len) == (NS_IN6ADDRSZ / NS_INT16SZ))
    *tp++ = ':';
  *tp++ = '\0';

  /*
   * Check for overflow, copy, and we're done.
   */
  if ((size_t)(tp - tmp) > size)
  {
    errno = (ENOSPC);
    return (nullptr);
  }
  return strcpy(dst, tmp);
}

/////////////////// inet_pton ///////////////////

/*
 * WARNING: Don't even consider trying to compile this on a system where
 * sizeof(int) < 4.  sizeof(int) > 4 is fine; all the world's not a VAX.
 */
static int inet_pton4(const char* src, u_char* dst);
static int inet_pton6(const char* src, u_char* dst);

/* int
 * inet_pton(af, src, dst)
 *	convert from presentation format (which usually means ASCII printable)
 *	to network format (which is usually some kind of binary format).
 * return:
 *	1 if the address was valid for the specified address family
 *	0 if the address wasn't valid (`dst' is untouched in this case)
 *	-1 if some other error occurred (`dst' is untouched in this case, too)
 * author:
 *	Paul Vixie, 1996.
 */
int inet_pton(int af, const char* src, void* dst)
{
  switch (af)
  {
    case AF_INET:
      return (inet_pton4(src, (u_char*)dst));
    case AF_INET6:
      return (inet_pton6(src, (u_char*)dst));
    default:
      errno = (EAFNOSUPPORT);
      return (-1);
  }
  /* NOTREACHED */
}

/* int
 * inet_pton4(src, dst)
 *	like inet_aton() but without all the hexadecimal, octal (with the
 *	exception of 0) and shorthand.
 * return:
 *	1 if `src' is a valid dotted quad, else 0.
 * notice:
 *	does not touch `dst' unless it's returning 1.
 * author:
 *	Paul Vixie, 1996.
 */
static int inet_pton4(const char* src, u_char* dst)
{
  int saw_digit, octets, ch;
  u_char tmp[NS_INADDRSZ], *tp;

  saw_digit   = 0;
  octets      = 0;
  *(tp = tmp) = 0;
  while ((ch = *src++) != '\0')
  {

    if (ch >= '0' && ch <= '9')
    {
      u_int newv = *tp * 10 + (ch - '0');

      if (saw_digit && *tp == 0)
        return (0);
      if (newv > 255)
        return (0);
      *tp = static_cast<u_char>(newv);
      if (!saw_digit)
      {
        if (++octets > 4)
          return (0);
        saw_digit = 1;
      }
    }
    else if (ch == '.' && saw_digit)
    {
      if (octets == 4)
        return (0);
      *++tp     = 0;
      saw_digit = 0;
    }
    else
      return (0);
  }
  if (octets < 4)
    return (0);
  memcpy(dst, tmp, NS_INADDRSZ);
  return (1);
}

/* int
 * inet_pton6(src, dst)
 *	convert presentation level address to network order binary form.
 * return:
 *	1 if `src' is a valid [RFC1884 2.2] address, else 0.
 * notice:
 *	(1) does not touch `dst' unless it's returning 1.
 *	(2) :: in a full address is silently ignored.
 * credit:
 *	inspired by Mark Andrews.
 * author:
 *	Paul Vixie, 1996.
 */
static int inet_pton6(const char* src, u_char* dst)
{
  static const char xdigits[] = "0123456789abcdef";
  u_char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
  const char* curtok;
  int ch, saw_xdigit;
  u_int val;

  tp     = (u_char*)memset(tmp, '\0', NS_IN6ADDRSZ);
  endp   = tp + NS_IN6ADDRSZ;
  colonp = nullptr;
  /* Leading :: requires some special handling. */
  if (*src == ':')
    if (*++src != ':')
      return (0);
  curtok     = src;
  saw_xdigit = 0;
  val        = 0;
  while ((ch = tolower(*src++)) != '\0')
  {
    const char* pch;

    pch = strchr(xdigits, ch);
    if (pch != nullptr)
    {
      val <<= 4;
      val |= (pch - xdigits);
      if (val > 0xffff)
        return (0);
      saw_xdigit = 1;
      continue;
    }
    if (ch == ':')
    {
      curtok = src;
      if (!saw_xdigit)
      {
        if (colonp)
          return (0);
        colonp = tp;
        continue;
      }
      else if (*src == '\0')
      {
        return (0);
      }
      if (tp + NS_INT16SZ > endp)
        return (0);
      *tp++      = (u_char)(val >> 8) & 0xff;
      *tp++      = (u_char)val & 0xff;
      saw_xdigit = 0;
      val        = 0;
      continue;
    }
    if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) && inet_pton4(curtok, tp) > 0)
    {
      tp += NS_INADDRSZ;
      saw_xdigit = 0;
      break; /* '\0' was seen by inet_pton4(). */
    }
    return (0);
  }
  if (saw_xdigit)
  {
    if (tp + NS_INT16SZ > endp)
      return (0);
    *tp++ = (u_char)(val >> 8) & 0xff;
    *tp++ = (u_char)val & 0xff;
  }
  if (colonp != nullptr)
  {
    /*
     * Since some memmove()'s erroneously fail to handle
     * overlapping regions, we'll do the shift by hand.
     */
    const auto n = tp - colonp;
    int i;

    if (tp == endp)
      return (0);
    for (i = 1; i <= n; i++)
    {
      endp[-i]      = colonp[n - i];
      colonp[n - i] = 0;
    }
    tp = endp;
  }
  if (tp != endp)
    return (0);
  memcpy(dst, tmp, NS_IN6ADDRSZ);
  return (1);
}
#endif
