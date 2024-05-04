/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#ifndef _AX_GROUPCOMMAND_H_
#define _AX_GROUPCOMMAND_H_

#include <vector>
#include <unordered_map>

#include "base/Object.h"
#include "renderer/RenderCommand.h"

/**
 * @addtogroup renderer
 * @{
 */

NS_AX_BEGIN

// Used for internal
class GroupCommandManager : public Object
{
public:
    int getGroupID();
    void releaseGroupID(int groupID);

protected:
    friend class Renderer;
    GroupCommandManager();
    ~GroupCommandManager();
    bool init();
    std::unordered_map<int, bool> _groupMapping;
    std::vector<int> _unusedIDs;
};

/**
 GroupCommand is used to group several command together, and more, it can be nested.
 So it is used to generate the hierarchy for the rendcommands. Every group command will be assigned by a group ID.
 */
class AX_DLL GroupCommand : public RenderCommand
{
public:
    /**@{
     Constructor and Destructor.
     */
    GroupCommand();
    ~GroupCommand();
    /**@}*/

    /**Init function for group command*/
    void init(float globalOrder);

    /**called by renderer, get the group ID.*/
    int getRenderQueueID() const { return _renderQueueID; }

    /**
     * @brief Reset the command state for reuse
     * 
     */
    void reset();

protected:
    int _renderQueueID;
};

NS_AX_END

/**
 end of support group
 @}
 */
#endif  //_AX_GROUPCOMMAND_H_
