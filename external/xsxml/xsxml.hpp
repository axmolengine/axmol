//////////////////////////////////////////////////////////////////////////////////////////
// The embedded xml SAX parser, extract from rapidxml DOM parser
// please see: http://rapidxml.sourceforge.net/
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

#include <assert.h>
#include <vector>
#include <utility>
#include <functional>

// On MSVC, disable "conditional expression is constant" warning (level 4).
// This warning is almost impossible to avoid with certain types of templated code
#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable : 4127) // Conditional expression is constant
#endif

#if !defined(XSXML__PARSE_ERROR)
#  define XSXML__PARSE_ERROR(what, where) throw parse_error(what, where)
#endif

namespace xsxml
{
//! Parse flag instructing the parser to not create data nodes.
//! Text of first data node will still be placed in value of parent element, unless
//! xsxml::parse_no_element_values flag is also specified. Can be combined with other flags by
//! use of | operator. <br><br> See xml_document::parse() function.
const int parse_no_data_nodes = 0x1;

//! Parse flag instructing the parser to not use text of first data node as a value of parent
//! element. Can be combined with other flags by use of | operator. Note that child data nodes of
//! element node take precendence over its value when printing. That is, if element has one or more
//! child data nodes <em>and</em> a value, the value will be ignored. Use
//! xsxml::parse_no_data_nodes flag to prevent creation of data nodes if you want to manipulate
//! data using values of elements. <br><br> See xml_document::parse() function.
const int parse_no_element_values = 0x2;

//! Parse flag instructing the parser to not place zero terminators after strings in the source
//! text. By default zero terminators are placed, modifying source text. Can be combined with other
//! flags by use of | operator. <br><br> See xml_document::parse() function.
const int parse_no_string_terminators = 0x4;

//! Parse flag instructing the parser to not translate entities in the source text.
//! By default entities are translated, modifying source text.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_no_entity_translation = 0x8;

//! Parse flag instructing the parser to disable UTF-8 handling and assume plain 8 bit characters.
//! By default, UTF-8 handling is enabled.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_no_utf8 = 0x10;

//! Parse flag instructing the parser to create XML declaration node.
//! By default, declaration node is not created.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_declaration_node = 0x20;

//! Parse flag instructing the parser to create comments nodes.
//! By default, comment nodes are not created.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_comment_nodes = 0x40;

//! Parse flag instructing the parser to create DOCTYPE node.
//! By default, doctype node is not created.
//! Although W3C specification allows at most one DOCTYPE node, RapidXml will silently accept
//! documents with more than one. Can be combined with other flags by use of | operator. <br><br>
//! See xml_document::parse() function.
static const int parse_doctype_node = 0x80;

//! Parse flag instructing the parser to create PI nodes.
//! By default, PI nodes are not created.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_pi_nodes = 0x100;

//! Parse flag instructing the parser to validate closing tag names.
//! If not set, name inside closing tag is irrelevant to the parser.
//! By default, closing tags are not validated.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_validate_closing_tags = 0x200;

//! Parse flag instructing the parser to trim all leading and trailing whitespace of data nodes.
//! By default, whitespace is not trimmed.
//! This flag does not cause the parser to modify source text.
//! Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_trim_whitespace = 0x400;

//! Parse flag instructing the parser to condense all whitespace runs of data nodes to a single
//! space character. Trimming of leading and trailing whitespace of data is controlled by
//! xsxml::parse_trim_whitespace flag. By default, whitespace is not normalized. If this flag is
//! specified, source text will be modified. Can be combined with other flags by use of | operator.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_normalize_whitespace = 0x800;

//! Parse flag instructing the parser to convert html entity
//! this flag only works when the flag 'parse_no_entity_translation' not specified
//! <br><br>
//! See xml_document::parse() function.
static const int parse_html_entity_translation = 0x1000;

// Compound flags

//! Parse flags which represent default behaviour of the parser.
//! This is always equal to 0, so that all other flags can be simply ored together.
//! Normally there is no need to inconveniently disable flags by anding with their negated (~)
//! values. This also means that meaning of each flag is a <i>negation</i> of the default setting.
//! For example, if flag name is xsxml::parse_no_utf8, it means that utf-8 is <i>enabled</i> by
//! default, and using the flag will disable it. <br><br> See xml_document::parse() function.
static const int parse_default = 0;

//! A combination of parse flags that forbids any modifications of the source text.
//! This also results in faster parsing. However, note that the following will occur:
//! <ul>
//! <li>names and values of nodes will not be zero terminated, you have to use xml_base::name_size()
//! and xml_base::value_size() functions to determine where name and value ends</li> <li>entities
//! will not be translated</li> <li>whitespace will not be normalized</li>
//! </ul>
//! See xml_document::parse() function.
static const int parse_non_destructive = parse_no_string_terminators | parse_no_entity_translation;

//! A combination of parse flags resulting in fastest possible parsing, without sacrificing
//! important data. <br><br> See xml_document::parse() function.
static const int parse_fastest = parse_non_destructive | parse_no_data_nodes;

//! A combination of parse flags resulting in largest amount of data being extracted.
//! This usually results in slowest parsing.
//! <br><br>
//! See xml_document::parse() function.
static const int parse_full = parse_declaration_node | parse_comment_nodes | parse_doctype_node |
                              parse_pi_nodes | parse_validate_closing_tags;

static const int parse_normal = parse_no_data_nodes;

typedef char char_t;

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

namespace internal
{
//! \cond internal
// Struct that contains lookup tables for the parser
// It must be a template to allow correct linking (because it has static data members, which are
// defined in a header file).
template <int Dummy> struct lookup_tables
{
  static const unsigned char lookup_whitespace[256];        // Whitespace table
  static const unsigned char lookup_node_name[256];         // Node name table
  static const unsigned char lookup_text[256];              // Text table
  static const unsigned char lookup_text_pure_no_ws[256];   // Text table
  static const unsigned char lookup_text_pure_with_ws[256]; // Text table
  static const unsigned char lookup_attribute_name[256];    // Attribute name table
  static const unsigned char lookup_attribute_data_1[256]; // Attribute data table with single quote
  static const unsigned char
      lookup_attribute_data_1_pure[256]; // Attribute data table with single quote
  static const unsigned char
      lookup_attribute_data_2[256]; // Attribute data table with double quotes
  static const unsigned char
      lookup_attribute_data_2_pure[256];         // Attribute data table with double quotes
  static const unsigned char lookup_digits[256]; // Digits
  static const unsigned char
      lookup_upcase[256]; // To uppercase conversion table for ASCII characters
};

// Compare strings for equality
template <class _CharT>
inline bool compare(const _CharT* p1, std::size_t size1, const _CharT* p2, std::size_t size2,
                    bool case_sensitive)
{
  if (size1 != size2)
    return false;
  if (case_sensitive)
  {
    for (const _CharT* end = p1 + size1; p1 < end; ++p1, ++p2)
      if (*p1 != *p2)
        return false;
  }
  else
  {
    for (const _CharT* end = p1 + size1; p1 < end; ++p1, ++p2)
      if (lookup_tables<0>::lookup_upcase[static_cast<unsigned char>(*p1)] !=
          lookup_tables<0>::lookup_upcase[static_cast<unsigned char>(*p2)])
        return false;
  }
  return true;
}
} // namespace internal

//! Parse error exception.
//! This exception is thrown by the parser when an error occurs.
//! Use what() function to get human-readable error message.
//! Use where() function to get a pointer to position within source text where error was detected.
//! <br><br>
//! If throwing exceptions by the parser is undesirable,
//! it can be disabled by defining RAPIDXML_NO_EXCEPTIONS macro before xsxml.hpp is included.
//! This will cause the parser to call xsxml::parse_error_handler() function instead of throwing
//! an exception. This function must be defined by the user. <br><br> This class derives from
//! <code>std::exception</code> class.
class parse_error : public std::exception
{

public:
  //! Constructs parse error
  parse_error(const char* what, void* where) : m_what(what), m_where(where) {}

