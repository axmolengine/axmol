//////////////////////////////////////////////////////////////////////////////////////////
// The embedded xml SAX parser, extract from pugixml DOM parser
// please see: https://github.com/zeux/pugixml
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)
Copyright (c) 2019 halx99
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
#ifndef SIMDSOFT__XSXML_HPP
#define SIMDSOFT__XSXML_HPP
#pragma once
#include <functional>
#include <string>
#include <assert.h>

#define XSXML__DECL inline

namespace xsxml
{

typedef char char_t;

// Parsing status, returned as part of xml_parse_result object
enum xml_parse_status
{
  status_ok = 0, // No error

  status_file_not_found, // File was not found during load_file()
  status_io_error,       // Error reading from file/stream
  status_out_of_memory,  // Could not allocate memory
  status_internal_error, // Internal error occurred

  status_unrecognized_tag, // Parser could not determine tag type

  status_bad_pi, // Parsing error occurred while parsing document declaration/processing instruction
  status_bad_comment,          // Parsing error occurred while parsing comment
  status_bad_cdata,            // Parsing error occurred while parsing CDATA section
  status_bad_doctype,          // Parsing error occurred while parsing document type declaration
  status_bad_pcdata,           // Parsing error occurred while parsing PCDATA section
  status_bad_start_element,    // Parsing error occurred while parsing start element tag
  status_bad_attribute,        // Parsing error occurred while parsing element attribute
  status_bad_end_element,      // Parsing error occurred while parsing end element tag
  status_end_element_mismatch, // There was a mismatch of start-end tags (closing tag had incorrect
                               // name, some tag was not closed or there was an excessive closing
                               // tag)

  status_append_invalid_root, // Unable to append nodes since root type is not node_element or
                              // node_document (exclusive to xml_node::append_buffer)

