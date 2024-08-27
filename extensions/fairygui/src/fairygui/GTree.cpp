#include "GTree.h"
#include "GButton.h"
#include "GList.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GTree::GTree()
    : _rootNode(nullptr),
      _indent(30),
      _clickToExpand(0),
      _expandedStatusInEvt(false)
{
}

GTree::~GTree()
{
    AX_SAFE_RELEASE(_rootNode);
}

void GTree::handleInit()
{
    GList::handleInit();

    _rootNode = GTreeNode::create(true);
    _rootNode->setTree(this);
    _rootNode->setExpaned(true);
    _rootNode->retain();
}

GTreeNode* GTree::getSelectedNode() const
{
    int i = getSelectedIndex();
    if (i != -1)
        return getChildAt(i)->_treeNode;
    else
        return nullptr;
}

void GTree::getSelectedNodes(std::vector<GTreeNode*>& result) const
{
    std::vector<int> ids;
    getSelection(ids);
    for (auto& it : ids)
    {
        GTreeNode* node = getChildAt(it)->_treeNode;
        result.push_back(node);
    }
}

void GTree::selectNode(GTreeNode* node, bool scrollItToView)
{
    GTreeNode* parentNode = node->_parent;
    while (parentNode != nullptr && parentNode != _rootNode)
    {
        parentNode->setExpaned(true);
        parentNode = parentNode->_parent;
    }
    if (node->_cell != nullptr)
        addSelection(getChildIndex(node->_cell), scrollItToView);
}

void GTree::unselectNode(GTreeNode* node)
{
    if (node->_cell != nullptr)
        removeSelection(getChildIndex(node->_cell));
}

void GTree::expandAll(GTreeNode* folderNode)
{
    folderNode->setExpaned(true);
    for (auto& it : folderNode->_children)
    {
        if (it->isFolder())
            expandAll(it);
    }
}

void GTree::collapseAll(GTreeNode* folderNode)
{
    if (folderNode != _rootNode)
        folderNode->setExpaned(false);
    for (auto& it : folderNode->_children)
    {
        if (it->isFolder())
            collapseAll(it);
    }
}

void GTree::createCell(GTreeNode* node)
{
    const std::string& url = node->_resURL.empty() ? getDefaultItem() : node->_resURL;
    GComponent* child = getItemPool()->getObject(url)->as<GComponent>();
    AXASSERT(child, "Unable to create tree cell");
    child->_treeNode = node;
    if (node->_cell != child)
    {
        AX_SAFE_RELEASE(node->_cell);
        node->_cell = child;
        AX_SAFE_RETAIN(node->_cell);
    }

    GObject* indentObj = node->_cell->getChild("indent");
    if (indentObj != nullptr)
        indentObj->setWidth(static_cast<float>((node->_level - 1) * _indent));

    GController* cc;

    cc = child->getController("expanded");
    if (cc != nullptr)
    {
        cc->addEventListener(UIEventType::Changed, AX_CALLBACK_1(GTree::onExpandedStateChanged, this));
        cc->setSelectedIndex(node->isExpanded() ? 1 : 0);
    }

    cc = child->getController("leaf");
    if (cc != nullptr)
        cc->setSelectedIndex(node->isFolder() ? 0 : 1);

    if (node->isFolder())
        child->addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GTree::onCellTouchBegin, this));

    if (treeNodeRender != nullptr)
        treeNodeRender(node, child);
}

void GTree::afterInserted(GTreeNode* node)
{
    if (node->_cell == nullptr)
        createCell(node);

    int index = getInsertIndexForNode(node);
    addChildAt(node->_cell, index);
    if (treeNodeRender != nullptr)
        treeNodeRender(node, node->_cell);

    if (node->isFolder() && node->isExpanded())
        checkChildren(node, index);
}

int GTree::getInsertIndexForNode(GTreeNode* node)
{
    GTreeNode* prevNode = node->getPrevSibling();
    if (prevNode == nullptr)
        prevNode = node->getParent();
    int insertIndex;
    if (prevNode->_cell != nullptr)
        insertIndex = getChildIndex(prevNode->_cell) + 1;
    else
        insertIndex = 0;
    int myLevel = node->_level;
    int cnt = numChildren();
    for (int i = insertIndex; i < cnt; i++)
    {
        GTreeNode* testNode = getChildAt(i)->_treeNode;
        if (testNode->_level <= myLevel)
            break;

        insertIndex++;
    }

    return insertIndex;
}

void GTree::afterRemoved(GTreeNode* node)
{
    removeNode(node);
}

void GTree::afterExpanded(GTreeNode* node)
{
    if (node == _rootNode)
    {
        checkChildren(_rootNode, 0);
        return;
    }

    if (treeNodeWillExpand != nullptr)
        treeNodeWillExpand(node, true);

    if (node->_cell == nullptr)
        return;

    if (treeNodeRender != nullptr)
        treeNodeRender(node, node->_cell);

    GController* cc = node->_cell->getController("expanded");
    if (cc != nullptr)
        cc->setSelectedIndex(1);

    if (node->_cell->getParent() != nullptr)
        checkChildren(node, getChildIndex(node->_cell));
}

