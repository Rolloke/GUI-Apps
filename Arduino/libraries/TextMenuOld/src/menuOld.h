#ifndef MENU_H
#define MENU_H

struct MenuItem;

#include <inttypes.h>
#include <Arduino.h>


typedef String (*pFunction)(uint8_t aDir, uint8_t aID);

#define length_of(X) sizeof(X) / sizeof(X[0])

class MenuItem
{
public:
    struct dir
    {
      enum to {unknown, left, right, up, down, enter, escape, begin_edit, text };
    };

    enum eFlags : uint8_t
    {
        idmask   = 0x0f,
        submenu  = 0x10,
        edit     = 0x20,
        flagmask = 0xC0,
        editable = 0x40,
        command  = 0x80
    };

    MenuItem(const char*aText, MenuItem* aSub, uint8_t aLength);
    MenuItem(const char*aText, pFunction aFunction=0, uint8_t aID=editable);

    String getText() const;
    void pressBtn(MenuItem::dir::to aDir, MenuItem* aParent=0);

private:
    bool isFlagSet(eFlags aFlag) const;
    void setFlag(eFlags aFlag, bool aSet);

    const char *mText;
    pFunction mFunction;
    MenuItem *mSub;
    uint8_t mLength;
    uint8_t mIndex;
    uint8_t mFlag;
};


#endif // MENU_H