  //! Gets human readable description of error.
  //! \return Pointer to null terminated description of the error.
  virtual const char* what() const throw() { return m_what; }

  //! Gets pointer to character data where error happened.
  //! _CharT should be the same as char type of xml_document that produced the error.
  //! \return Pointer to location within the parsed string where error occured.
  template <class _CharT> _CharT* where() const { return reinterpret_cast<_CharT*>(m_where); }

private:
  const char* m_what;
  void* m_where;
};

//! This class represents root of the DOM hierarchy.
//! It is also an xml_node and a memory_pool through public inheritance.
//! Use parse() function to build a DOM tree from a zero-terminated XML text string.
//! parse() function allocates memory for nodes and attributes by using functions of xml_document,
//! which are inherited from memory_pool.
//! To access root node of the document, use the document itself, as if it was an xml_node.
//! \param char_t Character type to use.
class xml_sax3_parser
{
  xml_sax3_parse_cb* handler_;

  enum class parse_result
  {
    ok,
    expected_close_tag,
    unrecognized_tag,
  };
  parse_result parse_result_ = parse_result::ok;

public:
  template <int Flags = parse_normal>
  static parse_result parse(char_t* text, int length, xml_sax3_parse_cb* handler)
  {
    xml_sax3_parser parser(handler);
    parser.parse<Flags>(text, length);
    return parser.parse_result_;
  }

  //! Constructs empty XML document
  xml_sax3_parser(xml_sax3_parse_cb* handler) { handler_ = handler; }

