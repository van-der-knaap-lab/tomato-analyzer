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
#include "stdafx.h"
#include "AdvImage.h"

using namespace std;


int bwfill(CPixelList * pCurve, CAdvImage * pOutput)
{
    if(NULL == pCurve)
        return 0;

    if(pCurve->empty())
        return 0;

    CPixelList::iterator    iter = pCurve->begin();
    CPixelList::iterator    iend = pCurve->end();

    RECT    rect;
    rect.left = rect.right = iter->x;
    rect.top = rect.bottom = iter->y;
    ++iter;

    while(iter != iend)
    {
        if(rect.left > iter->x)
            rect.left = iter->x;
        else if(rect.right < iter->x)
            rect.right = iter->x;

        if(rect.top > iter->y)
            rect.top = iter->y;
        else if(rect.bottom < iter->y)
            rect.bottom = iter->y;

        ++iter;
    }

    int nImgWidth = rect.right - rect.left + 5;
    int nImgHeight= rect.bottom - rect.top + 5;

//    CAdvImage * pOrg = new CAdvImage();
//    pOrg->CreateGrayScale(nImgWidth, nImgHeight);

    CAdvImage * pTobeFilled = new CAdvImage();
    pTobeFilled->CreateGrayScale(nImgWidth, nImgHeight);
	unsigned char** ppix = (unsigned char**)pTobeFilled->mImage->image_ptr[0]->rptr;

    int ntemp = nImgHeight - 1;
    for(int i=0; i<nImgWidth; i++)
        ppix[0][i] = ppix[ntemp][i] = 255;

    ntemp = nImgWidth - 1;
    for(int i=0; i<nImgHeight; i++)
        ppix[i][0] = ppix[i][ntemp] = 255;

    std::vector<POINT> srtrack;
    srtrack.reserve(2*(nImgWidth-2 + nImgHeight - 2) - 4);
    POINT   pt;
    for(int i=1; i<nImgWidth-1; i++)
    {
        pt.x = i;
        pt.y = 1;
        srtrack.push_back(pt);

        pt.y = nImgHeight-2;
        srtrack.push_back(pt);
    }
    for(int i=1; i<nImgHeight-1; i++)
    {
        pt.x = 1;
        pt.y = i;
        srtrack.push_back(pt);

        pt.x = nImgWidth - 2;
        srtrack.push_back(pt);
    }
    
    iter = pCurve->begin();
    while(iter != iend)
    {
        int x = iter->x - rect.left + 2;
        int y = iter->y - rect.top + 2;

        pTobeFilled->SetPixelAt(x, y, 0, 255);
//        pOrg->SetPixelAt(x, y, 0, 255);
        ++iter;
    }

    CAdvImage * pResult = new CAdvImage();
    pResult->CreateGrayScale(nImgWidth, nImgHeight);
	unsigned char** ppd = (unsigned char**)pResult->mImage->image_ptr[0]->rptr;

    POINT   ptadd;
    while(! srtrack.empty())
    {
        pt = srtrack.back();
        srtrack.pop_back();

        ptadd.x = pt.x;     ptadd.y = pt.y - 1;
        if(ppd[ptadd.y][ptadd.x]==0 && ppix[ptadd.y][ptadd.x]==0)
        {
            srtrack.push_back(ptadd);
            ppd[ptadd.y][ptadd.x] = 255;
        }

        ptadd.x = pt.x;     ptadd.y = pt.y + 1;
        if(ppd[ptadd.y][ptadd.x]==0 && ppix[ptadd.y][ptadd.x]==0)
        {
            srtrack.push_back(ptadd);
            ppd[ptadd.y][ptadd.x] = 255;
        }

        ptadd.x = pt.x-1;     ptadd.y = pt.y;
        if(ppd[ptadd.y][ptadd.x]==0 && ppix[ptadd.y][ptadd.x]==0)
        {
            srtrack.push_back(ptadd);
            ppd[ptadd.y][ptadd.x] = 255;
        }

        ptadd.x = pt.x+1;     ptadd.y = pt.y;
        if(ppd[ptadd.y][ptadd.x]==0 && ppix[ptadd.y][ptadd.x]==0)
        {
            srtrack.push_back(ptadd);
            ppd[ptadd.y][ptadd.x] = 255;
        }
    }
    
    int nSize = 0;
    for(int i=1; i<nImgHeight-1; i++)
    {
        unsigned char * pc = ppd[i];

        for(int k=1; k<nImgWidth-1; k++)
        {
            if(pc[k]==0)
                nSize++;
        }
    }


    if(pOutput != NULL)
    {
        nImgWidth -= 4;
        nImgHeight -= 4;

        if(pOutput->mImage)
            ::delete_Image(pOutput->mImage);
        pOutput->CreateGrayScale(nImgWidth, nImgHeight);
        ppd = (unsigned char**)pOutput->mImage->image_ptr[0]->rptr;
        ppix = (unsigned char**)pResult->mImage->image_ptr[0]->rptr;

        for(int i=0; i<nImgHeight; i++)
        {
            unsigned char * pd = ppd[i];
            unsigned char * ps = ppix[i+2] + 2; 
            for(int k=0; k<nImgWidth; k++)
            {
                *pd = 255 - *ps;

                ++pd;
                ++ps;
            }
        }

        //pOutput->Write("out.jpg");
        //pResult->Write("r.jpg");
    }

    delete pResult;
    delete pTobeFilled;
//    delete pOrg;

    return nSize;
}


