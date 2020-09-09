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
#ifndef _IPT_SHAPECONTOUR_H
#define _IPT_SHAPECONTOUR_H

#include "vector"
#include "ImgTool.h"

namespace ipt {

class CIptContour : public std::vector<IPTPOINT> {
public:
    virtual int perimeter(void);
    virtual int area(void);

private:
       



}; // class CIptContour


class CIptEllipse : public CIptContour {
public:
    CIptEllipse(int a, int b);

public:
    setRadius(int a, int b);
    getRadius(int& a, int& b);




}; // namespace ipt

#endif // _IPT_SHAPECONTOUR_H