  status_no_document_element // Parsing resulted in a document without element nodes
};

// Parsing options

// Minimal parsing mode (equivalent to turning all other flags off).
// Only elements and PCDATA sections are added to the DOM tree, no text conversions are performed.
const unsigned int parse_minimal = 0x0000;

// This flag determines if processing instructions (node_pi) are added to the DOM tree. This flag is
// off by default.
const unsigned int parse_pi = 0x0001;

// This flag determines if comments (node_comment) are added to the DOM tree. This flag is off by
// default.
const unsigned int parse_comments = 0x0002;

// This flag determines if CDATA sections (node_cdata) are added to the DOM tree. This flag is on by
// default.
const unsigned int parse_cdata = 0x0004;

// This flag determines if plain character data (node_pcdata) that consist only of whitespace are
// added to the DOM tree. This flag is off by default; turning it on usually results in slower
// parsing and more memory consumption.
const unsigned int parse_ws_pcdata = 0x0008;

// This flag determines if character and entity references are expanded during parsing. This flag is
// on by default.
const unsigned int parse_escapes = 0x0010;

// This flag determines if EOL characters are normalized (converted to #xA) during parsing. This
// flag is on by default.
const unsigned int parse_eol = 0x0020;

// This flag determines if attribute values are normalized using CDATA normalization rules during
// parsing. This flag is on by default.
const unsigned int parse_wconv_attribute = 0x0040;

// This flag determines if attribute values are normalized using NMTOKENS normalization rules during
// parsing. This flag is off by default.
const unsigned int parse_wnorm_attribute = 0x0080;

// This flag determines if document declaration (node_declaration) is added to the DOM tree. This
// flag is off by default.
const unsigned int parse_declaration = 0x0100;

// This flag determines if document type declaration (node_doctype) is added to the DOM tree. This
// flag is off by default.
const unsigned int parse_doctype = 0x0200;

// This flag determines if plain character data (node_pcdata) that is the only child of the parent
// node and that consists only of whitespace is added to the DOM tree. This flag is off by default;
// turning it on may result in slower parsing and more memory consumption.
const unsigned int parse_ws_pcdata_single = 0x0400;

// This flag determines if leading and trailing whitespace is to be removed from plain character
// data. This flag is off by default.
const unsigned int parse_trim_pcdata = 0x0800;

// This flag determines if plain character data that does not have a parent node is added to the DOM
// tree, and if an empty document is a valid document. This flag is off by default.
const unsigned int parse_fragment = 0x1000;

// This flag determines if plain character data is be stored in the parent element's value. This
// significantly changes the structure of the document; this flag is only recommended for parsing
// documents with many PCDATA nodes in memory-constrained environments. This flag is off by default.
const unsigned int parse_embed_pcdata = 0x2000;

// The default parsing mode.
// Elements, PCDATA and CDATA sections are added to the DOM tree, character/reference entities are
// expanded, End-of-Line characters are normalized, attribute values are normalized using CDATA
// normalization rules.
const unsigned int parse_default = parse_cdata | parse_escapes | parse_wconv_attribute | parse_eol;

// The full parsing mode.
// Nodes of all types are added to the DOM tree, character/reference entities are expanded,
// End-of-Line characters are normalized, attribute values are normalized using CDATA normalization
// rules.
const unsigned int parse_full =
    parse_default | parse_pi | parse_comments | parse_declaration | parse_doctype;

// The max parse deep of xml
// Don't define it to large, otherwise, will lead stack overflow
const unsigned int parse_max_deep = 512;

typedef char_t* (*strconv_attribute_t)(char_t*, char_t);
typedef char_t* (*strconv_pcdata_t)(char_t*);

enum chartype_t
{
  ct_parse_pcdata  = 1,  // \0, &, \r, <
  ct_parse_attr    = 2,  // \0, &, \r, ', "
  ct_parse_attr_ws = 4,  // \0, &, \r, ', ", \n, tab
  ct_space         = 8,  // \r, \n, space, tab
  ct_parse_cdata   = 16, // \0, ], >, \r
  ct_parse_comment = 32, // \0, -, >, \r
  ct_symbol        = 64, // Any symbol > 127, a-z, A-Z, 0-9, _, :, -, .
  ct_start_symbol  = 128 // Any symbol > 127, a-z, A-Z, _, :
};

static const unsigned char chartype_table[256] = {
    55,  0,   0,   0,   0,   0,   0,   0,   0,   12,  12,  0,   0,   63,  0,   0,   // 0-15
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 16-31
    8,   0,   6,   0,   0,   0,   7,   6,   0,   0,   0,   0,   0,   96,  64,  0,   // 32-47
    64,  64,  64,  64,  64,  64,  64,  64,  64,  64,  192, 0,   1,   0,   48,  0,   // 48-63
    0,   192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, // 64-79
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 0,   0,   16,  0,   192, // 80-95
    0,   192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, // 96-111
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 0,   0,   0,   0,   0,   // 112-127

    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, // 128+
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,
    192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192};

enum chartypex_t
{
  ctx_special_pcdata = 1, // Any symbol >= 0 and < 32 (except \t, \r, \n), &, <, >
  ctx_special_attr   = 2, // Any symbol >= 0 and < 32 (except \t), &, <, >, "
  ctx_start_symbol   = 4, // Any symbol > 127, a-z, A-Z, _
  ctx_digit          = 8, // 0-9
  ctx_symbol         = 16 // Any symbol > 127, a-z, A-Z, 0-9, _, -, .
};

static const unsigned char chartypex_table[256] = {
    3,  3,  3,  3,  3,  3,  3,  3,  3,  0,  2,  3,  3,  2,  3,  3, // 0-15
    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3, // 16-31
    0,  0,  2,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  16, 16, 0, // 32-47
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 0,  0,  3,  0,  3,  0, // 48-63

    0,  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, // 64-79
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 0,  0,  0,  0,  20, // 80-95
    0,  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, // 96-111
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 0,  0,  0,  0,  0,  // 112-127

    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, // 128+
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20};

// Branch weight controls
#if defined(__GNUC__)
#  define XSXML__UNLIKELY(cond) __builtin_expect(cond, 0)
#else
#  define XSXML__UNLIKELY(cond) (cond)
#endif

#define XSXML__IS_CHARTYPE_IMPL(c, ct, table) (table[static_cast<unsigned char>(c)] & (ct))

#define XSXML__IS_CHARTYPE(c, ct) XSXML__IS_CHARTYPE_IMPL(c, ct, chartype_table)
#define XSXML__IS_CHARTYPEX(c, ct) XSXML__IS_CHARTYPE_IMPL(c, ct, chartypex_table)

// Parser utilities
#define XSXML__ENDSWITH(c, e) ((c) == (e) || ((c) == 0 && endch == (e)))
#define XSXML__SKIPWS()                                                                            \
  {                                                                                                \
    while (XSXML__IS_CHARTYPE(*s, ct_space))                                                       \
      ++s;                                                                                         \
  }
#define XSXML__OPTSET(OPT) (optmsk & (OPT))
#define XSXML__PUSHNODE(TYPE)                                                                      \
  {                                                                                                \
    cursor = append_new_node(cursor, alloc, TYPE);                                                 \
    if (!cursor)                                                                                   \
      XSXML__THROW_ERROR(status_out_of_memory, s);                                                 \
  }
#define XSXML__POPNODE()                                                                           \
  {                                                                                                \
    cursor = cursor->parent;                                                                       \
  }
#define XSXML__SCANFOR(X)                                                                          \
  {                                                                                                \
    while (*s != 0 && !(X))                                                                        \
      ++s;                                                                                         \
  }
#define XSXML__SCANWHILE(X)                                                                        \
  {                                                                                                \
    while (X)                                                                                      \
      ++s;                                                                                         \
  }
#define XSXML__SCANWHILE_UNROLL(X)                                                                 \
  {                                                                                                \
    for (;;)                                                                                       \
    {                                                                                              \
      char_t ss = s[0];                                                                            \
      if (XSXML__UNLIKELY(!(X)))                                                                   \
      {                                                                                            \
        break;                                                                                     \
      }                                                                                            \
      ss = s[1];                                                                                   \
      if (XSXML__UNLIKELY(!(X)))                                                                   \
      {                                                                                            \
        s += 1;                                                                                    \
        break;                                                                                     \
      }                                                                                            \
      ss = s[2];                                                                                   \
      if (XSXML__UNLIKELY(!(X)))                                                                   \
      {                                                                                            \
        s += 2;                                                                                    \
        break;                                                                                     \
      }                                                                                            \
      ss = s[3];                                                                                   \
      if (XSXML__UNLIKELY(!(X)))                                                                   \
      {                                                                                            \
        s += 3;                                                                                    \
        break;                                                                                     \
      }                                                                                            \
      s += 4;                                                                                      \
    }                                                                                              \
  }
#define XSXML__ENDSEG()                                                                            \
  {                                                                                                \
    ch = *s;                                                                                       \
    *s = 0;                                                                                        \
    ++s;                                                                                           \
  }
#define XSXML__THROW_ERROR(err, m)                                                                 \
  return error_offset = m, error_status = err, static_cast<char_t*>(0)
#define XSXML__CHECK_ERROR(err, m)                                                                 \
  {                                                                                                \
    if (*s == 0)                                                                                   \
      XSXML__THROW_ERROR(err, m);                                                                  \
  }

// Simple static assertion
#define XSXML__STATIC_ASSERT(cond)                                                                 \
  {                                                                                                \
    static const char condition_failed[(cond) ? 1 : -1] = {0};                                     \
    (void)condition_failed[0];                                                                     \
  }

// Parsing result
struct xml_parse_result
{
  // Parsing status (see xml_parse_status)
  xml_parse_status status;

  // Last parsed offset (in char_t units from start of input data)
  ptrdiff_t offset;

  // Source document encoding
  // xml_encoding encoding;

  // Default constructor, initializes object to failed state
  xml_parse_result() : status(status_internal_error), offset(0) {}

  // Cast to bool operator
  operator bool() const { return status == status_ok; }

