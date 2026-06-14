/*******************************************************************************
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#include "SpineTest.h"
#include <iostream>
#include <fstream>
#include <string.h>

using namespace ax;

#define SET_UNIFORM(ps, name, value)                          \
    do                                                        \
    {                                                         \
        decltype(value) __v = value;                          \
        auto __loc          = (ps)->getUniformLocation(name); \
        (ps)->setUniform(__loc, &__v, sizeof(__v));           \
    } while (false)

#define NUM_SKELETONS           50
#define SPINE_NODE_SCALE_FACTOR 0.4

#define SCALE_SKELETON_NODE(node)                    \
    do                                               \
    {                                                \
        if (node)                                    \
            node->setScale(SPINE_NODE_SCALE_FACTOR); \
    } while (false)

//------------------------------------------------------------------
//
// SpineTestScene
//
//------------------------------------------------------------------

SpineTests::SpineTests()
{
    auto fu      = FileUtils::getInstance();
    _searchPaths = fu->getSearchPaths();
    fu->addSearchPath("spine", true);

    ADD_TEST_CASE(BatchingExample);
    ADD_TEST_CASE(CoinExample);
    ADD_TEST_CASE(GoblinsExample);
    ADD_TEST_CASE(IKExample);
    ADD_TEST_CASE(MixAndMatchExample);
    ADD_TEST_CASE(RaptorExample);
    ADD_TEST_CASE(SkeletonRendererSeparatorExample);
    ADD_TEST_CASE(SpineboyExample);
    ADD_TEST_CASE(TankExample);

#ifdef _AX_DEBUG
    debugExtension = new spine::DebugExtension(spine::SpineExtension::getInstance());
#endif
}

SpineTests::~SpineTests()
{
    FileUtils::getInstance()->setSearchPaths(_searchPaths);
#ifdef _AX_DEBUG
    debugExtension->reportLeaks();
    delete debugExtension;
#endif
}

SpineTestLayer::SpineTestLayer() : _title("") {}

SpineTestLayer::~SpineTestLayer() {}

std::string SpineTestLayer::title() const
{
    return _title;
}

bool SpineTestLayer::init()
{
    if (!TestCase::init())
        return false;

    PointerEventListener* listener = PointerEventListener::create();
    listener->onPointerDown        = [this](PointerEvent* event) -> bool {
        if (!skeletonNode)
            return true;
        _touchIndex = (_touchIndex + 1) % 3;
        if (_touchIndex == 0)
        {
            skeletonNode->setDebugBonesEnabled(false);
            skeletonNode->setTimeScale(1.0f);
        }
        else if (_touchIndex == 1)
        {
            skeletonNode->setDebugBonesEnabled(true);
            skeletonNode->setTimeScale(1.0f);
        }
        else if (_touchIndex == 2)
        {
            skeletonNode->setDebugBonesEnabled(true);
            skeletonNode->setTimeScale(0.3f);

            // refer issue: https://github.com/axmolengine/axmol/issues/482
            // Get Spine node PS and update uniforms
            auto skeleton1PS = skeletonNode->getProgramState();

            if (skeleton1PS)
            {
                Vec2 resolution{100.f, 100.f};
                float blurRadius = 50.0f;
                float sampleNum  = 7.0f;

                // This doesn't work on Spine node, works on sprite node
                SET_UNIFORM(skeleton1PS, "resolution", resolution);
                SET_UNIFORM(skeleton1PS, "blurRadius", blurRadius);
                SET_UNIFORM(skeleton1PS, "sampleNum", sampleNum);
            }
        }
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    SCALE_SKELETON_NODE(skeletonNode);

    return true;
}

// BatchingExample
bool BatchingExample::init()
{
    if (!SpineTestLayer::init())
        return false;

    _title = "Batching";

    auto asset = spine::SkeletonAssetCache::getInstance()->loadAsset("spineboy-pro.json", "spineboy.atlas", 0.6f);
    AXASSERT(asset, "Error reading skeleton asset file.");

    int xMin = _contentSize.width * 0.10f, xMax = _contentSize.width * 0.90f;
    int yMin = 0, yMax = _contentSize.height * 0.7f;
    for (int i = 0; i < NUM_SKELETONS; i++)
    {
        auto skeletonNode = spine::SkeletonAnimation::create(asset);

        skeletonNode->setMix("walk", "jump", 0.2f);
        skeletonNode->setMix("jump", "run", 0.2f);
        skeletonNode->setAnimation(0, "walk", true);
        skeletonNode->addAnimation(0, "jump", true, RandomHelper::random_int(0, 300) / 100.0f);
        skeletonNode->addAnimation(0, "run", true);

        // alternative setting two color tint for groups of 10 skeletons
        // should end up with #skeletons / 10 batches
        // if (j++ < 10)
        //			skeletonNode->setTwoColorTint(true);
        //		if (j == 20) j = 0;
        // skeletonNode->setTwoColorTint(true);

        skeletonNode->setPosition(Vec2(RandomHelper::random_int(xMin, xMax), RandomHelper::random_int(yMin, yMax)));
        addChild(skeletonNode);
    }
    SCALE_SKELETON_NODE(skeletonNode);
    return true;
}

bool CoinExample::init()
{

    if (!SpineTestLayer::init())
        return false;

    _title = "Coin";

    skeletonNode = spine::SkeletonAnimation::create("coin-pro.skel", "coin.atlas", 1);
    skeletonNode->setAnimation(0, "animation", true);

    skeletonNode->setPosition(Vec2(_contentSize.width / 2, _contentSize.height / 2));
    addChild(skeletonNode);

    scheduleUpdate();
    SCALE_SKELETON_NODE(skeletonNode);
    return true;
}

bool GoblinsExample::init()
{
    if (!SpineTestLayer::init())
        return false;

    _title       = "Goblins";
    skeletonNode = spine::SkeletonAnimation::create("goblins-pro.json", "goblins.atlas", 1.5f);
    skeletonNode->setAnimation(0, "walk", true);
    skeletonNode->setSkin("goblin");

    skeletonNode->setPosition(Vec2(_contentSize.width / 2, 20));
    addChild(skeletonNode);
    SCALE_SKELETON_NODE(skeletonNode);
    return true;
}

bool IKExample::init()
{
    if (!SpineTestLayer::init())
        return false;

    _title = "IKExample";

    // Load the Spineboy skeleton and create a SkeletonAnimation node from it
    // centered on the screen.
    skeletonNode = spine::SkeletonAnimation::create("spineboy-pro.json", "spineboy.atlas", 0.6f);
    skeletonNode->setPosition(Vec2(_contentSize.width / 2, 20));
    addChild(skeletonNode);

    // Queue the "walk" animation on the first track.
    skeletonNode->setAnimation(0, "walk", true);

    // Queue the "aim" animation on a higher track.
    // It consists of a single frame that positions
    // the back arm and gun such that they point at
    // the "crosshair" bone. By setting this
    // animation on a higher track, it overrides
    // any changes to the back arm and gun made
    // by the walk animation, allowing us to
    // mix the two. The mouse position following
    // is performed in the lambda below.
    skeletonNode->setAnimation(1, "aim", true);

    // Next we setup a listener that receives and stores
    // the current mouse location. The location is converted
    // to the skeleton's coordinate system.
    PointerEventListener* mouseListener = PointerEventListener::create();
    mouseListener->onPointerMove        = [this](ax::PointerEvent* event) -> bool {
        // convert the mosue location to the skeleton's coordinate space
        // and store it.
        position = skeletonNode->convertToNodeSpace(event->getLocation());

        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    // Position the "crosshair" bone at the mouse
    // location.
    //
    // When setting the crosshair bone position
    // to the mouse position, we need to translate
    // from "skeleton space" to "local bone space".
    // Note that the local bone space is calculated
    // using the bone's parent worldToLocal() function!
    //
    // After updating the bone position based on the
    // converted mouse location, we call updateWorldTransforms()
    // again so the change of the IK target position is
    // applied to the rest of the skeleton.
    skeletonNode->setPostUpdateWorldTransformsListener([this](spine::SkeletonAnimation* node) -> void {
        node->setBoneWorldPosition("crosshair", position);
        node->updateWorldTransform(spine::Physics_Update);
    });

    SCALE_SKELETON_NODE(skeletonNode);
    return true;
}

bool MixAndMatchExample::init()
{
    if (!SpineTestLayer::init())
        return false;

    _title = "Mix and Match";

    skeletonNode = spine::SkeletonAnimation::create("mix-and-match-pro.skel", "mix-and-match.atlas", 0.5);
    skeletonNode->setAnimation(0, "dance", true);

    static std::string_view skins[]{
        "skin-base"sv,        "nose/short"sv,      "eyelids/girly"sv,
        "eyes/violet"sv,      "hair/brown"sv,      "clothes/hoodie-orange"sv,
        "legs/pants-jeans"sv, "accessories/bag"sv, "accessories/hat-red-yellow"sv,
    };
    skeletonNode->setCombinedSkin("mix-and-match", std::span{skins});

    skeletonNode->setPosition(Vec2(_contentSize.width / 3, _contentSize.height / 2 - 100));
    addChild(skeletonNode);

    SCALE_SKELETON_NODE(skeletonNode);

    // load hsv as custom, we don't want batch draw
    auto hsvProg =
        ProgramManager::getInstance()->loadProgram(positionTextureColor_vert, hsv_frag, VertexLayoutKind::Sprite);

    auto ps1 = new rhi::ProgramState(hsvProg);
    SET_UNIFORM(ps1, "u_hsv", Vec3(92.0f, 1.0f, 1.2f));
    ps1->updateBatchId();
    skeletonNode->setProgramState(ps1, true);

    /* -------- skeletonNode2 with same spine animation file ------------ */
    auto skeletonNode2 = spine::SkeletonAnimation::create("mix-and-match-pro.skel", "mix-and-match.atlas", 0.5);
    skeletonNode2->setAnimation(0, "dance", true);

    skeletonNode2->setCombinedSkin("mix-and-match", skins);

    skeletonNode2->setPosition(Vec2(_contentSize.width / 1.5, _contentSize.height / 2 - 100));
    addChild(skeletonNode2);

    SCALE_SKELETON_NODE(skeletonNode2);

    auto ps2 = new rhi::ProgramState(hsvProg);
    SET_UNIFORM(ps2, "u_hsv", Vec3(-45.0f, 1.0f, 1.2f));
    ps2->updateBatchId();
    skeletonNode2->setProgramState(ps2, true);
    return true;
}

