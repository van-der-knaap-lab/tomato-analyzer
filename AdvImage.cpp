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

// AdvImage.cpp: implementation of the CAdvImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
//#include "BinaryObjects.h"
#include "BinObjs.h"
#include "BoundingBoxList.h"
#include "AdvImage.h"
#include "Histogram.h"
#include "Slice.h" // for findHue()
#include <stdlib.h>
#include <utility>
#include <assert.h>
#include <math.h>
#include <float.h>
#include "ImgTool.h"

#include <io.h>
#include <iostream>
#include <fstream>

#include "new_img_function.h"
#include <CVIPcolor.h>
#include "CVIPdecls.h"
#include <vector>
#include <string>
#include <algorithm>
#define pi 3.1415926535897932


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ipt;
using namespace std;

int compare(const void * a, const void *b);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAdvImage::CAdvImage()
{
	mImage = NULL;
	mUpdate = true;
	mScale = 0.35f;
    mChangeForegroundColor = true;
}
  
CAdvImage::~CAdvImage()
{

}


void CAdvImage::Serialize(CArchive &ar)
{
    if(ar.IsStoring())
    {
        unsigned char ** ppix;
        int imgh = GetHeight();
        int imgw = GetWidth();
        int nBand= GetNumBands();

        ar << imgw << imgh << nBand;
        for(int iBand=0; iBand<nBand; iBand++)
        {
            ppix = GetBand(iBand);

            for(int iLine=0; iLine<imgh; iLine++)
                ar.Write(ppix[iLine], imgw);
        }
    }
    else
    {
        unsigned char ** ppix;

        int imgw, imgh, nBand;
        ar >> imgw >> imgh >> nBand;

        if(nBand>1)
            CreateRGB(imgw, imgh);
        else
            CreateGrayScale(imgw, imgh);

        if(mImage != NULL)
        {
            for(int iBand=0; iBand<nBand; iBand++)
            {
                ppix = GetBand(iBand);
                for(int iLine=0; iLine<imgh; iLine++)
                    ar.Read(ppix[iLine], imgw);
            }
        }
    }
}

CAdvImage& CAdvImage::operator = (CAdvImage& other)
{
    if(mImage)
    {
		::delete_Image(mImage);
        mImage = NULL;
    }

    int imgh = other.GetHeight();
    int imgw = other.GetWidth();

    CreateRGB(imgw, imgh);

    unsigned char ** r = GetBand(0);
    unsigned char ** g = GetBand(1);
    unsigned char ** b = GetBand(2);

    unsigned char ** rA = other.GetBand(0);
    unsigned char ** gA = other.GetBand(1);
    unsigned char ** bA = other.GetBand(2);

    for(int i=0; i<imgh; i++)
    {
        for(int j=0; j<imgw; j++)
        {
            r[i][j] = rA[i][j];
            g[i][j] = gA[i][j];
            b[i][j] = bA[i][j];
        }
    }

    return *this;
}


CAdvImage&  CAdvImage::YUV(CAdvImage& other)
{
    if(mImage)
    {
		::delete_Image(mImage);
        mImage = NULL;
    }

    int imgh = other.GetHeight();
    int imgw = other.GetWidth();

    CreateRGB(imgw, imgh);

    unsigned char ** r = GetBand(0);
    unsigned char ** g = GetBand(1);
    unsigned char ** b = GetBand(2);

    unsigned char ** rA = other.GetBand(0);
    unsigned char ** gA = other.GetBand(1);
    unsigned char ** bA = other.GetBand(2);

    for(int i=0; i<imgh; i++)
    {
        for(int j=0; j<imgw; j++)
        {
            r[i][j] = (rA[i][j] + gA[i][j] + bA[i][j])/3;
            g[i][j] = (rA[i][j] + gA[i][j])/2;
			double maxCol = max(rA[i][j], max(gA[i][j],bA[i][j]) );
			double minCol = min(rA[i][j], min(gA[i][j],bA[i][j]) );
			b[i][j] = (minCol == maxCol ? 0 : 100 * double(maxCol - minCol) / double(maxCol)); // saturation
        }
    }

    return *this;
}



CAdvImage * CAdvImage::GetCopy()
{
	CAdvImage * newImage = new CAdvImage();

	newImage->mImage = duplicate_Image(mImage);
	newImage->SetScale(this->GetScale());

	return newImage;
}


CAdvImage& CAdvImage::fromIptImage(const CIptBwImage& bwImg)
{
    CIptBwImage::const_iterator  iter = bwImg.begin();
 
    if(mImage)
    {
		::delete_Image(mImage);
        mImage=0;    
    }

    CreateRGB(bwImg.width(), bwImg.height());

	unsigned char** destR = (unsigned char**)mImage->image_ptr[0]->rptr;
	unsigned char** destG = (unsigned char**)mImage->image_ptr[1]->rptr;
	unsigned char** destB = (unsigned char**)mImage->image_ptr[2]->rptr;
	
	for(int y=0; y<GetHeight(); y++)
	{
		for(int x=0; x<GetWidth(); x++)
		{
            BYTE tmp = (*iter) * 255;
			destR[y][x] = tmp;
			destG[y][x] = tmp;
			destB[y][x] = tmp;

            ++iter;
		}
	}

    return *this;
}


std::vector<IPTPOINT>*  CAdvImage::toPointLst(int iChannel, IPTPOINT ctr)
{
    std::vector<IPTPOINT>* ret = new std::vector<IPTPOINT>;

    IPTPOINT pt;

    int w = GetWidth();
    int h = GetHeight();

	unsigned char** dest = (unsigned char**)mImage->image_ptr[iChannel]->rptr;

    for(int i=0; i<h; i++)
    {
        for(int j=0; j<w; j++)
        {
            if(255==dest[i][j])
            {
                pt.x = j - ctr.x;
                pt.y = ctr.y - i;

                ret->push_back(pt);
            }
        }
    }

    return ret;
}


void CAdvImage::MakeBinary()
{
	//Extracting the three bands for easy manipulation
	unsigned char** rV = (unsigned char**)mImage->image_ptr[0]->rptr;
	unsigned char** gV = (unsigned char**)mImage->image_ptr[1]->rptr;
	unsigned char** bV = (unsigned char**)mImage->image_ptr[2]->rptr;

	for(unsigned int y=0; y<getNoOfRows_Image(mImage); y++)
		for(unsigned int x=0; x<getNoOfCols_Image(mImage); x++)
		{
			if(!((rV[y][x] == 0) && (gV[y][x] == 0) && (bV[y][x] == 0)))
			{
				rV[y][x] = 255;
				gV[y][x] = 255;
				bV[y][x] = 255;
			}
		}

}

/// Not used? 
CAdvImage * CAdvImage::Threshold(float inRMax, float inGMax, float inBMax,
									float inRMin, float inGMin, float inBMin, bool inverse)
{
	//Thresholding the CAdvImage and returning a thresholded copy.
	//if(inclusive) all values between {max,min} are white else black
	//	else all values between {max,min} are black else white
	
	//Quick test if image can be thresholded
	if(GetNumBands() != 3)
		return NULL;
	//Creating a return CAdvImage object
	// and an underlying baseImg structure.
	CAdvImage * thrImg = new CAdvImage();
	thrImg->mImage = duplicate_Image(mImage);

	//Extracting the three bands for easy manipulation
	unsigned char** origR = (unsigned char**)mImage->image_ptr[0]->rptr;
	unsigned char** origG = (unsigned char**)mImage->image_ptr[1]->rptr;
	unsigned char** origB = (unsigned char**)mImage->image_ptr[2]->rptr;

	unsigned char** binValue = (unsigned char**)thrImg->mImage->image_ptr[0]->rptr;
	unsigned char** gV = (unsigned char **)thrImg->mImage->image_ptr[1]->rptr;
	unsigned char** bV = (unsigned char **)thrImg->mImage->image_ptr[2]->rptr;

	bool black;
	for(unsigned int y=0; y<getNoOfRows_Image(mImage); y++)
		for(unsigned int x=0; x<getNoOfCols_Image(mImage); x++)
		{
			if(inverse == false)
			{
				black = true;
				if((origR[y][x] < inRMin) || (origR[y][x] > inRMax))
					black = false;
				if((origG[y][x] < inGMin) || (origG[y][x] > inGMax))
					black = false;
				if((origB[y][x] < inBMin) || (origB[y][x] > inBMax))
					black = false;

			}
			else
			{
				black = false;
				if((origR[y][x] < inRMin) || (origR[y][x] > inRMax))
					black = true;
				if((origG[y][x] < inGMin) || (origG[y][x] > inGMax))
					black = true;
				if((origB[y][x] < inBMin) || (origB[y][x] > inBMax))
					black = true;
			}

			if(black)
			{
				binValue[y][x] = 0;
				gV[y][x] = 0;
				bV[y][x] = 0;
			}
			else
			{
				binValue[y][x] = 255;
				gV[y][x] = 255;
				bV[y][x] = 255;
			}
		}

	return thrImg;



}

