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
#include "stdafx.h"
#include "ColorRangeDialog.h"

const int NUM_RANGES = 6; 

IMPLEMENT_DYNAMIC(ColorRangeDialog, CDialog)
ColorRangeDialog::ColorRangeDialog(vector<ColorRange> ranges, CWnd* pParent)
	: CDialog(ColorRangeDialog::IDD, pParent)
{
	lowerL.resize(NUM_RANGES); 
	upperL.resize(NUM_RANGES); 
	lowerHue.resize(NUM_RANGES); 
	upperHue.resize(NUM_RANGES); 
	lowerChroma.resize(NUM_RANGES); 
	upperChroma.resize(NUM_RANGES); 
	for (int i = 0; i < NUM_RANGES && i < ranges.size(); i++) {
		ColorRange r = ranges[i]; 
		if (! r.isWildcardL()) {
			lowerL[i].Format(_T("%d"), r.getLowerL()); 
			upperL[i].Format(_T("%d"), r.getUpperL()); 
		}
		if (! r.isWildcardHue()) {
			lowerHue[i].Format(_T("%d"), r.getLowerHue()); 
			upperHue[i].Format(_T("%d"), r.getUpperHue()); 
		}
		if (! r.isWildcardChroma()) { 
			lowerChroma[i].Format(_T("%d"), r.getLowerChroma()); 
			upperChroma[i].Format(_T("%d"), r.getUpperChroma()); 
		}
	}
}

ColorRangeDialog::~ColorRangeDialog()
{
}

void ColorRangeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorTest)
	DDX_Text(pDX, IDC_LOWERL1, lowerL[0]);
	DDX_Text(pDX, IDC_LOWERL2, lowerL[1]);
	DDX_Text(pDX, IDC_LOWERL3, lowerL[2]);
	DDX_Text(pDX, IDC_LOWERL4, lowerL[3]);
	DDX_Text(pDX, IDC_LOWERL5, lowerL[4]);
	DDX_Text(pDX, IDC_LOWERL6, lowerL[5]);
	DDX_Text(pDX, IDC_LOWERHUE1, lowerHue[0]);
	DDX_Text(pDX, IDC_LOWERHUE2, lowerHue[1]);
	DDX_Text(pDX, IDC_LOWERHUE3, lowerHue[2]);
	DDX_Text(pDX, IDC_LOWERHUE4, lowerHue[3]);
	DDX_Text(pDX, IDC_LOWERHUE5, lowerHue[4]);
	DDX_Text(pDX, IDC_LOWERHUE6, lowerHue[5]);
	DDX_Text(pDX, IDC_LOWERCHROMA1, lowerChroma[0]);
	DDX_Text(pDX, IDC_LOWERCHROMA2, lowerChroma[1]);
	DDX_Text(pDX, IDC_LOWERCHROMA3, lowerChroma[2]);
	DDX_Text(pDX, IDC_LOWERCHROMA4, lowerChroma[3]);
	DDX_Text(pDX, IDC_LOWERCHROMA5, lowerChroma[4]);
	DDX_Text(pDX, IDC_LOWERCHROMA6, lowerChroma[5]);
	DDX_Text(pDX, IDC_UPPERL1, upperL[0]);
	DDX_Text(pDX, IDC_UPPERL2, upperL[1]);
	DDX_Text(pDX, IDC_UPPERL3, upperL[2]);
	DDX_Text(pDX, IDC_UPPERL4, upperL[3]);
	DDX_Text(pDX, IDC_UPPERL5, upperL[4]);
	DDX_Text(pDX, IDC_UPPERL6, upperL[5]);
	DDX_Text(pDX, IDC_UPPERHUE1, upperHue[0]);
	DDX_Text(pDX, IDC_UPPERHUE2, upperHue[1]);
	DDX_Text(pDX, IDC_UPPERHUE3, upperHue[2]);
	DDX_Text(pDX, IDC_UPPERHUE4, upperHue[3]);
	DDX_Text(pDX, IDC_UPPERHUE5, upperHue[4]);
	DDX_Text(pDX, IDC_UPPERHUE6, upperHue[5]);
	DDX_Text(pDX, IDC_UPPERCHROMA1, upperChroma[0]);
	DDX_Text(pDX, IDC_UPPERCHROMA2, upperChroma[1]);
	DDX_Text(pDX, IDC_UPPERCHROMA3, upperChroma[2]);
	DDX_Text(pDX, IDC_UPPERCHROMA4, upperChroma[3]);
	DDX_Text(pDX, IDC_UPPERCHROMA5, upperChroma[4]);
	DDX_Text(pDX, IDC_UPPERCHROMA6, upperChroma[5]);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ColorRangeDialog, CDialog)
	ON_BN_CLICKED(IDOK, &ColorRangeDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CLEAR, &ColorRangeDialog::OnBnClickedClear)
END_MESSAGE_MAP()

void ColorRangeDialog::OnBnClickedOk()
{
	OnOK();

	for (int i = 0; i < lowerL.size(); i++) {
		bool noL = false, 
			noHue = false, 
			noChroma = false; 
		if (lowerL[i].IsEmpty() || upperL[i].IsEmpty()) {
			noL = true; 
			lowerL[i] = upperL[i] = "-1"; 
		}
		if (lowerHue[i].IsEmpty() || upperHue[i].IsEmpty()) {
			noHue = true; 
			lowerHue[i] = upperHue[i] = "-1"; 
		}
		if (lowerChroma[i].IsEmpty() || upperChroma[i].IsEmpty()) {
			noChroma = true; 
			lowerChroma[i] = upperChroma[i] = "-1"; 
		}
		if (! (noL && noHue && noChroma)) {
			ColorRange r(atoi(lowerL[i]), atoi(upperL[i]), atoi(lowerHue[i]), atoi(upperHue[i]), 
				atoi(lowerChroma[i]), atoi(upperChroma[i]), noL, noHue, noChroma); 
			ranges.push_back(r); 
		}
	}
}

void ColorRangeDialog::OnBnClickedClear()
{
	lowerL.clear(); 
	upperL.clear(); 
	lowerHue.clear(); 
	upperHue.clear(); 
	lowerChroma.clear(); 
	upperChroma.clear(); 
	lowerL.resize(NUM_RANGES); 
	upperL.resize(NUM_RANGES); 
	lowerHue.resize(NUM_RANGES); 
	upperHue.resize(NUM_RANGES); 
	lowerChroma.resize(NUM_RANGES); 
	upperChroma.resize(NUM_RANGES); 
	UpdateData(false); 
}
