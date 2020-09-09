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
// TomatoAnalyzerDoc.cpp : implementation of the CTomatoAnalyzerDoc class


#include "stdafx.h"
#include <io.h>
#include "Resource.h"
#include "ProgressDialog.h"
#include "BoundingBoxList.h"
#include "BinObjs.h"
#include "AdvImage.h"
#include "Slice.h"
#include "MainFrm.h"
#include "TomatoAnalyzer.h"
#include "TomatoAnalyzerDoc.h"
#include "TomatoAnalyzerView.h"
#include "ParameterSettings.h"
#include "SingleSliceView.h"
#include "SliceInfoFormView.h"
#include "MeasureItemList.h"
#include "FruitDataView.h"
#include "TracKShape.h"
#include "TipAreaTrack.h"
#include "TriangleTracker.h"
#include "RotationTracker.h"
#include "new_img_Function.h"
#include "CheckboxTreeCtrl.h"
#include "SelectMeasurement.h"
#include "EllipseTracker.h"
#include "TipAreaTracker.h"
#include "carpSize.h"
#include "PeriTracker.h"
#include "Batch.h"
#include "linreg.h"
#include "ColorTest.h"
#include "MeasurementSet.h"
#include "Msg.h"
#include "UserDefinedDialog.h"
#include "LABDialog.h"
#include "OldTmtDialog.h"  // added on 6/30/2015
#include <fstream>
using namespace std;

#include <iostream>
#include <string.h>
#include <fstream>
#include <iomanip>
#include <winuser.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include ".\tomatoanalyzerdoc.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern void sortvalue(std::vector<int>& row, std::vector<int>& seqIndex);

template<class T> T median(std::vector<T> vct)
{
	int n = vct.size();
	if(n==0)
		return T(0);
    if(n==1)
        return vct.at(0);

	std::sort(vct.begin(), vct.end());
	if((n & 2)==1)
		return vct.at((n+1)>>2);

	return (vct.at(n>>2)+vct.at((n>>2)+1))/2;
}
// CTomatoAnalyzerDoc

IMPLEMENT_DYNCREATE(CTomatoAnalyzerDoc, CDocument)

BEGIN_MESSAGE_MAP(CTomatoAnalyzerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_SAVE_FRUIT, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_IMAGE_CLOSE, OnCmdCloseImage)
	ON_COMMAND(ID_EXPORT_DATA, OnCmdSaveResults)
	ON_COMMAND(ID_ANALYZE_IMG, OnAnalyzeImage)
	ON_COMMAND(ID_MANUAL_REVISE, OnCmdSetManualRevise)
	ON_COMMAND(ID_MANUAL_ROTATE, OnCmdSetManualRotate)
	ON_COMMAND(ID_MANUAL_SHAPE, OnCmdShapeAdjust)
	ON_COMMAND(ID_MANUAL_RESET_SHAPE, OnCmdResetShape)
	ON_COMMAND(ID_MANUAL_RESET_ROTATE, OnCmdResetRotation)
    ON_COMMAND(IDC_DATA_SELECTION, OnDataSelection)
	ON_COMMAND(ID_REVISE_RESETAUTO, OnCmdResetAutoRotate)
	ON_COMMAND(ID_REVISE_ADJUSTELLIPSE, OnCmdEllipseAdjust)
	ON_COMMAND(ID_REVISE_RESETELLIPSE, OnCmdResetEllipse)
	ON_COMMAND(ID_REVISE_ADJUSTTIPAREA, OnCmdTipAreaAdjust)
	ON_COMMAND(ID_REVISE_RESETTIPAREA, OnCmdResetTipArea)
	ON_COMMAND(ID_ELIMINATE, OnCmdEliminateTipArea)
	ON_COMMAND(ID_INNER_BOUNDARY, OnCmdAdjustPericarpBoundary)
	ON_COMMAND(ID_PERICARPSIZE, OnCmdSetPericarpSize)
	ON_COMMAND(ID_RESETINNER, OnCmdResetPericarp)
	ON_COMMAND(ID_FLAT_PROXIMAL_MICRO, OnCmdFlatProximalMicro)     // added 6-2-15
	ON_COMMAND(ID_FLAT_PROXIMAL_MACRO, OnCmdFlatProximalMacro)     // added 6-2-15
	ON_COMMAND(ID_FLAT_DISTAL_MICRO, OnCmdFlatDistalMicro)         // added 6-2-15
	ON_COMMAND(ID_FLAT_DISTAL_MACRO, OnCmdFlatDistalMacro)         // added 6-2-15
	ON_COMMAND(ID_RESET_PROXIMAL_MICRO, OnCmdResetProximalMicro)   // added 6-5-15
	ON_COMMAND(ID_RESET_PROXIMAL_MACRO, OnCmdResetProximalMacro)   // added 6-5-15
	ON_COMMAND(ID_RESET_DISTAL_MICRO, OnCmdResetDistalMicro)       // added 6-5-15
	ON_COMMAND(ID_RESET_DISTAL_MACRO, OnCmdResetDistalMacro)       // added 6-5-15
	ON_COMMAND(ID_PEPPER_PERICARP, OnCmdPepperPericarp)            // added 6-9-15
	ON_COMMAND(ID_RESET_PEPPER_PERICARP, OnCmdResetPepperPericarp) // added 6-15-15

    ON_COMMAND(ID_AUTO_ROTATE, OnCmdAutoRotation)
    ON_COMMAND_RANGE(ID_MANUAL_ANGLE_FIRST, ID_MANUAL_ANGLE_LAST, OnCmdAngleAdjust)

	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateCmdSaveFruit)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateCmdSaveFruit)
	ON_UPDATE_COMMAND_UI(ID_SAVE_FRUIT, OnUpdateCmdSaveFruit)
	ON_UPDATE_COMMAND_UI(ID_IMAGE_CLOSE, OnUpdateCmdCloseImage)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_DATA, OnUpdateCmdExportDataUI)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_IMG, OnUpdateCmdAnalyzeImageUI)
	ON_UPDATE_COMMAND_UI(ID_ANALYZE_COLOR, OnUpdateCmdAnalyzeColorUI)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_RESET_ROTATE, OnUpdateCmdResetRotationUI)
    ON_UPDATE_COMMAND_UI(ID_MANUAL_RESET_SHAPE, OnUpdateCmdResetBoundaryUI)
	ON_UPDATE_COMMAND_UI(IDC_DATA_ADDINFO, OnUpdateCmdUserCreatedData)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZOOM, ID_ZOOM200, OnUpdateCmdZoomViewUI)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MANUAL_REVISE, ID_MANUAL_SHAPE, OnUpdateCmdSetManualReviseUI)
    ON_UPDATE_COMMAND_UI_RANGE(ID_MANUAL_ANGLE_FIRST, ID_MANUAL_ANGLE_LAST, OnCmdUpdateAngleAdjust)

END_MESSAGE_MAP()


// CTomatoAnalyzerDoc construction/destruction

/** 
 * Constructs an empty document. 
 */
CTomatoAnalyzerDoc::CTomatoAnalyzerDoc()
{
	m_bAnalyzed = FALSE;
	m_nThresholdValue = 0;
    m_archivedData = false;

	m_pImage  = NULL;
	m_bRevise = FALSE;

	m_AlreadySaved = FALSE;

	m_posSelectedObject = NULL;

	m_iSelectedDrawIndex = INDEX_AREA;

    //lstrcpy(m_strCurVersion, "TM3"); // It used to be "TM3" indicating the version number was 3.0
	/**
	 * From now on, we decide to change the naming rule to "TA" + version number, where "TA" stands for Tomato Analyzer (6/30/2015)
	 */
	strcpy(m_strCurVersion, "TA4.0");  // Current version: Tomato Analyzer 4.0 (6/30/2015)
}

CTomatoAnalyzerDoc::~CTomatoAnalyzerDoc()
{
}

/** 
 * Restores the document to an empty state. 
 */ 
void CTomatoAnalyzerDoc::DeleteContents()
{
	m_posSelectedObject = NULL;
	m_bAnalyzed = FALSE;
    m_archivedData = false;

	if(NULL != m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	POSITION pos = m_SliceList.GetHeadPosition();
	while (pos != NULL)
		delete m_SliceList.GetNext(pos);

	m_SliceList.RemoveAll();
}

/// Superfluous override of CDocument::OnNewDocument()? 
BOOL CTomatoAnalyzerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/** 
 * Saves or loads (depending on the state of 'ar') the document, including all slices, to/from the given archive.  
 */
void CTomatoAnalyzerDoc::Serialize(CArchive& ar)
{
    CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp *) AfxGetApp();
	CSlice * pSlice;

    if (ar.IsStoring())  // Saving
	{
        ar.SetObjectSchema(1);
        ar.Write(m_strCurVersion, 5);
        ar << m_ImageFileName;

        ar << pApp->m_nMac;				// Ignored, but left in for backward compatibility
        ar << pApp->m_nMic;				// 
        ar << pApp->m_fBlkUp;			// 
        ar << pApp->m_fBlkDn;			// 
        ar << pApp->m_dpi;				// 
        ar << pApp->m_iUnit;			// 
        ar << pApp->m_nShapeIndex;		// 
		//ar << pApp->m_nBrightLevel;
		//ar << pApp->m_ellipseSize;
		

        ar << m_nThresholdValue;
        ar << m_SliceList.GetCount();
	}
	else  // Loading
	{
        char szVersion[8];
		ar.Read(szVersion, 5);

		if (szVersion[0] != m_strCurVersion[0] ||
			szVersion[1] != m_strCurVersion[1] ||
			szVersion[2] != m_strCurVersion[2] ||
			szVersion[3] != m_strCurVersion[3] ||
			szVersion[4] != m_strCurVersion[4])
		{
			CArchiveException  * e = new CArchiveException;
			throw e;
			/** 
			 * New measurements are added in TA 4.0. The old .tmt files produced by the versions prior to 4.0 are not compatible with TA 4.0.
			 * Here, we handle this issue by showing up a dialog and stopping deserializing. (7/1/2015)
			 */
			if ((int)szVersion[2] < 4)
			{
				m_archivedData = false;
				return;
			}
		}

        CString rString;
		ar >> rString;

        float fdummy;
        int tmp;

        ar >> tmp;                 //ar >> pApp->m_nMac;
        ar >> tmp;                 //ar >> pApp->m_nMic;
        ar >> fdummy;              //ar >> pApp->m_fBlkUp;
        ar >> fdummy;              //ar >> pApp->m_fBlkDn;
        ar >> tmp;                 //ar >> pApp->m_dpi;

        ar >> tmp;                //ar >> pApp->m_iUnit;
        
        ar >> pApp->m_nShapeIndex;
		//ar >> pApp->m_nBrightLevel;

        // TODO: add loading code here

        ar >> m_nThresholdValue;

 

		int nSliceNumber;
		ar >> nSliceNumber;
		for(int iloop=0; iloop<nSliceNumber; iloop++)
		{
			pSlice = new CSlice();
			m_SliceList.AddTail(pSlice);
		}

        m_archivedData = true;
	}

	POSITION pos = m_SliceList.GetHeadPosition();
	bool first = true; 
	// Loops through the slices and saves or loads the data to/from the TMT
	while (pos != NULL)
    {
        pSlice = m_SliceList.GetNext(pos);
		if (first) {
			pSlice->SliceOne = true; 
			first = false; 
		}
		std::vector<CString> sliceData, colGroups;
		// Gets the user-created measurements
		MeasurementSet temp = ((CTomatoAnalyzerApp*)AfxGetApp())->getMeasurementSet();
		colGroups = temp.measurementsInGroup(CMsg::load(IDS_GROUP_USER_CREATED));
		int columns = colGroups.size();
		// (should) only matter(s) for saving to TMT file
		if (columns > 0)
		{
			// Builds a CString vector of the data for each slice
			for (int i = 0; i < columns; i++)
			{
				CString tempData;
				tempData = pSlice->getUserCreatedData(colGroups[i]);
				sliceData.push_back(tempData);
			}
		}
		// Jumps to Slice.cpp's Serialize function to save/load to TMT
        pSlice->Serialize(ar, sliceData);		
    }
}


