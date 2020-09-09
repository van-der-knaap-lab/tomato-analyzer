/* This file is part of Tomato Analyzer.

    Tomato Analyzer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tomato Analyzer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tomato Analyzer.  If not, see <http://www.gnu.org/licenses/>. */
// LinkedList.h: interface for the CLinkedList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINKEDLIST_H__ED22F7BD_488F_4AE4_92B1_2680E44CAF23__INCLUDED_)
#define AFX_LINKEDLIST_H__ED22F7BD_488F_4AE4_92B1_2680E44CAF23__INCLUDED_

#include "PixelList.h"
#include "AdvImage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * Helper class for CBinObjs - seems to be a linked list node. 
 */
class CBONode
{
public:
	CBONode();
	CBONode(CPixelList * inList, int inLabel);
	CBONode(CBONode * inNext);
	CBONode(CBONode * inNext, CPixelList * inList, int inLabel);
	virtual ~CBONode();


	CBONode * mNext;
	CPixelList * mPixList;
	int mLabel;


};

/**
 * This class seems to be a linked list of "binary objects", which are
 * the foreground objects (slices and other objects) identified when analyzing an image. 
 */
class CBinObjs
{
public:
	CBinObjs();
	virtual ~CBinObjs();

	void Add(CPixelList * inList, int inLabel);
	void Insert(CBONode * inAfter, CPixelList * inList, int inLabel);
	void Remove(CBONode * inNode);

	void RemoveSmallestObj();
	
	CPixelList * GetList(int inLabel);
	CBONode * Find(int inLabel);
	int GetMaxListSize();

	CBONode * GetHead() { return mHead; }
	CBONode * GetTail() { return mTail; }
	int GetSize() { return mSize; }

	CBinObjs * CompressPixLists();

	unsigned int FilterBinaryObjects(unsigned int minSize, unsigned int maxSize, CAdvImage * inImage);

protected:
	CBONode * mHead;
	CBONode * mTail;
	unsigned int mSize;

};

#endif // !defined(AFX_LINKEDLIST_H__ED22F7BD_488F_4AE4_92B1_2680E44CAF23__INCLUDED_)
