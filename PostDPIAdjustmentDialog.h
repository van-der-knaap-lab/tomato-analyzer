#pragma once


// PostDPIAdjustmentDialog dialog

class PostDPIAdjustmentDialog : public CDialog
{
	DECLARE_DYNAMIC(PostDPIAdjustmentDialog)
	double scale;

public:
	PostDPIAdjustmentDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~PostDPIAdjustmentDialog();

// Dialog Data
	enum { IDD = IDD_MANUAL_DPI_SCALE_DIALOG };

	double getScale();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
