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
// TomatoAnalyzer.cpp : Defines the class behaviors for the application.

#include "stdafx.h"
#include "BoundingBoxList.h"
#include "TomatoAnalyzer.h"
#include "AdvImage.h"
#include "Slice.h"
#include "MainFrm.h"
#include "ManualDPIDialog.h"
//#include "PostDPIAdjustmentDialog.h"

#include "TomatoAnalyzerDoc.h"
#include "TomatoAnalyzerView.h"

#include "ParameterSettings.h"

#include "SaveData.h"
#include "ScannerSetting.h"
#include "Blockiness.h"
#include "ColorTest.h"
#include "UserDefinedDialog.h"
#include "UserAddData.h"
#include "EditUserData.h"
#include "LABDialog.h"
#include "StartUpDialog.h"
#include "PCAPOINTS.h"
#include "EllipseSize.h"
#include "PostDPIDialog.h"

#include "CheckboxTreeCtrl.h"
#include "SaveDataSelectionDlg.h"
#include "SelectMeasurement.h"
#include "Msg.h"
#include "ColorRangeDialog.h"

#include "PointyLevel.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define _CRTDBG_MAP_ALLOC		// TMP!!!! 
#include <stdlib.h>
#include <crtdbg.h>



// Default IDs of measurements to save (in case no settings are saved in registry?) 
char szDefaultSaveData[]="0 1 2 3 4 5 6";		// Basic Measurements (Area, Perimeter, etc.) 

extern int atovct(const std::string& str, std::vector<int>& vct);
extern int atovct(const std::string& str, std::vector<float>& vct);

// Counts the number of measurements that the user has added
int userCounter = 0;


// CTomatoAnalyzerApp

BEGIN_MESSAGE_MAP(CTomatoAnalyzerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_DISCLAIMER, OnAppDisclaimer)
    ON_COMMAND(IDC_POINTI_LEVEL, OnCmdPointiLevelControl)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
//	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)

    ON_COMMAND(IDC_SCANNER_DPI, OnScannerDpiSetting)
	ON_COMMAND(IDC_USER_DPI, OnCmdAdjustDPI) // added 8/4/15 to adjust the scanner DPI by drawing 

	ON_COMMAND(IDC_BLOCKINESS_POS, OnCmdBlockinessPosition)
	ON_COMMAND(ID_COLORTEST_ANALYZE, OnCmdColorTest)
	//ON_COMMAND(ID_COLORTEST_MERGECSVS, OnCmdMergeCSVs)
	//ON_COMMAND(ID_ASYMMETRY_CHECK, OnAsymmetryCheck)
	ON_COMMAND(ID_SYSTEM_SETTING, OnCmdSettings)
	ON_COMMAND(ID_USERSETTINGS_LOADSETTINGS, OnLoadUserSettings)
	ON_COMMAND(ID_USERSETTINGS_SAVESETTINGS, OnSaveUserSettings)
	ON_COMMAND(ID_SETTINGS_EXPORTPCAPOINTS, OnSavePCAPoints)
	ON_COMMAND(ID_SETTINGS_DEFAULTELLIPSESIZE, OnSetEllipseSize)
	ON_COMMAND(IDC_DATA_ADDFIELD, OnCmdAddMeasure) // Message Map handler for the Add Measurement dialog
	ON_COMMAND(IDC_DATA_ADDINFO, OnCmdAddData) // Handler for the "Add/Edit Data..." dialog
	ON_COMMAND(ID_ANALYZE_COLOR, OnCmdColorTest)

	//ON_EN_CHANGE(IDC_ADDF_MEASURE1, &CTomatoAnalyzerApp::OnChangeMeasurement1)
END_MESSAGE_MAP()


// CTomatoAnalyzerApp construction

CTomatoAnalyzerApp::CTomatoAnalyzerApp()
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );	// TMP!!! 

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTomatoAnalyzerApp object

CTomatoAnalyzerApp theApp;

int CTomatoAnalyzerApp::GetSelectedSlices()
{
		CTomatoAnalyzerDoc * pDoc = GetCurrentDoc(); 
		std::vector<POSITION> vctPos;
		pDoc->GetSelectedSliceByNaturalSequence(vctPos);
	   int Slices = vctPos.size();
	  return Slices;
}

// CTomatoAnalyzerApp initialization

/**
 * Initializes the application and loads settings from the registry. 
 */
BOOL CTomatoAnalyzerApp::InitInstance()
{

	//CStartUpDialog dlg;
	//dlg.DoModal();
	
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	LoadSettings("Settins"); 

	/***************Changed 7/10/06 to set initial ellipse size from a saved value********************/
	CTomatoAnalyzerDoc * pDoc = GetCurrentDoc(); 
	if(NULL != pDoc)
	{
		std::vector<POSITION> vctPos;
		pDoc->GetSelectedSliceByNaturalSequence(vctPos);
		int nSelectedSlices = vctPos.size();	// how many slices are being analyzed?
		if (nSelectedSlices > 0)
		{
			std::vector<POSITION>::iterator  Iter = vctPos.begin();
			CSlice * pSlice = pDoc->GetSlice(*Iter);
			int tempSize = pSlice->m_ellipseSize;
			if (tempSize > 0 && tempSize <= 100)
				m_ellipseSize = tempSize;
			else
				m_ellipseSize = 90;
		}
	}
	/***************************************End Changes***********************************************/
	initMeasurementSet();

	// headless mode support 
	boolean isHeadless = false;

	// look for a sign that we're running in headless mode.
	// in headless mode, there is no gui. 
	for (int i = 0; i < __argc && !isHeadless; i++) {
		if (strcmp(__argv[i], "-h") == 0) {
			isHeadless = true;
		}
	}

	// headless application loop 
	// added by Maxwell Taylor <mtaylor18@wooster.edu>
	while (isHeadless) {
		using namespace std;
		string userInput;  // input 
		int width, height; // width and height of image (not of pixel list!) 
		bool hasWidth = false, hasHeight = false, ok = false; // error checking
		bool hasPixelArray = false;

		// read a line of input 
		getline(cin, userInput);

		// our tomato doc, image, and json parser
		CTomatoAnalyzerDoc tomatoDoc;
		CAdvImage image;
		rapidjson::Document document;

		// try to get some JSON parsed
		document.Parse(userInput.c_str());

		// try to get width
		if (document.HasMember("width") && document["width"].IsNumber()) {
			width = document["width"].GetInt();
			hasWidth = true;
		}

		// try to get height
		if (document.HasMember("height") && document["height"].IsNumber()) {
			height = document["height"].GetInt();
			hasHeight = true;
		}

		// try to get the pixels 
		if (document.HasMember("pixels") && document["pixels"].IsArray() && document["pixels"].Size() == width * height) {
			hasPixelArray = true;
		} 

		// change the dpi, if it was provided 
		if (document.HasMember("dpi") && document["dpi"].IsNumber()) {
			double dpi = document["dpi"].GetDouble();
			m_dpi = dpi;
		}

		// make sure we had requisite fields
		if (hasWidth && hasHeight && hasPixelArray) {
			ok = true;
		} else {
			cout << "{\"name\":\"missing or invalid field\", \"message\":\"" << (hasWidth ? "" : "width ") << (hasHeight ? "" : " height ") << (hasPixelArray ? "" : " array ") << "\"}" << endl;
			continue;
		}

		// create the RGB
		image.CreateRGB(width, height);

		// try to get the pixel array [r, g, b, r, g, b, etc...]
		if (document.HasMember("pixels")) {
			rapidjson::Value& pixels = document["pixels"];
			
			// loop through and populate the pixel array (we use 3 because there are 3 channels in the image)  
			for (int i = 0, n = 0; i < width * height * 3; i+=3, n++) {
				int flatValue = i / 3; // this is the index into a 'flattened' image 

				int widthIndex = flatValue % width; // width index into a two dimensional array
				int heightIndex = flatValue / width; // height index into a two dimensional array 

				int red = pixels[n].GetUint() >> 16;
				int green = (pixels[n].GetUint() >> 8) & ~(0xFFFF << 8);
				int blue = pixels[n].GetUint() & ~(0xFFFF << 8);

				// set the RGB values (red 0) (green 1) (blue 2) 
				image.SetPixelAt(widthIndex, heightIndex, 0, red);
				image.SetPixelAt(widthIndex, heightIndex, 1, green);
				image.SetPixelAt(widthIndex, heightIndex, 2, blue);
				
				//cout << "{\"foo\":\"bar\"}" << endl;
			}
		}
		//assign the image to the tomato doc's image
		tomatoDoc.m_pImage = &image;

		// now, let's analyze the image 
		tomatoDoc.OnAnalyzeImage();

		// at this point, we ought to be able to get the 
		// positions of the slices.
		vector<POSITION> imageSlices;
		tomatoDoc.GetAllSlicesByNaturalSequence(imageSlices);

		// Let's start the JSON object we're going to return 
		rapidjson::StringBuffer jsonString;
		rapidjson::Writer<rapidjson::StringBuffer> writer(jsonString); 

		// now, we can iterate through the position vector and hopefully
		// get some information about slices 
		MeasurementSet measurementSet = this->getMeasurementSet();

		writer.StartArray();
		int idx = 0;
		for (vector<POSITION>::iterator i = imageSlices.begin(); i != imageSlices.end(); ++i) {
			writer.StartObject();
			CSlice *tempSlice = tomatoDoc.GetSlice(*i);

			// let's go ahead and get any possible boundary changes 
			if (document.HasMember("boundaries") && document["boundaries"].IsArray()) {
				rapidjson::Value& objs = document["boundaries"];
				for (int j = 0; j < document["boundaries"].Size(); i++) {
					// make sure this is the id we're looking for 
					if (objs[j]["id"] != idx) {
						break;
					}

					// this will copy over the boundary 
					rapidjson::Value& adjustedBoundary = objs[j]["boundary"];
					CPixelList *newBoundary = new CPixelList();
					for (int p = 0; p < adjustedBoundary.Size(); p++) {
						// check for a bad value 
						if (!(adjustedBoundary[p].IsArray() && adjustedBoundary[p]["x"].IsNumber() && adjustedBoundary[p]["y"].IsNumber())
							|| (adjustedBoundary[p]).Size() != width * height * 3) {
							delete newBoundary;
							break;
						}
						newBoundary->push_back(CPixel(adjustedBoundary[p]["x"].GetInt(), adjustedBoundary[p]["y"].GetInt()));
					}

					// set the boundary 
					tempSlice->Set(newBoundary);
				}
			}

			// iterate over value set 
			for (vector<CString>::iterator a = measurementSet.measurements.begin(); a != measurementSet.measurements.end(); ++a) {
				char buffer[1024 + 1];
				double result;

				// get descriptor values 
				tempSlice->GetDescriptorValue(measurementSet.measurementID(*a), 0, buffer, &result);
				writer.String(*a);
				writer.Double(result);
			}

			// Now, lets get a list of {x,y} objects so we can send back the boundary 
			CPixelList* pixelList = tempSlice->GetBoundary(0);

			writer.String("boundary"); 
			writer.StartArray(); // begin the array of {x, y} objects 

			// iterate through the pixel list
			for (CPixelList::iterator iterator = pixelList->begin(); iterator != pixelList->end(); ++iterator) {
				writer.StartObject(); // start an {x, y} object
				CPixel pixel = *iterator;

				// get the x value
				writer.String("x");
				writer.Int(pixel.GetX() + tempSlice->mBBox.mUpperLeft.x);

				// get the y value 
				writer.String("y");
				writer.Int(pixel.GetY() + tempSlice->mBBox.mUpperLeft.y);
				
				writer.EndObject();   // end the current {x, y} object 
			}


			writer.EndArray();   // end the array of {x, y} objects 

			writer.EndObject();  // end the current slice 
			idx++;
		}
		writer.EndArray(); // end the array of slices 
		cout << jsonString.GetString() << endl;
	}

	//AsymmetryCheck = false;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTomatoAnalyzerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTomatoAnalyzerView));
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	return TRUE;
}

