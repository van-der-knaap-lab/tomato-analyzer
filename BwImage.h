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
#ifndef _IPT_BWIMAGE_H
#define _IPT_BWIMAGE_H

#include "ImgTool.h"
#include "ImageCore.h"

namespace ipt {

class CIptBwImage : public CIptImage<BYTE> {
public:
    CIptBwImage();

public:
    void contour();
    
    CIptBwImage& vector2img(const std::vector<IPTPOINT>& vec, 
                             IPTPOINT ctr, int width=0, int height=0);

    CIptBwImage& vector2img(const std::vector<IPTPIXEL>& vec, 
                             IPTPIXEL ctr, int width=0, int height=0);

    std::vector<IPTPIXEL> *  img2vector(void);
    std::vector<IPTPOINT> *  img2vector(const IPTPOINT& pt);
    
    int countForegroundPixel() const;
    CIptBwImage& convexFill(void);




}; // CIptBwImage


} // namespace ipt

#endif // _IPT_BWIMAGE_H
