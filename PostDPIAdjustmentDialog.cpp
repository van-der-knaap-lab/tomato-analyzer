// PostDPIAdjustmentDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "PostDPIAdjustmentDialog.h"
#include "afxdialogex.h"


// PostDPIAdjustmentDialog dialog

IMPLEMENT_DYNAMIC(PostDPIAdjustmentDialog, CDialog)

PostDPIAdjustmentDialog::PostDPIAdjustmentDialog(CWnd* pParent /*=NULL*/)
	: CDialog(PostDPIAdjustmentDialog::IDD, pParent)
{
	this->scale = 1;
}

PostDPIAdjustmentDialog::~PostDPIAdjustmentDialog()
{
}

void PostDPIAdjustmentDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

double PostDPIAdjustmentDialog::getScale() {
	return scale;
}


BEGIN_MESSAGE_MAP(PostDPIAdjustmentDialog, CDialog)
END_MESSAGE_MAP()


// PostDPIAdjustmentDialog message handlers
