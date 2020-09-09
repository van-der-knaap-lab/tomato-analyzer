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
#include "afxwin.h"
#include "resource.h"
#include <vector>
#include "ColorRange.h"
using namespace std; 

class ColorRangeDialog : public CDialog
{
	DECLARE_DYNAMIC(ColorRangeDialog)

public:
	ColorRangeDialog(vector<ColorRange> ranges, CWnd* pParent = NULL); 
	virtual ~ColorRangeDialog();
	vector<ColorRange> getRanges() { return ranges; }
	
// Dialog Data
	enum { IDD = IDD_COLORRANGEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

private: 
	vector<CString> lowerL, upperL, lowerHue, upperHue, lowerChroma, upperChroma; 
	vector<ColorRange> ranges; 
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedClear();
};
