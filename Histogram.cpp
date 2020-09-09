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
// Histogram.cpp: implementation of the CHistogram class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "AdvImage.h"
#include "Histogram.h"
#include <string.h>
#include <assert.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Builds a histogram of L* values of pixels within the given rectangle of an image, where 
 * r, g, and b are the red, green, and blue bands of the image. 
 */
CHistogram::CHistogram(CRect rect, std::vector< std::vector<double> > lum)
{
	CPoint tl = rect.TopLeft();
	CPoint lr = rect.BottomRight();

	hist.resize(101, 0);				// L* ranges from 0 through 100
	for(int i = tl.y; i < lr.y; i++)
	{
		for(int j = tl.x; j < lr.x; j++)
		{
			int roundedLum = (int) (lum[i][j] + 0.5); 
			hist[roundedLum]++; 
		}
	}
}

/**
 * Returns the average of histogram elements in index range [start, end), 
 * adjusting start and end if necessary to keep them within bounds. 
 * Returns 0 if there are no histogram elements in range. 
 */
float CHistogram::avgInRange(int start, int end) {
	if (start < 0) 
		start = 0; 
	if (end > hist.size()) 
		end = hist.size(); 
	float sum = 0; 
	for (int i = start; i < end; i++) {
		sum += hist[i]; 
	}
	int range = end - start; 
	return range > 0 ? sum/range : 0; 
}

/** 
 * Returns the average of L* values in index range [start, end), 
 * adjusting start and end if necessary to keep them within bounds. 
 * Returns 0 if there are no histogram elements in range. 
 */
float CHistogram::avgLValueInRange(int start, int end) {
	if (start < 0) 
		start = 0; 
	if (end > hist.size()) 
		end = hist.size(); 
	float sum = 0; 
	int count = 0; 
	for (int i = start; i < end; i++) {
		sum += hist[i] * i; 
		count += hist[i]; 
	}
	return count > 0 ? sum/count : 0; 
}

/**
 * Returns the index of the minimum histogram element in index range [start, end), 
 * adjusting start and end if necessary to keep them within bounds. 
 * Returns 0 if there are no histogram elements in range. 
 */
int CHistogram::minIndexInRange(int start, int end) {
	if (start < 0) 
		start = 0; 
	if (end > hist.size()) 
		end = hist.size(); 
	if (end - start > 0) {
		int min = start; 
		for (int i = start + 1; i < end; i++) {
			if (hist[i] < hist[min]) 
				min = i; 
		}
		return min; 
	} else {
		return 0; 
	}
}

/** 
 * Returns the smallest index where the histogram element is greater than 0, 
 * or the last index + 1 if all elements are 0. 
 */
int CHistogram::firstNonZeroIndex() {
	for (int i = 0; i < hist.size(); i++) {
		if (hist[i] > 0) 
			return i; 
	}
	return hist.size(); 
}

/** 
 * Returns the largest index where the histogram element is greater than 0, 
 * or -1 if all elements are 0. 
 */
int CHistogram::lastNonZeroIndex() {
	for (int i = hist.size() - 1; i >= 0; i--) {
		if (hist[i] > 0) 
			return i; 
	}
	return -1; 
}

int CHistogram::operator[](int i) {
	return hist[i]; 
}

