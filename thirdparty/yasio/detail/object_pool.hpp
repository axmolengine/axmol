//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any 
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)

Copyright (c) 2012-2021 HALX99

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
// object_pool.hpp: a simple & high-performance object pool implementation v1.3.3
#ifndef YASIO__OBJECT_POOL_HPP
#define YASIO__OBJECT_POOL_HPP

#include <assert.h>
#include <stdlib.h>
#include <memory>
#include <mutex>
#include <type_traits>

#define OBJECT_POOL_DECL inline

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4200)
#endif

namespace yasio
{
namespace gc
{
#define YASIO_POOL_FL_BEGIN(chunk) reinterpret_cast<free_link_node*>(chunk->data)
#define YASIO_POOL_PREALLOCATE 1

template <typename _Ty> static size_t aligned_storage_size()
{
  return sizeof(typename std::aligned_storage<sizeof(_Ty), std::alignment_of<_Ty>::value>::type);
}

namespace detail
{
class object_pool
{
  typedef struct free_link_node
  {
    free_link_node* next;
  } * free_link;

  typedef struct chunk_link_node
  {
    chunk_link_node* next;
    char data[0];
  } * chunk_link;

  object_pool(const object_pool&) = delete;
  void operator=(const object_pool&) = delete;

public:
  OBJECT_POOL_DECL object_pool(size_t element_size, size_t element_count)
      : free_link_(nullptr), chunk_(nullptr), element_size_(element_size),
        element_count_(element_count)
  {
#if YASIO_POOL_PREALLOCATE
    release(allocate_from_process_heap()); // preallocate 1 chunk
#endif
  }

  OBJECT_POOL_DECL virtual ~object_pool(void) { this->purge(); }

  OBJECT_POOL_DECL void purge(void)
  {
    if (this->chunk_ == nullptr)
      return;

    chunk_link_node *p, **q = &this->chunk_;
    while ((p = *q) != nullptr)
    {
      *q = p->next;
      free(p);
    }

    free_link_ = nullptr;
  }

  OBJECT_POOL_DECL void cleanup(void)
  {
    if (this->chunk_ == nullptr)
    {
      return;
    }

    chunk_link_node* chunk  = this->chunk_;
    free_link_node* linkend = this->tidy_chunk(chunk);

    while ((chunk = chunk->next) != nullptr)
    {
      linkend->next = YASIO_POOL_FL_BEGIN(chunk);

      linkend = this->tidy_chunk(chunk);
    }

    linkend->next = nullptr;

    this->free_link_ = YASIO_POOL_FL_BEGIN(this->chunk_);
  }

  OBJECT_POOL_DECL void* get(void)
  {
    if (this->free_link_ != nullptr)
    {
      return allocate_from_chunk();
    }

    return allocate_from_process_heap();
  }

  OBJECT_POOL_DECL void release(void* _Ptr)
  {
    free_link_node* ptr = reinterpret_cast<free_link_node*>(_Ptr);
    ptr->next           = this->free_link_;
    this->free_link_    = ptr;
  }

private:
  OBJECT_POOL_DECL void* allocate_from_chunk(void)
  {
    free_link_node* ptr = this->free_link_;
    this->free_link_    = ptr->next;
    return reinterpret_cast<void*>(ptr);
  }
  OBJECT_POOL_DECL void* allocate_from_process_heap(void)
  {
    chunk_link new_chunk =
        (chunk_link)malloc(sizeof(chunk_link_node) + element_size_ * element_count_);
#ifdef _DEBUG
    ::memset(new_chunk, 0x00, sizeof(chunk_link_node));
#endif
    tidy_chunk(new_chunk)->next = nullptr;

    // link the new_chunk
    new_chunk->next = this->chunk_;
    this->chunk_    = new_chunk;

    // allocate 1 object
    auto ptr         = YASIO_POOL_FL_BEGIN(new_chunk);
    this->free_link_ = ptr->next;

    return reinterpret_cast<void*>(ptr);
  }

