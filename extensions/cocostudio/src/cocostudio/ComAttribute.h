/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#ifndef __AX_EXTENTIONS_CCCOMATTRIBUTE_H__
#define __AX_EXTENTIONS_CCCOMATTRIBUTE_H__

#include "ComBase.h"
#include "2d/Component.h"
#include "base/Value.h"
#include "CocosStudioExport.h"

namespace cocostudio
{

class CCS_DLL ComAttribute : public ax::Component
{
    DECLARE_CLASS_COMPONENT_INFO
public:
    /**
     * @js ctor
     */
    ComAttribute(void);
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ComAttribute(void);

public:
    const static std::string COMPONENT_NAME;

    static ComAttribute* create(void);
    virtual bool init() override;
    virtual bool serialize(void* r) override;

    void setInt(std::string_view key, int value);
    void setFloat(std::string_view key, float value);
    void setBool(std::string_view key, bool value);
    void setString(std::string_view key, std::string_view value);
    int getInt(std::string_view key, int def = 0) const;
    float getFloat(std::string_view key, float def = 0.0f) const;
    bool getBool(std::string_view key, bool def = false) const;
    std::string getString(std::string_view key, std::string_view def = "") const;
    bool parse(std::string_view jsonFile);

private:
    ax::ValueMap _dict;
    rapidjson::Document _doc;
};

}  // namespace cocostudio

#endif  // __AX_EXTENTIONS_CCCOMATTRIBUTE_H__