// CTomatoAnalyzerDoc diagnostics

#ifdef _DEBUG
void CTomatoAnalyzerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTomatoAnalyzerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTomatoAnalyzerDoc commands
void ExtractFilaNameOnly(LPCSTR szFilePathAndName, LPSTR szFileNameOnly)
{
    int nLength = lstrlen(szFilePathAndName);
    int nBegin = nLength-1;
    while(nBegin>=0)
    {
        if(szFilePathAndName[nBegin]=='\\' || szFilePathAndName[nBegin]==':')
        {
            ++nBegin;
            break;
        }

        --nBegin;
        continue;
    }

    szFileNameOnly[0]=0;
    lstrcpyn(szFileNameOnly, szFilePathAndName+nBegin, nLength-nBegin+1);
    return;
}

BOOL CTomatoAnalyzerDoc::DecideFileType(CString& fileSelected)
{
    char ext[_MAX_EXT];
    memset(ext, 0, sizeof(ext));
    _splitpath(fileSelected, NULL, NULL, NULL, ext);

	// If the image is a JPG or a TIFF file, it parses the extension off and makes a .tmt file
	// to be used in TA. Adding other extensions here, provided that they are supported by CVIPtools,
	// will allow other types of files to be loaded.
    if(0==lstrcmpi(ext, ".jpg") || 0==lstrcmpi(ext, ".tif"))
    {
        m_ImageFileName = fileSelected;

	    CString tmp = fileSelected;
        int nLength = tmp.GetLength();
	    
	    int iDotPos = tmp.Find('.');
	    tmp.Delete(iDotPos, nLength-iDotPos);

	    tmp += ".tmt";
        SetPathName(tmp, FALSE);
        return TRUE;
    }
	
    CFile archFile;
    BOOL    bDataFileOk=FALSE;
    if(TRUE == archFile.Open(fileSelected, CFile::modeRead))
    {
        CArchive ar(&archFile, CArchive::load);
		char szVersion[8];
		ar.Read(szVersion, 5);
		if (szVersion[0] == m_strCurVersion[0] &&
			szVersion[1] == m_strCurVersion[1] &&
			szVersion[2] == m_strCurVersion[2] &&
			szVersion[3] == m_strCurVersion[3] &&
			szVersion[4] == m_strCurVersion[4])
        {
            SetPathName(fileSelected, FALSE);
	    	ar >> m_ImageFileName;
            bDataFileOk = TRUE;
        }

        ar.Close();
        archFile.Close();
    }

    if(FALSE == bDataFileOk)
    {
        AfxMessageBox("Invalid data format, might be an old version", MB_OK);
        return FALSE;
    }

    if(0 != _access(m_ImageFileName, 04))
    {
        CString str = "cannot access the original image:\n";
        str += m_ImageFileName;
        AfxMessageBox(str, MB_OK);
        
        m_ImageFileName.Empty();
        return FALSE;
    }

    SetPathName(fileSelected, FALSE);
    return TRUE;
}

/**
 * Allows the user to open a single file or perform batch analysis on multiple files. 
 */
void CTomatoAnalyzerDoc::OnFileOpen()
{
	// This lists the File Type options in the Open... dialog.
	CFileDialog fdlg(TRUE,
					 NULL,
					 NULL,
					 OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_EXPLORER |OFN_FILEMUSTEXIST,
					 _T("Image Files|*.jpg;*.tif|Analysis Results (*.tmt)|*.tmt|All (*.*)|*.*||")
					 );

	int nBufferLength = 5*1024;
	TCHAR * lpszFileBuffer = new TCHAR[nBufferLength];
	memset(lpszFileBuffer, 0, nBufferLength);

	fdlg.m_ofn.nMaxFile  = nBufferLength;
	fdlg.m_ofn.lpstrFile = lpszFileBuffer;

	if(fdlg.DoModal()==IDOK)
	{
		int bType = 0;  //determines the type of batch mode
		//1.update singlesliceview
		POSITION	 pos = fdlg.GetStartPosition();

		fdlg.GetNextPathName(pos);
		BOOL bMultiFile;
		if(pos !=  NULL)
			bMultiFile = TRUE;
		else
			bMultiFile = FALSE;

		pos = fdlg.GetStartPosition();

		std::fstream fResult;
		BOOL bSaveHeader = TRUE;
		if(bMultiFile)
		{
			CBatch dlg;  //Choose Batch Option

			if (dlg.DoModal() == TRUE)
			{
				if(dlg.m_radio_basic == 1)   //basic
					bType = 0;
				else if (dlg.m_radio_std == 1)	//include standard deviation
					bType = 1;
				else if (dlg.m_radio_indiv == 1)  //show individual slices
					bType = 2;
			
				CFileDialog fsave(FALSE, 
								  "csv", 
								  NULL, 
								  OFN_EXPLORER |OFN_OVERWRITEPROMPT,
								  _T("CSV file (*.csv)|*.csv||")
								);

				if(fsave.DoModal()==IDOK)
				{
					CString strName;
					strName.Format("%s",fsave.GetPathName());
					fResult.open(strName, ios_base::out|ios_base::trunc);
					if(!fResult.is_open())
					{
						AfxMessageBox("Cannot open the specified file to save results", MB_ICONEXCLAMATION);

						delete [] lpszFileBuffer;
						return;
					}
				}
				
				else
					return;
			}

			else
				return;
		}

		bool saveAllFiles = false;
		if (bMultiFile) {
			CString msg;
			msg.Format("Do you want to save changes made these selected files?");
			int nRet = AfxMessageBox(msg, MB_YESNOCANCEL);
			if (nRet == IDYES) {
				saveAllFiles = true;
			}
			
		}

		while(pos)
		{
			CString strFile = fdlg.GetNextPathName(pos);

			QuerySave(saveAllFiles);

			bool closedPreviousFile = OpenSingleFile(strFile); 
			if (!closedPreviousFile)
				break; 

            //multiple selection
            if(bMultiFile)
            {
	    		MSG msg;
		        while(PeekMessage(&msg, NULL,  WM_PAINT, WM_PAINT, PM_NOREMOVE))
			    {
				    DispatchMessage(&msg);
				}

				if(m_archivedData != true){
					OnAnalyzeImage();
				}
				/***************Uses correct settings for old .tmt files in batch mode now**********************/
				else
				{
					
						POSITION pos = m_SliceList.GetHeadPosition();

						while (pos != NULL)
						{
							CSlice* pSlice = m_SliceList.GetNext(pos);
							//pSlice->CalcParameters(true); removed by Hale
							pSlice->SliceOne = true;
							UpdateAllViews(NULL);
						}
				}

                if(bSaveHeader)
                {
                    fResult << "image files,";
                    m_SliceList.GetHead()->ExportHeader(fResult);
                    fResult << endl;

                    bSaveHeader = FALSE;
                }

                char szFileNameOnly[1024];
                ExtractFilaNameOnly(strFile, szFileNameOnly);
				
				if (bType == 0 || bType == 1)  //Basic and Std. Dev.
				{
					fResult<<szFileNameOnly<<", ";
					//std deviation
					bool stdDev = false;
					if (bType == 1)
						stdDev = true;

					SaveMeanValueOfAnalyzedResult(fResult, stdDev);
					fResult<<endl;
				}
				else if (bType == 2)  //Individual Images
				{
					SaveIndividualSlices(fResult, szFileNameOnly);
				}
			}
		}

        if(bMultiFile)
        {
            fResult.flush();
            fResult.close();
        }

		POSITION slicePos = m_SliceList.GetHeadPosition();
		if (slicePos != NULL) {
			CSlice* pSlice = m_SliceList.GetNext(slicePos);
			pSlice->SliceOne = true;
			
		}
	}
	delete[] lpszFileBuffer;

}

