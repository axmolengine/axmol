/****************************************************************************
 Copyright (c) 2013-2017 Chukong Technologies
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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

#ifndef __CCMAP_H__
#define __CCMAP_H__

#define USE_STD_UNORDERED_MAP 1
#define USE_ROBIN_MAP 1

#include "base/Macros.h"
#include "base/Object.h"
#include <vector>

#if USE_STD_UNORDERED_MAP
#    include <unordered_map>
#else
#    include <map>
#endif

/**
 * @addtogroup base
 * @{
 */
NS_AX_BEGIN

/**
 * Similar to std::unordered_map, but it will manage reference count automatically internally.
 * Which means it will invoke Object::retain() when adding an element, and invoke Object::release() when removing an element.
 * @warning The element should be `Object` or its sub-class.
 * @js NA
 * @lua NA
 */
template <class K, class V, typename H = std::hash<K>, typename E = std::equal_to<K>>
class Map
{
public:
#if USE_STD_UNORDERED_MAP
    typedef tsl::robin_map<K, V, H, E> RefMap;
#else
    typedef std::map<K, V> RefMap;
#endif

    // ------------------------------------------
    // Iterators
    // ------------------------------------------

    /** Iterator, can be used to loop the Map. */
    typedef typename RefMap::iterator iterator;
    /** Const iterator, can be used to loop the Map. */
    typedef typename RefMap::const_iterator const_iterator;

    /** Return iterator to beginning. */
    iterator begin() { return _data.begin(); }
    /** Return const_iterator to beginning. */
    const_iterator begin() const { return _data.begin(); }

    /** Return iterator to end.*/
    iterator end() { return _data.end(); }
    /** Return const_iterator to end.*/
    const_iterator end() const { return _data.end(); }

    /** Return const_iterator to beginning.*/
    const_iterator cbegin() const { return _data.cbegin(); }
    /** Return const_iterator to end.*/
    const_iterator cend() const { return _data.cend(); }

    /** Default constructor */
    Map() : _data()
    {
        static_assert(std::is_convertible<V, Object*>::value, "Invalid Type for ax::Map<K, V>!");
        AXLOGINFO("In the default constructor of Map!");
    }

    /** Constructor with capacity. */
    explicit Map(ssize_t capacity) : _data()
    {
        static_assert(std::is_convertible<V, Object*>::value, "Invalid Type for ax::Map<K, V>!");
        AXLOGINFO("In the constructor with capacity of Map!");
        _data.reserve(capacity);
    }

    /** Copy constructor. */
    Map(const Map& other)
    {
        static_assert(std::is_convertible<V, Object*>::value, "Invalid Type for ax::Map<K, V>!");
        AXLOGINFO("In the copy constructor of Map!");
        _data = other._data;
        addRefForAllObjects();
    }

    /** Move constructor. */
    Map(Map&& other)
    {
        static_assert(std::is_convertible<V, Object*>::value, "Invalid Type for ax::Map<K, V>!");
        AXLOGINFO("In the move constructor of Map!");
        _data = std::move(other._data);
    }

    /**
     * Destructor.
     * It will release all objects in map.
     */
    ~Map()
    {
        AXLOGINFO("In the destructor of Map!");
        clear();
    }

    /** Sets capacity of the map. */
    void reserve(ssize_t capacity)
    {
#if USE_STD_UNORDERED_MAP
        _data.reserve(capacity);
#endif
    }

    /** Returns the number of buckets in the Map container. */
    ssize_t bucketCount() const
    {
#if USE_STD_UNORDERED_MAP
        return _data.bucket_count();
#else
        return 0;
#endif
    }

    /** Returns the number of elements in bucket n. */
    ssize_t bucketSize(ssize_t n) const
    {
#if USE_STD_UNORDERED_MAP && !defined(USE_ROBIN_MAP)
        return _data.bucket_size(n);
#else
        return 0;
#endif
    }

    /** Returns the bucket number where the element with key k is located. */
    ssize_t bucket(const K& k) const
    {
#if USE_STD_UNORDERED_MAP && !defined(USE_ROBIN_MAP)
        return _data.bucket(k);
#else
        return 0;
#endif
    }

    /** The number of elements in the map. */
    ssize_t size() const { return _data.size(); }

    /**
     * Returns a bool value indicating whether the map container is empty, i.e. whether its size is 0.
     *  @note This function does not modify the content of the container in any way.
     *        To clear the content of an array object, member function unordered_map::clear exists.
     */
    bool empty() const { return _data.empty(); }

    /** Returns all keys in the map. */
    std::vector<K> keys() const
    {
        std::vector<K> keys;

        if (!_data.empty())
        {
            keys.reserve(_data.size());

            for (const auto& iter : _data)
            {
                keys.emplace_back(iter.first);
            }
        }
        return keys;
    }

    /** Returns all keys that matches the object. */
    std::vector<K> keys(V object) const
    {
        std::vector<K> keys;

        if (!_data.empty())
        {
            keys.reserve(_data.size() / 10);

            for (const auto& iter : _data)
            {
                if (iter.second == object)
                {
                    keys.emplace_back(iter.first);
                }
            }
        }

        keys.shrink_to_fit();

        return keys;
    }

