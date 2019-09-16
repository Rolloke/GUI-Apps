#include <menu.h>

#include <Arduino.h>

MenuItem::MenuItem(const char*aText, pFunction aFunction, uint8_t aID)
: mText(aText)
, mFunction(aFunction)
, mSub(0)
, mLength(0)
, mIndex(0)
, mFlag(aID)
{
}

MenuItem::MenuItem(const char*aText, MenuItem* aSub, uint8_t aLength)
: mText(aText)
, mFunction(0)
, mSub(aSub)
, mLength(aLength)
, mIndex(0)
, mFlag(0)
{
    if (aText == 0) mFlag = submenu;
}

String MenuItem::getText() const
{
    if (mFlag & submenu)
    {
        return mSub[mIndex].getText();
    }
    else
    {
        String fText;
        if (mFunction)
        {
            fText  = mText;
            if (mFlag & edit) fText += "|";
            fText += mFunction(MenuItem::dir::text, mFlag & idmask);
            if (mFlag & edit) fText += "|";
            return fText;
        }
        return mText;
    }
    return "";
}

void MenuItem::pressBtn(MenuItem::dir::to aDir, MenuItem* aParent)
{
    if (aDir == MenuItem::dir::enter)
    {
        if (isFlagSet(submenu))
        {
            mSub[mIndex].pressBtn(aDir);
        }
        else if (mSub && !isFlagSet(submenu))
        {
            setFlag(submenu, true);
        }
        else if (isFlagSet(edit) || isFlagSet(command))
        {
            mFunction(MenuItem::dir::enter, mFlag & idmask);
            if (!isFlagSet(command)) setFlag(edit, false);
        }
        else if (mFunction && isFlagSet(editable) && !isFlagSet(edit))
        {
            mFunction(MenuItem::dir::begin_edit, mFlag & idmask);
            setFlag(edit, true);
        }
    }
    else if (isFlagSet(submenu))
    {
        mSub[mIndex].pressBtn(aDir, this);
    }
    else if (isFlagSet(edit))
    {
        mFunction(aDir, mFlag & idmask);
        if (aDir == MenuItem::dir::escape)
        {
            setFlag(edit, false);
        }
    }
    else if (aParent)
    {
        switch (aDir)
        {
        case MenuItem::dir::left:
            ++aParent->mIndex;
            if (aParent->mIndex == aParent->mLength) aParent->mIndex = 0;
            break;
        case MenuItem::dir::right:
            if (aParent->mIndex == 0) aParent->mIndex = aParent->mLength - 1;
            else --aParent->mIndex;
            break;
        case MenuItem::dir::up:
        case MenuItem::dir::escape:
            if (aParent && aParent->isFlagSet(submenu) && aParent->mText)
            {
                aParent->setFlag(submenu, false);
            }
            break;
        case MenuItem::dir::down:
            if (mSub && !isFlagSet(submenu))
            {
                setFlag(submenu, true);
            }
            break;
        default:
            break;
        }
    }
}

bool MenuItem::isFlagSet(eFlags aFlag) const
{
    return (mFlag & aFlag) != 0;
}

void MenuItem::setFlag(eFlags aFlag, bool aSet)
{
    if (aSet)
    {
        mFlag |= aFlag;
    }
    else
    {
        mFlag &= ~aFlag;
    }
}