bool CTomatoAnalyzerDoc::OpenSingleFile(CString fileName) 
{
	//removed by Hale.
    /*if (!QueryForSaveModification())
		return false; */
	/*if (!QuerySave(saveAllFiles))
		return false;*/


	DeleteContents();
	m_AlreadySaved = FALSE; 

	if(TRUE==DecideFileType(fileName))
	{
		m_pImage = new CAdvImage();
		if(m_pImage->Read((LPCSTR)m_ImageFileName))
			{
			// if the image has a TMT file, then deserialize!
			CFile archFile;
			if(TRUE == archFile.Open(GetPathName(), CFile::modeRead))
			{
		
                // This is used to show the color results when a tmt file is opened
				CArchive archive(&archFile, CArchive::load);

                try
                {
                    Serialize(archive);
                }
                catch(CArchiveException * e)
                {
                    e->Delete();
                }

                archive.Close();
                archFile.Close();

                if(m_archivedData)
                    m_bAnalyzed = TRUE;
				else
				{
					OldTmtDialog oldTmtDlg;  // Pop up dialog to handle old tmt compatibility issue
					oldTmtDlg.DoModal();
				}
            }
	    }
	    else
	    {
		   delete m_pImage;
		    m_pImage = NULL;
	   }
	}

	CView * pView = GetCurrentAnalyzerView(); 
	pView->OnInitialUpdate();
	UpdateAllViews(pView);
	AfxGetApp()->AddToRecentFileList(fileName); 
	return true; 
}


void CTomatoAnalyzerDoc::OnFileSave()
{
    OnSaveDocument(GetPathName());
	m_AlreadySaved = TRUE; 
}

void CTomatoAnalyzerDoc::OnFileSaveAs()
{
    TCHAR   lpszFileBuffer[MAX_PATH];
	memset(lpszFileBuffer, 0, sizeof(lpszFileBuffer));
    wsprintf(lpszFileBuffer, GetPathName());

	CFileDialog fdlg(FALSE, 
		             NULL, 
				     NULL, 
				     OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT,
		             _T("Analysis Result (*.tmt)|*.tmt||")
				   );

	fdlg.m_ofn.nMaxFile  = sizeof(lpszFileBuffer);
	fdlg.m_ofn.lpstrFile = lpszFileBuffer;
    fdlg.m_ofn.lpstrDefExt = "tmt";

	if(fdlg.DoModal()==IDOK)
	{
        OnSaveDocument(lpszFileBuffer);
		m_AlreadySaved = TRUE; 
    }
}

/**
 * Auto-rotates all slices (not just the selected one). 
 */
void CTomatoAnalyzerDoc::OnCmdAutoRotation()
{
	m_menuSelected = ID_AUTO_ROTATE;

	CWaitCursor waitcursor;
	POSITION pos = m_SliceList.GetHeadPosition();
	CSlice * pSlice = GetSlice( pos );
	while( pos != NULL )
	{
		if(! pSlice->GetExclusionFlag())
		{
			double angle = pSlice->GetAutoRotateAngle();
			pSlice->Rotate( angle );
		}
		pSlice = m_SliceList.GetNext(pos);
	}
	UpdateAllViews(NULL);
}

void CTomatoAnalyzerDoc::OnCmdResetAutoRotate()
{
	m_menuSelected = ID_REVISE_RESETAUTO;
	CWaitCursor waitcursor;
	POSITION pos = m_SliceList.GetHeadPosition();
	CSlice * pSlice = GetSlice( pos );
    while( pos != NULL )
    {
        pSlice->Rotate(0.0);
       	UpdateAllViews(NULL);
		pSlice = m_SliceList.GetNext(pos);
    }
    return;
}

void CTomatoAnalyzerDoc::OnCmdSetManualRevise()
{
	switch (m_menuSelected)
	{
	case ID_MANUAL_ROTATE:
		OnCmdSetManualRotate();
		break;
	case ID_MANUAL_RESET_ROTATE:
		OnCmdResetRotation();
		break;
	case ID_MANUAL_SHAPE:
		OnCmdShapeAdjust();
		break;
	case ID_MANUAL_RESET_SHAPE:
		OnCmdResetShape();
		break;
	case ID_AUTO_ROTATE:
		OnCmdAutoRotation();
		break;
	case ID_REVISE_RESETAUTO:
		OnCmdResetAutoRotate();
		break;
	case ID_REVISE_ADJUSTELLIPSE:
		OnCmdEllipseAdjust();
		break;
	case ID_REVISE_RESETELLIPSE:
		OnCmdResetEllipse();
		break;
	case ID_REVISE_ADJUSTTIPAREA:
		OnCmdTipAreaAdjust();
		break;
	case ID_REVISE_RESETTIPAREA:
		OnCmdResetTipArea();
		break;
	case ID_ELIMINATE:
		OnCmdEliminateTipArea();
		break;
	case ID_PROXIMAL_POINT:
		OnCmdAngleAdjust(ID_MANUAL_ANGLE_STEM);
		break;
	case ID_DISTAL_POINT:
		OnCmdAngleAdjust(ID_MANUAL_ANGLE_BLM);
		break;
	case ID_INNER_BOUNDARY:
		OnCmdAdjustPericarpBoundary();
		break;
	case ID_PERICARPSIZE:
		OnCmdSetPericarpSize();
		break;
	case ID_RESETINNER:
		OnCmdResetPericarp();
		break;
	case ID_FLAT_PROXIMAL_MICRO:
		OnCmdFlatProximalMicro();
		break;
	case ID_FLAT_PROXIMAL_MACRO:
		OnCmdFlatProximalMacro();
		break;
	case ID_FLAT_DISTAL_MICRO:
		OnCmdFlatDistalMicro();
		break;
	case ID_FLAT_DISTAL_MACRO:
		OnCmdFlatDistalMacro();
		break;
	case ID_RESET_PROXIMAL_MICRO:
		OnCmdResetProximalMicro();
		break;
	case ID_RESET_PROXIMAL_MACRO:
		OnCmdResetProximalMacro();
		break;
	case ID_RESET_DISTAL_MICRO:
		OnCmdResetDistalMicro();
		break;
	case ID_RESET_DISTAL_MACRO:
		OnCmdResetDistalMacro();
		break;
	case ID_PEPPER_PERICARP:
		OnCmdPepperPericarp();
		break;
	case ID_RESET_PEPPER_PERICARP:
		OnCmdResetPepperPericarp();
		break;
	default:
		return;
	}
	return;	
}

void CTomatoAnalyzerDoc::OnCmdSetManualRotate()
{
	m_menuSelected = ID_MANUAL_ROTATE;
	POSITION    pos = GetSelectedObject();
    CSlice * pCurSlice = GetSlice(pos);

	CAdvImage *pSliceImage = m_pImage->Crop(pCurSlice->mBBox.GetRect());
    if(NULL == pSliceImage)
        return;

    double angle = pCurSlice->GetRotationDegree();
    BOOL bRet = FALSE;

	CTomatoAnalyzerView *    pAnalyzerView = NULL;
    pos = GetFirstViewPosition();
	while(NULL != pos)
	{
		CView * pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSingleSliceView)))
		{
            CSingleSliceView * pSliceView = (CSingleSliceView *)pView;

            POINT pt = pSliceView->GetOffsetPoint();
            double fScale = pSliceView->GetScale();
            if(1.0 != fScale)
            {
                CAdvImage * pScaled = CIimresize(pSliceImage, fScale, "near");
                delete  pSliceImage;
                pSliceImage = pScaled;
            }

		    CRotationTracker    rot;
            bRet = rot.Track(pView, pSliceImage, pt, angle);
            continue;
		}

        if(pView->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerView)))
            pAnalyzerView = (CTomatoAnalyzerView *)pView;
	}

    delete pSliceImage;
    if(bRet)
    {
        pCurSlice->Rotate(angle);
        UpdateAllViews(NULL);
    }
    else
    {
        UpdateAllViews(pAnalyzerView);
    }

	return;
}

void CTomatoAnalyzerDoc::OnCmdResetRotation()
{
	m_menuSelected = ID_MANUAL_RESET_ROTATE;
    CSlice * pSlice = GetSlice(m_posSelectedObject);
    if(NULL != pSlice)
    {
        pSlice->Rotate(0.0);
       	UpdateAllViews(NULL);
    }
}

void CTomatoAnalyzerDoc::OnCmdResetShape()
{
	m_menuSelected = ID_MANUAL_RESET_SHAPE;
	POSITION pos=m_posSelectedObject;
    CSlice * pSlice = GetSlice(pos);
    if(NULL != pSlice)
    {
        pSlice->ResetBoundary();

        RECT    rect = pSlice->m_OrgBounderyBox.GetRect();
        CAdvImage * pOrgImage = m_pImage->Crop(rect);
        if(NULL == pOrgImage)
            return;

        int nNew;
 	    CBinObjs  * myObjs=GetSlicesFromImage(NULL, pOrgImage, m_nThresholdValue, nNew, NULL, 0);
	    if(myObjs)
	    {
		    CBONode * head = myObjs->GetHead();
            
            CPixelList::iterator    myIter  = head->mPixList->begin();
            CPixelList::iterator    IterEnd = head->mPixList->end();
            while(myIter != IterEnd)
            {
                myIter->x += rect.left;
                myIter->y += rect.top;

                ++myIter;
            }

            pSlice->Set(head->mPixList);
		    head->mPixList = NULL;
    		delete myObjs;
        }

        delete pOrgImage;

		pSlice->CalcParameters(FALSE);

	    UpdateAllViews(NULL);
    }
}


void CTomatoAnalyzerDoc::OnUpdateCmdExportDataUI(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bAnalyzed);
}

void CTomatoAnalyzerDoc::OnUpdateCmdSetManualReviseUI(CCmdUI* pCmdUI)
{
	if(m_bRevise)
	{
		BOOL bSet = ((pCmdUI->m_nID==m_bRevise) || (pCmdUI->m_nID==ID_MANUAL_REVISE));

		pCmdUI->Enable(bSet);
		pCmdUI->SetCheck(bSet);
	}
	else
	{
		pCmdUI->Enable(m_bAnalyzed && NULL!=m_posSelectedObject);
		pCmdUI->SetCheck(FALSE);
	}
}

void CTomatoAnalyzerDoc::OnUpdateCmdAnalyzeImageUI(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((NULL != m_pImage));
}

