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
// AdvImage.h: interface for the CAdvImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADVIMAGE_H__06528451_069F_4F96_AB46_324E78DE0482__INCLUDED_)
#define AFX_ADVIMAGE_H__06528451_069F_4F96_AB46_324E78DE0482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CImage.h"
#include "PixelList.h"
#include "vector"
#include "ImgTool.h"
#include "BwImage.h"

class CBinaryObjects;
class CBinObjs;
class CBoundingBox;

// we adjust the threshold by 10% -- it just works. 
const float THESHOLD_ADJUST_CONST = 0.1;

/**
 * This class will support the implementation of some advanced image manipulation features 
 */
class CAdvImage : public CImage  
{
public:
	friend class CTomatoAnalyzerApp;
	CAdvImage(int iWidth, int iHeight);
	CAdvImage();

    void Serialize(CArchive &ar);

	CPixelList *  Contour(void);
	CPixelList *  NegativeContour(void);
	CPixel GetNeighbor(CPixel inPix, int direction);

	virtual ~CAdvImage();
	CAdvImage * GetCopy();
	CAdvImage * Crop(CRect selectRect);
	CAdvImage * CropTwo(CRect selectRect);
	void Paste(int nX, int nY, CAdvImage * pPasteImage);
    void SetForegroundColorReplacement(bool bReplace){mChangeForegroundColor=bReplace;}

    CAdvImage* Rotate(double angle);

	/// Not used? 
	///The actual thresholding algorithm based on RGB values. 
	///(inverse == true ? 1. : 2.)
	///	1. start with pixelcolor = white.  
	///	2. pixelcolor = black
	///	If pixel(r,g,b) in any band range {in(R,G,B)Max, in(R,G,B)Min} respectively
	///		1. pixelcolor = black
	///		2. pixelcolor = white
	CAdvImage * Threshold(float inRMax, float inGMax, float inBMax,
							float inRMin, float inGMin, float inBMin, bool inverse = false);

	virtual CBinObjs * GetBinaryObjects();

	///Given a map of binary objects, this routine steps through each object list, coloring each
	///	individual binary object a unique color.  The pixels in the list are used to index into
	///	the underlying CVIPImage object.
	void ColorImageObjects(CBinObjs * inList);


	///These two routines are very similar.  Threshold HSV has some complicated behavior.  See the
	///	implementation for the usage comments.
	CAdvImage * ThresholdHSV(	float inh1, float ins1, float inv1, 
								float inh2 = 0, float ins2 = 0, float inv2 = 0,
								bool inKeep = false);

	///Debug routine for custom behavior.
	CAdvImage * ThresholdHSVDebug ( float inh1, float ins1, float inv1, 
									float inh2 = 0, float ins2 = 0, float inv2 = 0,
									bool inKeep = false);

	CAdvImage * ThresholdSpecial();

	///If the channel value is <= inVal then if black is true outimg pixel is black.
	CAdvImage * ThresholdGeneric(float inVal, float inChannel, bool black);

	CAdvImage * ThresholdLocally(int inWDivs, int inHDivs, int inChannel, float inHistoPercent, int nDesired, int& nNew, int nBlue);

	double findHue(double ValL, double ValA, double ValB);

	void * CAdvImage::RGBtoLAB(int r, int g, int b, double lab[]);

	int ThresholdSubImage(CRect inSubImgRect, CAdvImage * resultImg, int inChannel, float inHistoPercent, int nDesire, int nBlue);

	///Static functiosn that will take an rgb color, and convert it to hsv.  HSV coordinates are
	///	returned in range {0..255, 0...255, 0...255} respectively, and most likely should be
	///	immediately converted to ranges {0...360, 0...100, 0...100} as this is the traditional view.
	static void RGBtoHSV( float r, float g, float b, float & h, float & s, float & v );
	///Static function converts an hsv color to an rgb color in ranges {0...255, 0...255, 0...255} 
	///	respectively.
	static void HSVtoRGB( float & r, float & g, float & b, float h, float s, float v );
	static void GetRandomColor(int & r, int & g, int & b);

