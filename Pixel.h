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
// Pixel.h: interface for the CPixel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIXEL_H__1019828A_AED0_4F65_BCAA_713D8DADED9D__INCLUDED_)
#define AFX_PIXEL_H__1019828A_AED0_4F65_BCAA_713D8DADED9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <iostream>

using namespace std;

class CPixel 
{
public:
	CPixel();
	CPixel(const CPixel & inPixel);
	CPixel(int inX, int inY);
	void Set(int inX, int inY);
	bool IsAdjacent(CPixel inPix);
	bool IsNeighbor(CPixel & inP1);
	int GetX()	{ return x;  }
	int GetY() { return y;	 }

	static void SwapF(float & f1, float & f2);

	virtual ~CPixel();
	void Swap(CPixel & inPix);
	int x, y;
	
	//Calculate the distance between two pixels.
	float Distance(CPixel inPix);

	//Operator overloads
	bool operator< (const CPixel& inPixel) const;
	bool operator> (const CPixel& inPixel) const;
	bool operator==(const CPixel& inPixel) const;

};

ostream & operator<<(ostream &, const CPixel);
#endif // !defined(AFX_PIXEL_H__1019828A_AED0_4F65_BCAA_713D8DADED9D__INCLUDED_)
