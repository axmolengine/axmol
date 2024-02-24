#ifndef __GTREE_H__
#define __GTREE_H__

#include "FairyGUIMacros.h"
#include "GList.h"
#include "GTreeNode.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GList;
class GComponent;

class GTree : public GList
{
public:
    typedef std::function<void(GTreeNode* node, GComponent* obj)> TreeNodeRenderFunction;
    typedef std::function<void(GTreeNode* node, bool expand)> TreeNodeWillExpandFunction;

    GTree();
    virtual ~GTree();

    CREATE_FUNC(GTree);

    int getIndent() const { return _indent; }
    void setIndent(int value) { _indent = value; }
    int getClickToExpand() const { return _clickToExpand; }
    void setClickToExpand(int value) { _clickToExpand = value; }

    GTreeNode* getRootNode() const { return _rootNode; }
    GTreeNode* getSelectedNode() const;
    void getSelectedNodes(std::vector<GTreeNode*>& result) const;
    void selectNode(GTreeNode* node, bool scrollItToView = false);
    void unselectNode(GTreeNode* node);
    void expandAll(GTreeNode* folderNode);
    void collapseAll(GTreeNode* folderNode);

    TreeNodeRenderFunction treeNodeRender;
    TreeNodeWillExpandFunction treeNodeWillExpand;

protected:
    virtual void handleInit() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;
    virtual void readItems(ByteBuffer* buffer) override;
    virtual void dispatchItemEvent(GObject* item, EventContext* context) override;

private:
    void createCell(GTreeNode* node);
    void afterInserted(GTreeNode* node);
    int getInsertIndexForNode(GTreeNode* node);
    void afterRemoved(GTreeNode* node);
    void afterExpanded(GTreeNode* node);
    void afterCollapsed(GTreeNode* node);
    void afterMoved(GTreeNode* node);
    int checkChildren(GTreeNode* folderNode, int index);
    void hideFolderNode(GTreeNode* folderNode);
    void removeNode(GTreeNode* node);
    int getFolderEndIndex(int startIndex, int level);

    void onCellTouchBegin(EventContext* context);
    void onExpandedStateChanged(EventContext* context);

    int _indent;
    GTreeNode* _rootNode;
    int _clickToExpand;
    bool _expandedStatusInEvt;

    friend class GTreeNode;
};

NS_FGUI_END

#endif
