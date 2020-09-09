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
#if !defined(AFX_CHECKBOXTREECTRL_H__79FBFCDC_2097_485A_9084_CD875E8E9270__INCLUDED_)
#define AFX_CHECKBOXTREECTRL_H__79FBFCDC_2097_485A_9084_CD875E8E9270__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckboxTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCheckboxTreeCtrl window

class CCheckboxTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CCheckboxTreeCtrl();

// Attributes
public:
    std::vector<int>    m_vctItemChecked;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckboxTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetCheckboxForAllItems(void);
	void RetrieveCheckedItemID(std::vector<int>& vctItemChecked, HTREEITEM hItem = TVGN_ROOT);
    void SetCheckboxStatus(std::vector<int>& vctItemChecked);
	virtual ~CCheckboxTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckboxTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *pLResult);
	afx_msg void OnBeginDrag(LPNMHDR pnmhdr, LRESULT *pLResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	void Initialize(void);
    void OnButtonUp(void);
    BOOL TransferItem(HTREEITEM hitemDrag, HTREEITEM hitemDrop);
    BOOL IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);
    void UpdateChildrenState(HTREEITEM hItem, UINT uNewState);
    void UpdateParentState(HTREEITEM hItem);
    UINT MatchChildrenState(HTREEITEM hItem);

    void SetCheckboxStatus(HTREEITEM hItem, std::vector<int>& vctItemChecked);
    void SynchronizeStatesTopDown(HTREEITEM   hItem);
    UINT MatchChildrenStateTopdown(HTREEITEM  hItem);


private:
    BOOL        m_bInDragging;
	HTREEITEM   m_hItemOfDrag;
	HTREEITEM   m_hItemOfDrop;
	CImageList  m_ImageList;
    CImageList* m_pImgListDragging;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKBOXTREECTRL_H__79FBFCDC_2097_485A_9084_CD875E8E9270__INCLUDED_)
