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
#include "float.h"
#include "imgTool.h"
#include "linreg.h"

//#include "math.h"

#include "AdvImage.h"

#include <iostream> 
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream> 


#include "new_img_function.h"

using namespace std;
using namespace ipt;


double * getTestData(char * fname, int& nLength);
void saveTmp(char * fn, INTVECTOR * pCp);
int indentation(CAdvImage * pImg, double *x, double *y, int nLength, int& nFrom, int& nTo);

#define pi 3.1415926535897932

CPixelList * MakeLine(double fx1, double fy1, double fx2, double fy2);

typedef struct tag_curvattr
{
	bool   isStraightLine;
	int    type;
	int    nFrom;
	int    nTo;
	int    maxPos;
	int    nLength;
	double sumHeight;
	double maxHeight;
}curvattr, * pcurvattr;



CAdvImage *
CIimresize(CAdvImage * pImg, int nNewWidth, int nNewHeight, LPCTSTR lpszMethod)
{
	if( NULL==pImg || nNewWidth<0 || nNewHeight<0 )
		return NULL;

	int nOrgWidth = pImg->GetWidth();
	int nOrgHeight = pImg->GetHeight();
	if( nOrgWidth<=0 || nOrgHeight<=0 )
		return NULL;

	if( 'n'==lpszMethod[0] || 'N'==lpszMethod[0])
	{
		unsigned char** destR;
		unsigned char** destG;
		unsigned char** destB;
		unsigned char** srcR;
		unsigned char** srcG;
		unsigned char** srcB;

		int nChannelNumber = pImg->GetNumBands();
		CAdvImage * pScaledImg = new CAdvImage;
        int nBands = pImg->GetNumBands();
        if(3==nBands)
        {
            pScaledImg->CreateRGB(nNewWidth, nNewHeight);
            destR = pScaledImg->GetBand(0);
            destG = pScaledImg->GetBand(1);
            destB = pScaledImg->GetBand(2);
            srcR = pImg->GetBand(0);
            srcG = pImg->GetBand(1);
            srcB = pImg->GetBand(2);
        }
        else
        {
            pScaledImg->CreateGrayScale(nNewWidth, nNewHeight);
            destG = destB = destR = pScaledImg->GetBand(0);
            srcR = srcG = srcB = pImg->GetBand(0);
        }

		float scH = float(nOrgHeight)/float(nNewHeight) + FLT_EPSILON;
		float scW = float(nOrgWidth)/float(nNewWidth) + FLT_EPSILON;

		float fNearRow = scH/2.0;
		for(int i=0; i<nNewHeight; i++)
		{
			int nSrcRow = (int) floorf(fNearRow);

			float fNearCol = scW/2.0;
			for(int j=0; j<nNewWidth; j++)
			{
				int nSrcCol = (int) floorf(fNearCol);

				destR[i][j] = srcR[nSrcRow][nSrcCol];
				destG[i][j] = srcG[nSrcRow][nSrcCol];
				destB[i][j] = srcB[nSrcRow][nSrcCol];

				fNearCol += scW;
			}

			fNearRow += scH;
		}

		return pScaledImg;
	}
	
	return NULL;
}



CAdvImage *
CIimresize(CAdvImage * pImg, float fs, LPCTSTR lpszMethod)
{
	if( NULL == pImg)
		return NULL;

	int nOldWidth  = pImg->GetWidth();
	int nOldHeight = pImg->GetHeight();

	int nNewWidth  = (int) floorf(fs * float(nOldWidth));
	int nNewHeight = (int) floorf(fs * float(nOldHeight));
	if( nNewWidth < 1 )
		nNewWidth = 1;
	if( nNewHeight < 1 )
		nNewHeight = 1;

	return CIimresize(pImg, nNewWidth, nNewHeight, lpszMethod);
}


CBitmap * CreateBitmap(CDC* pDC, CAdvImage * pImage)
{
	if(NULL == pImage)
		return NULL;

	int width  = pImage->GetWidth();
	int height = pImage->GetHeight();

	CBitmap * m_pBitmap = new CBitmap();
	BOOL state = m_pBitmap->CreateCompatibleBitmap(pDC, width, height);
	
	BITMAP bitmapinfo;
	m_pBitmap->GetBitmap(&bitmapinfo);  // extract bitmap info

 	int bitsPixel = bitmapinfo.bmBitsPixel;
	if(bitsPixel<16)
	{
		AfxMessageBox("This application must run in 16-bit color mode or better.");
		delete m_pBitmap;
		return NULL;
	}

	unsigned char** rpix = pImage->GetPixelArray(0);
	unsigned char** gpix = NULL;
	unsigned char** bpix = NULL;
	if(pImage->GetNumBands() >= 2)
			gpix = pImage->GetPixelArray(1);
	if(pImage->GetNumBands() >= 3)
			bpix = pImage->GetPixelArray(2);

	unsigned char * mBMPixels = NULL;
	if(bitsPixel == 16)
		mBMPixels = new unsigned char[width*height*2];
	else if(bitsPixel == 24)
		mBMPixels = new unsigned char[width*height*3];
	else if(bitsPixel == 32)
		mBMPixels = new unsigned char[width*height*4];
	else
	{
		AfxMessageBox("Unsupported number of bits per pixel!");
		delete m_pBitmap;
		return NULL;
	}

	for(int j=0; j<height; j++)
	{
		for(int i=0; i<width; i++)
		{
			switch(bitsPixel)
			{
					case 16:
						// and 6 bits for green - Check the color mask for RGB555 or RGB565

						// implementation for RGB565
						if(pImage->GetNumBands() == 1)
						{
							if(pImage->GetColorSpace() != BINARY)
							{
								// Gray scale image.
								mBMPixels[(j*width+i)*2+1] =
									(((unsigned char)(rpix[j][i]) >> 3) << 3) |
									((unsigned char)(rpix[j][i]) >> 5);
								mBMPixels[(j*width+i)*2] =
									(((unsigned char)(rpix[j][i]) >> 2) << 5) |
									((unsigned char)(rpix[j][i]) >> 3);
							}
							else
							{
								// Binary image.
								mBMPixels[(j*width+i)*2+1] =
									(((unsigned char)(rpix[j][i]*255) >> 3) << 3) |
									((unsigned char)(rpix[j][i]*255) >> 5);
								mBMPixels[(j*width+i)*2] =
									(((unsigned char)(rpix[j][i]*255) >> 2) << 5) |
									((unsigned char)(rpix[j][i]*255) >> 3);

							}
						}
						else
						{
							if(pImage->GetColorSpace() != BINARY)
							{
								mBMPixels[(j*width+i)*2+1] =
									(((unsigned char)(rpix[j][i]) >> 3) << 3) |
									((unsigned char)(gpix[j][i]) >> 5);
								mBMPixels[(j*width+i)*2] =
									(((unsigned char)(gpix[j][i]) >> 2) << 5) |
									((unsigned char)(bpix[j][i]) >> 3);
							}
							else
							{
								mBMPixels[(j*width+i)*2+1] =
									(((unsigned char)(rpix[j][i]*255) >> 3) << 3) |
									((unsigned char)(gpix[j][i]*255) >> 5);
								mBMPixels[(j*width+i)*2] =
									(((unsigned char)(gpix[j][i]*255) >> 2) << 5) |
									((unsigned char)(bpix[j][i]*255) >> 3);
							}
						}
						break;
					case 24:
						if(pImage->GetColorSpace() != BINARY)
						{
							mBMPixels[(j*bitmapinfo.bmWidthBytes+i)*3]   = bpix[j][i];
							mBMPixels[(j*bitmapinfo.bmWidthBytes+i)*3+1] = gpix[j][i];
							mBMPixels[(j*bitmapinfo.bmWidthBytes+i)*3+2] = rpix[j][i];
/*
							mBMPixels[(j*width+i)*3] =
								(bpix[j][i] << 16) |
								(gpix[j][i] << 8) |
								(rpix[j][i]);
*/
						}
						else
						{
							mBMPixels[(j*width+i)*3] =
								((bpix[j][i]*255) << 16) |
								((gpix[j][i]*255) << 8) |
								(rpix[j][i]*255);
						}	
						break;
					case 32:
						if(pImage->GetColorSpace() != BINARY)
						{
							mBMPixels[(j*width+i)*4] = 0;
							mBMPixels[(j*width+i)*4+2] = rpix[j][i];
							mBMPixels[(j*width+i)*4+1] = gpix[j][i];
							mBMPixels[(j*width+i)*4] = bpix[j][i];
						}
						else
						{
							mBMPixels[(j*width+i)*4] = 0;
							mBMPixels[(j*width+i)*4+2] = rpix[j][i]*255;
							mBMPixels[(j*width+i)*4+1] = gpix[j][i]*255;
							mBMPixels[(j*width+i)*4] = bpix[j][i]*255;

						}
						break;
					default:
						AfxMessageBox("Invalid number of bits per pixel!");
			}
		}
	}

	if(bitsPixel==16)
		m_pBitmap->SetBitmapBits(width*height*2, mBMPixels);
	else if(bitsPixel==24)
		m_pBitmap->SetBitmapBits(width*height*3, mBMPixels);
	else  // bitsPixel == 32
		m_pBitmap->SetBitmapBits(width*height*4, mBMPixels);

	delete[] mBMPixels;

	return m_pBitmap;
}

CAdvImage * CreateAdvImage(CDC* pDC, CBitmap * m_pBitmap)
{
    CAdvImage *   pImage;

	if(NULL == m_pBitmap)
		return NULL;

	BITMAP bitmapinfo;
	m_pBitmap->GetBitmap(&bitmapinfo);  // extract bitmap info
    int bitsPixel = bitmapinfo.bmBitsPixel;
	if(bitsPixel<=16)
	{
		AfxMessageBox("This application must run in 16-bit color mode or better.");
		return NULL;
	}

	int width  = bitmapinfo.bmWidth;
	int height = bitmapinfo.bmHeight;
    DWORD dwSize = bitmapinfo.bmWidthBytes*bitmapinfo.bmHeight*bitmapinfo.bmBitsPixel;
	unsigned char * mBMPixels = new unsigned char[dwSize];
 	if(NULL == mBMPixels)
        return NULL;
    m_pBitmap->GetBitmapBits(dwSize, mBMPixels);


    pImage = new CAdvImage(width, height);
	unsigned char** rpix = pImage->GetPixelArray(0);
	unsigned char** gpix = pImage->GetPixelArray(1);
	unsigned char** bpix = pImage->GetPixelArray(2);

    int iIndex;
	for(int j=0; j<height; j++)
	{
		for(int i=0; i<width; i++)
		{
			switch(bitsPixel)
			{
					case 24:
                        iIndex = (j*width+i)*3;
                        bpix[j][i] = mBMPixels[iIndex];
                        gpix[j][i] = mBMPixels[++iIndex];
                        rpix[j][i] = mBMPixels[++iIndex];
    					break;

					case 32:
                        iIndex = (j*width+i)*4;
                        bpix[j][i] = mBMPixels[iIndex];
                        gpix[j][i] = mBMPixels[++iIndex];
                        rpix[j][i] = mBMPixels[++iIndex];
						break;

					default:
						AfxMessageBox("Invalid number of bits per pixel!");
			}
		}
	}

	delete mBMPixels;
    return pImage;
}


CBitmap * 
CreateBitmap(CDC* pDC, CAdvImage * pImage, float fScale, LPCTSTR lpszMethod)
{
    if(pImage == NULL)
        return NULL;

	CBitmap * pBitmap;

	if(fScale != 1.0)
	{
		CAdvImage * ptemp = CIimresize(pImage, fScale, lpszMethod);
		pBitmap = CreateBitmap(pDC, ptemp);

		delete ptemp;
	}
	else
	{
		pBitmap = CreateBitmap(pDC, pImage);
	}

	return pBitmap;
}

CBitmap * 
CreateBitmap(CDC* pDC, CBitmap * pBitmapSrc, float fScale, LPCTSTR lpszMethod)
{
    if(pBitmapSrc == NULL)
        return NULL;

	CBitmap * pBitmap;

    CAdvImage * pImage = CreateAdvImage(pDC, pBitmapSrc);
	if(fScale != 1.0)
	{
		CAdvImage * ptemp = CIimresize(pImage, fScale, lpszMethod);
		pBitmap = CreateBitmap(pDC, ptemp);

		delete ptemp;
	}
	else
	{
		pBitmap = CreateBitmap(pDC, pImage);
	}

    delete pImage;
	return pBitmap;
}

CPixelList *
MakeLine(POINT p1, POINT p2)
{
	CPixelList * plst = new CPixelList;
	CPixel px;

	float fx, fy, dx, dy;
	int steps;

	dx = float(p2.x - p1.x);
	dy = float(p2.y - p1.y);

	fy = fabs(dy);
	fx = fabs(dx);
	if(fy > fx)
		steps = fy;
	else
		steps = fx;

	dx /= (float)steps;
	dy /= (float)steps;


	fx = 0.0;
	fy = 0.0;
	px.Set(p1.x, p1.y);
	plst->push_back(px);

	for(int i=0; i<steps; i++)
	{
		fx += dx;
		fy += dy;

		px.x = p1.x + (int) floorf(fx+0.5);
		px.y = p1.y + (int) floorf(fy+0.5);

		plst->push_back(px);
	}

	return plst;
}


int MOD(int x1, int x2)
{
	return x1-(x1/x2)*x2;
}

BOOL isEVEN(int x)
{
	if( MOD(x,2) == 0)
		return TRUE;
	else
		return FALSE;
}

int round(float fv)
{
	int nTmp = int(fv);
	
	if(fv==float(nTmp))
		return nTmp;

	if(fv<0)
		return int(fv-0.5);

	return int(fv+0.5);
}

bool TwoDimPCA(double ** d, int nCol, double& rot, double& mux, double& muy)
{
	int i, j, k;

	double mu[2];
	mu[0]=mu[1]=0.0;

	//compute mean vector
	for(i=0; i<2; i++)
	{
		for(j=0; j<nCol; j++)
			mu[i] += d[i][j];

		mu[i] /= double(nCol);

		for(j=0; j<nCol; j++)
			d[i][j] -= mu[i];
	}


	//Covariance matrix
	double cov[2][2];

	for(i=0; i<2; i++)
	{
		for(j=i; j<2; j++)
		{
			cov[i][j] = 0.0;

			for(k=0; k<nCol; k++)
			{
				cov[i][j] += (d[i][k] * d[j][k]);
			}
		}

		for(k=0; k<i; k++)
			cov[i][k] = cov[k][i]; 
	}

	double prod = cov[0][0]*cov[1][1] - cov[0][1]*cov[1][0];
	double varall = cov[0][0]+cov[1][1];
	double r1, r2;

	double tmp = cov[0][0]-cov[1][1];
	tmp = sqrt(tmp*tmp + 4.0*cov[0][1]*cov[0][1]);
	r1 = (varall + tmp)/2.0;
	r2 = (varall - tmp)/2.0;

	if(fabs(cov[0][1])<FLT_MIN)
	{
		rot=90.0;
		return false;
	}

	rot = atan(-(cov[0][0]-r1)/cov[0][1]);

	mux=mu[0];
	muy=mu[1];
	return true;
}  


