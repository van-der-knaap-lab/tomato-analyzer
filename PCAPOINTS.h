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
#pragma once
#include "afxcmn.h"


// PCAPOINTS dialog

class PCAPOINTS : public CDialog
{
	DECLARE_DYNAMIC(PCAPOINTS)

public:
	PCAPOINTS(CWnd* pParent = NULL);   // standard constructor
	virtual ~PCAPOINTS();

// Dialog Data
	enum { IDD = IDD_PCA_POINTS };
	//{{AFX_VIRTUAL(PCAPOINTS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(PCAPOINTS)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl PCA_Slider;
	int PCA_Value;
	int sliderValue;	// The value of the slider -- 1/2 the PCA_Value
	bool clickSave;
};