bool RaptorExample::init()
{
    if (!SpineTestLayer::init())
        return false;

    _title       = "Raptor";
    skeletonNode = spine::SkeletonAnimation::create("raptor-pro.json", "raptor.atlas", 0.5f);
    skeletonNode->setAnimation(0, "walk", true);
    skeletonNode->addAnimation(1, "gun-grab", false, 2);
    skeletonNode->setTwoColorTint(true);

    // effect.setCenterY(200);
    swirlTime = 0;

    // skeletonNode->setVertexEffect(&effect);

    skeletonNode->setPosition(Vec2(_contentSize.width / 2, 20));
    addChild(skeletonNode);

    scheduleUpdate();

    SCALE_SKELETON_NODE(skeletonNode);
    return true;
}

void RaptorExample::update(float fDelta)
{
    swirlTime += fDelta;
    float percent = spine::MathUtil::fmod(swirlTime, 2);
    if (percent > 1)
        percent = 1 - (percent - 1);
    // effect.setAngle(pow2.interpolate(-60.0f, 60.0f, percent));
}

bool SkeletonRendererSeparatorExample::init()
{
    if (!SpineTestLayer::init())
        return false;

    _title = "Seperator";

    // Spineboy's back, which will manage the animation and GPU resources
    // will render only the front slots of Spineboy
    skeletonNode = spine::SkeletonAnimation::create("spineboy-pro.json", "spineboy.atlas", 0.6f);
    skeletonNode->setMix("walk", "jump", 0.4);
    skeletonNode->setAnimation(0, "walk", true);
    skeletonNode->setSlotsRange(skeletonNode->findSlotIndex("rear-upper-arm"),
                                skeletonNode->findSlotIndex("rear-shin"));
    skeletonNode->setPosition(Vec2(_contentSize.width / 2, 20));

    // A simple rectangle to go between the front and back slots of Spineboy
    betweenNode = DrawNode::create();
    Vec2 rect[4];
    rect[0] = Vec2(0, 0);
    rect[1] = Vec2(40, 0);
    rect[2] = Vec2(40, 200);
    rect[3] = Vec2(0, 200);
    betweenNode->drawPolygon(rect, 4, ax::Color(1, 0, 0, 1), 1, ax::Color(1, 0, 0, 1));
    betweenNode->setPosition(Vec2(_contentSize.width / 2 + 30, 20));
    // Spineboy's front, doesn't manage any skeleton, animation or GPU resources, but simply
    // renders the back slots of Spineboy. The skeleton, animatio state and GPU resources
    // are shared with the front node!
    frontNode = skeletonNode->clone(skeletonNode->findSlotIndex("neck"), -1);
    frontNode->setPosition(Vec2(_contentSize.width / 2, 20));

    // Add the front, between and back node in the correct order to this scene
    addChild(skeletonNode);
    addChild(betweenNode);
    addChild(frontNode);

    scheduleUpdate();

    SCALE_SKELETON_NODE(skeletonNode);
    SCALE_SKELETON_NODE(frontNode);
    return true;
}