/// Not used?
CAdvImage * CAdvImage::ThresholdSpecial()
{
	//Thresholding the CAdvImage and returning a thresholded copy.
	//if(inclusive) all values between {max,min} are white else black
	//	else all values between {max,min} are black else white
	
	//Quick test if image can be thresholded
	if(GetNumBands() != 3)
		return NULL;
	//Creating a return CAdvImage object
	// and an underlying baseImg structure.
	CAdvImage * thrImg = new CAdvImage();
	thrImg->mImage = duplicate_Image(mImage);
	thrImg->SetScale(GetScale());

	//Extracting the three bands for easy manipulation
	unsigned char** origR = (unsigned char**)mImage->image_ptr[0]->rptr;
	unsigned char** origG = (unsigned char**)mImage->image_ptr[1]->rptr;
	unsigned char** origB = (unsigned char**)mImage->image_ptr[2]->rptr;

	unsigned char** binValue = (unsigned char**)thrImg->mImage->image_ptr[0]->rptr;
	unsigned char** gV = (unsigned char **)thrImg->mImage->image_ptr[1]->rptr;
	unsigned char** bV = (unsigned char **)thrImg->mImage->image_ptr[2]->rptr;

	//bool black;
	for(unsigned int y=0; y<getNoOfRows_Image(mImage); y++)
		for(unsigned int x=0; x<getNoOfCols_Image(mImage); x++)
		{
			int diff = origR[y][x] - origG[y][x];

			diff = (diff < 0 ? -diff : diff);
			if(diff <= 15)
			{
				binValue[y][x] = 255;
				gV[y][x] = 255;
				bV[y][x] = 255;
			} 	
			else
			{
				binValue[y][x] = 0;
				gV[y][x] = 0;
				bV[y][x] = 0;

			}


		}


	return thrImg;

}


/// Not used? 
//Thresholding based on (hue, sat, val) tuples.
CAdvImage *  CAdvImage::ThresholdHSV(	float inh1, float ins1, float inv1, 
										float inh2, float ins2, float inv2,
										bool inKeep)
{
	unsigned int i, j, numRows, numCols;

	CAdvImage * hsvImg = new CAdvImage();
	CAdvImage * outImg = new CAdvImage();
	float norm[3] = {255.0f, 255.0f, 255.0f};

	//Made change 4_26_00 chack that this works.
	//	May have been a mem leak.
	hsvImg->mImage = duplicate_Image(mImage);
	outImg->mImage = duplicate_Image(mImage);
	hsvImg->mImage = colorxform(hsvImg->mImage, HSV, norm, NULL, 1);

	//Setting up easy accessors
	CVIP_TYPE datatyp = hsvImg->mImage->image_ptr[0]->data_type;

	float ** hV = (float **)hsvImg->mImage->image_ptr[0]->rptr;
	float ** sV = (float **)hsvImg->mImage->image_ptr[1]->rptr;
	float ** vV = (float **)hsvImg->mImage->image_ptr[2]->rptr;

	CVIP_TYPE rgbType = mImage->image_ptr[0]->data_type;
	unsigned char ** rV = (unsigned char **)outImg->mImage->image_ptr[0]->rptr;
	unsigned char ** gV = (unsigned char **)outImg->mImage->image_ptr[1]->rptr;
	unsigned char ** bV = (unsigned char **)outImg->mImage->image_ptr[2]->rptr;
 	

	//More initializations
	numRows = getNoOfRows_Image(hsvImg->mImage);
	numCols = getNoOfCols_Image(hsvImg->mImage);

	//hsvImg is now in HSV space all are normalized (0.0...255.0, 0.0...255.0, 0.0...255.0)
	// For traditional rep, h of hsvImg should be /255 * 360.
	//	s and v /255 each.
	for(i=0; i< numRows; i++)
		for(j=0; j<numCols; j++)
		{
			float tradH, tradS, tradV;
			bool white = true;

			tradH = (hV[i][j] / 255.0f) * 360.0f;
			tradS = (sV[i][j] / 255.0f) * 100.0f;
			tradV = (vV[i][j] / 255.0f) * 100.0f;
			

			if(inKeep == true)
			{
				white = true;
				if((tradH > inh1) && (tradH < inh2))
					white = false;
				if((tradS > ins1) && (tradS < ins2))
					white = false;
				if((tradV > inv1) && (tradV < inv2))
					white = false;
			}
			else
			{
				white = true;
				if((tradH < inh1) && (tradH > inh2))
					white = false;
				if((tradS < ins1) && (tradS > ins2))
					white = false;
				if((tradV < inv1) && (tradV > inv2))
					white = false;


			}
			if(!white)
			{
				rV[i][j] = 0;
				gV[i][j] = 0;
				bV[i][j] = 0;
			}
			else
			{
				rV[i][j] = 255;
				gV[i][j] = 255;
				bV[i][j] = 255;
			}
				

		}			
	delete hsvImg;
	return outImg;
}

/**
 * A locally based thresholding algorithm.  This algorithm will divide a copy of this image
 * into inHDivs * inWDivs equal sized images, and threshold each subimage. It returns the copy, which has been
 * converted to black (background) and white (foreground). 
 * It sets nNew to the calculated threshold for the last subimage. 
 * 
 * The following parameters are now ignored: inChannel, inHistoPercent, nDesired, nBlue. 
 */
CAdvImage * CAdvImage::ThresholdLocally(int inWDivs, int inHDivs, int inChannel, float inHistoPercent, int nDesired, int& nNew, int nBlue)
{
	//Setup the resulting image.
	CAdvImage * outImage = new CAdvImage();
	outImage->mImage = duplicate_Image(this->mImage);

	CRect subimgrect(0, 0, 1, 1);
	int x1,y1, x2, y2;
	int imgw, imgh, wdiv, hdiv;

	imgw = this->GetWidth();
	imgh = this->GetHeight();

	assert(inWDivs != 0);
	assert(inHDivs != 0);

	wdiv = imgw/inWDivs;
	hdiv = imgh/inHDivs;

	for(int i = 0; i < inWDivs; i++)
	{
		for(int j = 0; j < inHDivs; j++)
		{
			x1 = i * wdiv;
			y1 = j * hdiv;
			x2 = (i+1 == inWDivs ? imgw : (i+1) * wdiv);
			y2 = (j+1 == inHDivs ? imgh : (j+1) * hdiv);
		
			// Makes the outImage black and white.
			subimgrect.SetRect(x1, y1, x2, y2);
			
			// inChannel, inHistoPercent, nDesired, nBlue are ignored
		   nNew = ThresholdSubImage(subimgrect, outImage, inChannel, inHistoPercent, nDesired, nBlue);
      
		}
	}
	
	return outImage;
}

/**
 * This is the function that is actually used to distinguish foreground from background by picking a threshold, 
 * which is the return value. The function
 * analyzes the section of resultImg defined by inSubImgRect and returns the result in resultImg by changing
 * all background pixels to black and all foreground pixels to white. (The other parameters are no longer used.) 
 * 
 * If the given image is all one color, then all pixels are marked as black (background). If the image has background
 * lighter than foreground, then (as usual) the lighter pixels are marked as white (foreground) and the darker
 * pixels are marked as black (background). 
 * 
 * See http://en.wikipedia.org/wiki/Thresholding_%28image_processing%29. We used to use K-means clustering, 
 * but it was too slow and not accurate enough. It was better to build a histogram and look for the valley point. 
 * 
 * The following parameters are now ignored: inChannel, inHistoPercent, nDesired, nBlue. 
 */
