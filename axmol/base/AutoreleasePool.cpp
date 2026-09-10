/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/base/AutoreleasePool.h"
#include "axmol/base/Macros.h"

namespace ax
{

AutoreleasePool::AutoreleasePool()
    : _name("")
#if defined(_AX_DEBUG) && (_AX_DEBUG > 0)
    , _isClearing(false)
#endif
{
    _managedObjectArray.reserve(150);
    PoolManager::getInstance()->push(this);
}

AutoreleasePool::AutoreleasePool(std::string_view name)
    : _name(name)
#if defined(_AX_DEBUG) && (_AX_DEBUG > 0)
    , _isClearing(false)
#endif
{
    _managedObjectArray.reserve(150);
    PoolManager::getInstance()->push(this);
}

AutoreleasePool::~AutoreleasePool()
{
    AXLOGV("deallocing AutoreleasePool: {}", fmt::ptr(this));
    clear();

    PoolManager::getInstance()->pop();
}

void AutoreleasePool::addObject(Object* object)
{
    _managedObjectArray.emplace_back(object);
}

void AutoreleasePool::clear()
{
#if defined(_AX_DEBUG) && (_AX_DEBUG > 0)
    _isClearing = true;
#endif
    std::vector<Object*> releasings;
    releasings.swap(_managedObjectArray);
    for (const auto& obj : releasings)
    {
        obj->release();
    }
#if defined(_AX_DEBUG) && (_AX_DEBUG > 0)
    _isClearing = false;
#endif
}

bool AutoreleasePool::contains(Object* object) const
{
    for (const auto& obj : _managedObjectArray)
    {
        if (obj == object)
            return true;
    }
    return false;
}

void AutoreleasePool::dump()
{
    AXLOGD("autorelease pool: {}, number of managed object {}\n", _name, static_cast<int>(_managedObjectArray.size()));
    AXLOGD("{}\t{}\t{}", "Object pointer", "Object id", "reference count");
    for (const auto& obj : _managedObjectArray)
    {
        AXLOGD("{}\t{}\n", fmt::ptr(obj), obj->getReferenceCount());
    }
}

//--------------------------------------------------------------------
//
// PoolManager
//
//--------------------------------------------------------------------

PoolManager* PoolManager::s_singleInstance = nullptr;

PoolManager* PoolManager::getInstance()
{
    if (s_singleInstance == nullptr)
    {
        s_singleInstance = new PoolManager();
        // Add the first auto release pool
        new AutoreleasePool("axmol autorelease pool");
    }
    return s_singleInstance;
}

void PoolManager::destroyInstance()
{
    delete s_singleInstance;
    s_singleInstance = nullptr;
}

PoolManager::PoolManager()
{
    _releasePoolStack.reserve(10);
}

PoolManager::~PoolManager()
{
    AXLOGD("deallocing PoolManager: {}", fmt::ptr(this));

    while (!_releasePoolStack.empty())
    {
        AutoreleasePool* pool = _releasePoolStack.back();

        delete pool;
    }
}

AutoreleasePool* PoolManager::getCurrentPool() const
{
    return _releasePoolStack.back();
}

bool PoolManager::isObjectInPools(Object* obj) const
{
    for (const auto& pool : _releasePoolStack)
    {
        if (pool->contains(obj))
            return true;
    }
    return false;
}

void PoolManager::push(AutoreleasePool* pool)
{
    _releasePoolStack.emplace_back(pool);
}

void PoolManager::pop()
{
    AX_ASSERT(!_releasePoolStack.empty());
    _releasePoolStack.pop_back();
}

}  // namespace ax
