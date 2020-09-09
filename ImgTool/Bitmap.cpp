#include "stdafx.h"
#include "../Bitmap.h"

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

using namespace ipt;


bool CIptBitmap::LoadImage(const char * szFileName)
{
    FILE * pFile = fopen(szFileName, "rb");
    if(0 == pFile)
        return false;

    bool blRet = LoadImage(pFile);

    fclose(pFile);

    return blRet;
}

bool CIptBitmap::LoadImage(FILE * pFile)
{
//    BYTE *              pData = 0;
    BITMAPFILEHEADER    bmpfh;

    assert(pFile);

    if(1 == 
        fread(&bmpfh, sizeof(BITMAPFILEHEADER), 1, pFile))
    {
        if(DIB_HEADER_MARKER != bmpfh.bfType)
            return false;

        size_t cBmpInfoSize = bmpfh.bfOffBits - sizeof(BITMAPFILEHEADER);
        BYTE * pbmpInfo = new BYTE[cBmpInfoSize];
        if(0 == pbmpInfo)
            return false;

        size_t cBytesRead = fread(pbmpInfo, cBmpInfoSize, 1, pFile);
        assert(cBytesRead == 1);

        // Analyze the bitmapinfoheader structure
        ExtractBitmapInfo((BITMAPINFO *)(void *)pbmpInfo);

        // biSizeImage in BITMAPFILEHEADER is only image buffer size
        m_pData = new BYTE[m_bmpInfo.biSizeImage];
        if(0 == m_pData)
            return 0;

        cBytesRead = fread(m_pData, 1, m_bmpInfo.biSizeImage, pFile);
        assert(cBytesRead == m_bmpInfo.biSizeImage);

        return true;
    }

    return false;
}

/**
 * Save the bitmap into a file specified by szFileName
 * If the file exist, it will be destroyed first
 */
bool CIptBitmap::SaveImage(const char * szFileName) const
{
    return true;
}

/**
 * Create a CIptColorImage instance which contains the bitmap
 */
CIptColorImage * CIptBitmap::CreateIptColorImage()
{
    CIptColorImage * pImage;
    
    pImage = new CIptColorImage();
    if(! pImage->create(m_bmpInfo.biWidth, m_bmpInfo.biHeight))
    {
        delete pImage;
        pImage = NULL;
    }
    else
    {
/*        Pixel_iterator r, g, b, rend;
        r = pImage->begin(CIptColorImage::RGB_R);
        g = pImage->begin(CIptColorImage::RGB_G);
        b = pImage->begin(CIptColorImage::RGB_B);
        rend = pImage->end(CIptColorImage::RGB_R);
        
        while(r != rend)
        {
            *r++ = *m_pData++;
            *g++ = *m_pData++;
            *b++ = *m_pData++;
        }

*/
        Pixel_blk_iterator r, g, b;

        r = pImage->blk_begin(CIptColorImage::RGB_R);
        g = pImage->blk_begin(CIptColorImage::RGB_G);
        b = pImage->blk_begin(CIptColorImage::RGB_B);

        for(int iRow=0; iRow<m_bmpInfo.biHeight; iRow++)
        {
            for(int iCol=0; iCol<m_bmpInfo.biWidth; iCol++)
            {
                r[iRow][iCol] = *m_pData++;
                g[iRow][iCol] = *m_pData++;
                b[iRow][iCol] = *m_pData++;
            }
        }

    }

    return pImage;
}

/**
 * 
 */
void CIptBitmap::ExtractBitmapInfo(BITMAPINFO * pMem)
{
    PBITMAPINFOHEADER pHeader = (PBITMAPINFOHEADER)pMem;
    if(pHeader->biSize == sizeof(BITMAPCOREHEADER))
    {
        PBITMAPCOREHEADER   pCoreHeader = (PBITMAPCOREHEADER) pMem;
        memset(&m_bmpInfo, 0, sizeof(BITMAPINFOHEADER));

        m_bmpInfo.biHeight      = pCoreHeader->bcHeight;
        m_bmpInfo.biWidth       = pCoreHeader->bcWidth;
        m_bmpInfo.biBitCount    = pCoreHeader->bcBitCount;
        m_bmpInfo.biPlanes      = pCoreHeader->bcPlanes;

        m_bmpInfo.biSize        = sizeof(BITMAPINFOHEADER);
        m_bmpInfo.biCompression = BI_RGB;
    }
    else
    {
        memcpy(&m_bmpInfo, pHeader, sizeof(BITMAPINFOHEADER));
    }

    if(0 == m_bmpInfo.biSizeImage)
        m_bmpInfo.biSizeImage = m_bmpInfo.biHeight *   \
                                m_bmpInfo.biWidth *    \
                                m_bmpInfo.biBitCount * \
                                m_bmpInfo.biPlanes /8;

    m_pClrTbl = NULL;





    return;
}

bool CIptBitmap::ConvertIntoRGB24()
{
    assert(m_pData);

    bool blResult = false;
    switch(m_bmpInfo.biBitCount)
    {
        case 1:
            blResult=ConvertRGB2To24();
            break;

        case 8:
            blResult=ConvertRGB8To24();
            break;

        case 16:
            blResult=ConvertRGB16To24();
            break;

        case 24:
            blResult = true;
            break;

        case 32:
            blResult=ConvertRGB32To24();
            break;

        default:
            blResult = false;
            break;
    } // switch(m_bmpInfo.biBitCount)

    return blResult;
}

bool CIptBitmap::ConvertRGB2To24()
{
    return true;
}

bool CIptBitmap::ConvertRGB8To24()
{
    return true;
}

/**
 * Convert the 16-bit format into 24-bit format
 * 
 * 16-bit RGB format : xRRRRRGGGGGBBBBB
 * 24-bit RGB format : BBBBBBBB GGGGGGGG RRRRRRRR ...
 *
 * The scale, 255/31=8.226, should be applied to the original values
 * Approximately, scalor 8.25 is used as introduced by 
 * Microsoft vision SDK
 */
bool CIptBitmap::ConvertRGB16To24()
{
	int nNewImageSize = m_bmpInfo.biWidth*m_bmpInfo.biHeight*3;
	BYTE * pbtRGB24 = new BYTE[nNewImageSize];
	if(0 == pbtRGB24)
        return false;

    const unsigned short * pwRGB16 = (const unsigned short *)m_pData;
    for(int iRow=0; iRow<m_bmpInfo.biHeight; iRow++)
	{
        for(int iCol=0; iCol<m_bmpInfo.biWidth; iCol++)
		{
			*pbtRGB24++ = (((*pwRGB16 << 3) & 0x00f8) | ((*pwRGB16 >> 2) & 0x0007));
			*pbtRGB24++ = (((*pwRGB16 >> 2) & 0x00f8) | ((*pwRGB16 >> 7) & 0x0007));
			*pbtRGB24++ = (((*pwRGB16 >> 7) & 0x00f8) | ((*pwRGB16++ >> 12) & 0x0007));
		}
	}

    delete m_pData;
    m_pData = pbtRGB24;

    return true;
}

bool CIptBitmap::ConvertRGB32To24()
{
    return true;
}