int CAdvImage::ThresholdSubImage(CRect inSubImgRect, CAdvImage * resultImg, int inChannel, float inHistoPercent, int nDesired, int nBlue)
{
	unsigned char ** rval = resultImg->GetBand(0);
	unsigned char ** gval = resultImg->GetBand(1);
	unsigned char ** bval = resultImg->GetBand(2);
	CPoint tl = inSubImgRect.TopLeft();
	CPoint lr = inSubImgRect.BottomRight();
	
	// Build histogram of L* values 
	std::vector<double> tmp(inSubImgRect.Width()); 
	std::vector< std::vector<double> > lval(inSubImgRect.Height(), tmp); 
	for (int i = tl.y; i < lr.y; i++) {
		for (int j = tl.x; j < lr.x; j++) {
			double lab[3]; 
			RGBtoLAB(rval[i][j], gval[i][j], bval[i][j], lab); 
			lval[i][j] = lab[0]; 
		}
	}
	CHistogram hist(inSubImgRect, lval); 
	int minLIndex = hist.firstNonZeroIndex(); 
	int maxLIndex = hist.lastNonZeroIndex(); 

	int threshold;	// minimum L* value for foreground
	if (minLIndex == maxLIndex) 
	{	
		threshold = maxLIndex + 1;	// image has uniform L* throughout --> all background
	} 
	else 
	{
		float avgL = hist.avgLValueInRange(minLIndex, maxLIndex + 1); 
		//threshold = avgL + 0.5;	// Tentative threshold value (rounded)
		threshold = avgL - (avgL * THESHOLD_ADJUST_CONST);

		// Adjust tentative threshold value. 
		// If it's on the slope of one of the peaks, move it to the valley (absolute minimum point between peaks). 
		const int WINDOW = 10;	// Assumes peaks are separated by > WINDOW indices in L* histogram
		
		// I commented this out because it's adjusting the average pixel value, and
		// the adjusted value is filtering out fruit. 
		
		/*
		while (true) { // changed to false for debugging 
			float leftAvg = hist.avgInRange(threshold - 3*WINDOW/2, threshold - WINDOW/2); 
			float midAvg = hist.avgInRange(threshold - WINDOW/2, threshold + WINDOW/2); 
			float rightAvg = hist.avgInRange(threshold + WINDOW/2, threshold + 3*WINDOW/2); 
			if (leftAvg < midAvg && midAvg < rightAvg) {
				threshold -= WINDOW; 
			} else if (leftAvg > midAvg && midAvg > rightAvg) {
				threshold += WINDOW; 
			} else if (leftAvg >= midAvg && midAvg <= rightAvg) {
				threshold = hist.minIndexInRange(threshold - WINDOW/2, threshold + WINDOW/2); 
				break; 
			} else {
				threshold = avgL + 0.5;	// Something went wrong - revert to average L* value
				break;	
			} 
		} */

	}

	// Convert image to black (background) and white (foreground)
	for (int i = tl.y; i < lr.y; i++) {
		for (int j = tl.x; j < lr.x; j++) { 
			if (lval[i][j] < threshold) {
				rval[i][j] = gval[i][j] = bval[i][j] = 0;
			} else {
				rval[i][j] = gval[i][j] = bval[i][j] = 255; 
			}
		}
	}

	return threshold; 
}


void * CAdvImage::RGBtoLAB(int r, int g, int b, double lab[]){

	double var_R = ( double(r) / 255.0 );        //Where R = 0 � 255
	double var_G = ( double(g) / 255.0 );        //Where G = 0 � 255
	double var_B = ( double(b) / 255.0 );        //Where B = 0 � 255

	if ( var_R > 0.04045 ) var_R = pow(( ( var_R + 0.055 ) / 1.055 ), 2.4);
	else                   var_R = var_R / 12.92;
	if ( var_G > 0.04045 ) var_G = pow(( ( var_G + 0.055 ) / 1.055 ), 2.4);
	else                   var_G = var_G / 12.92;
	if ( var_B > 0.04045 ) var_B = pow(( ( var_B + 0.055 ) / 1.055 ), 2.4);
	else                   var_B = var_B / 12.92;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	//Observer. = 2�, Illuminant = D65
	double X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
	double Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
	double Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

	double var_X = X / 95.047 ;         //ref_X =  95.047  Observer= 2�, Illuminant= D65
	double var_Y = Y / 100.000;          //ref_Y = 100.000
	double var_Z = Z / 108.883;          //ref_Z = 108.883

	if ( var_X > 0.008856 ) var_X = pow(var_X, ( 1.0/3.0 ));
	else                    var_X = ( 7.787 * var_X ) + ( 16.0 / 116.0 );
	if ( var_Y > 0.008856 ) var_Y = pow(var_Y, ( 1.0/3.0 ));
	else                    var_Y = ( 7.787 * var_Y ) + ( 16.0 / 116.0 );
	if ( var_Z > 0.008856 ) var_Z = pow(var_Z,( 1.0/3.0 ));
	else                    var_Z = ( 7.787 * var_Z ) + ( 16.0 / 116.0 );

	double CIEL = ( 116 * var_Y ) - 16;
	double CIEa = 500 * ( var_X - var_Y );
	double CIEb = 200 * ( var_Y - var_Z );

	//double * lab = new double[3];
	lab[0] = CIEL;
	lab[1] = CIEa;
	lab[2] = CIEb;

	return lab;
}

/**
 * Finds the hue of a pixel, given the red, green and blue values.
 * param	red		the red value of a pixel
 * param	gre		the green value of a pixel
 * param	blu		the blue value of a pixel
 * return	the hue (a double)
 */
double CAdvImage::findHue(double valL , double valA , double valB){
	 double hue = 0;
	// The hue value depends on the b* value which is being passed as valB
if (valB > 0)
	{
		hue = (180 / pi) * acos(valA / (sqrt(pow(valA,2) + pow(valB, 2))));
		
	}
	else 
	{
		hue = 360 - ((180 / pi) * acos(valA / (sqrt(pow(valA, 2) + pow(valB, 2)))));
		
	}

	return hue;
}

void CAdvImage::ColorBinaryObject(CPixelList & inList, float inr, float ing, float inb)
{
	CPixelList::iterator pixIter;
	CPixel myPix;
	
	unsigned char ** rV = (unsigned char **)mImage->image_ptr[0]->rptr;
	unsigned char ** gV = (unsigned char **)mImage->image_ptr[1]->rptr;
	unsigned char ** bV = (unsigned char **)mImage->image_ptr[2]->rptr;

	for(pixIter = inList.begin(); pixIter != inList.end(); pixIter++)
	{
		myPix = *pixIter;
		rV[myPix.y][myPix.x] = inr;
		gV[myPix.y][myPix.x] = ing;
		bV[myPix.y][myPix.x] = inb;
	}
}

/**
 * This function will scan through a documents image in binary format,
 *	and label all of the occuring clusters of white pixels.  For 
 *	every such cluster, there will be a CPixelList * entry in the
 *	returned list														
 */
