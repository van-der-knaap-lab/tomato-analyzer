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
//File CImage.cpp
//Andrew L. Hoffmaster
//The Ohio State University 
//3/28/01

// CImage class represents an image with basic IO capabilities.
// It is capable of reading/writing various image formats:
// PBM, PGM, PPM, EPS, TIF, GIF, RAS, ITX,IRIS, CCC,
// BIN, VIP, GLR, BTC, BRC, HUF, ZVL, ARITH, BTC2, BTC3,
// DPC, ZON, ZON2, SAFVR, JPG, WVQ, FRA, VQ

// This class uses CVIPtools for image IO.

// Comments:
// Simply declaring an ImageIO does not allow you to work with an image.
// You must first call Read to read an image from a file,
// or CreateRGB to create an RGB image of specified size.

#ifndef _CIMAGE_H
#define _CIMAGE_H

#include <CVIPimage.h>



class CImage
{
	public:
		CImage();

	virtual ~CImage()
	{
		if(mImage)
			::delete_Image(mImage);
	}

	void CreateRGB(int width, int height);  // create an RGB image
	void CreateGrayScale(int width, int height); // create a grayscale image
	BOOL Read(const char *filename);		// read image from file
	void Write(const char *filename);		// write image to file
	CImage* GetCopy();				// return a duplicate image
	int GetWidth();					// return # of horizontal pixels
	int GetHeight();				// return # of vertical pixels
	int GetNumBands();              // return # of bands (color planes)
	COLOR_FORMAT GetColorSpace();   // return color space of image
		// BINARY, GRAY_SCALE, RGB, HSL, HSV, SCT, CCT, LUV, LAB, XYZ

	// Return pixel value at (x,y) for the colorplane
	// colorplane: 0, 1, 2 corresponds to R, G, B
	unsigned char GetPixelAt(int x, int y, int colorplane);

	// sets pixel value at (x,y)
	void SetPixelAt(int x, int y, int colorplane, unsigned char value);

	// Returns the 2D array associated with the color plane.
	// This is for allowing direct access to pixels.
	unsigned char** GetPixelArray(int colorplane);

	bool IsValidImage();  // true if Read or Create was called.

	void Update(bool b);	// Call this function with the argument "true"
							//whenever the image was updated and
							// you want the change to reflect on the screen.
							// Basically, this is for faster viewing of the image.

	bool NeedUpdate();		// Used by the viewer.

	float GetScale() { return mScale; }
	void SetScale(float inScale) { mScale = inScale; }

	Image *mImage;    // CVIP image.	
	bool mUpdate;	// true if the image needs updating on the screen.
	float mScale;
};


#endif