#ifndef MENU_H
#define MENU_H

#include <inttypes.h>
#include <Arduino.h>


class ItemEditBase;
class MenuItem;

union Subfunction
{
    ItemEditBase* mItem;
    MenuItem*     mSubMenu;
};

#define length_of(X) sizeof(X) / sizeof(X[0])

typedef void (*pFunction)(uint8_t aDir, uint8_t aID);

class MenuItem
{
    friend class ItemSelect;
    friend class ItemEditBase;
    template <class Type>
    friend class ItemValue;
public:
    struct dir
    {
      enum to {unknown, left, right, up, down, enter, escape, begin_edit, text };
    };

    enum eFlags : uint8_t
    {
        submenu    = 0x01,
        edit       = 0x02,
        flagmask   = 0xFC,
        editable   = 0x04,
        command    = 0x08,
        limit_turn = 0x10,
        float_val  = 0x20,
        no_cursor  = 0x40,
        item_edit  = 0x80
    };

    MenuItem(const char*aText, MenuItem* aSub, int8_t aLength);
    MenuItem(const char*aText, ItemEditBase* aItemEdit=0, uint8_t aID=0, int8_t aLength=0, uint8_t aFlag=0);

    String getText(int* aCursorPos = 0) const;
    String getPath() const;
    void pressBtn(MenuItem::dir::to aDir, MenuItem* aParent=0);

    static void setNotificationFunction(pFunction aNotificationFunction);

private:
    bool isFlagSet(eFlags aFlag) const;
    void setFlag(eFlags aFlag, bool aSet);

    const char*   mText;
    Subfunction   mSubFunction;
    int8_t        mLength;
    int8_t        mIndex;
    uint8_t       mFlag;
    static pFunction mNotify;
};

template <class Type>
Type limitValue(Type aValue, Type aMin, Type aMax, bool aTurn)
{
    if (aValue < aMin)
    {
      aValue = aTurn ? aMax : aMin;
    }
    else if (aValue > aMax)
    {
      aValue = aTurn ? aMin : aMax;
    }
    return aValue;
}

class ItemEditBase
{
    friend class MenuItem;

protected:
    ItemEditBase();
    int8_t         getIndex() const;
    int8_t         getLength() const;
    uint8_t        getID() const;
    bool           isFlagSet(MenuItem::eFlags aFlag) const;
    void           notify(uint8_t aDir);

    virtual String editItem(uint8_t aDir) = 0;
    virtual void init(MenuItem* aMenuItem, uint8_t aID);
    MenuItem* mMenuItem;
    uint8_t mID;
};

class ItemSelect : public ItemEditBase
{
public:
    ItemSelect(const char** aItems);
    void select(int aIndex);
    void select(const char* aName);

protected:
    String editItem(uint8_t aDir);
    void init(MenuItem* aMenuItem, uint8_t aID);

    const char** mItems;
};


template <class Type>
class ItemValue : public ItemEditBase
{
public:
    ItemValue(Type& aValue, Type aMin, Type aMax) : mValue(aValue), mMin(aMin), mMax(aMax)
    {  }
protected:
    Type   getIncrement()
    {
        Type fIncrement = 1;
        int8_t fIndex = mMenuItem->mIndex;
        if (fIndex > 0)
        {
            while (fIndex--) fIncrement *= 10;
        }
        else
        {
            while (fIndex++) fIncrement *= 0.1;
        }

        return fIncrement;
    }

    String editItem(uint8_t aDir)
    {
        static Type fOldValue=0;
        switch (aDir)
        {
          case MenuItem::dir::begin_edit: fOldValue = mValue;       break;
          case MenuItem::dir::escape    : mValue    = fOldValue;    break;
          case MenuItem::dir::enter     : notify(aDir);             break;
          case MenuItem::dir::up        : mValue += getIncrement(); break;
          case MenuItem::dir::down      : mValue -= getIncrement(); break;
          case MenuItem::dir::left      : --mMenuItem->mIndex;      break;
          case MenuItem::dir::right     : ++mMenuItem->mIndex;      break;
          case MenuItem::dir::text      : return String(mValue);
        }
        mMenuItem->mIndex = limitValue(getIndex(),
                                       static_cast<int8_t>(isFlagSet(MenuItem::float_val) ? -2 : 0),
                                       static_cast<int8_t>(getLength()-1), false);
        mValue = limitValue(mValue, mMin, mMax, mMenuItem->isFlagSet(MenuItem::limit_turn));

        return String("");
    }

    Type& mValue;
    Type  mMin;
    Type  mMax;
};

#endif // MENU_H
