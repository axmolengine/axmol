#ifndef __WEAKPTR_H__
#define __WEAKPTR_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class GObject;

class WeakPtr
{
public:
    WeakPtr();
    explicit WeakPtr(GObject* obj);
    explicit WeakPtr(const WeakPtr& other);
    explicit WeakPtr(WeakPtr&& other);
    ~WeakPtr();

    WeakPtr& operator= (const WeakPtr& other);
    WeakPtr& operator= (WeakPtr&& other);
    WeakPtr& operator= (GObject* obj);
    bool operator!= (const WeakPtr& v);
    bool operator!= (const WeakPtr& v) const;
    bool operator== (const WeakPtr& v);
    bool operator== (const WeakPtr& v) const;
    bool operator== (const GObject* v);
    bool operator== (const GObject* v) const { return ptr() == v; }

    GObject* ptr() const;
    bool onStage() const;

private:
    uint64_t _id;

    static uint64_t add(GObject * obj);
    static GObject* remove(uint64_t id);
    static void markDisposed(GObject* obj);

    friend class GObject;
};

NS_FGUI_END

#endif