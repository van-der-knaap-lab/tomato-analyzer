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
// ColorTest.cpp : implementation file
//

#include "stdafx.h"
#include <cmath>
#include "TomatoAnalyzer.h"
#include "ColorTest.h"
#include ".\colortest.h"

const float DEFAULT_SLOPE = 1, DEFAULT_INTERCEPT = 0; 

// CColorTest dialog

IMPLEMENT_DYNAMIC(CColorTest, CDialog)
CColorTest::CColorTest(CString _calibrationFile, IlluminantObserver illumObs, double _lSlope, double _lIntercept, 
					   double _aSlope, double _aIntercept, double _bSlope, double _bIntercept, 
					   int _basic, int _defaultRanges, int _userRanges, CWnd* pParent)
	: CDialog(CColorTest::IDD, pParent)
{
	calibrationFile = _calibrationFile; 
	illuminantIndex = illumObs; 
	lSlopeOrig = lSlope = _lSlope; 
	lInterceptOrig = lIntercept = _lIntercept; 
	aSlopeOrig = aSlope = _aSlope; 
	aInterceptOrig = aIntercept = _aIntercept; 
	bSlopeOrig = bSlope = _bSlope; 
	bInterceptOrig = bIntercept = _bIntercept; 
	basic = _basic; 
	defaultRanges = _defaultRanges; 
	userRanges = _userRanges; 
	changed = false; 
	reset = false; 
}

CColorTest::~CColorTest()
{
}

IlluminantObserver CColorTest::getIlluminantObserver() {
	switch (illuminantIndex) {
		case 0: return C_2; 
		case 1: return D65_2; 
		case 2: return D65_10; 
		default: return C_2; 
	}
}

BOOL CColorTest::OnInitDialog() { 
	BOOL ret = CDialog::OnInitDialog(); 
	updateCalibrationMessage(); 

	return ret; 
}

void CColorTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorTest)
	DDX_Check(pDX, IDC_BASIC, basic);						// Tri-state property = true
	DDX_Check(pDX, IDC_DEFAULT_RANGES, defaultRanges);		// 
	DDX_Check(pDX, IDC_USER_RANGES, userRanges);			
	DDX_Text(pDX, IDC_L_SLOPE, lSlope);
	DDX_Text(pDX, IDC_L_INTERCEPT, lIntercept);
	DDX_Text(pDX, IDC_A_SLOPE, aSlope);
	DDX_Text(pDX, IDC_A_INTERCEPT, aIntercept);
	DDX_Text(pDX, IDC_B_SLOPE, bSlope);
	DDX_Text(pDX, IDC_B_INTERCEPT, bIntercept);
	DDX_Radio(pDX, IDC_C_2, illuminantIndex);
	DDX_Control(pDX, IDC_CALIBRATION_MESSAGE, calibrationMessage); 
	//}}AFX_DATA_MAP

	// For the radio buttons to work right, you have to properly set the Group property and tab order. 
}


BEGIN_MESSAGE_MAP(CColorTest, CDialog)
	ON_BN_CLICKED(IDC_ANALYZE_COLOR, OnBnClickedAnalyze)	
	ON_BN_CLICKED(IDC_SAVE_SETTINGS, OnBnClickedSaveSettings)	
	ON_BN_CLICKED(IDC_CALIBRATE, &CColorTest::OnBnClickedCalibrate)
	ON_BN_CLICKED(IDCANCEL, &CColorTest::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RESTORE_DEFAULTS, &CColorTest::OnBnClickedRestoreDefaults)
END_MESSAGE_MAP()

	void CColorTest::OnBnClickedAnalyze()
	{
		action = ANALYZE; 
		OnOK();
	}

	void CColorTest::OnBnClickedCalibrate()
	{
		action = CALIBRATE; 
		OnOK(); 
	}

	void CColorTest::OnBnClickedSaveSettings()
	{
		action = SAVE; 
		OnOK();
	}

	void CColorTest::OnBnClickedCancel()
	{
		OnCancel();
	}

	void CColorTest::OnBnClickedRestoreDefaults()
	{
		illuminantIndex = 0; 
		lSlope = DEFAULT_SLOPE; 
		lIntercept = DEFAULT_INTERCEPT; 
		aSlope = DEFAULT_SLOPE; 
		aIntercept = DEFAULT_INTERCEPT; 
		bSlope = DEFAULT_SLOPE; 
		bIntercept = DEFAULT_INTERCEPT; 
		UpdateData(false); 
		calibrationFile.Empty(); 
		updateCalibrationMessage(); 
		reset = true; 
	}

	void CColorTest::OnOK() {
		CDialog::OnOK(); 

		const double EPSILON = 0.00000001;	// This is a workaround for the fact that, for some unknown reason, 
											// even if the user has not edited anything, the original and new values
											// sometimes come out slightly different. (To reproduce: Calibrate, 
											// Choose File, Save Settings, re-open Color Test dialog. "Last
											// calibration" message is wrong.) 
		changed = ! (fabs(lSlopeOrig - lSlope) < EPSILON && fabs(lInterceptOrig - lIntercept) < EPSILON && 
			fabs(aSlopeOrig - aSlope) < EPSILON && fabs(aInterceptOrig - aIntercept) < EPSILON && 
			fabs(bSlopeOrig - bSlope) < EPSILON && fabs(bInterceptOrig - bIntercept) < EPSILON); 
		if (reset) {
			reset = (lSlope == DEFAULT_SLOPE && lIntercept == DEFAULT_INTERCEPT && 
				aSlope == DEFAULT_SLOPE && aIntercept == DEFAULT_INTERCEPT && 
				bSlope == DEFAULT_SLOPE && bIntercept == DEFAULT_INTERCEPT); 
		}
	}

	void CColorTest::updateCalibrationMessage() {
		if (calibrationFile.IsEmpty()) {
			calibrationMessage.SetWindowText("Warning: Calibration has not been performed."); 
		} else {
			std::string fileName = calibrationFile; 
			int lastFileSep = fileName.rfind('\\'); 
			fileName = fileName.substr(lastFileSep + 1); 
			CString msg; 
			msg.Format(_T("Last calibration: %s"), fileName.c_str()); 
			calibrationMessage.SetWindowText(msg); 
		}
	}

	/** 
	 * Changes text color to red.....? http://bobmoore.mvps.org/Win32/w32tip37.htm
	 * 
	 * I was trying to change the calibration file message to red, but what's here now
	 * changes all text in the dialog to red. ON_WM_CTLCOLOR() must be in the message map for this. 
	 *
	HBRUSH CColorTest::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
	{
	   HBRUSH hbr;

	   switch (nCtlColor)
	   {
		  case CTLCOLOR_STATIC:
			   pDC->SetTextColor (RGB(255,0,0)); 
			   hbr = (HBRUSH)GetStockObject(NULL_BRUSH);
			   break;

		  default:
			   hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor); 
			   break;
	   }
	   return hbr;
	}*/

