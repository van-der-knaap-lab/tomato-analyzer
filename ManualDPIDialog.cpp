// ManualDPIDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "ManualDPIDialog.h"
#include "afxdialogex.h"


// ManualDPIDialog dialog

IMPLEMENT_DYNAMIC(ManualDPIDialog, CDialog)

ManualDPIDialog::ManualDPIDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ManualDPIDialog::IDD, pParent)
{

}

ManualDPIDialog::~ManualDPIDialog()
{
}

void ManualDPIDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ManualDPIDialog, CDialog)
		ON_CBN_SELCHANGE(IDC_MEASURE_BOX, &ManualDPIDialog::OnCbnSelchangeMeasureBox)
		ON_BN_CLICKED(IDCANCEL, &ManualDPIDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// ManualDPIDialog message handlers
void ManualDPIDialog::OnCbnSelchangeMeasureBox() {
	// This source is terrible.
	// If you're reading this, the whole project needs to be rewritten.
	// I do these things here because they're done throughout the project and I need this stuff to work __now__.
	char text_buffer[256] = {0};
	CComboBox *box = (CComboBox*) GetDlgItem(IDC_MEASURE_BOX);
	box->GetLBText(box->GetCurSel(), text_buffer);

	if (strcmp(text_buffer, "Inches") == 0) {
		this->measurement = INCHES;
	} else if (strcmp(text_buffer, "Pixels") == 0) {
		this->measurement = PIXELS;
	} else if (strcmp(text_buffer, "Centimeters") == 0) {
		this->measurement = CENTIMETERS;
	} else {	
		this->measurement = MILLIMETERS;
	}
}

void ManualDPIDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	this->measurement = CANCEL;
	CDialog::OnCancel();
}
