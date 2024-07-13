/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "MaterialSystemTest.h"

#include <ctime>
#include <spine/spine-cocos2dx.h>

#include "../testResource.h"
#include "axmol.h"
#include "ui/CocosGUI.h"

USING_NS_AX;

static void printProperties(Properties* properties, int indent);

// MARK: Tests

MaterialSystemTest::MaterialSystemTest()
{
    ADD_TEST_CASE(Material_2DEffects);
    ADD_TEST_CASE(Material_AutoBindings);
    ADD_TEST_CASE(Material_setTechnique);
    ADD_TEST_CASE(Material_clone);
    ADD_TEST_CASE(Material_MultipleMeshRenderer);
    ADD_TEST_CASE(Material_MeshRendererTest);
    ADD_TEST_CASE(Material_parsePerformance);
}

std::string MaterialSystemBaseTest::title() const
{
    return "Material System";
}

// MARK: Tests start here

void Material_MeshRendererTest::onEnter()
{
    MaterialSystemBaseTest::onEnter();

    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(8.f);
    mesh->setTexture("MeshRendererTest/boss.png");
    this->addChild(mesh);
    mesh->setPositionNormalized(Vec2(0.5f, 0.5f));
}

std::string Material_MeshRendererTest::subtitle() const
{
    return "Material System on MeshRenderer";
}

void Material_MultipleMeshRenderer::onEnter()
{
    MaterialSystemBaseTest::onEnter();

    const char* names[] = {
        "MeshRendererTest/ReskinGirl.c3b", "MeshRendererTest/ReskinGirl.c3b", "MeshRendererTest/ReskinGirl.c3b",
        "MeshRendererTest/ReskinGirl.c3b", "MeshRendererTest/ReskinGirl.c3b", "MeshRendererTest/ReskinGirl.c3b",
        "MeshRendererTest/ReskinGirl.c3b", "MeshRendererTest/ReskinGirl.c3b",
    };

    const int totalNames = sizeof(names) / sizeof(names[0]);

    auto size = Director::getInstance()->getWinSize();

    for (int i = 0; i < totalNames; i++)
    {
        auto mesh = MeshRenderer::create(names[i]);
        this->addChild(mesh);
        mesh->setPosition(Vec2((size.width / (totalNames + 1)) * (i + 1), size.height / 4));
        mesh->setScale(3);
    }
}

std::string Material_MultipleMeshRenderer::subtitle() const
{
    return "Sprites with multiple meshes";
}

//
// MARK: Material_2DEffects
//
void Material_2DEffects::onEnter()
{
    MaterialSystemBaseTest::onEnter();

    auto properties = Properties::createNonRefCounted("Materials/2d_effects.material#sample");

    // Print the properties of every namespace within this one.
    printProperties(properties, 0);

    Material* mat1 = Material::createWithProperties(properties);

    auto meshBlur = Sprite::create("Images/grossini.png");
    meshBlur->setPositionNormalized(Vec2(0.2f, 0.5f));
    this->addChild(meshBlur);
    meshBlur->setProgramState(mat1->getTechniqueByName("blur")->getPassByIndex(0)->getProgramState());

    auto meshOutline = Sprite::create("Images/grossini.png");
    meshOutline->setPositionNormalized(Vec2(0.4f, 0.5f));
    this->addChild(meshOutline);
    meshOutline->setProgramState(mat1->getTechniqueByName("outline")->getPassByIndex(0)->getProgramState());

    auto meshNoise = Sprite::create("Images/grossini.png");
    meshNoise->setPositionNormalized(Vec2(0.6f, 0.5f));
    this->addChild(meshNoise);
    meshNoise->setProgramState(mat1->getTechniqueByName("noise")->getPassByIndex(0)->getProgramState());

    auto meshEdgeDetect = Sprite::create("Images/grossini.png");
    meshEdgeDetect->setPositionNormalized(Vec2(0.8f, 0.5f));
    this->addChild(meshEdgeDetect);
    meshEdgeDetect->setProgramState(mat1->getTechniqueByName("edge_detect")->getPassByIndex(0)->getProgramState());

    timeUniforms.clear();

#define FETCH_CCTIME_LOCATION(mesh)                                   \
    do                                                                  \
    {                                                                   \
        auto programState = mesh->getProgramState();                  \
        auto location     = programState->getUniformLocation("u_Time"); \
        timeUniforms.emplace_back(programState, location);              \
    } while (0)

    FETCH_CCTIME_LOCATION(meshBlur);
    FETCH_CCTIME_LOCATION(meshOutline);
    FETCH_CCTIME_LOCATION(meshNoise);
    FETCH_CCTIME_LOCATION(meshEdgeDetect);

    schedule(AX_SCHEDULE_SELECTOR(Material_2DEffects::updateCCTimeUniforms));

    // properties is not a "Object" object
    AX_SAFE_DELETE(properties);
}