CBinObjs * CAdvImage::GetBinaryObjects()
{
	//Duplicating the image to label.
	//	Image * labelImg = duplicate_Image(mImage);
	//The returned list

	CBinObjs * binObjs = new CBinObjs();
	//The list of pixels that make up a binary obj.
	CPixelList * addList;

	//The current maximum label
	int labelOn = 1;
	//Miscellaneous
	int i, j;
	int numRows, numCols;
	//Pixel coordinate
	CPixel pix;
	bool deb = false;

	
	//Need only the red channel because the image is binary.
	unsigned char** binValue = (unsigned char**)mImage->image_ptr[0]->rptr;
	unsigned char** gV = (unsigned char**)mImage->image_ptr[1]->rptr;
	unsigned char** bV = (unsigned char**)mImage->image_ptr[2]->rptr;

	numRows = getNoOfRows_Image(mImage);
	numCols = getNoOfCols_Image(mImage);

	//Allocating 2D array of labels.
	unsigned int ** labelValue = new unsigned int*[numRows];

    for(i=0; i<numRows; i++)
	{
		labelValue[i] = new unsigned int[numCols];
		memset(labelValue[i], 0, numCols);
	}


	//Label first row and initialize labelled image.  Special case.
	for(j = 0; j < numCols; j++)
	{
		//So background has initial label.
		labelValue[0][j] = 0;

		//if we encounter a new obj
		if(binValue[0][j] == 255)
		{
			//Special case no left neighbor
			if(j == 0)
			{
				//Maintaining list of objects as we go.
				addList = new CPixelList();
				pix.Set(j, 0);
				addList->push_back(pix);
				binObjs->Add(addList, labelOn);

				labelValue[0][j] = labelOn++;
			}
			else  //There is a neighbor
			{
				//if neighbor is marked
				if(labelValue[0][j-1] != 0)
				{
					pix.Set(j, 0);
					binObjs->GetList(labelValue[0][j-1])->push_back(pix);

					labelValue[0][j] = labelValue[0][j-1];
				}
				else //assign new label
				{
					//Maintaining list of objects as we go.
					addList = new CPixelList();
					pix.Set(j, 0); //Set(x,y)
					addList->push_back(pix);
					binObjs->Add(addList, labelOn);

					labelValue[0][j] = labelOn++;
				}
			}
		}
	}

	//General algorithm
	for(i = 1; i < numRows; i++)
	{
		for(j = 0; j < numCols; j++)
		{
			if(i == 305 && j == 518)
				deb = true;
			labelValue[i][j] = 0;
			
			//Encounter object
			if(binValue[i][j] == 255)
			{
				//Three cases.  
				//1. First column
				if(j == 0)
				{
					//[x][?]
					if(labelValue[i-1][j] != 0)
					{
						pix.Set(j, i);						
						binObjs->GetList(labelValue[i-1][j])->push_back(pix);
						labelValue[i][j] = labelValue[i-1][j];
					}
					//[0][x]
					else if(labelValue[i-1][j+1] != 0)
					{
						pix.Set(j, i);
						binObjs->GetList(labelValue[i-1][j+1])->push_back(pix);

						labelValue[i][j] = labelValue[i-1][j+1];
					}
					//[0][0]
					else
					{
						addList = new CPixelList();
						pix.Set(j, i); //Set(x,y)
						addList->push_back(pix);
						binObjs->Add(addList, labelOn);

						labelValue[i][j] = labelOn++;
					}
				}
				//2. Last column
				else if(j == (numCols - 1))
				{
					//Four cases
					//[x][x]
					//[x]
					if(labelValue[i-1][j-1] != 0)
					{						
						pix.Set(j, i);
						binObjs->GetList(labelValue[i-1][j-1])->push_back(pix);
						labelValue[i][j] = labelValue[i-1][j-1]; 
					}
					//[?][x]
					//[]
					else if((labelValue[i-1][j] != 0))
					{
						pix.Set(j, i);
						CPixelList * list = binObjs->GetList(labelValue[i-1][j]);
						int labelAt = labelValue[i-1][j];
						if(list == NULL)
						{
							list = binObjs->GetList(labelValue[i-1][j]);		
							this->Write("temp.gif");
						}
						list->push_back(pix);
						labelValue[i][j] = labelValue[i-1][j]; 
					}
					else if(labelValue[i][j-1] != 0)
					{
						pix.Set(j, i);
						binObjs->GetList(labelValue[i][j-1])->push_back(pix);
						labelValue[i][j] = labelValue[i][j-1]; 
					}
					else
					{
						addList = new CPixelList();
						pix.Set(j, i);
						addList->push_back(pix);
						binObjs->Add(addList, labelOn);
						labelValue[i][j] = labelOn++;
					}

				}
				//First two cases almost never executed.
				//Ignore previous notation.
				//3. In general
				//For a given pixel i, we only have to look at the four surrounding pixels
				//[ ][ ][ ]
				//[ ][i]
				//----------------------------
				//[o] means pixel is labeled
				//[x] means pixel can't be labeled
				//[?] means does not matter if pixel is labeled.
				else
				{
					//[?][o][?]
					//[?][i]
					//No label connections can occur if this pixel is labeled.
					//	all surrounding if labeled must be same label.
					if(labelValue[i-1][j] != 0)
					{
						pix.Set(j,i);
						binObjs->GetList(labelValue[i-1][j])->push_back(pix);
						labelValue[i][j] = labelValue[i-1][j];
					}
					else if(labelValue[i-1][j+1] != 0)
					{
						//[?][x][o]
						//[o][i]
						//A connection will occur.
						if(labelValue[i][j-1] != 0)
						{
							if(labelValue[i-1][j+1] != labelValue[i][j-1])
							{
								CombineLabelLists(binObjs, labelValue[i][j-1], 
												labelValue[i-1][j+1], labelValue);
							}
							
							pix.Set(j, i);
							binObjs->GetList(labelValue[i][j-1])->push_back(pix);
							labelValue[i][j] = labelValue[i][j-1];

						}
						//[o][x][o]
						//[x][i]
						//A connection will occur
						else if(labelValue[i-1][j-1] != 0)
						{
							if(labelValue[i-1][j-1] != labelValue[i-1][j+1])
							{
								CombineLabelLists(binObjs, labelValue[i-1][j-1], 
												labelValue[i-1][j+1], labelValue);
							}
							pix.Set(j, i);
							binObjs->GetList(labelValue[i-1][j-1])->push_back(pix);

							labelValue[i][j] = labelValue[i-1][j-1];


						}
						//[x][x][o]
						//[x][i]
						else
						{			
							pix.Set(j,i);
							binObjs->GetList(labelValue[i-1][j+1])->push_back(pix);
							labelValue[i][j] = labelValue[i-1][j+1];

						}
					}
					//[?][x][x]
					//[o][i]
					//No connection just set label.
					else if((labelValue[i][j-1] != 0))
					{
						pix.Set(j, i);
						binObjs->GetList(labelValue[i][j-1])->push_back(pix);
						labelValue[i][j] = labelValue[i][j-1];

					}
					//[o][x][x]
					//[x][i]
					//Just set label.
					else if(labelValue[i-1][j-1] != 0)
					{
						pix.Set(j, i);
						binObjs->GetList(labelValue[i-1][j-1])->push_back(pix);
						labelValue[i][j] = labelValue[i-1][j-1];
					}
					//[x][x][x]
					//[x][i]
					//New label created.
					else
					{
						//New Label added
						addList = new CPixelList();
						pix.Set(j, i);
						addList->push_back(pix);
						binObjs->Add(addList, labelOn);

						labelValue[i][j] = labelOn;
						labelOn++;
					}
				}

			}
			//else Do nothing

		}
	}

	//Cleaning up the duplicated memory.
	for(i = 0; i < numRows; i++)
	{
		delete[] labelValue[i];
	}
	delete[] labelValue;

	return binObjs;
}
void CAdvImage::CombineLabelLists(CBinObjs * inBinObjs, int label1, int label2, unsigned int ** inLabelImage)
{
	CBONode * list1 = inBinObjs->Find(label1);
	CBONode * list2 = inBinObjs->Find(label2);
	if(label2 == 14)
		int j = 5;

	//Iterating through the second list and changing all label2 pixels to label1 pixels.
	std::list<CPixel>::iterator l2iter = list2->mPixList->begin();

	while(l2iter != list2->mPixList->end())
	{
		CPixel myPixel;
		myPixel = *(l2iter);
			
		inLabelImage[myPixel.y][myPixel.x] = label1;
		l2iter++;
	}


	list1->mPixList->splice(list1->mPixList->end(), *(list2->mPixList));
	inBinObjs->Remove(list2);
}

void CAdvImage::GetRandomColor(int & r, int & g, int & b)
{
	float tr, tg, tb;

	tr = rand();
	tg = rand();
	tb = rand();
	r = (int)((tr / (float)RAND_MAX) * 255.0f);
	g = (int)((tg / (float)RAND_MAX) * 255.0f);
	b = (int)((tb / (float)RAND_MAX) * 255.0f);
}

//Labelling all objects red.
void CAdvImage::ColorImageObjects(CBinObjs *  inList)
{
	int r, g, b;
	unsigned char ** rV = (unsigned char **) mImage->image_ptr[0]->rptr;
	unsigned char ** gV = (unsigned char **) mImage->image_ptr[1]->rptr;
	unsigned char ** bV = (unsigned char **) mImage->image_ptr[2]->rptr;

	CBONode * head = inList->GetHead();

	while(head != NULL)
	{
		CPixelList * myList = head->mPixList;
		GetRandomColor(r, g, b);
		this->ColorBinaryObject(*myList, r, g, b);
		head = head->mNext;
	}
}