/**
 * Saves settings to the registry and exits the application. 
 */
int CTomatoAnalyzerApp::ExitInstance()
{
	/*WriteProfileInt(_T("Settins"), _T("ShapePercent"), m_nShapeIndex);
	
	char szTemp[64];
	sprintf(szTemp, "%0.1f", g_shapethresh);
	WriteProfileString(_T("Settins"), _T("ShapeThresh"), szTemp);*/

	SaveSettings("Settins");     
	return CWinApp::ExitInstance();
}

/**
 * Loads the settings from a text file.
 * param user is either "Settins" (the default user) or the absolute filepath of 
 * our settings file.
 */
void CTomatoAnalyzerApp::LoadSettings(char* user)
{
	// if user is Settins, then load the settings from the registry
	if(strcmp(user,"Settins") == 0) {
		// Change the registry key under which our settings are stored
		SetRegistryKey(_T("OSU-OARDC Tomato Analyze"));

		LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
		m_nShapeIndex = GetProfileInt(_T(user), _T("ShapePercent"), 10);

		m_dpi = GetProfileInt(user, "ScannerDPI", 300);
		m_iUnit = GetProfileInt(user, "MeasureUnit", 0);

		//set default PCA points
		m_numPCAPoints = GetProfileInt("Settins", "PCAPoints", 10);
		//set default ellipse size
		m_ellipseSize = GetProfileInt("Settins", "EllipseSize", 90);

		CString cs = GetProfileString(_T(user), _T("ShapeThresh"), "1.0");
		g_shapethresh = atof((LPCSTR)cs);

   		cs = GetProfileString(_T(user), _T("SaveData"), szDefaultSaveData);
		std::string str((LPCSTR)cs);
		atovct(str, m_vctSaveDataItem);

		std::vector<float> m_vctTmpFloat;
   		cs = GetProfileString(_T(user), _T("Blkiness"), "0.1 0.9");
		str.assign(cs);
		int result = atovct(str, m_vctTmpFloat);
		if(result==1 && m_vctTmpFloat.size()==2)
		{
			m_fBlkUp = m_vctTmpFloat[0];
			m_fBlkDn = m_vctTmpFloat[1];
		}
		else
		{
			m_fBlkUp = 0.1f;
			m_fBlkDn = 0.9f;
		}

		m_colorCalibrationFile = GetProfileString(user, "ColorCalibrationFile", ""); 
		int tmp = GetProfileInt(user, "ColorObserver", 0); 
		switch (tmp) {
			case 1: m_Observer = D65_2; break; 
			case 2: m_Observer = D65_10; break; 
			default: m_Observer = C_2; break; 
		}

		// equation values
		cs = GetProfileString(_T(user), _T("ColorTestEquation"), "1 1 1 0 0 0");
		str.assign(cs);
		result = atovct(str, m_vctTmpFloat);
		if(result==1 && m_vctTmpFloat.size()==6)
		{
			m_slope_l = m_vctTmpFloat[0];
			m_slope_a = m_vctTmpFloat[1];
			m_slope_b = m_vctTmpFloat[2];
			m_yint_l  = m_vctTmpFloat[3];
			m_yint_a  = m_vctTmpFloat[4];
			m_yint_b  = m_vctTmpFloat[5];
		}
		else
		{		
			m_slope_l = 1;
			m_slope_a = 1;
			m_slope_b = 1;
			m_yint_l  = 0;
			m_yint_a  = 0;
			m_yint_b  = 0;
		}

		BOOL bError = FALSE;
		m_nMac = GetProfileInt(user, "Macro", 10);
		if(m_nMac<=0 || m_nMac>=50)
		{
			bError = TRUE;
			m_nMac = 10;
		}

		m_nMic = GetProfileInt(user, "Micro", 5);
		if(m_nMic <= 0)
		{
			bError = TRUE;
			m_nMic = m_nMac/2;
		}
		
		m_nBrightLevel = GetProfileInt(user, "BrightLevel", 0);

		if(bError)
			AfxMessageBox("Pointi level needs to be set", MB_OK);

	}
	// else, load it in the from the file
	else {
		// open file
		FILE *fp;
		fp=fopen(user, "r");

		if(!fp){
			return;
		}

		char line_cstr[512]; // the current line of the file
		BOOL bError = FALSE;

		// read in each line
		while(!feof(fp) && fgets(line_cstr, sizeof(line_cstr), fp)) {
		
			std::string line(line_cstr); // make it a string

			// find the equals sign (each line looks like "attribute=val")
			if(line.find("=",0) != string::npos){
				// break the string into the attribute name and its value
				int equalpos = line.find("=",0);
				std::string tempattr = line.substr(0, equalpos);
				std::string tempval = line.substr(min(equalpos+1, line.length()));

				// trim endl
				int lastKept; 
				for (lastKept = tempval.length() - 1; lastKept > 0 && isspace(tempval[lastKept]); lastKept--) { }
				tempval.resize(lastKept + 1);	

				const char* attr = tempattr.c_str();
				const char* val = tempval.c_str();

				// check which attribute we're at and set it appropriately
				if(strcmp(attr,"ShapePercent") == 0)
					m_nShapeIndex = atof(val);
				else if(strcmp(attr,"ShapeThresh") == 0)
					g_shapethresh = atof(val);
				else if(strcmp(attr,"ScannerDPI") == 0)
					m_dpi = atof(val);
				else if(strcmp(attr,"MeasureUnit") == 0)
					m_iUnit = atof(val);
				else if(strcmp(attr,"SelectedMeasurements") == 0)
					atovct(val, m_vctSaveDataItem);
				else if(strcmp(attr,"Blkiness") == 0){
					std::vector<float> m_vctTmpFloat;
					int result = atovct(val, m_vctTmpFloat);
					if(result==1 && m_vctTmpFloat.size()==2)
					{
						m_fBlkUp = m_vctTmpFloat[0];
						m_fBlkDn = m_vctTmpFloat[1];
					}
					else
					{
						m_fBlkUp = 0.1f;
						m_fBlkDn = 0.9f;
					}
				}
				else if (strcmp(attr, "ColorObserver") == 0) {
					int tmp = atoi(val); 
					switch (tmp) {
						case 0: m_Observer = C_2; break; 
						case 1: m_Observer = D65_2; break; 
						case 2: m_Observer = D65_10; break; 
						default: m_Observer = C_2; 
					}
				}
				else if (strcmp(attr, "ColorCalibrationFile") == 0) {
					m_colorCalibrationFile = val; 
				}
				else if (strcmp(attr, "ColorTestEquation") == 0) {
					std::vector<float> m_vctTmpFloat;
					int result = atovct(val, m_vctTmpFloat);
					if(result==1 && m_vctTmpFloat.size()==6)
					{
						m_slope_l = m_vctTmpFloat[0];
						m_slope_a = m_vctTmpFloat[1];
						m_slope_b = m_vctTmpFloat[2];
						m_yint_l  = m_vctTmpFloat[3];
						m_yint_a  = m_vctTmpFloat[4];
						m_yint_b  = m_vctTmpFloat[5];
					}
					else
					{		
						m_slope_l = 1;
						m_slope_a = 1;
						m_slope_b = 1;
						m_yint_l  = 0;
						m_yint_a  = 0;
						m_yint_b  = 0;
					}
				}
				else if(strcmp(attr,"Macro") == 0){
					m_nMac = atof(val);
					if(m_nMac<=0 || m_nMac>=50)
					{
						bError = TRUE;
						m_nMac = 10;
					}
				}
				else if(strcmp(attr,"Micro") == 0){
					m_nMic = atof(val);
					if(m_nMic <= 0)
					{
						bError = TRUE;
						m_nMic = m_nMac/2;
					}
				}
				else if(strcmp(attr,"BrightLevel") == 0)
					m_nBrightLevel = atof(val); 

				else if(strcmp(attr,"PCAPoints") == 0)
					m_numPCAPoints = atof(val);

				else if(strcmp(attr,"EllipseSize") == 0)
					m_ellipseSize = atof(val);

					
			} 
		}
	// close the file stream
	fclose(fp);	
	//Following added by David 1/8/07
	//Apply the loaded settings to the fruit slices and recalculate with them
	CTomatoAnalyzerDoc * pDoc = GetCurrentDoc(); 
	if (NULL == pDoc)
		return; 

	std::vector<POSITION> vctPos;
	pDoc->GetSelectedSliceByNaturalSequence(vctPos);
	for (std::vector<POSITION>::iterator  Iter = vctPos.begin(); Iter != vctPos.end(); Iter++)
	{
		CSlice *  pSlice = pDoc->GetSlice(*Iter);
		pSlice->CalcParameters(FALSE);
	}
	if(NULL != pDoc)
		pDoc->UpdateAllViews(NULL); 
	}
}