  //! Parses zero-terminated XML string according to given flags.
  //! Passed string will be modified by the parser, unless xsxml::parse_non_destructive flag is
  //! used. The string must persist for the lifetime of the document. In case of error,
  //! xsxml::parse_error exception will be thrown. <br><br> If you want to parse contents of a
  //! file, you must first load the file into the memory, and pass pointer to its beginning. Make
  //! sure that data is zero-terminated. <br><br> Document can be parsed into multiple times. Each
  //! new call to parse removes previous nodes and attributes (if any), but does not clear memory
  //! pool. \param text XML data to parse; pointer is non-const to denote fact that this data may be
  //! modified by the parser.
  template <int Flags = parse_normal> void parse(char_t* text, int length)
  {
    assert(text);

    // save last character and make buffer zero-terminated (speeds up parsing)
    auto endch       = text[length - 1];
    text[length - 1] = 0;

    // Parse BOM, if any
    parse_bom(text);

    // Parse children
  _L_Loop : {
    // Skip whitespace before node
    skip<whitespace_pred, Flags>(text);
    if (*text == 0)
      goto _L_end;

    // Parse and append new child
    if (*text == char_t('<'))
    {
      ++text; // Skip '<'
      parse_node<Flags>(text);
    }
    else
      XSXML__PARSE_ERROR("expected <", text);

    goto _L_Loop;
  }

  _L_end:
    // check parse result.
    if (parse_result_ == parse_result::ok)
    {
      if (endch == '<')
      {
        parse_result_ = parse_result::unrecognized_tag;
        XSXML__PARSE_ERROR("unrecognized tag", text);
      }
    }
    else
    { // parse_result_: parse_result::expected_close_tag
      if (endch == '>')
        parse_result_ = parse_result::ok;
      else
        XSXML__PARSE_ERROR("expected >", text);
    }
  }

  //! Clears the document by deleting all nodes and clearing the memory pool.
  //! All nodes owned by document pool are destroyed.
  void clear()
  {
    // this->remove_all_nodes();
    // this->remove_all_attributes();
    // memory_pool<char_t>::clear();
  }

private:
  ///////////////////////////////////////////////////////////////////////
  // Internal character utility functions

  // Detect whitespace character
  struct whitespace_pred
  {
    static unsigned char test(char_t ch)
    {
      return internal::lookup_tables<0>::lookup_whitespace[static_cast<unsigned char>(ch)];
    }
  };

  // Detect node name character
  struct node_name_pred
  {
    static unsigned char test(char_t ch)
    {
      return internal::lookup_tables<0>::lookup_node_name[static_cast<unsigned char>(ch)];
    }
  };

  // Detect attribute name character
  struct attribute_name_pred
  {
    static unsigned char test(char_t ch)
    {
      return internal::lookup_tables<0>::lookup_attribute_name[static_cast<unsigned char>(ch)];
    }
  };

  // Detect text character (PCDATA)
  struct text_pred
  {
    static unsigned char test(char_t ch)
    {
      return internal::lookup_tables<0>::lookup_text[static_cast<unsigned char>(ch)];
    }
  };

  // Detect text character (PCDATA) that does not require processing
  struct text_pure_no_ws_pred
  {
    static unsigned char test(char_t ch)
    {
      return internal::lookup_tables<0>::lookup_text_pure_no_ws[static_cast<unsigned char>(ch)];
    }
  };

  // Detect text character (PCDATA) that does not require processing
  struct text_pure_with_ws_pred
  {
    static unsigned char test(char_t ch)
    {
      return internal::lookup_tables<0>::lookup_text_pure_with_ws[static_cast<unsigned char>(ch)];
    }
  };

  // Detect attribute value character
  template <char_t Quote> struct attribute_value_pred
  {
    static unsigned char test(char_t ch)
    {
      if (Quote == char_t('\''))
        return internal::lookup_tables<0>::lookup_attribute_data_1[static_cast<unsigned char>(ch)];
      if (Quote == char_t('\"'))
        return internal::lookup_tables<0>::lookup_attribute_data_2[static_cast<unsigned char>(ch)];
      return 0; // Should never be executed, to avoid warnings on Comeau
    }
  };

  // Detect attribute value character
  template <char_t Quote> struct attribute_value_pure_pred
  {
    static unsigned char test(char_t ch)
    {
      if (Quote == char_t('\''))
        return internal::lookup_tables<0>::lookup_attribute_data_1_pure[static_cast<unsigned char>(
            ch)];
      if (Quote == char_t('\"'))
        return internal::lookup_tables<0>::lookup_attribute_data_2_pure[static_cast<unsigned char>(
            ch)];
      return 0; // Should never be executed, to avoid warnings on Comeau
    }
  };

