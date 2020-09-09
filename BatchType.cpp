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
// BatchType.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "BatchType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BatchType dialog


BatchType::BatchType(CWnd* pParent /*=NULL*/)
	: CDialog(BatchType::IDD, pParent)
{
	EnableAutomation();

	//{{AFX_DATA_INIT(BatchType)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void BatchType::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialog::OnFinalRelease();
}

void BatchType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BatchType)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BatchType, CDialog)
	//{{AFX_MSG_MAP(BatchType)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(BatchType, CDialog)
	//{{AFX_DISPATCH_MAP(BatchType)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IBatchType to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {B4F165BB-EF0D-4A31-AC9F-845685F8E6A5}
static const IID IID_IBatchType =
{ 0xb4f165bb, 0xef0d, 0x4a31, { 0xac, 0x9f, 0x84, 0x56, 0x85, 0xf8, 0xe6, 0xa5 } };

BEGIN_INTERFACE_MAP(BatchType, CDialog)
	INTERFACE_PART(BatchType, IID_IBatchType, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BatchType message handlers
