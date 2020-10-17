
#include "libccs30/libccs30.h"

#include "libccs30/WidgetReader/NodeReader/NodeReader.h"
#include "libccs30/WidgetReader/SingleNodeReader/SingleNodeReader.h"
#include "libccs30/WidgetReader/SpriteReader/SpriteReader.h"
#include "libccs30/WidgetReader/ParticleReader/ParticleReader.h"
#include "libccs30/WidgetReader/GameMapReader/GameMapReader.h"
#include "libccs30/WidgetReader/ProjectNodeReader/ProjectNodeReader.h"
#include "libccs30/WidgetReader/ComAudioReader/ComAudioReader.h"

#include "libccs30/WidgetReader/ButtonReader/ButtonReader.h"
#include "libccs30/WidgetReader/CheckBoxReader/CheckBoxReader.h"
#include "libccs30/WidgetReader/ImageViewReader/ImageViewReader.h"
#include "libccs30/WidgetReader/TextBMFontReader/TextBMFontReader.h"
#include "libccs30/WidgetReader/TextReader/TextReader.h"
#include "libccs30/WidgetReader/TextFieldReader/TextFieldReader.h"
#include "libccs30/WidgetReader/TextAtlasReader/TextAtlasReader.h"
#include "libccs30/WidgetReader/LoadingBarReader/LoadingBarReader.h"
#include "libccs30/WidgetReader/SliderReader/SliderReader.h"
#include "libccs30/WidgetReader/LayoutReader/LayoutReader.h"
#include "libccs30/WidgetReader/ScrollViewReader/ScrollViewReader.h"
#include "libccs30/WidgetReader/PageViewReader/PageViewReader.h"
#include "libccs30/WidgetReader/ListViewReader/ListViewReader.h"
#include "libccs30/WidgetReader/ArmatureNodeReader/ArmatureNodeReader.h"
#include "libccs30/WidgetReader/Node3DReader/Node3DReader.h"
#include "libccs30/WidgetReader/Sprite3DReader/Sprite3DReader.h"
#include "libccs30/WidgetReader/UserCameraReader/UserCameraReader.h"
#include "libccs30/WidgetReader/Particle3DReader/Particle3DReader.h"


namespace cocostudio
{
    void destroyCocosStudio()
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
        
        cocos2d::CSLoader::destroyInstance();
        
        
        ArmatureDataManager::destroyInstance();
        SceneReader::destroyInstance();
        ActionManagerEx::destroyInstance();
        GUIReader::destroyInstance();
    }
}
