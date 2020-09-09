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
// LinkedList.cpp: implementation of the CLinkedList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "BinObjs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// CBONode Members
//////////////////////////////////////////////////////////////////////
CBONode::CBONode()
{
	mNext = NULL;
	mPixList = NULL;
	mLabel = 0;
}

CBONode::CBONode(CPixelList * inList, int inLabel)
{
	mNext = NULL;
	mPixList = inList;
	mLabel = inLabel;
}

CBONode::CBONode(CBONode * inNext)
{
	mNext = inNext;
	mPixList = NULL;
	mLabel = 0;
}

CBONode::CBONode(CBONode * inNext, CPixelList * inList, int inLabel)
{
	mNext = inNext;
	mPixList = inList;
	mLabel = inLabel;
}

CBONode::~CBONode()
{
	if(mPixList != NULL)
		delete mPixList;

}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBinObjs::CBinObjs()
{
	mHead = NULL;
	mTail = NULL;
	mSize = 0;
}

CBinObjs::~CBinObjs()
{
	CBONode * head = GetHead();
	CBONode * toDelete;

	if(head == NULL)
		return;

	while(head->mNext != NULL)
	{
		toDelete = head;
		head = head->mNext;
		delete toDelete;
	}
	delete head;
}

CBinObjs * CBinObjs::CompressPixLists()
{
	CBinObjs * newObjList = new CBinObjs();
	CPixelList * newList;
	newList = new CPixelList();
	CBONode * head;

	head = this->GetHead();

	while(head)
	{
		newList->splice(newList->end(), *head->mPixList);
		head = head->mNext;
	}

	newObjList->Add(newList, 0);

	return newObjList;
}

/**
 * Removes all binary objects consisting of fewer than minSize or more than maxSize pixels. 
 */ 
unsigned int CBinObjs::FilterBinaryObjects(unsigned int minSize, unsigned int maxSize, CAdvImage * iImage)
{

	if(this->GetSize() == 0)
		return 0;

    CBONode * toDelete = NULL;
	CBONode * head = GetHead();
	CPixelList * myList;
	unsigned int numRemoved = 0;

	while(head != NULL)
	{
		myList = head->mPixList;

		if(! (minSize <= myList->size() && myList->size() <= maxSize) )
		{
			toDelete = head;
			head = head->mNext;
			Remove(toDelete);
			numRemoved++;
		}
		else
			head = head->mNext;
	
	}
	return numRemoved;
}


void CBinObjs::Add(CPixelList * inList, int inLabel)
{
	if(mHead == NULL)
	{
		mHead = new CBONode(inList, inLabel);
		mTail = mHead;
		mSize = 1;
	}
	else
	{
		mTail->mNext = new CBONode(inList, inLabel);
		mTail = mTail->mNext;
		mSize++;
	}
}

void CBinObjs::Insert(CBONode * inAfter, CPixelList * inList, int inLabel)
{
	CBONode * temp;
	temp = inAfter->mNext;
	inAfter->mNext = new CBONode(inList, inLabel);
	inAfter->mNext->mNext = temp;

	mSize++;
	if(inAfter == mTail)
		mTail = mTail->mNext;
}

void CBinObjs::Remove(CBONode * inNode)
{
	if(mSize == 1)
	{
		delete mHead;
		mHead = mTail = NULL;
		mSize = 0;
	}
	else if(inNode == mHead) //Removing Head, size >= 2.
	{
		mHead = mHead->mNext;
		delete inNode;
		mSize--;
	}
	else
	{
		CBONode * prev;

		prev = mHead;
		while(prev->mNext != inNode)
		{
			prev = prev->mNext;
		}
		prev->mNext = inNode->mNext;
		if(inNode == mTail)
		{
			mTail = prev;
		}
		delete inNode;
		mSize--;
	}
	inNode = NULL;
}

void CBinObjs::RemoveSmallestObj()
{
	CBONode * head;
	CBONode * smallest;

	head = GetHead();
	smallest = head;
	head = head->mNext;

	while(head)
	{
		if(head->mPixList->size() <= smallest->mPixList->size())
		{
			smallest = head;
		}	

		head = head->mNext;
	}

	Remove(smallest);
}

CBONode * CBinObjs::Find(int inLabel)
{
	CBONode * head = GetHead();

	while(head != NULL)
	{
		if(head->mLabel == inLabel)
			return head;
		head = head->mNext;
	}
	return NULL;
}


CPixelList * CBinObjs::GetList(int inLabel)
{
	CBONode * theNode = Find(inLabel);

	if(theNode == NULL)
	{
		return NULL;
	}
	return theNode->mPixList;
}

int CBinObjs::GetMaxListSize()
{
	CBONode * head;
	unsigned int maxSize = 0;

	head = this->GetHead();
	while(head != NULL)
	{
		if(head->mPixList->size() > maxSize)
			maxSize = head->mPixList->size();
		head = head->mNext;
	}

	return maxSize;
}