  // Insert coded character, using UTF8 or 8-bit ASCII
  template <int Flags> static void insert_coded_character(char_t*& text, unsigned long code)
  {
    if (Flags & parse_no_utf8)
    {
      // Insert 8-bit ASCII character
      // Todo: possibly verify that code is less than 256 and use replacement char otherwise?
      text[0] = static_cast<unsigned char>(code);
      text += 1;
    }
    else
    {
      // Insert UTF8 sequence
      if (code < 0x80) // 1 byte sequence
      {
        text[0] = static_cast<unsigned char>(code);
        text += 1;
      }
      else if (code < 0x800) // 2 byte sequence
      {
        text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[0] = static_cast<unsigned char>(code | 0xC0);
        text += 2;
      }
      else if (code < 0x10000) // 3 byte sequence
      {
        text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[0] = static_cast<unsigned char>(code | 0xE0);
        text += 3;
      }
      else if (code < 0x110000) // 4 byte sequence
      {
        text[3] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
        code >>= 6;
        text[0] = static_cast<unsigned char>(code | 0xF0);
        text += 4;
      }
      else // Invalid, only codes up to 0x10FFFF are allowed in Unicode
      {
        XSXML__PARSE_ERROR("invalid numeric character entity", text);
      }
    }
  }

  // Skip characters until predicate evaluates to true
  template <class StopPred, int Flags> static void skip(char_t*& text)
  {
    char_t* tmp = text;
    while (StopPred::test(*tmp))
      ++tmp;
    text = tmp;
  }

  // Skip characters until predicate evaluates to true while doing the following:
  // - replacing XML character entity references with proper characters (&apos; &amp; &quot; &lt;
  // &gt; &#...;)
  // - condensing whitespace sequences to single space character
  template <class StopPred, class StopPredPure, int Flags>
  static char_t* skip_and_expand_character_refs(char_t*& text)
  {
    // If entity translation, whitespace condense and whitespace trimming is disabled, use plain
    // skip
    if (Flags & parse_no_entity_translation && !(Flags & parse_normalize_whitespace) &&
        !(Flags & parse_trim_whitespace))
    {
      skip<StopPred, Flags>(text);
      return text;
    }

    // Use simple skip until first modification is detected
    skip<StopPredPure, Flags>(text);

    // Use translation skip
    char_t* src  = text;
    char_t* dest = src;
    while (StopPred::test(*src))
    {
      // If entity translation is enabled
      if (!(Flags & parse_no_entity_translation))
      {
        // Test if replacement is needed
        if (src[0] == char_t('&'))
        {
          switch (src[1])
          {

              // &amp; &apos;
            case char_t('a'):
              if (src[2] == char_t('m') && src[3] == char_t('p') && src[4] == char_t(';'))
              {
                *dest = char_t('&');
                ++dest;
                src += 5;
                continue;
              }
              if (src[2] == char_t('p') && src[3] == char_t('o') && src[4] == char_t('s') &&
                  src[5] == char_t(';'))
              {
                *dest = char_t('\'');
                ++dest;
                src += 6;
                continue;
              }
              break;

              // &quot;
            case char_t('q'):
              if (src[2] == char_t('u') && src[3] == char_t('o') && src[4] == char_t('t') &&
                  src[5] == char_t(';'))
              {
                *dest = char_t('"');
                ++dest;
                src += 6;
                continue;
              }
              break;

              // &gt;
            case char_t('g'):
              if (src[2] == char_t('t') && src[3] == char_t(';'))
              {
                *dest = char_t('>');
                ++dest;
                src += 4;
                continue;
              }
              break;

              // &lt;
            case char_t('l'):
              if (src[2] == char_t('t') && src[3] == char_t(';'))
              {
                *dest = char_t('<');
                ++dest;
                src += 4;
                continue;
              }
              break;

              // &#...; - assumes ASCII
            case char_t('#'):
              if (src[2] == char_t('x'))
              {
                unsigned long code = 0;
                src += 3; // Skip &#x
                while (1)
                {
                  unsigned char digit =
                      internal::lookup_tables<0>::lookup_digits[static_cast<unsigned char>(*src)];
                  if (digit == 0xFF)
                    break;
                  code = code * 16 + digit;
                  ++src;
                }
                insert_coded_character<Flags>(dest, code); // Put character in output
              }
              else
              {
                unsigned long code = 0;
                src += 2; // Skip &#
                while (1)
                {
                  unsigned char digit =
                      internal::lookup_tables<0>::lookup_digits[static_cast<unsigned char>(*src)];
                  if (digit == 0xFF)
                    break;
                  code = code * 10 + digit;
                  ++src;
                }
                insert_coded_character<Flags>(dest, code); // Put character in output
              }
              if (*src == char_t(';'))
                ++src;
              else
                XSXML__PARSE_ERROR("expected ;", src);
              continue;

              // Something else
            default:
              if (Flags & parse_html_entity_translation)
              {
                switch (src[1])
                { // &nbsp;
                  case char_t('n'):
                    if (src[2] == char_t('b') && src[3] == char_t('s') && src[4] == char_t('p') &&
                        src[5] == char_t(';'))
                    {
                      *dest = char_t(' ');
                      ++dest;
                      src += 6;
                      continue;
                    }
                    break;
                    // &emsp;
                  case char_t('e'):
                    if (src[2] == char_t('m') && src[3] == char_t('s') && src[4] == char_t('p') &&
                        src[5] == char_t(';'))
                    {
                      *dest = char_t(' ');
                      ++dest;
                      *dest = char_t(' ');
                      ++dest;
                      src += 6;
                      continue;
                    }
                    break;
                }
              }
              // Ignore, just copy '&' verbatim
              break;
          }
        }
      }

      // If whitespace condensing is enabled
      if (Flags & parse_normalize_whitespace)
      {
        // Test if condensing is needed
        if (whitespace_pred::test(*src))
        {
          *dest = char_t(' ');
          ++dest; // Put single space in dest
          ++src;  // Skip first whitespace char
          // Skip remaining whitespace chars
          while (whitespace_pred::test(*src))
            ++src;
          continue;
        }
      }

      // No replacement, only copy character
      *dest++ = *src++;
    }

    // Return new end
    text = src;
    return dest;
  }

