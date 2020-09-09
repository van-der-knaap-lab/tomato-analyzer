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
// UserAddData Dialog Header file

#include "Resource.h"			// You have to include all of these to be able to include TomatoAnalyzerDoc.h...
#include "ProgressDialog.h"		//  
#include "BoundingBoxList.h"	//
#include "BinObjs.h"			//
#include "AdvImage.h"			//
#include "Slice.h"				//
#include "TomatoAnalyzer.h"		// ...
#include "TomatoAnalyzerDoc.h"

class UserAddData : public CDialog
{
	DECLARE_DYNAMIC(UserAddData)

public:
	UserAddData(CWnd* pParent = NULL);   // standard constructor
	UserAddData(CWnd* pParent, std::vector<CString> tempVec, int numFruit, CTomatoAnalyzerDoc* doc); //Secondary constructor
	virtual ~UserAddData();
	CComboBox m_comboColumnHeading; // Combo Box object for the Column Heading dropdownlist
	CComboBox m_comboFruitNumber; // Combo Box object for the Fruit Number dropdownlist
	CEdit m_editBox; // Edit box used to display/store the data
	//virtual BOOL OnSecondInit(std::vector<char *> &tempVec);

// Dialog Data
	enum { IDD = IDD_ADDDATA };

private: 
	CTomatoAnalyzerDoc* m_doc; 
	std::vector<POSITION> m_slicePositions;
	/// The position of the dialog the last time it was shown, or NULL if the dialog has not yet been shown
	static CPoint * m_topLeft;
	/// Stores the current position of the dialog
	void rememberPosition(); 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); // Initializes the dialog objects

	DECLARE_MESSAGE_MAP()
public:
	// regular "global" variables 
	bool finished, special; // Booleans to check if Finish was pressed and if the overloaded constructor is to be used
	CString box1; // String variable for the text in the CEdit box in the dialog
	int m_chNum, m_fnNum, numberFruits; // indices for the two combo boxes as well as the number of fruits (used to populate the fruit combo box)
	std::vector<CString> workingVec; // Used to place the column names into the combo box
	// MFC-generated handling functions
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedFinish();
	afx_msg void OnCbnSelchangeCombo();
};

