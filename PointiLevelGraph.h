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
#if !defined(AFX_POINTILEVELGRAPH_H__BCEF6151_E61E_4B9E_9846_F2A38D271B26__INCLUDED_)
#define AFX_POINTILEVELGRAPH_H__BCEF6151_E61E_4B9E_9846_F2A38D271B26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PointiLevelGraph.h : header file
//
#include "AdvImage.h"
#include "ImgTool.h"

/////////////////////////////////////////////////////////////////////////////
// CPointiLevelGraph window

class CPointiLevelGraph : public CStatic
{
// Construction
public:
	CPointiLevelGraph();

// Attributes
public:
    int m_nMin, m_nMax;
    std::vector<IPTPOINT>* m_pEllipse;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointiLevelGraph)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPointiLevelGraph();
    void    SetRange(int nMin, int nMax);
    void    DrawFigure(CDC * pDC);
    
	// Generated message map functions
protected:
	//{{AFX_MSG(CPointiLevelGraph)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTILEVELGRAPH_H__BCEF6151_E61E_4B9E_9846_F2A38D271B26__INCLUDED_)