/**
 * Save the settings to either the registry or a .ctmt file.
 * param user is either "Settins" (the default user) or the absolute filepath of 
 * our settings file.
 */
void CTomatoAnalyzerApp::SaveSettings(const char* user){
	// if it's the default settings, write it to the registry
	if(strcmp(user,"Settins") == 0) {
		// save the open tabs/columns
		SetSelectedMeasurements(m_vctSaveDataItem, user);

		// save blockiness setting
		char	szBuffer[128];
		sprintf(szBuffer, "%.2f  %.2f", m_fBlkUp, m_fBlkDn);
		WriteProfileString(user, _T("Blkiness"), szBuffer);

		WriteProfileInt(user, _T("ColorObserver"), m_Observer); 
		WriteProfileString(user, _T("ColorCalibrationFile"), m_colorCalibrationFile); 

		// save color test equations settings
		char	szBuffer3[128];
		sprintf(szBuffer3, "%f %f %f %f %f %f", m_slope_l, m_slope_a, m_slope_b, m_yint_l, m_yint_a, m_yint_b);
		WriteProfileString(user, _T("ColorTestEquation"), szBuffer3);

		//save shape thresh
		char szTemp[64];
		sprintf(szTemp, "%0.1f", g_shapethresh);
		WriteProfileString(user, _T("ShapeThresh"), szTemp);

		// save shape index
		WriteProfileInt(user, _T("ShapePercent"), m_nShapeIndex);

		WriteProfileInt(user, "ScannerDPI", m_dpi);
		WriteProfileInt(user, "MeasureUnit", m_iUnit);
		WriteProfileInt(user, "BrightLevel", m_nBrightLevel);

		WriteProfileInt(user, "Macro", m_nMac);
		WriteProfileInt(user, "Micro", m_nMic);

		WriteProfileInt(user, "PCAPoints", m_numPCAPoints);
		WriteProfileInt(user, "EllipseSize", m_ellipseSize);
			
	}
	// and if it's not, write it to a file
	else {
		ofstream f;
		f.open(user);		//open a file

		// save the open tabs/columns
		f << "SelectedMeasurements=";
		f << SetSelectedMeasurements(m_vctSaveDataItem, user).c_str() << endl;

		// save blockiness setting
		char	szBuffer[128];
		sprintf(szBuffer, "%.2f  %.2f", m_fBlkUp, m_fBlkDn);
		f << "Blkiness=" << szBuffer << endl;

		f << "ColorObserver=" << m_Observer << endl; 
		f << "ColorCalibrationFile=" << m_colorCalibrationFile << endl; 

		// save color test equations settings
		char	szBuffer3[128];
		sprintf(szBuffer3, "%f %f %f %f %f %f", m_slope_l, m_slope_a, m_slope_b, m_yint_l, m_yint_a, m_yint_b);
		f << "ColorTestEquation=" << szBuffer3 << endl;

		//save shape thresh
		char szTemp[64];
		sprintf(szTemp, "%0.1f", g_shapethresh);
		f << "ShapeThresh=" << szTemp << endl;

		// save shape index
		f << "ShapePercent=" << m_nShapeIndex << endl;

		// scanner dpi
		f << "ScannerDPI=" << m_dpi << endl;
		// measurement units
		f << "MeasureUnit=" << m_iUnit << endl;
		// brightness level
		f << "BrightLevel=" << m_nBrightLevel << endl;

		// macro level
		f << "Macro=" << m_nMac << endl;
		// micro level
		f << "Micro=" << m_nMic << endl;

		f << "PCAPoints=" << m_numPCAPoints << endl;
		f << "EllipseSize=" << m_ellipseSize << endl;

		// close the file
		f.close();

	}
}

/**
 * Opens a file dialog and saves the current settings to the selected .ctmt file.
 */
