
#include "sceneio/sceneio.h"

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
SCNIO_API void shutdownSceneIO()
{
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