void CTomatoAnalyzerDoc::OnUpdateCmdAnalyzeColorUI(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((NULL != m_pImage));
}

void CTomatoAnalyzerDoc::OnUpdateCmdResetBoundaryUI(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	POSITION pos=m_posSelectedObject;

	if(m_bAnalyzed && pos!=NULL)
	{
        CSlice * pSlice = GetSlice(pos);
        bEnable = pSlice->IsBoundaryAdjusted();
	}
	
	pCmdUI->Enable(bEnable);
}

void CTomatoAnalyzerDoc::OnUpdateCmdResetRotationUI(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;
	POSITION pos=m_posSelectedObject;

	if(m_bAnalyzed && pos!=NULL)
	{
        CSlice * pSlice = GetSlice(pos);
        bEnable = (0.0 != pSlice->GetRotationDegree())? TRUE : FALSE;
	}
	
	pCmdUI->Enable(bEnable);
}

void CTomatoAnalyzerDoc::OnUpdateCmdSaveFruit(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_bAnalyzed);
    return;
}

/** 
 * Closes the current document. First checks with the user if there are any unsaved changes. 
 */
void CTomatoAnalyzerDoc::OnCmdCloseImage()
{
	if (! QueryForSaveModification())
		return; 

    DeleteContents();
    UpdateAllViews(NULL);
}

void CTomatoAnalyzerDoc::OnUpdateCmdCloseImage(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(NULL != m_pImage);
}

void CTomatoAnalyzerDoc::OnUpdateCmdZoomViewUI(CCmdUI* pCmdUI)
{
	if(NULL != m_pImage)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTomatoAnalyzerDoc::OnUpdateCmdTipAreaAdjust(CCmdUI* pCmdUI)
{
	if(NULL != m_posSelectedObject)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTomatoAnalyzerDoc::OnCmdUpdateAngleAdjust(CCmdUI* pCmdUI)
{
	if(NULL != m_pImage)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTomatoAnalyzerDoc::OnUpdateCmdUserCreatedData(CCmdUI* pCmdUI) {
	MeasurementSet m = ((CTomatoAnalyzerApp *)AfxGetApp())->getMeasurementSet();
    pCmdUI->Enable( m_bAnalyzed && ! m.measurementsInGroup(CMsg::load(IDS_GROUP_USER_CREATED)).empty() );	
}

CAdvImage * CTomatoAnalyzerDoc::GetImage(void)
{
	return m_pImage;
}

//Menu option to adjust internal ellipse
//Allows ellipse to be resized then moved
void CTomatoAnalyzerDoc::OnCmdEllipseAdjust()
{
	m_menuSelected = ID_REVISE_ADJUSTELLIPSE;
	CView *pView = NULL;
    CView * pSingleView = NULL;
    CView * pMainView = NULL;
	POSITION pos = GetFirstViewPosition();
    CBitmap * bp = NULL;
	while(NULL != pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSingleSliceView)))
		{
            bp = ((CSingleSliceView *)pView)->getScaledBitmap();
            pSingleView = pView;
			continue;
		}

		if(pView->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerView)))
		{
            pMainView = pView;
			continue;
		}
	}

    POINT   pt[3];
	POINT ptIn[2];

	CSlice* pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->GetEllipsePoints(pt);	//Get the current ellipse points

	ptIn[0] = pt[0];	//Left point
	ptIn[1] = pt[1];	//Top point
	//Resize   
	CEllipseTracker    tracker;
    tracker.Track(pSingleView, bp, ptIn);
	tracker.GetResult(ptIn);
	

	pSlice->SetEllipseManual(true);
	pSlice->SetEllipsePoints(ptIn);
	
	UpdateAllViews(pMainView);
	
	//Move
	CmdEllipseMove(pSingleView);
	
}

//Resets the internal ellipse to the default size
void CTomatoAnalyzerDoc::OnCmdResetEllipse()
{
	m_menuSelected = ID_REVISE_RESETELLIPSE;
	CSlice* pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->SetEllipseManual(true);
	pSlice->SetEllipseDefault();

	UpdateAllViews(NULL);
	
}

//Allows the ellipse to be re-centered
void CTomatoAnalyzerDoc::CmdEllipseMove(CView *pSingleView)
{
	CView *pView = NULL;
    CView * pMainView = NULL;
	POSITION pos = GetFirstViewPosition();
    CBitmap * bp = NULL;
	while(NULL != pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSingleSliceView)))
		{
            bp = ((CSingleSliceView *)pView)->getScaledBitmap();
            pSingleView = pView;
			continue;
		}

		if(pView->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerView)))
		{
            pMainView = pView;
			continue;
		}
	}

    POINT   pt[3];

	CSlice* pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->GetEllipsePoints(pt);
    CEllipseTracker    tracker;
    tracker.Track(pSingleView, bp, pt);
	tracker.GetResult(pt);
	
	pSlice->SetEllipseManual(true);
	pSlice->SetEllipseCenter(pt);
	
	pSlice->CalcParameters(TRUE);	

	UpdateAllViews(pMainView);
}

void CTomatoAnalyzerDoc::OnCmdTipAreaAdjust()
{
	m_menuSelected = ID_REVISE_ADJUSTTIPAREA;
	CView *pView = NULL;
    CView * pSingleView = NULL;
    CView * pMainView = NULL;
	POSITION pos = GetFirstViewPosition();
    CBitmap * bp = NULL;
	while(NULL != pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSingleSliceView)))
		{
            bp = ((CSingleSliceView *)pView)->getScaledBitmap();
            pSingleView = pView;
			continue;
		}

		if(pView->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerView)))
		{
            pMainView = pView;
			continue;
		}
	}

    POINT   pt[2];

	CSlice* pSlice = m_SliceList.GetAt(m_posSelectedObject);
	CPixelList * pBoundary = pSlice->GetBoundary(1);
	pSlice->GetTipAreaPoints(pt);	//Get the current Tip Area points

	//Move them  
	CTipAreaTracker2  *  tracker = new CTipAreaTracker2(pBoundary);
    tracker->Track(pSingleView, bp, pt);
	tracker->GetResult(pt);
	
	//Recalculate tip area
	pSlice->SetTipAreaPoints(pt);
	
	UpdateAllViews(pMainView);
	
}

//Moves the tip area points back to their default positions
void CTomatoAnalyzerDoc::OnCmdResetTipArea()
{
	m_menuSelected = ID_REVISE_RESETTIPAREA;
	CSlice* pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->ResetTipArea(); 
	UpdateAllViews(NULL);
}

void CTomatoAnalyzerDoc::OnCmdEliminateTipArea()
{
	m_menuSelected = ID_ELIMINATE;
	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->EliminateTipArea();
	UpdateAllViews(NULL);
}

void CTomatoAnalyzerDoc::OnCmdAngleAdjust(UINT nID)
{
	CView *pView = NULL;
    CView * pSingleView = NULL;
    CView * pMainView = NULL;
	POSITION pos = GetFirstViewPosition();
    CBitmap * bp = NULL;
	while(NULL != pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSingleSliceView)))
		{
            bp = ((CSingleSliceView *)pView)->getScaledBitmap();
            pSingleView = pView;
			continue;
		}

		if(pView->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerView)))
		{
            pMainView = pView;
			continue;
		}
	}

    POINT   pt[3];
    int nAngleIndex = nID - ID_MANUAL_ANGLE_FIRST;
	CSlice* pSlice = m_SliceList.GetAt(m_posSelectedObject);
    pSlice->GetAnglePosition(pt, nAngleIndex);

    CTriangleTracker    tracker;
    if(tracker.Track(pSingleView, bp, pt))
    {
        tracker.GetResult(pt);
        pSlice->SetAnglePosition(nAngleIndex, pt);
		m_menuSelected = (nAngleIndex == 0) ? ID_PROXIMAL_POINT : ID_DISTAL_POINT; 
	}

	pSlice->CalcParameters(TRUE);

	UpdateAllViews(pMainView);
	return;
}

void CTomatoAnalyzerDoc::OnCmdResetPericarp()
{
	m_menuSelected = ID_RESETINNER;

	CSlice * pSlice = GetSelectedSlice();
	int periSize = pSlice->GetPericarpDefault();
	pSlice->SetPericarpDefault(periSize);

	UpdateAllViews(NULL);
}

void CTomatoAnalyzerDoc::OnCmdSetPericarpSize()
{
	m_menuSelected = ID_PERICARPSIZE;
	
	carpSize dlg;

	CSlice * pSlice = GetSelectedSlice();

	//Set slider to current value, if value has been set.
	int periSize = pSlice->GetPericarpDefault();
	if (periSize >= 25 && periSize <= 100)
	{
		dlg.Ellipse_Value = periSize;
	}	
	
	if (dlg.DoModal() == TRUE)
	{
		if (dlg.clickSave)
		{
			int pericarpSize = dlg.Ellipse_Value;

			pSlice->SetPericarpDefault(pericarpSize);
			UpdateAllViews(NULL);
		}
	}
}

void CTomatoAnalyzerDoc::OnCmdAdjustPericarpBoundary()
{
	m_menuSelected = ID_INNER_BOUNDARY;

	POINT pt;
	CView *pView = NULL;
	CBitmap * bp = NULL;
	POSITION pos = GetFirstViewPosition();
	while (NULL != pos)
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CSingleSliceView)))
		{
			bp = ((CSingleSliceView *)pView)->getScaledBitmap();
			pt = ((CSingleSliceView *)pView)->GetOffsetPoint();
			break;
		}

		pView = NULL;
	}

	if (NULL == pView || bp == NULL)
		return;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	CPixelList * pBoundary = pSlice->GetInnerBoundary();

	int left = 1000000;
	int top = 1000000;

	for(unsigned int i = 0; i < pBoundary->size(); i++ )
	{
		if( pBoundary->GetAt(i).x < left )
			left = pBoundary->GetAt(i).x;
		if( pBoundary->GetAt(i).y < top )
			top = pBoundary->GetAt(i).y;
	}

	PeriTracker st(pBoundary);
	if(st.Track(pView, NULL, bp))  // true if pericarp boundary has been modified
	{
        pSlice->SetNewInnerBoundary(st.GetShape(), left, top);
	}
	UpdateAllViews(NULL);
	return;
}

