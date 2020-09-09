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
#if !defined(AFX_SAVEDATA_H__D6E7E97D_6C3B_47DF_BFF0_15627CB3D0F8__INCLUDED_)
#define AFX_SAVEDATA_H__D6E7E97D_6C3B_47DF_BFF0_15627CB3D0F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSaveData dialog

/**
 * Not used? 
 */
class CSaveData : public CDialog
{
// Construction
public:
	CSaveData(CWnd* pParent = NULL);   // standard constructor

public:
    std::vector<int>  m_vctSelectedItem;
// Dialog Data
	//{{AFX_DATA(CSaveData)
	enum { IDD = IDD_SAVEDATADLG };
	CListBox	m_Candidates;
	CListBox	m_Selected;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveData)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    void AddRemoveItem(CListBox& lstadd, CListBox& lstremove);
    void SequenceAdjust(int iDir);

	// Generated message map functions
	//{{AFX_MSG(CSaveData)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnDblclkCandidates();
	afx_msg void OnDblclkSelected();
	afx_msg void OnSavedataUp();
	afx_msg void OnSavedataDown();
	virtual void OnOK();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVEDATA_H__D6E7E97D_6C3B_47DF_BFF0_15627CB3D0F8__INCLUDED_)
