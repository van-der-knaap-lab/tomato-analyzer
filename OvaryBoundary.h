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
// finds the correct boundary for the ovary pictures, hopefully

//#ifndef OVARYBOUNDARY_H
//#define OVARYBOUNDARY_H

#if !defined(AFX_OVARYBOUNDARY_H__ED22F7BD_488F_4AE4_92B1_2680E44CAF23__INCLUDED_)
#define AFX_OVARYBOUNDARY_H__ED22F7BD_488F_4AE4_92B1_2680E44CAF23__INCLUDED_

#include "PixelList.h"
#include <vector>

using namespace std;

class OvaryBoundary
{
public:
	// method that takes in the boundary list that is orginally found as a parameter
	// this then modifies the boundary so that what it returns should be the updated perimeter
	// the updated perimeter will be a pixel list of a circle that should enclose the ovary
	CPixelList * modifyBoundary(CPixelList * boundary);
	// this will get their weight center from where it originally was so that we can reset it
	CPixel GetWeightCenter();

private:
	// utility method that actually finds where the smooth part of the boundary of the ovary is so that it can be used
	// for further calculations such as where the new weight center is
	std::vector<int> findSmoothBoundary( CPixelList * boundary );
	// variable that will store the center of the tomato
	CPixel weightCenter;
};

#endif