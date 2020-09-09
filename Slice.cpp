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
// Slice.cpp: implementation of the CSlice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SliceImage.h"
#include "AdvImage.h"
#include "TomatoAnalyzer.h"
#include "BinObjs.h"

#include <float.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <assert.h>

#include "Slice.h"
#include "linreg.h"
#include "TomatoAnalyzerDoc.h"
#include "TomatoAnalyzer.h"

#include <vector>
#include <string>
#include <algorithm>

#define pi 3.1415926

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "ImgTool.h"
#include "bwImage.h"


using namespace ipt;
using namespace std;

int getBinaryImageScanLineWidth(CAdvImage* pImg, int y, int& nLeft, int& nRight);
int avgPosition(int* d, int nLength);
int  FindLobePosition(CPixelList * pCntur, double, double, int, CAdvImage * pImage, POINTY *, double fruitBtmY);
extern CPixelList * MakeLine(double fx1, double fy1, double fx2, double fy2);
extern void SortFromMinToMax(double * e, int nNum, int * iIndex);
extern BOOL IsSameArray(int a[], int b[], int n);
extern double TriangleArea(double xa, double ya, double xb, double yb, double xc, double yc);

extern INTVECTOR *  FindLobePositionTest(double * x, double * y, int nLength, int& nFoundedNum, double curv_thresh, CPixelList& newCntr);
extern BOOL GetMeasurementIndexAndMethodID(int iIndex, int& nMeasurementID, int& nMethod);

extern LPSTR GetMeasurementName(int nIndex, int& nParentID, int& nMethod);
extern LPSTR  GetSubMethodName(int nMeasurementIndex, int nMethod);
extern double computeAngle(double ptLx, double ptLy, double ptCx, double ptCy, double ptRx, double ptRy);
extern int DecideBlmSignificantPoint(CAdvImage * pImg, double * xt, double * yt, int nLength, double wty, int * iSigPoint);

extern int bwfill(CPixelList * pCurve, CAdvImage * pImg);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SliceParameter::SliceParameter()
{
	m_Area = 0;
    m_Width = 0;
	m_Height = 0;
	m_Perimeter = 0;
	m_Taperness = 0;
	m_PL = 0;
	m_PeriCarpIndex = 0;

	m_TipPointX = m_TipPointY =-1;
}

SliceParameter::~SliceParameter()
{

}
CSlice::CSlice()
{
	m_iIndex = 0;

	//lobe
	m_pLobePos = NULL;
	nLobePosLength = 0;

    m_computed = false;

	m_blExcludedFromSaving = FALSE;

    m_Rotation = 0.0;

    m_blRotated = FALSE;
    m_blBoundaryChanged = FALSE;

    m_pOrgImage = m_pCurImage = NULL;
    m_pCurBoundary = m_pOrgBoundary = NULL;

    m_pLobePosTemp = NULL;

    m_TipArea = 0;
}


CSlice::~CSlice()
{
	if(NULL != m_pLobePosTemp)
		delete m_pLobePosTemp;

	if(NULL != m_pLobePos)
		delete m_pLobePos;

    if(NULL != m_pCurBoundary)
        delete m_pCurBoundary;

    if(NULL != m_pOrgImage)
        delete m_pOrgImage;

    if(NULL != m_pOrgBoundary)
        delete m_pOrgBoundary;

    if(NULL != m_pCurImage)
        delete m_pCurImage;

}

void CSlice::Set(CPixelList * inBinObj)
{
	mBBox = inBinObj->GetBBox();
    m_OrgBounderyBox = mBBox;

    m_pOrgImage = inBinObj->MakeGrayScaleImage();
    m_pOrgBoundary = m_pOrgImage->Contour();
    
    if(0.0 == m_Rotation)
        m_pCurBoundary = new CPixelList(*m_pOrgBoundary);
    else
    {
        CAdvImage * pTmpImage = m_pOrgImage->Rotate(m_Rotation);
        m_pCurBoundary = pTmpImage->Contour();
        delete pTmpImage;
    }
}



CPixel CSlice::GetNeighbor(CPixel inPix, int direction)
{
	int x = inPix.x;
	int y = inPix.y;

	switch(direction)
	{
	case 0:
		x = x+1;
		break;
	case 1:
		x = x+1;
		y = y-1;
		break;
	case 2:
		y = y-1;
		break;
	case 3:
		x = x-1;
		y = y-1;
		break;
	case 4:
		x = x-1;
		break;
	case 5:
		x = x-1;
		y = y+1;
		break;
	case 6:
		y = y+1;
		break;
	case 7:
		x = x+1;
		y = y+1;
		break;
	}
	
	CPixel retPixel;
	
	retPixel.x = x;
	retPixel.y = y;

	return retPixel;
}


//does not include p1, includes p2.
CPixelList CSlice::DrawLine(CPixel & p1, CPixel & p2)
{
	CPixel pointOnLine;
	double fx1, fy1, fx2, fy2, px, py;
	double dx, dy;
	int steps;
	CPixelList outList;

	fx1 = p1.x;
	fx2 = p2.x;
	fy1 = p1.y;
	fy2 = p2.y;
	px = fx1;
	py = fy1;
	
	pointOnLine = p1;
    outList.push_back(pointOnLine);

	dx = fx2 - fx1;
	dy = fy2 - fy1;

	fy1 = fabs(dy);
	fx1 = fabs(dx);
	if(fy1 > fx1)
	{
		steps = fy1;
	}
	else
	{
		steps = fx1;
	}

	dx /= (double)steps;
	dy /= (double)steps;

	for(int i = 0; i < (steps); i++)
	{
		px += dx;
		py += dy;
		pointOnLine.Set(px, py);
		outList.push_back(pointOnLine);
				
	}
	if(!(pointOnLine == p2))
		outList.push_back(p2);

	return outList;
}

void CSlice::GetWidthAndHeightInPixel(int& width, int& height)
{
	width = mBBox.mLowerRight.x - mBBox.mUpperLeft.x + 1;
	height= mBBox.mLowerRight.y - mBBox.mUpperLeft.y + 1;
	return;
}

void CSlice::DrawBBox(CDC* pDC, float scale)
{
	CPen * pPen = new CPen(PS_SOLID, 1, RGB(255,0,0));

	CPen * pOldPen = pDC->SelectObject(pPen);
	pDC->SelectObject(GetStockObject(NULL_BRUSH));
	
	int nOldROP2 = pDC->SetROP2(R2_XORPEN);

    POINT   pt[2];
    pt[0].x = pt[0].y = 0;
    pt[1].x = 2;
    pt[1].y = 2;
    pDC->DPtoLP(pt, 2);
    pt[1].x -= pt[0].x;
    pt[1].y -= pt[0].y;

    RECT    rectView;
    GetDisplayRegion(&rectView);

	int nLeft  = - pt[1].x;
	int nTop   = - pt[1].y;
	int nRight = rectView.right - rectView.left + pt[1].x;
	int nBottom= rectView.bottom - rectView.top + pt[1].y;

	pDC->Rectangle(nLeft,nTop,nRight,nBottom);

	pDC->SetROP2(nOldROP2);

	pDC->SelectObject(pOldPen);
	delete pPen;
}

void CSlice::setExclusionStatus(int nFlag)
{
    m_blExcludedFromSaving = nFlag;
}

int CSlice::InvertExclusionFlag()
{
	m_blExcludedFromSaving = !m_blExcludedFromSaving;
	return m_blExcludedFromSaving;
}

void CSlice::SetExcludeFlag()
{
	m_blExcludedFromSaving = TRUE;
	return;
}

int CSlice::GetExclusionFlag()
{
	return m_blExcludedFromSaving;
}

void CSlice::DrawExclusionMask(CDC* pDC, float scale)
{
	int nLeft  = mBBox.mUpperLeft.x*scale;
	int nTop   = mBBox.mUpperLeft.y*scale;
	int nRight = mBBox.mLowerRight.x*scale;
	int nBottom= mBBox.mLowerRight.y*scale;
	
	int nOldROP2 = pDC->SetROP2(R2_MASKPEN);

	//pDC->SelectObject(GetStockObject(BLACK_BRUSH));
	HBRUSH hb = CreatePatternBrush(LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_TEST)));
	pDC->SelectObject(hb);
	pDC->Rectangle(nLeft,nTop,nRight,nBottom);

	pDC->SelectObject(GetStockObject(NULL_BRUSH));
	pDC->SetROP2(nOldROP2);
	return;
}

void DrawStraightLine(CDC * pDC, double coef[], CPixelList * pPixelList)
{
	int x, y;

	CPen * pPen=new CPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen * pOld=pDC->SelectObject(pPen);
	
	CPixelList::iterator myIter=pPixelList->begin();
	x = myIter->x;
	y = myIter->y;

	if(coef[1]==0.0) //vertical line
	{
		int yMin, yMax;
		yMin=yMax=y;

		for(; myIter!=pPixelList->end(); myIter++)
		{
			y = myIter->y;
			if(yMin > y)
				yMin = y;
			if(yMax < y)
				yMax = y;
		}

//		pDC->MoveTo(x, yMin);
//		pDC->LineTo(x, yMax);

		for(y=yMin; y<=yMax; y++)
			pDC->SetPixel(x, y, RGB(0, 255, 0));
	}
	else
	{
		int xMin, xMax;
		xMin = xMax = x;

		for(; myIter!=pPixelList->end(); myIter++)
		{
			x = myIter->x;
			if(xMin > x)
				xMin = x;
			if(xMax < x)
				xMax = x;
		}

		for(x=xMin; x<=xMax; x++)
		{
			y = ceil((-coef[2] - coef[0]*x)/coef[1] + 0.5);
			pDC->SetPixel(x, y, RGB(0, 255, 0));
		}

//		y = (-coef[2] - coef[0]*xMin)/coef[1];
//		pDC->MoveTo(xMin, y);
//		y = (-coef[2] - coef[0]*xMax)/coef[1];
//		pDC->LineTo(xMax, y);

	}

	pDC->SelectObject(pOld);

	pPen->DeleteObject();
	delete pPen;
	return;
}

void CSlice::DrawBumpy(CDC* pDC)
{
    if(NULL != m_pLobePosTemp)
    {
        DrawSingleEdge(pDC, RGB(255, 255, 0), 0, 0);

    	CPen pen;
	    pen.CreatePen(PS_DOT, 2, RGB(255, 255, 0));
	    CPen * pOldpen = pDC->SelectObject(&pen);

        pDC->SelectObject(GetStockObject(WHITE_BRUSH));

        CPixel pix;
        CPixelList * pList = GetBoundary(1);

        INTVECTOR::iterator iter = m_pLobePosTemp->begin();
        INTVECTOR::iterator iend = m_pLobePosTemp->end();
        
        POINT   pt[2];
        pt[0].x = pt[0].y = 0;
        pt[1].x = 2;
        pt[1].y = 2;
        pDC->DPtoLP(pt, 2);
        pt[1].x -= pt[0].x;
        pt[1].y -= pt[0].y;
        
        while(iter!=iend)
        {
            pix = pList->GetAt(*iter);
            //pDC->SetPixel(pix.x, pix.y, RGB(255, 0, 0));
            pDC->Ellipse(pix.x-pt[1].x, pix.y-pt[1].y, pix.x+pt[1].x, pix.y+pt[1].y);

            ++iter;
        }

        pDC->SelectObject(pOldpen);
        pen.DeleteObject();

    }
    return;

    DrawSingleEdge(pDC, RGB(127, 127,0), 0, 0);

	CPen pen;
	pen.CreatePen(PS_DOT, 2, RGB(0, 0, 255));
	CPen * pOldpen = pDC->SelectObject(&pen);

	CPixelList * pix = GetBoundary(1);
	CPixel pt = pix->GetAt(bumpy_radiusPos);
	
    pDC->MoveTo(weight_ctrx, weight_ctry);
    pDC->LineTo(pt.x, pt.y);

    pDC->SelectObject(pOldpen);
    pen.DeleteObject();

    return;
}

void CSlice::CalcIndentation()
{
    m_Indentation = 0.0;
    
    double ta= TriangleArea(m_pnty.ptTopShoulder[1].x, m_pnty.ptTopShoulder[1].y, 
                            m_pnty.ptTop.x, m_pnty.ptTop.y, 
                            m_pnty.ptTopShoulder[3].x, m_pnty.ptTopShoulder[3].y
                           );

    if(ta<0)
        m_Indentation = -ta;

    return;
}

void CSlice::DrawIndentation(CDC * pDC)
{
    if(m_Indentation > 0.0)
    {
        POINT   pt[3];
        pt[0] = m_pnty.ptTopShoulder[1];
        pt[1] = m_pnty.ptTop;
        pt[2] = m_pnty.ptTopShoulder[3];
        
        CBrush  br;
        br.CreateSolidBrush(RGB(255,0,255));

        CRgn    rgn;
        rgn.CreatePolygonRgn(pt, 3, ALTERNATE);

        pDC->FillRgn(&rgn, &br);

        rgn.DeleteObject();
        br.DeleteObject();

    }
    return;
}

bool CSlice::TipArea(int iMethod, char * szBuffer, double * pVal)
{
    double tmp = double(m_TipArea)/double(area);
    
    sprintf(szBuffer, "%.4f", tmp);

    if(NULL != pVal)
        *pVal = tmp;

    return true;
}

bool CSlice::IndentationValue(int iMethod, char * szBuffer, double * pVal)
{
    double tmp;

    if(IDM_STEMEND_INDENTATION == iMethod)
        tmp = double(m_Indentation)/double(area);
    else if(IDM_BLSMEND_INDENTATION == iMethod)
        tmp = double(m_IndentBlm)/double(area);
    
    if(tmp>=0.0)
    {
         sprintf(szBuffer, "%.4f", tmp);

         if(NULL != pVal)
            *pVal = tmp;

         return true;
    }
    
    lstrcpy(szBuffer, "");
    return false;
}