void CTomatoAnalyzerDoc::OnCmdShapeAdjust()
{
	m_menuSelected = ID_MANUAL_SHAPE;
	if(m_bRevise!=ID_MANUAL_SHAPE && m_bRevise!=0)
		return;

	POINT pt;
	CView *pView = NULL;
    CBitmap * bp = NULL;
	POSITION pos = GetFirstViewPosition();
	while(NULL != pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSingleSliceView)))
		{
            bp = ((CSingleSliceView *)pView)->getScaledBitmap();
			pt = ((CSingleSliceView *)pView)->GetOffsetPoint();
			break;
		}

		pView = NULL;
	}

	if(NULL == pView || bp==NULL)
		return;

	CSlice* pSlice = m_SliceList.GetAt(m_posSelectedObject);
	CPixelList * pBoundary = pSlice->GetBoundary(1);

	CShapeTracker st(pBoundary);
	if(st.Track(pView, pt, NULL, bp))
	{
        pSlice->SetNewBoundary(st.GetShape());
		pSlice->CalcParameters(FALSE);
	}

	UpdateAllViews(NULL);
}

void CTomatoAnalyzerDoc::OnCmdFlatProximalMicro()
{
	m_menuSelected = ID_FLAT_PROXIMAL_MICRO;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->EliminateAngle(true, true);

	pSlice->CalcParameters(TRUE);

	UpdateAllViews(NULL);

	return;
}

void CTomatoAnalyzerDoc::OnCmdFlatProximalMacro()
{
	m_menuSelected = ID_FLAT_PROXIMAL_MACRO;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->EliminateAngle(true, false);

	pSlice->CalcParameters(TRUE);

	UpdateAllViews(NULL);

	return;
}

void CTomatoAnalyzerDoc::OnCmdFlatDistalMicro()
{
	m_menuSelected = ID_FLAT_DISTAL_MICRO;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->EliminateAngle(false, true);
	
	pSlice->CalcParameters(TRUE);

	UpdateAllViews(NULL);
	return;
}

void CTomatoAnalyzerDoc::OnCmdFlatDistalMacro()
{
	m_menuSelected = ID_FLAT_DISTAL_MACRO;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->EliminateAngle(false, false);
	
	pSlice->CalcParameters(TRUE);

	UpdateAllViews(NULL);
	return;
}

void CTomatoAnalyzerDoc::OnCmdResetProximalMicro()
{
	m_menuSelected = ID_RESET_PROXIMAL_MICRO;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->ResetFlatAngle(true, true);

	pSlice->CalcParameters(TRUE);

	UpdateAllViews(NULL);

	return;
}

void CTomatoAnalyzerDoc::OnCmdResetProximalMacro()
{
	m_menuSelected = ID_RESET_PROXIMAL_MACRO;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->ResetFlatAngle(true, false);

	pSlice->CalcParameters(TRUE);

	UpdateAllViews(NULL);

	return;
}

void CTomatoAnalyzerDoc::OnCmdResetDistalMicro()
{
	m_menuSelected = ID_RESET_DISTAL_MICRO;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->ResetFlatAngle(false, true);

	pSlice->CalcParameters(TRUE);

	UpdateAllViews(NULL);
	return;
}

void CTomatoAnalyzerDoc::OnCmdResetDistalMacro()
{
	m_menuSelected = ID_RESET_DISTAL_MACRO;

	CSlice * pSlice = m_SliceList.GetAt(m_posSelectedObject);
	pSlice->ResetFlatAngle(false, false);
	
	pSlice->CalcParameters(TRUE);

	UpdateAllViews(NULL);
	return;
}

void CTomatoAnalyzerDoc::OnCmdPepperPericarp()
{
	m_menuSelected = ID_PEPPER_PERICARP;

	if(m_bRevise!=ID_PEPPER_PERICARP && m_bRevise!=0)
		return;
	
	POINT pt;
	CView *pView = NULL;
	CBitmap * bp = NULL;
	POSITION pos = GetFirstViewPosition();
	while (NULL != pos)
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CSingleSliceView)))
		{
			bp = ((CSingleSliceView *)pView)->getScaledBitmap();
			pt = ((CSingleSliceView *)pView)->GetOffsetPoint();
			break;
		}

		pView = NULL;
	}

	if (NULL == pView || bp == NULL)
		return;

	CSlice* pSlice = m_SliceList.GetAt(m_posSelectedObject);
	CPixelList * pBoundary = pSlice->GetPericarpBoundary(1);

	PeriTracker st(pBoundary);
	if(st.Track(pView, NULL, bp))
	{
        pSlice->SetNewPericarpBoundary(st.GetShape());
	}
	UpdateAllViews(NULL);
}

void CTomatoAnalyzerDoc::OnCmdResetPepperPericarp()
{
	m_menuSelected = ID_RESET_PEPPER_PERICARP;
	
	CSlice * pSlice = GetSelectedSlice();
	pSlice->ResetPericarpBoundary();

	UpdateAllViews(NULL);
}

BOOL CTomatoAnalyzerDoc::IsAnalyzed()
{
	return m_bAnalyzed;
}

void CTomatoAnalyzerDoc::OnAnalyzeImage()
{
	/* Do the following if the image has already been analyzed */
	if(m_bAnalyzed)
	{
		POSITION pos = m_SliceList.GetHeadPosition();
		while (pos != NULL)
			delete m_SliceList.GetNext(pos);

		m_SliceList.RemoveAll();

		m_bAnalyzed = FALSE;
        m_archivedData = FALSE;
		m_posSelectedObject=NULL;
	}

	if(!m_pImage)
		return;

	/* create a progress view, so the user can see the progress */
	CWaitCursor waitcursor;
	CProgressDialog * myDialog = new CProgressDialog();
	myDialog->Create(IDD_PROGRESS_DIALOG, NULL);
	myDialog->ShowWindow(SW_SHOWNORMAL);

    if(m_archivedData==false)
    {
		int nDesired = ((CTomatoAnalyzerApp*)AfxGetApp())->m_nBrightLevel;
    	CBinObjs * myObjs=GetSlicesFromImage(NULL, m_pImage, nDesired, m_nThresholdValue, myDialog, 70);

        if(myObjs!=NULL)
        {
		    int	iIndex=0;
		    CBONode * head = myObjs->GetHead();
		    while(head != NULL)
		    {
                CSlice* pSlice;

    	        pSlice = new CSlice();
		        pSlice->Set(head->mPixList);
		        pSlice->SetIndex(iIndex++);
		        m_SliceList.AddTail(pSlice);

			    head = head->mNext;
		    }
		    delete myObjs;
        }
	}

	if(m_SliceList.GetHeadPosition()== 0)
	{
		myDialog->DestroyWindow();
		delete myDialog;
		return;
	}

	//calculate slice parameters
    int nSliceNumber = m_SliceList.GetCount();
	double dStep = (nSliceNumber > 0) ? 20.0/nSliceNumber : 0; 
    double dProgress = 80.0;
    int iSeq = 1;
    char szBuffer[256];
	for (POSITION pos = m_SliceList.GetHeadPosition(); pos != NULL; iSeq++, dProgress += dStep)
	{
        wsprintf(szBuffer, "Calculating parameters... Object %d", iSeq);
    	myDialog->Update(round(dProgress), szBuffer);
		CSlice* pSlice = m_SliceList.GetNext(pos);
		pSlice->CalcParameters(TRUE);
	}

	ExcludeUnusualObject();

	m_bAnalyzed = TRUE;
	myDialog->DestroyWindow();
	delete myDialog;

	SetSelectedDrawIndex(0);
	this->UpdateAllViews(NULL, DOC_UPDATEVIEW_MEASURECHANGED);

}

/**
 * Sets the slice with the given index (in natural sequence order) to be the selected one for display. 
 */
void CTomatoAnalyzerDoc::ShowTiles(int index)
{
	int slices = ((CTomatoAnalyzerApp*)AfxGetApp())->GetSelectedSlices();
	if( index < slices)
	{
	         std::vector<POSITION>   vctPos;
            GetSelectedSliceByNaturalSequence(vctPos);
			SetSelectedObject(vctPos[index]);
			CTomatoAnalyzerView* view = GetCurrentAnalyzerView(); 
			view->InvalidateRect(NULL, TRUE);
			UpdateAllViews(view, DOC_UPDATEVIEW_FRUITSWITCH);
	}
}

// Shows the slice that the user is to be entering data for during Add Data
void CTomatoAnalyzerDoc::ShowObjectAddData(int index)
{
	// Gets the positions of all slices
	std::vector<POSITION> slicePositions; 
	GetAllSlicesByNaturalSequence(slicePositions);
	// Sets the slice to appear in the zoomed in area on the top right of the window
	SetSelectedObject(slicePositions[index]);
	CTomatoAnalyzerView* view = GetCurrentAnalyzerView(); 
	view->InvalidateRect(NULL, TRUE);
	UpdateAllViews(view, DOC_UPDATEVIEW_FRUITSWITCH);
}

void CTomatoAnalyzerDoc::OnBlockySettingChange()
{
	POSITION pos = m_SliceList.GetHeadPosition();
    if(NULL == pos)
        return;

	while (pos != NULL)
	{
		CSlice* pSlice = m_SliceList.GetNext(pos);
		pSlice->metric_blocky();
	}

    UpdateAllViews(NULL);
	return;
}

CString CTomatoAnalyzerDoc::stripFileExt(CString file) {
	int dotPos = file.ReverseFind('.'); 
	if (dotPos >= 0) {
		return file.Left(dotPos); 
	} else {
		return file; 
	}
}

/**
 * Performs color calibration, treating the current image as a color checker and each slice as a tile, 
 * and using the given illuminant and observer angle. 
 * Passes back the resulting equation in the given variable references (whose initial values are ignored). 
 * Returns false if the user cancels the calibration. 
 */ 
