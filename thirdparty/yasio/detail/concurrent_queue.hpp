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
#ifndef YASIO__CONCURRENT_QUEUE_HPP
#define YASIO__CONCURRENT_QUEUE_HPP

#include "yasio/detail/config.hpp"
#if defined(YASIO_USE_SPSC_QUEUE)
#  include "moodycamel/readerwriterqueue.h"
#else
#  include <queue>
#endif

namespace yasio
{
template <typename _Ty> inline void clear_queue(_Ty& queue)
{
  _Ty tmp;
  std::swap(tmp, queue);
}
namespace privacy
{
template <typename _Ty, bool _Dual = false> class concurrent_queue;

#if defined(YASIO_USE_SPSC_QUEUE)
template <typename _Ty, bool _Dual>
class concurrent_queue : public moodycamel::ReaderWriterQueue<_Ty>
{
public:
  bool empty() const { return this->peek() == nullptr; }
  void consume(int count, const std::function<void(_Ty&&)>& func)
  {
    _Ty event;
    while (count-- > 0 && this->try_dequeue(event))
      func(std::move(event));
  }
  void clear() { clear_queue(static_cast<moodycamel::ReaderWriterQueue<_Ty>&>(*this)); }
};

#else
template <typename _Ty> inline _Ty* release_pointer(_Ty*& pointer)
{
  auto tmp = pointer;
  pointer  = nullptr;
  return tmp;
}
template <typename _Ty> class concurrent_queue_primitive
{
  struct concurrent_item
  {

  public:
    concurrent_item() : pitem_(nullptr), pmtx_(nullptr) {}
    concurrent_item(_Ty* pitem, std::recursive_mutex* pmtx) : pitem_(pitem), pmtx_(pmtx) {}
    concurrent_item(const concurrent_item&) = delete;
    concurrent_item(concurrent_item&& rhs)
        : pitem_(release_pointer(rhs.pitem_)), pmtx_(release_pointer(rhs.pmtx_))
    {}
    ~concurrent_item()
    {
      if (pmtx_ != nullptr)
        pmtx_->unlock();
    }

    explicit operator bool() { return pitem_ != nullptr; }

    _Ty& operator*() { return *pitem_; }

  private:
    _Ty* pitem_; // the locked item
    std::recursive_mutex* pmtx_;
  };

public:
  template <typename... _Types> void emplace(_Types&&... values)
  {
    std::lock_guard<std::recursive_mutex> lck(this->mtx_);
    queue_.emplace(std::forward<_Types>(values)...);
  }

  void pop() { queue_.pop(); }
  bool empty() const { return this->queue_.empty(); }
  void clear()
  {
    std::lock_guard<std::recursive_mutex> lck(this->mtx_);
    clear_queue(this->queue_);
  }

  // peek item to read/write thread safe
  concurrent_item peek()
  {
    if (!empty())
    {
      mtx_.lock();
      if (!empty())
        return concurrent_item{&queue_.front(), &mtx_};
      mtx_.unlock();
    }
    return concurrent_item{};
  }

protected:
  std::queue<_Ty> queue_;
  std::recursive_mutex mtx_;
};
template <typename _Ty> class concurrent_queue<_Ty, false> : public concurrent_queue_primitive<_Ty>
{};
template <typename _Ty> class concurrent_queue<_Ty, true> : public concurrent_queue_primitive<_Ty>
{
public:
  void consume(int count, const std::function<void(_Ty&&)>& func)
  {
    if (this->deal_.empty())
    {
      if (this->empty())
        return;
      else
      {
        // swap event queue
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        std::swap(this->deal_, this->queue_);
      }
    }

    while (count-- > 0 && !this->deal_.empty())
    {
      auto event = std::move(this->deal_.front());
      deal_.pop();
      func(std::move(event));
    };
  }

  void clear()
  {
    concurrent_queue_primitive<_Ty>::clear();
    clear_queue(deal_);
  }

private:
  std::queue<_Ty> deal_;
};
#endif
} // namespace privacy
} // namespace yasio

#endif
