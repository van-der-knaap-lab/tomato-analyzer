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
#ifndef _IPT_GRAYLEVELIMAGE_H
#define _IPT_GRAYLEVELIMAGE_H

#include "ImageCore.h"

namespace ipt {

class CIptGrayImage : public CIptImage<BYTE> {
public:
    typedef CIptImage<BYTE>::iterator Pixel_Iterator;

public:
    CIptGrayImage();
    CIptGrayImage(const CIptGrayImage& cpy);
    CIptGrayImage& operator=(const CIptGrayImage& other);

    int *   histogram();


public:

    
private:

}; // CIptGrayLevelImage


}; // namespace ipt

#endif // _IPT_GRAYLEVELIMAGE_H
