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
#ifndef _IPT_BITMAP_H
#define _IPT_BITMAP_H

#include "windows.h"
#include "ImageCore.h"
#include "ColorImage.h"

namespace ipt {

    class CIptBitmap {
    public:
        bool LoadImage(const char * szFileName);
        bool LoadImage(FILE * pFile);

        bool SaveImage(const char * szFileName) const;

        CIptColorImage *    CreateIptColorImage(void);

        bool ConvertIntoRGB24();
        bool isGrayScale(void) const;



    private:
        void ExtractBitmapInfo(BITMAPINFO * pMem);

        /**
         * Convert into 24-bit Format from other formats
         */
        bool ConvertRGB2To24();
        bool ConvertRGB8To24();
        bool ConvertRGB16To24();
        bool ConvertRGB32To24();




    private:
        int                 m_nImageSize;
        BYTE *              m_pData;
        RGBQUAD *           m_pClrTbl;
        BITMAPINFOHEADER    m_bmpInfo;













    }; // class CIptBitmap

} // namespace ipt 

#endif //IPT_BITMAP_H