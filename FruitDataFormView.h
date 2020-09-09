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
#if !defined(AFX_FRUITDATAFORMVIEW_H__6D14F596_EFB3_4780_8E64_0F9FCF77E359__INCLUDED_)
#define AFX_FRUITDATAFORMVIEW_H__6D14F596_EFB3_4780_8E64_0F9FCF77E359__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FruitDataFormView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFruitDataFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CFruitDataFormView : public CFormView
{
protected:
	CFruitDataFormView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFruitDataFormView)

// Form Data
public:
	//{{AFX_DATA(CFruitDataFormView)
	enum { IDD = IDD_SLICEINFO_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFruitDataFormView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CFruitDataFormView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CFruitDataFormView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRUITDATAFORMVIEW_H__6D14F596_EFB3_4780_8E64_0F9FCF77E359__INCLUDED_)
