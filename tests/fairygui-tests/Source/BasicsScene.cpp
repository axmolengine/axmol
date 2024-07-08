#include "BasicsScene.h"
#include "Window1.h"
#include "Window2.h"

USING_NS_AX;

void BasicsScene::continueInit()
{
    UIConfig::buttonSound = "ui://Basics/click";
    UIConfig::verticalScrollBar = "ui://Basics/ScrollBar_VT";
    UIConfig::horizontalScrollBar = "ui://Basics/ScrollBar_HZ";
    UIConfig::tooltipsWin = "ui://Basics/WindowFrame";
    UIConfig::popupMenu = "ui://Basics/PopupMenu";

    UIPackage::addPackage("UI/Basics");
    _view = UIPackage::createObject("Basics", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _backBtn = _view->getChild("btn_Back");
    _backBtn->setVisible(false);
    _backBtn->addClickListener(AX_CALLBACK_1(BasicsScene::onClickBack, this));

    _demoContainer = _view->getChild("container")->as<GComponent>();
    _cc = _view->getController("c1");

    int cnt = _view->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GObject* obj = _view->getChildAt(i);
        if (obj->getGroup() != nullptr && obj->getGroup()->name.compare("btns") == 0)
            obj->addClickListener(AX_CALLBACK_1(BasicsScene::runDemo, this));
    }
}

BasicsScene::BasicsScene()
    : _winA(nullptr),
      _winB(nullptr),
      _pm(nullptr),
      _popupCom(nullptr)
{
}

BasicsScene::~BasicsScene()
{
    AX_SAFE_RELEASE(_winA);
    AX_SAFE_RELEASE(_winB);
    AX_SAFE_RELEASE(_pm);
    AX_SAFE_RELEASE(_popupCom);
}

void BasicsScene::onClickBack(EventContext* context)
{
    _cc->setSelectedIndex(0);
    _backBtn->setVisible(false);
}

void BasicsScene::runDemo(EventContext* context)
{
    std::string type = ((GObject*)context->getSender())->name.substr(4);
    auto it = _demoObjects.find(type);
    GComponent* obj;
    if (it == _demoObjects.end())
    {
        obj = UIPackage::createObject("Basics", "Demo_" + type)->as<GComponent>();
        _demoObjects.insert(type, obj);
    }
    else
        obj = it->second;

    _demoContainer->removeChildren();
    _demoContainer->addChild(obj);
    _cc->setSelectedIndex(1);
    _backBtn->setVisible(true);

    if (type == "Text")
        playText();
    else if (type == "Depth")
        playDepth();
    else if (type == "Window")
        playWindow();
    else if (type == "Drag&Drop")
        playDragDrop();
    else if (type == "Popup")
        playPopup();
    else if (type == "ProgressBar")
        playProgress();
}

void BasicsScene::playText()
{
    GComponent* obj = _demoObjects.at("Text");
    obj->getChild("n12")->addEventListener(UIEventType::ClickLink, [this](EventContext* context) {
        GRichTextField* t = dynamic_cast<GRichTextField*>(context->getSender());
        t->setText("[img]ui://Basics/pet[/img][color=#FF0000]You click the link[/color]:" + context->getDataValue().asString());
    });
    obj->getChild("n25")->addClickListener([this, obj](EventContext* context) {
        obj->getChild("n24")->setText(obj->getChild("n22")->getText());
    });
}

void BasicsScene::playPopup()
{
    if (_pm == nullptr)
    {
        _pm = PopupMenu::create();
        _pm->retain();
        _pm->addItem("Item 1", AX_CALLBACK_1(BasicsScene::onClickMenu, this));
        _pm->addItem("Item 2", AX_CALLBACK_1(BasicsScene::onClickMenu, this));
        _pm->addItem("Item 3", AX_CALLBACK_1(BasicsScene::onClickMenu, this));
        _pm->addItem("Item 4", AX_CALLBACK_1(BasicsScene::onClickMenu, this));
    }

    if (_popupCom == nullptr)
    {
        _popupCom = UIPackage::createObject("Basics", "Component12")->as<GComponent>();
        _popupCom->retain();
        _popupCom->center();
    }
    GComponent* obj = _demoObjects.at("Popup");
    obj->getChild("n0")->addClickListener([this](EventContext* context) {
        _pm->show((GObject*)context->getSender(), PopupDirection::DOWN);
    });

    obj->getChild("n1")->addClickListener([this](EventContext* context) {
        UIRoot->showPopup(_popupCom);
    });

    obj->addEventListener(UIEventType::RightClick, [this](EventContext* context) {
        _pm->show();
    });
}