bool CTomatoAnalyzerDoc::CalibrateColor(double &lSlope, double &lIntercept, double &aSlope, double &aIntercept, 
										double &bSlope, double &bIntercept, IlluminantObserver illumObs) {
	if (! m_bAnalyzed) {
		OnAnalyzeImage(); 
	}

	std::vector<POSITION> slicePos;
	GetSelectedSliceByNaturalSequence(slicePos);

	// Actual values for L*, a*, b* calculated from the image
	double *lActual = new double[slicePos.size()], 
		*aActual = new double[slicePos.size()], 
		*bActual = new double[slicePos.size()]; 
	for (int i = 0; i < slicePos.size(); i++) 
	{
		CSlice * slice = GetSlice(slicePos[i]); 
		slice->CalcParametersColor(1, 0, 1, 0, 1, 0, illumObs); 
		lActual[i] = slice->getAvgL(); 
		aActual[i] = slice->getAvgA(); 
		bActual[i] = slice->getAvgB(); 
	}

	LABDialog labDlg; 
	if (labDlg.DoModal() == IDOK) {

		// Expected values for L*, a*, b* specified by the user
		double *lExpected = new double[slicePos.size()], 
			*aExpected = new double[slicePos.size()], 
			*bExpected = new double[slicePos.size()]; 
		for (int i = 0; i < slicePos.size(); i++) {
			CSlice * slice = GetSlice(slicePos[i]); 
			lExpected[i] = slice->getExpectedL(); 
			aExpected[i] = slice->getExpectedA(); 
			bExpected[i] = slice->getExpectedB(); 
		}

		// Calculate equations.
		// This assumes that there are >= 2 tiles in the color checker; otherwise the lines come out vertical. 
		LinearRegression reg; 
		reg.Reset(); 
		reg.addXY(lActual, lExpected, slicePos.size());	
		reg.Calculate(); 
		lSlope = reg.getA();
		lIntercept = reg.getB();
		reg.Reset(); 
		reg.addXY(aActual, aExpected, slicePos.size());	
		reg.Calculate(); 
		aSlope = reg.getA();
		aIntercept = reg.getB();
		reg.Reset(); 
		reg.addXY(bActual, bExpected, slicePos.size());	
		reg.Calculate(); 
		bSlope = reg.getA();
		bIntercept = reg.getB();

		// Optionally, output to CSV file
		CString defaultFile; 
		defaultFile.Format("%s-calibration.csv", stripFileExt(m_ImageFileName)); 
		CFileDialog fsave(FALSE, "csv", defaultFile, OFN_EXPLORER|OFN_OVERWRITEPROMPT, _T("CSV file (*.csv)|*.csv||"));
		if (fsave.DoModal() == IDOK)
		{
			std::fstream fResult;
			fResult.open(fsave.GetPathName(), ios_base::out|ios_base::trunc);
			if (!fResult.is_open())
			{
				AfxMessageBox("Cannot open the specified file to save results", MB_ICONEXCLAMATION);
			}
			else
			{
				fResult << "Plot No.,Fruit No.,Actual L,Actual A,Actual B,Expected L,Expected A,Expected B" << endl; 

				char plotName[1024];
				ExtractFilaNameOnly(m_ImageFileName, plotName);

				for (int i = 0; i < slicePos.size(); i++) {
					CSlice * slice = GetSlice(slicePos[i]); 
					fResult << plotName << "," << (i + 1) << "," << 
						lActual[i] << "," << aActual[i] << "," << bActual[i] << "," << 	
						lExpected[i] << "," << aExpected[i] << "," << bExpected[i] << "," << endl; 
				}

				fResult << endl; 
				fResult << ",Slope,Y-intercept" << endl; 
				fResult << "Expected L vs Actual L," << lSlope << "," << lIntercept << endl; 
				fResult << "Expected A vs Actual A," << aSlope << "," << aIntercept << endl; 
				fResult << "Expected B vs Actual B," << bSlope << "," << bIntercept << endl; 
				fResult.close(); 
			}
		} 

		delete [] lExpected;  delete [] aExpected;  delete [] bExpected; 
		delete [] lActual;  delete [] aActual;  delete [] bActual; 
		return true; 
	}

    delete [] lActual;  delete [] aActual;  delete [] bActual; 
	return false; 
}

/**
 * Performs color analysis for all slices using the given calibration equations and illuminant / observer angle. 
 */
void CTomatoAnalyzerDoc::AnalyzeColor(double lSlope, double lIntercept, double aSlope, double aIntercept, 
									  double bSlope, double bIntercept, IlluminantObserver illumObs, 
									  std::vector<ColorRange> lRanges, std::vector<ColorRange> hueRanges, 
									  std::vector<ColorRange> chromaRanges, std::vector<ColorRange> userRanges) {
	if (! m_bAnalyzed) {
		OnAnalyzeImage(); 
	}
	
	std::vector<POSITION> slicePos;
	GetAllSlicesByNaturalSequence(slicePos);
	for (std::vector<POSITION>::iterator i = slicePos.begin(); i != slicePos.end(); i++) 
	{
		CSlice * slice = GetSlice(*i); 
		slice->CalcParametersColor(lSlope, lIntercept, aSlope, aIntercept, bSlope, bIntercept, illumObs, 
			lRanges, hueRanges, chromaRanges, userRanges); 
	}
}

/**
 * Analyzes an image to locate the slices, find their boundaries, and delete slices that are overly large or small. 
 * 
 * nDesired is now ignored. 
 */ 
CBinObjs * CTomatoAnalyzerDoc::GetSlicesFromImage(CAdvImage ** outImg,  CAdvImage* pImage, int nDesired, int& nNew, CProgressDialog * inProgDialog, int inMaxProg)
{
	BOOL		bKeepOutputImage;
	CAdvImage * pResultedImage;
	CBinObjs  * myObjs; 

	if(!pImage)
		return NULL;

	bKeepOutputImage = (NULL == outImg)? FALSE : TRUE;

	if(inProgDialog)
		inProgDialog->Update(1, "Identifying objects");
	pResultedImage = pImage->ThresholdLocally(1, 1, 0, 1.0, nDesired, nNew, blueAmt);  // nDesired, blueAmt are ignored

	if(inProgDialog)
		inProgDialog->Update(inMaxProg * 0.2, "Smoothing image");
	pResultedImage->MedianFilter(3);
	// If you want to smooth jagged parts of the boundaries even more, consider increasing the 
	// MedianFilter argument and/or calling MedianFilter before thresholding (i.e., on the color
	// image instead of the black and white image). 

	if(inProgDialog)
		inProgDialog->Update(inMaxProg * 0.4,"Drawing boundaries");
	myObjs = pResultedImage->GetBinaryObjects();

	if(inProgDialog)
		inProgDialog->Update(inMaxProg * 0.6,"Deleting small and large objects");
	myObjs->FilterBinaryObjects(5000, 1000000, pResultedImage);

	if(inProgDialog)
		inProgDialog->Update(inMaxProg);

	if(!bKeepOutputImage)
		delete pResultedImage;
	else
		*outImg = pResultedImage;

	return myObjs;
}


POSITION CTomatoAnalyzerDoc::DecideClickedObject(CPoint point)
{
	POSITION pos = m_SliceList.GetHeadPosition();
	while (pos != NULL)
	{
		POSITION BakPos= pos;
		CSlice* pSlice = m_SliceList.GetNext(pos);

		if(pSlice->mBBox.PointIn(point))
			return BakPos;
	}

	return NULL;
}

POSITION CTomatoAnalyzerDoc::GetSelectedObject()
{
	return m_posSelectedObject;
}


CSlice * CTomatoAnalyzerDoc::GetSlice(POSITION pos)
{
    if(NULL != pos)
        return m_SliceList.GetAt(pos);

    return NULL;
}

POSITION CTomatoAnalyzerDoc::GetSliceHeadPosition()
{
    return m_SliceList.GetHeadPosition();
}

CSlice * CTomatoAnalyzerDoc::GetNextSlice(POSITION& pos)
{
    return m_SliceList.GetNext(pos);
}

CSlice * CTomatoAnalyzerDoc::GetSelectedSlice()
{
    POSITION pos = GetSelectedObject();
    return GetSlice(pos);
}

VOID CTomatoAnalyzerDoc::SetSelectedObject(POSITION pos)
{
	m_posSelectedObject = pos;

	if(m_bRevise)
		m_bRevise=FALSE;
}

CSlice * CTomatoAnalyzerDoc::GetAnalyzeResult(POSITION pos)
{
	if(NULL == pos)
		pos = m_posSelectedObject;

	if(NULL == pos)
		return NULL;

	CSlice* pSlice = m_SliceList.GetAt(pos);
	return pSlice;
}


int CTomatoAnalyzerDoc::GetSelectedDrawIndex()
{
	return m_iSelectedDrawIndex; 
}


void CTomatoAnalyzerDoc::SetSelectedDrawIndex(int iIndex)
{
	if(m_iSelectedDrawIndex!=iIndex)
	{
		m_iSelectedDrawIndex=iIndex;

		POSITION pos = GetFirstViewPosition();
		while(NULL != pos)
		{
			CView * pView=GetNextView(pos);
			if(!pView->IsKindOf(RUNTIME_CLASS(CFruitDataView)))
			{
				pView->InvalidateRect(NULL, TRUE);
				pView->UpdateWindow();
			}
		}
	}
	return;
}


void CTomatoAnalyzerDoc::ExcludeUnusualObject()
{
	int nMedianWidth, nMedianHeight, nWidth, nHeight;
	CSlice* pSlice;

	std::vector<int> w, h;
	POSITION pos = m_SliceList.GetHeadPosition();
	while(pos!=NULL)
	{
		pSlice = m_SliceList.GetNext(pos);
		pSlice->GetWidthAndHeightInPixel(nWidth, nHeight);
		
		w.push_back(nWidth);
		h.push_back(nHeight);
	}

	nMedianWidth = median(w);
	nMedianHeight = median(h);

	int MaxWidth = floor(float(nMedianWidth)*2.0);
	int MinWidth = floor(float(nMedianWidth)/2.0);
	int MaxHeight = floor(float(nMedianHeight)*2.0);
	int MinHeight = floor(float(nMedianHeight)/2.0);

	pos = m_SliceList.GetHeadPosition();
	while(pos!=NULL)
	{
		pSlice = m_SliceList.GetNext(pos);
		pSlice->GetWidthAndHeightInPixel(nWidth, nHeight);

		if(nWidth>=MaxWidth || nWidth<=MinWidth || nHeight>=MaxHeight || nHeight<=MinHeight)
		{
			pSlice->SetExcludeFlag();
		}
	}
	return;
}


