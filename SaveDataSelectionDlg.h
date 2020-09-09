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
#if !defined(AFX_SAVEDATASELECTIONDLG_H__6076CB3D_9704_4649_883A_3C3F7A0B3824__INCLUDED_)
#define AFX_SAVEDATASELECTIONDLG_H__6076CB3D_9704_4649_883A_3C3F7A0B3824__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveDataSelectionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSaveDataSelectionDlg dialog

class CSaveDataSelectionDlg : public CDialog
{
// Construction
public:
	CSaveDataSelectionDlg(CWnd* pParent = NULL);   // standard constructor
    std::vector<int>  m_vctSelectedItem;

// Dialog Data
	//{{AFX_DATA(CSaveDataSelectionDlg)
	enum { IDD = IDD_SAVEDATA_DLG };
	CCheckboxTreeCtrl	m_ctrTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveDataSelectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    void  UpdateSelectionStatus(HTREEITEM hItem);
    void  UpdateUpdateParents(HTREEITEM hItem, UINT bChecked);
    void  UpdateAllChildren(HTREEITEM hItem, UINT bChecked);
    BOOL  allSelected(HTREEITEM hItem, UINT bChecked);


	// Generated message map functions
	//{{AFX_MSG(CSaveDataSelectionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedSelectionTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickSelectionTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVEDATASELECTIONDLG_H__6076CB3D_9704_4649_883A_3C3F7A0B3824__INCLUDED_)