std::string Material_2DEffects::subtitle() const
{
    return "Testing effects on Sprite";
}

void Material_2DEffects::updateCCTimeUniforms(float)
{
    float time = Director::getInstance()->getTotalFrames() * Director::getInstance()->getAnimationInterval();
    Vec4 random(time / 10.0f, time, time * 2.0f, time * 4.0f);
    for (auto&& loc : timeUniforms)
    {
        loc.programState->setUniform(loc.location, &random, sizeof(random));
    }
}

//
// MARK: Material_AutoBindings
//

/*
 * Custom material auto-binding resolver for terrain.
 */
class EffectAutoBindingResolver : public backend::ProgramState::AutoBindingResolver
{
    virtual bool resolveAutoBinding(backend::ProgramState* programState,
                                    /* Node* node,*/ std::string_view uniform,
                                    std::string_view autoBinding) override;

    void callbackRadius(backend::ProgramState* programState, backend::UniformLocation uniform);
    void callbackColor(backend::ProgramState* programState, backend::UniformLocation uniform);
};

bool EffectAutoBindingResolver::resolveAutoBinding(backend::ProgramState* programState,
                                                   /*Node* node,*/ std::string_view uniform,
                                                   std::string_view autoBinding)
{
    if (autoBinding.compare("DYNAMIC_RADIUS") == 0)
    {
        auto loc = programState->getUniformLocation(uniform);
        programState->setCallbackUniform(loc, AX_CALLBACK_2(EffectAutoBindingResolver::callbackRadius, this));
        return true;
    }
    else if (autoBinding.compare("OUTLINE_COLOR") == 0)
    {
        auto loc = programState->getUniformLocation(uniform);
        programState->setCallbackUniform(loc, AX_CALLBACK_2(EffectAutoBindingResolver::callbackColor, this));
        return true;
    }
    return false;
}

void EffectAutoBindingResolver::callbackRadius(backend::ProgramState* programState, backend::UniformLocation uniform)
{
    float f = AXRANDOM_0_1() * 10;
    programState->setUniform(uniform, &f, sizeof(f));
}

void EffectAutoBindingResolver::callbackColor(backend::ProgramState* programState, backend::UniformLocation uniform)
{
    float r = AXRANDOM_0_1();
    float g = AXRANDOM_0_1();
    float b = AXRANDOM_0_1();
    Vec3 color(r, g, b);

    programState->setUniform(uniform, &color, sizeof(color));
}

Material_AutoBindings::Material_AutoBindings()
{
    _resolver = new EffectAutoBindingResolver;
}

Material_AutoBindings::~Material_AutoBindings()
{
    delete _resolver;
}

