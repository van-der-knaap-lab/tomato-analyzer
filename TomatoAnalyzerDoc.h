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
// TomatoAnalyzerDoc.h : interface of the CTomatoAnalyzerDoc class
//
#pragma once

// Indices in Revise menu
#define ID_PROXIMAL_POINT			5
#define ID_DISTAL_POINT				6

// Old indices for indicating the measurement to draw-- probably redundant to measurement IDs
#define INDEX_PERIMETER		0
#define INDEX_AREA			1

// Hints to CFruitDataView to cause it to update itself in different ways
#define DOC_UPDATEVIEW_NORMAL           0		// Not used? 
#define DOC_UPDATEVIEW_CLOSEIMAGE       10		// Not used? 
#define DOC_UPDATEVIEW_FRUITSWITCH      11		// Fruit selection changed
#define DOC_UPDATEVIEW_MEASURECHANGED   12      // Measurement selection changed
#define DOC_UPDATEVIEW_FRUITESTATE      13		// ? 


class CProgressDialog;
class CTomatoAnalyzerView;

class CTomatoAnalyzerDoc : public CDocument
{
protected: // create from serialization only

	DECLARE_DYNCREATE(CTomatoAnalyzerDoc)

	// Attributes
public:
	friend class CTomatoAnalyzerApp;
	/// The list of all slices. Note that these are NOT in the order you would expect to see them (row-wise, 
	/// left-to-right). To get a list in order, use GetAllSlicesByNaturalSequence or GetSelectedSliceByNaturalSequence. 
	CSliceList  m_SliceList;
	/// Path of the image (.jpg or .tif) file that is currently open. 
	CString     m_ImageFileName;
	bool        m_archivedData;
	bool IsCalibrated;
	CAdvImage*	m_pImage;
	/// True if the image has been analyzed
	BOOL		m_bAnalyzed;
	BOOL		m_bRevise;
	int			m_iSelectedDrawIndex;
	int			m_nThresholdValue;
	/// A string representing the application version, apparently, that gets serialized with the .tmt file. 
	/// Not sure how it's used. 
	char        m_strCurVersion[8];
	int			blueAmt;
	int			m_menuSelected;
	/// TRUE if the user has saved the file since opening it, FALSE otherwise (even if no changes have been made)
	BOOL		m_AlreadySaved;

	// Operations
public:
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	CTomatoAnalyzerDoc();
	CTomatoAnalyzerView* GetCurrentAnalyzerView();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageDoc)
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual CBinObjs * GetSlicesFromImage(CAdvImage ** inImage, CAdvImage* pImage, int nDesired, int& nNew, CProgressDialog * inProgDialog, int inMaxProg);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL
	// Implementation
public:
	virtual ~CTomatoAnalyzerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	POSITION	m_posSelectedObject;

private:
	BOOL DecideFileType(CString& fileSelected);
	void SaveIndividualSlices(std::fstream& f, char * szFileName);
	void SaveMeanValueOfAnalyzedResult(std::fstream& f, bool printStd);
	void SaveStdDevResult(std::fstream& f, bool printStd);
	void SaveAnalyzedMeasureItems(std::fstream& f);
	void SetArchiveFile(CString szImageFile);
	CString stripFileExt(CString file);