  ///////////////////////////////////////////////////////////////////////
  // Internal parsing functions

  // Parse UTF-8 BOM, if any
  inline void parse_bom(char*& text)
  {
    if (static_cast<unsigned char>(text[0]) == 0xEF &&
        static_cast<unsigned char>(text[1]) == 0xBB && static_cast<unsigned char>(text[2]) == 0xBF)
    {
      text += 3;
    }
  }

  // Parse UTF-16/32 BOM, if any
  inline void parse_bom(wchar_t*& text)
  {
    const wchar_t bom = 0xFEFF;
    if (text[0] == bom)
    {
      ++text;
    }
  }
  // Parse XML declaration (<?xml...)
  template <int Flags> void parse_xml_declaration(char_t*& text)
  {
    // If parsing of declaration is disabled
    if (!(Flags & parse_declaration_node))
    {
      // Skip until end of declaration
      while (text[0] != char_t('?') || text[1] != char_t('>'))
      {
        if (!text[0])
          XSXML__PARSE_ERROR("unexpected end of data", text);
        ++text;
      }
      text += 2; // Skip '?>'
      return;    // return 0;
    }

    // Create declaration
    // xml_node<char_t> *declaration = this->allocate_node(node_declaration);

    // Skip whitespace before attributes or ?>
    skip<whitespace_pred, Flags>(text);

    // Parse declaration attributes
    parse_node_attributes<Flags>(text /*, declaration*/);

    // Skip ?>
    if (text[0] != char_t('?') || text[1] != char_t('>'))
      XSXML__PARSE_ERROR("expected ?>", text);
    text += 2;

    // return declaration;
  }

  // Parse XML comment (<!--...)
  template <int Flags> void parse_comment(char_t*& text)
  {
    // If parsing of comments is disabled
    if (!(Flags & parse_comment_nodes))
    {
      // Skip until end of comment
      while (text[0] != char_t('-') || text[1] != char_t('-') || text[2] != char_t('>'))
      {
        if (!text[0])
          XSXML__PARSE_ERROR("unexpected end of data", text);
        ++text;
      }
      text += 3; // Skip '-->'
      return;    // return 0;      // Do not produce comment node
    }

    // Skip until end of comment
    while (text[0] != char_t('-') || text[1] != char_t('-') || text[2] != char_t('>'))
    {
      if (!text[0])
        XSXML__PARSE_ERROR("unexpected end of data", text);
      ++text;
    }

    // Create comment node
    // xml_node<char_t> *comment = this->allocate_node(node_comment);
    // comment->value(value, text - value); // TODO: DNT implement comment

    // Place zero terminator after comment value
    if (!(Flags & parse_no_string_terminators))
      *text = char_t('\0');

    text += 3; // Skip '-->'
    return;
  }

  // Parse DOCTYPE
  template <int Flags> void parse_doctype(char_t*& text)
  {
    // Skip to >
    while (*text != char_t('>'))
    {
      // Determine character type
      switch (*text)
      {

          // If '[' encountered, scan for matching ending ']' using naive algorithm with depth
          // This works for all W3C test files except for 2 most wicked
        case char_t('['): {
          ++text; // Skip '['
          int depth = 1;
          while (depth > 0)
          {
            switch (*text)
            {
              case char_t('['):
                ++depth;
                break;
              case char_t(']'):
                --depth;
                break;
              case 0:
                XSXML__PARSE_ERROR("unexpected end of data", text);
              default:
                break;
            }
            ++text;
          }
          break;
        }

        // Error on end of text
        case char_t('\0'):
          XSXML__PARSE_ERROR("unexpected end of data", text);

          // Other character, skip it
        default:
          ++text;
      }
    }

    // If DOCTYPE nodes enabled
    if (Flags & parse_doctype_node)
    { // SAX3: ignore doctype node
      // Place zero terminator after value
      if (!(Flags & parse_no_string_terminators))
        *text = char_t('\0');

      text += 1; // skip '>'

      return; // return doctype;
    }
    else
    {
      text += 1; // skip '>'
      return;    // return 0;
    }
  }

