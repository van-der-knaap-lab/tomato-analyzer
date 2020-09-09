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
// PointiLevelGraph.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "PointiLevelGraph.h"
#include "new_img_function.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointiLevelGraph

CPointiLevelGraph::CPointiLevelGraph()
{
    m_nMin = 25;
    m_nMax = 50;
    m_pEllipse = NULL;
}

CPointiLevelGraph::~CPointiLevelGraph()
{
	if(m_pEllipse != NULL)
        delete m_pEllipse;
}


BEGIN_MESSAGE_MAP(CPointiLevelGraph, CStatic)
	//{{AFX_MSG_MAP(CPointiLevelGraph)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointiLevelGraph message handlers

void CPointiLevelGraph::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	DrawFigure(&dc);

	// Do not call CStatic::OnPaint() for painting messages
}


void CPointiLevelGraph::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_pEllipse != NULL)
        delete m_pEllipse;

	RECT    rect;
    GetClientRect(&rect);

    int cx = (rect.right >> 1);
    int cy = (rect.bottom >> 1);
	CPixel center(cx, cy); 
    m_pEllipse = ellipse(center, cx, cy, 0);
	
	CStatic::PreSubclassWindow();
}


void CPointiLevelGraph::SetRange(int nMin, int nMax)
{
    m_nMin = nMin;
    m_nMax = nMax;

    CClientDC   dc(this);
    DrawFigure(&dc);
}

void CPointiLevelGraph::DrawFigure(CDC* pdc)
{
	if(m_pEllipse==NULL)
		return;

    pdc->SelectObject(GetStockObject(NULL_PEN));
    
    CBrush  ba, bi;
    COLORREF colMac = RGB(255, 0, 0);
    ba.CreateSolidBrush(colMac);

    COLORREF colMic = RGB(0, 255, 0);
    bi.CreateSolidBrush(colMic);

	int nLength = m_pEllipse->size();
    int onequarter = (nLength>>2);
    std::vector<IPTPOINT>::iterator itr = m_pEllipse->begin();
    
    int nMacroNum = nLength * m_nMax / 200 ;
    int nMicroNum = nLength * m_nMin / 200 ;

    int nx = 1;
    pdc->SelectObject(GetStockObject(BLACK_BRUSH));
    int nTemp = onequarter - nMacroNum;
	int i; 
    for(i=0; i<nTemp; i++)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
    }

    pdc->SelectObject(&ba);
    nTemp = onequarter - nMicroNum;
    while(i < nTemp)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
        ++i;
    }

    pdc->SelectObject(&bi);
    nTemp = onequarter + nMicroNum;
    while(i < nTemp)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
        ++i;
    }

    pdc->SelectObject(&ba);
    nTemp = onequarter + nMacroNum;
    while(i < nTemp)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
        ++i;
    }

    pdc->SelectObject(GetStockObject(BLACK_BRUSH));
    int onehalf = (nLength>>1);
    nTemp = onehalf + onequarter - nMacroNum;
    while(i < nTemp)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
        ++i;
    }

    pdc->SelectObject(&ba);
    nTemp = onehalf + onequarter - nMicroNum;
    while(i < nTemp)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
        ++i;
    }
    
    pdc->SelectObject(&bi);
    nTemp = onehalf + onequarter + nMicroNum;
    while(i < nTemp)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
        ++i;
    }

    pdc->SelectObject(&ba);
    nTemp = onehalf + onequarter + nMacroNum;
    while(i < nTemp)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
        ++i;
    }

    pdc->SelectObject(GetStockObject(BLACK_BRUSH));
    while(i < nLength)
    {
        pdc->Rectangle(itr->x - nx, itr->y - nx, itr->x + nx + 1, itr->y + nx + 1);
		++itr;
        ++i;
    }

    return;
}