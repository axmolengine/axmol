#include "SpineSkeletonDataCache.h"

SpineSkeletonDataCache* SpineSkeletonDataCache::getInstance()
{
	static SpineSkeletonDataCache internalShared;
	return &internalShared;
}

SpineSkeletonDataCache::SpineSkeletonDataCache()
{
    _reportError = &cocos2d::log;
}

void SpineSkeletonDataCache::setErrorReportFunc(void(*errorfunc)(const char* pszFormat, ...))
{
    _reportError = std::move(errorfunc);
}

void SpineSkeletonDataCache::removeData(const char* dataFile)
{
	auto target = _cacheTable.find(dataFile);
	if (target != _cacheTable.end()) {
		target->second->release();
		_cacheTable.erase(target);
	}
}

SpineSkeletonDataCache::SkeletonData* SpineSkeletonDataCache::addData(const char* dataFile, const char* atlasFile, float scale)
{
	auto target = _cacheTable.find(dataFile);
	if (target != _cacheTable.end()) {
		return target->second;
	}

	spine::SkeletonData* skeletonData = nullptr;
    spine::AttachmentLoader* loader = nullptr;
	bool ok = false;

    auto fileExtension = cocos2d::FileUtils::getInstance()->getFileExtension(dataFile);

    static spine::Cocos2dTextureLoader s_textureLoader;

	do {
		auto atlas = new (__FILE__, __LINE__) spine::Atlas(atlasFile, &s_textureLoader);

		if (nullptr == (atlas))
			break;

		loader = new (__FILE__, __LINE__) spine::Cocos2dAtlasAttachmentLoader(atlas);

        int failed = 0;

        /*
        ** Atlas is used by shared attachment loader, temp json or binary should be dispose.
        ** Cache, we just need SkeletonData & atlas.
        */
        if (fileExtension == ".skel") {
            /*auto binary = new (__FILE__, __LINE__) spine::SkeletonBinary(loader);
            if (nullptr == binary) {
                delete (atlas);
                break;
            }*/
            spine::SkeletonBinary binary(loader);

            binary.setScale(scale);
            skeletonData = binary.readSkeletonDataFile(dataFile); // spSkeletonBinary_readSkeletonDataFile(binary, dataFile);
            if ((!binary.getError().isEmpty())) {
                ++failed;
                _reportError("#parse spine .skel data file failed, error:%s", binary.getError().buffer());
            }
        }
        else {
            spine::SkeletonJson json(loader);

            json.setScale(scale);
            skeletonData = json.readSkeletonDataFile(dataFile);
            if ((!json.getError().isEmpty())) {
                ++failed;
                _reportError("#parse spine .json data file failed, error:%s", json.getError().buffer());
            }
        }

		/*if ((loader->error1 != nullptr)) {
			++failed;
            _reportError("#parse spine attachment failed, error:%s%s", loader->error1, loader->error2);
		}*/
		
		if (failed > 0) {
			if (skeletonData != nullptr)
				delete (skeletonData);

			delete (atlas);
			delete (loader);
			break;
		}

		ok = true;
	} while (false);

	if (ok) {
		auto newData = new SkeletonData(skeletonData, loader);
		_cacheTable.emplace(dataFile, newData);
		return newData;
	}
	return nullptr;
}

void SpineSkeletonDataCache::removeAllData(void)
{
	for (auto & e : _cacheTable)
	{
		e.second->release();
	}
	_cacheTable.clear();
}

void SpineSkeletonDataCache::removeAllUnusedData(void)
{
	auto _First = _cacheTable.begin();
	auto _Last = _cacheTable.end();
	for (; _First != _Last; ) {
		if ((*_First).second->getReferenceCount() == 1) {
			(*_First).second->release();
			_cacheTable.erase(_First++);
			continue;
		}
		++_First;
	}
}
