/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include <unordered_map>
#include "axmol/base/Map.h"
#include "axmol/base/Data.h"

#include "sceneext/DictionaryHelper.h"
#include "sceneext/ActionTimeline/TimelineMacro.h"
#include "sceneext/SceneExtMacros.h"
#include "flatbuffers/flatbuffer_builder.h"

namespace flatbuffers
{
struct NodeAction;
struct TimeLine;
struct PointFrame;
struct ScaleFrame;
struct ColorFrame;
struct TextureFrame;
struct EventFrame;
struct IntFrame;
struct BoolFrame;
struct InnerActionFrame;
struct EasingData;
struct BlendFrame;
}  // namespace flatbuffers

namespace ax::ext::timeline
{
class ActionTimeline;
class Timeline;
class Frame;

}  // namespace ax::ext::timeline

namespace ax::ext::timeline
{
class SCNIO_API ActionTimelineCache
{
public:
    /** Gets the singleton */
    static ActionTimelineCache* getInstance();

    /** Destroys the singleton */
    static void destroyInstance();

    void init();

    /** Remove action with filename, and also remove other resource relate with this file */
    void removeAction(std::string_view fileName);

    static ActionTimeline* createAction(std::string_view fileName);

    /** Clone a action with the specified name from the container. */
    ActionTimeline* createActionFromJson(std::string_view fileName);
    ActionTimeline* createActionFromContent(std::string_view fileName, std::string_view content);

    ActionTimeline* loadAnimationActionWithFile(std::string_view fileName);
    ActionTimeline* loadAnimationActionWithContent(std::string_view fileName, std::string_view content);

    ActionTimeline* createActionWithFlatBuffersFile(std::string_view fileName);
    ActionTimeline* createActionWithDataBuffer(ax::Data data, std::string_view fileName);

    ActionTimeline* loadAnimationActionWithFlatBuffersFile(std::string_view fileName);
    ActionTimeline* loadAnimationWithDataBuffer(const ax::Data& data, std::string_view fileName);

    ActionTimeline* createActionWithFlatBuffersForSimulator(std::string_view fileName);

protected:
    Timeline* loadTimeline(const rapidjson::Value& json);

    Frame* loadVisibleFrame(const rapidjson::Value& json);
    Frame* loadPositionFrame(const rapidjson::Value& json);
    Frame* loadScaleFrame(const rapidjson::Value& json);
    Frame* loadSkewFrame(const rapidjson::Value& json);
    Frame* loadRotationSkewFrame(const rapidjson::Value& json);
    Frame* loadRotationFrame(const rapidjson::Value& json);
    Frame* loadAnchorPointFrame(const rapidjson::Value& json);
    Frame* loadInnerActionFrame(const rapidjson::Value& json);
    Frame* loadColorFrame(const rapidjson::Value& json);
    Frame* loadTextureFrame(const rapidjson::Value& json);
    Frame* loadEventFrame(const rapidjson::Value& json);
    Frame* loadZOrderFrame(const rapidjson::Value& json);

    Timeline* loadTimelineWithFlatBuffers(const flatbuffers::TimeLine* flatbuffers);

    Frame* loadVisibleFrameWithFlatBuffers(const flatbuffers::BoolFrame* flatbuffers);
    Frame* loadPositionFrameWithFlatBuffers(const flatbuffers::PointFrame* flatbuffers);
    Frame* loadScaleFrameWithFlatBuffers(const flatbuffers::ScaleFrame* flatbuffers);
    Frame* loadRotationSkewFrameWithFlatBuffers(const flatbuffers::ScaleFrame* flatbuffers);
    Frame* loadColorFrameWithFlatBuffers(const flatbuffers::ColorFrame* flatbuffers);
    Frame* loadTextureFrameWithFlatBuffers(const flatbuffers::TextureFrame* flatbuffers);
    Frame* loadEventFrameWithFlatBuffers(const flatbuffers::EventFrame* flatbuffers);
    Frame* loadAlphaFrameWithFlatBuffers(const flatbuffers::IntFrame* flatbuffers);
    Frame* loadAnchorPointFrameWithFlatBuffers(const flatbuffers::ScaleFrame* flatbuffers);
    Frame* loadZOrderFrameWithFlatBuffers(const flatbuffers::IntFrame* flatbuffers);
    Frame* loadInnerActionFrameWithFlatBuffers(const flatbuffers::InnerActionFrame* flatbuffers);
    Frame* loadBlendFrameWithFlatBuffers(const flatbuffers::BlendFrame* flatbuffers);
    void loadEasingDataWithFlatBuffers(Frame* frame, const flatbuffers::EasingData* flatbuffers);

    inline ActionTimeline* createActionWithDataBuffer(const ax::Data& data);

protected:
    typedef std::function<Frame*(const rapidjson::Value& json)> FrameCreateFunc;
    typedef std::pair<std::string, FrameCreateFunc> Pair;

    tlx::string_map<FrameCreateFunc> _funcs;
    ax::StringMap<ActionTimeline*> _animationActions;
};

}  // namespace ax::ext::timeline