double * conv(double * x, int nLenX, double * y, int nLenY)
{
	int i, nCenter;

	if((nLenY%2)==1)
	{
		nCenter = nLenY/2 ;
	}
	else
	{
		nCenter = nLenY/2 - 1;
	}

	double * tx = new double[nLenX + nLenY - 1];
	for(i=0; i<nCenter; i++)
	{
		tx[i]=x[nLenX-nCenter+i];
	}
	for(i=nCenter; i<nLenX+nCenter; i++)
	{
		tx[i]=x[i-nCenter];
	}
	for(i=nLenX+nCenter; i<nLenX+nLenY-1; i++)
	{
		tx[i]=x[i-nLenX-nCenter];
	}

	
	double * result = new double[nLenX];
	for(i=0; i<nLenX; i++)
	{
		result[i]=0.0;

		for(int j=0; j<nLenY; j++)
		{
			result[i] += tx[i+j]*y[nLenY-1-j];
		}
	}

	delete tx;
	
	return result;
}


double * GaussianCurvature(int xdat[], int ydat[], int nLength, double sigm, int * nx, int * ny)
{
	int * x = blipfilter(xdat, nLength);
	int * y = blipfilter(ydat, nLength);


	int nFilterHalfLength=int(floor(sigm*2.0));
	int nFilterLength=2*nFilterHalfLength + 1;

	double * g1d = new double[nFilterLength];
	double * g2d = new double[nFilterLength];
	double * g   = new double[nFilterLength]; 
	
	double sqrttp=sqrt(2*pi);
	double gsum=0.0;
	double sigmsqr=sigm*sigm;
	int i, j;
	for(i=-nFilterHalfLength, j=0; i<=nFilterHalfLength; i++, j++)
	{
		g[j]=exp((-0.5)*double(i*i)/sigmsqr)/sigm/sqrttp;
		gsum += g[j];
	}
	for(i=0; i<nFilterLength; i++)
	{
		g[i] /= gsum;
	}


	for(i=-nFilterHalfLength, j=0; i<=nFilterHalfLength; i++, j++)
	{
		g1d[j] = -double(i)*g[j]/sigmsqr;
		g2d[j] = -g[j]*(1-double(i*i)/sigmsqr)/sigmsqr;
	}

	double * dx = new double[nLength];
	double * dy = new double[nLength];
	for(i=0; i<nLength; i++)
	{
		dx[i] = double( x[i] );
		dy[i] = double( y[i] );
	}



	double * x1d = conv(dx, nLength, g1d, nFilterLength);
	double * x2d = conv(dx, nLength, g2d, nFilterLength);
	double * y1d = conv(dy, nLength, g1d, nFilterLength);
	double * y2d = conv(dy, nLength, g2d, nFilterLength);

	double * cuv = new double[nLength];
	for(i=0; i<nLength; i++)
	{
		double tmp = sqrt(x1d[i] * x1d[i] + y1d[i] * y1d[i]);

		cuv[i] = (x1d[i] * y2d[i] - x2d[i] * y1d[i])/tmp/tmp/tmp;
	}

	double * newx = conv(dx, nLength, g, nFilterLength);
	double * newy = conv(dy, nLength, g, nFilterLength);
	for(i=0; i<nLength; i++)
	{
		nx[i] = round(newx[i]);
		ny[i] = round(newy[i]);
	}
	
	delete x1d, x2d, y1d, y2d;
	delete g, g1d, g2d;
	delete dx, dy; 
	delete newx, newy;

	delete x, y;

	return cuv;
}

int signOf(double d)
{
	if(d>0)
		return 1;

	if(d<0)
		return -1;

	return 0;
}

int * findZeroCrossingPoint(double * data, int nLength, int& nFoundLength)
{
	int nFound=0;
	int * pos=new int[nLength];

	int sp, sc;
	sp=signOf(data[0]);
	for(int i=1; i<nLength; i++)
	{
		sc = signOf(data[i]);
		if(sp!=sc)
		{
			if( fabs(data[i]) > fabs(data[i-1]) )
				pos[nFound] = i-1;
			else
				pos[nFound] = i;

			nFound++;
		}

		sp = sc;
	}
	
	nFoundLength = nFound;
	return pos;
}

void sort(INTVECTOR * piv)
{
	INTVECTOR::iterator itr, itrNext, lastPos;

	itr=itrNext=piv->begin();
	lastPos=piv->end()-1;

	while(itr!=lastPos)
	{
		for(itrNext=itr+1; itrNext<=lastPos; ++itrNext)
		{
			if(*itrNext < *itr)
			{
				int ntemp = *itrNext;
				*itrNext = *itr;
				*itr = ntemp;
			}
		}
		++itr;
	}
}


template <class T> bool allLessThan(T * d, int nLength, T threshold)
{
	for(int i=0; i<nLength; i++)
	{
		if(d[i]>=threshold)
			return false;
	}

	return true;
}

template <class T> bool allLessOrEqualThan(T * d, int nLength, T threshold)
{
	for(int i=0; i<nLength; i++)
	{
		if(d[i]>threshold)
			return false;
	}

	return true;
}


template <class T> bool allLargerThan(T * d, int nLength, T threshold)
{
	for(int i=0; i<nLength; i++)
	{
		if(d[i]<=threshold)
			return false;
	}

	return true;
}

template <class T> T minValue(T * d, int nLength)
{
	T mnv = d[0];

	for(int i=1; i<nLength; i++)
	{
		if(mnv>d[i])
			mnv=d[i];
	}

	return mnv;
}

template <class T> T maxValue(T * d, int nLength)
{
	T maxv = d[0];

	for(int i=1; i<nLength; i++)
	{
		if(maxv<d[i])
			maxv=d[i];
	}

	return maxv;
}

template <class T> T maxValueWithIndex(T * d, int nLength, int * pos)
{
	int k=0;
	T maxv = d[0];

	for(int i=1; i<nLength; i++)
	{
		if(maxv<d[i])
		{
			maxv=d[i];
			k=i;
		}
	}

	if(NULL!=pos)
		*pos=k;

	return maxv;
}

template <class T> T minValueWithIndex(T * d, int nLength, int * pos)
{
	int k=0;
	T minv = d[0];

	for(int i=1; i<nLength; i++)
	{
		if(minv > d[i])
		{
			minv=d[i];
			k=i;
		}
	}

	if(NULL!=pos)
		*pos=k;

	return minv;
}

template <class T> int maxValueIndex(T * d, int nLength)
{
	int k=0;
	T maxv = d[0];

	for(int i=1; i<nLength; i++)
	{
		if(maxv<d[i])
		{
			maxv=d[i];
			k=i;
		}
	}

	return k;
}

template <class T> int minValueIndex(T * d, int nLength)
{
	int k=0;
	T minv = d[0];

	for(int i=1; i<nLength; i++)
	{
		if(minv > d[i])
		{
			minv = d[i];
			k=i;
		}
	}

	return k;
}

template <class T> int sign(T d)
{
	if(d>T(0))
		return 1;

	if(d<T(0))
		return -1;

	return 0;
}


INTVECTOR * findLocalMinMaxIndex(double *d, int nLength)
{
	INTVECTOR * pMMIndex = new INTVECTOR();

	for(int i=1; i<nLength-1; i++)
	{
		int signF = sign(d[i]-d[i-1]);
		int signB = sign(d[i+1]-d[i]);

		if( signF != signB)
		{
			pMMIndex->push_back(i);
		}
	}

	return pMMIndex;
}


INTVECTOR * findLocalMinimumIndex(double *d, int nLength)
{
	INTVECTOR * pMMIndex = new INTVECTOR();

	for(int i=1; i<nLength-1; i++)
	{
		int signF = sign(d[i]-d[i-1]);
		int signB = sign(d[i+1]-d[i]);

		if(signF<signB)
		{
			pMMIndex->push_back(i);
		}
	}

	return pMMIndex;
}

INTVECTOR * findLocalMaximumIndex(double *d, int nLength)
{
	INTVECTOR * pMMIndex = new INTVECTOR();

	for(int i=1; i<nLength-1; i++)
	{
		int signF = sign(d[i]-d[i-1]);
		int signB = sign(d[i+1]-d[i]);

		if(signF>signB)
		{
			pMMIndex->push_back(i);
		}
	}

	return pMMIndex;
}

int * findMaximumPoint(double * dataIn, int nLength, int& nMaxPointLength)
{
	int nFound = 0;
	
	int tmpMinPos=0;
	double tmpMin=0.0;
	double LocalMaxVal[10];
	for(int i=0; i<sizeof(LocalMaxVal)/sizeof(double); i++)
		LocalMaxVal[i]=0.0;

/*
	double * data = new double[nLength];
	for(i=0; i<nLength; i++)
		data[i]=fabs(dataIn[i]);
*/	
	double * data = dataIn;
	
	for(int i=1; i<nLength-1; i++)
	{
		if(data[i]>data[i-1] && data[i]>data[i+1])
		{
			if(data[i]>tmpMin)
			{
				nFound++;

				LocalMaxVal[tmpMinPos]=data[i];
				
				tmpMin=LocalMaxVal[0];
				tmpMinPos = 0;
				for(int j=1; j<sizeof(LocalMaxVal)/sizeof(double); j++)
				{
					if(tmpMin>LocalMaxVal[j])
					{
						tmpMin = LocalMaxVal[j];
						tmpMinPos = j;
					}
				}
			}
		}
	}

	if(nFound > sizeof(LocalMaxVal)/sizeof(double))
		nFound = sizeof(LocalMaxVal)/sizeof(double);

	double threshVal=0.0;
	for(int i=0; i<nFound; i++)
		threshVal += LocalMaxVal[i];

	threshVal /= (3.0*nFound);

	//minimum
	double tmpMax=0.0;
	int tmpMaxPos=0;
	nFound=0;
	for(int i=0; i<sizeof(LocalMaxVal)/sizeof(double); i++)
		LocalMaxVal[i]=0.0;

	for(int i=1; i<nLength-1; i++)
	{
		if(data[i]<data[i-1] && data[i]<data[i+1])
		{
			if(data[i]<tmpMax)
			{
				nFound++;

				LocalMaxVal[tmpMaxPos]=data[i];
				
				tmpMax=LocalMaxVal[0];
				tmpMaxPos = 0;
				for(int j=1; j<sizeof(LocalMaxVal)/sizeof(double); j++)
				{
					if(tmpMax<LocalMaxVal[j])
					{
						tmpMax = LocalMaxVal[j];
						tmpMaxPos = j;
					}
				}
			}
		}
	}

	if(nFound > sizeof(LocalMaxVal)/sizeof(double))
		nFound = sizeof(LocalMaxVal)/sizeof(double);

	double MinthreshVal=0.0;
	for(int i=0; i<nFound; i++)
		MinthreshVal += LocalMaxVal[i];

	MinthreshVal /= (3.0*nFound);

	nFound = 0;
	int * npos = new int[nLength];

	for(int i=1; i<nLength-1; i++)
	{
		if(data[i]>threshVal && data[i]>data[i-1] && data[i]>data[i+1])
		{
			npos[nFound]=i;
			++nFound;
		}
	}

	for(int i=1; i<nLength-1; i++)
	{
		if(data[i]<MinthreshVal && data[i]<data[i-1] && data[i]<data[i+1])
		{
			npos[nFound]=i;
			++nFound;
		}
	}

	nMaxPointLength=nFound;
	return npos;
}


int getNextSegmentLengthWithSameValue(int * d, int nLength)
{
	int dVal = *d;
	
	for(int i=1; i<nLength; i++)
	{
		++d;
		if(*d != dVal)
			return i;
	}
	
	return nLength;
}

int * blipfilter(int * d, int nLength)
{
	int * pRes = new int[nLength];
	int * pr = pRes;
	for(int i=0; i<nLength; i++)
		pRes[i] = d[i];


	int nVal = *d;
	int nLengthLeft = nLength;
	int nFormerLen = getNextSegmentLengthWithSameValue(d, nLengthLeft);

	while(nLengthLeft>nFormerLen)
	{
		nLengthLeft -= nFormerLen;
		int * pMiddleData = d + nFormerLen;
		int nMiddleLen = getNextSegmentLengthWithSameValue(pMiddleData, nLengthLeft);

		if(*pMiddleData==nVal+1 || *pMiddleData==nVal-1)
		{
			if(nLengthLeft > nMiddleLen)
			{
				int * pLatterData = pMiddleData + nMiddleLen;

				if(*pLatterData==nVal)
				{
					int nLatterLen = getNextSegmentLengthWithSameValue(pLatterData, nLengthLeft-nMiddleLen);

					if(nFormerLen+nLatterLen > nMiddleLen)
					{
						int * previse = pr+nFormerLen;
						for(int i=0; i<nMiddleLen; i++)
							*(previse++)=nVal;

						nLengthLeft += nFormerLen;
						nFormerLen += (nMiddleLen+nLatterLen);
						continue;
					}
				}
			}
		}

		pr += nFormerLen;

		d = pMiddleData;
		nVal = *d;

		nFormerLen = nMiddleLen;
	}

	return pRes;
}


void Cart2Polar(double *x, double *y, int nLength, double * r, double * t)
{
	double xc, yc;
	xc = yc =0.0;
	for(int i=0; i<nLength; i++)
	{
		xc += x[i];
		yc += y[i];
	}

	xc /= nLength;
	yc /= nLength;

	for(int i=0; i<nLength; i++)
	{
		double xnew = *x - xc;
		double ynew = *y - yc;

		*r = sqrt(xnew*xnew + ynew*ynew);
		*t = atan2(ynew, xnew);

		++x;
		++y;
		++r;
		++t;
	}
	return;
}




/*
 * On Critical Point Detection of Digital Shapes
 * Pengfei Zhu and Paul M. Chirlian
 */
INTVECTOR * GetTstylePoint(double *r, double *t, int nBounderyLength, INTVECTOR * cpc)
{
	INTVECTOR * pT = new INTVECTOR;

	INTVECTOR::iterator itr=cpc->begin();
	int nL = cpc->back();

	int nLen = cpc->size();
	double * d = new double[nBounderyLength];

	for(int i=0; i<nLen; i++)
	{
		int nR = *itr;

		double A = r[nL]*sin(t[nL]) - r[nR]*sin(t[nR]);
		double B = r[nR]*cos(t[nR]) - r[nL]*cos(t[nL]);
		double C = r[nL]*r[nR]*sin(t[nR]-t[nL]);
		double D = sqrt(r[nL]*r[nL] + r[nR]*r[nR] - 2.0*r[nL]*r[nR]*cos(t[nL] - t[nR]));

		int nFound=0;
		d[nFound]=0;
		nFound++;

		if(i==0)
		{
			int k;
			for(k=nL+1; k<nBounderyLength; k++)
			{
				d[nFound] = (r[k]*(A*cos(t[k]) + B*sin(t[k])) + C) / D;
				nFound++;
			}

			for(k=0; k<nR; k++)
			{
				d[nFound] = (r[k]*(A*cos(t[k]) + B*sin(t[k])) + C) / D;
				nFound++;
			}

		}
		else
		{
			for(int k=nL+1; k<nR; k++)
			{
				d[nFound] = (r[k]*(A*cos(t[k]) + B*sin(t[k])) + C) / D;
				nFound++;
			}
		}


		d[nFound]=0.0;
		nFound++;

		INTVECTOR * pivTmp = findLocalMinMaxIndex(d, nFound);
		pT->insert(pT->end(), pivTmp->begin(), pivTmp->end());
		delete pivTmp;

		nL = nR;
		++itr;
	}
	
	delete d;
	
	return pT;
}