void CTomatoAnalyzerApp::OnSaveUserSettings()
{

	TCHAR   lpszFileBuffer[MAX_PATH];
	memset(lpszFileBuffer, 0, sizeof(lpszFileBuffer));

	CFileDialog fdlg(FALSE, 
		             NULL, 
				     NULL, 
				     OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_OVERWRITEPROMPT,
		             _T("Config Files (*.ctmt)|*.ctmt||")
				   );

	fdlg.m_ofn.nMaxFile  = sizeof(lpszFileBuffer);
	fdlg.m_ofn.lpstrFile = lpszFileBuffer;
    fdlg.m_ofn.lpstrDefExt = "ctmt";


	if(fdlg.DoModal()==IDOK)
	{
		SaveSettings(lpszFileBuffer);
    }
	

}

/**
 * Opens a file dialog and loads the settings from the selected .ctmt file. 
 */
void CTomatoAnalyzerApp::OnLoadUserSettings()
{
	CFileDialog fdlg(TRUE, 
		             NULL, 
				     NULL, 
				     OFN_PATHMUSTEXIST | OFN_EXPLORER |OFN_FILEMUSTEXIST,
		             _T("Config Files|*.ctmt;|Config File (*.ctmt)|*.ctmt||")
				   );

	int nBufferLength = 5*1024;
	TCHAR * lpszFileBuffer = new TCHAR[nBufferLength];
	memset(lpszFileBuffer, 0, nBufferLength);

	fdlg.m_ofn.nMaxFile  = nBufferLength;
	fdlg.m_ofn.lpstrFile = lpszFileBuffer;

	// if the user clicked ok, load the settings from the file
	if(fdlg.DoModal()==IDOK){
		LoadSettings(lpszFileBuffer);
	}

}

/// Overridden so the MRU list will work. 
CDocument* CTomatoAnalyzerApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	CTomatoAnalyzerDoc* doc = GetCurrentDoc(); 
	CString s(lpszFileName); 
	doc->OpenSingleFile(s); 

	// http://www.codeproject.com/KB/dialog/rfldlg.aspx
	//We need to return a non null value otherwise the most recent file will be 
	//deleted from the menu.  If the calling function is the 
	//CWinApp::OnOpenRecentFile it will not use the return value, so it is safe to 
	//return a bogus value.
	return((CDocument*) 1); 
}

/// Not used? 
void CTomatoAnalyzerApp::OnMeasureSelection()
{
    CSaveDataSelectionDlg   dlg;
    dlg.m_vctSelectedItem = m_vctSaveDataItem;

    dlg.DoModal();
    int bOk = dlg.DoModal();
    if(TRUE == bOk)
    {
        m_vctSaveDataItem = dlg.m_vctSelectedItem;
    }
}


std::vector<int> CTomatoAnalyzerApp::getMeasureItemsNeededToSave()
{
    return m_vctSaveDataItem;
}

void CTomatoAnalyzerApp::GetSelectedMeasurements(std::vector<int>& dataSelected)
{
    dataSelected = m_vctSaveDataItem;
}

/**
 * Sets the measurements currently selected for display to the given list of measurement IDs, and 
 * returns the measurements IDs, separated by spaces, in a string. 
 * If user is "SETTINS", saves the updated list of selected measurements to the settings in the registry. 
 */
std::string CTomatoAnalyzerApp::SetSelectedMeasurements(std::vector<int>& dataSelected, const char* user)
{
    m_vctSaveDataItem = dataSelected;

	CString outStr; 
	for(int i=0; i < m_vctSaveDataItem.size(); i++)
    {
		CString s; 
		s.Format("%d ", m_vctSaveDataItem[i]);
		outStr += s; 
    }

	if(strcmp(user, "SETTINS"))  WriteProfileString(user, _T("SaveData"), (LPCTSTR) outStr);
	std::string meas_str((LPCTSTR) outStr);
	return meas_str;
}

/**
 * Displays dialog for scanner DPI and unit settings. 
 */
void CTomatoAnalyzerApp::OnScannerDpiSetting()
{
    CScannerSetting    dlg;
    dlg.m_dpi = m_dpi;
    dlg.m_iUnit = m_iUnit;
    
	dlg.DoModal();
    m_dpi = dlg.m_dpi;
    m_iUnit = dlg.m_iUnit;

    CDocument * pDoc = GetCurrentDoc(); 
    if(NULL != pDoc)
        pDoc->UpdateAllViews(NULL);

}

/**
 * Allow the user to draw a line to calibrate the DPI
 */
void CTomatoAnalyzerApp::OnCmdAdjustDPI() {
	// display the unit selection window
	
	ManualDPIDialog dlg;
	dlg.DoModal();

	// get whether or not our units are inches/centimers, and get the current document
	ManualDPIDialog::MeasurementType m_type = dlg.measurement; 

	// see if we were to cancel the measurement
	if (dlg.measurement == ManualDPIDialog::MeasurementType::CANCEL) {
		return;
	}

	this->m_iUnit = (int) m_type;

	CTomatoAnalyzerDoc *doc = GetCurrentDoc();

	// this should be finding the fruit view (all of the fruits displayed on the left)
	CView *view = NULL;
	POSITION pos = doc->GetFirstViewPosition();

	while (NULL != pos) {
		view = doc->GetNextView(pos);

		if (view->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerView))) {
			break;
		}
		view = NULL;
	}

	// make sure the view isn't null
	if (NULL == view) {
		std::cerr << "Wrong view inside OnCmdAdjustDPI()" << std::endl;
		return;
	}

	// we need to get our bitmap
	CBitmap *bp = ((CTomatoAnalyzerView*) view)->GetBitmap();

	if (NULL == bp) {
		return;
	}

	// try to get the capture
	view->SetCapture();
	CDC* drawDC = view->GetDC();

	bool cnu = true, beenClicked = false;
	POINT firstClick, secondClick;
	while (cnu) {
		MSG msg;
		VERIFY(::GetMessageA(&msg, NULL, 0, 0));

		// handle the different messages 
		switch (msg.message) {
			// handle the left button down event 
		case WM_LBUTTONDOWN:
			// we need to get information about the click 
			if (beenClicked) {
				secondClick.x = (int) LOWORD(msg.lParam);
				secondClick.y = (int) HIWORD(msg.lParam);
				cnu = false;
			} else {
				firstClick.x = (int) LOWORD(msg.lParam);
				firstClick.y = (int) HIWORD(msg.lParam);
				beenClicked = true;
			}
			break;

			// I don't want the other view getting the mouseup signal 
		case WM_LBUTTONUP:
			break;

			// handle when a key is pressed 
		case WM_KEYDOWN:
			// escape key
			if (msg.wParam == VK_ESCAPE) {
				return;
			}
			break;

		default:
			DispatchMessage(&msg);
			break;
		}
	}

	// if we get here, let's draw a line on the bitmap 
	int save = drawDC->SaveDC();
	CPen pen(PS_SOLID, 0, RGB(0, 255, 0));
	drawDC->SelectObject(&pen);

	drawDC->MoveTo(firstClick);
	drawDC->LineTo(secondClick);

	drawDC->RestoreDC(save);

	ReleaseCapture();

	// now, we have to count the pixels to determine DPI
	BITMAP bitmap;
	bp->GetBitmap(&bitmap);

	// These following two lines were added by Hale
	CTomatoAnalyzerDoc *docc= GetCurrentDoc();
	CAdvImage* img = docc->m_pImage;

	// we use the distance forumla to get length
	double ourDPI = sqrt(pow((double) secondClick.x - firstClick.x, 2) + pow((double) secondClick.y - firstClick.y, 2));
	double num_units = 0;

	PostDPIDialog dialog;
	dialog.DoModal();

	if ((num_units = dialog.get_num_units()) != 0) {
		this->m_dpi = ourDPI/num_units;
	} else {
		this->m_dpi = ourDPI;
	}
	
	CDocument * pDoc = GetCurrentDoc(); 
    if(NULL != pDoc)
        pDoc->UpdateAllViews(NULL);
}

/**
 * Returns the factor by which values in pixels must be multiplied to get values in the 
 * currently selected units, based on the currently selected DPI. 
 */
double CTomatoAnalyzerApp::getUnitScale()
{
    switch(m_iUnit)
    {
    case 0: //pixel
         return 1.0;

    case 1: //millimeter
         return double(25.4)/(double)m_dpi;

    case 2: //centimeter
         return double(2.54)/(double)m_dpi;
         
    case 3: //inch
         return double(1.0)/(double)m_dpi;
    
    default:
         break;
    }

    return -1;
}

/**
 * Displays dialog for blockiness position settings. 
 */
