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
// RotationTracker.h: interface for the CRotationTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROTATIONTRACKER_H__DF96A677_6C47_469F_8D33_CA0BD6D6676E__INCLUDED_)
#define AFX_ROTATIONTRACKER_H__DF96A677_6C47_469F_8D33_CA0BD6D6676E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRotationTracker  
{
public:
	CRotationTracker();
	virtual ~CRotationTracker();

    void Draw(CDC * pDC, CAdvImage * pImage, POINT * ppt);
    BOOL ClickOnIndicator(POINT pt);
    double GetRotationAngle(POINT ptCur);
    POINT GetLeftTop(double CntrX, double CntrY, double angle);
    BOOL Track(CView* pView, CAdvImage * pImage, POINT ptOrg, double& angle);

private:
    POINT m_ptIndicator, m_ptCenter, m_ptOffset, m_ptDiffIndi;
    int m_nViewWidth, m_nViewHeight;

};

#endif // !defined(AFX_ROTATIONTRACKER_H__DF96A677_6C47_469F_8D33_CA0BD6D6676E__INCLUDED_)