double LineSegmentLengthInPolar(double rlnL, double tlnL, double rlnR, double tlnR)
{
	double d = rlnL*rlnL + rlnR*rlnR - 2*rlnL*rlnR*cos(tlnL-tlnR);

	return sqrt(d);
}

double DistanceOfPointToStraightLineInPolar(double rpt, double tpt, double rlnL, double tlnL, double rlnR, double tlnR)
{
	double A = rlnL*sin(tlnL) - rlnR*sin(tlnR);
	double B = rlnR*cos(tlnR) - rlnL*cos(tlnL);
	double C = rlnL*rlnR*sin(tlnR-tlnL);
	double D = sqrt(rlnL*rlnL + rlnR*rlnR - 2.0*rlnL*rlnR*cos(tlnL-tlnR));
		
	double h = (rpt*(A*cos(tpt) + B*sin(tpt)) + C) / D;

	return h;
}

double CriticalLevelPerTriPoints(double rpt, double tpt, double rlnL, double tlnL, double rlnR, double tlnR)
{
	double h = DistanceOfPointToStraightLineInPolar(rpt, tpt, rlnL, tlnL, rlnR, tlnR);
	double a = LineSegmentLengthInPolar(rlnL, tlnL, rpt, tpt);
	double b = LineSegmentLengthInPolar(rlnR, tlnR, rpt, tpt);
	
	return (a+b)*h;
}

std::vector<double> * ComputeCriticalLevel(double *r, double *t, INTVECTOR * cpc)
{
	std::vector<double> * CrtLevel = new std::vector<double>;

	int nL = cpc->back();
	int nC = cpc->front();
	int nR = 0;
	
	INTVECTOR::iterator itr=cpc->begin();

	int nLen = cpc->size();
	for(int i=0; i<nLen; i++)
	{
		if(i<nLen-1)
			nR = *(++itr);
		else
			nR = cpc->front();

		double crt = CriticalLevelPerTriPoints(r[nC], t[nC], r[nL], t[nL], r[nR], t[nR]);
		CrtLevel->push_back(crt);

		nL=nC;
		nC=nR;
	}
	
	return CrtLevel;
}

void RemoveLowerCriticalLevel(std::vector<double> * crtlev, INTVECTOR * cpc)
{
	std::vector<double>::iterator minPos = min_element(crtlev->begin(), crtlev->end());
	double minVal = *minPos;

	cpc->erase(cpc->begin()+(minPos-crtlev->begin()));
	crtlev->erase(minPos);

	std::vector<double>::iterator itrLevel = crtlev->begin();
	std::vector<double>::iterator LevelStop = crtlev->end();
	std::vector<int>::iterator itrCpc = cpc->begin();
	while(itrLevel != LevelStop)
	{
		if(*itrLevel == minVal)
		{
			int kLevel = itrLevel - crtlev->begin();
			int kCpc = itrCpc - cpc->begin();

			crtlev->erase(itrLevel);
			cpc->erase(itrCpc);

			itrLevel = crtlev->begin() + kLevel;
			itrCpc = cpc->begin() + kCpc;

			LevelStop = crtlev->end();

			continue;
		}

		++itrLevel;
		++itrCpc;

	}
	return;
}

double * windowFilter(LPSTR szName, int nLength)
{
	double * f = new double[nLength];
	if(NULL==f)
		return NULL;

	int i;
	double fsum=0.0;

	if(szName[0]=='h' || szName[0]=='H')
	{
		double tmp = 2*pi/static_cast<double>(nLength-1);
		for(i=0; i<nLength; i++)
		{
			f[i]=0.54-0.46*cos(tmp*static_cast<double>(i));

			fsum += f[i];
		}
	}

	
	for(i=0; i<nLength; i++)
		f[i] /= fsum;

	return f;
}

double * CircularFilter(double * x, int nXlen, double * h, int nHlen)
{
	double * y = new double[nXlen];
	if(NULL==y)
		return NULL;

	int nLenHalfFilter;

	if(nHlen%2 == 1)
		nLenHalfFilter = (nHlen-1)/2;
	else
		nLenHalfFilter = nHlen/2;

	double * xtmp = new double[2*nLenHalfFilter+nXlen];
	if(NULL==xtmp)
		return NULL;

	int i, j;
	if(nXlen<nLenHalfFilter)
	{
	}
	else
	{
		for(i=0, j=nXlen-nLenHalfFilter; i<nLenHalfFilter; i++, j++)
			xtmp[i]=x[j];

		for(j=0; j<nXlen; i++, j++)
			xtmp[i]=x[j];

		for(j=0; j<nLenHalfFilter; i++, j++)
			xtmp[i]=x[j];
	}

	for(i=0; i<nXlen; i++)
	{
		y[i] = 0.0;
		for(j=0; j<nHlen; j++)
		{
			y[i] += xtmp[i+j]*h[nHlen-1-j];
		}
	}

	delete xtmp;

	return y;
}

double * GaussianFilter(double sigma, int& nLength)
{
	if(nLength<=0)
		nLength = static_cast<int>(floor(sigma*2.5))*2+1;

	double * g = new double[nLength];

	if(NULL==g)
		return NULL;

	double dblBegin= static_cast<double>(1-nLength) / 2.0;
	double dblOuter = sqrt(2*pi)*sigma;
	double dblInner = -2.0*sigma*sigma;
	double gsum=0.0;

	for(int i=0; i<nLength; i++)
	{
		g[i]=exp(dblBegin*dblBegin/dblInner)/dblOuter;
		gsum += g[i];

        dblBegin += 1.0;
	}
	
	for(int i=0; i<nLength; i++)
	{
		g[i] /= gsum;
	}
	
	return g;
}

#define LEFT_TURN       1
#define STRAIGHT_LINE   0
#define RIGHT_TURN     -1
int TurningDirection(double xa, double ya, double xb, double yb, double xc, double yc)
{
	double tmp = xa*(yb-yc) + xb*(yc-ya) + xc*(ya-yb);
	int nType = sign(tmp);

	return nType;
}

double TriangleArea(double xa, double ya, double xb, double yb, double xc, double yc)
{
	double tmp = (xa*(yb-yc) + xb*(yc-ya) + xc*(ya-yb))/2.0;

	return tmp;
}

int TurningDirectionInPolar(double ra, double ta, double rb, double tb, double rc, double tc)
{
	double tmp = ra*rb*sin(tb-ta) + rb*rc*sin(tc-tb) + ra*rc*sin(ta-tc);
	int nType = sign(tmp);

	return nType;
}

INTVECTOR * FindConcavePoints(double * x, double * y, int nLength, int L)
{
	INTVECTOR * pRet = new INTVECTOR;

    BOOL    bFound;
	for(int i=0; i<nLength; i++)
	{
        bFound = TRUE;
		for(int j=1; j<=L; j++)
		{
			int iA=i-j;
			if(iA<0)
				iA += nLength;

			int iC=i+j;
			if(iC>=nLength)
				iC -= nLength;
			
			int nDir=TurningDirection(x[iC], y[iC], x[i], y[i], x[iA], y[iA]);
            if(nDir != -1)
            {
                bFound = FALSE;
                break;
            }
		}

		if(TRUE == bFound)
			pRet->push_back(i);
	}


	return pRet;
}

double LocalHeightProperty(double * r, double * t, int nLength, int iPos, int L)
{
	double hSum=0.0;

	for(int n=1; n<L; n++)
	{
		int iL=iPos-n;
		if(iL<0)
			iL+=nLength;

		int iR=iPos+n;
		if(iR>=nLength)
			iR-=nLength;

		double h = distanceFromPointToLineInPolar(r[iL], t[iL], r[iPos], t[iPos], r[iR], t[iR]);
		hSum += fabs(h);
	}

	return hSum;
}

pcurvattr CurveAttribute(double *r, double *t, int nLength, int iL, int iR)
{
	int nPointNumber;
	if(iL>iR)
		nPointNumber = nLength-iL-1+iR;
	else
		nPointNumber = iR - iL - 1;

	if(nPointNumber==0)
		return NULL;

	int * nDir = new int[nPointNumber]; 
	double * h = new double[nPointNumber];

	double hSum=0.0;
	for(int i=iL+1, k=0; k<nPointNumber; i++, k++)
	{
		if(i>=nLength)
			i-=nLength;

		double tmp = distanceFromPointToLineInPolar(r[iL], t[iL], r[i], t[i], r[iR], t[iR]);
		h[k] = fabs(tmp);

		nDir[k] = TurningDirectionInPolar(r[iR], t[iR], r[i], t[i], r[iL], t[iL]);

		hSum += h[k];
	}

	pcurvattr pAttr = new curvattr;
	pAttr->nFrom = iL;
	pAttr->nTo = iR;
	pAttr->sumHeight = hSum;
	pAttr->isStraightLine = false;
	pAttr->nLength = nPointNumber;
	pAttr->maxHeight = maxValueWithIndex(h, nPointNumber, &(pAttr->maxPos));
	
	bool bStraightLine = (nDir[0]==0);
	int nType=nDir[0];
	for(int k=1; k<nPointNumber; k++)
	{
		bStraightLine &= (nDir[k]==0);

		if(nType!=nDir[k])
		{
			if(nType!=0)
			{
				nType = 0;
				break;
			}
			nType=nDir[k];
		}
	}

	pAttr->type = nType;
	pAttr->isStraightLine = bStraightLine;

	delete h;
	delete nDir;

	return pAttr;
}

double maxHeightInSegment(double *r, double *t, int nLength, int iFrom, int iEnd, int iL, int iR)
{
	int nPointNumber = iR-iL+1;
	if(nPointNumber<=0)
	{
		double hL = maxHeightInSegment(r, t, nLength, iFrom, iEnd, 0, iR);
		double hR = maxHeightInSegment(r, t, nLength, iFrom, iEnd, iL, nLength-1);

		return max(hL, hR);
	}

	double tmp;
	double * h = new double[nPointNumber];

	for(int i=iL; i<iR; i++)
	{
		tmp = distanceFromPointToLineInPolar(r[iFrom], t[iFrom], r[i], t[i], r[iEnd], t[iEnd]);
		h[i-iL] = fabs(tmp);
	}

	tmp = maxValue(h, nPointNumber);
	delete h;

	return tmp;

}

int maxIndexHeightInSegment(double *r, double *t, int nLength, int iFrom, int iEnd, int iL, int iR)
{
	int nPointNumber = iR-iL+1;
	if(nPointNumber<=0)
	{
		double hL = maxHeightInSegment(r, t, nLength, iFrom, iEnd, 0, iR);
		double hR = maxHeightInSegment(r, t, nLength, iFrom, iEnd, iL, nLength-1);

		return max(hL, hR);
	}

	double * h = new double[nPointNumber];
	for(int i=iL; i<iR; i++)
	{
		double tmp = distanceFromPointToLineInPolar(r[iFrom], t[iFrom], r[i], t[i], r[iEnd], t[iEnd]);
		h[i-iL] = fabs(tmp);
	}

	int k = maxValueIndex(h, nPointNumber);
	return k;

}

INTVECTOR * findCharacterPointPerSegment(double * r, double * t, int nLength, INTVECTOR * pCp)
{
	INTVECTOR::iterator itr, itrNext;
	itr = pCp->begin();

	int nSelSize = pCp->size();
	INTVECTOR * pCrtpt = new INTVECTOR;
	
	for(int i=0; i<nSelSize; i++)
	{
		if(i<nSelSize-1)
			itrNext=itr+1;
		else
			itrNext=pCp->begin();

		int k = maxIndexHeightInSegment(r, t, nLength, *itr, *itrNext, *itr, *itrNext);
		pCrtpt->push_back(*itr + k);
		
		itr=itrNext;
	}

	return pCrtpt;
}

void deleteTooShallowLobePoints(double * r, double * t, int nLength, int nLocalSize, INTVECTOR * pCp, double thrsh)
{
	INTVECTOR::iterator itr, itrNext;
	itr     = pCp->begin();

	int nSelSize = pCp->size();
	double * h= new double[nSelSize];

	for(int i=0; i<nSelSize; i++)
	{
		if(i<nSelSize-1)
			itrNext=itr+1;
		else
		{
			itrNext=pCp->begin();
		}

		h[i] = maxHeightInSegment(r, t, nLength, *itr, *itrNext, *itr, *itrNext);
		
		itr=itrNext;
	}

	itr=pCp->begin();
	for(int i=0; i<nSelSize; i++)
	{
		if(h[i]<=thrsh)
		{
			double heightCur, heightNext;
			heightCur= LocalHeightProperty(r, t, nLength, *itr, nLocalSize);
			
			if(itr == pCp->end()-1)
				itrNext = pCp->begin();
			else
				itrNext = itr+1;

			heightNext = LocalHeightProperty(r, t, nLength, pCp->back(), nLocalSize);

			if(heightCur>heightNext)
			{
				pCp->erase(itrNext);
			}
			else
			{
				itr = pCp->erase(itr);
			}
		}
		else
		{
			++itr;
		}
	}
/*	
	fstream g("height.txt", ios_base::out);
	for(i=0; i<nSelSize; i++)
	{
		char szT[16];
		sprintf(szT, "%2.4f", h[i]);
		
		g<<i<<",   "<<szT<<"\n";
	}
	g.flush();
	g.close();
*/
	delete h;
	


	return;
}

void deleteTooNearPoints(double * r, double * t, int nLength, INTVECTOR * pCp, int M)
{
	INTVECTOR::iterator itr, itrNext, itrLast;
	itr     = pCp->begin();
	itrLast = pCp->end()-1;

//	saveTmp("temp_tooclose.txt", pCp);
	int nDiff, nAveraged;

	while(itr!=itrLast)
	{
		itrNext = itr+1;
		int nDiff = *itrNext - *itr;

		//too close
		if( nDiff <= 3)
		{
			nAveraged = *itr + int(nDiff/2);

			itr = pCp->erase(itr);
			itr = pCp->erase(itr);
			itr = pCp->insert(itr, nAveraged);
		}
		else if(nDiff <= M)        //compute the distance
		{
			double heightCur  = LocalHeightProperty(r, t, nLength, *itr, nDiff/2);
			double heightNext = LocalHeightProperty(r, t, nLength, *itrNext, nDiff/2);

			if(heightCur>heightNext)
			{
				pCp->erase(itrNext);
			}
			else
			{
				itr = pCp->erase(itr);
			}
		}
		else
		{
			++itr;
		}

		itrLast = pCp->end()-1;
	}

	
	itr = pCp->end()-1;
	itrNext = pCp->begin();
	nDiff = nLength - *itrNext + *itr;
	if( nDiff <= 3)
	{
		nAveraged = *itrNext + int(nDiff/2);
		if(nAveraged >= nLength)
		{
			nAveraged -= nLength;
			
			pCp->erase(itr);
			itr = pCp->erase(itrNext);
			pCp->insert(itr, nAveraged);
		}
		else
		{
			itr = pCp->erase(itr);
			pCp->insert(itr, nAveraged);
			pCp->erase(itrNext);
		}
	}
	else if(nDiff <= M)        //compute the distance
	{
		double heightCur  = LocalHeightProperty(r, t, nLength, *itr,     nDiff/2);
		double heightNext = LocalHeightProperty(r, t, nLength, *itrNext, nDiff/2);

		if(heightCur>heightNext)
		{
			pCp->erase(itrNext);
		}
		else
		{
			pCp->erase(itr);
		}
	}
	return;	

}