void CTomatoAnalyzerApp::OnCmdBlockinessPosition()
{
	CBlockiness dlg;

	dlg.m_fUpPos = m_fBlkUp;
	dlg.m_fLwPos = m_fBlkDn;
	
	if(dlg.DoModal() == TRUE)
	{

		m_fBlkUp = dlg.m_fUpPos;
		m_fBlkDn = dlg.m_fLwPos;

		CTomatoAnalyzerDoc * pDoc = GetCurrentDoc(); 
		if(NULL != pDoc)
			pDoc->OnBlockySettingChange();
	}
}
 
/**
 * Handles the Color Analysis feature. Shows the Color Test dialog and reacts to user input-- 
 * saves settings, then may perform either color calibration or color analysis of the image. 
 */
void CTomatoAnalyzerApp::OnCmdColorTest()
{
	bool justCalibrated, cancelledRanges; 
	do {
		justCalibrated = false; 
		cancelledRanges = false; 
		int basicSel = getMeasurementGroupSelected(CMsg::load(IDS_GROUP_COLOR_ANALYSIS)); 
		int lRangeSel = getMeasurementGroupSelected(CMsg::load(IDS_GROUP_L_RANGES)); 
		int hueRangeSel = getMeasurementGroupSelected(CMsg::load(IDS_GROUP_HUE_RANGES)); 
		int chromaRangeSel = getMeasurementGroupSelected(CMsg::load(IDS_GROUP_CHROMA_RANGES)); 
		int defaultRangeSel = (lRangeSel == hueRangeSel && hueRangeSel == chromaRangeSel) ? lRangeSel : 2; 
		int userRangeSel = getMeasurementGroupSelected(CMsg::load(IDS_GROUP_COLOR_USER_RANGES)); 
		CColorTest colorDlg(m_colorCalibrationFile, m_Observer, m_slope_l, m_yint_l, m_slope_a, m_yint_a, m_slope_b, m_yint_b, 
			basicSel, defaultRangeSel, userRangeSel); 
		if (colorDlg.DoModal() == IDOK) 
		{
			// Save settings
			m_Observer = colorDlg.getIlluminantObserver(); 
			if (colorDlg.getSelectedAction() != colorDlg.CALIBRATE) {
				m_slope_l = colorDlg.getLSlope(); 
				m_yint_l = colorDlg.getLIntercept(); 
				m_slope_a = colorDlg.getASlope(); 
				m_yint_a = colorDlg.getAIntercept(); 
				m_slope_b = colorDlg.getBSlope(); 
				m_yint_b = colorDlg.getBIntercept(); 
				if (colorDlg.isCalibrationReset()) {
					m_colorCalibrationFile.Empty(); 
				} else if (colorDlg.isCalibrationChanged()) {
					m_colorCalibrationFile.Format("Manually entered equations"); 
				}
			}
			
			// Calibrate or analyze color
			if (colorDlg.getSelectedAction() == colorDlg.CALIBRATE || colorDlg.getSelectedAction() == colorDlg.ANALYZE) 
			{
				CTomatoAnalyzerDoc * pDoc = GetCurrentDoc(); 

				if (colorDlg.getSelectedAction() == colorDlg.CALIBRATE) 
				{
					if (pDoc->CalibrateColor(m_slope_l, m_yint_l, m_slope_a, m_yint_a, m_slope_b, m_yint_b, m_Observer)) {
						m_colorCalibrationFile = pDoc->m_ImageFileName; 
					}
					justCalibrated = true; 
				} 
				else 
				{
					// Set tabs in report
					vector<ColorRange> lRanges, hueRanges, chromaRanges, userRanges; 
					userRangeSel = colorDlg.getUserRanges(); 
					if (userRangeSel != 0) {
						ColorRangeDialog rangeDlg(m_userRanges); 
						if (rangeDlg.DoModal() == IDOK) {
							userRanges = m_userRanges = rangeDlg.getRanges(); 
							m_measurementSet.clearGroup(CMsg::load(IDS_GROUP_COLOR_USER_RANGES)); 
							for (vector<ColorRange>::iterator i = userRanges.begin(); i != userRanges.end(); i++) {
								m_measurementSet.addMeasurement(i->toString(), CMsg::load(IDS_GROUP_COLOR_USER_RANGES)); 
							}
						} else {
							cancelledRanges = true; 
						}
					}
					if (! cancelledRanges) { 
						setMeasurementGroupSelected(CMsg::load(IDS_GROUP_COLOR_USER_RANGES), userRangeSel); 
						basicSel = colorDlg.getBasic(); 
						if (basicSel != 2) {
							setMeasurementGroupSelected(CMsg::load(IDS_GROUP_COLOR_ANALYSIS), basicSel); 
						}
						defaultRangeSel = colorDlg.getDefaultRanges(); 
						if (defaultRangeSel != 0) {
							lRanges = m_lRanges; 
							hueRanges = m_hueRanges; 
							chromaRanges = m_chromaRanges; 
						}
						if (defaultRangeSel != 2) {
							setMeasurementGroupSelected(CMsg::load(IDS_GROUP_L_RANGES), defaultRangeSel); 
							setMeasurementGroupSelected(CMsg::load(IDS_GROUP_HUE_RANGES), defaultRangeSel); 
							setMeasurementGroupSelected(CMsg::load(IDS_GROUP_CHROMA_RANGES), defaultRangeSel); 
						}

						pDoc->AnalyzeColor(m_slope_l, m_yint_l, m_slope_a, m_yint_a, m_slope_b, m_yint_b, m_Observer, 
							lRanges, hueRanges, chromaRanges, userRanges); 
						pDoc->UpdateAllViews(NULL, DOC_UPDATEVIEW_MEASURECHANGED); 
					}
				}
			}
		}
	} while (justCalibrated || cancelledRanges);	
	// If user chose Calibrate, return to the Color Test dialog after calibrating. 
	// If user clicked Cancel in User-Defined Color Ranges dialog, return to the Color Test dialog. 
}

/** 
 * Displays a dialog to add user-created measurements. Each measurement is added to the measurement set, 
 * and empty data values for all added measurements are added to each slice. 
 */
void CTomatoAnalyzerApp::OnCmdAddMeasure()
{
	CString newline = "\r\n";
	// Creates an object of the MFC dialog class
	UserDefinedDialog dlg;
	dlg.text1 += newline;
	POSITION pos;
	std::vector<CString> UCList = m_measurementSet.measurementsInGroup(CMsg::load(IDS_GROUP_USER_CREATED));
	for(int i = 0; i < UCList.size(); i++)
	{
		if(i % 6 == 0 && i == 0)
			dlg.text1 += UCList[i];
		else if (i % 6 == 0)
			dlg.text1 += newline + UCList[i];
		else
			dlg.text1 += ", " + UCList[i];
	}

	// If the button has been pressed, then the dialog window appears
	do
	{
		dlg.numUserCols = userCounter;
		dlg.DoModal();

		// Grabs the data out of the the text fields
		CString userMeasure[] = {dlg.box1, dlg.box2, dlg.box3};

		// Stuff that is standard in all dialog windows
		pos = GetFirstDocTemplatePosition();
		if(NULL == pos)
			return;

		CDocTemplate * pDocTmpl = GetNextDocTemplate(pos);
		pos = pDocTmpl->GetFirstDocPosition();
		CTomatoAnalyzerDoc * pDoc = (CTomatoAnalyzerDoc *) pDocTmpl->GetNextDoc(pos);
		if(NULL != pDoc)
		{
			std::vector<POSITION> slicePositions;
			pDoc->GetSelectedSliceByNaturalSequence(slicePositions);
			if (userCounter != maxUserCreated)
			{
				// Goes through all three text fields
				for(int i = 0; i < 3; i++)
				{
					// If the field is not empty, then it adds the new measurement to the vector
					if(userMeasure[i].CompareNoCase("") != 0)
					{
						if(userCounter != maxUserCreated)
						{
							/*char * buffer = new char[userMeasure[i].GetLength() + 1]; 
							strcpy(buffer, userMeasure[i].GetBuffer());
							szMeasurementTemp.push_back(buffer);
							createdCopy.push_back(buffer);*/

							// Add to measurement set
							m_measurementSet.addMeasurement(userMeasure[i], CMsg::load(IDS_GROUP_USER_CREATED)); 

							if(userCounter % 6 == 0 && userCounter == 0)
								dlg.text1 = dlg.text1 + userMeasure[i];
							else if (userCounter % 6 == 0)
								dlg.text1 = dlg.text1 + newline + userMeasure[i];
							else
								dlg.text1 += ", " + userMeasure[i];
							userCounter++;

							// Add to each slice - measurement is added with an empty data value
							for (std::vector<POSITION>::iterator iter = slicePositions.begin(); iter != slicePositions.end(); iter++) {
								pDoc->GetSlice(*iter)->setUserCreatedData(userMeasure[i], ""); 
							}
						}
					}
				}
			}
		}

		dlg.box1 = "";
		dlg.box2 = "";
		dlg.box3 = "";
	}
	while (dlg.finished != true);

	pos = GetFirstDocTemplatePosition();
	CDocTemplate * pDocTmpl = GetNextDocTemplate(pos);
	pos = pDocTmpl->GetFirstDocPosition();
	CTomatoAnalyzerDoc * pDoc = (CTomatoAnalyzerDoc *) pDocTmpl->GetNextDoc(pos);
	pDoc->UpdateAllViews(NULL);

/*
	if(userCounter > 0)
	{
		CMenu menu;
		menu.LoadMenuA(IDR_MAINFRAME);
		CMenu * settingsMenu = menu.GetSubMenu(4);
		settingsMenu->EnableMenuItem(IDC_DATA_ADDINFO, MF_BYCOMMAND | MF_ENABLED);

		delete settingsMenu;
		menu.DestroyMenu();
	}*/
}

	/**
	 * Function that is called when "Add/Edit Data" is clicked.
	 */
