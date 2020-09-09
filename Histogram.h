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
// Histogram.h: interface for the CHistogram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTOGRAM_H__C9AAD19D_8CCE_413F_A4A0_F882956F6949__INCLUDED_)
#define AFX_HISTOGRAM_H__C9AAD19D_8CCE_413F_A4A0_F882956F6949__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAdvImage;

/**
 * CHistogram represents a histogram of L* values of pixels in an image. 
 */
class CHistogram  
{
private: 
	/// hist[i] is the number of pixels in the image analyzed having L* = i. 
	std::vector<int> hist; 

public:
	CHistogram(CRect rect, std::vector< std::vector<double> > lum);
	float avgInRange(int start, int end); 
	float avgLValueInRange(int start, int end); 
	int minIndexInRange(int start, int end); 
	int minIndexInRange() { return minIndexInRange(0, hist.size()); }
	int firstNonZeroIndex(); 
	int lastNonZeroIndex(); 
	int operator[](int i); 

};

#endif // !defined(AFX_HISTOGRAM_H__C9AAD19D_8CCE_413F_A4A0_F882956F6949__INCLUDED_)