std::vector<POINT> *  AdvancedProcessing(double *rg, double *tg, int nLength, INTVECTOR* pCp)
{
	int nSegNumber = pCp->size();
	if(nSegNumber>=1)
    {
    	std::vector<POINT> * pRet = new std::vector<POINT>;

        POINT  pt;
        for(int i=0; i<nSegNumber; i++)
        {
            pt.x = pCp->operator[](i);
            pRet->push_back(pt);
        }
		return pRet;
    }
    return NULL;


	pcurvattr * ppCurvAt = new pcurvattr[nSegNumber];
	if(NULL==ppCurvAt)
		return NULL;

	INTVECTOR::iterator itr = pCp->begin();
	for(int i=0; i<nSegNumber-1; i++)
	{
		ppCurvAt[i] = CurveAttribute(rg, tg, nLength, *itr, *(itr+1));

		++itr;
	}
	ppCurvAt[nSegNumber-1]=CurveAttribute(rg, tg, nLength, pCp->back(), pCp->front());

	std::vector<POINT> * pRet = new std::vector<POINT>;
	POINT pt;
	for(int i=0; i<nSegNumber; i++)
	{
		if(ppCurvAt[i])
		{
			pt.x = ppCurvAt[i]->nFrom;
			pt.y = ppCurvAt[i]->nTo;

			pRet->push_back(pt);			
		}
	}

/*
	fstream g("result.txt", ios_base::out);
	for(i=0; i<nSegNumber; i++)
	{
		if(ppCurvAt[i]==NULL)
			continue;

		char szT[32];
		
		g<<"Segment #:"<<i<<"\r\n";
		g<<"From "<<ppCurvAt[i]->nFrom<<"\r\n";
		g<<"End "<<ppCurvAt[i]->nTo<<"\r\n";
		g<<"Is Straight Line?"<<((ppCurvAt[i]->isStraightLine)? "yes" : "no")<<"\r\n";

		switch( ppCurvAt[i]->type )
		{
			case 1:
				 lstrcpy(szT, "1: convex");
				 break;

			case 0:
				 lstrcpy(szT, "0: both");
				 break;

			case -1:
				 lstrcpy(szT, "-1: concave");
				 break;
		}
		g<<"Type "<<szT<<"\r\n";

		g<<"Length "<<ppCurvAt[i]->nLength<<"\r\n";

		sprintf(szT, "%3.4f", ppCurvAt[i]->maxHeight);
		g<<"Max Height "<< szT <<"\r\n";

		sprintf(szT, "%3.4f", ppCurvAt[i]->sumHeight);
		g<<"Sum Height "<< szT <<"\r\n";

		double ttt = ppCurvAt[i]->sumHeight/ppCurvAt[i]->nLength;
		sprintf(szT, "%3.4f", ttt);
		g<<"Avg  "<< szT <<"\r\n";
		

		g<<"\r\n";
	}
	g.flush();
	g.close();
*/

	for(int i=0; i<nSegNumber; i++)
	{
		if(ppCurvAt[i])
			delete ppCurvAt[i];
	}

	delete ppCurvAt;


	return pRet;
}

void saveTmp(char * fn, INTVECTOR * pCp)
{
	fstream g(fn, ios_base::out);
	for(unsigned int i=0; i<pCp->size(); i++)
		g<<i<<",   "<<pCp->operator[](i)<<"\n";
	g.flush();
	g.close();
}

void saveTmp(char * fn, std::vector<POINT> * pCp)
{
    fstream g(fn, ios_base::out);

    int i = 1;
    std::vector<POINT>::iterator    iter=pCp->begin();
    std::vector<POINT>::iterator    rend=pCp->end();
	while(iter != rend)
    {
		g<<i<<",   "<<iter->x <<"   "<<iter->y<<"\n";
        ++i;
        ++iter;
    }
	g.flush();
	g.close();
}

std::vector<POINT> * findConsecutiveRegion(INTVECTOR * pPos, int nLength)
{
    if(NULL == pPos)
        return NULL;

    if(pPos->empty())
        return NULL;

    INTVECTOR::iterator     iter, IterEnd;
    iter = pPos->begin();
    IterEnd = pPos->end();
    
    POINT   pt;
    pt.x = pt.y = *iter;
    std::vector<POINT> *    pRet = new std::vector<POINT>;

    while((++iter) != IterEnd)
    {
        if(*iter != (pt.y+1))
        {
            pRet->push_back(pt);

            pt.x = pt.y = *iter;
        }
        else
        {
            pt.y = *iter;
        }
    }
    pRet->push_back(pt);

    std::vector<POINT>::iterator    iterpt = pRet->begin();
    if(iterpt->x==0 && pt.y==(nLength-1))
    {
        iterpt->x = pt.x;
        pRet->pop_back();
    }

    return pRet;
}


double MaxHeightInAnInternal(double * x, double * y, int nLength, int iFrom, int iTo, int& iPos)
{
    double hMax = 0.0;
    iPos = iFrom;
	
    for(int n=iFrom+1; n!=iTo; n++)
	{
        if(n >= nLength)
        {
            n -= nLength;
            if(n == iTo)
                break;
        }

		double h = distanceFromPointToLine(x[iFrom], y[iFrom], x[n], y[n], x[iTo], y[iTo]);
        if(h>hMax)
        {
            hMax = h;
            iPos = n;
        }
	}

	return hMax;
}


int ConcaveLevel(double * x, double * y, int nLength, int iPos)
{
	int j; 
	for(j=1; j<=200; j++)
	{
		int iA=iPos-j;
		if(iA<0)
			iA += nLength;

		int iC=iPos+j;
		if(iC>=nLength)
			iC -= nLength;
			
		int nDirIndicator = TurningDirection(x[iC], y[iC], x[iPos], y[iPos], x[iA], y[iA]);
        if(-1 != nDirIndicator)
        {
            break;
        }
 	}
    
    return j;
}


double MaxHeightToNeighboring(double * x, double * y, int nLength, int iPos, int iFrom, int iTo)
{
    double hMax = 0.0;

	for(int n=iFrom; n!=iTo; n++)
	{
        int iL=iPos-n;
		if(iL<0)
            iL+=nLength;

		int iR=iPos+n;
        if(iR>=nLength)
            iR-=nLength;

		double h = distanceFromPointToLine(x[iL], y[iL], x[iPos], y[iPos], x[iR], y[iR]);
        if(h<hMax)
        {
            break;
        }
        else
        {
            hMax = h;
        }

        ++n;
	}

	return hMax;
}



std::vector<POINT> *  TryToKeepTopAndBottom(double * xt, 
                                            double * yt, 
                                            int nLength, 
                                            INTVECTOR * pMnIndex,
                                            int nSegLength
                                          )
{
    std::vector<POINT> *    pRegion = findConsecutiveRegion(pMnIndex, nLength);
    if(NULL==pRegion)
        return NULL;

    std::vector<POINT>::iterator    iter=pRegion->begin();
    std::vector<POINT>::iterator IterEnd=pRegion->end();
    while(iter != IterEnd)
    {
        int nRegionLen = iter->y - iter->x + 1;
        if(nRegionLen < 0)
            nRegionLen += nLength;

        if(nRegionLen > nSegLength)
        {
            ++iter;
        }
        else
        {
            iter = pRegion->erase(iter);
            IterEnd = pRegion->end();
        }
    }

    return pRegion;
}

void SortFromMinToMax(double * e, int nNum, int * iIndex)
{
	int w;
	double t;

	for(int i=0; i<nNum; i++)
		iIndex[i]=i;

	for(int i=0; i<nNum-1; i++)
	{
		for(int j=i+1; j<nNum; j++)
		{
			if(e[i]>e[j])
			{
				t=e[j];
				e[j]=e[i];
				e[i]=t;

				w= iIndex[i];
				iIndex[i]=iIndex[j];
				iIndex[j]=w;
			}
		}
	}
}


BOOL IsSameArray(int a[], int b[], int n)
{
	int bFound;

	for(int i=0; i<n; i++)
	{
		bFound = 0;

		for(int j=0; j<n; j++)
		{
			if(a[i]==b[j])
			{
				bFound = 1;
				break;
			}
		}

		if(!bFound)
			return 0;
	}

	return 1;
}


int fitcurvewithline(double x[], double y[], int nCurveLen, int nLineNum, int iSelected[])
{
	int i, nOldNum;
	int bVertical;
	double e, newe;
	int * iSort=new int[nCurveLen];
	double * error=new double[nCurveLen];
    double slope, intcpt;
	
	LinearRegression linReg;
	while(1)
	{
		linReg.Reset();

		for(i=0; i<nLineNum; i++)
			linReg.addXY(x+iSelected[i], y+iSelected[i], 1);

		linReg.Calculate();
		slope=linReg.getA();
		intcpt=linReg.getB();

		bVertical = linReg.IsVertical();
		if(!bVertical)
		{
			for(i=0; i<nCurveLen; i++)
				error[i] = pow(y[i] - slope*x[i] - intcpt, 2);
		}
		else
		{
			intcpt = linReg.getVertical();
			for(i=0; i<nCurveLen; i++)
				error[i] = pow(x[i] - intcpt, 2);
		}
		
		e = 0.0;
		for(i=0; i<nLineNum; i++)
			e += error[iSelected[i]];


		newe = 0.0;
		SortFromMinToMax(error, nCurveLen, iSort);
		for(i=0; i<nLineNum; i++)
			newe += error[i];
		
		if(newe>e)
			break;


		e /= nLineNum;
		nOldNum = nLineNum;

		i=nLineNum+1;
		newe += error[i];
		while((newe/i <= e) && i<nCurveLen)
		{
			++nLineNum;

			i++;
			newe += error[i];
		}


		if(nOldNum == nLineNum)
		{
			if(IsSameArray(iSort, iSelected, nLineNum))
				break;
		}
		
		for(i=0; i<nLineNum; i++)
			iSelected[i]=iSort[i];
	}

	delete[] iSort;
	delete[] error;

    int iMaxPos = maxValueIndex(iSelected, nLineNum);
	return iMaxPos;
}

void combineRegion(std::vector<POINT> * pSeg, int nWholeLength)
{
    std::vector<POINT>::iterator    iter = pSeg->begin();
    int nFirstLength = iter->y - iter->x + 1;
    std::vector<POINT>::iterator    iterPre = iter;

    ++iter;

    std::vector<POINT>::iterator    iterEnd = pSeg->end();
    while(iter != iterEnd)
    {
        int nNextLength = iter->y - iter->x + 1; 
        int nMidLength = iter->x - iterPre->y -1;
        
        if(nMidLength <= ((nNextLength+nFirstLength)>>2))
        {
            iterPre->y = iter->y;
            iter = pSeg->erase(iter);
            iterEnd = pSeg->end();

            nFirstLength = iterPre->y - iterPre->x + 1;
        }
        else
        {
            iterPre = iter;
            ++iter;

            nFirstLength = nNextLength;
        }
    }

    return;
}

int LinearRegressionToLinize(double *xt, double * yt, int nLength, int nOrgPointPos, int nMaxLength, int nInitSelected, int nDir)
{
    double * xreg = new double[2*nMaxLength];
    double * yreg = xreg + nMaxLength;
    int * iSelected = new int[nMaxLength];

    int nFrom = nOrgPointPos;
    for(int i=0; i<nMaxLength; i++)
    {
        xreg[i] = xt[nFrom];
        yreg[i] = yt[nFrom];

        nFrom += nDir;
        if(nFrom>=nLength)
            nFrom = 0;
        else if(nFrom < 0)
            nFrom = nLength - 1;
    }

    for(int i=0; i<nInitSelected; i++)
        iSelected[i] = i;
    
    int nFurthest = fitcurvewithline(xreg, yreg, nMaxLength, nInitSelected, iSelected);

    int nConfirmed = nOrgPointPos + nFurthest * nDir;
    if(nConfirmed>=nLength)
        nConfirmed -= nLength;
    else if(nConfirmed<0)
        nConfirmed += nLength;

    delete xreg;
    delete iSelected;

    return nConfirmed;

}

int getMidPoint(int nFrom, int nTo, int nWholeLength)
{
    if(nTo < nFrom)
        nTo += nWholeLength;

    int nMidPoint =((nFrom+nTo)>>1);
    if(nMidPoint >= nWholeLength)
        nMidPoint -= nWholeLength;

    return nMidPoint;
}


void AdjustForTwoPoints(double * x, double * y, int nLength, int * pSigPoints, int& nPointNumber)
{
    int iL = pSigPoints[0];
    int iR = pSigPoints[1];

    double dMax=0.0;
	for(int iPos=iL+1; iPos!=iR; iPos++)
	{
		if(iPos >= nLength)
            iPos -= nLength;

		double h = distanceFromPointToLine(x[iL], y[iL], x[iPos], y[iPos], x[iR], y[iR]);
        if(dMax < h)
            dMax = h;
	}

    if(dMax > 5)
        return;

    double dFirst  = MaxHeightToNeighboring(x, y, nLength, iL, 3, 15);
    double dSecond = MaxHeightToNeighboring(x, y, nLength, iR, 3, 15);
    if(dSecond>dFirst)
        pSigPoints[0] = iR;

    nPointNumber = 1;
    return;
}


int predictOtherSideOfPoint(double * x, 
                             double * y, 
                             int nLength,
                             double wx,
                             double wy,
                             double onex,
                             double oney
                            )
{
    double * px = x;
    double * py = y;

    int iIndex = 0;
    int * nPos = new int[nLength];
    double * h = new double[nLength];
    for(int i=0; i<nLength; i++)
    {
        if(wy <= fabs(*(py) - oney))
        {
            nPos[iIndex] = i;
            h[iIndex] = distanceFromPointToLine(wx, wy, *px, *py, onex, oney);
            
            ++iIndex;
        }

        ++px;
        ++py;
    }

    double dMin=h[0];
    int iOtherPointPos = 0;
    for(int i=0; i<iIndex; i++)
    {
        if(dMin>h[i])
        {
            dMin = h[i];
            iOtherPointPos = nPos[i];
        }
    }

    delete h;
    delete nPos;

    return iOtherPointPos;
}

