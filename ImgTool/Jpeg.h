#ifndef _IPT_JPEG_H
#define _IPT_JPEG_H

#include "ColorImage.h"

/**
 * Not used? 
 */
namespace ipt {

    class CIptJpeg {
    public:
        CIptJpeg();
        CIptJpeg(const char * szFileName);

        bool LoadImage(const char * szFileName);
        bool LoadImage(FILE * pFile);

        bool SaveImage(const char * szFileName) const;

        CIptColorImage *    CreateIptColorImage(void);
    }; // class CIptJpeg

} // namespace ipt

#endif // _IPT_JPEG_H