/// Not used? 
CAdvImage * CAdvImage::ThinImage()
{
	// Performs thinning of a binary image
	// Copy original image
	CAdvImage * outImg = new CAdvImage();

	outImg->mImage = duplicate_Image(mImage);
	Image * newimg = outImg->mImage;

	if(!newimg)
		AfxMessageBox("Not enough memory!");
	//Let pix be a pointer to newimg->image_ptr[0]-rptr
	//I guess for easier writing?
	unsigned char **pix = (unsigned char **)newimg->image_ptr[0]->rptr;
	unsigned char ** gV = (unsigned char **)newimg->image_ptr[1]->rptr;
	unsigned char ** bV = (unsigned char **)newimg->image_ptr[2]->rptr;
	int m = 100;  int ir = 1, ia[9], ic[9];

	int numrows = mImage->image_ptr[0]->rows;
	int numcols = mImage->image_ptr[0]->cols;


	while(ir!=0)
	{
		ir=0;

		for(int iy=0; iy < numrows; iy++)
		{
			for(int ix=0; ix < numcols; ix++)
			{
				//If not equal to white.
				if(pix[iy][ix] != 255)
					continue;

				//ia[5]		ia[6]		ia[7]
				//ia[4]		iyix		ia[0]
				//ia[3]		ia[2]		ia[1]
				if(ix == numcols-1)
					ia[0] = 0;
				else
					ia[0] = pix[iy][ix+1];
				if(iy == 0 || ix == numcols-1)
					ia[1] = 0;
				else
					ia[1] = pix[iy-1][ix+1];
				if(iy == 0)
					ia[2] = 0;
				else
					ia[2] = pix[iy-1][ix  ];
				if(iy == 0 || ix == 0)
					ia[3] = 0;
				else
					ia[3] = pix[iy-1][ix-1];
				if(ix == 0)
					ia[4] = 0;
				else
					ia[4] = pix[iy  ][ix-1];
				if(iy == numrows-1 || ix == 0)
					ia[5] = 0;
				else
					ia[5] = pix[iy+1][ix-1];
				if(iy == numrows-1)
					ia[6] = 0;
				else
					ia[6] = pix[iy+1][ix  ];
				if(iy == numrows-1 || ix == numcols-1)
					ia[7] = 0;
				else
					ia[7] = pix[iy+1][ix+1];

				for(int i=0; i < 8; i++)
				{
					//ia[i] == 100 then ia[i] = 255
					if(ia[i] == m)
					{
						ia[i] = 255; ic[i] = 0;
					}
					else	//ia[i] != 100
					{
						if(ia[i] < 255)  //ia[i] != 100 and ia[i] < 255
							ia[i] = 0;
						//if(ia[i] == 255)
						//	ic[i] = 255
						//else
						//	ic[i] = 0
						ic[i] = ia[i];
					}
				}  // for

				//Wrapping.
				ia[8] = ia[0];
				ic[8] = ic[0];

				//ixiy in the center of white pixels.  Will not be thinned.
				if(ia[0]+ia[2]+ia[4]+ia[6]==255*4)
					continue;

				//iv represents the number of white pixels surrounding ixiy
				int iv, iw;
				iv = 0;
				iw = 0;
				for(int i=0; i<8; i++)
				{
					if(ia[i]==255)
						iv++;
					if(ic[i]==255)
						iw++;
				}

				if(iv<=1)
					continue;
				if(iw==0)
					continue;
				if(cconc(ia)!=1)
					continue;

				int temppix = (iy == 0) ? 0 : pix[iy-1][ix];
				if(temppix == m)
				{
					ia[2] = 0;
					if(cconc(ia) != 1)
						continue;
					ia[2] = 255;
				}
				temppix = (ix == 0) ? 0 : pix[iy][ix-1];
				if(temppix == m)
				{
					ia[4]=0;
					if(cconc(ia)!=1)
						continue;
					ia[4] = 255;
				}
				//m is the proposed thin?
				pix[iy][ix] = m;
				ir++; //This represents a change being made.
				//We continue to process the image until no more thins are made.

			}  // for ix
		}  // for iy
		m++;
	}  // while

	//Simply interprets all of the thinned images as being black and the rest white.
	for(int iy=0; iy<(signed)mImage->image_ptr[0]->rows; iy++)
	{
		for(int ix=0; ix<(signed)mImage->image_ptr[0]->cols; ix++)
		{
			if(pix[iy][ix] < 255)
			{
				pix[iy][ix] = 0;
				gV[iy][ix] = 0;
				bV[iy][ix] = 0;
			}
		}
	}
	return outImg;

}



//******************************************************************************/
void CAdvImage::RGBtoHSV( float r, float g, float b, float  & h, float & s, float & v )
{
	// Covert r,g,b so that the range is [0,1].

	r /= 255.0f;
	g /= 255.0f;
	b /= 255.0f;

	float min, max, delta;
	min = min3( r, g, b );
	max = max3( r, g, b );
	v = max;				// v
	delta = max - min;
	if( max != 0 )
		s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		s = 0;
		h = -1;
		return;
	}
	if( r == max )
		h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		h = 4 + ( r - g ) / delta;	// between magenta & cyan
	h *= 60;				// degrees
	if( h < 0 )
		h += 360;
}

void CAdvImage::HSVtoRGB( float & r, float  & g, float & b, float h, float s, float v )
{
	int i;
	float f, p, q, t;
	if( s == 0 ) {
		// achromatic (grey)
		r = g = b = v;
		return;
	}
	h /= 60;			// sector 0 to 5
	i = (int)h;			// integral of h
	f = h - i;			// fractional part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}

	// Convert range of r,g,b to [0,255];

	r *= 255.0f;
	g *= 255.0f;
	b *= 255.0f;
}
 
float CAdvImage::max3(float a, float b, float c)
{
	float max = (a > b ? a : b);
	max = (c > max ? c : max);

	return max;
}

float CAdvImage::min3(float a, float b, float c)
{
	float min = (a < b ? a : b);
	min = (c < min ? c : min);

	return min;
}

// Connectedness.  Used in ImageThinning function.
//All of Yusaku's Image thinning is kind of complicated to understand.
//	take it that it works, and provides good results.
int CAdvImage::cconc(int inb[9])
{
	int icn = 0;

	for(int i=0; i<8; i+=2)
	{
		if(inb[i]==0)
			if(inb[i+1] == 255 || inb[i+2] == 255)
				icn++;
	}
	return icn;
}


void CAdvImage::DrawLine(CPixel inEndPoint1, CPixel inEndPoint2, float inR, float inG, float inB)
{
	CPixel pointOnLine;
	float fx1, fy1, fx2, fy2, px, py;
	float dx, dy;
	int steps;
	unsigned char ** rBand = this->GetBand(0);
	unsigned char ** gBand = this->GetBand(1);
	unsigned char ** bBand = this->GetBand(2);
	int width = this->GetWidth();
	int height = this->GetHeight();


	
	fx1 = inEndPoint1.x;
	fx2 = inEndPoint2.x;
	fy1 = inEndPoint1.y;
	fy2 = inEndPoint2.y;
	px = fx1;
	py = fy1;
	
	pointOnLine = inEndPoint1;
	dx = fx2 - fx1;
	dy = fy2 - fy1;

	fy1 = fabs(dy);
	fx1 = fabs(dx);
	if(fy1 > fx1)
	{
		steps = fy1;
	}
	else
	{
		steps = fx1;
	}

	dx /= (float)steps;
	dy /= (float)steps;

	rBand[inEndPoint2.y][inEndPoint2.x] = inR;
	gBand[inEndPoint2.y][inEndPoint2.x] = inG;
	bBand[inEndPoint2.y][inEndPoint2.x] = inB;

	for(int i = 0; i < steps; i++)
	{
		if((pointOnLine.x > 0) && (pointOnLine.x < width) && (pointOnLine.y > 0) && (pointOnLine.y < height))
		{
			rBand[pointOnLine.y][pointOnLine.x] = inR;
			gBand[pointOnLine.y][pointOnLine.x] = inG;
			bBand[pointOnLine.y][pointOnLine.x] = inB;
		}	
		px += dx;
		py += dy;
		pointOnLine.Set(px, py);
	}

}

void CAdvImage::DrawBoundingBox(CBoundingBox & inBBox, float inR, float inG, float inB)
{
	CPixel upRight, lowLeft;

	upRight.Set(inBBox.mLowerRight.x, inBBox.mUpperLeft.y);
	lowLeft.Set(inBBox.mUpperLeft.x, inBBox.mLowerRight.y);
	DrawLine(inBBox.mUpperLeft, upRight, inR, inG, inB);
	DrawLine(upRight, inBBox.mLowerRight, inR, inG, inB);
	DrawLine(inBBox.mUpperLeft, lowLeft, inR, inG, inB);
	DrawLine(lowLeft, inBBox.mLowerRight, inR, inG, inB);
}

unsigned char ** CAdvImage::GetBand(int inWhich)
{
	return (unsigned char **)this->mImage->image_ptr[inWhich]->rptr;
}


CAdvImage * CAdvImage::TestOp()
{
	return NULL;
}

int CAdvImage::GetArea()
{
    int AreaVal = 0;

    unsigned char ** pp = GetBand(0);
    int nWidth = GetWidth();
    int nHeight= GetHeight();
    
    for(int i=0; i<nHeight; i++)
    {
        unsigned char * ppix = pp[i];
        for(int j=0; j<nWidth; j++)
        {
            if(ppix[j]!=0)
                AreaVal++;
        }
    }

    return AreaVal;
}

//flag = 1:discard all the part above the line
//flag = -1:discard all the part underneath the line
void CAdvImage::CutImageByLine(CPixel p1, CPixel p2, BOOL flag)
{
    unsigned char ** pp = GetBand(0);

	int imWidth = GetWidth();
	int imHeight = GetHeight();

	if(p2.x == p1.x)
	{
		for(int i=0;i<imWidth;i++)
		{
			for (int j=0; j<imHeight; j++)
			{
				if(flag > 0)
				{
					if(i<p1.x)
					{
						pp[j][i]=0;
					}
				}
				else
				{
					if(i>p1.x)
					{
						pp[j][i]=0;
					}
				}
			}
		}
		return;
	}

	float k=((float)(p2.y-p1.y))/((float)(p2.x-p1.x));

	for(int i=0;i<imWidth;i++)
	{
		for (int j=0; j<imHeight; j++)
		{
			float Zy=p1.y+k*(i-p1.x);
			if(flag > 0)
			{
				if(j<Zy)
				{
					pp[j][i]=0;
				}
			}
			else
			{
				if(j>Zy)
				{
					pp[j][i]=0;
				}
			}
		}
	}

}

void CAdvImage::Set(unsigned char & inR, unsigned char & inG, unsigned char & inB,
						unsigned char rval, unsigned char gval, unsigned char bval)
{
	inR = rval;
	inG = gval;
	inB = bval;
}

