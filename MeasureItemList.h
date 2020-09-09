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
#if !defined(AFX_MEASUREITEMLIST_H__84FD78F5_A23C_49D5_8E5E_235226D3D0B1__INCLUDED_)
#define AFX_MEASUREITEMLIST_H__84FD78F5_A23C_49D5_8E5E_235226D3D0B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MeasureItemList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeasureItemList window

class CMeasureItemList : public CListCtrl
{
// Construction
public:
	CMeasureItemList();

// Attributes
public:

private:
    int m_nCurColumn;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasureItemList)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DeleteAllColumns(void);
	virtual ~CMeasureItemList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMeasureItemList)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASUREITEMLIST_H__84FD78F5_A23C_49D5_8E5E_235226D3D0B1__INCLUDED_)