void CSlice::DrawLeafLobe(CDC* pDC, int offsetx, int offsety)
{
    if(0 == m_pLobePos)
        return;

    if(m_pLobePos->empty())
        return;

	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	pDC->SelectObject(pen);
	pDC->SelectObject(GetStockObject(WHITE_BRUSH));

	CPixelList * pix = GetBoundary(1);

	CPixel pt;
	POINT pos;
	nLobePosLength = m_pLobePos->size();
	for(int i=0; i<nLobePosLength; i++)
	{
		pos = m_pLobePos->operator[](i);
		pt = pix->GetAt(pos.x);
		pDC->Ellipse(pt.x-3, pt.y-3, pt.x+3, pt.y+3);
	}
}

POINT ExtendSegmentLine(POINT ptMove, POINT ptFix, double dNewLen)
{
    POINT   ptNew;
    int dx = -ptFix.x + ptMove.x;
    int dy = -ptFix.y + ptMove.y;
    if(dx==0 && dy==0)
    {
        ptNew.x = 0;
        ptNew.y = 0;
        return ptNew;
    }

    double dscale = dNewLen / sqrt(dx*dx + dy*dy);
    ptNew.x = ptFix.x + round(dscale*double(dx));
    ptNew.y = ptFix.y + round(dscale*double(dy));

    return ptNew;
}

POINT ExtendSegmentLine(double ptxMove, double ptyMove, double ptxFix, double ptyFix, double dNewLen)
{
    POINT   ptNew;
    double dx = -ptxFix + ptxMove;
    double dy = -ptyFix + ptyMove;
    if(dx==0.0 && dy==0.0)
    {
        ptNew.x = round(ptxFix);
        ptNew.y = round(ptyFix);
        return ptNew;
    }

    double dscale = dNewLen / sqrt(dx*dx + dy*dy);
    ptNew.x = round(ptxFix + dscale*double(dx));
    ptNew.y = round(ptyFix + dscale*double(dy));

    return ptNew;
}

void CSlice::DrawShoulder(CDC * pDC)
{
    CPen    pen;
    CPen * oldpen;
    POINT   pt;

    pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
    oldpen = pDC->SelectObject(&pen);

    pt=m_pnty.ptTopShoulder[1];
    pDC->Ellipse(pt.x-2,  pt.y-2, pt.x+2, pt.y+2);
    pt=m_pnty.ptTopShoulder[3];
    pDC->Ellipse(pt.x-2,  pt.y-2, pt.x+2, pt.y+2);

    pDC->MoveTo(m_pnty.ptTop);
    pDC->LineTo(round(weight_ctrx), round(weight_ctry));

    pDC->SelectObject(oldpen);
    pen.DeleteObject();
    
    return;
}

void CSlice::DrawIndentationAngleBlmEnd(CDC * pDC)
{
	CPixelList * pPixelList = GetBoundary(1);
	if(pPixelList->size () == 0)
		return;
    int nLength = pPixelList->size();
	CPixelList::iterator myIter=pPixelList->begin();

    CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp *)AfxGetApp();
    int nMin = pApp->m_nMic;

    CPixel pix;
    int iPos = m_iTipBlm - nLength * nMin /200;
    int nLen = nLength * nMin /100;
	for(int i=0; i<nLen; i++)
    {
        if(iPos>=nLength)
            iPos -= nLength;
        else if(iPos < 0)
            iPos += nLength;

        pix = pPixelList->GetAt(iPos);
        ++iPos;

        pDC->SetPixel(pix.x, pix.y, RGB(255, 255, 0));
    }

    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    CPen* oldpen = pDC->SelectObject(&pen);
    pDC->SelectObject(GetStockObject(WHITE_BRUSH));

    pix = pPixelList->GetAt(m_iTipBlm);
    //pDC->Rectangle(pix.x-3, pix.y-3, pix.x+3, pix.y+3);

    pDC->MoveTo(m_ptTipAngle[0]);
    pDC->LineTo(m_ptTipAngle[1]);
    pDC->LineTo(m_ptTipAngle[2]);

    pDC->SelectObject(oldpen);
    pen.DeleteObject();
}

void CSlice::DrawPointy(CDC* pDC, int iDrawIndex)
{
    DrawVisualEffect(pDC, IDM_PARIMETER);

    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    CPen* oldpen = pDC->SelectObject(&pen);
    pDC->SelectObject(GetStockObject(WHITE_BRUSH));

    POINT   ptEnd;
    if(IDM_STEMEND_POINTY == iDrawIndex)
    {
        pDC->Ellipse(m_pnty.ptTop.x-2,  m_pnty.ptTop.y-2, m_pnty.ptTop.x+2, m_pnty.ptTop.y+2);
        pDC->Ellipse(m_pnty.ptBtm.x-2,  m_pnty.ptBtm.y-2, m_pnty.ptBtm.x+2, m_pnty.ptBtm.y+2);
    
        ptEnd = ExtendSegmentLine(m_pnty.ptTopShoulder[1], m_pnty.ptTop, 100.0);
        pDC->MoveTo(ptEnd);
        pDC->LineTo(m_pnty.ptTop);
        ptEnd = ExtendSegmentLine(m_pnty.ptTopShoulder[3], m_pnty.ptTop, 100.0);
        pDC->LineTo(ptEnd);
    }

    if(IDM_BLSMEND_POINTY == iDrawIndex)
    {
        if(m_pnty.ptBtmShoulder[1].x>0)
        {
            ptEnd = ExtendSegmentLine(m_pnty.ptBtmShoulder[1], m_pnty.ptBtm, 50.0);
            pDC->MoveTo(ptEnd);
            pDC->LineTo(m_pnty.ptBtm);
        }
        if(m_pnty.ptBtmShoulder[3].x>0)
        {
            ptEnd = ExtendSegmentLine(m_pnty.ptBtmShoulder[3], m_pnty.ptBtm, 50.0);
            pDC->MoveTo(m_pnty.ptBtm);
            pDC->LineTo(ptEnd);
        }
    }
   
    pDC->SelectObject(oldpen);
    pen.DeleteObject();

    DrawShoulder(pDC);
    return;

}

bool CSlice::LargeRegionPointyValue(int iMethod, char *szBuffer, double *pVal)
{
    if(0==iMethod)
    {
        double dang = m_fBlmAngle * 180/ pi;
        sprintf(szBuffer, "%.4f", dang);

        if(NULL != pVal)
            *pVal = dang;

        return false;
    }

    return false;
}


void CSlice::DrawLargeRegionPointy(CDC* pDC)
{
	CPixelList * pPixelList = GetBoundary(1);
	if(pPixelList->size () == 0)
		return;
    int nLength = pPixelList->size();
	CPixelList::iterator myIter=pPixelList->begin();

    CPixel pix;
/*
    CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp *)AfxGetApp();
    int nMax = pApp->m_nMac;
    int nMin = pApp->m_nMic;
    int nLen = nLength * (nMax - nMin)/200;

    int iPos = m_iTipBlm + nLength * nMin /200;
	for(int i=0; i<nLen; i++)
    {
        if(iPos>=nLength)
            iPos -= nLength;
        else if(iPos < 0)
            iPos += nLength;

        CPixel pix = pPixelList->GetAt(iPos);
        ++iPos;

        pDC->SetPixel(pix.x, pix.y, RGB(255, 255, 0));
    }

    iPos = m_iTipBlm - nLength * nMin /200;
	for(i=0; i<nLen; i++)
    {
        if(iPos>=nLength)
            iPos -= nLength;
        else if(iPos < 0)
            iPos += nLength;

        pix = pPixelList->GetAt(iPos);
        --iPos;

        pDC->SetPixel(pix.x, pix.y, RGB(255, 255, 0));
    }
*/    
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    CPen* oldpen = pDC->SelectObject(&pen);

    pix = pPixelList->GetAt(m_iTipBlm);
    pDC->Rectangle(pix.x-3, pix.y-3, pix.x+3, pix.y+3);

    pDC->MoveTo(m_ptBlmAngle[0]);
    pDC->LineTo(m_ptBlmAngle[1]);
    pDC->LineTo(m_ptBlmAngle[2]);

    pDC->SelectObject(oldpen);
    pen.DeleteObject();
    return;
}

void CSlice::GetAnglePosition(POINT * ppt, int iIndex)
{
    CPixelList * pcurve = GetBoundary(1);
    CPixel  pix;

    if(iIndex==0)
    {
        ppt[0] = m_pnty.ptTopShoulder[1];
        ppt[1] = m_pnty.ptTop;
        ppt[2] = m_pnty.ptTopShoulder[3];
        return;
    }

    if(iIndex==1)
    {
        pix = pcurve->GetAt(m_iTipBlm);
        ppt[1].x = pix.x;   ppt[1].y = pix.y;

        ppt[0].x = ppt[0].y = 0;
        ppt[2].x = ppt[2].y = 0;
        return;
    }

    return;
}

void CSlice::SetAnglePosition(int iIndex, POINT * ppt)
{
    if(iIndex==0)
    {
        m_pnty.ptTopShoulder[1] = ppt[0];
        m_pnty.ptTop = ppt[1];
        m_pnty.ptTopShoulder[3] = ppt[2];

        m_pnty.topAngle = computeAngle(ppt[0], ppt[1], ppt[2])*180.0/pi;
        return;
    }
/*
    if(iIndex==1)
    {

        m_pnty.ptBtmShoulder[1] = ppt[0];
        m_pnty.ptBtm = ppt[1] ;
        m_pnty.ptBtmShoulder[3] = ppt[2];

        m_pnty.btmAngle = computeAngle(ppt[2], ppt[1], ppt[0])*180.0/pi;
        return;
    }
*/
    if(iIndex==1)
    {
        metric_IndentationAngle_Custom(ppt[1]);

        //m_ptBtmLargeAngle[0] = ppt[0];
        //m_ptBtmLargeAngle[1] = ppt[1];
        //m_ptBtmLargeAngle[2] = ppt[2];
        return;
    }

    return;
}


void CSlice::DrawFruitFrame(CDC* pDC)
{
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
    CPen* oldpen = pDC->SelectObject(&pen);

    pDC->SelectObject(GetStockObject(NULL_BRUSH));

    pDC->Rectangle(m_rtFruit.left-1, m_rtFruit.top-1, m_rtFruit.right+1, m_rtFruit.bottom+1);

    pDC->SelectObject(oldpen);
    pen.DeleteObject();

}

void CSlice::DrawAxis(CDC * pDC, int iWhich)
{
    CPixelList::iterator myIter, IterEnd;

    if(IDM_MIDHEIGHT_WIDTH==iWhich)
    {
    	myIter=MidHeightFruitAxis.begin();
        IterEnd=MidHeightFruitAxis.end();
    }
    else if(IDM_MIDWIDTH_HEIGHT==iWhich)
    {
    	myIter=VerticalAxis.begin();
        IterEnd=VerticalAxis.end();
    }
    else
        return;

	for(; myIter != IterEnd; ++myIter)
	{
		pDC->SetPixel(myIter->x, myIter->y,RGB(255,0,0));
	}

    return;
}

void CSlice::DrawAspectRatio(CDC * pDC, int iMethod)
{
    if(iMethod==0)
    {
        DrawFruitFrame(pDC);
        return;
    }

    if(iMethod==1)
    {
        DrawAxis(pDC, IDM_MIDHEIGHT_WIDTH);
        DrawAxis(pDC, IDM_MIDWIDTH_HEIGHT);
        return;
    }

    if(iMethod==2)
    {
        DrawElliptic(pDC);
        return;
    }

    return;
}

void CSlice::DrawPrimaryWidestAixs(CDC* pDC, int offsetx, int offsety)
{
	CPoint point;
	point = CPoint(offsetx, offsety);

    CPixelList::iterator	myIter=MidHeightFruitAxis.begin();
    CPixelList::iterator    IterEnd=MidHeightFruitAxis.end();
	for(; myIter!=IterEnd; ++myIter)
	{
		CPixel pixel = *myIter;
		POINT point;
		point.x = pixel.x+offsetx;
		point.y = pixel.y+offsety;

		pDC->SetPixel (point,RGB(255,255,0));
	}

	myIter=MaxFruitAxis.begin();
    IterEnd=MaxFruitAxis.end();
	for(; myIter != IterEnd; ++myIter)
	{
		CPixel pixel = *myIter;
		POINT point;
		point.x = pixel.x+offsetx;
		point.y = pixel.y+offsety;

		pDC->SetPixel (point,RGB(0,0,0));
	}

	myIter=MidHeightFruitAxis.begin();
    IterEnd=MidHeightFruitAxis.end();
	for(; myIter != IterEnd; ++myIter)
	{
		CPixel pixel = *myIter;
		POINT point;
		point.x = pixel.x+offsetx;
		point.y = pixel.y+offsety;

		pDC->SetPixel (point,RGB(0,128,128));
	}
}


