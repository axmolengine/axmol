
#ifndef __COCOSSTUDIOEXTENSION_H__
#define __COCOSSTUDIOEXTENSION_H__

#include "math/CCAffineTransform.h"
#include "CocosStudioExport.h"

NS_CC_BEGIN

// Define at GUIDefine.h
// struct CC_DLL ResourceData
// {
//     int         type;
//     std::string file;
//     std::string plist;
// 
//     ResourceData()
//     {
//         type = 0;
//         file = "";
//         plist = "";
//     }
//     ResourceData(const ResourceData& rv)
//     {
//         type = rv.type;
//         file = rv.file;
//         plist = rv.plist;
//     }
//     ResourceData(ResourceData&& rv)
//     {
//         type = rv.type;
//         file = std::move(rv.file);
//         plist = std::move(rv.plist);
//     }
// 
//     ResourceData(int iType, std::string sFile, std::string sPlist)
//     {
//         type = iType;
//         file = sFile;
//         plist = sPlist;
//     }
//     ResourceData& operator=(const ResourceData& rv)
//     {
//         type = rv.type;
//         file = rv.file;
//         plist = rv.plist;
//         return *this;
//     }
//     ResourceData& operator=(ResourceData&& rv)
//     {
//         type = rv.type;
//         file = std::move(rv.file);
//         plist = std::move(rv.plist);
//         return *this;
//     }
// };

class CCS_DLL NodeExtension
{
public:
    NodeExtension();
    ~NodeExtension();

private:

};


NS_CC_END


#endif