int DecideBtmSignificantPoint(double * xt, double * yt, int nLength, int nSigBtmPos, int * pBtmSigPoints, int nSigPointNumber, int* Found)
{
    Found[0] = Found[1] = Found[2] = -1;

    int nBtmAnglePointC, nBtmAnglePointL, nBtmAnglePointR;
    nBtmAnglePointC = nSigBtmPos;
    nBtmAnglePointL = nBtmAnglePointR = -1;

    if(nSigPointNumber==0)
    {
        nBtmAnglePointC = nSigBtmPos;
    }

    if(1==nSigPointNumber)
    {
        int nLevel;
        double dMaxHeight, dMaxHeightBtm;

        nLevel = ConcaveLevel(xt, yt, nLength, pBtmSigPoints[0]);
        dMaxHeight = MaxHeightToNeighboring(xt, yt, nLength, pBtmSigPoints[0], 1, nLevel);

        int nLevelMaxHeight = ConcaveLevel(xt, yt, nLength, nSigBtmPos);
        dMaxHeightBtm = MaxHeightToNeighboring(xt, yt, nLength, nSigBtmPos, 1, nLevelMaxHeight);

        nBtmAnglePointC = (dMaxHeightBtm>=dMaxHeight || dMaxHeight<=2.0)? nSigBtmPos : pBtmSigPoints[0];
    }

    if(2==nSigPointNumber)
    {
        int nMidPoint;

        int nTmp = pBtmSigPoints[1]-pBtmSigPoints[0];
        if(nTmp>(nLength>>1))
        {
            nTmp = pBtmSigPoints[1];
            pBtmSigPoints[1] = pBtmSigPoints[0];
            pBtmSigPoints[0] = nTmp;
        }

        MaxHeightInAnInternal(xt, yt, nLength, pBtmSigPoints[0], pBtmSigPoints[1], nMidPoint);
        int nTurningDir = TurningDirection(xt[pBtmSigPoints[1]], yt[pBtmSigPoints[1]], 
                                           xt[nMidPoint], yt[nMidPoint], 
                                           xt[pBtmSigPoints[0]], yt[pBtmSigPoints[0]]
                                          );

        nBtmAnglePointC = nMidPoint;
        nBtmAnglePointL = pBtmSigPoints[0];
        nBtmAnglePointR = pBtmSigPoints[1];
    }

    Found[0] = nBtmAnglePointL;
    Found[1] = nBtmAnglePointC;
    Found[2] = nBtmAnglePointR;
    
    return 0;
}

int AdjustBtmRegion(double *x, double *y, int nLength, int nSigBtmPos, std::vector<POINT>* pSeg, int * pPointPos)
{
    int nRegNumber = pSeg->size();
    if(0==nRegNumber)
        return 0;

    std::vector<POINT>::iterator    iter = pSeg->begin();
    if(1 == nRegNumber)
    {
        MaxHeightInAnInternal(x, y, nLength, iter->x, iter->y, pPointPos[0]);
    }

    if(2 == nRegNumber)
    {
        int nTmpL, nTmpR, nTmp, nTmpC;

        MaxHeightInAnInternal(x, y, nLength, iter->x, iter->y, nTmpL);
        
        ++iter;
        MaxHeightInAnInternal(x, y, nLength, iter->x, iter->y, nTmpR);

        nTmp = nTmpR - nTmpL;
        if(nTmp>=(nLength>>1))
        {
            nTmp  = nTmpR;
            nTmpR = nTmpL;
            nTmpL = nTmp;
        }
        MaxHeightInAnInternal(x, y, nLength, nTmpL, nTmpR, nTmpC);

        pPointPos[0] = nTmpC;
        pPointPos[1] = nTmpL;
        pPointPos[2] = nTmpR;

        return 3;
    }

    int * pSegPoint = new int[nRegNumber];
    for(int i=0; i<nRegNumber; i++)
    {
        MaxHeightInAnInternal(x, y, nLength, iter->x, iter->y, pSegPoint[i]);
        ++iter;
    }

    double * dx = new double[nRegNumber];
    int * pNewSegPoint = new int[nRegNumber];
    for(int i=0; i<nRegNumber; i++)
    {
        int nTmp = pSegPoint[i];
        dx[i] = x[nTmp];
        pNewSegPoint[i] = nTmp;
    }

    for(int i=0; i<nRegNumber-1; i++)
    {
        for(int j=i+1; j<nRegNumber; j++)
        {
            if(dx[i] > dx[j])
            {
                double dTmp = dx[i];
                dx[i] = dx[j];
                dx[j] = dTmp;

                int nTmp = pNewSegPoint[i];
                pNewSegPoint[i] = pNewSegPoint[j];
                pNewSegPoint[j] = nTmp;
            }
        }
    }

    for(int i=0; i<nRegNumber-1; i++)
    {
        dx[i] = MaxHeightInAnInternal(x, y, nLength, pNewSegPoint[i], pNewSegPoint[i+1], pSegPoint[i]);
    }

    double dMax = dx[0];
    int nMaxPos = 0;
    for(int i=1; i<nRegNumber-1; i++)
    {
        if(dMax < dx[i])
        {
            dMax = dx[i];
            nMaxPos = i;
        }
    }

    pPointPos[1] = pNewSegPoint[nMaxPos];
    pPointPos[2] = pNewSegPoint[nMaxPos+1];
    pPointPos[0] = pSegPoint[nMaxPos];

    delete pSegPoint;
    delete pNewSegPoint;
    delete dx;

    return 3;
}

/**
 * Finds the point in the given pixel list nearest to the given point (x,y). 
 */
int findPointPosition(CPixelList * pCntr, int x, int y)
{
    int nLength=pCntr->size();
    double * d = new double[nLength];

    CPixelList::iterator    iter=pCntr->begin();
    for(int i=0; i<nLength; i++)
    {
        int dx = x - iter->x;
        int dy = y - iter->y;
        d[i] = sqrt((double)(dx*dx + dy*dy));
        ++iter;
    }

    double dMin=d[0];
    int iPos=0;
    for(int i=1; i<nLength; i++)
    {
        if(dMin>d[i])
        {
            dMin=d[i];
            iPos=i;
        }
    }

    delete[] d;
    return iPos;
}

double myAtan2(double dy, double dx)
{
    double angle;

    if(dx==0)
        angle = pi/2.0*double(sign(dy));
    else
        angle = atan2((double)dy, (double)dx);

    if(angle<0)
        angle += 2.0*pi;

    return angle;
}

int indentationValue(CAdvImage * pImg, double *xt, double *yt, int nLength, int& nFrom, int& nTo)
{
    int imgHeight = pImg->GetHeight();

    int nFromAdd = 1;
    int nToAdd = 1;

    int nArea = 0;
    int nNewArea;
    int nFromPrev = nFrom;
    int nToPrev = nTo;

    int nScheme = 0;
    while(nScheme < 4)
    {
        nNewArea = 0;
        CPixelList * ppix = MakeLine(xt[nFrom], yt[nFrom], xt[nTo], yt[nTo]);
        
        CPixelList::iterator    iter = ppix->begin();
        CPixelList::iterator    iend = ppix->end();
        
        while(iter != iend)
        {
            int y = iter->y;
            BOOL bEverZero = FALSE;
            
            int x = iter->x;
            unsigned char c = pImg->GetPixelAt(x, y, 0);
            
            if(pImg->GetPixelAt(x, y, 0)==0)
            {
                while(y>=0)
                {
                    unsigned char grayLevel = pImg->GetPixelAt(iter->x, y, 0);
                    if(0!=grayLevel)
                    {
                        break;
                    }

                    nNewArea++;
                    y--;
                }

                bEverZero = TRUE;
            }
            else
            {
                if(bEverZero)
                    break;
            }

            ++iter;
        }

        delete ppix;

        if(0 == nNewArea )
        {
            nArea = 0;
            break;
        }

        if(nNewArea>=nArea)
        {
            nFromPrev = nFrom;
            nToPrev = nTo;

            nArea = nNewArea;
        }
        else
        {
            nFrom = nFromPrev;
            nTo = nToPrev;

            if((nScheme % 2) == 0)
            {
                nFromAdd = 0;
                nToAdd = 1;
            }
            else
            {
                nFromAdd = 1;
                nToAdd = 0;
            }
            
            nScheme++;
        }

        nFrom -= nFromAdd;
        if(nFrom < 0)
            nFrom += nLength;

        nTo += nToAdd;
        if(nTo >= nLength)
            nTo -= nLength;
    }

    nFrom = nFromPrev;
    nTo = nToPrev;
    return nArea;
}


void removeShortRegions(std::vector<POINT>* pSeg, double *xt, double *yt, int nLength, int nMinLength)
{
    std::vector<POINT>::iterator    iter = pSeg->begin();
    
    int nSegNumber = pSeg->size();
    for(int i=0; i<nSegNumber; i++)
    {
        int nSegLen = iter->y - iter->x + 1;
        if(nSegLen<nMinLength)
        {
            int iPos;
            double  dHeight = MaxHeightInAnInternal(xt, yt, nLength, iter->x, iter->y, iPos);
            if(dHeight < 0.01)
                iter = pSeg->erase(iter);
        }
        else
            ++iter;
    }

    return;
}

void recoverpoint(INTVECTOR * pMnIndex, std::vector<POINT>* pSeg)
{
    pMnIndex->clear();

    std::vector<POINT>::iterator    iter=pSeg->begin();
    std::vector<POINT>::iterator    iend=pSeg->end();
    while(iter != iend)
    {
        for(int i=iter->x; i<=iter->y; i++)
        {
            pMnIndex->push_back(i);
        }

        ++iter;
    }

    return;
}


double getMaxConcaveRegionLength(double *x, double * y, int nLength, int nPos, int nLocalSetting, int& iL, int& iR)
{
    int iA, iC, nDir, j;

    j = nLocalSetting;
    while(j<nLength)
    {
        iA = nPos-j;
		if(iA<0)
			iA += nLength;

		iC = nPos+j;
		if(iC>=nLength)
			iC -= nLength;
			
		nDir=TurningDirection(x[iC], y[iC], x[nPos], y[nPos], x[iA], y[iA]);
        if(nDir != -1)
        {
            break;
        }
        j++;
    }

    iL = iA + 1;
    if(iL >= nLength)
        iL -= nLength;

    iR = iC - 1;
    if(iR < 0)
        iR += nLength;

	nDir=-1;
    while(nDir == -1)
    {
        --iL;
        if(iL<0)
            iL += nLength;

	    nDir=TurningDirection(x[iR], y[iR], x[nPos], y[nPos], x[iL], y[iL]);
    }

    ++iL;
    if(iL>=nLength)
        iL = 0;

    nDir = -1;
    while(nDir == -1)
    {
        ++iR;
        if(iR>=nLength)
            iR = 0;

	    nDir=TurningDirection(x[iR], y[iR], x[nPos], y[nPos], x[iL], y[iL]);
    }
   
    --iR;
    if(iR<0)
        iR += nLength;

    double dArea = TriangleArea(x[iR], y[iR], x[nPos], y[nPos], x[iL], y[iL]);
    if(dArea < 0)
        dArea = -dArea;
    
    return dArea;
}

double decideLargestArea(double *x, double *y, int nLength, int iL, int iR, int& iPos)
{
    double dMax = 0.0;
    for(int i=iL+1; i!=iR; i++)
    {
        if(i >= nLength)
            i=0;

        double dArea = TriangleArea(x[iR], y[iR], x[i], y[i], x[iL], y[iL]);
        if(dArea>dMax)
        {
            dMax = dArea;
            iPos = i;
        }
    }

    return dMax;
}

double decideConcaveRegion(double *x, double *y, int nLength, int nFrom, int nTo, int nLocalScaleSetting, int& iL, int& iR, int& iPos)
{
    int iLtmp, iRtmp;
    double dMax;

    iPos = nFrom;
    dMax = getMaxConcaveRegionLength(x, y, nLength, nFrom, nLocalScaleSetting, iL, iR);
    for(int i=nFrom+1; i!=nTo; i++)
    {
        if(i>=nLength)
            i=0;

        double  dNext = getMaxConcaveRegionLength(x, y, nLength, i, nLocalScaleSetting, iLtmp, iRtmp);
        if(dNext>dMax)
        {
            dMax = dNext;
            iPos = i;
            iL = iLtmp;
            iR = iRtmp;
        }
    }

    return dMax;
}

int decideRegionRelationship(double *xt, double *yt, int nLength, int iPos, int iPosNext, int nLocalRegion)
{
    if((iPosNext-iPos)<2*nLocalRegion)
        return 1;

    int iPosNULL;
    double  dHeight = MaxHeightInAnInternal(xt, yt, nLength, iPos, iPosNext, iPosNULL);
    if(dHeight<2.0)
        return 1;

    return 0;
}

void removeNarrowRegions(std::vector<POINT>* pSeg, double *xt, double * yt, int nLength, int nLocalRegion)
{
    std::vector<POINT>::iterator    iter = pSeg->begin();
    
    int nSegNumber = pSeg->size();
    for(int i=0; i<nSegNumber; i++)
    {
        if(1)
            iter = pSeg->erase(iter);
        else
            ++iter;
    }

    return;
}

void removeShallowRegions(std::vector<POINT>* pSeg, double *xt, double *yt, int nLength, double dMin)
{
    std::vector<POINT>::iterator    iter = pSeg->begin();
    
    int nSegNumber = pSeg->size();
    for(int i=0; i<nSegNumber; i++)
    {
        int     iPos;
        double  dHeight = MaxHeightInAnInternal(xt, yt, nLength, iter->x, iter->y, iPos);
        if(dHeight<dMin)
            iter = pSeg->erase(iter);
        else
            ++iter;
    }

    return;
}

void findIndentationRegion(CAdvImage * pImg, double *x, double *y, int nLength, std::vector<POINT> * pRegion)
{
    std::vector<POINT>::iterator    ptIter = pRegion->begin();
    std::vector<POINT>::iterator    ptIend = pRegion->end();
    while(ptIter != ptIend)
    {
        int iFrom, iEnd;

        iFrom = ptIter->x;
        iEnd = ptIter->y;
        
        indentation(pImg, x, y, nLength, iFrom, iEnd);

        ptIter->x = iFrom;
        ptIter->y = iEnd;

        ++ptIter;
    }

    return;
}

void RemoveSmallerConcaveRegionByLength(std::vector<POINT> * pRegion, int nLength, int nThreshold)
{
    std::vector<POINT>::iterator    ptIter = pRegion->begin();
    std::vector<POINT>::iterator    ptIend = pRegion->end();
    while(ptIter != ptIend)
    {
        int iSpan = ptIter->y - ptIter->x + 1;
        if(iSpan < 0)
            iSpan += nLength;

        if(iSpan <= nThreshold)
        {
            ptIter = pRegion->erase(ptIter);
            ptIend = pRegion->end();
        }
        else
        {
            ++ptIter;
        }
    }

    return;
}

void combineIndentationRegion(CAdvImage * pImg, double *x, double *y, int nLength, std::vector<POINT> * pRegion, int nTipMinArea)
{
    std::vector<POINT>::iterator    ptIter = pRegion->begin();
    std::vector<POINT>::iterator    ptIend = pRegion->end();
    
    std::vector<POINT>::iterator    ptIterPrev = ptIter;
    ++ptIter;

    while(ptIter != ptIend)
    {
        //int n


        ++ptIter;
    }

    return;
}

