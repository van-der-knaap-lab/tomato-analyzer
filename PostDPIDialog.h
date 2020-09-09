#pragma once


// PostDPIDialog dialog

class PostDPIDialog : public CDialog
{
	DECLARE_DYNAMIC(PostDPIDialog)

public:
	PostDPIDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~PostDPIDialog();

// Dialog Data
	enum { IDD = IDD_POST_DPI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeNumUnits();
	double get_num_units();

private:
	double num_units;
};
