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
#include "IlluminantObserver.h"

// CColorTest dialog

class CColorTest : public CDialog
{
	DECLARE_DYNAMIC(CColorTest)

public:
	/** 
	 * Initializes the selected measurements and calibration settings to be displayed in this dialog. 
	 * 
	 * _basic, _defaultRanges, and _userRanges can be 0, 1, or 2, following the MFC convention for tri-state checkboxes. 
	 */
	CColorTest(CString calibrationFile, IlluminantObserver illumObs, double _lSlope, double _lIntercept, 
					   double _aSlope, double _aIntercept, double _bSlope, double _bIntercept, 
					   int _basic, int _defaultRanges, int _userRanges, CWnd* pParent = NULL); 
	virtual ~CColorTest();
	
// Dialog Data
	enum { IDD = IDD_COLORTEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	IlluminantObserver getIlluminantObserver(); 
	double getLSlope() { return lSlope; }
	double getLIntercept() { return lIntercept; }
	double getASlope() { return aSlope; }
	double getAIntercept() { return aIntercept; }
	double getBSlope() { return bSlope; }
	double getBIntercept() { return bIntercept; }

	/**
	 * Returns true if the user clicked Restore Defaults or edited the equations, changing them from 
	 * their original values. This always returns true when isCalibrationReset returns true. 
	 * Assumes the user has closed the dialog using any button but Cancel. 
	 */
	bool isCalibrationChanged() { return changed; }

	/** 
	 * Returns true if the user clicked Restore Defaults (and possibly edited the equations), and
	 * the equation values are still at the defaults when the dialog is closed. 
	 * Assumes the user has closed the dialog using any button but Cancel. 
	 */
	bool isCalibrationReset() { return reset; }

	enum SelectedAction { SAVE, CALIBRATE, ANALYZE }; 
	/** 
	 * Returns a value representing the button used to close the window. 
	 * Assumes the user has closed the dialog using any button but Cancel.  
	 */
	SelectedAction getSelectedAction() { return action; }

	int getBasic() { return basic; }
	int getDefaultRanges() { return defaultRanges; }
	int getUserRanges() { return userRanges; }

	afx_msg void OnBnClickedAnalyze();
	afx_msg void OnBnClickedSaveSettings();
	afx_msg void OnBnClickedCalibrate();
	afx_msg void OnBnClickedCancel();
	void OnOK(); 
	BOOL OnInitDialog(); 

private: 
	int basic, defaultRanges, userRanges; 
	CString calibrationFile; 
	int illuminantIndex; 
	double lSlope, lIntercept, aSlope, aIntercept, bSlope, bIntercept; 
	double lSlopeOrig, lInterceptOrig, aSlopeOrig, aInterceptOrig, bSlopeOrig, bInterceptOrig; 
	bool changed, reset; 
	SelectedAction action; 
	CStatic calibrationMessage; 

	void updateCalibrationMessage(); 

public:
	afx_msg void OnBnClickedRestoreDefaults();
};
