// PostDPIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "PostDPIDialog.h"
#include "afxdialogex.h"
#include <cstring>
#include <iostream>
#include <fstream>


// PostDPIDialog dialog

IMPLEMENT_DYNAMIC(PostDPIDialog, CDialog)

PostDPIDialog::PostDPIDialog(CWnd* pParent /*=NULL*/)
	: CDialog(PostDPIDialog::IDD, pParent)
{
	num_units = 0;
}

PostDPIDialog::~PostDPIDialog()
{
}

void PostDPIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NUM_UNITS, num_units);
}


BEGIN_MESSAGE_MAP(PostDPIDialog, CDialog)
	ON_EN_CHANGE(IDC_NUM_UNITS, &PostDPIDialog::OnEnChangeNumUnits)
END_MESSAGE_MAP()


// PostDPIDialog message handlers


void PostDPIDialog::OnEnChangeNumUnits()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

double PostDPIDialog::get_num_units() {
	/*
	double units = 0;

	auto text = (CEdit*) (this->m_hWnd, IDC_NUM_UNITS);

	if (text == NULL) {
		exit(0);	
	}
	std::ofstream ofs;
	ofs.open("C:\\Users\\madmax\\test.txt", std::ofstream::out | std::ofstream::app);

	char *buffer = new char[text->GetWindowTextLength() + 1];
	memset(buffer, 0, text->GetWindowTextLength() + 1);

	CString foo;
	text->GetWindowTextA(buffer, text->GetWindowTextLengthA());

	GetDlgItemText(IDC_NUM_UNITS, foo);
	ofs << num_units << std::endl;
	ofs.close();

	units = atof(buffer);

	delete[] buffer;

	return units;*/
	return num_units;
}