void CSlice::CalcParameters(BOOL bContour)
{
    if(NULL != m_pCurImage)
        delete m_pCurImage;

    m_pCurImage = m_pOrgImage->Rotate(m_Rotation);
    if(NULL==m_pCurImage)
    {
        AfxMessageBox("Low memory!", MB_OK|MB_ICONSTOP);
        return;
    }

  	m_pCurBoundary->GetBoundingRect(&m_rtFruit);
    m_nFruitWidth = m_rtFruit.right - m_rtFruit.left + 1;
    m_nFruitHeight= m_rtFruit.bottom - m_rtFruit.top + 1;

    imgWidth  = m_pCurImage->GetWidth();
	imgHeight = m_pCurImage->GetHeight();

    metric_weightcenter(m_pCurImage, weight_ctrx, weight_ctry, area);

    //create center relative boundary
    int wx, wy;
    if(imgWidth>600 || imgHeight>600)
    {
        CAdvImage * pTmpImage = CIimresize(m_pCurImage, 0.5, "near");
        m_pCenterRelativeBoundary = pTmpImage->Contour();

        int nIgnore;
        double dx, dy;
        metric_weightcenter(pTmpImage, dx, dy, nIgnore);
        delete pTmpImage;

        m_fShapeScale = 2.0;
        wx = round(dx);
        wy = round(dy);
    }
    else
    {
        m_pCenterRelativeBoundary = new CPixelList(*m_pCurBoundary);
        m_fShapeScale = 1.0;
        wx = round(weight_ctrx);
        wy = round(weight_ctry);
    }

    CPixelList::iterator    iter = m_pCenterRelativeBoundary->begin();
    CPixelList::iterator    iterEnd = m_pCenterRelativeBoundary->end();
    while(iter != iterEnd)
    {
        iter->x -= wx;
        iter->y -= wy;

        ++iter;
    }


	GetPrimaryAxis();
	GetHorizontalAxis();

    int nTopSigPos = findBlossomPoint();

    m_suggestedbtmsigpos = FindLobePosition(m_pCurBoundary, weight_ctrx, weight_ctry, nTopSigPos, m_pCurImage, &m_pnty, m_rtFruit.bottom);
    m_iTipStm = m_pnty.ptTop;
    m_

    {
		CPixelList * pb = GetBoundary(1);
		int nLength = pb->size();

		double * x = new double[nLength];
		double * y = new double[nLength];
		int i=0;
		CPixelList::iterator myIter;
		for(myIter = pb->begin(); myIter!=pb->end(); myIter++)
		{
			x[i] = double(myIter->x);
			y[i] = double(myIter->y);

			i++;
		}

		CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp * ) AfxGetApp();

        CPixelList  filteredCntr;
		//m_pLobePos = 
          m_pLobePosTemp =  FindLobePositionTest(x, y, nLength, nLobePosLength, pApp->g_shapethresh, filteredCntr);

//        pb->clear();
//        pb->insert(pb->end(), filteredCntr.begin(), filteredCntr.end());

        delete x;
        delete y;

        //autoAdjustContour();
    }

    metric_computeAll();


    delete m_pCenterRelativeBoundary;
    m_pCenterRelativeBoundary = NULL;
    return;
}


int CSlice::findBlossomPoint()
{
	CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp * ) AfxGetApp();
    POSITION pos = pApp->GetFirstDocTemplatePosition();
    CDocTemplate * pDocTpl = pApp->GetNextDocTemplate(pos);
    if(NULL == pDocTpl)
        return 0;

    pos = pDocTpl->GetFirstDocPosition();
    CTomatoAnalyzerDoc *    pDoc = (CTomatoAnalyzerDoc *) pDocTpl->GetNextDoc(pos);
   	CAdvImage *pSliceImage = pDoc->m_pImage->Crop(mBBox.GetRect());
    if(0.0 != m_Rotation)
    {
        CAdvImage * pTmp = pSliceImage->Rotate(m_Rotation);
        delete pSliceImage;
        pSliceImage = pTmp;
    }

    unsigned char ** ppR = pSliceImage->GetBand(0);
    unsigned char ** ppG = pSliceImage->GetBand(1);
    unsigned char ** ppB = pSliceImage->GetBand(2);
    
    CPixelList::iterator iter = m_pCurBoundary->begin();
    CPixelList::iterator iterFrom;
    int nContourLength = m_pCurBoundary->size();
    int nFrom, nTo;
    int nThreshold = m_rtFruit.top + ((m_rtFruit.bottom - m_rtFruit.top)>>2);
    for(int i=0; i<nContourLength; i++)
    {
        if(iter->y <= nThreshold)
        {
            nFrom = i;
            iterFrom = iter;
            break;
        }

        ++iter;
    }

    nThreshold += 5;
    while(i<nContourLength)
    {
        if(iter->y > nThreshold)
        {
            nTo = i;
            break;
        }

        ++i;
        ++iter;
    }

    int nSegmentLength = nTo - nFrom + 1;
    double * pGrayLevel = new double[nSegmentLength];
    iter = iterFrom;
    for(i=0; i<nSegmentLength; i++)
    {
        CPixelList * pList = MakeLine(weight_ctrx, weight_ctry, iter->x, iter->y);
        
        CPixelList::iterator    ir = pList->begin();
        CPixelList::iterator    ir_end = pList->end();
        int nSum = 0;
        while(ir != ir_end)
        {
            nSum += int(ppR[ir->y][ir->x]);
            nSum += int(ppG[ir->y][ir->x]);
            nSum += int(ppB[ir->y][ir->x]);

            ++ir;
        }

        pGrayLevel[i] = double(nSum)/double(pList->size());
        delete pList;

        ++iter;
    }

    int nGrayMax = pGrayLevel[0];
    int nMaxPos = 0;
    for(i=1; i<nSegmentLength; i++)
    {
        if(nGrayMax < pGrayLevel[i])
        {
            nGrayMax = pGrayLevel[i];
            nMaxPos = i;
        }
    }

/*
    iter = iterFrom;
    for(i=0; i<nSegmentLength; i++)
    {
        if(i==nMaxPos)
        {
            m_nBlossomEnd.x = iter->x;
            m_nBlossomEnd.y = iter->y;
        }

        ++iter;
    }
*/
    delete pSliceImage;
    delete pGrayLevel;
    return nFrom + nMaxPos;
}


CPixelList * CSlice::GetBoundary(int nWhich)
{
    if(0==nWhich)
        return m_pOrgBoundary;

    return m_pCurBoundary;
}


bool CSlice::GetDescriptorValue(int iDescriptor, int iMethod, char * szBuffer, double * pVal)
{
    if(!m_computed)
    {
        metric_computeAll();
    }

	bool bRet;
    double tmp, tmpScale;

    if(NULL != pVal)
        *pVal = 0.0;

	switch(iDescriptor)
	{
	case IDM_AREA: // Area
		 tmpScale = ((CTomatoAnalyzerApp *)AfxGetApp())->getUnitScale();
         tmp = double(area)*tmpScale*tmpScale;

         sprintf(szBuffer, "%.4f", tmp);
         if(NULL != pVal)
             *pVal = tmp;

		 bRet = true;
		 break;

	case IDM_PARIMETER: // Perimeter
         bRet = PerimeterValue(1, szBuffer, pVal);
		 break;

    case IDM_MIDWIDTH_HEIGHT:
    case IDM_HIGHEST_HEIGHT:
         bRet = HeightValue(iDescriptor, szBuffer, pVal);
         break;

    case IDM_MIDHEIGHT_WIDTH:
    case IDM_WIDEST_WIDTH:
         bRet = WidthValue(iDescriptor, szBuffer, pVal);
         break;

	case IDM_ASPECT_RATIO:
		 bRet = AspectRatioValue(iMethod, szBuffer, pVal);
		 break;

	case IDM_ELLIPTIC: // Elliptic
		 bRet = EllipticValue(iMethod, szBuffer, pVal);
		 break;

    case IDM_CIRCULAR: // Circular
         bRet = CircularValue(iMethod, szBuffer, pVal);
         break;

    case IDM_RECTANGULAR: // Rectangle
         bRet = RectangularValue(iMethod, szBuffer, pVal);
         break;

    case IDM_BLSMEND_BLOCKINESS: // Block Blossom End
         bRet = BlockyValue(iMethod, szBuffer, 0, pVal);
         break;

    case IDM_STEMEND_BLOCKINESS: //Block Stem end
         bRet = BlockyValue(iMethod, szBuffer, 1, pVal);
         break;

    case IDM_OBOVATE: //Obovate
         bRet = ObovateValue(iMethod, szBuffer, 0, pVal);
         break;

    case IDM_OVATE: //Ovate
         bRet = ObovateValue(iMethod, szBuffer, 1, pVal);
         break;

/*    case IDM_BUMPINESS: //Bumpy
         bRet = BumpyValue(iMethod, szBuffer, pVal);
         break;
*/
    case IDM_STEMEND_POINTY:
    case IDM_BLSMEND_POINTY:
         bRet = PointyValue(iDescriptor, szBuffer, pVal);
         break;

    case IDM_TAPERNESS:
         bRet = TapernessValue(iMethod, szBuffer, pVal);
         break;

    case IDM_STEMEND_SHUDRHEIGHT:
         bRet = ShoulderHeightValue(iMethod, szBuffer, pVal);
         break;

    case IDM_BLSMEND_INDENTATION:
    case IDM_STEMEND_INDENTATION:
         bRet = IndentationValue(iDescriptor, szBuffer, pVal);
         break;

    case IDM_BLSMEND_LARGEANGLE:
         bRet = LargeRegionPointyValue(iMethod, szBuffer, pVal);
         break;

    case IDM_TRIANGLE:
         bRet = TriangleValue(iMethod, szBuffer, pVal);
         break;

    case IDM_ECCENTRICITY:
    case IDM_ASYMMETRY:
    case IDM_PRACTAL:
    case IDM_HEARSHAPE: //
    case IDM_PEARSHAPE: //
         lstrcpy(szBuffer, "");
         break;

    case IDM_TIPAREA:
         bRet = TipArea(iMethod, szBuffer, pVal);
         break;

    case IDM_SHAPECATEGORY:
         bRet = SuperquadraticsValue(iMethod, szBuffer, pVal);
         break;


	default:
		 wsprintf(szBuffer, "%d, %d", iDescriptor, iMethod);
		 bRet = false;
		 break;
	}

	return bRet;
}



void CSlice::metric_weightcenter(CAdvImage * pImg, double& ctrx, double& ctry, int& areasize)
{
    int nWidth, nHeight, x, y, xSum, ySum, nNum;
    
    unsigned char ** ppr = pImg->GetBand(0);
    unsigned char * ppix;

    nWidth = pImg->GetWidth();
    nHeight= pImg->GetHeight();

    xSum = ySum = nNum = 0;
    for(y=0; y<nHeight; y++)
    {
        ppix = ppr[y];

        for(x=0; x<nWidth; x++)
        {
            if(0 != *ppix)
            {
                xSum += x;
                ySum += y;

                nNum++;
            }

            ++ppix;
        }
    }

    areasize = nNum;
    ctrx = double(xSum)/double(nNum);
    ctry = double(ySum)/double(nNum);
    return;
}


void CSlice::metric_ellipse()
{
	ellipse(m_pCenterRelativeBoundary, ellipse_a, ellipse_b, ellipse_cd);
    ellipse_a *= m_fShapeScale;
    ellipse_b *= m_fShapeScale;
	return;
}

void CSlice::metric_superquadric()
{
	CPixelList::iterator itr, itr_end;

	CPixelList * pCtr = GetBoundary(1);
	itr=pCtr->begin();
	itr_end=pCtr->end();

	std::vector<IPTPOINT>	pix;
	pix.reserve(pCtr->size());

	int xc = round(weight_ctrx);
	int yc = round(weight_ctry);

	while(itr != itr_end)
	{
		IPTPOINT pt;
		pt.x = xc - itr->x;
		pt.y = yc - itr->y;
		pix.push_back(pt);

		++itr;
	}

    double cd;
    spellipse_n = 0.0;
    superquadric(&pix, ellipse_a, ellipse_b, cd, spellipse_n);

/*
    double cd;
    spellipse_n = 0.0;
    superquadric(m_pCenterRelativeBoundary, ellipse_a/m_fShapeScale, ellipse_b/m_fShapeScale, cd, spellipse_n);
*/
    return;
}

/*
void CSlice::metric_cylindral()
{
    CPixelList * pCtr = GetBoundary(1);
    CPixelList::iterator  iter, iterEnd;
    iterEnd = pCtr->end();

    int nLeft, nRight, nNumber;
	int thresh = pCtr->size()/2;


    for(int i=1; i<imgWidth/2; i++)
    {
        nNumber = 0;

        nLeft  = i;
        nRight = imgWidth - 1 - i;

        iter = pCtr->begin();
        while(iter != iterEnd)
        {
            if(iter->x<=nLeft || iter->x>=nRight)
                nNumber++;

            ++iter;
        }

        if(nNumber>=thresh)
            break;
    }

    cynlindral_left = nLeft;
    cynlindral_right = nRight;
    return;
}
*/

void CSlice::DrawObovate(CDC* pDC)
{
 
	CPen * pPen = new CPen;
	pPen->CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	CPen * pOldPen = pDC->SelectObject(pPen);
 
    int nLeft, nRight, y;
    y=obovate_posTop;
    getBinaryImageScanLineWidth(m_pCurImage, y, nLeft, nRight);
    pDC->MoveTo(nLeft, y);
    pDC->LineTo(nRight, y);

    y=obovate_posMid;
    getBinaryImageScanLineWidth(m_pCurImage, y, nLeft, nRight);
    pDC->MoveTo(nLeft, y);
    pDC->LineTo(nRight, y);

    y=obovate_posBtm;
    getBinaryImageScanLineWidth(m_pCurImage, y, nLeft, nRight);
    pDC->MoveTo(nLeft, y);
    pDC->LineTo(nRight, y);

    pDC->SelectObject(pOldPen);
    pPen->DeleteObject();
    delete pPen;

    return;
}

bool CSlice::ShoulderHeightValue(int iMethod, char * szBuffer, double * pVal)
{
    if(1 >= iMethod)
    {
        double tmpScale = ((CTomatoAnalyzerApp *)AfxGetApp())->getUnitScale();
        double tmp = double(m_pnty.ShoulderTop[iMethod])*tmpScale;
    
        sprintf(szBuffer, "%.4f", tmp);

        if(NULL != pVal)
            *pVal = tmp;

        return (0==iMethod)? true: false;
    }
    return false;
}

bool CSlice::SuperquadraticsValue(int iMethod, char * szBuffer, double * pVal)
{
    if(0==iMethod)
    {
		sprintf(szBuffer, "%.4f", spellipse_n);
		
        if(NULL != pVal)
            *pVal = spellipse_n;

        return true;
    }

    lstrcpy(szBuffer, "");

    if(NULL != pVal)
            *pVal = spellipse_n;
        
    return false;
}

