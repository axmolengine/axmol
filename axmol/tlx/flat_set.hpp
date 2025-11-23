
/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#pragma once
#include <vector>
#include <algorithm>

namespace axstd
{
template <typename Key, typename Compare = std::less<Key>>
class flat_set
{
public:
    using container_type = std::vector<Key>;
    using iterator       = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    flat_set() = default;

    void reserve(size_t capacity) { _data; }

    void clear() { _data.clear(); }

    // Find element by key
    iterator find(const Key& key)
    {
        auto it = lower_bound(key);
        if (it != _data.end() && !_comp(key, *it) && !_comp(*it, key))
            return it;
        return _data.end();
    }

    const_iterator find(const Key& key) const
    {
        auto it = lower_bound(key);
        if (it != _data.end() && !_comp(key, *it) && !_comp(*it, key))
            return it;
        return _data.end();
    }

    // Insert element (no duplicates)
    std::pair<iterator, bool> insert(const Key& key)
    {
        auto it = lower_bound(key);
        if (it != _data.end() && !_comp(key, *it) && !_comp(*it, key))
        {
            return {it, false};  // already exists
        }
        return {_data.insert(it, key), true};
    }

    // Erase element by key
    void erase(const Key& key)
    {
        auto it = find(key);
        if (it != _data.end())
            _data.erase(it);
    }

    // Iterators
    iterator begin() { return _data.begin(); }
    iterator end() { return _data.end(); }
    const_iterator begin() const { return _data.begin(); }
    const_iterator end() const { return _data.end(); }

    bool empty() const { return _data.empty(); }
    size_t size() const { return _data.size(); }

    Key* data() { return _data.data(); }

private:
    container_type _data;
    Compare _comp{};

    // Binary search for lower bound
    iterator lower_bound(const Key& key) { return std::lower_bound(_data.begin(), _data.end(), key, _comp); }

    const_iterator lower_bound(const Key& key) const
    {
        return std::lower_bound(_data.begin(), _data.end(), key, _comp);
    }
};

}  // namespace axstd
