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
#ifndef _IPT_COLORIMAGE_H
#define _IPT_COLORIMAGE_H

#include "ImageCore.h"
#include "GrayImage.h"

namespace ipt {

class CIptColorImage : public CIptMulImage<CIptGrayImage, 3> {
public:
    enum tagColorSpace {RGB_SPACE, YUV_SPACE, HSV_SPACE} COLOR_SPACE;
    enum tagRgbChannel {RGB_R, RGB_G, RGB_B} RGB_CHANNEL;
    enum tagYuvChannel {YUV_Y, YUV_U, YUV_V} YUV_CHANNEL;

public:
   CIptColorImage();

   CIptColorImage * Clone();
}; // CIptColorImage






}; // namespace ipt

#endif // _IPT_COLORIMAGE_H