bool CSlice::ObovateValue(int iMethod, char * szBuffer, int iWhich, double * pVal)
{
    if(0 == iMethod)
    {
        double scale = 0.0;
        double t = double(obovate_posMid)/double(imgHeight);
        if(t>=0.6 && iWhich==0)
            scale = 2.5*(t-0.6);
        else if(t<0.4 && iWhich==1)
            scale = 2.5*(0.4-t);
        
        int nLeft, nRight;
        double w1 = getBinaryImageScanLineWidth(m_pCurImage, obovate_posTop, nLeft, nRight);
        double W  = getBinaryImageScanLineWidth(m_pCurImage, obovate_posMid, nLeft, nRight);
        double w2 = getBinaryImageScanLineWidth(m_pCurImage, obovate_posBtm, nLeft, nRight);
        
        if(iWhich==0)
            t = scale*(1 - w1/W + w2/W)*0.5;
        else
            t = scale*(1 + w1/W - w2/W)*0.5;

        sprintf(szBuffer, "%.4f", t);

        if(NULL != pVal)
            *pVal = t;

        return false;
    }

    return false;
}


void CSlice::metric_perimeter()
{
        double dst = 0;

        CPixelList * pc = GetBoundary(1);
        CPixelList::iterator  itr, itrPre, itrend;
        itrPre = pc->begin();
        itrend = pc->end();
        itr = itrPre;
        ++itr;

        while(itr != itrend)
        {
            int dx, dy;
            dx = itr->x - itrPre->x;
            dy = itr->y - itrPre->y;
            
            dst += sqrt(dx*dx + dy*dy);

            ++itrPre;
            ++itr;
        }
    
        m_perimeter = dst;

        return;
}

bool CSlice::PerimeterValue(int iMethod, char * szBuffer, double * pVal)
{
    double tmpScale = ((CTomatoAnalyzerApp *)AfxGetApp())->getUnitScale();
    double tmp = m_perimeter * tmpScale;

    sprintf(szBuffer, "%.4f", tmp);

    if(NULL != pVal)
         *pVal = tmp;

    return true;
}


bool CSlice::AspectRatioValue(int iMethod, char * szBuffer, double * pVal)
{
	if(0==iMethod)
	{
        double tmp = double(m_nFruitHeight)/double(m_nFruitWidth);
        sprintf(szBuffer, "%.4f", tmp);

        if(NULL != pVal)
            *pVal = tmp;

		return true;
	}
	
    if(1 == iMethod)
    {
        double tmp = double(VerticalAxis.size())/double(MidHeightFruitAxis.size());
        sprintf(szBuffer, "%.4f", tmp);

        if(NULL != pVal)
            *pVal = tmp;

        return true;
    }

	if(2 == iMethod)
	{
        if(ellipse_b>0.0 && ellipse_a>0)
        {
            double tmp = ellipse_b/ellipse_a;
		    sprintf(szBuffer, "%.4f", tmp);
		
            if(NULL != pVal)
                *pVal = tmp;
        }
        return true;
	}
	
    lstrcpy(szBuffer, "error");
	return false;
}

bool CSlice::BlockyValue(int iMethod, char * szBuffer, int iIndex, double * pVal)
{
    int nLeft, nRight, nWidth, pos;
    
    if(0==iIndex)
        pos = blk_blm_pos;
    else
        pos = blk_stm_pos;

    nWidth = getBinaryImageScanLineWidth(m_pCurImage, pos, nLeft, nRight);
    double tmp = double(nWidth)/double(MidHeightFruitAxis.size());
    sprintf(szBuffer, "%.4f", tmp);

    if(NULL != pVal)
        *pVal = tmp;

    return false;
}

bool CSlice::TriangleValue(int iMethod, char * szBuffer, double * pVal)
{
    if(0 == iMethod)
    {
        int nLeft, nRight;
        int nWidthTop = getBinaryImageScanLineWidth(m_pCurImage, blk_stm_pos, nLeft, nRight);
        int nWidthBtm = getBinaryImageScanLineWidth(m_pCurImage, blk_blm_pos, nLeft, nRight);

        double dRatio = double(nWidthTop) / double(nWidthBtm);

        sprintf(szBuffer, "%.4f", dRatio);
    
        if(NULL != pVal)
            *pVal = dRatio;
    }
    return false;
}

bool CSlice::RectangularValue(int iMethod, char * szBuffer, double * pVal)
{
    if(0 == iMethod)
    {
        double tmp = (m_rtInner.right - m_rtInner.left + 1) * (m_rtInner.bottom - m_rtInner.top + 1);
        tmp /= double(m_nFruitHeight*m_nFruitWidth);

        sprintf(szBuffer, "%.4f", tmp);
        
        if(NULL != pVal)
            *pVal = tmp;

        return true;
    }

    if(1 == iMethod)
    {
        double tmp = 1.0 - 1.0/spellipse_n;
 
        sprintf(szBuffer, "%.4f", tmp);

        if(NULL != pVal)
            *pVal = tmp;

        return false;
    }

    return false;
}


bool CSlice::HeightValue(int iWhich, char * szBuffer, double * pVal)
{
    double tmp;

    if(IDM_HIGHEST_HEIGHT == iWhich)
        tmp = double(m_nFruitHeight);
    else if(IDM_MIDWIDTH_HEIGHT == iWhich)
        tmp = double(VerticalAxis.size());
    else
        return false;
    
    double tmpScale = ((CTomatoAnalyzerApp *)AfxGetApp())->getUnitScale();
    tmp *= tmpScale;
    sprintf(szBuffer, "%.4f", tmp);

    if(NULL != pVal)
        *pVal = tmp;

    return true;
}


bool CSlice::WidthValue(int iWhich, char * szBuffer, double * pVal)
{
    double tmp;

    if(IDM_WIDEST_WIDTH == iWhich)
        tmp = double(m_nFruitWidth);
    else if(IDM_MIDHEIGHT_WIDTH == iWhich)
        tmp = double(MidHeightFruitAxis.size());
    else
        return false;
            
    double tmpScale = ((CTomatoAnalyzerApp *)AfxGetApp())->getUnitScale();
    tmp *= tmpScale;
        
    sprintf(szBuffer, "%.4f", tmp);

    if(NULL != pVal)
        *pVal = tmp;

    return true;
}

/*
bool CSlice::CylindralValue(int iMethod, char * szBuffer, double * pVal)
{
    if(0 == iMethod)
    {
        double tmp = double(cynlindral_right - cynlindral_left)/double(imgWidth);

        sprintf(szBuffer, "%.4f", tmp);
        
        if(NULL != pVal)
            *pVal = tmp;

        return true;
    }

    if(1 == iMethod)
    {
        double tmp = double(imgWidth)/double(imgHeight);

        sprintf(szBuffer, "%.4f", tmp);
        
        if(NULL != pVal)
            *pVal = tmp;

        return true;
    }

    if(2 == iMethod)
    {
        double tmp = double(ellipse_a)/double(ellipse_b);
        
        sprintf(szBuffer, "%.4f", tmp);
        
        if(NULL != pVal)
            *pVal = tmp;

        return false;
    }

    return false;
}
*/

bool CSlice::CircularValue(int iMethod, char * szBuffer, double * pVal)
{
    double tmp;

    if(0 == iMethod)
    {
        IPTPOINT ctr = IPTPOINT(round(circle_ox), round(circle_oy));
        std::vector<IPTPOINT>* pOri = m_pCurImage->toPointLst(0, ctr);

    	int a, b;
        a = round(circle_r);
        b = round(circle_r);

		std::vector<IPTPOINT>* pEllipse = ellipse(a, b);
        IPTPOINT tr = topright(*pEllipse);
        IPTPOINT bl = bottomleft(*pEllipse);

        int xMin = (bl.x < -ctr.x)? bl.x : -ctr.x;
        int yMin = (bl.y < -ctr.y)? bl.y : -ctr.y;
        int xMax = (tr.x < ctr.x)? ctr.x : tr.x;
        int yMax = (tr.y < ctr.y)? ctr.y : tr.y;

        int newWidth  = xMax - xMin + 1;
        int newHeight = yMax - yMin + 1;
        ctr.x = newWidth/2;
        ctr.y = newHeight/2;

        CIptBwImage imgEllipse;
        imgEllipse.vector2img(*pEllipse, ctr, newWidth, newHeight);
        imgEllipse.convexFill();
        delete pEllipse;

        CIptBwImage imgOriginal;
        imgOriginal.vector2img(*pOri, ctr, newWidth, newHeight);
        delete pOri;

        CIptBwImage imgXor;
        imgXor.xor(imgEllipse, imgOriginal);

        int nAreaDiff = imgXor.countForegroundPixel();
        int nAreaSlce = imgOriginal.countForegroundPixel();

        tmp = 1.0 - double(nAreaDiff)/double(nAreaSlce);

		sprintf(szBuffer, "%.4f", tmp);
        
        if(NULL != pVal)
            *pVal = tmp;

        return true;
    }


    if(1 == iMethod)
    {
        tmp = 1.0 - circle_cd;

        sprintf(szBuffer, "%.4f", tmp);
        
        if(NULL != pVal)
            *pVal = tmp;

        return true;
    }

    return false;
}

/*
void CSlice::DrawCylindral(CDC* pDC)
{
	CPen * pPen = new CPen;
	pPen->CreatePen(PS_DASH, 1, RGB(255, 255, 0));
	CPen * pOldPen = pDC->SelectObject(pPen);
    pDC->SetBkMode(TRANSPARENT);

    pDC->MoveTo(cynlindral_left, 0);
    pDC->LineTo(cynlindral_left, imgHeight);
    pDC->MoveTo(cynlindral_right, 0);
    pDC->LineTo(cynlindral_right, imgHeight);


    pDC->SelectObject(pOldPen);
    pPen->DeleteObject();
    delete pPen;

    return;
}
*/

void CSlice::metric_blocky()
{
    int nLeft, nRight;
    
    blk_blm_pos = m_rtFruit.top + round(double(m_nFruitHeight)*((CTomatoAnalyzerApp*)AfxGetApp())->m_fBlkDn);
    blk_blm = getBinaryImageScanLineWidth(m_pCurImage, blk_blm_pos, nLeft, nRight);

    blk_stm_pos = m_rtFruit.top + round(double(m_nFruitHeight)*((CTomatoAnalyzerApp*)AfxGetApp())->m_fBlkUp);
    blk_stm = getBinaryImageScanLineWidth(m_pCurImage, blk_stm_pos, nLeft, nRight);
    return;
}


void CSlice::metrix_circular()
{
    circular(m_pCurBoundary, circle_ox, circle_oy, circle_cd, circle_r);
    return;
}


void CSlice::metrix_rectangular()
{
    int width, height, x, y;
    RECT    rt;

    width = m_pCurImage->GetWidth();
    height = m_pCurImage->GetHeight();
    unsigned char**  ppix = m_pCurImage->GetBand(0);
    unsigned char * prow;

    int nArea = -1;
    for(y=0; y<height; y++)
    {
        int nTopLeft, nTopRight, nBtmLeft, nBtmRight;

        prow = ppix[y];
        nTopLeft = -1;
        for(x=0; x<width; x++)
        {
            if(prow[x] == 255)
            {
                nTopLeft = x;
                break;
            }
        }
        if(nTopLeft == -1)
            continue;

        nTopRight = -1;
        for(x=width-1; x>=0; x--)
        {
            if(prow[x] == 255)
            {
                nTopRight = x;
                break;
            }
        }
        if(nTopRight == -1)
            continue;

        for(int iRow=y+1; iRow<height; iRow++)
        {
            prow = ppix[iRow];

            nBtmLeft = -1;
            for(x=0; x<width; x++)
            {
                if(prow[x] == 255)
                {
                    nBtmLeft = x;
                    break;
                }
            }
            if(nBtmLeft == -1)
                continue;

            nBtmRight=-1;
            for(x=width-1; x>=0; x--)
            {
                if(prow[x] == 255)
                {
                    nBtmRight = x;
                    break;
                }
            }
            if(nBtmRight == -1)
                continue;

            int nLeft  = (nTopLeft>nBtmLeft)? nTopLeft : nBtmLeft;
            int nRight = (nTopRight<nBtmRight)? nTopRight : nBtmRight;
            int nTmpArea = (iRow - y)*(nRight-nLeft);
            if(nTmpArea > nArea)
            {
                rt.left = nLeft;
                rt.top  = y;
                rt.right = nRight;
                rt.bottom = iRow;

                nArea = nTmpArea;
            }
        }
    }

    m_rtInner = rt;

    return;
}


void CSlice::metric_obovate()
{
    int* w = new int[m_rtFruit.bottom - m_rtFruit.top + 1];
    int y, nLeft, nRight, wmax, posTop, posBtm, posMid;
    
    wmax = 0;
    int iIndex = 0;
    for(y=m_rtFruit.top; y<=m_rtFruit.bottom; y++)
    {
        w[iIndex] = getBinaryImageScanLineWidth(m_pCurImage, y, nLeft, nRight);
        if(wmax < w[iIndex])
        {
            wmax = w[iIndex];
            posMid = y;
        }

        ++iIndex;
    }

    posTop = avgPosition(w, posMid-m_rtFruit.top+1);
    posBtm = avgPosition(w+posMid - m_rtFruit.top + 1, m_rtFruit.bottom - posMid) + posMid + 1;

    obovate_posTop = posTop;
    obovate_posMid = posMid;
    obovate_posBtm = posBtm;

    delete w;
    return;
}

