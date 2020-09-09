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
#if !defined(AFX_PARAMETERSETTINGS_H__994624CD_14D3_47E7_8346_D908B689C8FE__INCLUDED_)
#define AFX_PARAMETERSETTINGS_H__994624CD_14D3_47E7_8346_D908B689C8FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ParameterSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParameterSettings dialog

class CParameterSettings : public CDialog
{
// Construction
public:
	VOID SetParameters(int nShapeIndex);
	VOID GetParameter(int& nShapeIndex);
	CParameterSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParameterSettings)
	enum { IDD = IDD_PARAMETER };
	UINT	m_ShapeIndex;
	double	m_shapeThreshold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParameterSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParameterSettings)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARAMETERSETTINGS_H__994624CD_14D3_47E7_8346_D908B689C8FE__INCLUDED_)