// Handling method for the "Add/Edit Data..." dialog box

/** 
 * Displays a dialog to add/edit data for each user-created measurement for each slice. 
 */
void CTomatoAnalyzerApp::OnCmdAddData()
{
	// Standard stuff to get the number of slices
	POSITION pos = GetFirstDocTemplatePosition();
    if(NULL == pos)
		return;
	CDocTemplate * pDocTmpl = GetNextDocTemplate(pos);
    pos = pDocTmpl->GetFirstDocPosition();
    CTomatoAnalyzerDoc * pDoc = (CTomatoAnalyzerDoc *) pDocTmpl->GetNextDoc(pos);
	
	// Measurements must have been added and there must be slices selected to use Add/Edit Data
	int count = pDoc->m_SliceList.GetCount();
	std::vector<CString> userCreatedMeasurements = m_measurementSet.measurementsInGroup(CMsg::load(IDS_GROUP_USER_CREATED)); 
	if(userCreatedMeasurements.size() != 0 && count != 0)
	{
		// Get the slices in order
		std::vector<POSITION> slicePositions; 
		pDoc->GetAllSlicesByNaturalSequence(slicePositions); 

		// Since the data is not assigned, grab total number of user-created columns
		int cols = userCreatedMeasurements.size();
		// If the data structure has not been initialized, then initialize it
		/*if(!set)
		{
			// Initializes the multidimensional vector, indexing as vector_name[col][row] (to suit the dynamic aspect of the data better)
			for(int i = 0; i < cols; i++)
			{
				std::vector<CString> indivRow;
				for(int j = 0; j < count; j++)
				{
					indivRow.push_back("");
				}
				userData.push_back(indivRow);
			}

			// This block is no longer executed while the program is running (will eventually not execute after the data is loaded (in later revision)
			set = true;
		}*/

		// Creates a dialog object using the overloaded constructor
		UserAddData dlg(NULL, userCreatedMeasurements, count, pDoc);

		// Declares three holding variables for while the program is running
		int row = 0;
		int col = 0;
		int ret = -1;

		// do-while loop ensures that the first section will always run once
		do
		{
			// Makes the combo boxes increment and loop around
			if(ret != -1)
			{
				if (row == (count - 1))
				{
					row = 0;
					if(col == (cols - 1))
					{
						col = 0;
					}
					else
						col++;
				}
				else
				{
					row++;
				}
			}
			else
			{
			}

			// Pre-dialog appearance code

			// Loads in the last-entered data
			//if(userData[col][row].Compare("") != 0)
			//{
			//	dlg.box1.Format(_T("%s"), userData[col][row]);
			//}
			// If the data is empty, then load a blank
			//else
			//	dlg.box1.Format(_T("%s"), "");

			dlg.m_chNum = col;
			dlg.m_fnNum = row;

			//----------------------

			// Makes the dialog appear
			ret = dlg.DoModal();
			// Window Position "memory" placement GOES HERE

			// col and row grab the index values from the combo boxes
			col = dlg.m_chNum;
			row = dlg.m_fnNum;

			// If the user has entered a blank, store a blank
			//if(dlg.box1.Compare("") == 0)
			//{
			//	userData[col][row] = "";
			//}
			// Otherwise, store what they typed
			//else
			//{
			//	userData[col][row] = dlg.box1;
			//}
			pDoc->GetSlice(slicePositions[row])->setUserCreatedData(userCreatedMeasurements[col], dlg.box1);
		}
		while (dlg.finished != true); // Loop continues until Finish is pressed
	}

	pDoc->UpdateAllViews(NULL);
}


/**
 * Sets the default internal ellipse size, can be modified under the settings menu
 */
void CTomatoAnalyzerApp::OnSetEllipseSize()
{
	EllipseSize dlg;
	
	CTomatoAnalyzerDoc * pDoc = GetCurrentDoc(); 
	if (NULL == pDoc) 
		return; 
	std::vector<POSITION> vctPos;
	pDoc->GetSelectedSliceByNaturalSequence(vctPos);
	int nSelectedSlices = vctPos.size();	// how many slices are being analyzed?
	if (nSelectedSlices > 0)
	{
		std::vector<POSITION>::iterator  Iter = vctPos.begin();
		CSlice * pSlice = pDoc->GetSlice(*Iter);
		int tempSize = pSlice->m_ellipseSize;
		if (tempSize > 0 && tempSize <= 100)
			m_ellipseSize = tempSize;
	}

	dlg.Ellipse_Value = m_ellipseSize;
	
	if (dlg.DoModal() == TRUE)
	{
		if (dlg.clickSave)
		{
			m_ellipseSize = dlg.Ellipse_Value;

			CTomatoAnalyzerDoc * pDoc = GetCurrentDoc(); 
			if (NULL == pDoc)
				return; 

			std::vector<POSITION> vctPos;
			pDoc->GetSelectedSliceByNaturalSequence(vctPos);

			int nSelectedSlices = vctPos.size();	// how many slices are being analyzed?
			std::vector<POSITION>::iterator  Iter = vctPos.begin();
				
			for (int i = 0; i < nSelectedSlices; i++)
			{
				CSlice *  pSlice = pDoc->GetSlice(*Iter);
				pSlice->SetEllipseManual(false);
				//if (!(pSlice->m_manualEllipse))
				pSlice->SetEllipseDefault();
				//pSlice->findInnerBoundary();
				++Iter;
			}
			if(NULL != pDoc)
				pDoc->UpdateAllViews(NULL);
		}
	}
}

/**
 * Creates a dialog box with a slider to allow the user to choose the number of points used for morphometrics. 
 */
void CTomatoAnalyzerApp::OnSavePCAPoints()
{
	PCAPOINTS dlg;		//create dialog box to choose number of points
	dlg.PCA_Value = m_numPCAPoints;
	
	if(dlg.DoModal() == TRUE){	//if open

		if(dlg.clickSave){		//if user clicks "save"
			m_numPCAPoints = dlg.PCA_Value;		//global for number of points

			CTomatoAnalyzerDoc * pDoc = GetCurrentDoc(); 
			if (NULL == pDoc)
				return; 

			std::vector<POSITION>   vctPos;
			pDoc->GetSelectedSliceByNaturalSequence(vctPos);

			int nSelectedSlices = vctPos.size();	// how many slices are being analyzed?
			std::vector<POSITION>::iterator  Iter = vctPos.begin();
			
			//Change the points when the slider for # of points is adjusted
			for (int i = 0; i < nSelectedSlices; i++)
			{
				CSlice *  pSlice = pDoc->GetSlice(*Iter);
				pSlice->CalcNewPCA();
				++Iter;
			}

			// Since it's a lot of clicks to select/deselect morphometrics for display, do this automatically. 
			setMeasurementGroupSelected(CMsg::load(IDS_GROUP_MORPHOMETRICS), false); 
			for (int i = 1; i <= m_numPCAPoints; i++) {
				CString s; 
				s.Format("%dx", i); 
				m_vctSaveDataItem.push_back( m_measurementSet.measurementID(s) ); 
				s.Format("%dy", i); 
				m_vctSaveDataItem.push_back( m_measurementSet.measurementID(s) ); 
			}

			if(NULL != pDoc)
				pDoc->UpdateAllViews(NULL, DOC_UPDATEVIEW_MEASURECHANGED);
		}

	}
}

