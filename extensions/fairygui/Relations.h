#ifndef __RELATIONS_H__
#define __RELATIONS_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "RelationItem.h"

NS_FGUI_BEGIN

class GObject;
class ByteBuffer;

class Relations
{
public:
    Relations(GObject* owner);
    ~Relations();

    void add(GObject* target, RelationType relationType);
    void add(GObject* target, RelationType relationType, bool usePercent);
    void remove(GObject* target, RelationType relationType);
    bool contains(GObject* target);
    void clearFor(GObject* target);
    void clearAll();
    void copyFrom(const Relations& source);
    void onOwnerSizeChanged(float dWidth, float dHeight, bool applyPivot);
    bool isEmpty() const;
    void setup(ByteBuffer* buffer, bool parentToChild);

    GObject* handling;

private:
    GObject* _owner;
    std::vector<RelationItem*> _items;
};

NS_FGUI_END

#endif