INTVECTOR *  FindLobeNumber(CAdvImage * pImg, CPixelList * pCntur, double dthreshold)
{
    int nLength = pCntur->size();
    CPixelList::iterator myIter = pCntur->begin();
    
    double * x = new double[nLength*2];
    double * y = x + nLength;
    for(int i=0; i<nLength; i++)
    {
        x[i] = double(myIter->x);
        y[i] = double(myIter->y);

        ++myIter;
    }

	int nHammingLength=21;
	double * hamFilter = windowFilter("hamming", nHammingLength);
	double * xt = CircularFilter(x, nLength, hamFilter, nHammingLength);
	double * yt = CircularFilter(y, nLength, hamFilter, nHammingLength);
	delete [] hamFilter;    

    INTVECTOR  *    pResult = new INTVECTOR;

	int nLocalRegion=6;
	INTVECTOR * pMnIndex = FindConcavePoints(xt, yt, nLength, nLocalRegion);
	if(pMnIndex->size()>=1)
    {
        std::vector<POINT> * pRegion = findConsecutiveRegion(pMnIndex, nLength);
        
        RemoveSmallerConcaveRegionByLength(pRegion, nLength, 6);
        
        findIndentationRegion(pImg, x, y, nLength, pRegion);

//        combineIndentationRegion(pImg, x, y, nLength, pRegion);
        


        std::vector<POINT>::iterator    ptIter = pRegion->begin();
        std::vector<POINT>::iterator    ptIend = pRegion->end();
        while(ptIter != ptIend)
        {
            int nFrom = ptIter->x;
            int nTo = ptIter->y;
            while(nFrom != nTo)
            {
                pResult->push_back(nFrom);

                ++nFrom;
                if(nFrom >= nLength)
                    nFrom -= nLength;
            }
/*
            int iPos;
            MaxHeightInAnInternal(xt, yt, nLength, ptIter->x, ptIter->y, iPos);
            pResult->push_back(iPos);
*/
            ++ptIter;
        }


        delete pMnIndex;
        delete pRegion;
    }

    delete [] x;
    delete [] xt;
    delete [] yt;

    return pResult;
}

double distanceFromPointToLineInPolar(double rL, double tL, double rC, double tC, double rR, double tR)
{
	if(sin(tL-tR)<0)
	{
		double tmp=rL;
		rL = rR;
		rR = tmp;

		tmp = tL;
		tL = tR;
		tR = tmp;
	}

	double area = rL*rC*sin(tL-tC) + rC*rR*sin(tC-tR) - rL*rR*sin(tL-tR);
	double d = sqrt(rL*rL + rR*rR - 2.0*rL*rR*cos(tL-tR));
	
	return area/d;
}

int * Zhu_Chirlian_CriticalPointEstimation(int * x, int * y, int nLength, int& nFoundedNum)
{
	double * r = new double[nLength];
	double * t = new double[nLength];
//	Cart2Polar(x, y, nLength, r, t);

	INTVECTOR * cpc = findLocalMinMaxIndex(r, nLength);
/*
	INTVECTOR * cpcByAngle = findLocalMinMaxIndex(t, nLength);
	if(! cpcByAngle->empty())
	{
		cpc->insert(cpc->end(), cpcByAngle->begin(), cpcByAngle->end());
	}
	delete cpcByAngle;

	INTVECTOR * cpcT = GetTstylePoint(r, t, nLength, cpc);
	if(! cpcT->empty())
	{
		cpc->insert(cpc->end(), cpcT->begin(), cpcT->end()-1);
	}
	delete cpcT;

	if(cpc->empty())
	{
		delete cpc;
		return NULL;
	}
*/
	sort(cpc);
/*
	std::vector<double> * pCL = ComputeCriticalLevel(r, t, cpc);
	
	for(int k=1; k<100; k++)
	{
		RemoveLowerCriticalLevel(pCL, cpc);
		delete pCL;
		pCL = ComputeCriticalLevel(r, t, cpc);
	}
*/
	int nLeft = cpc->size();
	int * pResult = new int[nLeft];
	INTVECTOR::iterator itr = cpc->begin();
	for(int i=0; i<nLeft; i++)
	{
		pResult[i] = *itr;
		itr++;
	}

	nFoundedNum = nLeft;

	delete r;
	delete t;

	return pResult;

}

COLORREF GetColor(int iIndex, int nTotal)
{
	int N;
	int nLevel[4];

	if(iIndex>=nTotal)
		iIndex=iIndex%nTotal;

	if(nTotal<8)
	{
		N=2;
		nLevel[0]=0;
		nLevel[1]=255;
	}
	else if(nTotal<27)
	{
		N=3;
		nLevel[0]=191;
		nLevel[1]=127;
		nLevel[2]=255;
	}
	else if(nTotal<64) 
	{
		N=4;
		nLevel[0]=0;
		nLevel[1]=127;
		nLevel[2]=191;
		nLevel[2]=255;
	}
	else
	{
		return RGB(0, 0, 0);
	}

	int nR=nLevel[iIndex/(N*N)];
	int nG=nLevel[N-1-((iIndex/N)%N)];
	int nB=nLevel[iIndex%N];
	
	return RGB(nR, nG, nB);
}


double * getTestData(char * fname, int& nLength)
{
	int x;
	INTVECTOR v;

	fstream f(fname, ios_base::in);
	while(!f.eof())
	{
		f>>x;
		v.push_back(x);
	}
	f.close();

	int n=v.size();
	double * ret=new double[n];
	for(int i=0; i<n; i++)
		ret[i]=double(v[i]);

	nLength = n;
	return ret;
}

CPixelList* convert(std::vector<POINT>* p)
{
	CPixelList * ret = new CPixelList;
	std::vector<POINT>::iterator iter = p->begin();
	std::vector<POINT>::iterator iter_end = p->end();

	CPixel pxl;
	while(iter!=iter_end)
	{
		pxl.Set(iter->x, iter->y);
		ret->push_back(pxl);

		++iter;
	}

	return ret;
}

std::vector<IPTPIXEL>* convert(CPixelList * p)
{
	std::vector<IPTPIXEL>* ret = new std::vector<IPTPIXEL>;

	CPixelList::iterator iter = p->begin();
	CPixelList::iterator iter_end = p->end();

	IPTPIXEL pxl;
	while(iter!=iter_end)
	{
		pxl.iCol = iter->GetX();
		pxl.iRow = iter->GetY();
		ret->push_back(pxl);

		++iter;
	}

	return ret;

}

//b to ac
double distanceFromPointToLine(double xa, double ya, double xb, double yb, double xc, double yc)
{
	double tmp = xa*(yb-yc) + xb*(yc-ya) + xc*(ya-yb);
    double dx = xc - xa;
    double dy = yc - ya;
    double dst = fabs(tmp)/sqrt(dx*dx + dy*dy);

	return dst;
}

RECT GetBoundaryBoxAfterRotation(RECT &src, double angle)
{
    RECT    result;
    double  cosa = cos(angle);
    double  sina = sin(angle);

    double xLT = double(src.left)  * cosa;
    double yLT = double(src.top)   * sina;
    double xRB = double(src.right) * cosa;
    double yRB = double(src.bottom)* sina;

    int t[4];
    t[0] = round(xLT - yLT);
    t[1] = round(xLT - yRB);
    t[2] = round(xRB - yRB);
    t[3] = round(xRB - yLT);
    
    result.left  = t[0];
    result.right = t[0];
    for(int i=1; i<=3; i++)
    {
        if(result.left > t[i])
            result.left = t[i];
        else if(result.right < t[i])
            result.right = t[i];
    }


    xLT = double(src.left)  * sina;
    yLT = double(src.top)   * cosa;
    xRB = double(src.right) * sina;
    yRB = double(src.bottom)* cosa;
    t[0] = round(xLT + yLT);
    t[1] = round(xLT + yRB);
    t[2] = round(xRB + yRB);
    t[3] = round(xRB + yLT);

    result.top = t[0];
    result.bottom = t[0];
    for(int i=1; i<=3; i++)
    {
        if(result.top < t[i])
            result.top = t[i];
        else if(result.bottom > t[i])
            result.bottom = t[i];
    }

    return result;
}


CPixelList * MakeLine(double fx1, double fy1, double fx2, double fy2)
{
	int steps;
	double px, py, dx, dy;
	CPixel pointOnLine;
	CPixelList * outList = new CPixelList;

	px = fx1;
	py = fy1;
	
	pointOnLine.x = round(fx1);
	pointOnLine.y = round(fy1);
	outList->push_back(pointOnLine);

	dx = fx2 - fx1;
	dy = fy2 - fy1;

	fy1 = fabs(dy);
	fx1 = fabs(dx);
	if(fy1 > fx1)
	{
		steps = round(fy1);
	}
	else
	{
		steps = round(fx1);
	}

	dx /= (double)steps;
	dy /= (double)steps;

	for(int i = 0; i < (steps); i++)
	{
		px += dx;
		py += dy;
		pointOnLine.x = round(px);
        pointOnLine.y = round(py);

		outList->push_back(pointOnLine);
				
	}

    int xTmp = round(fx2);
    int yTmp = round(fy2);
	if(pointOnLine.x!=xTmp || pointOnLine.y!=yTmp)
    {
        pointOnLine.x = xTmp;
        pointOnLine.y = yTmp;
		outList->push_back(pointOnLine);
    }

	return outList;
}


int indentation(CAdvImage * pImg, double *x, double *y, int nLength, int& nFrom, int& nTo)
{
    int imgHeight = pImg->GetHeight();

    int nFromAdd = 1;
    int nToAdd = 1;

    int nArea = 0;
    int nNewArea;
    int nFromPrev = nFrom;
    int nToPrev = nTo;

    int nScheme = 0;
    while(nScheme < 4)
    {
        unsigned char cgray = 0;
        nNewArea = 0;

        CPixelList * ppix = MakeLine(x[nFrom], y[nFrom], x[nTo], y[nTo]);
        
        CPixelList::iterator    iter = ppix->begin();
        CPixelList::iterator    iend = ppix->end();
        
        while(iter != iend)
        {
            cgray = pImg->GetPixelAt(iter->x, iter->y, 0);
            if(cgray==0)
            {
                break;
            }
            else
            {
                ++iter;
            }
        }

        BOOL bExterior = FALSE;
        if(iter != iend)
        {
            while(iter != iend)
            {
                cgray = pImg->GetPixelAt(iter->x, iter->y, 0);
                if(cgray==0)
                {
                    ++iter;
                }
                else
                {
                    break;
                }
            }
        
            if(iter != iend)
            {
                while(iter != iend)
                {
                    cgray = pImg->GetPixelAt(iter->x, iter->y, 0);
                    if(cgray!=0)
                    {
                        ++iter;
                    }
                    else
                    {
                        break;
                    }
                }

                if(iter == iend)
                    bExterior = TRUE;
            }
        }

        delete ppix;

        if(bExterior)
        {
            nFromPrev = nFrom;
            nToPrev = nTo;

            nArea = nNewArea;
        }
        else
        {
            nFrom = nFromPrev;
            nTo = nToPrev;

            if((nScheme % 2) == 0)
            {
                nFromAdd = 0;
                nToAdd = 1;
            }
            else
            {
                nFromAdd = 1;
                nToAdd = 0;
            }
            
            nScheme++;
        }

        nFrom -= nFromAdd;
        if(nFrom < 0)
            nFrom += nLength;

        nTo += nToAdd;
        if(nTo >= nLength)
            nTo -= nLength;
    }

    nFrom = nFromPrev;
    nTo = nToPrev;
    return nArea;
}

/**
 * Returns true if the given pixel lies on or very close to the line with the given slope and y-intercept. 
 * It may return true for multiple adjacent points near a line, even if one is actually closer. 
 */
bool isPointOnLine(CPixel pt, double slope, double intercept) {
	if (slope == 0) {
		return (intercept - 1 <= pt.y && pt.y <= intercept + 1); 
	} else if (fabs(slope) > 1) {
		double yLeft = slope * (pt.x - 1) + intercept, 
			yRight = slope * (pt.x + 1) + intercept; 
		return ((yLeft <= pt.y && pt.y <= yRight) || (yLeft >= pt.y && pt.y >= yRight)); 
	} else {
		double xLower = (pt.y - 1 - intercept) / slope, 
			xUpper = (pt.y + 1 - intercept) / slope; 
		return ((xLower <= pt.x && pt.x <= xUpper) || (xLower >= pt.x && pt.x >= xUpper)); 
	}
}

/** 
 * Finds the leftmost and rightmost boundary point lying on a series of parallel lines. The lines have the given 
 * slope, the first line has y-intercept startIntercept, and the y-intercepts are incremented by incrIntercept
 * until no boundary points lie on the line (i.e., the end of the fruit is reached). Only boundary points 
 * between indices fromIndex and toIndex (inclusive) are considered. Returns a vector where the entry at index i
 * is a size-2 vector that stores the leftmost and rightmost boundary points on the line with y-intercept of
 * startIntercept + i * incrIntercept. 
 */
vector<vector<int> > pointsOnLines(vector<CPixel> boundary, double slope, double startIntercept, double incrIntercept, 
									 int fromIndex, int toIndex) {
	vector<vector<int> > points;  
	for (int i = fromIndex; i != (toIndex + 1) % boundary.size(); i = (i + 1) % boundary.size()) {
		double intercept = boundary[i].y - slope * boundary[i].x; 
		double numIncrs = (intercept - startIntercept) / incrIntercept; 
		int numIncrsArr[2] = {(int)numIncrs, (int)numIncrs + 1};
		for (int j = 0; j < 2; j++) {
			int currNumIncrs = numIncrsArr[j]; 
			if (currNumIncrs >= 0) {
				if (points.size() <= currNumIncrs) {
					points.resize(currNumIncrs + 1) ;
					points[currNumIncrs].resize(2, i); 
				} else if (points[numIncrsArr[j]].empty()) {
					points[currNumIncrs].resize(2, i); 
				} else {
					if (boundary[i].x < boundary[ points[currNumIncrs][0] ].x) {
						points[currNumIncrs][0] = i; 
					}
					if (boundary[i].x > boundary[ points[currNumIncrs][1] ].x) {
						points[currNumIncrs][1] = i; 
					}
				}
			}
		}
	}
	return points; 
}

/**
 * Finds the indices of the leftmost and rightmost boundary points on (or pretty close to) the line with 
 * the given slope and y-intercept. 
 * If multiple boundary points are tied for leftmost or rightmost, returns the pair that are farthest apart. 
 * Searches along the boundary only from fromIndex to toIndex (inclusive). 
 * Returns true if at least one boundary point lies on the line, false otherwise. 
 */
