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

#include <vector>
using namespace std; 

// Header file for the Enter L*a*b* Dialog

// LABDialog dialog

class LABDialog : public CDialog
{
	DECLARE_DYNAMIC(LABDialog)

public:
	LABDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~LABDialog();

// Dialog Data
	enum { IDD = IDD_LAB_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); // Initializes the dialog objects
	afx_msg void OnCbnSelchangePalette();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedFinish();
	afx_msg void OnBnClickedFile();

	DECLARE_MESSAGE_MAP()

private:
	CTomatoAnalyzerDoc * doc; 
	vector<POSITION> slicePos; 
	/// Combo Box object for the Tile Selected drop down list
	CComboBox m_comboTiles; 
	/// Index variable for the combo box
	int m_TileNum; 
	double lBox; 
	double aBox; 
	double bBox; 

	void readFromCurrentTile(); 
	void writeToCurrentTile(); 
	bool splitNextLine(istream& in, vector<string>& cols); 

};
