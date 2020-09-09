#include "stdafx.h"
#include "../ImgTool.h"

using namespace ipt;
using namespace std;

inline 
IPTPOINT::IPTPOINT(const IPTPIXEL& pxl)
           : x(pxl.iCol), y(pxl.iRow)
{
}


inline
IPTPOINT& IPTPOINT::toPoint(const IPTPIXEL& pxl, const IPTPIXEL& ctr)
{
    x = pxl.iCol-ctr.iCol;
    y = ctr.iRow-pxl.iRow;

    return *this;
}


//IPTPOINT  bottomleft(const std::vector<IPTPOINT> vec)
IPTPOINT  bottomleft(const std::vector<IPTPOINT>& vec)
{
    std::vector<IPTPOINT>::const_iterator iter, iter_end;
    iter = vec.begin();
    iter_end = vec.end();
    
    int xmin = iter->x;
    int ymin = iter->y;
    
    while(++iter != iter_end)
    {
        if(xmin > iter->x)
            xmin = iter->x;
        
        if(ymin > iter->y)
            ymin = iter->y;
    } //while(++iter != iter_end)

    return IPTPOINT(xmin, ymin);
}


IPTPOINT  topright(const std::vector<IPTPOINT>& vec)
{
    std::vector<IPTPOINT>::const_iterator iter, iter_end;
    iter = vec.begin();
    iter_end = vec.end();
    
    int xmax = iter->x;
    int ymax = iter->y;
    
    while(++iter != iter_end)
    {
        if(xmax < iter->x)
            xmax = iter->x;
        
        if(ymax < iter->y)
            ymax = iter->y;
    } //while(++iter != iter_end)

    return IPTPOINT(xmax, ymax);
}


IPTPIXEL  topleft(const std::vector<IPTPIXEL>& vec)
{
    std::vector<IPTPIXEL>::const_iterator iter, iter_end;
    iter = vec.begin();
    iter_end = vec.end();
    
    int rmin = iter->iRow;
    int cmin = iter->iCol;
    
    while(++iter != iter_end)
    {
        if(rmin > iter->iRow)
            rmin = iter->iRow;
        
        if(cmin > iter->iCol)
            cmin = iter->iCol;
    } //while(++iter != iter_end)

    return IPTPOINT(rmin, cmin);
}


IPTPIXEL  bottomright(const std::vector<IPTPIXEL>& vec)
{
    std::vector<IPTPIXEL>::const_iterator iter, iter_end;
    iter = vec.begin();
    iter_end = vec.end();
    
    int rmax = iter->iRow;
    int cmax = iter->iCol;
    
    while(++iter != iter_end)
    {
        if(rmax < iter->iRow)
            rmax = iter->iRow;
        
        if(cmax < iter->iCol)
            cmax = iter->iCol;
    } //while(++iter != iter_end)

    return IPTPOINT(rmax, cmax);
}


std::vector<IPTPIXEL>* toPixel(const std::vector<IPTPOINT>& ptvec, IPTPOINT ctr)
{
    std::vector<IPTPIXEL>*  ret = new std::vector<IPTPIXEL>;
    if(0 != ret)
    {
        IPTPIXEL  pxl;

        std::vector<IPTPOINT>::const_iterator  iter, iter_end;
        iter = ptvec.begin();
        iter_end = ptvec.end();

        while(iter != iter_end)
        {
            pxl.toPixel(*iter, ctr);
            ret->push_back(pxl);
        }
    }

    return ret;
}

/*
IPTPOINT  min(const IPTPOINT& A, const IPTPOINT& B)
{
    IPTPOINT    pt;

    pt.x = (A.x > B.x)? B.x: A.x;
    pt.y = (A.y > B.y)? B.y: A.y;

    return pt;
}


IPTPOINT  max(const IPTPOINT& A, const IPTPOINT& B)
{
    IPTPOINT    pt;

    pt.x = (A.x > B.x)? A.x: B.x;
    pt.y = (A.y > B.y)? A.y: B.y;

    return pt;
}
*/