/// Not used? 
void CTomatoAnalyzerApp::OnCmdSettings()
{
	CParameterSettings dlg;

	dlg.SetParameters(m_nShapeIndex);
	dlg.m_shapeThreshold = g_shapethresh;
	dlg.DoModal();
	dlg.GetParameter(m_nShapeIndex);
	g_shapethresh = dlg.m_shapeThreshold;
	ChangeShapeIndex();
	return;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

/**
 * Displays About dialog
 */
void CTomatoAnalyzerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/**
 * Displays Disclaimer dialog
 */
void CTomatoAnalyzerApp::OnAppDisclaimer()
{
	CStartUpDialog dlg;
	dlg.DoModal();
}

/** 
 * Displays dialog for macro/micro distance settings. 
 */
void CTomatoAnalyzerApp::OnCmdPointiLevelControl()
{
    CPointyLevel    pc;
    pc.m_nMacroLevel = m_nMac;
    pc.m_nMicroLevel = m_nMic;
    if(IDOK == pc.DoModal())
    {
        m_nMac = pc.m_nMacroLevel;
        m_nMic = pc.m_nMicroLevel;
    }
    
	CTomatoAnalyzerDoc* pDoc = GetCurrentDoc(); 

	POSITION pos = pDoc->m_SliceList.GetHeadPosition();
	while(pos !=NULL)
	{
		CSlice* pSlice = pDoc->m_SliceList .GetNext(pos);
		CPixelList *    pBorder = pSlice->GetBoundary(1);
		CPixel p = pBorder->GetAt(pSlice->m_iTipBlm);
		POINT po;
		po.x = p.GetX();
		po.y = p.GetY();
		pSlice->metric_IndentationAngle();
		pSlice->metric_ProximalAngle();
	} 

	pDoc->UpdateAllViews (NULL);
    return;
}


// CTomatoAnalyzerApp message handlers

/// Not used? 
void CTomatoAnalyzerApp::ChangeShapeIndex()
{
	CTomatoAnalyzerDoc* pDoc = GetCurrentDoc(); 

	POSITION pos = pDoc->GetSelectedObject();
	if(NULL == pos)
		return;

	pos = pDoc->m_SliceList.GetHeadPosition();
	while(pos !=NULL)
	{
		CSlice* pSlice = pDoc->m_SliceList .GetNext(pos);
	}

	pDoc->UpdateAllViews (NULL);

}

/**
 * Adds all the default measurements to the measurement set, plus an empty measurement group for user-created
 * measurements. A measurement group must be added here if it has an icon in Resources -> Bitmap -> IDB_LEGEND
 * for its column in the report; otherwise, the icons will end up on the wrong columns. 
 */
void CTomatoAnalyzerApp::initMeasurementSet() {

	CString aBasic[] = {CMsg::load(IDS_MEAS_PERIMETER), CMsg::load(IDS_MEAS_AREA), CMsg::load(IDS_MEAS_WIDTH_MIDHEIGHT), 
		CMsg::load(IDS_MEAS_MAXIMUM_WIDTH), CMsg::load(IDS_MEAS_HEIGHT_MIDWIDTH), CMsg::load(IDS_MEAS_MAXIMUM_HEIGHT), 
		CMsg::load(IDS_MEAS_CURVED_LENGTH)}; 
	std::vector<CString> vBasic(aBasic, aBasic + 7); // there are 7 elements in the array  
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_BASIC_MEASUREMENTS), vBasic); 

	CString aShape[] = {CMsg::load(IDS_MEAS_FRUIT_SHAPE_INDEX_EXTERNAL_I), 
		CMsg::load(IDS_MEAS_FRUIT_SHAPE_INDEX_EXTERNAL_II), CMsg::load(IDS_MEAS_CURVED_FRUIT_SHAPE_INDEX)}; 
	std::vector<CString> vShape(aShape, aShape + 3); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_FRUIT_SHAPE_INDEX), vShape); 

	CString aBlock[] = {CMsg::load(IDS_MEAS_PROXIMAL_FRUIT_BLOCKINESS), CMsg::load(IDS_MEAS_DISTAL_FRUIT_BLOCKINESS), 
		CMsg::load(IDS_MEAS_FRUIT_SHAPE_TRIANGLE)}; 
	std::vector<CString> vBlock(aBlock, aBlock + 3); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_BLOCKINESS), vBlock); 

	CString aHomog[] = {CMsg::load(IDS_MEAS_ELLIPSOID), CMsg::load(IDS_MEAS_CIRCULAR), CMsg::load(IDS_MEAS_RECTANGULAR)};  
	std::vector<CString> vHomog(aHomog, aHomog + 3); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_HOMOGENEITY), vHomog); 

	CString aProx[] = {CMsg::load(IDS_MEAS_SHOULDER_HEIGHT), CMsg::load(IDS_MEAS_PROXIMAL_ANGLE_MICRO), 
		CMsg::load(IDS_MEAS_PROXIMAL_ANGLE_MACRO), CMsg::load(IDS_MEAS_PROXIMAL_INDENTATION_AREA)}; 
	std::vector<CString> vProx(aProx, aProx + 4); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_PROXIMAL_FRUIT_END_SHAPE), vProx); 

	CString aDist[] = {CMsg::load(IDS_MEAS_DISTAL_ANGLE_MICRO), CMsg::load(IDS_MEAS_DISTAL_ANGLE_MACRO), 
		CMsg::load(IDS_MEAS_DISTAL_INDENTATION_AREA), CMsg::load(IDS_MEAS_DISTAL_END_PROTRUSION)}; 
	std::vector<CString> vDist(aDist, aDist + 4); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_DISTAL_FRUIT_END_SHAPE), vDist); 

	CString aAsym[] = {CMsg::load(IDS_MEAS_OBOVOID), CMsg::load(IDS_MEAS_OVOID), CMsg::load(IDS_MEAS_V_ASYMMETRY), 
		CMsg::load(IDS_MEAS_H_ASYMMETRY_OBOVOID), CMsg::load(IDS_MEAS_H_ASYMMETRY_OVOID), CMsg::load(IDS_MEAS_WIDTH_WIDEST_POS)}; 
	std::vector<CString> vAsym(aAsym, aAsym + 6); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_ASYMMETRY), vAsym); 

	CString aEcc[] = {CMsg::load(IDS_MEAS_ECCENTRICITY), CMsg::load(IDS_MEAS_PROXIMAL_ECCENTRICITY), 
		CMsg::load(IDS_MEAS_DISTAL_ECCENTRICITY), CMsg::load(IDS_MEAS_FRUIT_SHAPE_INDEX_INTERNAL), 
		CMsg::load(IDS_MEAS_ECCENTRICITY_AREA_INDEX)}; 
	std::vector<CString> vEcc(aEcc, aEcc + 5); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_INTERNAL_ECCENTRICITY), vEcc); 

	CString aLat[] = {CMsg::load(IDS_MEAS_LOBEDNESS_DEGREE), CMsg::load(IDS_MEAS_PERICARP_AREA), CMsg::load(IDS_MEAS_PERICARP_AREA_RATIO), 
		CMsg::load(IDS_MEAS_PERICARP_THICKNESS), CMsg::load(IDS_MEAS_PERICARP_THICKNESS_RATIO), CMsg::load(IDS_MEAS_PERICARP_BOUNDARY_LENGTH),
	    CMsg::load(IDS_MEAS_REAL_PERICARP_AREA), CMsg::load(IDS_PEPPER_PERICARP_AREA_RATIO), CMsg::load(IDS_MEAS_REAL_PERICARP_THICKNESS),
	    CMsg::load(IDS_PEPPER_PERICARP_THICKNESS_RATIO)}; 
	std::vector<CString> vLat(aLat, aLat + 10); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_LATITUDINAL_SECTION), vLat); 

	std::vector<CString> vMorph(2 * 200); 
	for (int i = 0; i < vMorph.size() / 2; i++) {
		CString s; 
		s.Format("%dx", i + 1); 
		vMorph[2*i] = s; 
		s.Format("%dy", i + 1); 
		vMorph[2*i + 1] = s; 
	}
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_MORPHOMETRICS), vMorph); 

	CString aColor[] = {CMsg::load(IDS_MEAS_AVERAGE_RED), CMsg::load(IDS_MEAS_AVERAGE_GREEN), CMsg::load(IDS_MEAS_AVERAGE_BLUE), 
		CMsg::load(IDS_MEAS_AVERAGE_LUMINOSITY), CMsg::load(IDS_MEAS_AVERAGE_L_VALUE), CMsg::load(IDS_MEAS_AVERAGE_A_VALUE), 
		CMsg::load(IDS_MEAS_AVERAGE_B_VALUE), CMsg::load(IDS_MEAS_AVERAGE_HUE), CMsg::load(IDS_MEAS_AVERAGE_CHROMA)}; 
	std::vector<CString> vColor(aColor, aColor + 9); 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_COLOR_ANALYSIS), vColor); 

	std::vector<CString> vLRanges; 
	for (int i = 0; i <= 100; i += 10) {
		ColorRange r(i, i + 10, -1, -1, -1, -1, false, true, true); 
		m_lRanges.push_back(r); 
		vLRanges.push_back(r.toStringL()); 
	}
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_L_RANGES), vLRanges); 
	std::vector<CString> vHueRanges; 
	for (int i = 0; i < 360; i += 30) {
		ColorRange r(-1, -1, i, i + 30, -1, -1, true, false, true); 
		m_hueRanges.push_back(r); 
		vHueRanges.push_back(r.toStringHue()); 
	}
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_HUE_RANGES), vHueRanges); 
	std::vector<CString> vChromaRanges; 
	for (int i = 0; i < 140; i += 10) {
		ColorRange r(-1, -1, -1, -1, i, i + 10, true, true, false); 
		m_chromaRanges.push_back(r); 
		vChromaRanges.push_back(r.toStringChroma()); 
	}
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_CHROMA_RANGES), vChromaRanges); 

	std::vector<CString> empty; 
	m_measurementSet.addGroup(CMsg::load(IDS_GROUP_COLOR_USER_RANGES), empty); 
	//m_measurementSet.addGroup(CMsg::load(IDS_GROUP_USER_CREATED), empty); 
}

