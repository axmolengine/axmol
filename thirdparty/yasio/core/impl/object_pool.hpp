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
// core/impl/object_pool.hpp: a simple & high-performance object pool implementation v1.3.5
#pragma once

#include <assert.h>
#include <stdlib.h>
#include <cstddef>
#include <memory>
#include <mutex>
#include <type_traits>

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4200)
#endif

#define OBJECT_POOL_DECL inline

namespace yasio
{
namespace detail
{
template <typename _Ty>
struct aligned_storage_size {
  static const size_t value = sizeof(typename std::aligned_storage<sizeof(_Ty)>::type);
};

class object_pool {
#if defined(_DEBUG)
  typedef struct free_link_node {
    free_link_node* next;
  }* free_link;
#endif
  typedef struct chunk_link_node {
    union {
      chunk_link_node* next;
      char padding[sizeof(std::max_align_t)];
    };
    char data[0];
  }* chunk_link;

  object_pool(const object_pool&)    = delete;
  void operator=(const object_pool&) = delete;

public:
  OBJECT_POOL_DECL object_pool(size_t element_size, size_t element_count)
      : first_(nullptr), chunk_(nullptr), element_size_(element_size), element_count_(element_count)
  {
    release(allocate_from_process_heap()); // preallocate 1 chunk
  }

  OBJECT_POOL_DECL object_pool(size_t element_size, size_t element_count, std::false_type /*preallocate?*/)
      : first_(nullptr), chunk_(nullptr), element_size_(element_size), element_count_(element_count)
  {}

  OBJECT_POOL_DECL virtual ~object_pool(void) { this->purge(); }

  OBJECT_POOL_DECL void purge(void)
  {
    if (this->chunk_ == nullptr)
      return;

    chunk_link_node *p, **q = &this->chunk_;
    while ((p = *q) != nullptr)
    {
      *q = p->next;
      delete[] (uint8_t*)(p);
    }

    first_ = nullptr;
  }

  OBJECT_POOL_DECL void cleanup(void)
  {
    if (this->chunk_ == nullptr)
      return;

    chunk_link_node* chunk = this->chunk_;
    void* last             = this->tidy_chunk(chunk);

    while ((chunk = chunk->next) != nullptr)
    {
      nextof(last) = firstof(chunk);
      last         = this->tidy_chunk(chunk);
    }

    nextof(last) = nullptr;

    first_ = firstof(this->chunk_);
  }

  OBJECT_POOL_DECL void* get(void) { return (first_ != nullptr) ? allocate_from_chunk(first_) : allocate_from_process_heap(); }

  OBJECT_POOL_DECL void release(void* _Ptr)
  {
    nextof(_Ptr) = first_;
    first_       = _Ptr;
  }

private:
  OBJECT_POOL_DECL void* allocate_from_chunk(void* current)
  {
    first_ = nextof(current);
    return current;
  }
  OBJECT_POOL_DECL static void* firstof(chunk_link chunk) { return chunk->data; }
  OBJECT_POOL_DECL static void*& nextof(void* const ptr) { return *(static_cast<void**>(ptr)); }
  OBJECT_POOL_DECL void* allocate_from_process_heap(void)
  {
    chunk_link new_chunk = (chunk_link) new uint8_t[sizeof(chunk_link_node) + element_size_ * element_count_];
#ifdef _DEBUG
    ::memset(new_chunk, 0x00, sizeof(chunk_link_node));
#endif
    nextof(tidy_chunk(new_chunk)) = nullptr;

    // link the new_chunk
    new_chunk->next = this->chunk_;
    this->chunk_    = new_chunk;

    // allocate 1 object
    return allocate_from_chunk(firstof(new_chunk));
  }

  OBJECT_POOL_DECL void* tidy_chunk(chunk_link chunk)
  {
    char* last = chunk->data + (element_count_ - 1) * element_size_;
    for (char* ptr = chunk->data; ptr < last; ptr += element_size_)
      nextof(ptr) = (ptr + element_size_);
    return last;
  }

private:
  void* first_;      // link to free head
  chunk_link chunk_; // chunk link
  const size_t element_size_;
  const size_t element_count_;
};
} // namespace detail
} // namespace yasio

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
