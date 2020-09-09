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
#if !defined(AFX_SLICEINFOFORMVIEW_H__FD02D063_E1C3_4E77_A163_DC6F37765A2E__INCLUDED_)
#define AFX_SLICEINFOFORMVIEW_H__FD02D063_E1C3_4E77_A163_DC6F37765A2E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CSliceInfoFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSliceInfoFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CSliceInfoFormView : public CFormView
{
protected:
	CSliceInfoFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSliceInfoFormView)

// Form Data
public:
	//{{AFX_DATA(CSliceInfoFormView)
	enum { IDD = IDD_SLICEINFO };
	CListCtrl	m_data;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSliceInfoFormView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSliceInfoFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CSliceInfoFormView)
	afx_msg void OnItemchangedSliceInfoList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLICEINFOFORMVIEW_H__FD02D063_E1C3_4E77_A163_DC6F37765A2E__INCLUDED_)
