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
// TomatoAnalyzer.h : main header file for the TomatoAnalyzer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include <vector>
#include "MeasurementSet.h"
#include "ColorRange.h"
#include "IlluminantObserver.h"

class CTomatoAnalyzerDoc; 

// Defines the maximum number of measurements that the user can add
#define maxUserCreated 30

/**
 * Basically the main class of the application. Handles events related to, and stores, the application settings
 * (Settings menu and Color Test dialog). 
 */
class CTomatoAnalyzerApp : public CWinApp
{
public:
	CTomatoAnalyzerApp();
	CTomatoAnalyzerDoc * GetCurrentDoc(); 

public:
	/// DPI setting
    int m_dpi;
	/// DPI units setting -- 0 indicates pixel, 1 millimeter, 2 centimeter, and 3 inches
    int m_iUnit;

	/// Macro level setting
    int m_nMac; 
	/// Micro level setting
	int m_nMic; 
	/// Brightness level setting -- NOT USED (It really is used, the before comment was wrong. :)) -- which is it? we need to investigate this. IT REALLY NEEDS DONE, IT HASNT BEEN YET.
	int	m_nBrightLevel;
	/// Number of PCA points setting
	int m_numPCAPoints; 
	/// Ellipse size setting
	int m_ellipseSize;
	/// Color analysis observer angle setting
	IlluminantObserver m_Observer;
	/// Blockiness upper setting
	float	m_fBlkUp;
	/// Blockiness lower setting
	float   m_fBlkDn;
	/// Color calibration equation setting - L* slope
	double m_slope_l; 
	/// Color calibration equation setting - a* slope
	double m_slope_a; 
	/// Color calibration equation setting - b* slope
	double m_slope_b; 
	/// Color calibration equation setting - L* intercept
	double m_yint_l; 
	/// Color calibration equation setting - a* intercept
	double m_yint_a; 
	/// Color calibration equation setting - b* intercept
	double m_yint_b; 
	std::vector<ColorRange> getLRanges() { return m_lRanges; }
	std::vector<ColorRange> getHueRanges() { return m_hueRanges; }
	std::vector<ColorRange> getChromaRanges() { return m_chromaRanges; }
	std::vector<ColorRange> getUserRanges() { return m_userRanges; }
	/// Contains the set of IDs of measurements that are to appear in the report
    std::vector<int> m_vctSaveDataItem;
	/// Used in the Add/Edit Dialog window
	int ret; 

private: 
	/// For color analysis, this is either the name of the last calibration file, something about manual data entry
	/// if that was the most recent method of changing the calibration equations, or empty if never calibrated. 
	CString m_colorCalibrationFile; 

	/// The L* ranges over which to find the L* distribution
	std::vector<ColorRange> m_lRanges; 
	/// The hue ranges over which to find the hue distribution
	std::vector<ColorRange> m_hueRanges; 
	/// The chroma ranges over which to find the chroma distribution
	std::vector<ColorRange> m_chromaRanges; 
	/// The custom color parameters
	std::vector<ColorRange> m_userRanges; 

	/// Keeps track of the number of user-generated entries to match up against the total specified
	int userCounter;

	/// Contains the list of all column headings
	MeasurementSet m_measurementSet; 
	
	void initMeasurementSet(); 
	
	//bool		AsymmetryCheck;

// Overrides
public:
	/// Returns a list of IDs of all measurements currently selected for display
    std::vector<int> getMeasureItemsNeededToSave(void);
	/// Copies into the given vector a list of IDs of all measurements currently selected for display
    void GetSelectedMeasurements(std::vector<int>&);
    std::string SetSelectedMeasurements(std::vector<int>&, const char* user = _T("Settins"));
    double getUnitScale();
	void ChangeShapeIndex();
	/// Not used? 
	int m_nShapeIndex;
	/// Always 1.0? 
	double g_shapethresh;
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void LoadSettings(char* user);
	void SaveSettings(const char* user);
	CDocument* OpenDocumentFile(LPCTSTR lpszFileName); 
	void OnSavePCAPoints();
	void OnSetEllipseSize();
	/// Returns the number of slices currently selected. 
	int GetSelectedSlices();
	MeasurementSet getMeasurementSet() { return m_measurementSet; }
	void editMeasurements(bool remEdit, CString newStr, int index); 
	void ClearUserMeasurements(); 
	void LoadUserMeasurements(CString userCol); 
	void setMeasurementGroupSelected(CString name, bool select); 
	int getMeasurementGroupSelected(CString name); 
	void removeMeasurementGroup(CString name); 

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnAppDisclaimer();
	afx_msg void OnCmdSettings();
//    afx_msg void OnDataSelection();
    afx_msg void OnMeasureSelection(void);
    afx_msg void OnScannerDpiSetting();
    afx_msg void OnCmdPointiLevelControl(void);
	afx_msg void OnCmdBlockinessPosition();
	afx_msg void OnCmdColorTest();
	afx_msg void OnCmdAddMeasure();							    
	afx_msg void OnLoadUserSettings();
	afx_msg void OnSaveUserSettings();
	afx_msg void OnCmdAddData();
	//afx_msg void OnAsymmetryCheck();

	// allow users to draw DPI - 8/4/2015
	void OnCmdAdjustDPI();

	DECLARE_MESSAGE_MAP()

	protected:
		void CTomatoAnalyzerApp::OnBnClickedManualDpi();
	
};

extern CTomatoAnalyzerApp theApp;