bool CSlice::EllipticValue(int iMethod, char * szBuffer, double * pVal)
{
    double tmp;

	if(0 == iMethod)
	{
        IPTPOINT ctr = IPTPOINT(round(weight_ctrx), round(weight_ctry));
        std::vector<IPTPOINT>* pOri = m_pCurImage->toPointLst(0, ctr);


    	int a, b;
        a = round(ellipse_a);
        b = round(ellipse_b);

		std::vector<IPTPOINT>* pEllipse = ellipse(a, b);
		if(pEllipse==NULL)
		{
			szBuffer[0]=0;
			return true;
		}

        IPTPOINT tr = topright(*pEllipse);
        IPTPOINT bl = bottomleft(*pEllipse);

        int xMin = (bl.x < -ctr.x)? bl.x : -ctr.x;
        int yMin = (bl.y < -ctr.y)? bl.y : -ctr.y;
        int xMax = (tr.x < ctr.x)? ctr.x : tr.x;
        int yMax = (tr.y < ctr.y)? ctr.y : tr.y;

        int newWidth  = xMax - xMin + 1;
        int newHeight = yMax - yMin + 1;
        ctr.x = newWidth/2;
        ctr.y = newHeight/2;

        CIptBwImage imgEllipse;
        imgEllipse.vector2img(*pEllipse, ctr, newWidth, newHeight);
        imgEllipse.convexFill();
        delete pEllipse;

        CIptBwImage imgOriginal;
        imgOriginal.vector2img(*pOri, ctr, newWidth, newHeight);
        delete pOri;

        CIptBwImage imgXor;
        imgXor.xor(imgEllipse, imgOriginal);

        int nAreaDiff = imgXor.countForegroundPixel();
        int nAreaSlce = imgOriginal.countForegroundPixel();

		tmp = 1.0 - double(nAreaDiff)/double(nAreaSlce);

        sprintf(szBuffer, "%.4f", tmp);

        if(NULL != pVal)
            *pVal = tmp;

		return true;
	}
	

    if(1 == iMethod)
	{
		sprintf(szBuffer, "%.4f", ellipse_cd);

        if(NULL != pVal)
            *pVal = ellipse_cd;

        return true;
	}
	
    lstrcpy(szBuffer, "wrong");
	return false;
}


void CSlice::GetPrimaryAxis()
{
	CPixel upper,lower,left,right;
	
	upper.y = m_rtFruit.top;
	upper.x = ((m_rtFruit.left + m_rtFruit.right) >> 1);
	lower.y = m_rtFruit.bottom;
	lower.x = upper.x;

	CPixelList list;
	list = DrawLine(upper, lower);

	//find the actural upper.y and lower.y
	CPixelList::iterator myIter, IterEnd;
    IterEnd = list.end();
	for(myIter = list.begin(); myIter != IterEnd; ++myIter)
	{
		CPixel pixel = *myIter;
		if(m_pCurImage->GetPixelAt(pixel.x, pixel.y, 0)!=0)
		{
			upper.y = pixel.y;
			break;
		}
	}

	list = DrawLine(lower,upper);
	for(myIter = list.begin(); myIter != list.end(); myIter++)
	{
		CPixel pixel = *myIter;
		if(m_pCurImage->GetPixelAt (pixel.x,pixel.y,0)!=0)
		{
			lower.y = pixel.y;
			break;
		}
	}
	
    VerticalAxis = DrawLine(upper,lower);
/*
    //Get the position of index in current boundary
    int nFound = 0;

    CPixelList *    pContour = GetBoundary(1);

    CPixelList::iterator    iter = pContour->begin();
    int nContourLen = pContour->size();
    for(int i=0; i<nContourLen; i++)
    {
        if(iter->x==upper.x && iter->y==upper.y)
        {
            int nVertAxisUpperIndex = i;

            ++nFound;
            if(nFound==2)
                break;
        }

        if(iter->x==lower.x && iter->y==lower.y)
        {
            int nVertAxisLowerIndex = i;

            ++nFound;
            if(nFound==2)
                break;
        }

        ++iter;
    }
*/
}


void CSlice::GetHorizontalAxis()
{
    int nLeft, nRight, nTmp, iIndex, iMax, nMaxFruitWidth;

    CPixelList::iterator    Iter, IterEnd;
    Iter = VerticalAxis.begin();
    IterEnd = VerticalAxis.end();
    iIndex = nMaxFruitWidth = 0;
    while(Iter != IterEnd)
    {
        nTmp = getBinaryImageScanLineWidth(m_pCurImage, Iter->y, nLeft, nRight);
        if(nMaxFruitWidth < nTmp)
        {
            nMaxFruitWidth=nTmp;
            iMax = iIndex;
        }

        ++Iter;
        ++iIndex;
    }

    CPixel  tmp, pixL, pixR;
    tmp = VerticalAxis.GetAt(iMax);
    nTmp = getBinaryImageScanLineWidth(m_pCurImage, tmp.y, nLeft, nRight);
    pixL.x = nLeft; pixL.y = tmp.y;
    pixR.x = nRight; pixR.y = tmp.y;
    MaxFruitAxis = DrawLine(pixL, pixR);

    //compute the area under the max width axis
    m_AreaBelowMaxWidth = 0;
    nRight = m_pCurImage->GetWidth();
    for(iIndex=pixL.y; iIndex<=m_rtFruit.bottom; iIndex++)
    {
        unsigned char * ppix = m_pCurImage->GetBand(0)[iIndex];
        for(nLeft=0; nLeft<nRight; nLeft++)
        {
            if(0 != ppix[nLeft])
                ++m_AreaBelowMaxWidth;
        }
    }



    iIndex = ((m_rtFruit.top + m_rtFruit.bottom)>>1);
    getBinaryImageScanLineWidth(m_pCurImage, iIndex, nLeft, nRight);
    pixL.x = nLeft; pixL.y = iIndex;
    pixR.x = nRight; pixR.y = iIndex;
    MidHeightFruitAxis = DrawLine(pixL, pixR);

      
    
    return;
}

/*
void CSlice::GetWidestAxis()
{
	CPixel upper,lower,left,right;
	int imWidth = m_pCurImage->GetWidth();
	int imHeight = m_pCurImage->GetHeight();
	
	int width = 0;
	CPixel maxpix,prevpix,pix,nextpix;

	CPixelList list;
	list = m_DrawParameter.PrimaryAxis;

	CPixelList::iterator myIter=list.begin();

	//Setting  up prevpix, pix, and nextpix.
	prevpix = *myIter;
	++myIter;
	pix = *myIter;
	++myIter;
	nextpix = *myIter;
	++myIter;

	CPixelList::iterator IterEnd=list.end();
	while(myIter!= IterEnd)
	{
		//find the widest part
		prevpix = pix;
		pix = nextpix;
		nextpix = *myIter;

		float fWidth = GetWidth(prevpix, pix, nextpix, m_pCurImage);

		CWidthsPixel widthpixel;
		widthpixel.pixel = pix;
		widthpixel.width = fWidth;

		m_Parameter.m_WidthList.AddTail(widthpixel);

		if( fWidth>width)
		{
			width = fWidth;
			maxpix=pix;
		}
		myIter++;
	}

	int x = imWidth; int y = maxpix.y;

	//find the right and left point
	int value = m_pCurImage->GetPixelAt (x,y,0);
	while((value == 0)&&(x>=0))
	{
		x -= 1;
		value = m_pCurImage->GetPixelAt(x,y,0);
	}
	right.x= x;

	x = 0; y = maxpix.y;

	value = m_pCurImage->GetPixelAt(x,y,0);
	while((value == 0)&&(x<imWidth))
	{
		x += 1;
		value = m_pCurImage->GetPixelAt(x,y,0);
	}
	left.x= x;

	left.y = right.y = maxpix.y;

	m_DrawParameter.WidestAxis = DrawLine(left,right);

}
*/

/*
int fitcurvewithline(double x[], double y[], int nCurveLen, int& nLineNum, int iSelected[], double& slope, double& intcpt)
{
	int i, nOldNum;
	int bVertical;
	double e, newe;
	int * iSort=new int[nCurveLen];
	double * error=new double[nCurveLen];
	
	LinearRegression linReg;
	while(1)
	{
		linReg.Reset();

		for(i=0; i<nLineNum; i++)
			linReg.addXY(x+iSelected[i], y+iSelected[i], 1);

		linReg.Calculate();
		slope=linReg.getA();
		intcpt=linReg.getB();

		bVertical = linReg.IsVertical();
		if(!bVertical)
		{
			for(i=0; i<nCurveLen; i++)
				error[i] = pow(y[i] - slope*x[i] - intcpt, 2);
		}
		else
		{
			intcpt = linReg.getVertical();
			for(i=0; i<nCurveLen; i++)
				error[i] = pow(x[i] - intcpt, 2);
		}
		
		e = 0.0;
		for(i=0; i<nLineNum; i++)
			e += error[iSelected[i]];


		newe = 0.0;
		SortFromMinToMax(error, nCurveLen, iSort);
		for(i=0; i<nLineNum; i++)
			newe += error[i];
		
		if(newe>e)
			break;


		e /= nLineNum;
		nOldNum = nLineNum;

		i=nLineNum+1;
		newe += error[i];
		while((newe/i <= e) && i<nCurveLen)
		{
			++nLineNum;

			i++;
			newe += error[i];
		}


		if(nOldNum == nLineNum)
		{
			if(IsSameArray(iSort, iSelected, nLineNum))
				break;
		}
		
		for(i=0; i<nLineNum; i++)
			iSelected[i]=iSort[i];
	}

	delete[] iSort;
	delete[] error;

	return bVertical;
}
*/

/*
int fitcurvewithline(double x[], double y[], int nCurveLen, double& slope, double& intcpt)
{
	int i, nOldNum;
	int bVertical;
	double e, newe;

	int * iSort=new int[nCurveLen*2];
    int * iSelected = iSort+nCurveLen;
	double * error=new double[nCurveLen];

    int nLineNum = (nCurveLen>>2);
    for(i=0; i<nLineNum; i++)
        iSelected[i] = i;

	LinearRegression linReg;
	while(1)
	{
		linReg.Reset();

		for(i=0; i<nLineNum; i++)
			linReg.addXY(x+iSelected[i], y+iSelected[i], 1);

		linReg.Calculate();
		slope=linReg.getA();
		intcpt=linReg.getB();

		bVertical = linReg.IsVertical();
		if(!bVertical)
		{
			for(i=0; i<nCurveLen; i++)
				error[i] = pow(y[i] - slope*x[i] - intcpt, 2);
		}
		else
		{
			intcpt = linReg.getVertical();
			for(i=0; i<nCurveLen; i++)
				error[i] = pow(x[i] - intcpt, 2);
		}
		
		e = 0.0;
		for(i=0; i<nLineNum; i++)
			e += error[iSelected[i]];


		newe = 0.0;
		SortFromMinToMax(error, nCurveLen, iSort);
		for(i=0; i<nLineNum; i++)
			newe += error[i];
		
		if(newe>e)
			break;


		e /= nLineNum;
		nOldNum = nLineNum;

		i=nLineNum+1;
		newe += error[i];
		while((newe/double(i) <= e) && i<nCurveLen)
		{
			++nLineNum;

			i++;
			newe += error[i];
		}


		if(nOldNum == nLineNum)
		{
			if(IsSameArray(iSort, iSelected, nLineNum))
				break;
		}
		
		for(i=0; i<nLineNum; i++)
			iSelected[i]=iSort[i];
	}

	delete[] iSort;
	delete[] error;

	return bVertical;
}
*/

int fitcurvewithline(double x[], double y[], int nCurveLen, double& slope, double& intcpt)
{
	int bVertical;

	LinearRegression linReg;
    
    linReg.Reset();
    linReg.addXY(x, y, nCurveLen);

    linReg.Calculate();
	slope=linReg.getA();
	intcpt=linReg.getB();

	bVertical = linReg.IsVertical();
    return bVertical;
}

//Computer intersection of two straight lines
int ComputerIntersection(double * l1, double * l2, double& x, double& y)
{
	double a1, b1, c1;
	double a2, b2, c2;

	a1 = l1[0]; b1 = l1[1]; c1 = l1[2];
	a2 = l2[0]; b2 = l2[1]; c2 = l2[2];

	double d = a2*b1 - a1*b2;

	if(fabs(d)>DBL_EPSILON)
	{
		x = (c2*b1 - c1*b2)/(-d);
		y = (c2*a1 - c1*a2)/d;
		
		return 1;
	}
	
	d = fabs(c2*b1 - c1*b2);
	if(d>DBL_EPSILON)
		return 0;

	return 2;
}


#define EXCLUDE_PIXEL_NUM	20
#define REGRESSION_MAX_NUM	300
#define REGRESSION_MIN_NUM  50

void CSlice::CalcPointinessNew()
{
}


void CSlice::metric_taperness()
{
   	CPixel left,right,bottom;
    
    left   = MaxFruitAxis.front();
    right  = MaxFruitAxis.back();
    bottom = GetBoundary(1)->front();

    double dAreaTri = TriangleArea(left.x, left.y, bottom.x, bottom.y, right.x, right.y);
    double dRatio = fabs(dAreaTri)/double(m_AreaBelowMaxWidth);

    m_TapernessOne = dRatio * double(m_nFruitHeight) / double(m_nFruitWidth) / 0.64;
    m_TapernessTwo = dRatio;

    return;
}



void CSlice::DrawEdge(CDC* pDC, float scale)
{
	COLORREF	edgeClr = m_blExcludedFromSaving? RGB(0,0,196) : RGB(255,255,0);
    
    int nStep = int(((m_Rotation==0.0)? 1.0 : 4.0)/scale);

	int offsetx,offsety;
	offsetx = mBBox.mUpperLeft .x;
	offsety = mBBox.mUpperLeft .y;

    CPixelList * pContour = GetBoundary(0);
	CPixelList::iterator myIter = pContour->begin();
    int iloop = 0;
    int nSize = pContour->size();
    while(iloop<nSize)
    {
		CPixel pixel = *myIter;
		POINT point;
		point.x = (pixel.x + offsetx)*scale;
		point.y = (pixel.y + offsety)*scale;
		pDC->SetPixel (point, edgeClr);

        iloop++;
        ++myIter;
        for(int iStep=1; iStep<nStep; iStep++)
        {
            iloop++;
            ++myIter;
        }
    }
}

void CSlice::DrawElliptic(CDC* pDC)
{
    DrawSingleEdge(pDC, RGB(0,0,255), 0, 0);

	int a, b;

    a = round(ellipse_a);
    b = round(ellipse_b);


	std::vector<IPTPOINT>* pEllipse = ellipse(a, b);
	if(pEllipse==NULL)
		return;

	xy2ij(pEllipse, weight_ctrx, weight_ctry);

	std::vector<IPTPOINT>::iterator itr, itr_end;
	itr = pEllipse->begin();
	itr_end = pEllipse->end();
	while(itr != itr_end)
	{
		pDC->SetPixel(itr->x, itr->y, RGB(255, 255, 0));


		++itr;
	}

	delete pEllipse;
}

