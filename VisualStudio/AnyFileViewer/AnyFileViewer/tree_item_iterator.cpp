#include "StdAfx.h"
#include "tree_item_iterator.h"


tree_item_iterator::tree_item_iterator(CTreeCtrl&aCtrl, HTREEITEM aItem) : 
      mCtrl(aCtrl), mItem(aItem) 
{
}

tree_item_iterator::tree_item_iterator(const tree_item_iterator& aIter) : 
mCtrl(aIter.mCtrl), mItem(aIter.mItem), mParents(aIter.mParents) 
{
}

tree_item_iterator& tree_item_iterator::operator++()
{
    if (mItem != NULL)
    {
        HTREEITEM fItem = mCtrl.GetNextItem(mItem, TVGN_CHILD);
        if (fItem)
        {
            mParents.push_back(mItem);
            mItem = fItem;
        }
        else
        {
            do
            {
                fItem =  mCtrl.GetNextItem(mItem, TVGN_NEXT);
                if (fItem)
                {
                    mItem = fItem;
                    return (*this); 
                }
                else if (mParents.size())
                {
                    mItem = mParents.back();
                    if (mItem == TVI_ROOT) mItem = 0;
                    mParents.pop_back();
                }
                else
                {
                    break;
                }
            } while(fItem == 0 && mItem != 0);
        }
    }
    return (*this); 
}

tree_item_iterator tree_item_iterator::operator=(const tree_item_iterator& _Right) 
{
    mItem    = _Right.mItem;
    mParents = _Right.mParents;
    return *this;
}