  OBJECT_POOL_DECL free_link_node* tidy_chunk(chunk_link chunk)
  {
    char* rbegin = chunk->data + (element_count_ - 1) * element_size_;

    for (char* ptr = chunk->data; ptr < rbegin; ptr += element_size_)
    {
      reinterpret_cast<free_link_node*>(ptr)->next =
          reinterpret_cast<free_link_node*>(ptr + element_size_);
    }
    return reinterpret_cast<free_link_node*>(rbegin);
  }

private:
  free_link free_link_; // link to free head
  chunk_link chunk_;    // chunk link
  const size_t element_size_;
  const size_t element_count_;
};
} // namespace detail

template <typename _Ty, typename _Mutex = std::mutex> class object_pool : public detail::object_pool
{
public:
  object_pool(size_t _ElemCount = 512)
      : detail::object_pool(yasio::gc::aligned_storage_size<_Ty>(), _ElemCount)
  {}

  template <typename... _Types> _Ty* construct(_Types&&... args)
  {
    return new (allocate()) _Ty(std::forward<_Types>(args)...);
  }

  void destroy(void* _Ptr)
  {
    ((_Ty*)_Ptr)->~_Ty(); // call the destructor
    release(_Ptr);
  }

  void* allocate()
  {
    std::lock_guard<_Mutex> lk(this->mutex_);
    return get();
  }

  void deallocate(void* _Ptr)
  {
    std::lock_guard<_Mutex> lk(this->mutex_);
    release(_Ptr);
  }

  _Mutex mutex_;
};

template <typename _Ty> class object_pool<_Ty, void> : public detail::object_pool
{
  object_pool(const object_pool&) = delete;
  void operator=(const object_pool&) = delete;

public:
  object_pool(size_t _ElemCount = 512)
      : detail::object_pool(yasio::gc::aligned_storage_size<_Ty>(), _ElemCount)
  {}

  template <typename... _Types> _Ty* construct(_Types&&... args)
  {
    return new (allocate()) _Ty(std::forward<_Types>(args)...);
  }

  void destroy(void* _Ptr)
  {
    ((_Ty*)_Ptr)->~_Ty(); // call the destructor
    release(_Ptr);
  }

  void* allocate() { return get(); }

  void deallocate(void* _Ptr) { release(_Ptr); }
};

#define DEFINE_OBJECT_POOL_ALLOCATION(ELEMENT_TYPE, ELEMENT_COUNT)                                 \
public:                                                                                            \
  static void* operator new(size_t /*size*/) { return get_pool().allocate(); }                     \
                                                                                                   \
  static void* operator new(size_t /*size*/, std::nothrow_t) { return get_pool().allocate(); }     \
                                                                                                   \
  static void operator delete(void* p) { get_pool().deallocate(p); }                               \
                                                                                                   \
  static yasio::gc::object_pool<ELEMENT_TYPE, void>& get_pool()                                    \
  {                                                                                                \
    static yasio::gc::object_pool<ELEMENT_TYPE, void> s_pool(ELEMENT_COUNT);                       \
    return s_pool;                                                                                 \
  }

// The thread safe edition
#define DEFINE_CONCURRENT_OBJECT_POOL_ALLOCATION(ELEMENT_TYPE, ELEMENT_COUNT)                      \
public:                                                                                            \
  static void* operator new(size_t /*size*/) { return get_pool().allocate(); }                     \
                                                                                                   \
  static void* operator new(size_t /*size*/, std::nothrow_t) { return get_pool().allocate(); }     \
                                                                                                   \
  static void operator delete(void* p) { get_pool().deallocate(p); }                               \
                                                                                                   \
  static yasio::gc::object_pool<ELEMENT_TYPE, std::mutex>& get_pool()                              \
  {                                                                                                \
    static yasio::gc::object_pool<ELEMENT_TYPE, std::mutex> s_pool(ELEMENT_COUNT);                 \
    return s_pool;                                                                                 \
  }

//////////////////////// allocator /////////////////
// TEMPLATE CLASS object_pool_allocator, can't used by std::vector, DO NOT use at non-msvc compiler.
template <class _Ty, size_t _ElemCount = 512, class _Mutex = void> class object_pool_allocator
{ // generic allocator for objects of class _Ty
public:
  typedef _Ty value_type;

  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type* const_pointer;
  typedef const value_type& const_reference;

  typedef size_t size_type;
#ifdef _WIN32
  typedef ptrdiff_t difference_type;
#else
  typedef long difference_type;
#endif

  template <class _Other> struct rebind
  { // convert this type to _ALLOCATOR<_Other>
    typedef object_pool_allocator<_Other> other;
  };

  pointer address(reference _Val) const
  { // return address of mutable _Val
    return ((pointer) & (char&)_Val);
  }

  const_pointer address(const_reference _Val) const
  { // return address of nonmutable _Val
    return ((const_pointer) & (const char&)_Val);
  }

  object_pool_allocator() throw()
  { // construct default allocator (do nothing)
  }

  object_pool_allocator(const object_pool_allocator<_Ty>&) throw()
  { // construct by copying (do nothing)
  }

  template <class _Other> object_pool_allocator(const object_pool_allocator<_Other>&) throw()
  { // construct from a related allocator (do nothing)
  }

  template <class _Other>
  object_pool_allocator<_Ty>& operator=(const object_pool_allocator<_Other>&)
  { // assign from a related allocator (do nothing)
    return (*this);
  }

  void deallocate(pointer _Ptr, size_type)
  { // deallocate object at _Ptr, ignore size
    _Spool().release(_Ptr);
  }

  pointer allocate(size_type count)
  { // allocate array of _Count elements
    assert(count == 1);
    (void)count;
    return static_cast<pointer>(_Spool().get());
  }

  pointer allocate(size_type count, const void*)
  { // allocate array of _Count elements, not support, such as std::vector
    return allocate(count);
  }

  void construct(_Ty* _Ptr)
  { // default construct object at _Ptr
    ::new ((void*)_Ptr) _Ty();
  }

  void construct(pointer _Ptr, const _Ty& _Val)
  { // construct object at _Ptr with value _Val
    new (_Ptr) _Ty(_Val);
  }

  void construct(pointer _Ptr, _Ty&& _Val)
  { // construct object at _Ptr with value _Val
    new ((void*)_Ptr) _Ty(std::forward<_Ty>(_Val));
  }

  template <class _Other> void construct(pointer _Ptr, _Other&& _Val)
  { // construct object at _Ptr with value _Val
    new ((void*)_Ptr) _Ty(std::forward<_Other>(_Val));
  }

  template <class _Objty, class... _Types> void construct(_Objty* _Ptr, _Types&&... _Args)
  { // construct _Objty(_Types...) at _Ptr
    ::new ((void*)_Ptr) _Objty(std::forward<_Types>(_Args)...);
  }

  template <class _Uty> void destroy(_Uty* _Ptr)
  { // destroy object at _Ptr, do nothing
    _Ptr->~_Uty();
  }

  size_type max_size() const throw()
  { // estimate maximum array size
    size_type _Count = (size_type)(-1) / sizeof(_Ty);
    return (0 < _Count ? _Count : 1);
  }

  static object_pool<_Ty, _Mutex>& _Spool()
  {
    static object_pool<_Ty, _Mutex> s_pool(_ElemCount);
    return s_pool;
  }
};

template <class _Ty, class _Other>
inline bool operator==(const object_pool_allocator<_Ty>&,
                       const object_pool_allocator<_Other>&) throw()
{ // test for allocator equality
  return (true);
}

template <class _Ty, class _Other>
inline bool operator!=(const object_pool_allocator<_Ty>& _Left,
                       const object_pool_allocator<_Other>& _Right) throw()
{ // test for allocator inequality
  return (!(_Left == _Right));
}

} // namespace gc
} // namespace yasio

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif
