
#include "CocoStudio.h"

#include "WidgetReader/NodeReader/NodeReader.h"
#include "WidgetReader/SingleNodeReader/SingleNodeReader.h"
#include "WidgetReader/SpriteReader/SpriteReader.h"
#include "WidgetReader/ParticleReader/ParticleReader.h"
#include "WidgetReader/GameMapReader/GameMapReader.h"
#include "WidgetReader/ProjectNodeReader/ProjectNodeReader.h"
#include "WidgetReader/ComAudioReader/ComAudioReader.h"

#include "WidgetReader/ButtonReader/ButtonReader.h"
#include "WidgetReader/CheckBoxReader/CheckBoxReader.h"
#include "WidgetReader/ImageViewReader/ImageViewReader.h"
#include "WidgetReader/TextBMFontReader/TextBMFontReader.h"
#include "WidgetReader/TextReader/TextReader.h"
#include "WidgetReader/TextFieldReader/TextFieldReader.h"
#include "WidgetReader/TextAtlasReader/TextAtlasReader.h"
#include "WidgetReader/LoadingBarReader/LoadingBarReader.h"
#include "WidgetReader/SliderReader/SliderReader.h"
#include "WidgetReader/LayoutReader/LayoutReader.h"
#include "WidgetReader/ScrollViewReader/ScrollViewReader.h"
#include "WidgetReader/PageViewReader/PageViewReader.h"
#include "WidgetReader/ListViewReader/ListViewReader.h"
#include "WidgetReader/ArmatureNodeReader/ArmatureNodeReader.h"
#include "WidgetReader/Node3DReader/Node3DReader.h"
#include "WidgetReader/Sprite3DReader/Sprite3DReader.h"
#include "WidgetReader/UserCameraReader/UserCameraReader.h"
#include "WidgetReader/Particle3DReader/Particle3DReader.h"

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