void BasicsScene::onClickMenu(EventContext* context)
{
    GObject* itemObject = (GObject*)context->getData();
    AXLOGD("click {}", itemObject->getText());
}

void BasicsScene::playWindow()
{
    GComponent* obj = _demoObjects.at("Window");
    if (_winA == nullptr)
    {
        _winA = Window1::create();
        _winA->retain();

        _winB = Window2::create();
        _winB->retain();

        obj->getChild("n0")->addClickListener([this](EventContext*) {
            _winA->show();
        });

        obj->getChild("n1")->addClickListener([this](EventContext*) {
            _winB->show();
        });
    }
}

Vec2 startPos;
void BasicsScene::playDepth()
{
    GComponent* obj = _demoObjects.at("Depth");
    GComponent* testContainer = obj->getChild("n22")->as<GComponent>();
    GObject* fixedObj = testContainer->getChild("n0");
    fixedObj->setSortingOrder(100);
    fixedObj->setDraggable(true);

    int numChildren = testContainer->numChildren();
    int i = 0;
    while (i < numChildren)
    {
        GObject* child = testContainer->getChildAt(i);
        if (child != fixedObj)
        {
            testContainer->removeChildAt(i);
            numChildren--;
        }
        else
            i++;
    }
    startPos = fixedObj->getPosition();

    obj->getChild("btn0")->addClickListener([obj](EventContext*) {
        GGraph* graph = GGraph::create();
        startPos.x += 10;
        startPos.y += 10;
        graph->setPosition(startPos.x, startPos.y);
        graph->drawRect(150, 150, 1, Color4F::BLACK, Color4F::RED);
        obj->getChild("n22")->as<GComponent>()->addChild(graph);
    },
                                            EventTag(this)); //avoid duplicate register

    obj->getChild("btn1")->addClickListener([obj](EventContext*) {
        GGraph* graph = GGraph::create();
        startPos.x += 10;
        startPos.y += 10;
        graph->setPosition(startPos.x, startPos.y);
        graph->drawRect(150, 150, 1, Color4F::BLACK, Color4F::GREEN);
        graph->setSortingOrder(200);
        obj->getChild("n22")->as<GComponent>()->addChild(graph);
    },
                                            EventTag(this));
}

void BasicsScene::playDragDrop()
{
    GComponent* obj = _demoObjects.at("Drag&Drop");
    obj->getChild("a")->setDraggable(true);

    GButton* b = obj->getChild("b")->as<GButton>();
    b->setDraggable(true);
    b->addEventListener(UIEventType::DragStart, [b](EventContext* context) {
        //Cancel the original dragging, and start a new one with a agent.
        context->preventDefault();

        DragDropManager::getInstance()->startDrag(b->getIcon(), Value(b->getIcon()), context->getInput()->getTouchId());
    });

    GButton* c = obj->getChild("c")->as<GButton>();
    c->setIcon("");
    c->addEventListener(UIEventType::Drop, [c](EventContext* context) {
        c->setIcon(context->getDataValue().asString());
    });

    GObject* bounds = obj->getChild("n7");
    Rect rect = bounds->transformRect(Rect(Vec2::ZERO, bounds->getSize()), _groot);

    //---!!Because at this time the container is on the right side of the stage and beginning to move to left(transition), so we need to caculate the final position
    rect.origin.x -= obj->getParent()->getX();
    //----

    GButton* d = obj->getChild("d")->as<GButton>();
    d->setDraggable(true);
    d->setDragBounds(rect);
}

void BasicsScene::playProgress()
{
    GComponent* obj = _demoObjects.at("ProgressBar");
    ax::Director::getInstance()->getScheduler()->schedule(
        AX_SCHEDULE_SELECTOR(BasicsScene::onPlayProgress), this, 0.02f, false);
    obj->addEventListener(UIEventType::Exit, [this](EventContext*) {
        ax::Director::getInstance()->getScheduler()->unschedule(AX_SCHEDULE_SELECTOR(BasicsScene::onPlayProgress), this);
    });
}

void BasicsScene::onPlayProgress(float dt)
{
    GComponent* obj = _demoObjects.at("ProgressBar");
    int cnt = obj->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GProgressBar* child = obj->getChildAt(i)->as<GProgressBar>();
        if (child != nullptr)
        {
            child->setValue(child->getValue() + 1);
            if (child->getValue() > child->getMax())
                child->setValue(child->getMin());
        }
    }
}
