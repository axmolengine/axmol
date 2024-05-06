#ifndef __GTREENODE_H__
#define __GTREENODE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GTree;
class GComponent;

class GTreeNode : public ax::Object
{
public:
    static GTreeNode* create(bool isFolder = false, const std::string& resURL = ax::STD_STRING_EMPTY);

    GTreeNode();
    virtual ~GTreeNode();

    GTreeNode* getParent() const { return _parent; }
    GTree* getTree() const { return _tree; }
    GComponent* getCell() const { return _cell; }
    const ax::Value& getData() const { return _data; }
    void setData(const ax::Value& value) { _data = value; }
    bool isExpanded() const { return _expanded; }
    void setExpaned(bool value);
    bool isFolder() const { return _isFolder; }
    const std::string& getText() const;
    void setText(const std::string& value);
    const std::string& getIcon() const;
    void setIcon(const std::string& value);

    GTreeNode* addChild(GTreeNode* child);
    GTreeNode* addChildAt(GTreeNode* child, int index);

    void removeChild(GTreeNode* child);
    void removeChildAt(int index);
    void removeChildren() { removeChildren(0, -1); }
    void removeChildren(int beginIndex, int endIndex);

    GTreeNode* getChildAt(int index) const;
    GTreeNode* getPrevSibling() const;
    GTreeNode* getNextSibling() const;

    int getChildIndex(const GTreeNode* child) const;
    void setChildIndex(GTreeNode* child, int index);
    int setChildIndexBefore(GTreeNode* child, int index);
    void swapChildren(GTreeNode* child1, GTreeNode* child2);
    void swapChildrenAt(int index1, int index2);

    int numChildren() const;

private:
    bool init(bool isFolder, const std::string& resURL);
    int moveChild(GTreeNode* child, int oldIndex, int index);
    void setTree(GTree* value);

    GTree* _tree;
    GTreeNode* _parent;
    GComponent* _cell;
    int _level;
    bool _expanded;
    bool _isFolder;
    ax::Value _data;
    ax::Vector<GTreeNode*> _children;
    std::string _resURL;

    friend class GTree;
};

NS_FGUI_END

#endif