bool pointsOnLine(int& leftmost, int& rightmost, vector<CPixel> boundary, double slope, double intercept, 
				  int fromIndex, int toIndex) 
{
	// Find candidate leftmost and rightmost points					  
	bool foundOnLine = false; 
	vector<int> leftList, rightList; 
	bool firstOfAll = (fromIndex == (toIndex + 1) % boundary.size() && fromIndex != toIndex);
	for (int i = fromIndex; firstOfAll || i != (toIndex + 1) % boundary.size(); i = (i + 1) % boundary.size()) {
		firstOfAll = false; 
		if (isPointOnLine(boundary[i], slope, intercept)) {
			if (! foundOnLine) {
				leftList.push_back(i); 
				rightList.push_back(i); 
				foundOnLine = true; 
			} else {
				if (boundary[i].x <= boundary[leftList[0]].x) {
					if (boundary[i].x < boundary[leftList[0]].x) 
						leftList.clear(); 
					leftList.push_back(i); 
				}
				if (boundary[i].x >= boundary[rightList[0]].x) {
					if (boundary[i].x > boundary[rightList[0]].x) 
						rightList.clear(); 
					rightList.push_back(i); 
				}
			}
		}
	}

	// Find farthest apart candidate leftmost and rightmost points
	if (foundOnLine) {
		leftmost = leftList[0]; 
		rightmost = rightList[0]; 
		float maxDist = boundary[leftmost].Distance(boundary[rightmost]); 
		for (vector<int>::iterator l = leftList.begin(); l != leftList.end(); l++) {
			for (vector<int>::iterator r = rightList.begin(); r != rightList.end(); r++) {
				float dist = boundary[*l].Distance(boundary[*r]); 
				if (dist > maxDist) {
					maxDist = dist; 
					leftmost = *l; 
					rightmost = *r; 
				}
			}
		}
	}

	return foundOnLine; 
}

/**
 * Finds the indices of the top and bottom boundary points on (or pretty close to) the vertical line with 
 * the given x-intercept. 
 * If multiple boundary points are tied for top or bottom, returns the pair that are farthest apart. 
 * Searches along the boundary only from fromIndex to toIndex (inclusive). 
 * Returns true if at least one boundary point lies on the line, false otherwise. 
 */
bool pointsOnLine(int& bottom, int& top, vector<CPixel> boundary, double xIntercept, int fromIndex, int toIndex) 
{
	// Find candidate bottom and top points					  
	bool foundOnLine = false; 
	vector<int> bottomList, topList; 
	bool firstOfAll = (fromIndex == (toIndex + 1) % boundary.size() && fromIndex != toIndex);
	for (int i = fromIndex; firstOfAll || i != (toIndex + 1) % boundary.size(); i = (i + 1) % boundary.size()) {
		firstOfAll = false; 
		if (xIntercept - 1 <= boundary[i].x && boundary[i].x <= xIntercept + 1) {
			if (! foundOnLine) {
				bottomList.push_back(i); 
				topList.push_back(i); 
				foundOnLine = true; 
			} else {
				if (boundary[i].y <= boundary[bottomList[0]].y) {
					if (boundary[i].y < boundary[bottomList[0]].y) 
						bottomList.clear(); 
					bottomList.push_back(i); 
				}
				if (boundary[i].y >= boundary[topList[0]].y) {
					if (boundary[i].y > boundary[topList[0]].y) 
						topList.clear(); 
					topList.push_back(i); 
				}
			}
		}
	}

	// Find farthest apart candidate bottom and top points
	if (foundOnLine) {
		bottom = bottomList[0]; 
		top = topList[0]; 
		float maxDist = boundary[bottom].Distance(boundary[top]); 
		for (vector<int>::iterator l = bottomList.begin(); l != bottomList.end(); l++) {
			for (vector<int>::iterator r = topList.begin(); r != topList.end(); r++) {
				float dist = boundary[*l].Distance(boundary[*r]); 
				if (dist > maxDist) {
					maxDist = dist; 
					bottom = *l; 
					top = *r; 
				}
			}
		}
	}

	return foundOnLine; 
}

/**
 * Finds the proximal/distal point of a fruit (with boundary b, in image img) by looking at the symmetry of
 * the top/bottom part of the fruit. It finds the boundary point along the best line of symmetry (and farthest
 * out on the tip end, if the line passes through more than one boundary point). 
 */
int findTipBySymmetry(CAdvImage * img, CPixelList * b, bool proximal) 
{
	// Copy boundary into vector to allow indexing -- need modular arithmetic to circle around boundary
	vector<CPixel> boundary(b->begin(), b->end()); 

	int ctrX = getMidXHorizontal(boundary, img->GetHeight() / 2);
	CPixel centerOfTarget(ctrX, img->GetHeight() / 2);

	int top = getBoundaryIndexByX(boundary, centerOfTarget.x, true);  // center top of the target

	int farthestXLeft = findFarthestXFromCenter(boundary, centerOfTarget, top, false);
	int farthestXRight = findFarthestXFromCenter(boundary, centerOfTarget, top, true);
	
	bool symmetric = isSymmetric(boundary, centerOfTarget, img->GetWidth(), img->GetHeight());
	int farthestYIndex = findFarthestY(boundary, proximal);

	bool curved = false;
	double slope = findSlopeForPivot(boundary, centerOfTarget, farthestYIndex, farthestXLeft, farthestXRight, proximal, symmetric, &curved); 
	
	//CPixel axisPivot(img->GetWidth() / 2, img->GetHeight() / 2);                                                             // The pivot point is set at the center of frame image originally. 
	CPixel axisPivot = findPivot(boundary, centerOfTarget, slope, farthestXLeft, farthestXRight, proximal, symmetric, curved); // Modified in 2015.

	double startAngle = findStartAngle(slope);

	int bestTip; 
	double minError; 
	bool minErrorSet = false; 

	for (double angle = startAngle; angle <= startAngle + 90; angle++)
	{
		// Avoid special cases (vertical lines) that arise when angle is 0
		if (angle == 0) {
			angle = 0.01; 
		}

		// New x-axis at angle, through axisPivot
		double xAxisSlope = tan(angle * pi / 180); 
		double xAxisIntercept = -xAxisSlope * axisPivot.x + axisPivot.y; 

		// Left-most and right-most (or vice versa) boundary points on new x-axis
		int xAxisBegin, xAxisEnd; 
		bool foundBegin = false; 
		for (int i = 0; i < boundary.size(); i++) {
			if (isPointOnLine(boundary[i], xAxisSlope, xAxisIntercept)) {
				if (! foundBegin) {
					xAxisBegin = xAxisEnd = i; 
					foundBegin = true; 
				} else {
					if (boundary[i].x < boundary[xAxisBegin].x) {
						xAxisBegin = i; 
					}
					if (boundary[i].x > boundary[xAxisEnd].x) {
						xAxisEnd = i; 
					}
				}
			}
		}
		if (! foundBegin) {
			continue;	// Serious problem -- skip this angle
		}
		if (proximal) {
			int tmp = xAxisBegin; 
			xAxisBegin = xAxisEnd; 
			xAxisEnd = tmp; 
		}

		CPixel xAxisMid(axisPivot.x, axisPivot.y);
		// New y-axis, perpendicular to new x-axis, through midpoint between xAxisBegin and xAxisEnd points
		double yAxisSlope = -1 / xAxisSlope; 
		double yAxisIntercept = -yAxisSlope * xAxisMid.x + xAxisMid.y; 

		// Boundary point along new y-axis, on tip end, farthest from midpoint between xAxisBegin and xAxisEnd
		int currTip; 
		float maxDist = 0; 
		bool foundFirstCurrTip = false; 
  		for (int i = xAxisBegin; i != xAxisEnd; i = (i + 1) % boundary.size()) {
			if (isPointOnLine(boundary[i], yAxisSlope, yAxisIntercept)) {
				float dist = boundary[i].Distance(xAxisMid); 
				if (! foundFirstCurrTip || dist > maxDist) {
					currTip = i; 
					maxDist = boundary[currTip].Distance(xAxisMid); 
					foundFirstCurrTip = true; 
				}
			}
		}
		if (! foundFirstCurrTip) {
			continue;	// Serious problem -- skip this angle
		}

		// Sum of error, where error is the distance between the midpoint of 2 opposite boundary points and 
		// the intersection point between the new y-axis and a line through those boundary points. The sum is 
		// taken over a series of parallel lines from the new x-axis to the tip end of the fruit. 
		double currError = 0; 
		int numInSum = 0; 
		vector<vector<int> > lrPointsOnLines = pointsOnLines(boundary, xAxisSlope, xAxisIntercept, 
			(proximal ? -1 : 1), xAxisBegin, xAxisEnd); 
		for (int numInterceptIncrs = 0; numInterceptIncrs < lrPointsOnLines.size(); numInterceptIncrs++) 
		{
			// Get leftmost and rightmost points on the current line
			if (lrPointsOnLines[numInterceptIncrs].size() == 0) 
				continue; 
			double intercept = xAxisIntercept + (proximal ? -1 : 1) * numInterceptIncrs; 
			int leftmost = lrPointsOnLines[numInterceptIncrs].at(0); 
			int rightmost = lrPointsOnLines[numInterceptIncrs].at(1); 
			
			// Midpoint of boundary points
			double midX = (boundary[leftmost].x + boundary[rightmost].x) / 2.0, 
				midY = (boundary[leftmost].y + boundary[rightmost].y) / 2.0; 
			CPixel mid(long(midX + 0.5), long(midY + 0.5)); 	// rounding		

			// Intersection point between new y-axis and line through boundary points
			double intersectX = (intercept - yAxisIntercept) / (yAxisSlope - xAxisSlope), 
				intersectY = yAxisSlope * intersectX + yAxisIntercept; 
			CPixel intersect(long(intersectX + 0.5), long(intersectY + 0.5));	// rounding

			float dist = mid.Distance(intersect); 
			currError += fabs(dist); 
			numInSum++; 
		}
		if (numInSum == 0) {
			continue;	// Serious problem -- skip this angle
		}
		currError /= numInSum; 

		if (! minErrorSet || currError < minError) { 
			minError = currError; 
			bestTip = currTip; 
			minErrorSet = true; 
		}

		if (-1 < angle && angle < 1) {
			angle = 0; 
		}

	}
	//*/
if (minErrorSet) 
	{
		// Normal case
		return bestTip; 
	} 
	else
	{
		// Error case -- somehow no bestTip was chosen. Pick the leftmost boundary point having the max/min y-value. 
		if (proximal) {
			int minIndex = 0; 
			for (int i = 1; i < boundary.size(); i++) {
				if (boundary[i].y < boundary[minIndex].y || 
					(boundary[i].y == boundary[minIndex].y && boundary[i].x < boundary[minIndex].x)) {
						minIndex = i; 
				}
			}
			return minIndex; 
		} else {
			int maxIndex = 0; 
			for (int i = 1; i < boundary.size(); i++) {
				if (boundary[i].y > boundary[maxIndex].y || 
					(boundary[i].y == boundary[maxIndex].y && boundary[i].x < boundary[maxIndex].x)) {
						maxIndex = i; 
				}
			}
			return maxIndex; 
		}
	}
}

/**
* Returns the x-value of the center of a horizontal line at given y-value.
*/
int getMidXHorizontal(vector<CPixel> boundary, int Y)
{
	int leftMostX = boundary[getBoundaryIndexByY(boundary, Y, true)].x;
	int	rightMostX = boundary[getBoundaryIndexByY(boundary, Y, false)].x;
	int centerX = (leftMostX + rightMostX) / 2;
	return centerX;
}

/**
* Finds the point that has either the smallest or largest x-value on the boundary.
*/
int findFarthestXFromCenter(vector<CPixel> boundary, CPixel& centerOfTarget, int top, bool isRight)
{
	int farthestPointIndex = 0;
	int maxDifference = 0;
	if (isRight)
	{
		for (int i = 0; i < top; ++i)
		{
			if (boundary[i].x - centerOfTarget.x > maxDifference)
			{
				farthestPointIndex = i;
				maxDifference = abs(boundary[i].x - centerOfTarget.x);
			}
		}
	}
	else
	{
		for (int i = top; i < boundary.size(); ++i)
		{
			if (centerOfTarget.x - boundary[i].x > maxDifference)
			{
				farthestPointIndex = i;
				maxDifference = abs(boundary[i].x - centerOfTarget.x);
			}
		}
	}

	return farthestPointIndex;
}

/**
* Finds the point that has either the smallest or largest y-value on the boundary.
*/
int findFarthestY(vector<CPixel> boundary, bool proximal)
{
	int farthestY_x = 0;  //x-value of higher shoulder point
	int farthestYIndex = 0;
	int count = 0;
	if (proximal)
	{
		for (int i = 0; i < boundary.size(); ++i)  // find the minimum y-value (y-value of the highest point) in the boundary
		{
			if (boundary[i].y == 0)
			{
				farthestY_x += boundary[i].x;
				count++;
			}
		}

		if (count != 0)
		{
			farthestY_x /= count;
		}
		farthestYIndex = getBoundaryIndexByX(boundary, farthestY_x, true);
	}
	else
	{
		int maxY = 0;
		for (int i = 0; i < boundary.size(); ++i)  // find the maximum y-value (y-value of the lowest point) in the boundary
		{
			if (boundary[i].y > maxY)
			{
				maxY = boundary[i].y;
			}
		}
		for (int j = 0; j < boundary.size(); ++j)
		{
			if (boundary[j].y == maxY)
			{
				farthestY_x += boundary[j].x;
				count++;
			}
		}

		if (count != 0)
		{
			farthestY_x /= count;
		}
		farthestYIndex = getBoundaryIndexByX(boundary, farthestY_x, false);
	}
	return farthestYIndex;
}