void CSlice::DrawRectangle(CDC* pDC)
{
    pDC->SelectObject(GetStockObject(NULL_BRUSH));

	CPen * pPen = new CPen;
	pPen->CreatePen(PS_SOLID, 1, RGB(255,0,0));
	CPen * pOldPen = pDC->SelectObject(pPen);

    pDC->Rectangle(&m_rtFruit);

    pDC->SelectObject(pOldPen);
    pPen->DeleteObject();
    pPen->CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	pOldPen = pDC->SelectObject(pPen);
    pDC->Rectangle(&m_rtInner);

    pDC->SelectObject(pOldPen);
    pPen->DeleteObject();
    delete pPen;
}

void CSlice::DrawBlocky(CDC* pDC, int iIndex)
{
    int nLeft, nRight, y;

    if(0==iIndex)
        y = blk_blm_pos;
    else
        y = blk_stm_pos;

	CPen * pPen = new CPen;
	pPen->CreatePen(PS_SOLID, 2, RGB(255,255,0));
	CPen * pOldPen = pDC->SelectObject(pPen);

    getBinaryImageScanLineWidth(m_pCurImage, y, nLeft, nRight);
    pDC->MoveTo(nLeft, y);
    pDC->LineTo(nRight, y);

    y = imgHeight/2;
    getBinaryImageScanLineWidth(m_pCurImage, y, nLeft, nRight);
    pDC->MoveTo(nLeft, y);
    pDC->LineTo(nRight, y);

    pDC->SelectObject(pOldPen);
    pPen->DeleteObject();
    delete pPen;
}

void CSlice::DrawTriangle(CDC * pDC)
{
	CPen * pPen = new CPen;
	pPen->CreatePen(PS_SOLID, 2, RGB(255,255,0));
	CPen * pOldPen = pDC->SelectObject(pPen);

    int y, nLeft, nRight;
    y = blk_stm_pos;
    getBinaryImageScanLineWidth(m_pCurImage, y, nLeft, nRight);
    pDC->MoveTo(nLeft, y);
    pDC->LineTo(nRight, y);

    y = blk_blm_pos;
    getBinaryImageScanLineWidth(m_pCurImage, y, nLeft, nRight);
    pDC->MoveTo(nLeft, y);
    pDC->LineTo(nRight, y);

    pDC->SelectObject(pOldPen);
    pPen->DeleteObject();
    delete pPen;
}

void CSlice::DrawCircular(CDC* pDC)
{
	int a, b;

    a = round(circle_r);
    b = round(circle_r);

	std::vector<IPTPOINT>* pEllipse = ellipse(a, b);
	xy2ij(pEllipse, circle_ox, circle_oy);

	std::vector<IPTPOINT>::iterator itr, itr_end;
	itr = pEllipse->begin();
	itr_end = pEllipse->end();
	while(itr != itr_end)
	{
		pDC->SetPixel(itr->x, itr->y, RGB(255, 255, 0));


		++itr;
	}

	delete pEllipse;
}


//must call this after calculated widthest axis
void CSlice::CalcShapeIndex()
{
}



void CSlice::DrawTaperness(CDC* pDC, int offsetx, int offsety)
{
	CPixel left,right,bottom;

    left   = MaxFruitAxis.front();
    right  = MaxFruitAxis.back();
    bottom = GetBoundary(1)->front();

	CPen * pPen = new CPen;
	pPen->CreatePen(PS_SOLID, 2, RGB(255,0,0));
	CPen * pOldPen = pDC->SelectObject(pPen);
	
	pDC->MoveTo(left.x,   left.y);
	pDC->LineTo(right.x,  right.y);
	pDC->LineTo(bottom.x, bottom.y);
	pDC->LineTo(left.x,   left.y);
	pDC->SelectObject(pOldPen);

    pPen->DeleteObject();
    delete pPen;
	
    DrawSingleEdge(pDC, RGB(255, 255, 0), offsetx, offsety);

}


void CSlice::SetIndex(int iIndex)
{
	m_iIndex = iIndex;
}

int CSlice::GetIndex()
{
	return m_iIndex;
}

void CSlice::DrawSingleEdge(CDC *pDC, COLORREF pixcolr, int offsetx, int offsety)
{
    DrawWeightCenter(pDC);

	CPixelList * pPixelList;
	pPixelList = GetBoundary(1);
	if(pPixelList->size () == 0)
		return;

	CPixelList::iterator myIter=pPixelList->begin();
	CPixelList::iterator IterEnd=pPixelList->end();
	for(; myIter!=IterEnd; ++myIter)
	{
        pDC->SetPixel(myIter->x, myIter->y, pixcolr);
	}
}


void CSlice::SetNewBoundary(CPixelList * pNewBoundary)
{
    CBoundingBox boxOrg = m_pCurBoundary->GetBBox();
    RECT    rect = boxOrg.GetRect();
    rect.bottom = -rect.bottom;
    RECT    rectPrev = GetBoundaryBoxAfterRotation(rect, -m_Rotation);

    CBoundingBox box = pNewBoundary->GetBBox();
    RECT    rt = box.GetRect();
    rt.bottom = -rt.bottom;
    RECT    rectAfter= GetBoundaryBoxAfterRotation(rt, -m_Rotation);

    mBBox.mUpperLeft.x  += (rectAfter.left - rectPrev.left);
    mBBox.mUpperLeft.y  += (rectAfter.top - rectPrev.top);
    mBBox.mLowerRight.x += (rectAfter.right - rectPrev.right);
    mBBox.mLowerRight.y -= (rectAfter.bottom - rectPrev.bottom);

    if(NULL != m_pCurBoundary)
        delete m_pCurBoundary;
    if(NULL != m_pOrgBoundary)
        delete m_pOrgBoundary;
    if(NULL != m_pOrgImage)
        delete m_pOrgImage;

    CAdvImage * pTmpImage = pNewBoundary->ContourToImage();
    m_pOrgImage = pTmpImage->Rotate(-m_Rotation);
    m_pOrgBoundary = m_pOrgImage->Contour();
    m_pCurBoundary = pTmpImage->Contour();
    delete pTmpImage;

    m_blBoundaryChanged = TRUE;
    CalcParameters(FALSE);
    return;
}

void CSlice::ResetBoundary()
{
    if(NULL != m_pCurBoundary)
    {
        delete m_pCurBoundary;
        m_pCurBoundary = NULL;
    }

    if(NULL != m_pOrgImage)
    {
        delete m_pOrgImage;
        m_pOrgImage = NULL;
    }

    if(NULL != m_pOrgBoundary)
    {
        delete m_pOrgBoundary;
        m_pOrgBoundary = NULL;
    }
    
    m_blBoundaryChanged = FALSE;
    return;
}

void CSlice::ResetRotation(BOOL bCalculate)
{
    m_Rotation = 0.0;

    if(NULL != m_pCurBoundary)
    {
        delete m_pCurBoundary;
        m_pCurBoundary = NULL;
    }

    m_blRotated = FALSE;

    if(bCalculate)
        CalcParameters(TRUE);

	return;
}

void CSlice::Rotate(double angle)
{
	ResetRotation(FALSE);
    m_Rotation = angle;

    if(0.0 != fabs(angle))
    {
        CAdvImage * pTmpImage = m_pOrgImage->Rotate(angle);
        m_pCurBoundary = pTmpImage->Contour();
        delete pTmpImage;

        m_blRotated = TRUE;
    }
    else
    {
        m_pCurBoundary = new CPixelList(*m_pOrgBoundary);        
    }

    CalcParameters(FALSE);
	return;
}

double CSlice::AutoRotate(double& mux, double& muy)
{
//	CPixelList * pShapePixel=GetContour();
//	CPixelList * pShapePixel=mBinObj;
	CPixelList * pShapePixel = new CPixelList();

	int i, j;
	for(i=0; i<m_pCurImage->GetHeight(); i++)
	{
		for(j=0; j<m_pCurImage->GetWidth(); j++)
		{
			if(m_pCurImage->GetPixelAt(j, i, 0)>0)
				pShapePixel->push_back(CPixel(j, i));
		}
	}

//	mSliceImage->Write("test.jpg");

	int nSize = pShapePixel->size();
	double * d[2];

	for(i=0; i<2; i++)
		d[i] = new double[nSize];

	CPixelList::iterator myIter=pShapePixel->begin();
	for(j=0; j<nSize; j++)
	{
		d[0][j]=myIter->x;
		d[1][j]=myIter->y;

		++myIter;
	}

	double rot;
	bool bRet = TwoDimPCA(d, nSize, rot, mux, muy);

	double degree = -rot*180.0/3.1415927;

	for(i=0; i<2; i++)
		delete d[i];

	delete pShapePixel;

	return degree;
}


int getBinaryImageScanLineWidth(CAdvImage* pImg, int y, int& nLeft, int& nRight)
{
    int x;
    int width = pImg->GetWidth();

    unsigned char * ppix = (pImg->GetBand(0))[y];
    
    nLeft = -1;
    for(x=0; x<width; x++)
    {
        if(ppix[x] == 255)
        {
            nLeft = x;
            break;
        }
    }
    if(nLeft == -1)
        return 0;
    
    nRight = 0;
    for(x=width-1; x>=0; x--)
    {
        if(ppix[x] == 255)
        {
            nRight = x;
            break;
        }
    }
    if(nRight == -1)
        return 0;

    return nRight-nLeft+1;
}


int avgPosition(int* d, int nLength)
{
	if(nLength==0)
		return 0;

    int iloop, davg;

    davg = 0;
    for(iloop=0; iloop<nLength; iloop++)
    {
        davg += d[iloop];
    }

    double dblavg = double(davg)/double(nLength);

    double * diff = new double[nLength];
    for(iloop=0; iloop<nLength; iloop++)
    {
        diff[iloop] = fabs(double(d[iloop]) - dblavg);
    }

    int pos=0;
    double dmin = diff[0];
    for(iloop=1; iloop<nLength; iloop++)
    {
        if(dmin > diff[iloop])
        {
            dmin = diff[iloop];
            pos = iloop;
        }
    }

    delete diff;

    return pos;
}

void CSlice::metric_bumpy()
{
    bumpy_radius = 0.0;

    CPixelList * pContour = GetBoundary(1);
    int nLength = pContour->size();
    CPixelList::iterator    iter = pContour->begin();

    double  tmpx = double(iter->x) - weight_ctrx;
    double  tmpy = double(iter->y) - weight_ctry;
    double  dmin = tmpx*tmpx + tmpy*tmpy;
    bumpy_radiusPos = 0;
    ++iter;

    for(int iIndex=1; iIndex<nLength; iIndex++)
    {
        tmpx = double(iter->x) - weight_ctrx;
        tmpy = double(iter->y) - weight_ctry;
        double  dst = tmpx*tmpx + tmpy*tmpy;
        if(dst < dmin)
        {
            dmin = dst;
            bumpy_radiusPos = iIndex;
        }

        ++iter;
    }

    bumpy_radius = sqrt(dmin);
    return;
}


bool CSlice::TapernessValue(int iMethod, char * szBuffer, double * pVal)
{
    if(0 == iMethod)
    {
        sprintf(szBuffer, "%.4f", m_TapernessOne);

        if(NULL != pVal)
            *pVal = m_TapernessOne;

        return true;
    }

    if(1 == iMethod)
    {
        sprintf(szBuffer, "%.4f", m_TapernessTwo);

        if(NULL != pVal)
            *pVal = m_TapernessTwo;

        return false;
    }

    return false;
}

bool CSlice::PointyValue(int iWhich, char * szBuffer, double * pVal)
{
    if(IDM_STEMEND_POINTY == iWhich)
    {
        sprintf(szBuffer, "%.1f", m_pnty.topAngle);
        if(NULL != pVal)
            *pVal = m_pnty.topAngle;

        return true;
    }

    if(IDM_BLSMEND_POINTY == iWhich)
    {
        sprintf(szBuffer, "%.1f", m_fTipAngle*180.0/pi);
        if(NULL != pVal)
            *pVal = m_pnty.btmAngle;

        return true;
    }

    return false;
}


bool  CSlice::BumpyValue(int iMethod, char * szBuffer, double * pVal)
{
    double tmp=0.0;

    if(0 == iMethod)
    {
        tmp = m_perimeter /(2.0*3.1416*bumpy_radius);
        sprintf(szBuffer, "%.4f", tmp);

        if(NULL != pVal)
            *pVal = tmp;

        return true;
    }

    if(1 == iMethod)
    {
        int nLobeCount = 0;

        if(NULL != m_pLobePosTemp)
            nLobeCount = m_pLobePosTemp->size();

        sprintf(szBuffer, "%d", nLobeCount);

        if(NULL != pVal)
            *pVal = nLobeCount;

        return true;
    }

    if(2 == iMethod)
    {
        double d1, d2, res;
        
        BumpyValue(1, szBuffer, &d1);
        BumpyValue(0, szBuffer, &d2);
        
        res = d1/d2;

        sprintf(szBuffer, "%.4f", res);
        
        if(NULL != pVal)
            *pVal = res;

        return true;
    }

    return false;
}

void CSlice::metric_pointy()
{
}


void CSlice::metric_computeAll()
{
    metric_IndentationAngle();

    metric_perimeter();

    metric_ellipse();

    metric_superquadric();

    metrix_circular();
    
    metrix_rectangular();
    
    metric_blocky();
    
    metric_obovate();
    
    metric_bumpy();
    
    metric_pointy();
    
    metric_taperness();

    CalcIndentation();

    m_computed = true;

    return;
}


