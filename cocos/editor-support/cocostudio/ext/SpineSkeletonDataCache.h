#ifndef _SPINESKELETONDATACACHE_H_
#define _SPINESKELETONDATACACHE_H_
#include <cocos2d.h>
#include "spine/spine.h"
#include "spine/spine-cocos2dx.h"
#include "editor-support/cocostudio/CocosStudioExport.h"
#include <functional>

class CC_STUDIO_DLL SpineSkeletonDataCache
{
public:
	class SkeletonData : public cocos2d::Ref
	{
	public:
		SkeletonData(spine::SkeletonData* d, spine::AttachmentLoader* loader) : data(d), attachmentLoader(loader) {}
		~SkeletonData()
		{
			if (data != nullptr)
				delete (data);
			if (attachmentLoader != nullptr)
				delete (attachmentLoader);
		}
		spine::AttachmentLoader* attachmentLoader;
        spine::SkeletonData* data;
	};

	static SpineSkeletonDataCache* getInstance();

    SpineSkeletonDataCache();

    void setErrorReportFunc(void(*errorfunc)(const char* pszFormat, ...));

	SkeletonData* addData(const char* dataFile, const char* atlasFile, float scale);

	void removeData(const char* dataFile);

	void removeAllData(void);
	void removeAllUnusedData(void);

public:
	std::map<std::string, SkeletonData*> _cacheTable;
    void(*_reportError)(const char* pszFormat, ...);
};

#endif