void CAdvImage::MarkImage(CPixelList * inList, COLORREF inColor, int size)
{
	char r, g, b;
	int height, width;
	CPixel upleft, lowright;
	CPixelList::iterator myIter;


	unsigned char ** rBand, ** gBand, ** bBand;

	rBand = this->GetBand(0);
	gBand = this->GetBand(1);
	bBand = this->GetBand(2);
	height = this->GetHeight();
	width = this->GetWidth();

	r = GetRValue(inColor);
	g = GetGValue(inColor);
	b = GetBValue(inColor);

	for(myIter = inList->begin(); myIter != inList->end(); myIter++)
	{
		upleft.x = (*myIter).x - size;
		upleft.y = (*myIter).y - size;
		lowright.x = (*myIter).x + size;
		lowright.y = (*myIter).y + size;

		if(upleft.x < 0)
			upleft.x = 0;
		if(upleft.y < 0)
			upleft.y = 0;
		if(lowright.x < 0)
			lowright.x = 0;
		if(lowright.y < 0)
			lowright.y = 0;
		if(upleft.x > (width-1))
			upleft.x = width-1;
		if(upleft.y > (height-1))
			upleft.y = height-1;
		if(lowright.x > (width-1))
			lowright.x = width-1;
		if(lowright.y > (height-1))
			lowright.y = height-1;

		if(!((upleft.x > lowright.x) || (upleft.y > lowright.y)))
		{
			for(int i = upleft.x; i <= lowright.x; i++)
			{
				for(int j = upleft.y; j <= lowright.y; j++)
				{
					  rBand[j][i] = r;
					  gBand[j][i] = g;
					  bBand[j][i] = b;
				}
			}
		}
	}
	
}

void CAdvImage::FillInColorInformation(CBinObjs * inObjs, CAdvImage * inSourceColor)
{
	unsigned char ** sRBand, ** sGBand, **sBBand;
	unsigned char ** dRBand, ** dGBand, **dBBand;
	CBONode * head;
	CPixelList * thePList;
	CPixelList::iterator plIter;
	CPixel thePixel;

	//Getting the band information
	sRBand = inSourceColor->GetBand(0);
	sGBand = inSourceColor->GetBand(1);
	sBBand = inSourceColor->GetBand(2);

	dRBand = this->GetBand(0);
	dGBand = this->GetBand(1);
	dBBand = this->GetBand(2);

	head = inObjs->GetHead();

	while(head != NULL)
	{
		thePList = head->mPixList;
		for(plIter = thePList->begin(); plIter != thePList->end(); plIter++)
		{
			thePixel = *plIter;
			dRBand[thePixel.y][thePixel.x] = sRBand[thePixel.y][thePixel.x];
			dGBand[thePixel.y][thePixel.x] = sGBand[thePixel.y][thePixel.x];
			dBBand[thePixel.y][thePixel.x] = sBBand[thePixel.y][thePixel.x];
		}
		head = head->mNext;
	}
}

void CAdvImage::ColorWith(CPixelList & inList, CAdvImage * inImage)
{
	unsigned char ** sRBand, ** sGBand, **sBBand;
	unsigned char ** dRBand, ** dGBand, **dBBand;
	CPixelList::iterator plIter;
	CPixel thePixel;

	//Getting the band information
	sRBand = inImage->GetBand(0);
	sGBand = inImage->GetBand(1);
	sBBand = inImage->GetBand(2);

	dRBand = this->GetBand(0);
	dGBand = this->GetBand(1);
	dBBand = this->GetBand(2);

	for(plIter = inList.begin(); plIter != inList.end(); plIter++)
	{
		thePixel = *plIter;
		dRBand[thePixel.y][thePixel.x] = sRBand[thePixel.y][thePixel.x];
		dGBand[thePixel.y][thePixel.x] = sGBand[thePixel.y][thePixel.x];
		dBBand[thePixel.y][thePixel.x] = sBBand[thePixel.y][thePixel.x];
	}
}

void CAdvImage::FillIn(float inR, float inG, float inB)
{
	int width, height;
	unsigned char ** rBand, ** gBand, ** bBand;

	width = this->GetWidth();
	height = this->GetHeight();
	rBand = this->GetBand(0);
	gBand = this->GetBand(1);
	bBand = this->GetBand(2);

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			rBand[i][j] = inR;
			gBand[i][j] = inG;
			bBand[i][j] = inB;
		}
	}

}

/********************************************************************************************/
/*					Operations on Images representing Seedlings								*/
/********************************************************************************************/




/********************************************************************************************/
//DEBUG

/********************************************************************************************/
CAdvImage *  CAdvImage::ThresholdHSVDebug(	float inh1, float ins1, float inv1, 
										float inh2, float ins2, float inv2,
										bool inKeep)
{
	unsigned int i, j, numRows, numCols;

	CAdvImage * hsvImg = new CAdvImage();
	CAdvImage * outImg = new CAdvImage();
	float norm[3] = {255.0f, 255.0f, 255.0f};

	//Made change 4_26_00 chack that this works.
	//	May have been a mem leak.
	hsvImg->mImage = duplicate_Image(mImage);
	outImg->mImage = duplicate_Image(mImage);
	hsvImg->mImage = colorxform(hsvImg->mImage, HSV, norm, NULL, 1);

	//Setting up easy accessors
	CVIP_TYPE datatyp = hsvImg->mImage->image_ptr[0]->data_type;

	float ** hV = (float **)hsvImg->mImage->image_ptr[0]->rptr;
	float ** sV = (float **)hsvImg->mImage->image_ptr[1]->rptr;
	float ** vV = (float **)hsvImg->mImage->image_ptr[2]->rptr;

	CVIP_TYPE rgbType = mImage->image_ptr[0]->data_type;
	unsigned char ** rV = (unsigned char **)outImg->mImage->image_ptr[0]->rptr;
	unsigned char ** gV = (unsigned char **)outImg->mImage->image_ptr[1]->rptr;
	unsigned char ** bV = (unsigned char **)outImg->mImage->image_ptr[2]->rptr;
 	

	//More initializations
	numRows = getNoOfRows_Image(hsvImg->mImage);
	numCols = getNoOfCols_Image(hsvImg->mImage);

	//hsvImg is now in HSV space all are normalized (0.0...255.0, 0.0...255.0, 0.0...255.0)
	// For traditional rep, h of hsvImg should be /255 * 360.
	//	s and v /255 each.
	for(i=0; i< numRows; i++)
		for(j=0; j<numCols; j++)
		{
			float tradH, tradS, tradV;
			bool white = true;

			tradH = (hV[i][j] / 255.0f) * 360.0f;
			tradS = (sV[i][j] / 255.0f) * 100.0f;
			tradV = (vV[i][j] / 255.0f) * 100.0f;
			

			if(inKeep == true)
			{
				white = false;
				if(!((tradH >= inh1) && (tradH <= inh2)))
					white = true;
				if(!((tradS >= ins1) && (tradS <= ins2)))
					white = true;
				if(!((tradV >= inv1) && (tradV <= inv2)))
					white = true;
			}
			if(!white)
			{
				rV[i][j] = 0;
				gV[i][j] = 0;
				bV[i][j] = 0;
			}
			else
			{
				if(!((rV[i][j] == 0) && (gV[i][j] == 0) && (bV[i][j] == 0)))
				{
					rV[i][j] = 255;
					gV[i][j] = 255;
					bV[i][j] = 255;
				}
			}
				

		}			
	delete hsvImg;
	return outImg;
}

//For any pixel in the thisimage, if its value is <= inVal and black is true
// its color will be black. if(black == false) its value is white.
CAdvImage * CAdvImage::ThresholdGeneric(float inVal, float inChannel, bool black)
{
	CAdvImage * outImg = this->GetCopy();
	int width, height;
	unsigned char ** binVal, ** newR, ** newG, ** newB;

	newR = outImg->GetBand(0);
	newG = outImg->GetBand(1);
	newB = outImg->GetBand(2);
	binVal = this->GetBand(inChannel);

	width = outImg->GetWidth();
	height = outImg->GetHeight();

	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
		{
			if(binVal[i][j] <= inVal)
			{
				if(black)
				{
					newR[i][j] = newB[i][j] = newG[i][j] = 0;
				}
				else
				{
					if(binVal[i][j] != 0)
						newR[i][j] = newB[i][j] = newG[i][j] = 255;

				}
			}
			else
			{
				if(black)
				{
					newR[i][j] = newB[i][j] = newG[i][j] = 255;
				}
				else
				{
					newR[i][j] = newB[i][j] = newG[i][j] = 0;

				}


			}

		}

	return outImg;


}

/**
 * Performs a median filter of this image, modifying it. See MedianFilterImage. 
 */
void CAdvImage::MedianFilter(int filterSize, bool blackAndWhite)
{
	CAdvImage * outImage;
	
	outImage = MedianFilterImage(filterSize, blackAndWhite);
	delete_Image(mImage);
	mImage = outImage->mImage;
	outImage->mImage = NULL;
	delete outImage;
}

