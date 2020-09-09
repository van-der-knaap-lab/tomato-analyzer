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
#if !defined(AFX_SINGLESLICEVIEW_H__7B98085C_0535_4BAF_9B9F_FBB24E344391__INCLUDED_)
#define AFX_SINGLESLICEVIEW_H__7B98085C_0535_4BAF_9B9F_FBB24E344391__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SingleSliceView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSingleSliceView view

class CSingleSliceView : public CView
{
protected:
	CSingleSliceView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSingleSliceView)

// Attributes
public:
	BOOL RotateSliceOperation(BOOL bRotate);
//    double getScale(){return m_fScale;};

// Operations
public:
    double  GetScale(){return m_fScale;}
	POINT GetOffsetPoint();
	void DrawImage(CDC* pDC);
	//unsigned char * mBMPixels;
	CBitmap* m_pBitmap;
    CBitmap* getScaledBitmap(){return m_pBitmap;};

private:
    int         m_imgw, m_imgh;
	BOOL		m_bFirstClickForRotate;
	POINT		m_ptRotate[2];
    double      m_fScale;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSingleSliceView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSingleSliceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CSingleSliceView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLESLICEVIEW_H__7B98085C_0535_4BAF_9B9F_FBB24E344391__INCLUDED_)
