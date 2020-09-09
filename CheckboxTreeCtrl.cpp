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
// CheckboxTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "CheckboxTreeCtrl.h"
#include "algorithm"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckboxTreeCtrl
#define STATE_CLEARED   1
#define STATE_CHECKED   2
#define STATE_GRAYED    3

#define STATEIMAGEMASKFROMINDEX(i) ((i) >> 12)

CCheckboxTreeCtrl::CCheckboxTreeCtrl()
{
    m_bInDragging = FALSE;
    m_hItemOfDrag = NULL;
    m_hItemOfDrop = NULL;

    m_pImgListDragging = NULL;
}

CCheckboxTreeCtrl::~CCheckboxTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CCheckboxTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CCheckboxTreeCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_WM_LBUTTONUP()
    ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, OnBeginDrag)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckboxTreeCtrl message handlers

int CCheckboxTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
    Initialize();  
	return 0;
}


void CCheckboxTreeCtrl::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
//    CImageList  imgLst;
    Initialize();  
	
	CTreeCtrl::PreSubclassWindow();
    return;
}



void CCheckboxTreeCtrl::Initialize()
{
    ASSERT(m_hWnd != NULL);

    LONG  oldStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE ); 
    SetWindowLong(m_hWnd, GWL_EXSTYLE, TVS_CHECKBOXES | oldStyle);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	
    BOOL bRet = m_ImageList.Create(IDB_CHECKBOX_STATE, 16, 1, RGB(255, 0, 255));
    if(bRet)
        SetImageList(&m_ImageList, TVSIL_STATE);
}


void CCheckboxTreeCtrl::OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	TV_DISPINFO * ptvinfo = (TV_DISPINFO *)pnmhdr;
	if(ptvinfo->item.pszText != NULL)
	{
		ptvinfo->item.mask = TVIF_TEXT;
		SetItem(&ptvinfo->item);
	}

	*pLResult = TRUE;
}


void CCheckboxTreeCtrl::OnBeginDrag(LPNMHDR pnmhdr, LRESULT * pLResult)
{
	UINT        nFlags;
	POINT      ptAction;

    ptAction = ((NMTREEVIEW *)pnmhdr)->ptDrag;
    
	ASSERT(!m_bInDragging);
	m_bInDragging = TRUE;

	m_hItemOfDrag = HitTest(ptAction, &nFlags);
	m_hItemOfDrop = NULL;

	ASSERT(m_pImgListDragging == NULL);
	m_pImgListDragging = CreateDragImage(m_hItemOfDrag);
	m_pImgListDragging->DragShowNolock(TRUE);
	m_pImgListDragging->SetDragCursorImage(0, CPoint(0, 0));
	m_pImgListDragging->BeginDrag(0, CPoint(0,0));
	m_pImgListDragging->DragMove(ptAction);
	m_pImgListDragging->DragEnter(this, ptAction);
	SetCapture();

    return;
}


void CCheckboxTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	UINT        flags;
	HTREEITEM   hitem;

	if(m_bInDragging)
	{
		ASSERT(m_pImgListDragging != NULL);

		m_pImgListDragging->DragMove(point);
		if((hitem = HitTest(point, &flags)) != NULL)
		{
			m_pImgListDragging->DragLeave(this);
			SelectDropTarget(hitem);
			m_hItemOfDrop = hitem;
			m_pImgListDragging->DragEnter(this, point);
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}


void CCheckboxTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	OnButtonUp();
	CTreeCtrl::OnLButtonUp(nFlags, point);
}



void CCheckboxTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	OnButtonUp();
	CTreeCtrl::OnRButtonUp(nFlags, point);
}


void CCheckboxTreeCtrl::OnButtonUp()
{
	if(m_bInDragging)
	{
		ASSERT(m_pImgListDragging != NULL);

		m_pImgListDragging->DragLeave(this);
		m_pImgListDragging->EndDrag();
		delete m_pImgListDragging;
		m_pImgListDragging = NULL;

		if(m_hItemOfDrag != m_hItemOfDrop && 
            !IsChildNodeOf(m_hItemOfDrop, m_hItemOfDrag) && 
            GetParentItem(m_hItemOfDrag) != m_hItemOfDrop
          )
		{
			TransferItem(m_hItemOfDrag, m_hItemOfDrop);
			DeleteItem(m_hItemOfDrag);
		}

		ReleaseCapture();
		m_bInDragging = FALSE;
		SelectDropTarget(NULL);
	}
    
    return;
}


