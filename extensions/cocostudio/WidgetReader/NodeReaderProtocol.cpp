//
//  NodeReaderProtocol.cpp
//  cocos2d_libs
// 
//  Created by pipu on 14/11/18.
//
//

#include "cocostudio/WidgetReader/NodeReaderProtocol.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CCArmature.h"
#include "cocostudio/ActionTimeline/CCSkeletonNode.h"

cocos2d::ResourceData cocos2d::wext::makeResourceData(const flatbuffers::ResourceData* orig)
{
    cocos2d::ResourceData fileData;
    fileData.file = orig->path()->c_str();
    fileData.plist = orig->plistFile()->c_str();
    fileData.type = orig->resourceType();
    return fileData;
}

cocos2d::ResourceData cocos2d::wext::makeResourceData(const std::string& path, int type)
{
    cocos2d::ResourceData fileData;
    fileData.file = path;
    fileData.type = type;
    return fileData;
}

cocos2d::ResourceData cocos2d::wext::makeResourceData(std::string&& path, int type)
{
    cocos2d::ResourceData fileData;
    fileData.file = std::move(path);
    fileData.type = type;
    return fileData;
}