public:
	void OnDataSelection(void);
	void OnBlockySettingChange(void);
	bool CalibrateColor(double &lSlope, double &lIntercept, double &aSlope, double &aIntercept,
		double &bSlope, double &bIntercept, IlluminantObserver illumObs);
	void AnalyzeColor(double lSlope, double lIntercept, double aSlope, double aIntercept, double bSlope, double bIntercept,
		IlluminantObserver illumObs, std::vector<ColorRange> lRanges, std::vector<ColorRange> hueRanges, std::vector<ColorRange> chromaRanges,
		std::vector<ColorRange> userRanges);
	void OnColorSettingChange(bool doBatch, int isC2, bool calibrating);
	void ShowTiles(int index); //used when entering the L* a* b* values
	void ShowObjectAddData(int index); // Used when entering User Created Data for objects
	void GetArchiveName(LPSTR szBuffer);
	void decideSliceSequence(std::vector<int>& vct);
	void putSlicesInOrder(std::vector<int>& vct);
	CSlice * GetAnalyzeResult(POSITION pos);
	CSlice * GetSliceBySequenceIndex(int iSeq);
	void    GetSelectedSliceByNaturalSequence(std::vector<POSITION>&  vct);
	void	GetAllSlicesByNaturalSequence(std::vector<POSITION>& vctPos);
	VOID     SetSelectedObject(POSITION pos);
	POSITION DecideClickedObject(CPoint point);
	POSITION GetSelectedObject();
	CSlice * GetSelectedSlice(void);

	CSlice * GetSlice(POSITION pos);

	int  GetSelectedDrawIndex();
	void SetSelectedDrawIndex(int nWhich);

	BOOL	IsAnalyzed(void);
	BOOL    IsThereAnySliceChanged(void);
	BOOL    IsThereAnySliceRotated(void);
	bool    QueryForSaveModification(void);
	bool    QuerySave(bool nRet);

	void	CmdEllipseMove(CView *pSingleView);

	POSITION    GetSliceHeadPosition();
	CSlice *    GetNextSlice(POSITION& pos);

	void ExcludeUnusualObject(void);

	CAdvImage * GetImage(void);
	bool OpenSingleFile(CString fileName);


	// Generated message map functions
protected:
	//{{AFX_MSG(CImageDoc)
	void OnAnalyzeImage();
	void OnFileSave();
	void OnFileSaveAs();
	void OnCmdCloseImage();
	void OnCmdSaveResults();
	void OnCmdSetManualRevise();
	void OnCmdSetManualRotate();
	/// Message handler for manual revise of boundary
	void OnCmdShapeAdjust();
	void OnCmdResetRotation();
	void OnCmdResetShape();
	void OnCmdAutoRotation();
	void OnCmdAngleAdjust(UINT nID); /// Message handler for manual revise of distal point or proximal point
	void OnCmdTipAreaAdjust();       /// Message handler for manual revise of distal end protrusion
	void OnCmdResetAutoRotate();
	void OnCmdEllipseAdjust();
	void OnCmdResetTipArea();
	void OnCmdResetEllipse();
	void OnCmdEliminateTipArea();
	void OnCmdAdjustPericarpBoundary();
	void OnCmdSetPericarpSize();
	void OnCmdResetPericarp();
	/// Message handler for flat proximal or distal angle 
	void OnCmdFlatProximalMicro();
	void OnCmdFlatProximalMacro();
	void OnCmdFlatDistalMicro();
	void OnCmdFlatDistalMacro();
	void OnCmdResetProximalMicro();
	void OnCmdResetProximalMacro();
	void OnCmdResetDistalMicro();
	void OnCmdResetDistalMacro();
	void OnCmdPepperPericarp();
	void OnCmdResetPepperPericarp();

	void OnUpdateCmdExportDataUI(CCmdUI* pCmdUI);
	void OnUpdateCmdAnalyzeImageUI(CCmdUI* pCmdUI);
	void OnUpdateCmdAnalyzeColorUI(CCmdUI* pCmdUI);
	void OnUpdateCmdSetManualReviseUI(CCmdUI* pCmdUI);
	void OnUpdateCmdShapeAdjustUI(CCmdUI* pCmdUI);
	void OnUpdateCmdResetRotationUI(CCmdUI* pCmdUI);
	void OnUpdateCmdResetBoundaryUI(CCmdUI* pCmdUI);
	void OnUpdateCmdAutoRotationUI(CCmdUI* pCmdUI);
	void OnUpdateCmdSaveFruit(CCmdUI* pCmdUI);
	void OnUpdateCmdCloseImage(CCmdUI* pCmdUI);
	void OnUpdateCmdTipAreaAdjust(CCmdUI* pCmdUI);
	/// Enables/disables "Add/Edit Data" menu item for user-created tab
	void OnUpdateCmdUserCreatedData(CCmdUI* pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateCmdZoomViewUI(CCmdUI* pCmdUI);
	afx_msg void OnCmdUpdateAngleAdjust(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	//public:
	//	afx_msg void OnAsymmetryCheck();
};