BOOL CCheckboxTreeCtrl::IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent)
{
    if(hitemChild == hitemSuspectedParent)
        return FALSE;

	do
	{
		if(hitemChild == hitemSuspectedParent)
			break;
	}
	while((hitemChild = GetParentItem(hitemChild)) != NULL);

	return(hitemChild != NULL);
}


BOOL CCheckboxTreeCtrl::TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop)
{
	TVINSERTSTRUCT      tvstruct;
	TCHAR               sztBuffer[256];
	HTREEITEM           hNewItem, hFirstChild;

    memset(sztBuffer, 0, sizeof(sztBuffer));

	tvstruct.item.hItem = hitemDrag;
	tvstruct.item.cchTextMax = sizeof(sztBuffer);
	tvstruct.item.pszText = sztBuffer;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	GetItem(&tvstruct.item);
	
    tvstruct.hParent = hitemDrop;
	tvstruct.hInsertAfter = TVI_SORT;
	tvstruct.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	hNewItem = InsertItem(&tvstruct);

	while((hFirstChild = GetChildItem(hitemDrag)) != NULL)
	{
		TransferItem(hFirstChild, hNewItem);
		DeleteItem(hFirstChild);
	}
	return TRUE;
}

void CCheckboxTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
    UINT    Flags;

    HTREEITEM hItem = HitTest(point, &Flags);
    if(NULL != hItem &&
        ((TVHT_ONITEMSTATEICON & Flags) == TVHT_ONITEMSTATEICON )
       )
    {
        UINT  uStateMasked = GetItemState(hItem, TVIS_STATEIMAGEMASK);
        UINT  uChecked = STATEIMAGEMASKFROMINDEX(uStateMasked);
        UINT  uNewState = (STATE_CHECKED==uChecked)? STATE_CLEARED : STATE_CHECKED;
        SetItemState(hItem, INDEXTOSTATEIMAGEMASK(uNewState), TVIS_STATEIMAGEMASK);

        if(ItemHasChildren(hItem))
            UpdateChildrenState(hItem, uNewState);

        UpdateParentState(hItem);
        return;
    }

	CTreeCtrl::OnLButtonDown(nFlags, point);
}


void CCheckboxTreeCtrl::UpdateChildrenState(HTREEITEM  hItem, UINT uNewState)
{
    HTREEITEM  hChildItem = GetNextItem(hItem, TVGN_CHILD);
    while(NULL != hChildItem)
    {
        SetItemState(hChildItem, INDEXTOSTATEIMAGEMASK(uNewState), TVIS_STATEIMAGEMASK);

        if(ItemHasChildren(hChildItem))
            UpdateChildrenState(hChildItem, uNewState);

        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }
    
    return;
}


void CCheckboxTreeCtrl::UpdateParentState(HTREEITEM  hItem)
{
    HTREEITEM   hParentItem = GetParentItem(hItem);
    while(NULL != hParentItem)
    {
        UINT  uNewState = MatchChildrenState(hParentItem);
        UINT  uStateMasked = GetItemState(hParentItem, TVIS_STATEIMAGEMASK);
        UINT  uCurParentState = STATEIMAGEMASKFROMINDEX(uStateMasked);

        if(uNewState == uCurParentState)
            break;

        SetItemState(hParentItem, INDEXTOSTATEIMAGEMASK(uNewState), TVIS_STATEIMAGEMASK);
        hParentItem = GetParentItem(hParentItem);
    }

    return ;
}


UINT CCheckboxTreeCtrl::MatchChildrenState(HTREEITEM  hItem)
{
    HTREEITEM  hChildItem = GetNextItem(hItem, TVGN_CHILD);
    UINT  uFirstState = GetItemState(hChildItem, TVIS_STATEIMAGEMASK);
    uFirstState = STATEIMAGEMASKFROMINDEX(uFirstState);

    hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    while(NULL != hChildItem)
    {
        UINT uOtherState = GetItemState(hChildItem, TVIS_STATEIMAGEMASK);
        uOtherState = STATEIMAGEMASKFROMINDEX(uOtherState);
        if(uOtherState != uFirstState)
        {
            uFirstState = STATE_GRAYED;
            break;
        }

        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }

    return uFirstState;
}