void CSlice::ExportHeader(std::ostream& ostrm)
{
    std::vector<int>  measureItems = ((CTomatoAnalyzerApp *)AfxGetApp())->getMeasureItemsNeededToSave();
    std::vector<int>::iterator bg, ed;
    bg = measureItems.begin();
    ed = measureItems.end();

//    char szBuffer[256];
    while(bg != ed)
    {
 //       int nParentID, nMethod, nIndex;

        LPSTR lpName = GetSubMethodName((*bg)/10, (*bg) % 10);
        if(NULL != lpName)
            ostrm << lpName;

        ostrm << ",  ";
        
            

        //LPSTR lpName=GetMeasurementName(nIndex, nParentID, nMethod);
/*
        if(nMethod>1)
        {
            wsprintf(szBuffer, "%s.%d, ", lpName, ((*bg)%10) + 1 );
            lpName = szBuffer;
        }
        else
        {
            wsprintf(szBuffer, "%s, ", lpName);
            lpName = szBuffer;
        }

        ostrm << lpName;
*/
        ++bg;
    }

    return;
}


void CSlice::ExportValue(std::vector<double>& Vct)
{
    char   szBuffer[256];
    double dblValue;

    std::vector<int>  measureItems = ((CTomatoAnalyzerApp *)AfxGetApp())->getMeasureItemsNeededToSave();
    std::vector<int>::iterator bg, ed;
    bg = measureItems.begin();
    ed = measureItems.end();

    int iDescrpt, iMethod;
    while(bg != ed)
    {
        iDescrpt = (*bg)/10;
        iMethod  = (*bg)%10;

        szBuffer[0]=0;
        GetDescriptorValue(iDescrpt, iMethod, szBuffer, &dblValue);

        Vct.push_back(dblValue);

        ++bg;
    }
    
    return;
}


void CSlice::ExportValue(std::ostream& os)
{
    char   szBuffer[256];
    double dblValue;

    std::vector<int>  measureItems = ((CTomatoAnalyzerApp *)AfxGetApp())->getMeasureItemsNeededToSave();
    std::vector<int>::iterator bg, ed;
    bg = measureItems.begin();
    ed = measureItems.end();

    int iIndex = 1;
    int iDescrpt, iMethod;
    while(bg != ed)
    {
        iDescrpt = (*bg)/10;
        iMethod  = (*bg)%10;

        szBuffer[0]=0;
        GetDescriptorValue(iDescrpt, iMethod, szBuffer, &dblValue);

        os << szBuffer <<", ";

        ++iIndex;
        ++bg;
    }
    
    return;
}


void CSlice::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_iIndex;                                             
        ar << m_blExcludedFromSaving;
        
        ar << imgHeight << imgWidth << m_nFruitHeight << m_nFruitWidth;
        ar << m_rtFruit.left << m_rtFruit.top << m_rtFruit.right << m_rtFruit.bottom;
        ar << m_rtInner.left << m_rtInner.top << m_rtInner.right << m_rtInner.bottom;
        ar << m_viewRect.left << m_viewRect.top << m_viewRect.right << m_viewRect.bottom;

        ar << m_Rotation;
        ar << m_fShapeScale;

        ar << area << m_perimeter;
        ar << weight_ctrx << weight_ctry;
        ar << blk_blm << blk_blm_pos << blk_stm << blk_stm_pos;     //2
        ar << m_TapernessOne << m_TapernessTwo;
        ar << bumpy_radius << bumpy_radiusPos;
        ar << obovate_posTop << obovate_posMid << obovate_posBtm;

        ar << m_AreaBelowMaxWidth;
        
        ar << circle_cd << circle_ox << circle_oy << circle_r;      //5
        ar << ellipse_a << ellipse_b << ellipse_cd;                 //6
        ar << spellipse_n;

        ar << m_iTipStm << m_iTipBlm;
        ar << m_fBlmAngle << m_fStmAngle << m_fTipAngle;
        ar << m_Indentation << m_IndentBlm << m_TipArea;
        ar << m_IndentBlmPoints[0] << m_IndentBlmPoints[1];
        ar << m_TipEndPoints[0] << m_TipEndPoints[1];

        for(int i=0; i<3; i++)
        {
            ar << m_ptTipAngle[i].x << m_ptTipAngle[i].y;
            ar << m_ptBlmAngle[i].x << m_ptBlmAngle[i].y;
            ar << m_ptStmAngle[i].x << m_ptStmAngle[i].y;
        }

        
        ar << nLobePosLength;
        if(nLobePosLength>0)
        {
            std::vector<int>::iterator  iter = m_pLobePosTemp->begin();
            std::vector<int>::iterator  iend = m_pLobePosTemp->end();
            while(iter != iend)
            {
                ar << *iter;

                ++iter;
            }
        }
	}
	else
	{
		// TODO: add loading code here
		ar >> m_iIndex;                                             
        ar >> m_blExcludedFromSaving;
        
        ar >> imgHeight >> imgWidth >> m_nFruitHeight >> m_nFruitWidth;
        ar >> m_rtFruit.left >> m_rtFruit.top >> m_rtFruit.right >> m_rtFruit.bottom;
        ar >> m_rtInner.left >> m_rtInner.top >> m_rtInner.right >> m_rtInner.bottom;
        ar >> m_viewRect.left >> m_viewRect.top >> m_viewRect.right >> m_viewRect.bottom;

        ar >> m_Rotation;
        ar >> m_fShapeScale;

        ar >> area >> m_perimeter;
        ar >> weight_ctrx >> weight_ctry;
        ar >> blk_blm >> blk_blm_pos >> blk_stm >> blk_stm_pos;     //2
        ar >> m_TapernessOne >> m_TapernessTwo;
        ar >> bumpy_radius >> bumpy_radiusPos;
        ar >> obovate_posTop >> obovate_posMid >> obovate_posBtm;

        ar >> m_AreaBelowMaxWidth;
        
        ar >> circle_cd >> circle_ox >> circle_oy >> circle_r;      //5
        ar >> ellipse_a >> ellipse_b >> ellipse_cd;                 //6
        ar >> spellipse_n;

        ar >> m_iTipStm >> m_iTipBlm;
        ar >> m_fBlmAngle >> m_fStmAngle >> m_fTipAngle;
        ar >> m_Indentation >> m_IndentBlm >> m_TipArea;
        ar >> m_IndentBlmPoints[0] >> m_IndentBlmPoints[1];
        ar >> m_TipEndPoints[0] >> m_TipEndPoints[1];

        for(int i=0; i<3; i++)
        {
            ar >> m_ptTipAngle[i].x >> m_ptTipAngle[i].y;
            ar >> m_ptBlmAngle[i].x >> m_ptBlmAngle[i].y;
            ar >> m_ptStmAngle[i].x >> m_ptStmAngle[i].y;
        }

        
        ar >> nLobePosLength;
        if(nLobePosLength>0)
        {
            if(NULL == m_pLobePosTemp)
                m_pLobePosTemp = new std::vector<int>;
            
            m_pLobePosTemp->clear();
            m_pLobePosTemp->reserve(nLobePosLength);
            
            int ntemp;
            for(int i=0; i<nLobePosLength; i++)
            {
                ar >> ntemp;
                m_pLobePosTemp->push_back(ntemp);
            }
        }

        if(NULL == m_pOrgBoundary)
            m_pOrgBoundary = new CPixelList;

        m_pOrgBoundary->clear();

        m_computed = true;
    }

    mBBox.Serialize(ar);
    m_OrgBounderyBox.Serialize(ar);
    m_pOrgBoundary->Serialize(ar);

    if(ar.IsLoading())
    {
        //messy
        if(NULL == m_pOrgImage)
        {
            m_pOrgImage = new CAdvImage();
        }
        bwfill(m_pOrgBoundary, m_pOrgImage);

        if(NULL != m_pCurImage)
        {
            delete m_pCurImage;
            m_pCurImage = NULL;
        }
        m_pCurImage = m_pOrgImage->Rotate(m_Rotation);
        m_pCurBoundary = m_pCurImage->Contour();

    	GetPrimaryAxis();
	    GetHorizontalAxis();
    }
	return;
}

void CSlice::DrawWeightCenter(CDC * pDC)
{
    HBRUSH  hBrush, hBrushOld;
    hBrush = CreateSolidBrush(RGB(0,255,0));
    hBrushOld = (HBRUSH)pDC->SelectObject(hBrush);

    RECT    rect;
    int wx = (int)weight_ctrx;
    int wy = (int)weight_ctry;
    
    POINT   pt, ptorg;
    pt.x = pt.y = 3;
    ptorg.x = ptorg.y = 0;
    pDC->DPtoLP(&pt);
    pDC->DPtoLP(&ptorg);
    pt.x -= ptorg.x;
    pt.y -= ptorg.y;

    rect.left  = wx - pt.x;
    rect.right = wx + pt.x;
    rect.top   = wy - pt.y;
    rect.bottom= wy + pt.y;

    pDC->Ellipse(&rect);

    pDC->SelectObject(hBrushOld);
    DeleteObject(hBrush);

    return;
}

BOOL CSlice::WasManuallyAdjusted()
{
    return (m_blRotated || m_blBoundaryChanged);
}

void CSlice::metric_fractal()
{
    CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp * ) AfxGetApp();
    
    int nUnit=10;
    
    return;
}

double CSlice::GetAutoRotateAngle()
{
    return 0.0;
}

/*void CSlice::DrawVisualEffect(CDC * pDC, int iMeasurementItem)
{
    int iDrawIndex, iMethod;
    GetMeasurementIndexAndMethodID(iMeasurementItem, iDrawIndex, iMethod);
    if(GetExclusionFlag())
        iDrawIndex = IDM_AREA;

	switch(iDrawIndex)
	{
    case IDM_AREA:
	case IDM_PARIMETER:
         if(! GetExclusionFlag())
             DrawSingleEdge(pDC, RGB(255, 255, 0), 0, 0);
         else
             DrawSingleEdge(pDC, RGB(0, 0, 255), 0, 0);
		 break;

    case IDM_HIGHEST_HEIGHT:
    case IDM_WIDEST_WIDTH:
         DrawFruitFrame(pDC);
         break;

    case IDM_MIDWIDTH_HEIGHT:
    case IDM_MIDHEIGHT_WIDTH:
         DrawAxis(pDC, iDrawIndex);
         break;

	case IDM_ASPECT_RATIO:
		 DrawAspectRatio(pDC, iMethod);
		 break;

	case IDM_ELLIPTIC:
		 DrawElliptic(pDC);
		 break;

    case IDM_CIRCULAR:
         DrawCircular(pDC);
         break;

    case IDM_RECTANGULAR:
         DrawRectangle(pDC);
         break;

    case IDM_BLSMEND_BLOCKINESS:
         DrawBlocky(pDC, 0);
         break;

    case IDM_STEMEND_BLOCKINESS:
         DrawBlocky(pDC, 1);
         break;

    case IDM_OBOVATE:
         DrawObovate(pDC);
         break;

    case IDM_OVATE:
         DrawObovate(pDC);
         break;
         
    case IDM_BUMPINESS:
         DrawBumpy(pDC); 
         break;

    case IDM_STEMEND_POINTY:
         DrawPointy(pDC, iDrawIndex);
         break;

    case IDM_BLSMEND_POINTY:
         DrawIndentationAngleBlmEnd(pDC);
         break;

    case IDM_PEARSHAPE:
         break;

    case IDM_HEARSHAPE:
         break;

    case IDM_TAPERNESS:
         DrawTaperness(pDC, 0, 0);
         break;

    case IDM_STEMEND_SHUDRHEIGHT:
         DrawShoulder(pDC);
         break;

    case IDM_BLSMEND_INDENTATION:
         DrawIndentationBlm(pDC);
         break;

    case IDM_STEMEND_INDENTATION:
         DrawIndentation(pDC);
         break;

    case IDM_BLSMEND_LARGEANGLE:
         DrawLargeRegionPointy(pDC);
         break;

    case IDM_TRIANGLE:
         DrawTriangle(pDC);
         break;

    case IDM_TIPAREA:
         DrawTipArea(pDC);
         break;
	}

    return;
}*/


void CSlice::GetDisplayRegion(RECT* prt)
{
    if(m_Rotation != 0.0)
    {
        prt->left  = m_viewRect.left;
        prt->top   = m_viewRect.top;
        prt->right = m_viewRect.right;
        prt->bottom= m_viewRect.bottom;
    }
    else
    {
        prt->left  = mBBox.mUpperLeft.x;
        prt->top   = mBBox.mUpperLeft.y;
        prt->right = mBBox.mLowerRight.x;
        prt->bottom= mBBox.mLowerRight.y;
    }
    return;
}

void CSlice::SetDisplayRegion(RECT* prt)
{
    m_viewRect.left  = prt->left;
    m_viewRect.top   = prt->top;
    m_viewRect.right = prt->right;
    m_viewRect.bottom= prt->bottom;
}

extern int findCriticalPoint(CAdvImage *, double * xt, double * yt, int nLength, int, int *);
extern double * windowFilter(char *, int);
extern double * CircularFilter(double *, int, double *, int nHammingLength);
extern int indentationValue(CAdvImage * m_pCurImage, double * xt, double * yt, int nLength, int& nLeft, int& nRight);