using namespace std;


void CTomatoAnalyzerDoc::OnCmdSaveResults()
{
	CFileDialog fdlg(FALSE, 
		             "csv", 
				     NULL, 
				     OFN_EXPLORER |OFN_OVERWRITEPROMPT,
					 _T("CSV file (*.csv)|*.csv||")
				   );

	if(fdlg.DoModal()!=IDOK)
        return;
	else
	{
		CString strName;
		strName.Format ("%s",fdlg.GetPathName());
		std::fstream f(strName, ios_base::out|ios_base::trunc);

		if(!f.is_open())
			return;

		f << "slice No., ";
		m_SliceList.GetHead()->ExportHeader(f);
		f << endl;

		SaveAnalyzedMeasureItems(f);

		f << "mean, ";
		SaveMeanValueOfAnalyzedResult(f, true);
		f << endl;
				
		f.flush();
		f.close();
	}
    return;
}

CSlice * CTomatoAnalyzerDoc::GetSliceBySequenceIndex(int iSeq)
{
    CSlice *  pSlice = NULL;

    POSITION pos = m_SliceList.FindIndex(iSeq);
    if(NULL != pos)
        pSlice = m_SliceList.GetAt(pos);

    return pSlice;
}

void CTomatoAnalyzerDoc::SaveAnalyzedMeasureItems(std::fstream& f)
{
    std::vector<POSITION>   PosSlice;
    GetSelectedSliceByNaturalSequence(PosSlice);

    int iIndex = 0;

    std::vector<POSITION>::iterator  bg, ed;
    bg = PosSlice.begin();
    ed = PosSlice.end();
    while(bg != ed)
    {
        CSlice * pSlice = GetSlice(*bg);
        {
			++iIndex;
			f << iIndex << ", ";

			pSlice->ExportValue(f);
			f << endl;
		}

		++bg;
	}
    return;
}

//Find Standard Deviation to output in batch mode
void CTomatoAnalyzerDoc::SaveStdDevResult(std::fstream& f, bool printStd)
{
	// vector of the position of each individual slice
	std::vector<POSITION>   PosSlice;
    GetSelectedSliceByNaturalSequence(PosSlice);

    // vector of the values stored
	std::vector<double> dblValueVct;

    int nSliceNumber = 0;

    std::vector<POSITION>::iterator  bg, ed;
    bg = PosSlice.begin();
    ed = PosSlice.end();
    // while not at the last tomato
	while(bg != ed)
    {
		// create a slice object for that slice
		CSlice* pSlice = m_SliceList.GetAt(*bg);
		{
			// advance through the vector
			++nSliceNumber;
			pSlice->ExportValue(dblValueVct);
		}

		++bg;
    }

	// if no slices being analyzed
	if(nSliceNumber==0)
		return;

    // MeasureItemNumber is the size of the value vector divided by the number of slices
	int  nMeasureItemNumber = dblValueVct.size()/nSliceNumber;
    // If the above division didn't work then you have an error in saving the values
	if(nMeasureItemNumber*nSliceNumber != dblValueVct.size())
    {
        AfxMessageBox("error in saving std deviation values");
        return;
    }
    
    int nSliceTestedNum;
    double tmp, sum, sqsum;
	// stdValue is an array that has size of the number of values divided by the number of slices
    double * stdValue = new double[nMeasureItemNumber];
    for(int i=0; i<nMeasureItemNumber; i++)
    {
        nSliceTestedNum = 0;
        sum = sqsum = 0.0;
		// for as many slices as you are analyzing
        for(int j=0; j<nSliceNumber; j++)
        {

			tmp = dblValueVct.at(j*nMeasureItemNumber+i);
            if(tmp >= 0.0)
            {
                sum += tmp;
                nSliceTestedNum += 1;
            }
        }

        if(nSliceTestedNum > 0)
            tmp = (sum/nSliceTestedNum);
        else
            tmp = 0.0;

		double avg_temp = tmp;
		double deviation = 0.0;
		nSliceTestedNum = 0;
		for (int k = 0; k<nSliceNumber; k++)
		{
			tmp = dblValueVct.at(k*nMeasureItemNumber+i);
			if (tmp >= 0.0)
			{
				deviation += pow(tmp - avg_temp, 2);
			}
		}

		deviation = sqrt(deviation / nSliceNumber);
	
		f<<setiosflags(ios::fixed)<<setprecision(4)<<deviation<<", ";

        if(nSliceTestedNum > 1)
            stdValue[i]=sqrt((sqsum - double(nSliceTestedNum)*pow(tmp, 2))/(nSliceTestedNum-1));
        else
        {
            stdValue[i]=0.0;
        }
    }
    
    delete stdValue;
    return;
}


void CTomatoAnalyzerDoc::SaveIndividualSlices(std::fstream& f, char * szFileName)
{
    // vector of the position of each individual slice
	std::vector<POSITION>   PosSlice;
    GetSelectedSliceByNaturalSequence(PosSlice);

    // vector of the values stored
	std::vector<double> dblValueVct;

    int nSliceNumber = 0;

    std::vector<POSITION>::iterator  bg, ed;
    bg = PosSlice.begin();
    ed = PosSlice.end();
	int count = 0;
    // while not at the last tomato
	while(bg != ed)
    {
		// create a slice object for that slice
		CSlice* pSlice = m_SliceList.GetAt(*bg);
		{
			// advance through the vector
			++nSliceNumber;
			pSlice->ExportValue(dblValueVct);
		}
		++bg;
    }

	int nMeasureItemNumber = dblValueVct.size()/nSliceNumber;
	for (int i = 0; i < nSliceNumber; i++)
	{
		f<<szFileName <<" - "<<(i + 1)<<", ";
		for (int j = 0; j < nMeasureItemNumber; j++)
		{
			f<<setiosflags(ios::fixed)<<setprecision(4)<<dblValueVct[j + (i * nMeasureItemNumber)]<<", ";
		}
		f<<setiosflags(ios::fixed)<<setprecision(4)<<endl;
	}

    return;
}


void CTomatoAnalyzerDoc::SaveMeanValueOfAnalyzedResult(std::fstream& f, bool printStd)
{
    // vector of the position of each individual slice
	std::vector<POSITION>   PosSlice;
    GetSelectedSliceByNaturalSequence(PosSlice);

    // vector of the values stored
	std::vector<double> dblValueVct;

    int nSliceNumber = 0;

    std::vector<POSITION>::iterator  bg, ed;
    bg = PosSlice.begin();
    ed = PosSlice.end();
    // while not at the last tomato
	while(bg != ed)
    {
		// create a slice object for that slice
		CSlice* pSlice = m_SliceList.GetAt(*bg);
		{
			// advance through the vector
			++nSliceNumber;
			pSlice->ExportValue(dblValueVct);
		}

		++bg;
    }

	// if no slices being analyzed
	if(nSliceNumber==0)
		return;

    // MeasureItemNumber is the size of the value vector divided by the number of slices
	int  nMeasureItemNumber = dblValueVct.size()/nSliceNumber;
    // If the above division didn't work then you have an error in saving the values
	if(nMeasureItemNumber*nSliceNumber != dblValueVct.size())
    {
        AfxMessageBox("error in saving mean values");
        return;
    }
    
    int nSliceTestedNum;
    double tmp, sum, sqsum;
	// stdValue is an array that has size of the number of values divided by the number of slices
    double * stdValue = new double[nMeasureItemNumber];
    for(int i=0; i<nMeasureItemNumber; i++)
    {
        nSliceTestedNum = 0;
        sum = sqsum = 0.0;
		// for as many slices as you are analyzing
        for(int j=0; j<nSliceNumber; j++)
        {

			tmp = dblValueVct.at(j*nMeasureItemNumber+i);
            //if(tmp >= 0.0) //commented by Hale
            //{
                sum += tmp;
                sqsum += tmp*tmp;
                nSliceTestedNum += 1;
            //}
        }

        if(nSliceTestedNum != 0) //if(nSliceTestedNum > 0)
            tmp = (sum/nSliceTestedNum);
        else
            tmp = 0.0;
        
        f<<setiosflags(ios::fixed)<<setprecision(4)<<tmp<<", ";
		//SaveStdDevResult(f, false);

        if(nSliceTestedNum > 1)
            stdValue[i]=sqrt((sqsum - double(nSliceTestedNum)*pow(tmp, 2))/(nSliceTestedNum-1));
        else
        {
            stdValue[i]=0.0;
        }
    }

    if(printStd)
    {
        f<<endl<<"Std. Dev., ";
        for(int i=0; i<nMeasureItemNumber; i++)
            f<<setiosflags(ios::fixed)<<setprecision(4)<<stdValue[i]<<", ";
    }
    
    delete stdValue;
    return;
}