void CCheckboxTreeCtrl::RetrieveCheckedItemID(std::vector<int>& vctItemChecked, HTREEITEM hItem)
{
    HTREEITEM  hChildItem = GetNextItem(hItem, TVGN_CHILD);
    while(NULL != hChildItem)
    {
        if(ItemHasChildren(hChildItem))
            RetrieveCheckedItemID(vctItemChecked, hChildItem);
        else
        {
            UINT  uStateMasked = GetItemState(hChildItem, TVIS_STATEIMAGEMASK);
            UINT  uChecked = STATEIMAGEMASKFROMINDEX(uStateMasked);
            if(STATE_CHECKED == uChecked)
            {
                int nID = GetItemData(hChildItem);
                vctItemChecked.push_back(nID);
            }
        }

        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }

    return;
}


UINT CCheckboxTreeCtrl::MatchChildrenStateTopdown(HTREEITEM  hItem)
{
    HTREEITEM  hChildItem = GetNextItem(hItem, TVGN_CHILD);
    UINT  uFirstState = GetItemState(hChildItem, TVIS_STATEIMAGEMASK);

    hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    while(NULL != hChildItem)
    {
        if(ItemHasChildren(hChildItem))
            SynchronizeStatesTopDown(GetNextItem(hChildItem, TVGN_CHILD));

        UINT uOtherState = GetItemState(hChildItem, TVIS_STATEIMAGEMASK);
        if(uOtherState != uFirstState)
        {
            uFirstState = INDEXTOSTATEIMAGEMASK(STATE_GRAYED);
            break;
        }

        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }

    return STATEIMAGEMASKFROMINDEX(uFirstState);
}


void CCheckboxTreeCtrl::SynchronizeStatesTopDown(HTREEITEM   hItem)
{
    if(NULL==hItem)
        return;

    if(! ItemHasChildren(hItem))
        return;

    UINT    uStatus = MatchChildrenStateTopdown(hItem);

    if(hItem != TVGN_ROOT)
        SetItemState(hItem, INDEXTOSTATEIMAGEMASK(uStatus), TVIS_STATEIMAGEMASK);
}


void CCheckboxTreeCtrl::SetCheckboxStatus(std::vector<int>& vctItemChecked)
{
    HTREEITEM   hItem = GetNextItem(TVGN_ROOT, TVGN_CHILD);
    while(NULL != hItem)
    {
        if(ItemHasChildren(hItem))
            SetCheckboxStatus(hItem, vctItemChecked);

        hItem = GetNextItem(hItem, TVGN_NEXT);
    }
}


void CCheckboxTreeCtrl::SetCheckboxStatus(HTREEITEM hItem, std::vector<int>& vctItemChecked)
{
    HTREEITEM   hChildItem = GetNextItem(hItem, TVGN_CHILD);
    while(NULL != hChildItem)
    {
        if(ItemHasChildren(hChildItem))
            SetCheckboxStatus(hChildItem, vctItemChecked);
        else
        {
            int nID = GetItemData(hChildItem);
            if(vctItemChecked.end() != std::find(vctItemChecked.begin(), vctItemChecked.end(), nID))
                SetItemState(hChildItem, INDEXTOSTATEIMAGEMASK(STATE_CHECKED), TVIS_STATEIMAGEMASK);
        }

        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }

    UINT  uNewState = MatchChildrenState(hItem);
    SetItemState(hItem, INDEXTOSTATEIMAGEMASK(uNewState), TVIS_STATEIMAGEMASK);
    return;
}

void CCheckboxTreeCtrl::SetCheckboxForAllItems()
{
    HTREEITEM   hChildItem = GetNextItem(TVGN_ROOT, TVGN_CHILD);
    while(NULL != hChildItem)
    {
        SetItemState(hChildItem, INDEXTOSTATEIMAGEMASK(STATE_CHECKED), TVIS_STATEIMAGEMASK);
        if(ItemHasChildren(hChildItem))
            UpdateChildrenState(hChildItem, STATE_CHECKED);

        hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
    }

    return;
}
