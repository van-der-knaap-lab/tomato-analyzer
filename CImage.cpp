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
//Computer and INformation Sciences
//3/28/01

//IMPORTANT LESSON: of the day.  Must include "stdafx.h" as the first file
// in every C++ declaration.  TWGD.

#include "stdafx.h"
#include "CImage.h"
#include "CVIPdecls.h"


CImage::CImage()
{
	mImage = NULL;
	mUpdate = true;
}

//Wrapper Class to read and write CVIP images.

void CImage::CreateRGB(int width, int height)
{
	if(mImage)
		::delete_Image(mImage);

	mImage = new_Image(PPM, RGB, 3, height, width, CVIP_BYTE, REAL);

	unsigned char** destR = (unsigned char**)mImage->image_ptr[0]->rptr;
	unsigned char** destG = (unsigned char**)mImage->image_ptr[1]->rptr;
	unsigned char** destB = (unsigned char**)mImage->image_ptr[2]->rptr;
	
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			destR[y][x] = 0;
			destG[y][x] = 0;
			destB[y][x] = 0;
		}
	}
}

void CImage::CreateGrayScale(int width, int height)
{
	if(mImage)
		::delete_Image(mImage);

	mImage = new_Image(PPM, GRAY_SCALE, 1, height, width, CVIP_BYTE, REAL);

	unsigned char** destR = (unsigned char**)mImage->image_ptr[0]->rptr;
	
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			destR[y][x] = 0;
		}
	}
}

BOOL CImage::Read(const char *filename)
{
	const char *pdot = strrchr(filename, '.') + 1;

	// This commented code was once used to append a jpg file. Since CVIP tools updated its no longer an issue
	/*if(_stricmp(pdot,"jpg")==0)
	{
		// Change the header of the JPEG to "ff d8 ff e0" instead
		// of "ff d8 ff e1" since the CVIP image reader does not
		// recognize it as a jpeg if the 4th byte is "e1".
		char code = (char)0xe0;

		CFile file;
		BOOL b=file.Open(filename, CFile::modeWrite);
		if( !b )
		{
			if( mImage )
				delete_Image(mImage);
			mImage = NULL;

			return FALSE;
		}
		file.Seek(3, CFile::begin);
		file.Write(&code, 1);
		file.Close();
	}*/

	if(mImage)
		delete_Image(mImage);

	IMAGE_FORMAT format;
	format = getFormat_CVIP((char *)filename);
	mImage = read_Image((char *)filename, 0);

	if(mImage == NULL) return false; // Nancy added this. (06/15/05)

	// Force the image to be a color (RGB) image if it is monochrome.
	if(mImage->bands == 1)
	{
		Image* tempimg = new_Image(PPM, RGB, 3, 
			getNoOfRows_Image(mImage), getNoOfCols_Image(mImage), CVIP_BYTE, REAL);

		unsigned char** destR = (unsigned char**)tempimg->image_ptr[0]->rptr;
		unsigned char** destG = (unsigned char**)tempimg->image_ptr[1]->rptr;
		unsigned char** destB = (unsigned char**)tempimg->image_ptr[2]->rptr;
		unsigned char** src = (unsigned char**)mImage->image_ptr[0]->rptr;

		for(unsigned int y=0; y<getNoOfRows_Image(mImage); y++)
		{
			for(unsigned int x=0; x<getNoOfCols_Image(mImage); x++)
			{
				destR[y][x] = src[y][x];
				destG[y][x] = src[y][x];
				destB[y][x] = src[y][x];
			}
		}
		delete_Image(mImage);
		mImage = tempimg;
	}
	mUpdate = true;
  return TRUE;
}

void CImage::Write(const char *filename)
{
	// Look at the extension for image format.
	const char *pdot = strrchr(filename, '.') + 1;
	IMAGE_FORMAT format;


	if(_stricmp(pdot,"gif")==0)
	{
		format = GIF;
	}
	else if(_stricmp(pdot,"jpg")==0)
	{
		format = JPG;
	}
	else if(_stricmp(pdot,"tif")==0)
	{
		format = TIF;
	}
	else if(_stricmp(pdot,"pbm")==0)
	{
		format = PBM;
	}
	else if(_stricmp(pdot,"pgm")==0)
	{
		format = PGM;
	}
	else if(_stricmp(pdot,"ppm")==0)
	{
		format = PPM;
	}
	else if(_stricmp(pdot,"eps")==0)
	{
		format = EPS;
	}
	else
		format = TIF;  // Let TIF be the default format

	write_Image(mImage, (char *)filename, CVIP_YES, CVIP_NO, format, CVIP_NO);
}

CImage* CImage::GetCopy()
{
	CImage* newimg = new CImage;
	newimg->mImage = duplicate_Image(mImage);

	return newimg;
}

bool CImage::NeedUpdate()
{
	return mUpdate;
}

COLOR_FORMAT CImage::GetColorSpace()
{
	return mImage->color_space;
}

void CImage::Update(bool b)
{
	mUpdate = b;
}

unsigned char** CImage::GetPixelArray(int colorplane)
{
	ASSERT(colorplane < GetNumBands());
	return (unsigned char**)mImage->image_ptr[colorplane]->rptr;
}

bool CImage::IsValidImage()
{
	return (mImage != NULL);
}

int CImage::GetWidth()
{
	ASSERT(mImage!=NULL);
	return getNoOfCols_Image(mImage);
}

int CImage::GetHeight()
{
	//ASSERT(mImage!=NULL);
	return getNoOfRows_Image(mImage);
}

unsigned char CImage::GetPixelAt(int x, int y, int colorplane)
{
	return mImage->image_ptr[colorplane]->rptr[y][x];
}

void CImage::SetPixelAt(int x, int y, int colorplane, unsigned char value)
{
	ASSERT(colorplane < mImage->bands);
	ASSERT((unsigned int)x < mImage->image_ptr[colorplane]->cols);
	ASSERT((unsigned int)y < mImage->image_ptr[colorplane]->rows);
	mImage->image_ptr[colorplane]->rptr[y][x] = value;
}

int CImage::GetNumBands()
{
	return mImage->bands;
}