  // Get error description
  const char* description() const;
};

struct opt_false
{
  enum
  {
    value = 0
  };
};

struct opt_true
{
  enum
  {
    value = 1
  };
};

struct gap
{
  char_t* end;
  size_t size;

  gap() : end(0), size(0) {}

  // Push new gap, move s count bytes further (skipping the gap).
  // Collapse previous gap.
  void push(char_t*& s, size_t count)
  {
    if (end) // there was a gap already; collapse it
    {
      // Move [old_gap_end, new_gap_start) to [old_gap_start, ...)
      assert(s >= end);
      memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));
    }

    s += count; // end of current gap

    // "merge" two gaps
    end = s;
    size += count;
  }

  // Collapse all gaps, return past-the-end pointer
  char_t* flush(char_t* s)
  {
    if (end)
    {
      // Move [old_gap_end, current_pos) to [old_gap_start, ...)
      assert(s >= end);
      memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));

      return s - size;
    }
    else
      return s;
  }
};

template <typename opt_trim, typename opt_eol, typename opt_escape> struct strconv_pcdata_impl
{
  static char_t* parse(char_t* s)
  {
    gap g;

    char_t* begin = s;

    while (true)
    {
      XSXML__SCANWHILE_UNROLL(!XSXML__IS_CHARTYPE(ss, ct_parse_pcdata));

      if (*s == '<') // PCDATA ends here
      {
        char_t* end = g.flush(s);

        if (opt_trim::value)
          while (end > begin && XSXML__IS_CHARTYPE(end[-1], ct_space))
            --end;

        *end = 0;

        return s + 1;
      }
      else if (opt_eol::value && *s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
      {
        *s++ = '\n'; // replace first one with 0x0a

        if (*s == '\n')
          g.push(s, 1);
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (*s == 0)
      {
        char_t* end = g.flush(s);

        if (opt_trim::value)
          while (end > begin && XSXML__IS_CHARTYPE(end[-1], ct_space))
            --end;

        *end = 0;

        return s;
      }
      else
        ++s;
    }
  }
};

XSXML__DECL strconv_pcdata_t get_strconv_pcdata(unsigned int optmask)
{
  XSXML__STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20 && parse_trim_pcdata == 0x0800);

  switch (((optmask >> 4) & 3) | ((optmask >> 9) & 4)) // get bitmask for flags (eol escapes trim)
  {
    case 0:
      return strconv_pcdata_impl<opt_false, opt_false, opt_false>::parse;
    case 1:
      return strconv_pcdata_impl<opt_false, opt_false, opt_true>::parse;
    case 2:
      return strconv_pcdata_impl<opt_false, opt_true, opt_false>::parse;
    case 3:
      return strconv_pcdata_impl<opt_false, opt_true, opt_true>::parse;
    case 4:
      return strconv_pcdata_impl<opt_true, opt_false, opt_false>::parse;
    case 5:
      return strconv_pcdata_impl<opt_true, opt_false, opt_true>::parse;
    case 6:
      return strconv_pcdata_impl<opt_true, opt_true, opt_false>::parse;
    case 7:
      return strconv_pcdata_impl<opt_true, opt_true, opt_true>::parse;
    default:
      assert(false);
      return 0; // should not get here
  }
}

struct utf8_writer
{
  typedef uint8_t* value_type;

  static value_type low(value_type result, uint32_t ch)
  {
    // U+0000..U+007F
    if (ch < 0x80)
    {
      *result = static_cast<uint8_t>(ch);
      return result + 1;
    }
    // U+0080..U+07FF
    else if (ch < 0x800)
    {
      result[0] = static_cast<uint8_t>(0xC0 | (ch >> 6));
      result[1] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
      return result + 2;
    }
    // U+0800..U+FFFF
    else
    {
      result[0] = static_cast<uint8_t>(0xE0 | (ch >> 12));
      result[1] = static_cast<uint8_t>(0x80 | ((ch >> 6) & 0x3F));
      result[2] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
      return result + 3;
    }
  }

  static value_type high(value_type result, uint32_t ch)
  {
    // U+10000..U+10FFFF
    result[0] = static_cast<uint8_t>(0xF0 | (ch >> 18));
    result[1] = static_cast<uint8_t>(0x80 | ((ch >> 12) & 0x3F));
    result[2] = static_cast<uint8_t>(0x80 | ((ch >> 6) & 0x3F));
    result[3] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
    return result + 4;
  }

  static value_type any(value_type result, uint32_t ch)
  {
    return (ch < 0x10000) ? low(result, ch) : high(result, ch);
  }
};

XSXML__DECL char_t* strconv_escape(char_t* s, gap& g)
{
  char_t* stre = s + 1;

  switch (*stre)
  {
    case '#': // &#...
    {
      unsigned int ucsc = 0;

      if (stre[1] == 'x') // &#x... (hex code)
      {
        stre += 2;

        char_t ch = *stre;

        if (ch == ';')
          return stre;

        for (;;)
        {
          if (static_cast<unsigned int>(ch - '0') <= 9)
            ucsc = 16 * ucsc + (ch - '0');
          else if (static_cast<unsigned int>((ch | ' ') - 'a') <= 5)
            ucsc = 16 * ucsc + ((ch | ' ') - 'a' + 10);
          else if (ch == ';')
            break;
          else // cancel
            return stre;

          ch = *++stre;
        }

        ++stre;
      }
      else // &#... (dec code)
      {
        char_t ch = *++stre;

        if (ch == ';')
          return stre;

        for (;;)
        {
          if (static_cast<unsigned int>(static_cast<unsigned int>(ch) - '0') <= 9)
            ucsc = 10 * ucsc + (ch - '0');
          else if (ch == ';')
            break;
          else // cancel
            return stre;

          ch = *++stre;
        }

        ++stre;
      }

      s = reinterpret_cast<char_t*>(utf8_writer::any(reinterpret_cast<uint8_t*>(s), ucsc));

      g.push(s, stre - s);
      return stre;
    }

    case 'a': // &a
    {
      ++stre;

      if (*stre == 'm') // &am
      {
        if (*++stre == 'p' && *++stre == ';') // &amp;
        {
          *s++ = '&';
          ++stre;

          g.push(s, stre - s);
          return stre;
        }
      }
      else if (*stre == 'p') // &ap
      {
        if (*++stre == 'o' && *++stre == 's' && *++stre == ';') // &apos;
        {
          *s++ = '\'';
          ++stre;

          g.push(s, stre - s);
          return stre;
        }
      }
      break;
    }

    case 'g': // &g
    {
      if (*++stre == 't' && *++stre == ';') // &gt;
      {
        *s++ = '>';
        ++stre;

        g.push(s, stre - s);
        return stre;
      }
      break;
    }

    case 'l': // &l
    {
      if (*++stre == 't' && *++stre == ';') // &lt;
      {
        *s++ = '<';
        ++stre;

        g.push(s, stre - s);
        return stre;
      }
      break;
    }

    case 'q': // &q
    {
      if (*++stre == 'u' && *++stre == 'o' && *++stre == 't' && *++stre == ';') // &quot;
      {
        *s++ = '"';
        ++stre;

        g.push(s, stre - s);
        return stre;
      }
      break;
    }

    default:
      break;
  }

  return stre;
}

template <typename opt_escape> struct strconv_attribute_impl
{
  static char_t* parse_wnorm(char_t* s, char_t end_quote)
  {
    gap g;

    // trim leading whitespaces
    if (XSXML__IS_CHARTYPE(*s, ct_space))
    {
      char_t* str = s;

      do
        ++str;
      while (XSXML__IS_CHARTYPE(*str, ct_space));

      g.push(s, str - s);
    }

    while (true)
    {
      XSXML__SCANWHILE_UNROLL(!XSXML__IS_CHARTYPE(ss, ct_parse_attr_ws | ct_space));

      if (*s == end_quote)
      {
        char_t* str = g.flush(s);

        do
          *str-- = 0;
        while (XSXML__IS_CHARTYPE(*str, ct_space));

        return s + 1;
      }
      else if (XSXML__IS_CHARTYPE(*s, ct_space))
      {
        *s++ = ' ';

        if (XSXML__IS_CHARTYPE(*s, ct_space))
        {
          char_t* str = s + 1;
          while (XSXML__IS_CHARTYPE(*str, ct_space))
            ++str;

          g.push(s, str - s);
        }
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (!*s)
      {
        return 0;
      }
      else
        ++s;
    }
  }

  static char_t* parse_wconv(char_t* s, char_t end_quote)
  {
    gap g;

    while (true)
    {
      XSXML__SCANWHILE_UNROLL(!XSXML__IS_CHARTYPE(ss, ct_parse_attr_ws));

      if (*s == end_quote)
      {
        *g.flush(s) = 0;

        return s + 1;
      }
      else if (XSXML__IS_CHARTYPE(*s, ct_space))
      {
        if (*s == '\r')
        {
          *s++ = ' ';

          if (*s == '\n')
            g.push(s, 1);
        }
        else
          *s++ = ' ';
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (!*s)
      {
        return 0;
      }
      else
        ++s;
    }
  }

  static char_t* parse_eol(char_t* s, char_t end_quote)
  {
    gap g;

    while (true)
    {
      XSXML__SCANWHILE_UNROLL(!XSXML__IS_CHARTYPE(ss, ct_parse_attr));

      if (*s == end_quote)
      {
        *g.flush(s) = 0;

        return s + 1;
      }
      else if (*s == '\r')
      {
        *s++ = '\n';

        if (*s == '\n')
          g.push(s, 1);
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (!*s)
      {
        return 0;
      }
      else
        ++s;
    }
  }

  static char_t* parse_simple(char_t* s, char_t end_quote)
  {
    gap g;

    while (true)
    {
      XSXML__SCANWHILE_UNROLL(!XSXML__IS_CHARTYPE(ss, ct_parse_attr));

      if (*s == end_quote)
      {
        *g.flush(s) = 0;

        return s + 1;
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (!*s)
      {
        return 0;
      }
      else
        ++s;
    }
  }
};

XSXML__DECL xml_parse_result make_parse_result(xml_parse_status status, ptrdiff_t offset = 0)
{
  xml_parse_result result;
  result.status = status;
  result.offset = offset;

  return result;
}

XSXML__DECL char_t* strconv_comment(char_t* s, char_t endch)
{
  gap g;

  while (true)
  {
    XSXML__SCANWHILE_UNROLL(!XSXML__IS_CHARTYPE(ss, ct_parse_comment));

    if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
    {
      *s++ = '\n'; // replace first one with 0x0a

      if (*s == '\n')
        g.push(s, 1);
    }
    else if (s[0] == '-' && s[1] == '-' && XSXML__ENDSWITH(s[2], '>')) // comment ends here
    {
      *g.flush(s) = 0;

      return s + (s[2] == '>' ? 3 : 2);
    }
    else if (*s == 0)
    {
      return 0;
    }
    else
      ++s;
  }
}

XSXML__DECL char_t* strconv_cdata(char_t* s, char_t endch)
{
  gap g;

  while (true)
  {
    XSXML__SCANWHILE_UNROLL(!XSXML__IS_CHARTYPE(ss, ct_parse_cdata));

    if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
    {
      *s++ = '\n'; // replace first one with 0x0a

      if (*s == '\n')
        g.push(s, 1);
    }
    else if (s[0] == ']' && s[1] == ']' && XSXML__ENDSWITH(s[2], '>')) // CDATA ends here
    {
      *g.flush(s) = 0;

      return s + 1;
    }
    else if (*s == 0)
    {
      return 0;
    }
    else
      ++s;
  }
}

XSXML__DECL strconv_attribute_t get_strconv_attribute(unsigned int optmask)
{
  XSXML__STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20 &&
                       parse_wconv_attribute == 0x40 && parse_wnorm_attribute == 0x80);

  switch ((optmask >> 4) & 15) // get bitmask for flags (wconv wnorm eol escapes)
  {
    case 0:
      return strconv_attribute_impl<opt_false>::parse_simple;
    case 1:
      return strconv_attribute_impl<opt_true>::parse_simple;
    case 2:
      return strconv_attribute_impl<opt_false>::parse_eol;
    case 3:
      return strconv_attribute_impl<opt_true>::parse_eol;
    case 4:
      return strconv_attribute_impl<opt_false>::parse_wconv;
    case 5:
      return strconv_attribute_impl<opt_true>::parse_wconv;
    case 6:
      return strconv_attribute_impl<opt_false>::parse_wconv;
    case 7:
      return strconv_attribute_impl<opt_true>::parse_wconv;
    case 8:
      return strconv_attribute_impl<opt_false>::parse_wnorm;
    case 9:
      return strconv_attribute_impl<opt_true>::parse_wnorm;
    case 10:
      return strconv_attribute_impl<opt_false>::parse_wnorm;
    case 11:
      return strconv_attribute_impl<opt_true>::parse_wnorm;
    case 12:
      return strconv_attribute_impl<opt_false>::parse_wnorm;
    case 13:
      return strconv_attribute_impl<opt_true>::parse_wnorm;
    case 14:
      return strconv_attribute_impl<opt_false>::parse_wnorm;
    case 15:
      return strconv_attribute_impl<opt_true>::parse_wnorm;
    default:
      assert(false);
      return 0; // should not get here
  }
}

// Skip utf-8 bom
static char_t* parse_skip_bom(char_t* s)
{
  return (s[0] == '\xef' && s[1] == '\xbb' && s[2] == '\xbf') ? s + 3 : s;
}

// Simple string view
class string_view
{
public:
  string_view() : _Mystr(nullptr), _Mysize(0) {}
  string_view(char_t* str, size_t size) : _Mystr(str), _Mysize(size) {}
  const char* c_str() const { return _Mystr != nullptr ? _Mystr : ""; }
  size_t length() const { return _Mysize; }
  bool empty() const { return _Mysize == 0; }

private:
  char_t* _Mystr;
  size_t _Mysize;
};

// The sax3 parse callbacks
struct xml_sax3_parse_cb
{
  std::function<void(char* name, size_t)> xml_start_element_cb;
  std::function<void(const char* name, size_t, const char* value, size_t)> xml_attr_cb;
  std::function<void()> xml_end_attr_cb;
  std::function<void(const char* name, size_t)> xml_end_element_cb;
  std::function<void(const char* text, size_t len)> xml_text_cb;
};

/////////////// xml_sax3_parser ///////////
struct xml_sax3_parser
{
  // xml_allocator alloc;
  char_t* error_offset;
  xml_parse_status error_status;

  xml_sax3_parse_cb* handler;

  xml_sax3_parser(xml_sax3_parse_cb* handler_)
      : handler(handler_), error_offset(0), error_status(status_ok)
  {}

  ~xml_sax3_parser()
  {
    // *alloc_state = alloc;
  }

  // DOCTYPE consists of nested sections of the following possible types:
  // <!-- ... -->, <? ... ?>, "...", '...'
  // <![...]]>
  // <!...>
  // First group can not contain nested groups
  // Second group can contain nested groups of the same type
  // Third group can contain all other groups
  char_t* parse_doctype_primitive(char_t* s)
  {
    if (*s == '"' || *s == '\'')
    {
      // quoted string
      char_t ch = *s++;
      XSXML__SCANFOR(*s == ch);
      if (!*s)
        XSXML__THROW_ERROR(status_bad_doctype, s);

      s++;
    }
    else if (s[0] == '<' && s[1] == '?')
    {
      // <? ... ?>
      s += 2;
      XSXML__SCANFOR(s[0] == '?' &&
                     s[1] == '>'); // no need for ENDSWITH because ?> can't terminate proper doctype
      if (!*s)
        XSXML__THROW_ERROR(status_bad_doctype, s);

      s += 2;
    }
    else if (s[0] == '<' && s[1] == '!' && s[2] == '-' && s[3] == '-')
    {
      s += 4;
      XSXML__SCANFOR(s[0] == '-' && s[1] == '-' &&
                     s[2] ==
                         '>'); // no need for ENDSWITH because --> can't terminate proper doctype
      if (!*s)
        XSXML__THROW_ERROR(status_bad_doctype, s);

      s += 3;
    }
    else
      XSXML__THROW_ERROR(status_bad_doctype, s);

    return s;
  }

  char_t* parse_doctype_ignore(char_t* s)
  {
    size_t depth = 0;

    assert(s[0] == '<' && s[1] == '!' && s[2] == '[');
    s += 3;

    while (*s)
    {
      if (s[0] == '<' && s[1] == '!' && s[2] == '[')
      {
        // nested ignore section
        s += 3;
        depth++;
      }
      else if (s[0] == ']' && s[1] == ']' && s[2] == '>')
      {
        // ignore section end
        s += 3;

        if (depth == 0)
          return s;

        depth--;
      }
      else
        s++;
    }

    XSXML__THROW_ERROR(status_bad_doctype, s);
  }

  char_t* parse_doctype_group(char_t* s, char_t endch)
  {
    size_t depth = 0;

    assert((s[0] == '<' || s[0] == 0) && s[1] == '!');
    s += 2;

    while (*s)
    {
      if (s[0] == '<' && s[1] == '!' && s[2] != '-')
      {
        if (s[2] == '[')
        {
          // ignore
          s = parse_doctype_ignore(s);
          if (!s)
            return s;
        }
        else
        {
          // some control group
          s += 2;
          depth++;
        }
      }
      else if (s[0] == '<' || s[0] == '"' || s[0] == '\'')
      {
        // unknown tag (forbidden), or some primitive group
        s = parse_doctype_primitive(s);
        if (!s)
          return s;
      }
      else if (*s == '>')
      {
        if (depth == 0)
          return s;

        depth--;
        s++;
      }
      else
        s++;
    }

    if (depth != 0 || endch != '>')
      XSXML__THROW_ERROR(status_bad_doctype, s);

    return s;
  }

  char_t* parse_exclamation(char_t* s, unsigned int optmsk, char_t endch)
  {
    // parse node contents, starting with exclamation mark
    ++s;

    if (*s == '-') // '<!-...'
    {
      ++s;

      if (*s == '-') // '<!--...'
      {
        ++s;

        char_t* value = nullptr;
        if (XSXML__OPTSET(parse_comments))
        {
          // SAX3: Ignore comment.
          // XSXML__PUSHNODE(node_comment); // Append a new node on the tree.
          // cursor->value = s; // Save the offset.
          value = s;
        }

        if (XSXML__OPTSET(parse_eol) && XSXML__OPTSET(parse_comments))
        {
          s = strconv_comment(s, endch);

          if (!s)
            XSXML__THROW_ERROR(status_bad_comment, value);
        }
        else
        {
          // Scan for terminating '-->'.
          XSXML__SCANFOR(s[0] == '-' && s[1] == '-' && XSXML__ENDSWITH(s[2], '>'));
          XSXML__CHECK_ERROR(status_bad_comment, s);

          if (XSXML__OPTSET(parse_comments))
            *s = 0; // Zero-terminate this segment at the first terminating '-'.

          s += (s[2] == '>' ? 3 : 2); // Step over the '\0->'.
        }
      }
      else
        XSXML__THROW_ERROR(status_bad_comment, s);
    }
    else if (*s == '[')
    {
      // '<![CDATA[...'
      if (*++s == 'C' && *++s == 'D' && *++s == 'A' && *++s == 'T' && *++s == 'A' && *++s == '[')
      {
        ++s;

        if (XSXML__OPTSET(parse_cdata))
        {
          // SAX3: Ignore CDATA
          // XSXML__PUSHNODE(node_cdata); // Append a new node on the tree.
          auto value = s; // Save the offset.

          if (XSXML__OPTSET(parse_eol))
          {
            s = strconv_cdata(s, endch);

            if (!s)
              XSXML__THROW_ERROR(status_bad_cdata, value);
          }
          else
          {
            // Scan for terminating ']]>'.
            XSXML__SCANFOR(s[0] == ']' && s[1] == ']' && XSXML__ENDSWITH(s[2], '>'));
            XSXML__CHECK_ERROR(status_bad_cdata, s);

            *s++ = 0; // Zero-terminate this segment.
          }
        }
        else // Flagged for discard, but we still have to scan for the terminator.
        {
          // Scan for terminating ']]>'.
          XSXML__SCANFOR(s[0] == ']' && s[1] == ']' && XSXML__ENDSWITH(s[2], '>'));
          XSXML__CHECK_ERROR(status_bad_cdata, s);

          ++s;
        }

        s += (s[1] == '>' ? 2 : 1); // Step over the last ']>'.
      }
      else
        XSXML__THROW_ERROR(status_bad_cdata, s);
    }
    else if (s[0] == 'D' && s[1] == 'O' && s[2] == 'C' && s[3] == 'T' && s[4] == 'Y' &&
             s[5] == 'P' && XSXML__ENDSWITH(s[6], 'E'))
    {
      s -= 2;

      // TODO: check for doctype, parent must be nullptr
      // if (cursor->parent) XSXML__THROW_ERROR(status_bad_doctype, s);

      char_t* mark = s + 9;

      s = parse_doctype_group(s, endch);
      if (!s)
        return s;

      assert((*s == 0 && endch == '>') || *s == '>');
      if (*s)
        *s++ = 0;

      if (XSXML__OPTSET(parse_doctype))
      {
        while (XSXML__IS_CHARTYPE(*mark, ct_space))
          ++mark;

        // SAX3: Ignore doctype
        // XSXML__PUSHNODE(node_doctype);

        // cursor->value = mark;
      }
    }
    else if (*s == 0 && endch == '-')
      XSXML__THROW_ERROR(status_bad_comment, s);
    else if (*s == 0 && endch == '[')
      XSXML__THROW_ERROR(status_bad_cdata, s);
    else
      XSXML__THROW_ERROR(status_unrecognized_tag, s);

    return s;
  }

  char_t* parse_question(char_t* s, unsigned int optmsk, char_t endch)
  {
    // load into registers
    // xml_node_struct* cursor = ref_cursor;
    char_t ch = 0;

    // parse node contents, starting with question mark
    ++s;

    // read PI target
    char_t* target = s;

    if (!XSXML__IS_CHARTYPE(*s, ct_start_symbol))
      XSXML__THROW_ERROR(status_bad_pi, s);

    XSXML__SCANWHILE(XSXML__IS_CHARTYPE(*s, ct_symbol));
    XSXML__CHECK_ERROR(status_bad_pi, s);

    // determine node type; stricmp / strcasecmp is not portable
    bool declaration = (target[0] | ' ') == 'x' && (target[1] | ' ') == 'm' &&
                       (target[2] | ' ') == 'l' && target + 3 == s;

    if (declaration ? XSXML__OPTSET(parse_declaration) : XSXML__OPTSET(parse_pi))
    {
      if (declaration)
      {
        // TODO: disallow non top-level declarations
        // if (cursor->parent) XSXML__THROW_ERROR(status_bad_pi, s);

        // SAX3: Ignore declaration.
        // XSXML__PUSHNODE(node_declaration);
      }
      else
      {
        // SAX3: Ignore pi.
        // XSXML__PUSHNODE(node_pi);
      }

      XSXML__ENDSEG();

      // parse value/attributes
      if (ch == '?')
      {
        // empty node
        if (!XSXML__ENDSWITH(*s, '>'))
          XSXML__THROW_ERROR(status_bad_pi, s);
        s += (*s == '>');

        // XSXML__POPNODE();
      }
      else if (XSXML__IS_CHARTYPE(ch, ct_space))
      {
        XSXML__SKIPWS();

        // scan for tag end
        char_t* value = s;

        XSXML__SCANFOR(s[0] == '?' && XSXML__ENDSWITH(s[1], '>'));
        XSXML__CHECK_ERROR(status_bad_pi, s);

        if (declaration)
        {
          // replace ending ? with / so that 'element' terminates properly
          *s = '/';

          // we exit from this function with cursor at node_declaration, which is a signal to
          // parse() to go to LOC_ATTRIBUTES
          s = value;
        }
        else
        {
          // store value and step over >
          // cursor->value = value;

          // XSXML__POPNODE();

          XSXML__ENDSEG();

          s += (*s == '>');
        }
      }
      else
        XSXML__THROW_ERROR(status_bad_pi, s);
    }
    else
    {
      // scan for tag end
      XSXML__SCANFOR(s[0] == '?' && XSXML__ENDSWITH(s[1], '>'));
      XSXML__CHECK_ERROR(status_bad_pi, s);

      s += (s[1] == '>' ? 2 : 1);
    }

    // store from registers
    // ref_cursor = cursor;

    return s;
  }

  template <typename _T, size_t _Capacity> struct fixed_stack
  {
  public:
    fixed_stack() : size_(0) {}

    void push(const _T& val)
    {
      if (size_ < _Capacity)
        elements_[size_++] = val;
    }

    _T pop()
    {
      if (size_ > 0)
        return elements_[size_-- - 1];
      return _T{};
    }

  private:
    _T elements_[_Capacity];
    size_t size_;
  };

  char_t* parse_tree(char_t* s, unsigned int optmsk, char_t endch)
  {
    strconv_attribute_t strconv_attribute = get_strconv_attribute(optmsk);
    strconv_pcdata_t strconv_pcdata       = get_strconv_pcdata(optmsk);

    char_t ch     = 0;
    char_t* mark  = s;
    char_t* value = nullptr;
    size_t n      = 0;

    fixed_stack<string_view, parse_max_deep> stk; // 4K on 32bits, 6K on 64bits

    while (*s != 0)
    {
      if (*s == '<')
      {
        ++s;

      LOC_TAG:
        if (XSXML__IS_CHARTYPE(*s, ct_start_symbol)) // '<#...'
        {
          // SAX3: TODO: xmlStartElement.
          // XSXML__PUSHNODE(node_element); // Append a new node to the tree.

          mark = s;

          XSXML__SCANWHILE_UNROLL(XSXML__IS_CHARTYPE(ss, ct_symbol)); // Scan for a terminator.

          handler->xml_start_element_cb(mark, s - mark);
          stk.push(::xsxml::string_view(mark, s - mark));

          XSXML__ENDSEG(); // Save char in 'ch', terminate & step over.

          if (ch == '>')
          {
            handler->xml_end_attr_cb(); // end of tag
          }
          else if (XSXML__IS_CHARTYPE(ch, ct_space))
          {
            while (true)
            {                  // parse attributes
              XSXML__SKIPWS(); // Eat any whitespace.

              if (XSXML__IS_CHARTYPE(*s, ct_start_symbol)) // <... #...
              {
                // SAX3: TODO: implement attribute.
                // xml_attribute_struct* a = append_new_attribute(cursor, alloc); // Make space for
                // this attribute. if (!a) XSXML__THROW_ERROR(status_out_of_memory, s);

                mark = s; // Save the offset.

                XSXML__SCANWHILE_UNROLL(
                    XSXML__IS_CHARTYPE(ss, ct_symbol)); // Scan for a terminator.
                n = s - mark;
                XSXML__ENDSEG(); // Save char in 'ch', terminate & step over.

                if (XSXML__IS_CHARTYPE(ch, ct_space))
                {
                  XSXML__SKIPWS(); // Eat any whitespace.

                  ch = *s;
                  ++s;
                }

                if (ch == '=') // '<... #=...'
                {
                  XSXML__SKIPWS(); // Eat any whitespace.

                  if (*s == '"' || *s == '\'') // '<... #="...'
                  {
                    ch = *s;   // Save quote char to avoid breaking on "''" -or- '""'.
                    ++s;       // Step over the quote.
                    value = s; // a->value = s; // Save the offset.

                    s = strconv_attribute(s, ch);

                    if (!s)
                      XSXML__THROW_ERROR(status_bad_attribute, value);

                    // After this line the loop continues from the start;
                    // Whitespaces, / and > are ok, symbols and EOF are wrong,
                    // everything else will be detected
                    if (XSXML__IS_CHARTYPE(*s, ct_start_symbol))
                      XSXML__THROW_ERROR(status_bad_attribute, s);
                    handler->xml_attr_cb(mark, n, value, s - value - 1);
                  }
                  else
                    XSXML__THROW_ERROR(status_bad_attribute, s);
                }
                else
                  XSXML__THROW_ERROR(status_bad_attribute, s);
              }
              else if (*s == '/')
              {
                ++s;
                if (*s == '>')
                {
                  auto ele_name = stk.pop();
                  handler->xml_end_attr_cb();
                  handler->xml_end_element_cb(ele_name.c_str(), ele_name.length());
                  ++s;
                  break;
                }
                else if (*s == 0 && endch == '>')
                {
                  auto ele_name = stk.pop();
                  handler->xml_end_attr_cb();
                  handler->xml_end_element_cb(ele_name.c_str(), ele_name.length());
                  break;
                }
                else
                  XSXML__THROW_ERROR(status_bad_start_element, s);
              }
              else if (*s == '>')
              {
                ++s;
                handler->xml_end_attr_cb();
                break;
              }
              else if (*s == 0 && endch == '>')
              {
                break;
              }
              else
                XSXML__THROW_ERROR(status_bad_start_element, s);
            }

            // !!!
          }
          else if (ch == '/') // '<#.../'
          {
            if (!XSXML__ENDSWITH(*s, '>'))
              XSXML__THROW_ERROR(status_bad_start_element, s);

            stk.pop();
            handler->xml_end_element_cb(mark, s - mark);
            s += (*s == '>');
          }
          else if (ch == 0)
          {
            // we stepped over null terminator, backtrack & handle closing tag
            --s;

            if (endch != '>')
              XSXML__THROW_ERROR(status_bad_start_element, s);
          }
          else
            XSXML__THROW_ERROR(status_bad_start_element, s);
        }
        else if (*s == '/')
        {
          ++s;

          mark = s;

          // SAX3, we don't check end element name
          while (XSXML__IS_CHARTYPE(*s, ct_symbol))
            ++s;

          stk.pop();
          handler->xml_end_element_cb(mark, s - mark);

          XSXML__SKIPWS();

          if (*s == 0)
          {
            if (endch != '>')
              XSXML__THROW_ERROR(status_bad_end_element, s);
          }
          else
          {
            if (*s != '>')
              XSXML__THROW_ERROR(status_bad_end_element, s);
            ++s;
          }
        }
        else if (*s == '?') // '<?...'
        {
          // SAX3: TODO: parse question.
          s = parse_question(s, optmsk, endch);
          if (!s)
            return s;

          // assert(cursor);
          // if (XSXML__NODETYPE(cursor) == node_declaration) goto LOC_ATTRIBUTES;
          // goto LOC_ATTRIBUTES; // SAX3: always regard as a valid node_declaration
        }
        else if (*s == '!') // '<!...'
        {
          // SAX3: TODO: parse exclamation.
          s = parse_exclamation(s, optmsk, endch);
          if (!s)
            return s;
        }
        else if (*s == 0 && endch == '?')
          XSXML__THROW_ERROR(status_bad_pi, s);
        else
          XSXML__THROW_ERROR(status_unrecognized_tag, s);
      }
      else
      {
        mark = s; // Save this offset while searching for a terminator.

        XSXML__SKIPWS(); // Eat whitespace if no genuine PCDATA here.

        if (*s == '<' || !*s)
        {
          // We skipped some whitespace characters because otherwise we would take the tag branch
          // instead of PCDATA one
          assert(mark != s);

          if (!XSXML__OPTSET(parse_ws_pcdata | parse_ws_pcdata_single) ||
              XSXML__OPTSET(parse_trim_pcdata))
          {
            continue;
          }
          else if (XSXML__OPTSET(parse_ws_pcdata_single))
          {
            // SAX3: TODO: parse_ws_pcdata_single
            // if (s[0] != '<' || s[1] != '/' || cursor->first_child) continue;
          }
        }

        if (!XSXML__OPTSET(parse_trim_pcdata))
          s = mark;

        // SAX3: Ignore node_pcdata.
        if (/*cursor->parent ||*/ XSXML__OPTSET(parse_fragment))
        { // Currently, SAX3 simplely skip, do not regard text it node
          if (XSXML__OPTSET(parse_embed_pcdata) /*&& cursor->parent && !cursor->first_child && !cursor->value*/)
          {
            // cursor->value = s; // Save the offset.
          }
          else
          {
            // XSXML__PUSHNODE(node_pcdata); // Append a new node on the tree.

            // cursor->value = s; // Save the offset.

            // XSXML__POPNODE(); // Pop since this is a standalone.
          }

          s = strconv_pcdata(s);

          if (!*s)
            break;
        }
        else
        {
          XSXML__SCANFOR(*s == '<'); // '...<'
          if (!*s)
            break;

          handler->xml_text_cb(mark, s - mark);

          ++s;
        }

        // We're after '<'
        goto LOC_TAG;
      }
    }

    // SAX3: TODO: check that last tag is closed,
    // if (cursor != root) XSXML__THROW_ERROR(status_end_element_mismatch, s);

    return s;
  }

  static char_t* parse_skip_bom(char_t* s)
  {
    return (s[0] == '\xef' && s[1] == '\xbb' && s[2] == '\xbf') ? s + 3 : s;
  }

  /*static bool has_element_node_siblings(xml_node_struct* node)
  {
      while (node)
      {
          if (XSXML__NODETYPE(node) == node_element) return true;

          node = node->next_sibling;
      }

      return false;
  }*/

  static xml_parse_result parse(char_t* buffer, size_t length, xml_sax3_parse_cb* handler,
                                unsigned int optmsk = parse_default)
  {
    // early-out for empty documents
    if (length == 0)
      return make_parse_result(XSXML__OPTSET(parse_fragment) ? status_ok
                                                             : status_no_document_element);

    // get last child of the root before parsing
    // xml_node_struct* last_root_child = root->first_child ? root->first_child->prev_sibling_c + 0
    // : 0;

    // create parser on stack
    xml_sax3_parser parser(handler);

    // save last character and make buffer zero-terminated (speeds up parsing)
    char_t endch       = buffer[length - 1];
    buffer[length - 1] = 0;

    // skip BOM to make sure it does not end up as part of parse output
    char_t* buffer_data = parse_skip_bom(buffer);

    // perform actual parsing
    parser.parse_tree(buffer_data, optmsk, endch);

    xml_parse_result result = make_parse_result(
        parser.error_status, parser.error_offset ? parser.error_offset - buffer : 0);
    assert(result.offset >= 0 && static_cast<size_t>(result.offset) <= length);

    if (result)
    {
      // since we removed last character, we have to handle the only possible false positive (stray
      // <)
      if (endch == '<')
        return make_parse_result(status_unrecognized_tag, length - 1);
    }
    else
    {
      // roll back offset if it occurs on a null terminator in the source buffer
      if (result.offset > 0 && static_cast<size_t>(result.offset) == length - 1 && endch == 0)
        result.offset--;
    }

    return result;
  }
}; /* xml_sax3_parser */
}; // namespace xsxml

#endif
