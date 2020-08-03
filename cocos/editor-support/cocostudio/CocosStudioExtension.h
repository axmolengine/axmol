/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-2020 simdsoft, @HALX99
 
 http://www.cocos2d-x.org
 
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


#ifndef __COCOSSTUDIOEXTENSION_H__
#define __COCOSSTUDIOEXTENSION_H__

#include "math/CCAffineTransform.h"

NS_CC_BEGIN

struct CC_DLL ResourceData
{
    int         type;
    std::string file;
    std::string plist;

    ResourceData()
    {
        type = 0;
        file = "";
        plist = "";
    }
    ResourceData(const ResourceData& rv)
    {
        type = rv.type;
        file = rv.file;
        plist = rv.plist;
    }
    ResourceData(ResourceData&& rv)
    {
        type = rv.type;
        file = std::move(rv.file);
        plist = std::move(rv.plist);
    }

    ResourceData(int iType, std::string sFile, std::string sPlist)
    {
        type = iType;
        file = sFile;
        plist = sPlist;
    }
    ResourceData& operator=(const ResourceData& rv)
    {
        type = rv.type;
        file = rv.file;
        plist = rv.plist;
        return *this;
    }
    ResourceData& operator=(ResourceData&& rv)
    {
        type = rv.type;
        file = std::move(rv.file);
        plist = std::move(rv.plist);
        return *this;
    }
};

class CC_DLL NodeExtension
{
public:
    NodeExtension();
    ~NodeExtension();

private:

};


NS_CC_END


#endif