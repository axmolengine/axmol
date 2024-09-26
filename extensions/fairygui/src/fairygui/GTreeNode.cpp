#include "GTreeNode.h"
#include "GComponent.h"
#include "GTree.h"

NS_FGUI_BEGIN
using namespace ax;

GTreeNode* GTreeNode::create(bool isFolder, const std::string& resURL)
{
    GTreeNode* pRet = new GTreeNode();

    if (pRet->init(isFolder, resURL))
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }

    return pRet;
}

GTreeNode::GTreeNode()
    : _tree(nullptr),
      _parent(nullptr),
      _cell(nullptr),
      _level(0),
      _expanded(false),
      _isFolder(false)
{
}

GTreeNode::~GTreeNode()
{
    for (auto& it : _children)
        it->_parent = nullptr;

    _children.clear();

    if (_parent)
        _parent->removeChild(this);

    AX_SAFE_RELEASE(_cell);
}

bool GTreeNode::init(bool isFolder, const std::string& resURL)
{
    _isFolder = isFolder;
    _resURL = resURL;

    return true;
}

void GTreeNode::setExpaned(bool value)
{
    if (!_isFolder)
        return;

    if (_expanded != value)
    {
        _expanded = value;
        if (_tree != nullptr)
        {
            if (_expanded)
                _tree->afterExpanded(this);
            else
                _tree->afterCollapsed(this);
        }
    }
}

const std::string& GTreeNode::getText() const
{
    if (_cell != nullptr)
        return _cell->getText();
    else
        return STD_STRING_EMPTY;
}

void GTreeNode::setText(const std::string& value)
{
    if (_cell != nullptr)
        return _cell->setText(value);
}

const std::string& GTreeNode::getIcon() const
{
    if (_cell != nullptr)
        return _cell->getIcon();
    else
        return STD_STRING_EMPTY;
}

void GTreeNode::setIcon(const std::string& value)
{
    if (_cell != nullptr)
        return _cell->setIcon(value);
}

GTreeNode* GTreeNode::addChild(GTreeNode* child)
{
    addChildAt(child, (int)_children.size());
    return child;
}

GTreeNode* GTreeNode::addChildAt(GTreeNode* child, int index)
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    if (child->_parent == this)
    {
        setChildIndex(child, index);
    }
    else
    {
        child->retain();
        if (child->_parent != nullptr)
            child->_parent->removeChild(child);
        child->_parent = this;

        int cnt = (int)_children.size();
        if (index == cnt)
            _children.pushBack(child);
        else
            _children.insert(index, child);
        child->release();

        child->_level = _level + 1;
        child->setTree(_tree);
        if ((_tree != nullptr && this == _tree->getRootNode()) || (_cell != nullptr && _cell->getParent() != nullptr && _expanded))
            _tree->afterInserted(child);
    }
    return child;
}

void GTreeNode::removeChild(GTreeNode* child)
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    int childIndex = (int)_children.getIndex(child);
    if (childIndex != -1)
        removeChildAt(childIndex);
}

void GTreeNode::removeChildAt(int index)
{
    AXASSERT(index >= 0 && index < _children.size(), "Invalid child index");

    GTreeNode* child = _children.at(index);
    child->_parent = nullptr;

    if (_tree != nullptr)
    {
        child->setTree(nullptr);
        _tree->afterRemoved(child);
    }

    _children.erase(index);
}

void GTreeNode::removeChildren(int beginIndex, int endIndex)
{
    if (endIndex < 0 || endIndex >= _children.size())
        endIndex = (int)_children.size() - 1;

    for (int i = beginIndex; i <= endIndex; ++i)
        removeChildAt(beginIndex);
}

GTreeNode* GTreeNode::getChildAt(int index) const
{
    AXASSERT(index >= 0 && index < _children.size(), "Invalid child index");

    return _children.at(index);
}

GTreeNode* GTreeNode::getPrevSibling() const
{
    if (_parent == nullptr)
        return nullptr;

    ssize_t i = _parent->_children.getIndex((GTreeNode*)this);
    if (i <= 0)
        return nullptr;

    return _parent->_children.at(i - 1);
}

GTreeNode* GTreeNode::getNextSibling() const
{
    if (_parent == nullptr)
        return nullptr;

    ssize_t i = _parent->_children.getIndex((GTreeNode*)this);
    if (i < 0 || i >= _parent->_children.size() - 1)
        return nullptr;

    return _parent->_children.at(i + 1);
}

int GTreeNode::getChildIndex(const GTreeNode* child) const
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    return (int)_children.getIndex((GTreeNode*)child);
}

void GTreeNode::setChildIndex(GTreeNode* child, int index)
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = (int)_children.getIndex(child);
    AXASSERT(oldIndex != -1, "Not a child of this container");

    moveChild(child, oldIndex, index);
}

int GTreeNode::setChildIndexBefore(GTreeNode* child, int index)
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = (int)_children.getIndex(child);
    AXASSERT(oldIndex != -1, "Not a child of this container");

    if (oldIndex < index)
        return moveChild(child, oldIndex, index - 1);
    else
        return moveChild(child, oldIndex, index);
}

int GTreeNode::moveChild(GTreeNode* child, int oldIndex, int index)
{
    int cnt = (int)_children.size();
    if (index > cnt)
        index = cnt;

    if (oldIndex == index)
        return oldIndex;

    child->retain();
    _children.erase(oldIndex);
    if (index >= cnt)
        _children.pushBack(child);
    else
        _children.insert(index, child);
    child->release();
    if ((_tree != nullptr && this == _tree->getRootNode()) || (_cell != nullptr && _cell->getParent() != nullptr && _expanded))
        _tree->afterMoved(child);

    return index;
}

void GTreeNode::swapChildren(GTreeNode* child1, GTreeNode* child2)
{
    AXASSERT(child1 != nullptr, "Argument1 must be non-nil");
    AXASSERT(child2 != nullptr, "Argument2 must be non-nil");

    int index1 = (int)_children.getIndex(child1);
    int index2 = (int)_children.getIndex(child2);

    AXASSERT(index1 != -1, "Not a child of this container");
    AXASSERT(index2 != -1, "Not a child of this container");

    swapChildrenAt(index1, index2);
}

void GTreeNode::swapChildrenAt(int index1, int index2)
{
    GTreeNode* child1 = _children.at(index1);
    GTreeNode* child2 = _children.at(index2);

    setChildIndex(child1, index2);
    setChildIndex(child2, index1);
}

int GTreeNode::numChildren() const
{
    return (int)_children.size();
}

void GTreeNode::setTree(GTree* value)
{
    _tree = value;
    if (_tree != nullptr && _tree->treeNodeWillExpand != nullptr && _expanded)
        _tree->treeNodeWillExpand(this, true);

    if (_isFolder)
    {
        for (auto& child : _children)
        {
            child->_level = _level + 1;
            child->setTree(value);
        }
    }
}

NS_FGUI_END