void GTree::afterCollapsed(GTreeNode* node)
{
    if (node == _rootNode)
    {
        checkChildren(_rootNode, 0);
        return;
    }

    if (treeNodeWillExpand != nullptr)
        treeNodeWillExpand(node, false);

    if (node->_cell == nullptr)
        return;

    if (treeNodeRender != nullptr)
        treeNodeRender(node, node->_cell);

    GController* cc = node->_cell->getController("expanded");
    if (cc != nullptr)
        cc->setSelectedIndex(0);

    if (node->_cell->getParent() != nullptr)
        hideFolderNode(node);
}

void GTree::afterMoved(GTreeNode* node)
{
    int startIndex = getChildIndex(node->_cell);
    int endIndex;
    if (node->isFolder())
        endIndex = getFolderEndIndex(startIndex, node->_level);
    else
        endIndex = startIndex + 1;
    int insertIndex = getInsertIndexForNode(node);
    int cnt = endIndex - startIndex;

    if (insertIndex < startIndex)
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* obj = getChildAt(startIndex + i);
            setChildIndex(obj, insertIndex + i);
        }
    }
    else
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* obj = getChildAt(startIndex);
            setChildIndex(obj, insertIndex);
        }
    }
}

int GTree::getFolderEndIndex(int startIndex, int level)
{
    int cnt = numChildren();
    for (int i = startIndex + 1; i < cnt; i++)
    {
        GTreeNode* node = getChildAt(i)->_treeNode;
        if (node->_level <= level)
            return i;
    }

    return cnt;
}

int GTree::checkChildren(GTreeNode* folderNode, int index)
{
    int cnt = folderNode->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        index++;
        GTreeNode* node = folderNode->getChildAt(i);
        if (node->_cell == nullptr)
            createCell(node);

        if (node->_cell && !(node->_cell->getParent()))
            addChildAt(node->_cell, index);

        if (node->isFolder() && node->isExpanded())
            index = checkChildren(node, index);
    }

    return index;
}

void GTree::hideFolderNode(GTreeNode* folderNode)
{
    int cnt = folderNode->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GTreeNode* node = folderNode->getChildAt(i);
        if (node->_cell != nullptr && node->_cell->getParent() != nullptr)
            removeChild(node->_cell);

        if (node->isFolder() && node->isExpanded())
            hideFolderNode(node);
    }
}

void GTree::removeNode(GTreeNode* node)
{
    if (node->_cell != nullptr)
    {
        if (node->_cell->getParent() != nullptr)
            removeChild(node->_cell);
        getItemPool()->returnObject(node->_cell);
        node->_cell->_treeNode = nullptr;
        node->_cell = nullptr;
    }

    if (node->isFolder())
    {
        int cnt = node->numChildren();
        for (int i = 0; i < cnt; i++)
        {
            GTreeNode* node2 = node->getChildAt(i);
            removeNode(node2);
        }
    }
}

void GTree::onCellTouchBegin(EventContext* context)
{
    GTreeNode* node = ((GObject*)context->getSender())->_treeNode;
    _expandedStatusInEvt = node->isExpanded();
}

void GTree::onExpandedStateChanged(EventContext* context)
{
    GController* cc = (GController*)context->getSender();
    GTreeNode* node = cc->getParent()->_treeNode;
    node->setExpaned(cc->getSelectedIndex() == 1);
}

void GTree::dispatchItemEvent(GObject* item, EventContext* context)
{
    if (_clickToExpand != 0)
    {
        GTreeNode* node = item->_treeNode;
        if (node != nullptr && _expandedStatusInEvt == node->isExpanded())
        {
            if (_clickToExpand == 2)
            {
                if (context->getInput()->isDoubleClick())
                    node->setExpaned(!node->isExpanded());
            }
            else
                node->setExpaned(!node->isExpanded());
        }
    }

    GList::dispatchItemEvent(item, context);
}

void GTree::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GList::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 9);

    _indent = buffer->readInt();
    _clickToExpand = buffer->readByte();
}

void GTree::readItems(ByteBuffer* buffer)
{
    int nextPos;
    std::string str;
    bool isFolder;
    GTreeNode* lastNode = nullptr;
    int level;
    int prevLevel = 0;

    int cnt = buffer->readShort();
    for (int i = 0; i < cnt; i++)
    {
        nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        str = buffer->readS();
        if (!str.empty())
        {
            str = getDefaultItem();
            if (str.empty())
            {
                buffer->setPos(nextPos);
                continue;
            }
        }

        isFolder = buffer->readBool();
        level = buffer->readByte();

        GTreeNode* node = GTreeNode::create(isFolder, str);
        node->setExpaned(true);
        if (i == 0)
            _rootNode->addChild(node);
        else
        {
            if (level > prevLevel)
                lastNode->addChild(node);
            else if (level < prevLevel)
            {
                for (int j = level; j <= prevLevel; j++)
                    lastNode = lastNode->getParent();
                lastNode->addChild(node);
            }
            else
                lastNode->getParent()->addChild(node);
        }
        lastNode = node;
        prevLevel = level;

        setupItem(buffer, node->_cell);

        buffer->setPos(nextPos);
    }
}

NS_FGUI_END