  // Parse PI
  template <int Flags> void parse_pi(char_t*& text)
  {
    // If creation of PI nodes is enabled
    if (Flags & parse_pi_nodes)
    {
      // Create pi node
      // xml_node<char_t> *pi = this->allocate_node(node_pi);

      // Extract PI target name
      char_t* name = text;
      skip<node_name_pred, Flags>(text);
      if (text == name)
        XSXML__PARSE_ERROR("expected PI target", text);
      // pi->name(name, text - name); // TODO: DNT notify for pi

      // Skip whitespace between pi target and pi
      skip<whitespace_pred, Flags>(text);

      // Skip to '?>'
      while (text[0] != char_t('?') || text[1] != char_t('>'))
      {
        if (*text == char_t('\0'))
          XSXML__PARSE_ERROR("unexpected end of data", text);
        ++text;
      }

      text += 2; // Skip '?>'
      return;    // return pi;
    }
    else
    {
      // Skip to '?>'
      while (text[0] != char_t('?') || text[1] != char_t('>'))
      {
        if (*text == char_t('\0'))
          XSXML__PARSE_ERROR("unexpected end of data", text);
        ++text;
      }
      text += 2; // Skip '?>'
      return;    // return 0;
    }
  }

  // Parse and append data
  // Return character that ends data.
  // This is necessary because this character might have been overwritten by a terminating 0
  template <int Flags>
  char_t parse_and_append_data(/*const string_view& elementName unused for SAX,*/ char_t*& text,
                               char_t* contents_start)
  {
    // Backup to contents start if whitespace trimming is disabled
    if (!(Flags & parse_trim_whitespace))
      text = contents_start;

    // Skip until end of data
    char_t *value = text, *end;
    if (Flags & parse_normalize_whitespace)
      end = skip_and_expand_character_refs<text_pred, text_pure_with_ws_pred, Flags>(text);
    else
      end = skip_and_expand_character_refs<text_pred, text_pure_no_ws_pred, Flags>(text);

    // Trim trailing whitespace if flag is set; leading was already trimmed by whitespace skip after
    // >
    if (Flags & parse_trim_whitespace)
    {
      if (Flags & parse_normalize_whitespace)
      {
        // Whitespace is already condensed to single space characters by skipping function, so just
        // trim 1 char off the end
        if (*(end - 1) == char_t(' '))
          --end;
      }
      else
      {
        // Backup until non-whitespace character is found
        while (whitespace_pred::test(*(end - 1)))
          --end;
      }
    }

    char_t ch = *text;
    // Place zero terminator after value
    if (!(Flags & parse_no_string_terminators))
    {
      // char_t ch = *text;
      *end = char_t('\0');
      // return ch;      // Return character that ends data; this is required because zero
      // terminator overwritten it
    }

    handler_->xml_text_cb(value, end - value);

    // Return character that ends data
    return ch;
  }

  // Parse CDATA
  template <int Flags> void parse_cdata(char_t*& text)
  {
    // If CDATA is disabled
    if (Flags & parse_no_data_nodes)
    {
      // Skip until end of cdata
      while (text[0] != char_t(']') || text[1] != char_t(']') || text[2] != char_t('>'))
      {
        if (!text[0])
          XSXML__PARSE_ERROR("unexpected end of data", text);
        ++text;
      }
      text += 3; // Skip ]]>
      return;    // return 0;       // Do not produce CDATA node
    }

    // Skip until end of cdata
    while (text[0] != char_t(']') || text[1] != char_t(']') || text[2] != char_t('>'))
    {
      if (!text[0])
        XSXML__PARSE_ERROR("unexpected end of data", text);
      ++text;
    }

    // Place zero terminator after value
    if (!(Flags & parse_no_string_terminators))
      *text = char_t('\0');

    text += 3; // Skip ]]>
    return;    // return cdata;
  }

  // Parse element node
  template <int Flags> void parse_element(char_t*& text)
  {
    // Create element node
    // xml_node<char_t> *element = this->allocate_node(node_element);

    // Extract element name
    auto mark = text;
    skip<node_name_pred, Flags>(text);
    size_t n = text - mark;
    if (n == 0)
      XSXML__PARSE_ERROR("expected element name", text);

    // Skip whitespace between element name and attributes or >
    skip<whitespace_pred, Flags>(text);

    auto chTmp = *text;
    // Place zero terminator after name
    if (!(Flags & parse_no_string_terminators))
      mark[n] = (char_t)'\0';

    // Notify start element
    handler_->xml_start_element_cb(mark, n); // <abc>

    // Parse attributes, if any
    if (chTmp != '>' && chTmp != char_t('/'))
    {
      parse_node_attributes<Flags>(text);
      chTmp = *text;
    }

    // Notify end attr
    handler_->xml_end_attr_cb();

    // Determine ending type
    if (chTmp == char_t('>'))
    {
      ++text;
      parse_node_contents<Flags>(text, mark, n);
    }
    else if (chTmp == char_t('/'))
    {
      ++text;
      if (*text != char_t('>'))
      {
        parse_result_ = parse_result::expected_close_tag;
        if (*text != 0)
          XSXML__PARSE_ERROR("expected >", text);
      }
      else
        ++text;
    }
    else
    {
      if (chTmp != 0)
      {
        parse_result_ = parse_result::expected_close_tag;
        XSXML__PARSE_ERROR("expected >", text);
      } // else, parse to eof
    }

    // Return parsed element
    handler_->xml_end_element_cb(mark, n);
    // return element;
  }

