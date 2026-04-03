
#include "sceneio/sceneio.h"
#include "axmol/axmolver.h"

#include "sceneio/WidgetReader/NodeReader/NodeReader.h"
#include "sceneio/WidgetReader/SingleNodeReader/SingleNodeReader.h"
#include "sceneio/WidgetReader/SpriteReader/SpriteReader.h"
#include "sceneio/WidgetReader/ParticleReader/ParticleReader.h"
#include "sceneio/WidgetReader/GameMapReader/GameMapReader.h"
#include "sceneio/WidgetReader/ProjectNodeReader/ProjectNodeReader.h"
#include "sceneio/WidgetReader/ComAudioReader/ComAudioReader.h"

#include "sceneio/WidgetReader/ButtonReader/ButtonReader.h"
#include "sceneio/WidgetReader/CheckBoxReader/CheckBoxReader.h"
#include "sceneio/WidgetReader/ImageViewReader/ImageViewReader.h"
#include "sceneio/WidgetReader/TextBMFontReader/TextBMFontReader.h"
#include "sceneio/WidgetReader/TextReader/TextReader.h"
#include "sceneio/WidgetReader/TextFieldReader/TextFieldReader.h"
#include "sceneio/WidgetReader/TextAtlasReader/TextAtlasReader.h"
#include "sceneio/WidgetReader/LoadingBarReader/LoadingBarReader.h"
#include "sceneio/WidgetReader/SliderReader/SliderReader.h"
#include "sceneio/WidgetReader/LayoutReader/LayoutReader.h"
#include "sceneio/WidgetReader/ScrollViewReader/ScrollViewReader.h"
#include "sceneio/WidgetReader/PageViewReader/PageViewReader.h"
#include "sceneio/WidgetReader/ListViewReader/ListViewReader.h"
#include "sceneio/WidgetReader/ArmatureNodeReader/ArmatureNodeReader.h"
#include "sceneio/WidgetReader/Node3DReader/Node3DReader.h"
#include "sceneio/WidgetReader/Sprite3DReader/Sprite3DReader.h"
#include "sceneio/WidgetReader/UserCameraReader/UserCameraReader.h"
#include "sceneio/WidgetReader/Particle3DReader/Particle3DReader.h"

namespace ax::ext
{
SCNIO_API bool __isSceneIOReady = false;

SCNIO_API void initializeSceneIO()
{
    if (__isSceneIOReady)
        return;
    __isSceneIOReady = true;

    ObjectFactory* factoryCreate = ObjectFactory::getInstance();

    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(ButtonReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(CheckBoxReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(SliderReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(ImageViewReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(LoadingBarReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(TextAtlasReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(TextReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(TextBMFontReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(TextFieldReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(LayoutReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(PageViewReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(ScrollViewReader));
    factoryCreate->registerType(CREATE_CLASS_WIDGET_READER_INFO(ListViewReader));
}

SCNIO_API void cleanupSceneIO()
{
    if (!__isSceneIOReady)
        return;
    __isSceneIOReady = false;

    ObjectFactory* factoryCreate = ObjectFactory::getInstance();

    factoryCreate->unregisterType(_AXSTR(ButtonReader));
    factoryCreate->unregisterType(_AXSTR(CheckBoxReader));
    factoryCreate->unregisterType(_AXSTR(SliderReader));
    factoryCreate->unregisterType(_AXSTR(ImageViewReader));
    factoryCreate->unregisterType(_AXSTR(LoadingBarReader));
    factoryCreate->unregisterType(_AXSTR(TextAtlasReader));
    factoryCreate->unregisterType(_AXSTR(TextReader));
    factoryCreate->unregisterType(_AXSTR(TextBMFontReader));
    factoryCreate->unregisterType(_AXSTR(TextFieldReader));
    factoryCreate->unregisterType(_AXSTR(LayoutReader));
    factoryCreate->unregisterType(_AXSTR(PageViewReader));
    factoryCreate->unregisterType(_AXSTR(ScrollViewReader));
    factoryCreate->unregisterType(_AXSTR(ListViewReader));

    NodeReader::destroyInstance();
    SingleNodeReader::destroyInstance();
    SpriteReader::destroyInstance();
    ParticleReader::destroyInstance();
    GameMapReader::destroyInstance();
    ProjectNodeReader::destroyInstance();
    ComAudioReader::destroyInstance();

    WidgetReader::destroyInstance();
    ButtonReader::destroyInstance();
    CheckBoxReader::destroyInstance();
    ImageViewReader::destroyInstance();
    TextBMFontReader::destroyInstance();
    TextReader::destroyInstance();
    TextFieldReader::destroyInstance();
    TextAtlasReader::destroyInstance();
    LoadingBarReader::destroyInstance();
    SliderReader::destroyInstance();
    LayoutReader::destroyInstance();
    ScrollViewReader::destroyInstance();
    PageViewReader::destroyInstance();
    ListViewReader::destroyInstance();

    ArmatureNodeReader::destroyInstance();
    Node3DReader::destroyInstance();
    Sprite3DReader::destroyInstance();
    UserCameraReader::destroyInstance();
    Particle3DReader::destroyInstance();

    ax::CSLoader::destroyInstance();

    ArmatureDataManager::destroyInstance();
    SceneReader::destroyInstance();
    ActionManagerEx::destroyInstance();
    GUIReader::destroyInstance();
}
}  // namespace ax::ext
