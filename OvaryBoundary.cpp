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
#include "stdafx.h"
#include "OvaryBoundary.h"
#include <math.h>
#include "ImgTool.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern std::vector<IPTPOINT>* circle(CPixel center, double radius); 

CPixelList * OvaryBoundary::modifyBoundary( CPixelList * boundary)
{
	int length = boundary->size();
	// check to see if length is larger than 3000 so that we know it is the ruler and can keep the original boundary
	if( length > 3000 )
		return boundary;
	//find the slopes of the endpoints of the arc, found in the smooth boundary
	vector<int> smoothBoundary = findSmoothBoundary( boundary );
	int SBlength = smoothBoundary.size();
	double firstSlope = (double)((boundary->GetAt(smoothBoundary[0]).y - boundary->GetAt(smoothBoundary[3]).y))/
						(boundary->GetAt(smoothBoundary[0]).x - boundary->GetAt(smoothBoundary[3]).x);
	double lastSlope = (double)((boundary->GetAt(smoothBoundary[SBlength - 1]).y - boundary->GetAt(smoothBoundary[SBlength - 4]).y))/
						(boundary->GetAt(smoothBoundary[SBlength - 1]).x - boundary->GetAt(smoothBoundary[SBlength - 4]).x);

	// find the perpendicular lines to those two slopes
	double perpFirstSlope = (-1)/firstSlope;
	double perpLastSlope = (-1)/lastSlope;

	// finds the equations of the perpendicular lines at the two ends of the arc
	double FirstB = boundary->GetAt(smoothBoundary[0]).y - (perpFirstSlope * boundary->GetAt(smoothBoundary[0]).x);
	double LastB = boundary->GetAt(smoothBoundary[SBlength - 1]).y - (perpLastSlope * boundary->GetAt(smoothBoundary[SBlength - 1]).x);

	// find the intersection of the perpendicular lines, that is the center
	double xCenter = (LastB - FirstB)/(perpFirstSlope - perpLastSlope);

	// use the x coordinate to find the y coordinate of the intersection point
	double yCenter = perpFirstSlope * xCenter + FirstB;

	// sets the weight center of ovary to be whatever we found as the intersection point of the lines
	weightCenter.x = xCenter;
	weightCenter.y = yCenter;

	// finds the radius as the distance from the endpoint to the center
	double radius = sqrt( pow((boundary->GetAt(smoothBoundary[0]).y - yCenter), 2) + pow((boundary->GetAt(smoothBoundary[0]).x - xCenter), 2));

	// fill the pixel list with the points that form a circle around that center point
	CPixel center(0, 0); 
	std::vector<IPTPOINT> * ovaryCircle = circle(center, radius); 
	CPixelList * ovaryBoundary = new CPixelList();

	// fills ovary boundary vector with the new modified boundary that we drew, it will always be a circle
	for( int k = 0; k < length; k++ )
	{
		std::vector<IPTPOINT>::iterator itr = ovaryCircle->begin();
		std::vector<IPTPOINT>::iterator itr_end = ovaryCircle->end();
		while(itr != itr_end)
		{
			CPixel pix;
			pix.x = itr->x;
			pix.y = itr->y;
			ovaryBoundary->push_back(pix);
	
			++itr;
		}
		
	}
	return ovaryBoundary;

}

// this will find the indices of the boundary list that are smooth, and we believe that the smooth part will
//  be able to give us the closest fitting circle that will then become the boundary for the ovary
// check to see if it's smooth by seeing if the slopes are close to each other or are going positive to negative

// later we might need to modify this in case it finds two parts on the ovary that are smooth!!!!!!!!!!!!!!!
		//Doesn't it already handle that by choosing the longest one? Is the above comment merely outdated? (7/11/06)
vector<int> OvaryBoundary::findSmoothBoundary( CPixelList * boundary )
{
	int nLength = boundary->size();
	// this will get filled with the indices of the two endpoints of the arc that is the longest, hopefully the 
	//    boundary of the smoothest part of the ovary
	vector<int> smoothOvaryBoundaryIndices;
	CPixelList::iterator myIter = boundary->begin();
    double * xVal = new double[nLength*2];
    double * yVal = xVal + nLength;
	// fills the xVal and yVal arrays with the original boundary points
	for( int i = 0; i < nLength; i++)
    {
        xVal[i] = myIter->x;
        yVal[i] = myIter->y;

        ++myIter;
    }
	double tempSlope = 0;
	double	previousSlope = 0; 
	int beginIndex = -1;
	int endIndex = -1;
	// longestArc will hold three points, the first one is the beginning index, the second is the last index, and the 
	//		third is the length of the arc
	int longestArc[3];
	longestArc[2] = 0;
	for( int j = 0; j < nLength; j += 10 )
	{
		int nextIndex;
		// make sure that the index is in bounds
		if (j + 10 < nLength)
			nextIndex = j + 10;
		else
			nextIndex = (j + 10) - nLength;
		// sets previous slope orginally then goes back to start the for loop again, don't need to go through the rest
		if (previousSlope == 0)
		{
			previousSlope = ( yVal[nextIndex] - yVal[j])/(xVal[nextIndex] - xVal[j]);
			continue;
		}
		else
			tempSlope = ( yVal[nextIndex] - yVal[j])/(xVal[nextIndex] - xVal[j]);
		double difference = abs(previousSlope - tempSlope);
		// if the difference between slopes is staying close then we think we might have a smooth part of the boundary
		if( (difference) < 1 && beginIndex == -1) // checks to see if the slopes are staying constant
			beginIndex = j;
		// while the slopes are staying constant keep updating endIndex until the slopes become jagged
		else if( difference < 1 )
			endIndex = j - 1;
		// see if we have an arc
		if( beginIndex != -1 && endIndex != -1 && difference >= 1 )
		{
			int limit;
			if (beginIndex > endIndex)
				limit = (nLength - beginIndex) + endIndex;
			else
				limit = endIndex - beginIndex;
			// this is a test that will find the longest section of the fruit that has similar slopes
			//   hopefully this will not let us find a section in the beginning that is a very large jagged part
			if(longestArc[2] < limit )
			{
				longestArc[0] = beginIndex;
				longestArc[1] = endIndex;
				longestArc[2] = limit;
				beginIndex = -1;
				endIndex = -1;
				// previousSlope = 0;
			}
		}
		previousSlope = tempSlope;
	}
	if( beginIndex == -1 )
		beginIndex = 0;
	else
		beginIndex = longestArc[0];
	if( endIndex == -1 )
		endIndex = nLength;
	else
		endIndex = longestArc[1];
	int cnt = 0;
	int cntLimit;
	if (beginIndex > endIndex)
		cntLimit = (nLength - beginIndex) + endIndex;
	else
		cntLimit = endIndex - beginIndex;
	int m = beginIndex;
	while (cnt < cntLimit)
	{
		if (m == nLength)
			m = 0;
		smoothOvaryBoundaryIndices.push_back(m);
		m++;
		cnt++;
	}

	return smoothOvaryBoundaryIndices;
}

CPixel OvaryBoundary::GetWeightCenter()
{
	return weightCenter;
}
