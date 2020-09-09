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
#ifndef NEW_IMG_FUNCTION_H
#define NEW_IMG_FUNCTION_H

typedef std::vector<int> INTVECTOR;

#include "ImgTool.h"
using namespace ipt;

CAdvImage *
CIimresize(CAdvImage * pImg, int nNewWidth, int nNewHeight, LPCTSTR lpszMethod);

CAdvImage *
CIimresize(CAdvImage * pImg, float fs, LPCTSTR lpszMethod);

CBitmap * 
CreateBitmap(CDC* pDC, CAdvImage * pImage);

CBitmap * 
CreateBitmap(CDC* pDC, CAdvImage * pImage, float fScale, LPCTSTR lpszMethod);

CBitmap * 
CreateBitmap(CDC* pDC, CBitmap * pBitmapSrc, float fScale, LPCTSTR lpszMethod);

CPixelList *
MakeLine(POINT p1, POINT p2);

int MOD(int x1, int x2);
BOOL isEVEN(int x);
int round(float fv);

bool TwoDimPCA(double ** d, int nCol, double& rot, double&, double&);
double * conv(double * x, int nLenX, double * y, int nLenY);
double * GaussianCurvature(int x[], int y[], int nLength, double sigm, int * nx, int * ny);
int * findZeroCrossingPoint(double * data, int nLength, int& nFoundLength);
int * findMaximumPoint(double * data, int nLength, int& nMaxPointLength);
int * blipfilter(int * d, int nLength);
void Cart2Polar(double *x, double *y, int nLength, double * r, double * t);
int * Zhu_Chirlian_CriticalPointEstimation(int * x, int * y, int nLength, int&);
double * GaussianFilter(double sigma, int& nLength);
double distanceFromPointToLineInPolar(double rL, double tL, double rC, double tC, double rR, double tR);
double distanceFromPointToLine(double xa, double ya, double xb, double yb, double xc, double yc);
COLORREF GetColor(int iIndex, int nTotal);

std::vector<IPTPOINT>* circle(CPixel center, double radius); 
std::vector<IPTPOINT>* ellipse(CPixel center, CPixel a, CPixel b); 
std::vector<IPTPOINT>* ellipse(CPixel center, double aDist, double bDist, double angle); 
void fitEllipse(CPixelList * points, CPixel& center, CPixel& a, CPixel& b, double& rsquared, bool doCircle); 
bool area_convex(std::vector<POINT>& pix, int& area);

CPixelList* convert(std::vector<POINT>* p);
std::vector<IPTPIXEL>* convert(CPixelList * p);
int diffImgArea(std::vector<IPTPIXEL>*, std::vector<IPTPOINT>*);

void circular(std::vector<IPTPOINT>* pixel, double& xc, double& yc, double& cd, double& r);
void circular(CPixelList* ppixel, double& xc, double& yc, double& cd, double& r);

RECT GetBoundaryBoxAfterRotation(RECT &src, double angle);
double computeAngle(POINT ptL, POINT ptC, POINT ptR);

template <class T> int minValueIndex(T * d, int nLength);

vector<vector<int> > pointsOnLines(vector<CPixel> boundary, double slope, double startIntercept, double incrIntercept, 
									 int fromIndex, int toIndex); 
bool pointsOnLine(int& bottom, int& top, vector<CPixel> boundary, double xIntercept, int fromIndex, int toIndex); 
bool pointsOnLine(int& leftmost, int& rightmost, std::vector<CPixel> boundary, double slope, double intercept, 
				  int fromIndex, int toIndex); 
int findTipBySymmetry(CAdvImage * img, CPixelList * b, bool proximal); // modified by the 2015 AMRE team

// The following functions are added by Nan (5/30/2015)
// They are used to improve the algorithm used to calculate proximal/distal point
int getMidXHorizontal(vector<CPixel> boundary, int Y);
int findFarthestXFromCenter(vector<CPixel> boundary, CPixel& centerOfMass, int top, bool isRight);
int findFarthestY(vector<CPixel> boundary, bool proximal);
double findSlopeForPivot(vector<CPixel> boundary, CPixel& centerOfMass, int higherShoulderIndex, int farthestXLeftIndex, int farthestXRightIndex, bool proximal, bool symmetric, bool* curved);
CPixel findPivot(vector<CPixel> boundary, CPixel& centerOfMass, double slopeOrHigherShoulderIndex, int farthestXLeftIndex, int farthestXRightIndex, bool proximal, bool symmetric, bool curved);
double findStartAngle(double slope);
bool isSymmetric(vector<CPixel> boundary, CPixel& centerOfMass, int width, int height);
int getBoundaryIndexByY(vector<CPixel> boundary, int Y, bool isLeft);
int getBoundaryIndexByX(vector<CPixel> boundary, int X, bool isTop);
double slopeBetweenTwoPoints(int x1, int y1, int x2, int y2);
#endif