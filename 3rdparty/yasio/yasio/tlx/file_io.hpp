//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2025 HALX99

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

#pragma once

#include "yasio/tlx/string.hpp"
#include "yasio/tlx/string_view.hpp"
#include <fstream>

namespace tlx
{
inline tlx::string read_text_file(std::string_view file_path)
{
  std::ifstream fin(file_path.data(), std::ios_base::binary);
  if (fin.is_open())
  {
    fin.seekg(std::ios_base::end);
    auto n = static_cast<size_t>(fin.tellg());
    if (n > 0)
    {
      tlx::string ret;
      ret.resize_and_overwrite(n, [&fin](char* out, size_t outlen) {
        fin.seekg(std::ios_base::beg);
        fin.read(out, outlen);
        return outlen;
      });
      return ret;
    }
  }
  return tlx::string{};
}

inline bool is_regular_file(const char* path)
{
  struct stat st;
  return (::stat(path, &st) == 0) && (st.st_mode & S_IFREG);
}

} // namespace tlx
