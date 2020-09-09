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
// Pixel.cpp: implementation of the CPixel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Pixel.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPixel::CPixel()
{
	x = 0; y = 0;
}

CPixel::CPixel(const CPixel & inPixel)
{
	x = inPixel.x;
	y = inPixel.y;

}

CPixel::CPixel(int inX, int inY)
{
	x = inX; y = inY;
}
void CPixel::Set(int inX, int inY)
{
	x = inX; y = inY;
}

CPixel::~CPixel()
{
	x=1;
}

void CPixel::Swap(CPixel & inPix)
{
	CPixel temp;
	temp = inPix;
	inPix = *this;
	this->Set(temp.x, temp.y);
}

float CPixel::Distance(CPixel inPix)
{
	float dx, dy;
	
	dx = (float)x - (float)inPix.GetX();
	dy = (float)y - (float)inPix.GetY();
	
	return (float)::sqrt(dx*dx + dy*dy);	

}

bool CPixel::IsAdjacent(CPixel inPix)
{
	if(inPix.x == (x + 1))
	{
		return true;
	}
	else if(inPix.x == (x - 1))
	{
		return true;
	}
	else if(inPix.y == (y + 1))
	{
		return true;
	}
	else if(inPix.y == (y - 1))
	{
		return true;
	}

	return false;
}


bool CPixel::operator<(const CPixel& inPixel) const
{
	if(y < inPixel.y)
	{
		return true;
	}
	else if(y > inPixel.y)
	{
		return false;
	}
	if(x < inPixel.x)
		return true;
	return false;
}

bool CPixel::operator==(const CPixel& inPixel) const
{
	return ((x == inPixel.x) && (y == inPixel.y));
}

bool CPixel::operator>(const CPixel& inPixel) const
{
	bool result;

	if(*this < inPixel)
		result = false;
	else
	{
		if(*this == inPixel)
			result = false;
		else	
			result = true;
	}
	return result;
}


ostream & operator<<(ostream & outs, const CPixel inPixel)
{
	outs<<"\n(x,y) = ("<<inPixel.x<<", "<<inPixel.y<<")";
	return outs;

}


void CPixel::SwapF(float & f1, float & f2)
{
	float temp = f1;
	f1 = f2;
	f2 = temp;
}

bool CPixel::IsNeighbor(CPixel & inP1)
{
	CPixel inP2;

	inP2.Set(this->x, this->y);

	//Above
	if((inP1.x == inP2.x) && (inP1.y == inP2.y+1))
		return true;
	//Below
	if((inP1.x == inP2.x) && (inP1.y == inP2.y-1))
		return true;
	//Left of
	if((inP1.x == inP2.x - 1) && (inP1.y == inP2.y))
		return true;
	//Right of
	if((inP1.x == inP2.x + 1) && (inP1.y == inP2.y))
		return true;

	//CORNERS
	if(inP1.x == inP2.x + 1)
	{
		//Bottom Right
		if(inP1.y == inP2.y - 1)	
			return true;
		//Top Right
		if(inP1.y == inP2.y + 1)
			return true;
	}

	if(inP1.x == inP2.x - 1)
	{
		//Bottom Left
		if(inP1.y == inP2.y - 1)	
			return true;
		//Top Left
		if(inP1.y == inP2.y + 1)
			return true;
	}
	return false;
}