	///This procedure steps through all pixel locations in the inList, and sets their color to the
	///	color defined by r, g, b.
	virtual void ColorBinaryObject(CPixelList & inList, float inr, float ing, float inb);

	//The median filter is used to reduce noise in an image while preserving useful detail.
	//	masksize represents the size of the box filter used to filter the image.
	//	This function operates on the CAdvImage object being called on.  If an duplicate
	//	filtered image is desired, one should duplicate the image to be filtered first,
	//	and then call the MedianFilter() on that duplicated image.
//	void MedianFilter(int masksize);

	///this routine will go and draw a square in the this image of dimensions 1 + 2 * size of
	/// color rgb centered at every pixel in inList.
	void MarkImage(CPixelList * inList, COLORREF inColor, int size);
	
	CAdvImage * ThinImage();

	///All pixels with rgb not in (0, 0,0) are set to (255, 255, 255);
	void MakeBinary();
	
	///Something quick to avoid all of those stupid casts.
	unsigned char ** GetBand(int inWhich);

	///To mark the image with a line.  A safe function with bounds checking.
	void DrawLine(CPixel inEndPoint1, CPixel inEndPoint2, float inR, float inG, float inB);

	///This routine will for every mPixelList in inObjs it will fill in the color on 
	///	the calling image to the pixel color found in inSourceColor image.
	void FillInColorInformation(CBinObjs * inObjs, CAdvImage * inSourceColor);

	///This routine will color the pixels specified in inList the color specified in 
	///	inImage in the this object.
	void CAdvImage::ColorWith(CPixelList & inList, CAdvImage * inImage);

	void FillIn(float inR = 0.0f, float inG = 0.0f, float inB = 0.0f);

	void DrawBoundingBox(CBoundingBox & inBBox, float inR, float inG, float inB);


	///CVIP tools wrappers for erode and dilate operations.
	///<k_type>  -  kernel  type  (1=disk   2=square   3=rectangle 4=cross)
	///ksize - height and width of mask.
	///height - height for square rectangle and width of cross.
	///width - width for rectangle, size of cross.
	void Erode(int k_type, int  ksize, int height, int width);

	void Dilate(int k_type, int  ksize, int height, int width);

	void MedianFilter(int filterSize, bool blackAndWhite = true);
	CAdvImage * MedianFilterImage(int filterSize, bool blackAndWhite = true);


    std::vector<ipt::IPTPOINT>*  toPointLst(int iChannel, ipt::IPTPOINT ctr);
    CAdvImage& fromIptImage(const CIptBwImage& bwImg);

    CAdvImage& operator = (CAdvImage& other);
    CAdvImage&  YUV(CAdvImage& other);

    int GetArea(void);
    void CutImageByLine(CPixel p1, CPixel p2, BOOL flag);

    void Strip(void);



	int mTVal;
	CAdvImage * TestOp();
protected:
	///helper function that takes in two key values to the map inBinObjs representing two CPixelList(s)
	///	that should be merged.  Let the lists represented by label1 and label2 be respectively known as
	///	l1 and l2.  Besides just merging l1 and l2, this routing changes all pixel locations in l2 
	///	referenced into image inLabelImage) to have label == label1.  It deletes the map entry for
	///	l2 from inBinObjs.
	void CombineLabelLists(CBinObjs * inBinObjs, int label1, int label2, unsigned int ** inLabelImage);
	
	///A helper function for Yusaku's Image Thinning procedure based on Hilditch's thinning algorithm.
	int cconc(int inb[9]);

	///Two static helper functions that determine the max and min of three floating point values.
	static float max3(float a, float b, float c);
	static float min3(float a, float b, float c);
	void Set(unsigned char & inR, unsigned char & inG, unsigned char & inB,
				unsigned char rval, unsigned char gval, unsigned char bval);
	///int compare(char* a, char* b) ;
	bool mChangeForegroundColor;	

};

#endif // !defined(AFX_ADVIMAGE_H__06528451_069F_4F96_AB46_324E78DE0482__INCLUDED_)
