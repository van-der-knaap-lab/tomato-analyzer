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
// TomatoAnalyzerView.h : interface of the CTomatoAnalyzerView class
//


#pragma once


class CTomatoAnalyzerView : public CScrollView
{
protected: // create from serialization only
	
	DECLARE_DYNCREATE(CTomatoAnalyzerView)

// Attributes
public:
	CTomatoAnalyzerView();
	CTomatoAnalyzerDoc* GetDocument() const;
    void RenewImageAfterSliceRotation(CDC * pDC, CBitmap * , CSlice * pSlice);
    void RenewImageAfterSliceRotation(CAdvImage * pFullImage, CSlice * pSlice);

// Operations
public:
	VOID SelectSlice(CPoint point);

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	CBitmap* GetBitmap() {
		return m_pBitmap;
	}

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CTomatoAnalyzerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_MSG(CImageView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg void OnCmdZoomView(UINT nID);
// Generated message map functions
protected:
	float		m_fZoom;
	CBitmap	*	m_pBitmap;
	CPoint		m_ptExclude[2];

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TomatoAnalyzerView.cpp
inline CTomatoAnalyzerDoc* CTomatoAnalyzerView::GetDocument() const
   { return reinterpret_cast<CTomatoAnalyzerDoc*>(m_pDocument); }
#endif