void SkeletonRendererSeparatorExample::update(float deltaTime)
{
    // Test releasing memory.
    // Director::getInstance()->replaceScene(SpineboyExample::scene());
}

template <>
class fmt::formatter<spine::String>
{
public:
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format(spine::String const& value, Context& ctx) const
    {
        return fmt::format_to(ctx.out(), FMT_COMPILE("{}"), fmt::string_view{value.buffer(), value.length()});
    }
};

bool SpineboyExample::init()
{
    if (!SpineTestLayer::init())
        return false;

    _title = "Spineboy";

    skeletonNode = spine::SkeletonAnimation::create("spineboy-pro.json", "spineboy.atlas", 0.6f);

    skeletonNode->setStartListener([](spine::TrackEntry* entry) {
        AXLOGI("{} start: {}", entry->getTrackIndex(), entry->getAnimation().getName().buffer());
    });
    skeletonNode->setInterruptListener(
        [](spine::TrackEntry* entry) { AXLOGI("{} interrupt", entry->getTrackIndex()); });
    skeletonNode->setEndListener([](spine::TrackEntry* entry) { AXLOGI("{} end", entry->getTrackIndex()); });
    skeletonNode->setCompleteListener([](spine::TrackEntry* entry) { AXLOGI("{} complete", entry->getTrackIndex()); });
    skeletonNode->setDisposeListener([](spine::TrackEntry* entry) { AXLOGI("{} dispose", entry->getTrackIndex()); });
    skeletonNode->setEventListener([](spine::TrackEntry* entry, spine::Event* event) {
        AXLOGI("{} event: {}, {}, {}, {}", entry->getTrackIndex(), event->getData().getName().buffer(), event->getInt(),
               event->getFloat(), event->getString());
    });

    skeletonNode->setMix("walk", "jump", 0.4);
    skeletonNode->setMix("jump", "run", 0.4);
    skeletonNode->setAnimation(0, "walk", true);
    spine::TrackEntry* jumpEntry = skeletonNode->addAnimation(0, "jump", false, 1);
    skeletonNode->addAnimation(0, "run", true);

    skeletonNode->setTrackStartListener(jumpEntry, [](spine::TrackEntry* entry) { AXLOGI("jumped!"); });

    // skeletonNode->addAnimation(1, "test", true);
    // skeletonNode->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1),
    // DelayTime::create(5), nullptr)));

    skeletonNode->setPosition(Vec2(_contentSize.width / 2, 20));
    addChild(skeletonNode);

    auto program = ProgramManager::getInstance()->loadProgram(positionTextureColor_vert, "custom/example_Blur_fs",
                                                              VertexLayoutKind::Sprite);
    skeletonNode->setProgramState(new rhi::ProgramState(program), true);

    // auto skeleton1PS = skeletonNode->getProgramState();

    // Vec2 resolution{100.f, 100.f};
    // float blurRadius = 50.0f;
    // float sampleNum  = 7.0f;
    // SET_UNIFORM(skeleton1PS, "resolution", resolution);
    // SET_UNIFORM(skeleton1PS, "blurRadius", blurRadius);
    // SET_UNIFORM(skeleton1PS, "sampleNum", sampleNum);

    scheduleUpdate();

    SCALE_SKELETON_NODE(skeletonNode);
    return true;
}

void SpineboyExample::update(float deltaTime)
{
    // Test releasing memory.
    // Director::getInstance()->replaceScene(SpineboyExample::scene());
}

bool TankExample::init()
{
    if (!SpineTestLayer::init())
        return false;

    _title       = "Tank";
    skeletonNode = spine::SkeletonAnimation::create("tank-pro.skel", "tank.atlas", 0.5f);
    skeletonNode->setAnimation(0, "shoot", true);

    skeletonNode->setPosition(Vec2(_contentSize.width / 2 + 400, 20));
    addChild(skeletonNode);

    SCALE_SKELETON_NODE(skeletonNode);
    return true;
}