void CTomatoAnalyzerDoc::decideSliceSequence(std::vector<int>& vct)
{
    int iIndex = 0;
    std::vector<int>  row, seqIndex;

    vct.clear();
    
    int nCount = m_SliceList.GetCount();
    if(nCount<=0)
        return;

    if(nCount==1)
    {
        vct.push_back(0);
        return;
    }

	POSITION pos = m_SliceList.GetHeadPosition();
	if(pos != NULL)
    {
        CSlice* pSlice = m_SliceList.GetNext(pos);

	    int xc = (pSlice->mBBox.mUpperLeft.x + pSlice->mBBox.mLowerRight.x)/2;
	    int ymin = pSlice->mBBox.mUpperLeft.y;
        int ymax = pSlice->mBBox.mLowerRight.y;
        row.push_back(xc);

        while(pos != NULL)
        {
            POSITION posOld = pos;
            CSlice* pNextArray = m_SliceList.GetNext(pos);

    	    int xtmp = (pNextArray->mBBox.mUpperLeft.x + pNextArray->mBBox.mLowerRight.x)/2;
    	    int ytmp = (pNextArray->mBBox.mUpperLeft.y + pNextArray->mBBox.mLowerRight.y)/2;
            
            if(ytmp<=ymax && ytmp>=ymin)
                row.push_back(xtmp);
            else
                pos = posOld;

            if(pos == NULL || ytmp>ymax || ytmp<ymin)
            {
                int nCount = row.size();
                seqIndex.reserve(nCount);
                seqIndex.clear();
                for(int i=0; i<nCount; i++)
                {
                    seqIndex.push_back(iIndex);
                    iIndex++;
                }
                sortvalue(row, seqIndex);
                vct.insert(vct.end(), seqIndex.begin(), seqIndex.end());
                

                xc = xtmp;
        	    ymin = pNextArray->mBBox.mUpperLeft.y;
                ymax = pNextArray->mBBox.mLowerRight.y;

                row.clear();
                row.push_back(xc);
            }
        }
    }
    return;
}

// For some reason the objects in m_SliceList are not in complete order.
// This method gives vct indexes in the proper order, so that they can be accessed in order.
// This is very similar to the method decideSliceSequence( std::vectore<int>& vct ),
// in fact, it only has a few lines that are different.
// There is just one small problem with this method.  If there is a label at the bottom
// of the image and it is unflagged (marked to be analyzed) its measurements are displayed
// between the middle two fruits of the row above it.
void CTomatoAnalyzerDoc::putSlicesInOrder( std::vector<int>& vct )
{
	int iIndex = 0;
    std::vector<int>  row, seqIndex;

    vct.clear();
    
    int nCount = m_SliceList.GetCount();
    if(nCount<=0)		// There are no slices in the list
        return;

    if(nCount==1)		// Only one slice
    {
        vct.push_back(0);
        return;
    }

	POSITION pos = m_SliceList.GetHeadPosition();
	if(pos != NULL)
    {
        CSlice* pSlice = m_SliceList.GetNext(pos);

	    int xc = (pSlice->mBBox.mUpperLeft.x + pSlice->mBBox.mLowerRight.x)/2;  // Find the x-center of the object
	    int ymin = pSlice->mBBox.mUpperLeft.y;		// find top y-value
        int ymax = pSlice->mBBox.mLowerRight.y;		// find bottom y-value
        row.push_back(xc);			// add the center to the row vector

        while(pos != NULL)
        {
            POSITION posOld = pos;
            CSlice* pNextArray = m_SliceList.GetNext(pos);
			
			// Find mid x- and y-values
    	    int xtmp = (pNextArray->mBBox.mUpperLeft.x + pNextArray->mBBox.mLowerRight.x)/2;
    	    int ytmp = (pNextArray->mBBox.mUpperLeft.y + pNextArray->mBBox.mLowerRight.y)/2;
            
            if(ytmp<=ymax && ytmp>=ymin)	// The mid y-value is in the same row as the last slice
                row.push_back(xtmp);		//push its mid x-value to the back of the row vector

			if( pos == NULL || ytmp>ymax || ytmp<ymin )		// The fruit is not in the same row
            {
				if( pos == NULL )		// avoids a pesky off-by-one error
					row.push_back( xtmp );

				// The following code creates a vector that stores, basically, index values into the vector
                int nCount = row.size();
                seqIndex.reserve(nCount);
                seqIndex.clear();
                for(int i=0; i<nCount; i++)
                {
                    seqIndex.push_back(iIndex);
                    iIndex++;
                }
				// sortvalue sorts row from smallest to largest, it puts each row in order.
				// Whenever it makes a change to row, it makes the same change to seqIndex.
				// This means that the index values in seqIndex stay corresponding to the same slices
                sortvalue(row, seqIndex);
                vct.insert(vct.end(), seqIndex.begin(), seqIndex.end());	// insert the index values to vct
                

                xc = xtmp;		// get new x mid
        	    ymin = pNextArray->mBBox.mUpperLeft.y;	// Find new top y-value
                ymax = pNextArray->mBBox.mLowerRight.y;	// find new bottom y-value

                row.clear();	// We are ready to start on the next row, so clear the row vector
                row.push_back(xc);	// push the first element of the ext row on the vector
            }
        }
    }
}

/**
 * Populates the given vector with the POSITIONs of all selected slices, in the order
 * you would expect to see them (row-wise, left-to-right). 
 */
void CTomatoAnalyzerDoc::GetSelectedSliceByNaturalSequence(std::vector<POSITION>& vctPos)
{
	std::vector<int>    vctIndex;
	putSlicesInOrder( vctIndex );

	vctPos.clear();
	std::vector<int>::iterator  Iter = vctIndex.begin();
	std::vector<int>::iterator  IterEnd = vctIndex.end();
	while(Iter != IterEnd)
	{
		POSITION pos = m_SliceList.FindIndex(*Iter);
		CSlice * pSlice = GetSlice(pos);
		{
			if(NULL != pSlice)
			{
				if(! pSlice->GetExclusionFlag())
				{
					vctPos.push_back(pos);
				}
			}
		}

		++Iter;
	}
}

/**
 * Populates the given vector with the POSITIONs of all slices, regardless of whether they 
 * are currently selected, in the order you would expect to see them (row-wise, left-to-right). 
 */
void CTomatoAnalyzerDoc::GetAllSlicesByNaturalSequence(std::vector<POSITION>& vctPos)
{
	std::vector<int>    vctIndex;
	putSlicesInOrder( vctIndex );

	vctPos.clear();
	std::vector<int>::iterator  Iter = vctIndex.begin();
	std::vector<int>::iterator  IterEnd = vctIndex.end();
	while(Iter != IterEnd)
	{
		POSITION pos = m_SliceList.FindIndex(*Iter);
		CSlice * pSlice = GetSlice(pos);
		{
			if(NULL != pSlice)
			{
					vctPos.push_back(pos);
			}
		}

		++Iter;
	}
}

void CTomatoAnalyzerDoc::GetArchiveName(LPSTR szBuffer)
{
    char    szPath[_MAX_PATH+1];
    char *  lpName;

    GetFullPathName(GetPathName(), _MAX_PATH, szPath, &lpName);
    lstrcpy(szBuffer, lpName);
    return;
}

BOOL CTomatoAnalyzerDoc::IsThereAnySliceChanged()
{
	POSITION pos = m_SliceList.GetHeadPosition();
	while(pos != NULL)
    {
        CSlice * pSlice = m_SliceList.GetNext(pos);
        if(pSlice->IsAdjusted())
            return TRUE;
    }

    return FALSE;
}

BOOL CTomatoAnalyzerDoc::IsThereAnySliceRotated()
{
	POSITION pos = m_SliceList.GetHeadPosition();
	while(pos != NULL)
    {
        CSlice * pSlice = m_SliceList.GetNext(pos);
        double angle = pSlice->GetRotationDegree();
        if(angle != 0.0)
            return TRUE;
    }

    return FALSE;
}

/**
 * If there are any unsaved changes to the file, asks if the user wants to save. Returns true if 
 * the user wants to proceed with closing the document, false if the user cancels. 
 */
bool CTomatoAnalyzerDoc::QueryForSaveModification()
{
	if(!m_AlreadySaved && IsThereAnySliceChanged())
    {
		CString file = m_ImageFileName; 
		int pos = m_ImageFileName.ReverseFind('\\'); 
		file = file.Mid(pos + 1); 

		CString msg; 
		msg.Format("Do you want to save the changes made to %s?", file); 
        int nRet = AfxMessageBox(msg, MB_YESNOCANCEL);
		if (nRet == IDYES)
            OnFileSaveAs();
		if (nRet == IDNO)
			return true; 
		/*if (!m_AlreadySaved)	// checks if user canceled the message box or the save dialog
			return false; */
    }
	return true; 
}

//Added by Hale - 04/07/2018
bool CTomatoAnalyzerDoc::QuerySave(bool nRet)
{
	if (!m_AlreadySaved && IsThereAnySliceChanged())
	{
		CString file = m_ImageFileName;
		int pos = m_ImageFileName.ReverseFind('\\');
		file = file.Mid(pos + 1);

		if (nRet == true) {
			OnFileSaveAs();
			return true;
		}
		
	}
	return false;
}


void CTomatoAnalyzerDoc::OnDataSelection()
{
    CSelectMeasurement  dlg;
    ((CTomatoAnalyzerApp * )AfxGetApp())->GetSelectedMeasurements(dlg.m_vctSelectedItem);

    int bOk = dlg.DoModal();
    if(TRUE == bOk)
    {
        ((CTomatoAnalyzerApp * )AfxGetApp())->SetSelectedMeasurements(dlg.m_vctSelectedItem);
      	m_iSelectedDrawIndex=INDEX_AREA;
        UpdateAllViews(NULL, DOC_UPDATEVIEW_MEASURECHANGED);
    }
}

/**
 * Returns TRUE if the document can be closed (i.e., there are no unsaved changes that the user wants to save). 
 */
BOOL CTomatoAnalyzerDoc::CanCloseFrame( CFrameWnd* pFrame )
{
	if (! QueryForSaveModification()) 
		return FALSE; 
	
	//Deletes extra file created when exiting and choosing not to save
	
	/*if (m_ImageFileName != "")
	{
		string fileName = m_ImageFileName;
		int size = fileName.size();
		fileName.erase(size - 4);
		CString flNm(fileName.c_str());	
		if( remove( flNm) == -1 )
			perror( "Error deleting file" );
	}*/
  
    return CDocument::CanCloseFrame(pFrame);

}

/** 
 * Returns a pointer to the current CTomatoAnalyzerView, or NULL if there is none. 
 */
CTomatoAnalyzerView* CTomatoAnalyzerDoc::GetCurrentAnalyzerView() {
	POSITION pos_view = GetFirstViewPosition();
	CView * pView;
	while(NULL != pos_view)
	{
		pView = GetNextView(pos_view);
		if(pView->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerView)))
		{
			return (CTomatoAnalyzerView*) pView; 
		}
	}
	return NULL; 
}