void CSlice::metric_IndentationAngle()
{
	int nLength=GetBoundary(1)->size();
	if(nLength<=REGRESSION_MAX_NUM)
		return;

    double * xVal = new double[nLength*2];
    double * yVal = xVal + nLength;

    CPixelList *    pBorder = GetBoundary(1);
	CPixelList::iterator myIter = pBorder->begin();
    for(int i=0; i<nLength; i++)
    {
        xVal[i] = myIter->x;
        yVal[i] = myIter->y;

        ++myIter;
    }

	int nHammingLength=11;
	double * hamFilter = windowFilter("hamming", nHammingLength);
	double * xt = CircularFilter(xVal, nLength, hamFilter, nHammingLength);
	double * yt = CircularFilter(yVal, nLength, hamFilter, nHammingLength);
	delete hamFilter;


    CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp *) AfxGetApp();
    int nMacro = pApp->m_nMac;
    int nMicro = pApp->m_nMic;

    int iSigPoint[3];
    //int nSigNumber = DecideBlmSignificantPoint(m_pCurImage, xVal, yVal, nLength, weight_ctry, iSigPoint);
    int nSigNumber = findCriticalPoint(m_pCurImage, xt, yt, nLength, weight_ctry, iSigPoint);
    
    m_TipArea = 0;
    m_TipEndPoints[0] = m_TipEndPoints[1] = 0;
    if(2 == nSigNumber)
    {
        CPixel  pix;
        
        int nLeft = iSigPoint[0];
        int nRight = iSigPoint[1];
        CPixelList * plst = MakeLine(xVal[nLeft], yVal[nLeft], xt[nRight], yt[nRight]);
        
        ++nRight;
        while(nLeft != nRight)
        {
            pix.x = xVal[nLeft];
            pix.y = yVal[nLeft];
            plst->push_back(pix);
            
            ++nLeft;
            if(nLeft >= nLength)
                nLeft -= nLength;
        }
        
        int ntmp = bwfill(plst, NULL);
        if(ntmp > 10)
        {
            m_TipArea = ntmp;
            m_TipEndPoints[0] = iSigPoint[0];
            m_TipEndPoints[1] = nRight;
            
            m_iTipBlm = iSigPoint[0];
            nLeft = iSigPoint[0];
            while(nLeft != nRight)
            {
                if(yVal[nLeft]>yVal[m_iTipBlm])
                {
                    m_iTipBlm = nLeft;
                }
                
                ++nLeft;
                if(nLeft >= nLength)
                    nLeft -= nLength;
            }
            
        }
        else
            nSigNumber = 0;
        
        delete plst;
    }
        
    BOOL bUseLowestPoint = FALSE;
    if(2 != nSigNumber)
    {
        if(m_nFruitHeight >= (3 * m_nFruitWidth)/2)
        {
            m_iTipBlm = 0;
            bUseLowestPoint = TRUE;
        }
        else
        {
            if(nSigNumber == 1)
                m_iTipBlm = iSigPoint[0];
            else
                m_iTipBlm = m_suggestedbtmsigpos;
        }
    }

    int nOrg = m_iTipBlm; 
    int nFrom= 1;
    int nTo = nLength * nMicro / 200;
    m_fTipAngle = CalcRegressionAngle(xt, yt, nLength, nOrg, -1, nFrom, nTo, imgHeight, m_ptTipAngle, 50);
    if(m_fTipAngle<pi && nSigNumber==1 && !bUseLowestPoint)
    {
        m_iTipBlm = m_suggestedbtmsigpos;
        nOrg = m_iTipBlm; 
        m_fTipAngle = CalcRegressionAngle(xt, yt, nLength, nOrg, -1, nFrom, nTo, imgHeight, m_ptTipAngle, 50);
    }

    nFrom = nTo + 1;
    nTo = nLength * nMacro / 200;
    m_fBlmAngle = CalcRegressionAngle(xt, yt, nLength, nOrg, -1, nFrom, nTo, imgHeight, m_ptBlmAngle, 100);
    
/*
    if(bNoneLowest)
    {
        int iloop = 0;
        POINT pt[3];
        double ftmp;

        double chg = 0;
        nOrg = 0;
        while(iloop<10)
        {
            ftmp = CalcRegressionAngle(xt, yt, nLength, nOrg, -1, nFrom, nTo, imgHeight, pt, 50);

            int NewOrg = nLength * 3/4;
            int nEnd = nLength / 4;
            double xmin = fabs(xt[NewOrg] - pt[1].x);
            int i = NewOrg;
            while(i != nEnd)
            {
                double tmpdst = fabs(xt[i] - pt[1].x);
                if(tmpdst < xmin)
                {
                    xmin = tmpdst;
                    NewOrg = i;
                }

                ++i;
                if(i>=nLength)
                    i-=nLength;

            }

            double tmpx = xt[nOrg]-xt[NewOrg];
            double tmpy = yt[nOrg]-yt[NewOrg];
            chg = sqrt(tmpx*tmpx + tmpy*tmpy);

            nOrg = NewOrg;
            ++iloop;
        }

        
    }
*/

    if(m_fTipAngle>pi)
    {
        int nLeft = m_iTipBlm - 4;
        if(nLeft < 0)
            nLeft += nLength;
        
        int nRight = m_iTipBlm + 4;
        if(nRight >= nLength)
            nRight -= nLength;
        
        //FindConvexPointsAround(xt, yt, nLength, m_iTipBlm, nLeft, nRight);

        m_IndentBlm = indentationValue(m_pCurImage, xt, yt, nLength, nLeft, nRight);
        m_IndentBlmPoints[0] = nLeft;
        m_IndentBlmPoints[1] = nRight;
    }
    else
    {
        m_IndentBlm = 0;
    }
    
    delete xVal;
    delete xt;
    delete yt;
}

void CSlice::metric_IndentationAngle_Custom(POINT ptCenter)
{
	int nLength=GetBoundary(1)->size();
	if(nLength<=REGRESSION_MAX_NUM)
		return;

    double * xVal = new double[nLength*2];
    double * yVal = xVal + nLength;

    CPixelList *    pBorder = GetBoundary(1);
	CPixelList::iterator myIter = pBorder->begin();
    for(int i=0; i<nLength; i++)
    {
        xVal[i] = myIter->x;
        yVal[i] = myIter->y;

        ++myIter;
    }

    int NewOrg = nLength * 3/4;
    int nEnd = nLength / 4;
    double xmin = fabs(xVal[NewOrg] - ptCenter.x);
    i = NewOrg;
    while(i != nEnd)
    {
        double tmpdst = fabs(xVal[i] - ptCenter.x);
        if(tmpdst < xmin)
        {
              xmin = tmpdst;
              NewOrg = i;
        }

        ++i;
        if(i>=nLength)
            i-=nLength;
    }

    m_iTipBlm = NewOrg;


	int nHammingLength=11;
	double * hamFilter = windowFilter("hamming", nHammingLength);
	double * xt = CircularFilter(xVal, nLength, hamFilter, nHammingLength);
	double * yt = CircularFilter(yVal, nLength, hamFilter, nHammingLength);
	delete hamFilter;

    CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp *) AfxGetApp();
    int nMacro = pApp->m_nMac;
    int nMicro = pApp->m_nMic;

    int nOrg = m_iTipBlm; 
    int nFrom= 1;
    int nTo = nLength * nMicro / 200;
    m_fTipAngle = CalcRegressionAngle(xt, yt, nLength, nOrg, -1, nFrom, nTo, imgHeight, m_ptTipAngle, 50);

    nFrom = nTo + 1;
    nTo = nLength * nMacro / 200;
    m_fBlmAngle = CalcRegressionAngle(xt, yt, nLength, nOrg, -1, nFrom, nTo, imgHeight, m_ptBlmAngle, 100);

    if(m_fTipAngle>pi)
    {
        int nLeft = m_iTipBlm - 4;
        if(nLeft < 0)
            nLeft += nLength;
        
        int nRight = m_iTipBlm + 4;
        if(nRight >= nLength)
            nRight -= nLength;


        m_IndentBlm = indentationValue(m_pCurImage, xt, yt, nLength, nLeft, nRight);
        m_IndentBlmPoints[0] = nLeft;
        m_IndentBlmPoints[1] = nRight;
    }
    else
    {
        m_IndentBlm = 0;
    }
    


    delete xVal;
  
    delete xt;
    delete yt;
}


double CSlice::BtmFlatness(double * xVal, double * yVal, int nLength, int nMicro)
{
    int nBufferLen = 2*nMicro - 1;

    double * x = new double[nBufferLen*2];
    double * y = x + nBufferLen;

    for(int i=0; i<nMicro; i++)
    {
        x[i] = double(xVal[i]);
        y[i] = double(yVal[i]);
    }

    int iFrom = nLength - nMicro;
    for(i=nMicro; i<nBufferLen; i++)
    {
        x[i] = double(xVal[iFrom]);
        y[i] = double(yVal[iFrom]);

        ++iFrom;
    }
        
    LinearRegression lnRegress;
	lnRegress.addXY(x, y, nBufferLen);
	lnRegress.Calculate();
	double rs = lnRegress.getCoefDeterm();

    return rs;
}

double CSlice::CalcRegressionAngle(double * xVal, double * yVal, int nLength, int nOrg, int nDirection, int nFrom, int nTo, int H, POINT * ptAngle, int nDist)
{
    int nBufferLen = nTo - nFrom + 1;
	double * x=new double[nBufferLen*2];
	double * y=x+nBufferLen;

    int iPos = nOrg - nFrom * nDirection;
    for(int i=0; i<nBufferLen; i++)
    {
        if(iPos>=nLength)
            iPos -= nLength;
        else if(iPos < 0)
            iPos += nLength;

        x[i] = double(xVal[iPos]);
        y[i] = double(H) - double(yVal[iPos]);

        iPos -= nDirection;
    }

	int bVertical;
	double slope, intcpt;
    double coefL[3], coefR[3];

	bVertical = fitcurvewithline(x, y, nBufferLen, slope, intcpt);
	if(bVertical)
    {
		coefR[0]=1.0; 
        coefR[1]=0.0; 
        coefR[2]=-intcpt;
    }
    else
    {
		coefR[0]=slope; 
        coefR[1]=-1.0; 
        coefR[2]=intcpt;
	}

    iPos = nOrg + nFrom * nDirection;
    for(i=0; i<nBufferLen; i++)
    {
        if(iPos>=nLength)
            iPos -= nLength;
        else if(iPos < 0)
            iPos += nLength;

        x[i] = double(xVal[iPos]);
        y[i] = double(H) - double(yVal[iPos]);

        iPos += nDirection;
    }

	bVertical = fitcurvewithline(x, y, nBufferLen, slope, intcpt);
	if(bVertical)
	{
		coefL[0]=1; 
        coefL[1]=0; 
        coefL[2]=-intcpt;
	}
	else
	{
		coefL[0]=slope; 
        coefL[1]=-1; 
        coefL[2]=intcpt;
	}

	double ptAnglex[3], ptAngley[3];
    double xintc, yintc;
	int nNum = ComputerIntersection(coefL, coefR, xintc, yintc);
	if(nNum == 1)
	{
        ptAnglex[1] = xintc;
        ptAngley[1] = double(H) - yintc;

        if(coefL[1]==0.0)
        {
            ptAnglex[0] = xintc;
            ptAngley[0] = ptAngley[1] - 40.0;
        }
        else
        {
            ptAnglex[0] = xintc - 10;
            ptAngley[0] = double(H) - coefL[0] * ptAnglex[0] - coefL[2];
        }

        if(coefR[1]==0.0)
        {
            ptAnglex[2] = xintc;
            ptAngley[2] = ptAngley[1] - 40.0;
        }
        else
        {
            ptAnglex[2] = xintc + 10;
            ptAngley[2] = double(H) - coefR[0] * ptAnglex[2] - coefR[2];
        }
	}
    else
    {
        ptAnglex[1] = xVal[nOrg];
        ptAngley[1] = yVal[nOrg];

        
        int nL = nOrg + nDirection * ((nFrom + nTo)>>1);
        if(nL >= nLength)
            nL -= nLength;
        else if(nL < 0)
            nL += nLength;

        ptAnglex[0] = xVal[nL];
        ptAngley[0] = yVal[nL];

        
        int nR = nOrg - nDirection * ((nFrom + nTo)>>1);
        if(nR >= nLength)
            nR -= nLength;
        else if(nR < 0)
            nR += nLength;

        ptAnglex[2] = xVal[nR];
        ptAngley[2] = yVal[nR];
    }
	
	double angleVal = computeAngle(ptAnglex[2], ptAngley[2], ptAnglex[1], ptAngley[1], ptAnglex[0], ptAngley[0]);

    POINT   ptTip, ptEnd;
    ptTip.x = round(ptAnglex[1]);
    ptTip.y = round(ptAngley[1]);

    ptEnd = ExtendSegmentLine(ptAnglex[0], ptAngley[0], 
                              ptAnglex[1], ptAngley[1],
                              nDist
                             );
    ptAngle[0] = ptEnd;

    ptEnd = ExtendSegmentLine(ptAnglex[2], ptAngley[2], 
                              ptAnglex[1], ptAngley[1],
                              nDist
                             );
    ptAngle[2] = ptEnd;

    ptAngle[1] = ptTip;
   
    delete x;

	return angleVal;
}	

void CSlice::DrawIndentationBlm(CDC* pDC)
{
    if(0 >= m_IndentBlm)
        return;

    CBrush  hb;
    hb.CreateSolidBrush(RGB(0, 255, 0));
    pDC->SelectObject(&hb);
    pDC->SelectObject(GetStockObject(NULL_PEN));

    CPixelList * ppix = GetBoundary(1);
    int nLength = ppix->size();
    
    CPixel pix;
    int nFrom = m_IndentBlmPoints[0];
    while(nFrom != m_IndentBlmPoints[1])
    {
        CPixel pix = ppix->GetAt(nFrom);
        pDC->Rectangle(pix.x-1, pix.y-1, pix.x+1, pix.y+1);

        ++nFrom;
        if(nFrom >= nLength)
            nFrom -= nLength;
    }

    pDC->SelectObject(GetStockObject(BLACK_BRUSH));
    
    hb.DeleteObject();
    return;
}

void CSlice::DrawTipArea(CDC * pDC)
{
    if(0 >= m_TipArea)
        return;

    CBrush  hb;
    hb.CreateSolidBrush(RGB(0, 255, 0));
    pDC->SelectObject(&hb);
    pDC->SelectObject(GetStockObject(NULL_PEN));

    CPixelList * ppix = GetBoundary(1);
    int nLength = ppix->size();

    
    CPixel pix;
    int nFrom = m_TipEndPoints[0];
    while(nFrom != m_TipEndPoints[1])
    {
        CPixel pix = ppix->GetAt(nFrom);
        pDC->Rectangle(pix.x-1, pix.y-1, pix.x+1, pix.y+1);

        ++nFrom;
        if(nFrom >= nLength)
            nFrom -= nLength;
    }

    pDC->SelectObject(GetStockObject(BLACK_BRUSH));
    
    hb.DeleteObject();
    return;
}