void Material_AutoBindings::onEnter()
{
    MaterialSystemBaseTest::onEnter();

    //    auto properties = Properties::createNonRefCounted("Materials/2d_effects.material#sample");
    auto properties = Properties::createNonRefCounted("Materials/auto_binding_test.material#sample");

    // Print the properties of every namespace within this one.
    printProperties(properties, 0);

    Material* mat1 = Material::createWithProperties(properties);

    auto meshBlur = Sprite::create("Images/grossini.png");
    meshBlur->setPositionNormalized(Vec2(0.2f, 0.5f));
    this->addChild(meshBlur);
    meshBlur->setProgramState(mat1->getTechniqueByName("blur")->getPassByIndex(0)->getProgramState());

    auto meshOutline = Sprite::create("Images/grossini.png");
    meshOutline->setPositionNormalized(Vec2(0.4f, 0.5f));
    this->addChild(meshOutline);
    meshOutline->setProgramState(mat1->getTechniqueByName("outline")->getPassByIndex(0)->getProgramState());

    auto meshNoise = Sprite::create("Images/grossini.png");
    meshNoise->setPositionNormalized(Vec2(0.6f, 0.5f));
    this->addChild(meshNoise);
    meshNoise->setProgramState(mat1->getTechniqueByName("noise")->getPassByIndex(0)->getProgramState());

    auto meshEdgeDetect = Sprite::create("Images/grossini.png");
    meshEdgeDetect->setPositionNormalized(Vec2(0.8f, 0.5f));
    this->addChild(meshEdgeDetect);
    meshEdgeDetect->setProgramState(mat1->getTechniqueByName("edge_detect")->getPassByIndex(0)->getProgramState());

    _noiseProgramState = meshNoise->getProgramState();
    _locationTime      = _noiseProgramState->getUniformLocation("u_Time");

    schedule(AX_SCHEDULE_SELECTOR(Material_AutoBindings::updateUniformTime));
    // properties is not a "Object" object
    AX_SAFE_DELETE(properties);
}

std::string Material_AutoBindings::subtitle() const
{
    return "Testing auto-bindings uniforms";
}

void Material_AutoBindings::updateUniformTime(float dt)
{
    float time = Director::getInstance()->getTotalFrames() * Director::getInstance()->getAnimationInterval();
    Vec4 random(time / 10.0f, time, time * 2.0f, time * 4.0f);
    _noiseProgramState->setUniform(_locationTime, &random, sizeof(random));
}

//
//
//
void Material_setTechnique::onEnter()
{
    MaterialSystemBaseTest::onEnter();

    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(6);
    this->addChild(mesh);
    mesh->setPositionNormalized(Vec2(0.5f, 0.5f));
    _mesh = mesh;

    Material* mat = Material::createWithFilename("Materials/3d_effects.material");
    mesh->setMaterial(mat);

    // lights
    auto light1 = AmbientLight::create(Color3B::RED);
    addChild(light1);

    auto light2 = DirectionLight::create(Vec3(-1, 1, 0), Color3B::GREEN);
    addChild(light2);

    this->schedule(AX_CALLBACK_1(Material_setTechnique::changeMaterial, this), 1, "cookie");
    _techniqueState = 0;

    auto rot    = RotateBy::create(5, Vec3(30.0f, 60.0f, 270.0f));
    auto repeat = RepeatForever::create(rot);
    mesh->runAction(repeat);
}

std::string Material_setTechnique::subtitle() const
{
    return "Testing setTechnique()";
}

void Material_setTechnique::changeMaterial(float dt)
{
    // get it from Mesh 0
    switch (_techniqueState)
    {
    case 0:
        _mesh->getMaterial(0)->setTechnique("lit");
        break;
    case 1:
        _mesh->getMaterial(0)->setTechnique("normal");
        break;
    case 2:
        _mesh->getMaterial(0)->setTechnique("outline");
        break;
    default:
        break;
    }

    _techniqueState = (_techniqueState + 1) % 3;
}

//
//
//
void Material_clone::onEnter()
{
    MaterialSystemBaseTest::onEnter();

    auto mesh = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh->setScale(3);
    this->addChild(mesh);
    mesh->setPositionNormalized(Vec2(0.25f, 0.5f));

    Material* mat = Material::createWithFilename("Materials/3d_effects.material");
    mesh->setMaterial(mat);

    auto rot    = RotateBy::create(5, Vec3(360.0f, 240.0f, 120.0f));
    auto repeat = RepeatForever::create(rot);
    mesh->runAction(repeat);

    // mesh 2... using same material
    auto mesh2 = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh2->setScale(3);
    this->addChild(mesh2);
    mesh2->setPositionNormalized(Vec2(0.5f, 0.5f));
    mesh2->setMaterial(mat);
    mesh2->runAction(repeat->clone());

    // mesh 3... using cloned material
    auto mesh3 = MeshRenderer::create("MeshRendererTest/boss1.obj");
    mesh3->setScale(3);
    this->addChild(mesh3);
    mesh3->setPositionNormalized(Vec2(0.75f, 0.5f));
    auto mat2 = mat->clone();
    mesh3->setMaterial(mat2);
    mesh3->runAction(repeat->clone());

    // testing clone
    // should affect both mesh 1 and mesh 2
    mat->setTechnique("outline");

    // should affect only mesh 3
    //    mat2->setTechnique("normal");
}

