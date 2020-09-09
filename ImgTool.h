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
#ifndef _IPT_IMGTOOL_H
#define _IPT_IMGTOOL_H

#if !defined BYTE
typedef unsigned char BYTE;
#endif //BYTE

#include "vector"

namespace ipt {

class IPTPIXEL;

class IPTPOINT {
public:
    IPTPOINT() : x(0), y(0) {
    };
    
    IPTPOINT(int xval, int yval) : x(xval), y(yval){
    };
    
    IPTPOINT(const IPTPOINT& pt) : x(pt.x), y(pt.y){
    };

    IPTPOINT(const IPTPIXEL& pxl);


    IPTPOINT& toPoint(const IPTPIXEL& pxl, const IPTPIXEL& ctr);

    bool operator==(const IPTPOINT& other) const{ 
        return (x==other.x && y==other.y);
    };
    
    bool operator!=(const IPTPOINT& other) const{
        return (x!=other.x || y!=other.y);
    };

    IPTPOINT& operator = (const IPTPOINT& pt){
        x = pt.x;
        y = pt.y;
        return *this;
    };

    IPTPOINT operator + (const IPTPOINT& pt) const{
        return IPTPOINT(x+pt.x, y+pt.y);
    };

    IPTPOINT operator - (const IPTPOINT& pt) const{
        return IPTPOINT(x-pt.x, y-pt.y);
    };

public:
    int x, y;
};


class IPTPIXEL {
public:
    IPTPIXEL() : iRow(0), iCol(0) {
    }
    
    IPTPIXEL(int row, int col) : iRow(row), iCol(col){
    }
    
    IPTPIXEL(const IPTPIXEL& pxl) : iRow(pxl.iRow), iCol(pxl.iCol){
    }

    IPTPIXEL(const IPTPOINT& pt) : iRow(pt.y), iCol(pt.x){
    }

    IPTPIXEL&  toPixel(const IPTPOINT& pt, const IPTPOINT& ctr)
    {
        iRow = ctr.y - pt.y;
        iCol = pt.x + ctr.x;

        return *this;
    }

    IPTPIXEL&  operator = (const IPTPIXEL& other){
        iRow = other.iRow;
        iCol = other.iCol;
        return *this;
    }
    
        
    bool operator==(const IPTPIXEL& other){ 
        return (iRow==other.iRow && iCol==other.iCol);
    }

    bool operator!=(const IPTPIXEL& other){
        return (iRow!=other.iRow || iCol!=other.iCol);
    }

    IPTPIXEL operator + (const IPTPIXEL& pxl) const{
        return IPTPOINT(iRow+pxl.iRow, iCol+pxl.iCol);
    }

    IPTPIXEL operator - (const IPTPIXEL& pxl) const{
        return IPTPOINT(iRow-pxl.iRow, iCol-pxl.iCol);
    }

public:
    int iRow, iCol;
};


} // namespace ipt


using namespace ipt;
IPTPOINT  topright(const std::vector<IPTPOINT>& vec);
IPTPOINT  bottomleft(const std::vector<IPTPOINT>& vec);
IPTPIXEL  topleft(const std::vector<IPTPIXEL>& vec);
IPTPIXEL  bottomright(const std::vector<IPTPIXEL>& vec);


#endif //IPT_IMGTOOL_H