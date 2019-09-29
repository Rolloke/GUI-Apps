#pragma once
#include <vector>

class tree_item_iterator :
    public std::iterator<std::forward_iterator_tag, HTREEITEM, size_t, HTREEITEM*, HTREEITEM&>
{
public:
    tree_item_iterator(CTreeCtrl&aCtrl, HTREEITEM aItem);
    tree_item_iterator(const tree_item_iterator& aIter);

    /// reference
    tree_item_iterator& operator*()  { return *this; }
    /// postincrement
    tree_item_iterator operator++(int)
    {
        tree_item_iterator _Tmp = *this;
        ++(*this);
        return (_Tmp);
    }
    /// preincrement
    tree_item_iterator& operator++(); 

    /// test for iterator equality
    bool operator==(const tree_item_iterator& _Right) const 
    {
        return (mItem == _Right.mItem); 
    }
    /// test for iterator inequality
    bool operator!=(const tree_item_iterator& _Right) const 
    {
        return (! (*this == _Right)); 
    }
    /// assignement operator
    tree_item_iterator operator=(const tree_item_iterator& _Right); 
    /// difference operator
    friend size_t operator-(const tree_item_iterator _Left, tree_item_iterator _Right)
    {
        tree_item_iterator _Temp(_Left);
        size_t i;
        for ( i=0; _Temp.getItem() != 0 && _Temp != _Right; --i, ++_Temp);
        if (_Temp.getItem() == 0)
        {
            tree_item_iterator _Temp(_Right);
            for ( i=0; _Temp.getItem() != 0 && _Temp != _Left; ++i, ++_Temp);
        }
        return i;
    }

    /// operator iterator + position
    tree_item_iterator operator+(size_t _Pos) const 
    {    
        tree_item_iterator _Temp(tree_item_iterator(*this));
        for (size_t i=0; i<_Pos; ++i, ++_Temp);
        return _Temp; 
    }

    CTreeCtrl& getCtrl()  { return mCtrl; };
    size_t     getLevel() { return mParents.size(); };
    HTREEITEM  getItem()  { return mItem; };
private:
    std::vector<HTREEITEM> mParents;
    HTREEITEM mItem;
    CTreeCtrl&mCtrl;
};
