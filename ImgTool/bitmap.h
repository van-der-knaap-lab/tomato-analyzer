#ifndef _IPT_BITMAP_H
#define _IPT_BITMAP_H

#include "windows.h"
#include "../ImageCore.h"
#include "ColorImage.h"

/** 
 * Not used? 
 */
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