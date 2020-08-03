#include "BaseObject.h"
DRAGONBONES_NAMESPACE_BEGIN

unsigned BaseObject::_hashCode = 0;
unsigned BaseObject::_defaultMaxCount = 3000;
std::map<std::size_t, unsigned> BaseObject::_maxCountMap;
std::map<std::size_t, std::vector<BaseObject*>> BaseObject::_poolsMap;

void BaseObject::_returnObject(BaseObject* object)
{
    const auto classType = object->getClassTypeIndex();
    const auto maxCountIterator = _maxCountMap.find(classType);
    const auto maxCount = maxCountIterator != _maxCountMap.end() ? maxCountIterator->second : _defaultMaxCount;
    auto& pool = _poolsMap[classType];
    if (pool.size() < maxCount)
    {
        if (!object->_isInPool)
        {
            object->_isInPool = true;
            pool.push_back(object);
        }
        else
        {
            DRAGONBONES_ASSERT(false, "The object is already in the pool.");
        }
    }
    else
    {
        delete object;
    }
}

void BaseObject::setMaxCount(std::size_t classType, unsigned maxCount)
{
    if (classType > 0)
    {
        const auto iterator = _poolsMap.find(classType);
        if (iterator != _poolsMap.end())
        {
            auto& pool = iterator->second;
            if (pool.size() > (size_t)maxCount)
            {
                for (auto i = (size_t)maxCount, l = pool.size(); i < l; ++i)
                {
                    delete pool[i];
                }

                pool.resize(maxCount);
            }
        }

        _maxCountMap[classType] = maxCount;
    }
    else
    {
        _defaultMaxCount = maxCount;
        for (auto& pair : _poolsMap)
        {
            auto& pool = pair.second;
            if (pool.size() > (size_t)maxCount)
            {
                for (auto i = (size_t)maxCount, l = pool.size(); i < l; ++i)
                {
                    delete pool[i];
                }

                pool.resize(maxCount);
            }

            if (_maxCountMap.find(pair.first) != _maxCountMap.end())
            {
                _maxCountMap[pair.first] = maxCount;
            }
        }
    }
}

void BaseObject::clearPool(std::size_t classType)
{
    if (classType > 0)
    {
        const auto iterator = _poolsMap.find(classType);
        if (iterator != _poolsMap.end())
        {
            auto& pool = iterator->second;
            if (!pool.empty())
            {
                for (auto object : pool)
                {
                    delete object;
                }

                pool.clear();
            }
        }
    }
    else
    {
        for (auto& pair : _poolsMap)
        {
            auto& pool = pair.second;
            if (!pool.empty())
            {
                for (auto object : pool)
                {
                    delete object;
                }

                pool.clear();
            }
        }
    }
}

void BaseObject::returnToPool()
{
    _onClear();
    BaseObject::_returnObject(this);
}

DRAGONBONES_NAMESPACE_END