std::string Material_clone::subtitle() const
{
    return "Testing material->clone()";
}

//
//
//
const int SHOW_LEBAL_TAG = 114;

void Material_parsePerformance::onEnter()
{
    MaterialSystemBaseTest::onEnter();

    _maxParsingCoumt = 5e3;

    auto screenSize = Director::getInstance()->getWinSize();

    ui::Slider* slider = ui::Slider::create();
    slider->loadBarTexture("cocosui/sliderTrack.png");
    slider->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
    slider->loadProgressBarTexture("cocosui/sliderProgress.png");
    slider->setPercent(50);

    slider->setPosition(Vec2(screenSize.width / 2.0f, screenSize.height / 3.0f));
    slider->addEventListener([&](Object* sender, ui::Slider::EventType type) {
        if (type == ui::Slider::EventType::ON_SLIDEBALL_UP)
        {
            ui::Slider* slider = dynamic_cast<ui::Slider*>(sender);
            float p            = slider->getPercent() / 100.0f;
            slider->setTouchEnabled(false);
            AXLOGD("Will parsing material {} times", (int)(p * _maxParsingCoumt));
            Label* label = dynamic_cast<Label*>(this->getChildByTag(SHOW_LEBAL_TAG));
            if (label)
            {
                label->setString("Testing start!");
            }
            this->scheduleOnce(
                [this, p, slider](float) {
                    this->parsingTesting(p * _maxParsingCoumt);
                    slider->setTouchEnabled(true);
                },
                1.0, "schedule test parsing");
        }
    });

    addChild(slider);

    auto label = Label::createWithSystemFont(
        "Max parsing count is 10000, which may crash because of high memory consumption.", "Helvetica", 10);
    label->setPosition(Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f - 20));
    addChild(label);
    label = Label::createWithSystemFont("Slide to test parsing performance", "Helvetica", 10);
    label->setPosition(Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f));
    addChild(label);

    label = Label::createWithSystemFont("", "Helvetica", 10);
    label->setPosition(Vec2(screenSize.width / 2.0f, screenSize.height / 2.0f + 20));
    label->setTag(SHOW_LEBAL_TAG);
    addChild(label);
}

void Material_parsePerformance::parsingTesting(unsigned int count)
{
    std::clock_t begin = std::clock();

    for (unsigned int i = 0; i < count; i++)
    {
        Material::createWithFilename("Materials/2d_effects.material");
        Material::createWithFilename("Materials/3d_effects.material");
    }

    std::clock_t end    = std::clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    Label* label        = dynamic_cast<Label*>(this->getChildByTag(SHOW_LEBAL_TAG));
    if (label)
    {
        std::string str = fmt::format("Testing completed! Took: {:.3} seconds for parsing material {} times.",
                                              elapsed_secs, count);
        label->setString(str);

        AXLOGD("Took: {:.3} seconds for parsing material {} times.", elapsed_secs, count);
    }
}

std::string Material_parsePerformance::subtitle() const
{
    return "Testing parsing performance";
}

// MARK: Helper functions

static void printProperties(Properties* properties, int indent)
{
    // Print the name and ID of the current namespace.
    const char* spacename = properties->getNamespace();
    const char* id        = properties->getId().data();
    char chindent[64];
    int i = 0;
    for (i = 0; i < indent * 2; i++)
        chindent[i] = ' ';
    chindent[i] = '\0';

    AXLOGD("{}Namespace: {}  ID: {}\n{}{{", chindent, spacename, id, chindent);

    // Print all properties in this namespace.
    const char* name  = properties->getNextProperty();
    const char* value = NULL;
    while (name != NULL)
    {
        value = properties->getString(name);
        AXLOGD("{}{} = {}", chindent, name, value);
        name = properties->getNextProperty();
    }

    Properties* space = properties->getNextNamespace();
    while (space != NULL)
    {
        printProperties(space, indent + 1);
        space = properties->getNextNamespace();
    }

    AXLOGD("{}}}\n", chindent);
}