/**
 * Performs a median filter of this image, without modifying it but returning the result. A median filter removes
 * noise from an image (http://en.wikipedia.org/wiki/Median_filter). It smooths jagged
 * edges in an image (http://dossy.org/2007/08/what-is-gimps-equivalent-of-photoshops-median-filter/). 
 * 
 * filterSize is the size, in pixels, of the filter window. 
 * blackAndWhite is true if the image consists only of black and white (no color or gray). 
 */
CAdvImage * CAdvImage::MedianFilterImage(int filterSize, bool blackAndWhite)
{
	CAdvImage * outImage = new CAdvImage();
	int height, width;
	int x, y;
	unsigned char ** sR, ** sG, ** sB;
	unsigned char ** dR, ** dG, ** dB;
	int kc;
	int stinc = filterSize/2;
	int dx, dy;
	int xstart, xend, ystart, yend;
	int rwhite, gwhite, bwhite;
	int kcover2;

	sR = GetBand(0);
	sG = GetBand(1);
	sB = GetBand(2);


	height = GetHeight();
	width = GetWidth();

	outImage->mImage = new_Image(PPM, RGB, 3, height, width, CVIP_BYTE, REAL);

	dR = outImage->GetBand(0);
	dG = outImage->GetBand(1);
	dB = outImage->GetBand(2);

	for(x=0; x<width; x++)
	{
		for(y=0; y<height; y++) 
		{
			kc = 0;
			rwhite = gwhite = bwhite = 0;
			xstart = max(0, x-stinc);
			xend = min(width-1, x+stinc);
			ystart = max(0, y-stinc);
			yend = min(height-1, y+stinc);
			float rSum = 0, gSum = 0, bSum = 0; 
			for(dx=xstart;dx<=xend;dx++)
			{
				for(dy=ystart;dy<=yend;dy++) 
				{
					if (blackAndWhite) {
						if(sR[dy][dx] == 255)
							rwhite++;
						if(sG[dy][dx] == 255)
							gwhite++;
						if(sB[dy][dx] == 255)
							bwhite++;
						kc++;
					} else {
						rSum += sR[dy][dx]; 
						gSum += sG[dy][dx]; 
						bSum += sB[dy][dx]; 
					}
				}
			}
			if (blackAndWhite) {
				kcover2 = kc/2;
				dR[y][x] = (rwhite >= kcover2 ? 255 : 0);
				dG[y][x] = (gwhite >= kcover2 ? 255 : 0);
				dB[y][x] = (bwhite >= kcover2 ? 255 : 0);
			} else {
				int numPixels = (xend - xstart + 1) * (yend - ystart + 1); 
				dR[y][x] = rSum/numPixels; 
				dG[y][x] = gSum/numPixels; 
				dB[y][x] = bSum/numPixels; 
			}
		}
	}		

	return outImage;
}

int compare(const void* inA, const void * inB) 
{
	char a, b;

	a = *((char*)inA);
	b = *((char*)inB);
	if(a >  b) return  1;
	if(a == b) return  0;
	return -1;
}



CAdvImage::CAdvImage(int iWidth, int iHeight)
{
	mImage = NULL;
	mUpdate = true;
	mScale = 0.35f;
    mChangeForegroundColor = true;

		Image* tempimg = new_Image(PPM, RGB, 3, 
			iHeight, iWidth, CVIP_BYTE, REAL);

		unsigned char** destR = (unsigned char**)tempimg->image_ptr[0]->rptr;
		unsigned char** destG = (unsigned char**)tempimg->image_ptr[1]->rptr;
		unsigned char** destB = (unsigned char**)tempimg->image_ptr[2]->rptr;

		for(int y=0; y<iHeight; y++)
		{
			for(int x=0; x<iWidth; x++)
			{
				destR[y][x] = 0;
				destG[y][x] = 0;
				destB[y][x] = 0;
			}
		}
		mImage = tempimg;

}

CAdvImage* CAdvImage::Crop(CRect selectRect)
{
	int lx, ly, rx, ry;

	lx = selectRect.left ;
	ly = selectRect.top ;
	rx = selectRect.right ;
	ry = selectRect.bottom ;

	Image* dupimage = duplicate_Image(mImage);

	CAdvImage* returnimage = new CAdvImage;
	returnimage->mImage = ::crop(dupimage, ly, lx, ry-ly+1, rx-lx+1);	// ::crop returns a pointer to dupimage

	return returnimage;
}


CAdvImage* CAdvImage::CropTwo(CRect selectRect)
{
	int nWidth = selectRect.right - selectRect.left + 1;
	int nHeight= selectRect.bottom - selectRect.top + 1;
	
	CAdvImage * pCropImage = new CAdvImage(nWidth, nHeight);
	unsigned char** destR = (unsigned char**)pCropImage->mImage->image_ptr[0]->rptr;
	unsigned char** destG = (unsigned char**)pCropImage->mImage->image_ptr[1]->rptr;
	unsigned char** destB = (unsigned char**)pCropImage->mImage->image_ptr[2]->rptr;

	unsigned char** srcR = (unsigned char**)mImage->image_ptr[0]->rptr;
	unsigned char** srcG = (unsigned char**)mImage->image_ptr[1]->rptr;
	unsigned char** srcB = (unsigned char**)mImage->image_ptr[2]->rptr;

	int srcI = selectRect.top;
	for(int i=0; i<nHeight; i++)
	{
		int srcJ = selectRect.left;
		for(int j=0; j<nWidth; j++)
		{
			destR[i][j] = srcR[srcI][srcJ];
			destG[i][j] = srcG[srcI][srcJ];
			destB[i][j] = srcB[srcI][srcJ];

			srcJ++;
		}

		srcI++;
	}

	return pCropImage;
}

void CAdvImage::Paste(int nX, int nY, CAdvImage * pPasteImage)
{
	unsigned char** srcR = (unsigned char**)pPasteImage->mImage->image_ptr[0]->rptr;
	unsigned char** srcG = (unsigned char**)pPasteImage->mImage->image_ptr[1]->rptr;
	unsigned char** srcB = (unsigned char**)pPasteImage->mImage->image_ptr[2]->rptr;

	unsigned char** dstR = (unsigned char**)mImage->image_ptr[0]->rptr;
	unsigned char** dstG = (unsigned char**)mImage->image_ptr[1]->rptr;
	unsigned char** dstB = (unsigned char**)mImage->image_ptr[2]->rptr;

    int nDestWidth = GetWidth();
    int nDestHeight= GetHeight();
    int nSrcWidth  = pPasteImage->GetWidth();
    int nSrcHeight = pPasteImage->GetHeight();
    
    int nRight  = nX + nSrcWidth;
    if( nRight >= nDestWidth)
        nRight = nDestWidth;
        
    int nBottom = nY + nSrcHeight;
    if(nBottom >= nDestHeight)
        nBottom = nDestHeight;

	int srcI = 0;
	for(int i=nY; i<nBottom; i++)
	{
		int srcJ = 0;
		for(int j=nX; j<nRight; j++)
		{
			dstR[i][j] = srcR[srcI][srcJ];
			dstG[i][j] = srcG[srcI][srcJ];
			dstB[i][j] = srcB[srcI][srcJ];

			srcJ++;
		}

		srcI++;
	}

	return;
}



CPixel CAdvImage::GetNeighbor(CPixel inPix, int direction)
{
	int x = inPix.x;
	int y = inPix.y;

	switch(direction)
	{
	case 0:
		x = x+1;
		break;
	case 1:
		x = x+1;
		y = y-1;
		break;
	case 2:
		y = y-1;
		break;
	case 3:
		x = x-1;
		y = y-1;
		break;
	case 4:
		x = x-1;
		break;
	case 5:
		x = x-1;
		y = y+1;
		break;
	case 6:
		y = y+1;
		break;
	case 7:
		x = x+1;
		y = y+1;
		break;
	}
	
	CPixel retPixel;
	
	retPixel.x = x;
	retPixel.y = y;

	return retPixel;
}


