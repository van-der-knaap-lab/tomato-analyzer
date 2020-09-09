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
#if !defined(AFX_FRUITDATAVIEW_H__4E05BA47_81C1_4FFE_9A72_9F4969415E0A__INCLUDED_)
#define AFX_FRUITDATAVIEW_H__4E05BA47_81C1_4FFE_9A72_9F4969415E0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FruitDataView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFruitDataView view
#define IDC_GROUP_PAGE     18022004
#define IDC_MEMBER_LIST    19022004
#include <fstream>

class CFruitDataView : public CView
{
protected:
          // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFruitDataView)
    CFruitDataView(); 
// Attributes
public:
	
	CMeasureItemList m_ListData;
    //CListCtrl   m_ListData;
    std::vector<int> m_vctSelectedItem;
	int Slices;
   
private:
    std::vector<int> m_vctGroupItem;
	int m_nColumNumber;
	CTabCtrl m_page;
    CImageList  m_ImageList;
 
// Operations
public:
	CTomatoAnalyzerDoc* GetDocument() const;
    int  GetCurMeasurementGroup(void);
	void OnMeasurementChange(void);
    void ShowMeasurementValues(void);
 
private:
    void OnCurFruitChanged(void);
    void BuildReportItems(void);
    void BuildReportColumns(void);
    void Test(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFruitDataView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	    virtual ~CFruitDataView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CFruitDataView)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGroupTabChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCurMeasurementChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TomatoAnalyzerView.cpp
inline CTomatoAnalyzerDoc* CFruitDataView::GetDocument() const
   { return reinterpret_cast<CTomatoAnalyzerDoc*>(m_pDocument); }
#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRUITDATAVIEW_H__4E05BA47_81C1_4FFE_9A72_9F4969415E0A__INCLUDED_)