  // Determine node type, and parse it
  template <int Flags> void parse_node(char_t*& text)
  {
    // Parse proper node type
    switch (text[0])
    {

        // <...
      default:
        // Parse and append element node
        return parse_element<Flags>(text);

        // <?...
      case char_t('?'):
        ++text; // Skip ?
        if ((text[0] == char_t('x') || text[0] == char_t('X')) &&
            (text[1] == char_t('m') || text[1] == char_t('M')) &&
            (text[2] == char_t('l') || text[2] == char_t('L')) && whitespace_pred::test(text[3]))
        {
          // '<?xml ' - xml declaration
          text += 4; // Skip 'xml '
          return parse_xml_declaration<Flags>(text);
        }
        else
        {
          // Parse PI
          return parse_pi<Flags>(text);
        }

        // <!...
      case char_t('!'):

        // Parse proper subset of <! node
        switch (text[1])
        {

            // <!-
          case char_t('-'):
            if (text[2] == char_t('-'))
            {
              // '<!--' - xml comment
              text += 3; // Skip '!--'
              return parse_comment<Flags>(text);
            }
            break;

            // <![
          case char_t('['):
            if (text[2] == char_t('C') && text[3] == char_t('D') && text[4] == char_t('A') &&
                text[5] == char_t('T') && text[6] == char_t('A') && text[7] == char_t('['))
            {
              // '<![CDATA[' - cdata
              text += 8; // Skip '![CDATA['
              return parse_cdata<Flags>(text);
            }
            break;

            // <!D
          case char_t('D'):
            if (text[2] == char_t('O') && text[3] == char_t('C') && text[4] == char_t('T') &&
                text[5] == char_t('Y') && text[6] == char_t('P') && text[7] == char_t('E') &&
                whitespace_pred::test(text[8]))
            {
              // '<!DOCTYPE ' - doctype
              text += 9; // skip '!DOCTYPE '
              return parse_doctype<Flags>(text);
            }
            break;

          default:
            break;
        } // switch

        // Attempt to skip other, unrecognized node types starting with <!
        ++text; // Skip !
        while (*text != char_t('>'))
        {
          if (*text == 0)
            XSXML__PARSE_ERROR("unexpected end of data", text);
          ++text;
        }
        ++text; // Skip '>'
        return; // return 0;   // No node recognized
    }
  }

  // Parse contents of the node - children, data etc.
  template <int Flags> void parse_node_contents(char_t*& text, const char_t* mark, size_t n)
  {
    // For all children and text
    while (1)
    {
      // Skip whitespace between > and node contents
      char_t* contents_start = text; // Store start of node contents before whitespace is skipped
      skip<whitespace_pred, Flags>(text);
      char_t next_char = *text;

      // After data nodes, instead of continuing the loop, control jumps here.
      // This is because zero termination inside parse_and_append_data() function
      // would wreak havoc with the above code.
      // Also, skipping whitespace after data nodes is unnecessary.
    after_data_node:

      // Determine what comes next: node closing, child node, data node, or 0?
      switch (next_char)
      {

          // Node closing or child node
        case char_t('<'):
          if (text[1] == char_t('/'))
          {
            // Node closing
            text += 2; // Skip '</'
            if (Flags & parse_validate_closing_tags)
            {
              // Skip and validate closing tag name
              char_t* closing_name = text;
              skip<node_name_pred, Flags>(text);
              if (!internal::compare(mark, n, closing_name, text - closing_name, true))
                XSXML__PARSE_ERROR("invalid closing tag name", text);
            }
            else
              skip<node_name_pred, Flags>(text); // No validation, just skip name

            // Skip remaining whitespace after node name
            skip<whitespace_pred, Flags>(text);
            if (*text != char_t('>'))
            {
              parse_result_ = parse_result::expected_close_tag;
              if (*text != 0)
                XSXML__PARSE_ERROR("expected >", text);
            }
            else
              ++text; // Skip '>'
            return;   // Node closed, finished parsing contents
          }
          else
          {
            // Child node
            ++text; // Skip '<'
            parse_node<Flags>(text);
          }
          break;

          // End of data - error
        case char_t('\0'):
          XSXML__PARSE_ERROR("unexpected end of data", text);

          // Data node
        default:
          next_char = parse_and_append_data<Flags>(/*elementName, */ text, contents_start);
          goto after_data_node; // Bypass regular processing after data nodes
      }
    }
  }