CPixelList * CAdvImage::Contour()	//I think this finds the perimeter (contour) of the shape?
{
	int imWidth = GetWidth();
	int imHeight = GetHeight();

	CPixelList * plist = new CPixelList;

	CPixel startPoint;
	//1. Find a point on the edge as the start point
	BOOL bFound = FALSE;
	for(int j=imHeight-1 ; j>=0; j--)
	{
		for(int i=imWidth-1; i>=0; i--)
		{
			if(GetPixelAt(i,j,0)!=0)
			{
				startPoint.x = i;
				startPoint.y = j;
				bFound = TRUE;
				break;
			}
		}
		if(bFound)
			break;
	}

	//2. begin contour tracing
	int direction;
	direction = 1;
	
	CPoint firstPoint;
	firstPoint.x = startPoint.x;
	firstPoint.y = startPoint.y;
    plist->push_back(startPoint);
	
	int	dir;

	while(1)
	{
		if(isEVEN(direction))					//Tests if direction is even? why?
			direction = MOD(direction+7,8);		//Significance of 7 and 8?
		else
			direction = MOD(direction+6,8);		//Significance of 6 and 8?

		for(int i=0;i<8;i++)					//Checks the 8 neighboring pixels
		{
			dir=MOD(i+direction,8);
			
			CPixel pixel = GetNeighbor(startPoint,dir);
			if( (pixel.x<0) || 					//Checks if pixel is inside image
				(pixel.x>imWidth-1)||
				(pixel.y<0) ||
				(pixel.y>imHeight-1))
				continue;						

			if((pixel.x == firstPoint.x)&&(pixel.y == firstPoint.y))
				return plist;
			
            if(GetPixelAt(pixel.x,pixel.y,0)>0)
			{
				if((pixel.x==startPoint.x)&&(pixel.y==startPoint.y))
					return plist;
				
				plist->push_back(pixel);

				startPoint=pixel;
				break;
			}
		}

		direction = dir;
	}

}

CPixelList * CAdvImage::NegativeContour()	// Used to find pericarp boundary
{
	int imWidth = GetWidth();
	int imHeight = GetHeight();
	int centerx = imWidth/2;
	int centery = imHeight/2;

	CPixelList * plist = new CPixelList;

	CPixel startPoint;
	//1. Find a point on the pericarp boundary as the start point
	BOOL bFound = FALSE;

	// If the center of the fruit is black
	if (GetPixelAt(centerx,centery,0)==0) {
		for(int i=centerx+1; i<=imWidth-1; i++) 
		{
			if(GetPixelAt(i,centery,0)!=0)
			{
				startPoint.x = i;
				startPoint.y = centery;
				bFound = TRUE;
				break;
			}

			if (i==imWidth-1)
				return plist;
		}
	}

	// If the center of the fruit is not black
	else {
		for(int i=centerx+1; i<=imWidth-1; i++) 
		{
			if (i==imWidth-1)
				return plist;
			
			if(GetPixelAt(i,centery,0)==0)
			{
				startPoint.x = i-1;
				startPoint.y = centery;
				bFound = TRUE;

				for(int j=i+1; j<=imWidth-1;j++)
				{
					if (GetPixelAt(j,centery,0)!=0)
						break;
					if (j==imWidth-1)
						return plist;
				}

				break;
			}

			
		}
	}


	//2. begin contour tracing
	int direction;
	direction = 1;
	
	CPoint firstPoint;
	firstPoint.x = startPoint.x;
	firstPoint.y = startPoint.y;
    plist->push_back(startPoint);
	
	int	dir;
	int count = 0;

	while(1)
	{
		//if(isEVEN(direction))					//Tests if direction is even? why?
		//	direction = MOD(direction+7,8);		//Significance of 7 and 8?
		//else
		//	direction = MOD(direction+6,8);		//Significance of 6 and 8?

		direction = MOD(direction+6,8);

		for(int i=0;i<8;i++)					//Checks the 8 neighboring pixels
		{
			dir=MOD(i+direction,8);
			
			CPixel pixel = GetNeighbor(startPoint,dir);
			if( (pixel.x<0) || 					//Checks if pixel is inside image
				(pixel.x>imWidth-1)||
				(pixel.y<0) ||
				(pixel.y>imHeight-1))
				continue;						

			if((pixel.x == firstPoint.x)&&(pixel.y == firstPoint.y)) {
				if ((dir == direction) && (dir == 1) && (count < 5))
					continue;
				return plist;
			}
			
            if(GetPixelAt(pixel.x,pixel.y,0)>0)
			{
				if((pixel.x==startPoint.x)&&(pixel.y==startPoint.y))
					return plist;
				
				plist->push_back(pixel);
				count++;

				startPoint=pixel;
				break;
			}
		}

		direction = dir;

	}

}

#define pi 3.1415926535897932

/** 
 * Rotates this image by the given angle (radians), changing its width, height, and 
 * total number of pixels. This changes the number and arrangement of pixels returned by 
 * Contour (e.g., a vertical line of pixels in the original image would map to a diagonal
 * line of fewer pixels in the rotated image). 
 */
CAdvImage * CAdvImage::Rotate(double angle)	//Actual image rotation here
{
	double sint = sin(angle);
	double cost = cos(angle);

    double fWidth = (double) GetWidth() - 1.0;
    double fHeight= (double) GetHeight()- 1.0;

	while(angle<0)
		angle+=(pi*2);

	while(angle>(2*pi))
		angle-=(pi*2);

	int xRotLeft, xRotRight, yRotTop, yRotBottom;
	int iRotateAmp = int(angle*2.0/pi);
	iRotateAmp = iRotateAmp % 4;
	switch(iRotateAmp)
	{
		case 0:
			 xRotLeft  = round(-fHeight*sint);
			 xRotRight = round(fWidth*cost);
			 yRotTop   = round(fWidth*sint + fHeight*cost);
			 yRotBottom= 0;
			 break;

		case 1:
			 xRotLeft  = round(fWidth*cost - fHeight*sint);
			 xRotRight = 0;
			 yRotTop   = round(fWidth*sint);
			 yRotBottom= round(fHeight*cost);
			 break;

		case 2:
			 xRotLeft  = round(fWidth*cost);
			 xRotRight = round(-fHeight*sint);
			 yRotTop   = 0;
			 yRotBottom= round(fWidth*sint + fHeight*cost);
			 break;

		case 3:
			 xRotLeft  = 0;
			 xRotRight = round(fWidth*cost - fHeight*sint);
			 yRotTop   = round(fHeight*cost);
			 yRotBottom= round(fWidth*sint);
			 break;
	}

	int nNewWidth  = xRotRight - xRotLeft;
	int nNewHeight = yRotTop - yRotBottom;
	CAdvImage * pNewImage = new CAdvImage;
    if(3 == GetNumBands())
        pNewImage->CreateRGB(nNewWidth+1, nNewHeight+1);
    else
        pNewImage->CreateGrayScale(nNewWidth+1, nNewHeight+1);

    if(NULL == pNewImage->mImage)
    {
        delete pNewImage;
        return NULL;
    }

	unsigned char** destR = (unsigned char**)pNewImage->GetBand(0);
	unsigned char** srceR = (unsigned char**)GetBand(0);
    unsigned char** destG;
    unsigned char** destB;
    unsigned char** srceG;
    unsigned char** srceB;
    if(GetNumBands()>1)
    {
        destG = (unsigned char**)pNewImage->GetBand(1);
        destB = (unsigned char**)pNewImage->GetBand(2);
	    srceG = (unsigned char**)GetBand(1);
	    srceB = (unsigned char**)GetBand(2);
    }
    else
    {
        destG = destR;
        destB = destR;
	    srceG = srceR;
	    srceB = srceR;
    }

    int nOrgHeight = (int)fHeight;
    int nOrgWidth = (int)fWidth;
	int i, j;
    i = 0;
	for(int y=yRotTop; y>=yRotBottom; y--)
	{
		j=0;

		for(int x=xRotLeft; x<=xRotRight; x++)
		{
			int xOrg = round(x*cost + y*sint);
			int yOrg = nOrgHeight-round(-x*sint + y*cost);

			if(xOrg<=nOrgWidth && xOrg>=0 && yOrg<=nOrgHeight && yOrg>=0)
			{
				destR[i][j] = srceR[yOrg][xOrg];
				destG[i][j] = srceG[yOrg][xOrg];
				destB[i][j] = srceB[yOrg][xOrg];
			}
            
			j++;
		}

		i++;
	}
    return pNewImage;
}

void CAdvImage::Strip()
{
    int nWidth = GetWidth();
    int nHeight= GetHeight();

    unsigned char ** pp = GetBand(0);

    int     i, j;
    RECT    rt;
    unsigned char * ppix;

    for(i=0; i<nHeight; i++)
    {
        ppix = pp[i];
        for(j=0; j<nWidth; j++)
        {
            if(ppix[j]==255)
            {
                rt.top = i;
                break;
            }
        }
    }

    for(i=nHeight-1; i>=0; i--)
    {
        ppix = pp[i];
        for(j=0; j<nWidth; j++)
        {
            if(ppix[j]==255)
            {
                rt.bottom = i;
                break;
            }
        }
    }

    for(j=0; j<nWidth; j++)
    {
        for(i=0; i<nHeight; i++)
        {
            if(pp[i][j]==255)
            {
                rt.left = i;
                break;
            }
        }
    }

    for(j=0; j<nWidth; j++)
    {
        for(i=0; i<nHeight; i++)
        {
            if(pp[i][j]==255)
            {
                rt.left = i;
                break;
            }
        }
    }


    return;
}