    /**
     * Returns a reference to the mapped value of the element with key k in the map.
     *
     *  @note If key does not match the key of any element in the container, the function return nullptr.
     *  @param key Key value of the element whose mapped value is accessed.
     *       Member type K is the keys for the elements in the container. defined in Map<K, V> as an alias of its first
     * template parameter (Key).
     */
    template <typename K2>
    const V at(const K2& key) const
    {
        auto iter = _data.find(key);
        if (iter != _data.end())
            return iter->second;
        return nullptr;
    }

    template <typename K2>
    V at(const K2& key)
    {
        auto iter = _data.find(key);
        if (iter != _data.end())
            return iter->second;
        return nullptr;
    }

    /**
     * Searches the container for an element with 'key' as key and returns an iterator to it if found,
     *         otherwise it returns an iterator to Map<K, V>::end (the element past the end of the container).
     *
     * @param key Key to be searched for.
     *        Member type 'K' is the type of the keys for the elements in the container,
     *        defined in Map<K, V> as an alias of its first template parameter (Key).
     */
    template <typename K2>
    const_iterator find(const K2& key) const
    {
        return _data.find(key);
    }

    template <typename K2>
    iterator find(const K2& key)
    {
        return _data.find(key);
    }

    /**
     * Inserts new elements in the map.
     *
     * @note If the container has already contained the key, this function will erase the old pair(key, object)  and
     * insert the new pair.
     * @param key The key to be inserted.
     * @param object The object to be inserted.
     */
    template <typename K2>
    void insert(const K2& key, V object)
    {
        AXASSERT(object != nullptr, "Object is nullptr!");
        object->retain();
        erase(key);
        _data.emplace(key, object);
    }

    /**
     * Removes an element with an iterator from the Map<K, V> container.
     *
     * @param position Iterator pointing to a single element to be removed from the Map<K, V>.
     *        Member type const_iterator is a forward iterator type.
     */
    iterator erase(const_iterator position)
    {
        AXASSERT(position != _data.cend(), "Invalid iterator!");
        position->second->release();
        return _data.erase(position);
    }

    iterator erase(iterator position)
    {
        AXASSERT(position != _data.cend(), "Invalid iterator!");
        position->second->release();
        return _data.erase(position);
    }

    /**
     * Removes an element with an iterator from the Map<K, V> container.
     *
     * @param k Key of the element to be erased.
     *        Member type 'K' is the type of the keys for the elements in the container,
     *        defined in Map<K, V> as an alias of its first template parameter (Key).
     */
    // size_t erase(const K& k)
    //{
    //     auto iter = _data.find(k);
    //     if (iter != _data.end())
    //     {
    //         iter->second->release();
    //         _data.erase(iter);
    //         return 1;
    //     }
    //     return 0;
    // }

    template <typename _K2>
    size_t erase(const _K2& k)
    {
        auto iter = _data.find(k);
        if (iter != _data.end())
        {
            iter->second->release();
            _data.erase(iter);
            return 1;
        }
        return 0;
    }

    /**
     * Removes some elements with a vector which contains keys in the map.
     *
     * @param keys Keys of elements to be erased.
     */
    template <typename _K2>
    void erase(const std::vector<_K2>& keys)
    {
        for (const auto& key : keys)
        {
            this->erase(key);
        }
    }

    /**
     * All the elements in the Map<K,V> container are dropped:
     *  their reference count will be decreased, and they are removed from the container,
     *  leaving it with a size of 0.
     */
    void clear()
    {
        for (const auto& iter : _data)
        {
            iter.second->release();
        }

        _data.clear();
    }

    /**
     * Gets a random object in the map.
     * @return Returns the random object if the map isn't empty, otherwise it returns nullptr.
     */
    V getRandomObject() const
    {
        if (!_data.empty())
        {
            ssize_t randIdx         = RandomHelper::random_int<int>(0, static_cast<int>(_data.size()) - 1);
            const_iterator randIter = _data.begin();
            std::advance(randIter, randIdx);
            return randIter->second;
        }
        return nullptr;
    }

    // Don't uses operator since we could not decide whether it needs 'retain'/'release'.
    //    V& operator[] ( const K& key )
    //    {
    //        AXLOG("copy: [] ref");
    //        return _data[key];
    //    }
    //
    //    V& operator[] ( K&& key )
    //    {
    //        AXLOG("move [] ref");
    //        return _data[key];
    //    }

    //    const V& operator[] ( const K& key ) const
    //    {
    //        AXLOG("const copy []");
    //        return _data.at(key);
    //    }
    //
    //    const V& operator[] ( K&& key ) const
    //    {
    //        AXLOG("const move []");
    //        return _data.at(key);
    //    }

    /** Copy assignment operator. */
    Map& operator=(const Map& other)
    {
        if (this != &other)
        {
            AXLOGINFO("In the copy assignment operator of Map!");
            clear();
            _data = other._data;
            addRefForAllObjects();
        }
        return *this;
    }

    /** Move assignment operator. */
    Map& operator=(Map&& other)
    {
        if (this != &other)
        {
            AXLOGINFO("In the move assignment operator of Map!");
            clear();
            _data = std::move(other._data);
        }
        return *this;
    }

protected:
    /** Retains all the objects in the map */
    void addRefForAllObjects()
    {
        for (auto&& iter : _data)
        {
            iter.second->retain();
        }
    }

    RefMap _data;
};

template <typename _Valty>
using StringMap = Map<std::string, _Valty, hlookup::string_hash, hlookup::equal_to>;

NS_AX_END
// end group
/// @}

#endif /* __CCMAP_H__ */