  // Parse XML attributes of the node
  template <int Flags> void parse_node_attributes(char_t*& text)
  {
    // For all attributes
    while (attribute_name_pred::test(*text))
    {
      // Extract attribute name
      char_t* name = text;
      ++text; // Skip first character of attribute name
      skip<attribute_name_pred, Flags>(text);
      if (text == name)
        XSXML__PARSE_ERROR("expected attribute name", name);

      // Create new attribute
      // xml_attribute<char_t> *attribute = this->allocate_attribute();
      // attribute->name(name, text - name);
      auto namesize = text - name;
      // node->append_attribute(attribute);

      // Skip whitespace after attribute name
      skip<whitespace_pred, Flags>(text);

      // Skip =
      if (*text != char_t('='))
        XSXML__PARSE_ERROR("expected =", text);
      ++text;

      // Add terminating zero after name
      if (!(Flags & parse_no_string_terminators))
        name[namesize] = 0;

      // Skip whitespace after =
      skip<whitespace_pred, Flags>(text);

      // Skip quote and remember if it was ' or "
      char_t quote = *text;
      if (quote != char_t('\'') && quote != char_t('"'))
        XSXML__PARSE_ERROR("expected ' or \"", text);
      ++text;

      // Extract attribute value and expand char refs in it
      char_t *value = text, *end;
      const int AttFlags =
          Flags & ~parse_normalize_whitespace; // No whitespace normalization in attributes
      if (quote == char_t('\''))
        end =
            skip_and_expand_character_refs<attribute_value_pred<char_t('\'')>,
                                           attribute_value_pure_pred<char_t('\'')>, AttFlags>(text);
      else
        end =
            skip_and_expand_character_refs<attribute_value_pred<char_t('"')>,
                                           attribute_value_pure_pred<char_t('"')>, AttFlags>(text);

      // Set attribute value
      // attribute->value(value, end - value);
      auto valuesize = end - value;

      // Make sure that end quote is present
      if (*text != quote)
        XSXML__PARSE_ERROR("expected ' or \"", text);
      ++text; // Skip quote

      // Add terminating zero after value
      if (!(Flags & parse_no_string_terminators))
        value[valuesize] = 0;

      handler_->xml_attr_cb(name, namesize, value, valuesize);

      // Skip whitespace after attribute value
      skip<whitespace_pred, Flags>(text);
    }
  }

}; /* CLASS xml_sax3_parser */

//! \cond internal
namespace internal
{
// Whitespace (space \n \r \t)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_whitespace[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, // 0
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 1
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 3
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // C
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // D
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // E
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // F
};

// Node name (anything but space \n \r \t / > ? \0)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_node_name[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Text (i.e. PCDATA) (anything but < \0)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_text[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Text (i.e. PCDATA) that does not require processing when ws normalization is disabled
// (anything but < \0 &)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_text_pure_no_ws[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Text (i.e. PCDATA) that does not require processing when ws normalizationis is enabled
// (anything but < \0 & space \n \r \t)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_text_pure_with_ws[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Attribute name (anything but space \n \r \t / < > = ? ! \0)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_attribute_name[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Attribute data with single quote (anything but ' \0)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_attribute_data_1[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Attribute data with single quote that does not require processing (anything but ' \0 &)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_attribute_data_1_pure[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Attribute data with double quote (anything but " \0)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_attribute_data_2[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Attribute data with double quote that does not require processing (anything but " \0 &)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_attribute_data_2_pure[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
    1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // A
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // B
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // C
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // D
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // E
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // F
};

// Digits (dec and hex, 255 denotes end of numeric character reference)
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_digits[256] = {
    // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 0
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 1
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 2
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   255, 255, 255, 255, 255, 255, // 3
    255, 10,  11,  12,  13,  14,  15,  255, 255, 255, 255, 255, 255, 255, 255, 255, // 4
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 5
    255, 10,  11,  12,  13,  14,  15,  255, 255, 255, 255, 255, 255, 255, 255, 255, // 6
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 7
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 8
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // 9
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // A
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // B
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // C
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // D
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, // E
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255  // F
};

// Upper case conversion
template <int Dummy>
const unsigned char lookup_tables<Dummy>::lookup_upcase[256] = {
    // 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  A   B   C   D   E   F
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,  // 0
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  // 1
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  // 2
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  // 3
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  // 4
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  // 5
    96,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  // 6
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  123, 124, 125, 126, 127, // 7
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, // 8
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, // 9
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, // A
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, // B
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, // C
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, // D
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, // E
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255  // F
};
} // namespace internal
  //! \endcond
} // namespace xsxml

// Undefine internal macros
#undef XSXML__PARSE_ERROR

// On MSVC, restore warnings state
#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#endif
