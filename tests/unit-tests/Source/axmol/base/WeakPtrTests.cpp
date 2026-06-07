/****************************************************************************

 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include <doctest.h>
#include "axmol/base/WeakPtr.h"
#include "axmol/base/RefPtr.h"
#include "axmol/base/Object.h"

using namespace ax;

// Mock classes for testing
class Weapon;

class Character : public Object
{
public:
    RefPtr<Weapon> weapon;
    bool destroyed = false;
    ~Character() { destroyed = true; }
};

class Weapon : public Object
{
public:
    WeakPtr<Character> owner;
    bool destroyed = false;
    ~Weapon() { destroyed = true; }
};

class DisposeProbe : public Object
{
public:
    WeakPtr<DisposeProbe>* weak = nullptr;
    bool* weakValidInDispose    = nullptr;
    bool* lockWasNullInDispose  = nullptr;

protected:
    void dispose() override
    {
        *weakValidInDispose   = !weak->expired();
        *lockWasNullInDispose = weak->lock().get() == nullptr;
        Object::dispose();
    }
};

TEST_SUITE("base/WeakPtr")
{
    TEST_CASE("CircularReferenceStability")
    {
        // 1. Setup the scene
        auto hero  = new Character();
        auto sword = new Weapon();

        // 2. Establish strong reference: Character owns Weapon
        hero->weapon = sword;

        // 3. Establish weak reference: Weapon knows Owner
        sword->owner = hero;

        // 4. Verify initial state
        CHECK(hero->weapon.get() == sword);
        CHECK(!sword->owner.expired());
        CHECK(sword->owner.get() == hero);

        // 5. Test lock() functionality
        {
            RefPtr<Character> strongOwner = sword->owner.lock();
            REQUIRE(strongOwner != nullptr);
            CHECK(strongOwner.get() == hero);
        }

        // 6. Destroy the Character (owner)
        // Since the sword is the owner's child (RefPtr),
        // releasing hero drops that ownership and invalidates the weak owner link.
        hero->release();

        // 7. Verification: hero should be dead, and sword's weak reference should be invalid
        CHECK(sword->owner.expired());
        CHECK(sword->owner.get() == nullptr);

        // Verify that lock() returns an empty RefPtr
        RefPtr<Character> lockedHero = sword->owner.lock();
        CHECK(lockedHero.get() == nullptr);

        sword->release();
    }

    TEST_CASE("SlotReuseSafety")
    {
        // This test ensures that when a memory slot is reused,
        // the WeakPtr correctly identifies the invalidation via serialNumber.

        auto obj1 = new Character();
        WeakPtr<Character> weakRef(obj1);

        // Verify valid
        CHECK(!weakRef.expired());

        obj1->release();

        // Verify invalidated
        CHECK(weakRef.expired());

        // Create a new object, which will likely reuse the same _weakSlot
        auto obj2 = new Character();

        WeakPtr<Character> weakPtr2(obj2);

        // The WeakPtr should still be invalid because the serialNumber has incremented
        CHECK(weakRef.expired());

        obj2->release();

        CHECK(weakPtr2.expired());
    }

    TEST_CASE("InvalidBeforeDispose")
    {
        auto obj = new DisposeProbe();
        WeakPtr<DisposeProbe> weak(obj);

        bool weakValidInDispose   = true;
        bool lockWasNullInDispose = false;
        obj->weak                 = &weak;
        obj->weakValidInDispose   = &weakValidInDispose;
        obj->lockWasNullInDispose = &lockWasNullInDispose;

        obj->release();

        CHECK(weakValidInDispose == false);
        CHECK(lockWasNullInDispose == true);
        CHECK(weak.expired());
    }
}