/**
* Finds the best slope value, which determines the degree of start angle and the position of axis pivot, based on the shape of the target.
* The shape is divided
* Start angle and axis pivot are two important concepts that are used in findTipBySymmetry() function to look for the best symmetry line.
*/
double findSlopeForPivot(vector<CPixel> boundary, CPixel& centerOfTarget, int farthestYIndex, int farthestXLeftIndex, int farthestXRightIndex, bool proximal, bool symmetric, bool* curved)
{
	double slope = 0;

	int farthestY_x = boundary[farthestYIndex].x;  // x-value of lowest or highest point depending on whether it is calculating proximal point or distal point

	if (proximal)  // proximal point calculation
	{
		double slopeOnLeft, slopeOnRight;
		CPixel p1_left = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y / 3, true)];
		CPixel p2_left = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y / 2, true)];
		slopeOnLeft = slopeBetweenTwoPoints(p1_left.x, p1_left.y, p2_left.x, p2_left.y);

		CPixel p1_right = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y / 3, false)];
		CPixel p2_right = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y / 2, false)];
		slopeOnRight = slopeBetweenTwoPoints(p1_right.x, p1_right.y, p2_right.x, p2_right.y);

		if (symmetric)  // the target is symmetric
		{
			int proximalLeftIndex = getBoundaryIndexByY(boundary, centerOfTarget.y / 4, true);  // leftmost point on the line where y = centerY / 4
			int proximalRightIndex = getBoundaryIndexByY(boundary, centerOfTarget.y / 4, false);  // rightmost point on the line where y = centerY / 4

			int leftIndex = getBoundaryIndexByX(boundary, (3 * boundary[proximalLeftIndex].x + boundary[proximalRightIndex].x) / 4, true);
			int rightIndex = getBoundaryIndexByX(boundary, (3 * boundary[proximalRightIndex].x + boundary[proximalLeftIndex].x) / 4, true);

			if (slopeOnLeft * slopeOnRight > 0)  // the fruit is curved
			{
				*curved = true;
				// to see if the proximal area is rather flat or pointed
				int headDifference = abs((farthestY_x - boundary[proximalLeftIndex].x) - (boundary[proximalRightIndex].x - farthestY_x));
				int tailDifference = abs((farthestY_x - p2_left.x) - (p2_right.x - farthestY_x));

				int higherLineLength = p1_right.x - p1_left.x;
				int centerLineLength = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y, false)].x - boundary[getBoundaryIndexByY(boundary, centerOfTarget.y, true)].x;

				if (headDifference < tailDifference && (double(centerLineLength) / double(higherLineLength) >= 1.3))  // the proximal area is pointed
				{
					int midX = (boundary[proximalLeftIndex].x + boundary[proximalRightIndex].x) / 2;
					int midY = (boundary[proximalLeftIndex].y + boundary[proximalRightIndex].y) / 2;
					slope = -1 / slopeBetweenTwoPoints(boundary[farthestYIndex].x, boundary[farthestYIndex].y, midX, midY);
				}
				else  // the proximal area is rather flat
				{
					slope = slopeBetweenTwoPoints(boundary[leftIndex].x, boundary[leftIndex].y, boundary[rightIndex].x, boundary[rightIndex].y);
				}
			}
			else  // the target is round
			{
				int topY = boundary[getBoundaryIndexByX(boundary, centerOfTarget.x, true)].y;
				if (((boundary[leftIndex].x < centerOfTarget.x) && (boundary[rightIndex].x > centerOfTarget.x)) && ((boundary[leftIndex].y < topY) && (boundary[rightIndex].y < topY))) // if it is clearly indented
				{
					slope = slopeBetweenTwoPoints(boundary[leftIndex].x, boundary[leftIndex].y, boundary[rightIndex].x, boundary[rightIndex].y);
				}
				else  // if it is not clearly indented
				{
					*curved = true;
					// to see if the proximal area is rather flat or pointed
					if (double(boundary[farthestXRightIndex].x - boundary[farthestXLeftIndex].x) / double(boundary[proximalRightIndex].x - boundary[proximalLeftIndex].x) > 1.5)  // the proximal area is pointed
					{
						slope = -1 / slopeBetweenTwoPoints(boundary[farthestYIndex].x, boundary[farthestYIndex].y, centerOfTarget.x, centerOfTarget.y);
					}
					else
					{
						slope = slopeBetweenTwoPoints(boundary[leftIndex].x, boundary[leftIndex].y, boundary[rightIndex].x, boundary[rightIndex].y);
					}
				}
			}
		}
		else  // the target is not symmetric
		{
			if (slopeOnLeft * slopeOnRight > 0)  // the target shape is curved
			{
				*curved = true;
				if ((p1_right.x - farthestY_x) > (farthestY_x - p1_left.x))  // the highest point is on left
				{
					int tempPoint = getBoundaryIndexByX(boundary, (3 * p1_right.x + p1_left.x) / 4, true);
					slope = slopeBetweenTwoPoints(boundary[tempPoint].x, boundary[tempPoint].y, boundary[farthestYIndex].x, boundary[farthestYIndex].y);
					if ((-1 / slope) * slopeOnRight < 0)
					{
						slope = -1 / slope;
					}
				}
				else  // the highest point is on right
				{
					int tempPoint = getBoundaryIndexByX(boundary, (3 * p1_left.x + p1_right.x) / 4, true);
					slope = slopeBetweenTwoPoints(boundary[tempPoint].x, boundary[tempPoint].y, boundary[farthestYIndex].x, boundary[farthestYIndex].y);
					if ((-1 / slope) * slopeOnLeft < 0)
					{
						slope = -1 / slope;
					}
				}
			}
			else // the target shape is round
			{
				*curved = false;
				int leftIndex = getBoundaryIndexByX(boundary, (3 * p1_left.x + p1_right.x) / 4, true);
				int rightIndex = getBoundaryIndexByX(boundary, (3 * p1_right.x + p1_left.x) / 4, true);
				slope = slopeBetweenTwoPoints(boundary[leftIndex].x, boundary[leftIndex].y, boundary[rightIndex].x, boundary[rightIndex].y);
			}
		}
	}
	else  // distal point calculation
	{
		double slopeOnLeft, slopeOnRight;
		CPixel p1_left = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y / 3 * 5, true)];
		CPixel p2_left = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y / 2 * 3, true)];
		slopeOnLeft = slopeBetweenTwoPoints(p1_left.x, p1_left.y, p2_left.x, p2_left.y);

		CPixel p1_right = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y / 3 * 5, false)];
		CPixel p2_right = boundary[getBoundaryIndexByY(boundary, centerOfTarget.y / 2 * 3, false)];
		slopeOnRight = slopeBetweenTwoPoints(p1_right.x, p1_right.y, p2_right.x, p2_right.y);

		if (symmetric)  // the target is symmetric
		{
			int distalLeftIndex = getBoundaryIndexByY(boundary, centerOfTarget.y / 4 * 7, true);  // leftmost point on the line where y = centerY / 4 * 7
			int distalRightIndex = getBoundaryIndexByY(boundary, centerOfTarget.y / 4 * 7, false);  // rightmost point on the line where y = centerY / 4 * 7

			// to see if the lower half of the target is curved or round
			if (slopeOnLeft * slopeOnRight > 0)  // the target is curved
			{
				*curved = true;
				if ((p1_right.x - farthestY_x) > (farthestY_x - p1_left.x))  // the lowest point is on left
				{
					int tempPoint = getBoundaryIndexByX(boundary, (3 * p1_right.x + p1_left.x) / 4, false);
					slope = slopeBetweenTwoPoints(boundary[tempPoint].x, boundary[tempPoint].y, boundary[farthestYIndex].x, boundary[farthestYIndex].y);
					if ((-1 / slope) * slopeOnRight < 0)
					{
						slope = -1 / slope;
					}
				}
				else  // the lowest point is on right
				{
					int tempPoint = getBoundaryIndexByX(boundary, (3 * p1_left.x + p1_right.x) / 4, false);
					slope = slopeBetweenTwoPoints(boundary[tempPoint].x, boundary[tempPoint].y, boundary[farthestYIndex].x, boundary[farthestYIndex].y);
					if ((-1 / slope) * slopeOnLeft < 0)
					{
						slope = -1 / slope;
					}
				}
			}
			else  // the target is round
			{
				int minY = boundary[farthestYIndex].y;
				int minYIndex = farthestYIndex;
				if (centerOfTarget.x / double(centerOfTarget.y) >= 1.1)  // the target shape is flat
				{
					int distanceFromCenter = abs(farthestY_x - centerOfTarget.x);
					int leftIndex = getBoundaryIndexByX(boundary, centerOfTarget.x - distanceFromCenter, false);
					int rightIndex = getBoundaryIndexByX(boundary, centerOfTarget.x + distanceFromCenter, false);
					slope = slopeBetweenTwoPoints(boundary[leftIndex].x, boundary[leftIndex].y, boundary[rightIndex].x, boundary[rightIndex].y);
				}
				else
				{
					*curved = true;
					if ((centerOfTarget.y / double(centerOfTarget.x) >= 1.8))  // the target shape is long
					{
						int midX = (boundary[distalLeftIndex].x + boundary[distalRightIndex].x) / 2;
						int midY = (boundary[distalLeftIndex].y + boundary[distalRightIndex].y) / 2;
						slope = -1 / slopeBetweenTwoPoints(boundary[farthestYIndex].x, boundary[farthestYIndex].y, midX, midY);
					}
					else
					{
						slope = -1 / slopeBetweenTwoPoints(centerOfTarget.x, centerOfTarget.y, boundary[farthestYIndex].x, boundary[farthestYIndex].y);
					}
				}
			}
		}
		else  // the target is not symmetric
		{
			if (slopeOnLeft * slopeOnRight > 0)  // the target shape is curved
			{
				*curved = true;
				if ((p1_right.x - farthestY_x) > (farthestY_x - p1_left.x))  // the lowest point is on left
				{
					int tempPoint = getBoundaryIndexByX(boundary, (3 * p1_right.x + p1_left.x) / 4, false);
					slope = slopeBetweenTwoPoints(boundary[tempPoint].x, boundary[tempPoint].y, boundary[farthestYIndex].x, boundary[farthestYIndex].y);
					if ((-1 / slope) * slopeOnRight < 0)
					{
						slope = -1 / slope;
					}
				}
				else  // the lowest point is on right
				{
					int tempPoint = getBoundaryIndexByX(boundary, (3 * p1_left.x + p1_right.x) / 4, false);
					slope = slopeBetweenTwoPoints(boundary[tempPoint].x, boundary[tempPoint].y, boundary[farthestYIndex].x, boundary[farthestYIndex].y);
					if ((-1 / slope) * slopeOnLeft < 0)
					{
						slope = -1 / slope;
					}
				}
			}
			else // the target shape is round
			{
				*curved = false;
				slope = -1 / slopeBetweenTwoPoints(boundary[farthestYIndex].x, boundary[farthestYIndex].y, centerOfTarget.x, centerOfTarget.y);
			}
		}
	}
	return slope;
}

/**
* Finds a good position of pivot point by looking at the shape of the target and slope value.
*/
CPixel findPivot(vector<CPixel> boundary, CPixel& centerOfTarget, double slope, int farthestXLeftIndex, int farthestXRightIndex, bool proximal, bool symmetric, bool curved)
{
	int tempX, tempY; // temporary x-value and y-value of axisPivot
	if (symmetric)  // the target is symmetric
	{
		if (proximal)  // proximal point calculation
		{
			if ((boundary[farthestXLeftIndex].y - centerOfTarget.y) > 100 || (boundary[farthestXRightIndex].y - centerOfTarget.y) > 100)  // if the lower half of the target is bigger than the upper half
			{
				tempY = centerOfTarget.y / 4;
			}
			else
			{
				if (curved)  // the target shape is curved
				{
					tempY = centerOfTarget.y / 4;
				}
				else  // the target shape is round
				{
					tempY = (boundary[farthestXLeftIndex].y + boundary[farthestXRightIndex].y) / 2;
				}
			}
		}
		else  // distal point calculation
		{
			if (curved)  // the target shape is curved
			{
				tempY = centerOfTarget.y / 4 * 7;
			}
			else  // the target shape is round
			{
				tempY = centerOfTarget.y / 5 * 6;
			}
		}
	}
	else  // the target is not symmetric
	{
		if (proximal)  // proximal point calculation  
		{
			if (curved)  // the target shape is curved
			{
				tempY = centerOfTarget.y / 4;
			}
			else  // the target shape is round
			{
				tempY = centerOfTarget.y / 3;
			}
		}
		else  // distal point calculation
		{
			if (curved)  // the target shape is curved
			{
				tempY = centerOfTarget.y / 4 * 7;
			}
			else  // the target shape is round
			{
				tempY = centerOfTarget.y / 3 * 5;
			}
		}
	}
	tempX = getMidXHorizontal(boundary, tempY);

	// Calculates the best pivot point. 
	// Using the slope value and y-value, it sets up a x-axis, finds the endpoints on the x-axis, and takes the x-value of the midpoint as the x-value of the pivot point.
	double intercept = -slope * tempX + tempY;
	int begin, end;
	int count = 0;
	vector<int> results;
	bool foundBegin = false;
	for (int i = 0; i < boundary.size(); i++) {
		if (isPointOnLine(boundary[i], slope, intercept)) {
			if (!foundBegin) {
				begin = end = i;
				results.push_back(i);
				count++;
				foundBegin = true;
			}
			else {
				if (boundary[i].x < boundary[begin].x) {
					if (abs(i - results[count - 1]) > 5)
					{
						results.push_back(i);
						count++;
					}
					begin = i;
				}
				if (boundary[i].x > boundary[end].x) {
					end = i;
				}
			}
		}
	}
	
	if (count > 1)
	{
		begin = results[1];
	}
	
	CPixel axisPivot((boundary[begin].x + boundary[end].x) / 2,
		(boundary[begin].y + boundary[end].y) / 2);
	return axisPivot;
}

/**
 * Returns the degree of start angle given the slope value.
 */
double findStartAngle(double slope)
{
	double newSlope = 0;
	double startAngle = 0;
	if (-slope > 0)
	{
		newSlope = tan(atan(-1 / -slope) - (45 * pi / 180));
		if (newSlope < 0)
		{
			startAngle = -1 * (180 - (atan(newSlope) * -180 / pi));
		}
		else
		{
			startAngle = -1 * atan(newSlope) * 180 / pi;
		}
	}
	else
	{
		newSlope = tan(atan(-1 / -slope) - (45 * pi / 180));
		if (newSlope < 0)
		{
			startAngle = -1 * (atan(newSlope) * 180 / pi);
		}
		else
		{
			startAngle = -1 * atan(newSlope) * 180 / pi;
		}
	}
	return startAngle;
}

/**
* Returns true if the target is symmetric, false otherwise. 
* The distance between the center of target and the center of image is used as the criterion to decide 
* whether a target is symmetric or not.
*/
bool isSymmetric(vector<CPixel> boundary, CPixel& centerOfTarget, int width, int height)
{
	if (abs(width / 2 - centerOfTarget.x) > 50)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/**
* Returns the index of the leftmost or rightmost point on the line at given y-value
*/
int getBoundaryIndexByY(vector<CPixel> boundary, int Y, bool isLeft)
{
	if (isLeft)
	{
		int leftIndex = 0;
		int minX = 999999;
		for (int i = 0; i < boundary.size(); ++i)
		{
			if (boundary[i].y == Y)
			{
				if (boundary[i].x < minX)
				{
					minX = boundary[i].x;
					leftIndex = i;
				}
			}
		}
		return leftIndex;
	}
	else
	{
		int rightIndex = 0;
		int maxX = 0;
		for (int i = 0; i < boundary.size(); ++i)
		{
			if (boundary[i].y == Y)
			{
				if (boundary[i].x > maxX)
				{
					maxX = boundary[i].x;
					rightIndex = i;
				}
			}
		}
		return rightIndex;
	}
}

/**
* Returns the index of the top or bottom point on the line at given x-value
*/
int getBoundaryIndexByX(vector<CPixel> boundary, int X, bool isTop)
{
	if (isTop)
	{
		int topIndex = 0;
		int minY = 999999;
		for (int i = 0; i < boundary.size(); ++i)
		{
			if (boundary[i].x == X)
			{
				if (boundary[i].y < minY)
				{
					minY = boundary[i].y;
					topIndex = i;
				}
			}
		}
		return topIndex;
	}
	else
	{
		int bottomIndex = 0;
		int maxY = 0;
		for (int i = 0; i < boundary.size(); ++i)
		{
			if (boundary[i].x == X)
			{
				if (boundary[i].y > maxY)
				{
					maxY = boundary[i].y;
					bottomIndex = i;
				}
			}
		}
		return bottomIndex;
	}
}

/**
* Calculates and returns the slope between point (x1, y1) and point (x2, y2)
*/
double slopeBetweenTwoPoints(int x1, int y1, int x2, int y2)
{
	if (abs(x1 - x2) == 0)  // error handling, avoid division by zero error.
	{
		return 999.9;
	}
	else
	{
		return double(y1 - y2) / double(x1 - x2);
	}
}