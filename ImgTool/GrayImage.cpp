#include "stdafx.h"
#include "../GrayImage.h"

using namespace ipt;

CIptGrayImage::CIptGrayImage()
{
}

CIptGrayImage::CIptGrayImage(const CIptGrayImage& cpy)
{
    *this = cpy;
}

CIptGrayImage& CIptGrayImage::operator = (const CIptGrayImage& other)
{
    return *this;
}
