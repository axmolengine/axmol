/****************************************************************************
*
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <deque>
#include <mutex>

namespace ax
{

template <typename _Ty>
class ConcurrentDeque
{
public:
    /** Iterator, can be used to loop the Vector. */
    using iterator = typename std::deque<_Ty>::iterator;
    /** Const iterator, can be used to loop the Vector. */
    using const_iterator = typename std::deque<_Ty>::const_iterator;

    void emplace_back(_Ty&& value)
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        queue_.emplace_back(std::forward<_Ty>(value));
    }
    void emplace_back(const _Ty& value)
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        queue_.emplace_back(value);
    }
    _Ty& front()
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        return queue_.front();
    }
    void pop_front()
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        queue_.pop_front();
    }
    void push_front(_Ty&& value)
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        queue_.push_front(std::forward<_Ty>(value));
    }
    void push_front(const _Ty& value)
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        queue_.push_front(value);
    }
    size_t size() const
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        return this->queue_.size();
    }
    bool empty() const
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        return this->queue_.empty();
    }
    void clear()
    {
        std::lock_guard<std::recursive_mutex> lck(this->mtx_);
        this->queue_.clear();
    }

    std::unique_lock<std::recursive_mutex> get_lock() { return std::unique_lock<std::recursive_mutex>{this->mtx_}; }

    void lock() { this->mtx_.lock(); }
    void unlock() { this->mtx_.unlock(); }

    void unsafe_emplace_back(_Ty&& value) { queue_.emplace_back(std::forward<_Ty>(value)); }
    void unsafe_emplace_back(const _Ty& value) { queue_.emplace_back(value); }
    _Ty& unsafe_front() { return queue_.front(); }
    void unsafe_pop_front() { queue_.pop_front(); }
    void unsafe_push_front(_Ty&& value) { queue_.push_font(std::forward<_Ty>(value)); }
    void unsafe_push_front(const _Ty& value) { queue_.push_font(value); }
    bool unsafe_empty() const { return this->queue_.empty(); }
    size_t unsafe_size() const { return this->queue_.size(); }
    void unsafe_clear() { this->queue_.clear(); }

    iterator unsafe_begin() { return this->queue_.begin(); }
    iterator unsafe_end() { return this->queue_.end(); }

    const_iterator unsafe_begin() const { return this->queue_.begin(); }

    const_iterator unsafe_end() const { return this->queue_.end(); }

    iterator unsafe_erase(iterator iter) { return this->queue_.erase(iter); }

private:
    std::deque<_Ty> queue_;
    mutable std::recursive_mutex mtx_;
};
}  // namespace ax