/**
 * This has the same effect as opening the Select Attributes dialog and checking (select) or unchecking (!select) 
 * the box for the given group. All measurements in the given group (name) are either selected or deselected
 * for display. 
 */
void CTomatoAnalyzerApp::setMeasurementGroupSelected(CString name, bool select) {
	int groupID = m_measurementSet.groupID(name); 
	if (groupID != -1) {
		if (select) {
			std::vector<CString> meas = m_measurementSet.measurementsInGroup(name); 
			for (std::vector<CString>::iterator i = meas.begin(); i != meas.end(); i++) {
				int id = m_measurementSet.measurementID(*i); 
				if (std::find(m_vctSaveDataItem.begin(), m_vctSaveDataItem.end(), id) == m_vctSaveDataItem.end()) {
					m_vctSaveDataItem.push_back(id); 
				}
			}
		} else {
			for (int i = m_vctSaveDataItem.size() - 1; i >= 0; i--) {
				if (m_measurementSet.groupForMeasurement( m_vctSaveDataItem[i] ) == groupID) {
					m_vctSaveDataItem.erase(m_vctSaveDataItem.begin() + i); 
				}
			}
		}
	}
}

/** 
 * Returns a value that corresponds to the checkbox value next to the given group in the Select Attributes dialog: 
 * 0 if no measurements in the group are selected or the group is empty, 1 if all are selected, or 2 if some are selected. 
 */
int CTomatoAnalyzerApp::getMeasurementGroupSelected(CString name) {
	int groupID = m_measurementSet.groupID(name); 
	if (groupID != -1) {
		bool allSelected = true, 
			someSelected = false; 
		std::vector<CString> meas = m_measurementSet.measurementsInGroup(name); 
		if (meas.empty()) {
			return 0; 
		}
		for (std::vector<CString>::iterator i = meas.begin(); i != meas.end(); i++) {
			int id = m_measurementSet.measurementID(*i); 
			if (std::find(m_vctSaveDataItem.begin(), m_vctSaveDataItem.end(), id) == m_vctSaveDataItem.end()) {
				allSelected = false; 
			} else {
				someSelected = true; 
			}
		}
		if (allSelected) {
			return 1; 
		} else if (someSelected) {
			return 2; 
		}
	}
	return 0; 
}

/**
 * Deselects and removes the given measurement group, making it unavailable to the user. 
 */
void CTomatoAnalyzerApp::removeMeasurementGroup(CString name) 
{ 
	int groupID = m_measurementSet.groupID(name); 
	setMeasurementGroupSelected(name, false); 
	m_measurementSet.removeGroup(name); 
}

/** 
 * Edits/Removes User-Created columns from the measurement set (list of all column headings). 
 * If remEdit is true, removes the User-Created measurement whose measurement ID is 'index' more than
 * the ID of the first User-Created measurement. Otherwise, replaces that measurement with 'newStr'. 
 */
void CTomatoAnalyzerApp::editMeasurements(bool remEdit, CString newStr, int index)
{
	std::vector<CString> userCols = m_measurementSet.measurementsInGroup(CMsg::load(IDS_GROUP_USER_CREATED));
	POSITION pos = GetFirstDocTemplatePosition();
	CDocTemplate * pDocTmpl = GetNextDocTemplate(pos);
	pos = pDocTmpl->GetFirstDocPosition();
	CTomatoAnalyzerDoc * pDoc = (CTomatoAnalyzerDoc *) pDocTmpl->GetNextDoc(pos);
	std::vector<POSITION> slicePositions; 
	pDoc->GetAllSlicesByNaturalSequence(slicePositions);
	// If the user is removing measurements, use this block
	if(remEdit)
	{
		m_measurementSet.removeMeasurement(userCols[index]);
		
		for(std::vector<POSITION>::iterator iter = slicePositions.begin(); iter != slicePositions.end(); iter++)
		{
			pDoc->GetSlice(*iter)->removeUserCreatedData(userCols[index]);
		}
	}
	// Otherwise, the measurement is only being fixed
	else
	{
		m_measurementSet.modifyMeasurement(userCols[index], newStr);

		for(std::vector<POSITION>::iterator iter = slicePositions.begin(); iter != slicePositions.end(); iter++)
		{
			pDoc->GetSlice(*iter)->editUserCreatedData(userCols[index], newStr);
		}
	}
}

/** 
 * Removes all User-Created measurement columns. This is used during serialization (loading data from the TMT). 
 */
void CTomatoAnalyzerApp::ClearUserMeasurements()
{
	std::vector<CString> clearMeasure = m_measurementSet.measurementsInGroup(CMsg::load(IDS_GROUP_USER_CREATED));

	if(clearMeasure.size() > 0)
	{
		for(int i = 0; i < clearMeasure.size(); i++)
		{
			m_measurementSet.removeMeasurement(clearMeasure[i]);
		}
	}
}

/**
 * Adds a new User-Created measurement column with the given name. 
 * This is used during serialization (loading data from the TMT). 
 */
void CTomatoAnalyzerApp::LoadUserMeasurements(CString userCol)
{
	m_measurementSet.addMeasurement(userCol, CMsg::load(IDS_GROUP_USER_CREATED));
}

/**
 * Returns a pointer to the current open document, or NULL if there is none. 
 */
CTomatoAnalyzerDoc * CTomatoAnalyzerApp::GetCurrentDoc() {
	POSITION pos = GetFirstDocTemplatePosition();
	if(NULL == pos)
		return NULL;

	CDocTemplate * pDocTmpl = GetNextDocTemplate(pos);
	pos = pDocTmpl->GetFirstDocPosition();
	CTomatoAnalyzerDoc * pDoc = (CTomatoAnalyzerDoc *) pDocTmpl->GetNextDoc(pos);
	return pDoc; 
}


void CTomatoAnalyzerApp::OnBnClickedManualDpi() {

}
