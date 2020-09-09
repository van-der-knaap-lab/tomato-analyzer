#include "stdafx.h"
#include "../ImageCore.h"
#include "../jpeg.h"

using namespace ipt;

CIptJpeg::CIptJpeg()
{
}


bool CIptJpeg::LoadImage(const char *szFileName)
{
    FILE * pFile = fopen(szFileName, "rb");
    if(0 == pFile)
        return false;

    bool blRet = LoadImage(pFile);

    fclose(pFile);

    return blRet;
}


bool CIptJpeg::LoadImage(FILE * pFile)
{
    return false;
}


CIptColorImage * CIptJpeg::CreateIptColorImage()
{
    return 0;
}


