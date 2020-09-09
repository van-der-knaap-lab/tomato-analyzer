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
#include "PixelList.h"
#include "AdvImage.h"
#include "TomatoAnalyzer.h"
#include "BinObjs.h"
#include "vector"
#include "new_img_function.h"
#include <math.h>
#include "imgTool.h"
#include "Matrix.h"
#include "linreg.h"
#include <fstream>

using namespace ipt;

#define pi 3.1415926535897932

void shapeSampling(char * szParemeter)
{
}


void shapeSampling(double gridSize, double a, double b, double n)
{

}

std::vector<IPTPOINT>* circle(CPixel center, double radius) 
{
	return ellipse(center, radius, radius, 0); 
}

/** 
 * Returns a vector of points that form the boundary of an ellipse (special case: circle) with axis lengths of
 * aDist and bDist and rotation of angle (in radians). 
 */ 
std::vector<IPTPOINT>* ellipse(CPixel center, double aDist, double bDist, double angle) 
{
	std::vector<IPTPOINT>* pts = new std::vector<IPTPOINT>;

	double incr = pi/2 / aDist; 
	for (double t = 0; t < 2 * pi; t += incr)
	{
		// http://en.wikipedia.org/wiki/Ellipse#General_parametric_form
		IPTPOINT p( round( center.x + aDist * cos(t) * cos(angle) - bDist * sin(t) * sin(angle) ), 
			round( center.y + aDist * cos(t) * sin(angle) + bDist * sin(t) * cos(angle) ) ); 
		pts->push_back(p); 
	}

	return pts; 
}

/** 
 * Returns a vector of points that form the boundary of an ellipse (special case: circle) with axes a and b. 
 */ 
std::vector<IPTPOINT>* ellipse(CPixel center, CPixel a, CPixel b) 
{
	double aDist = sqrt( pow( double(a.x), 2) + pow( double(a.y), 2) ); 
	double bDist = sqrt( pow( double(b.x), 2) + pow( double(b.y), 2) ); 
	double angle = atan2( double(a.y) , double(a.x) ); 
	return ellipse(center, aDist, bDist, angle); 
}


/** 
 * Calculates the best-fit ellipse (doCircle=false) or circle (doCircle=true) for the given points. 
 * Passes back the center point, the ellipse major and minor axis vectors (a and b) or circle radius (a.x and b.y), 
 * and the average residual magnitude across all of the points (error). 
 */
void fitEllipse(CPixelList * points, CPixel& center, CPixel& a, CPixel& b, double& error, bool doCircle)
{
	// Find center -- average x and y of all points
	double avgX = 0, avgY = 0; 
	for (CPixelList::iterator i = points->begin(); i != points->end(); i++) {
		avgX += i->x; 
		avgY += i->y; 
	}
	avgX /= points->size(); 
	avgY /= points->size(); 
	center.x = round(avgX); 
	center.y = round(avgY); 

	double * regX = new double[points->size()]; 
	double * regY = new double[points->size()]; 
	double aDist, bDist;	// magnitude of a and b
	if (doCircle) 
	{
		// Find radius -- average distance from center to each point
		double avgR = 0; 
		for (CPixelList::iterator i = points->begin(); i != points->end(); i++) {
			avgR += sqrt( pow(i->x - avgX, 2) + pow(i->y - avgY, 2) ); 
		}
		avgR /= points->size(); 
		aDist = bDist = avgR; 
		a.x = b.y = round(aDist); 
		a.y = b.x = 0; 		
	} 
	else 
	{
		// Find a and b -- use principal component analysis
		// http://ask.metafilter.com/36213/Best-Fit-Ellipse (2nd reply)
		// http://number-none.com/product/My%20Friend,%20the%20Covariance%20Body/index.html

		double mat[2][2];	// Will be the covariance matrix. 
							// Eigenvectors will be major & minor axes. Eigenvalues will be lengths of axes, squared. 

		mat[0][0] = mat[0][1] = mat[1][0] = mat[1][1] = 0; 
		for (CPixelList::iterator i = points->begin(); i != points->end(); i++) 
		{
			// Add  [ x - avgX, y - avgY ] * [ x - avgX ]  to mat
			//								 [ y - avgY ]
			double diffX = i->x - avgX; 
			double diffY = i->y - avgY; 
			mat[0][0] += diffX * diffX; 
			mat[0][1] += diffX * diffY; 
			mat[1][1] += diffY * diffY; 
		}
		mat[0][0] /= points->size(); 
		mat[0][1] /= points->size(); 
		mat[1][1] /= points->size(); 
		mat[1][0] = mat[0][1]; 

		// http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
		double T = mat[0][0] + mat[1][1];							// Trace
		double D = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];	// Determinant
		double L1 = T/2 + sqrt(T*T/4 - D);		// Eigenvalues
		double L2 = T/2 - sqrt(T*T/4 - D);		// 
		aDist = sqrt(2*L1); 
		bDist = sqrt(2*L2); 
		double aX, aY, bX, bY; 
		if (mat[0][1] != 0) {
			aX = L1 - mat[1][1];			// Eigenvectors
			aY = mat[1][0];					//
			bX = L2 - mat[1][1];			// 
			bY = mat[1][0];					// 
		} else {
			if (mat[0][0] >= mat[1][1]) {
				aX = bY = 1;	// a along x-axis, b along y-axis
				aY = bX = 0;	// 
			} else {
				aX = bY = 0;	// a along y-axis, b along x-axis
				aY = bX = 1;	// 
			}
		}

		// Scale each eigenvector to have the magnitude of the corresponding eigenvalue. 
		double aAngle = atan( double(aY) / double(aX) ); 
		a.x = round( aDist * cos(aAngle) ); 
		a.y = round( aDist * sin(aAngle) ); 
		double bAngle = atan( double(bY) / double(bX) ); 
		b.x = round( bDist * cos(bAngle) ); 
		b.y = round( bDist * sin(bAngle) ); 
	}

	// Find average residual magnitude -- for each point, the difference between:  
	// - the actual distance from the center to the point
	// - the distance from the center to a point on the circle/ellipse at the same angle as the point
	double rotAngle = doCircle ? 0 : atan( double(a.y) / double(a.x) ); 
	double sumResiduals = 0; 
	int index = 0; 
	for (CPixelList::iterator i = points->begin(); i != points->end(); i++, index++) {
		double actualX = i->x - avgX; 
		double actualY = i->y - avgY; 
		double actualDist = sqrt( pow(actualX, 2) + pow(actualY, 2) ); 
		double angle = atan2(actualY, actualX) - rotAngle; 
		// We know expectedY = expectedX * tan(angle), so plug into ellipse equation and solve. 
		double expectedX = sqrt( pow(aDist*bDist, 2) / (pow(bDist, 2) + pow(aDist*tan(angle), 2)) ); 
		double expectedY = expectedX * tan(angle); 
		if (angle < -pi/2 || angle > pi/2) {
			expectedX = -expectedX; 
			expectedY = -expectedY; 
		}
		double expectedDist = sqrt( pow(expectedX, 2) + pow(expectedY, 2) ); 
		sumResiduals += fabs(actualDist - expectedDist); 
	}
	error = sumResiduals / points->size(); 
	if (doCircle) {
		error /= aDist; 
	} else {
		error /= aDist; 
	}

	delete[] regX; 
	delete[] regY; 
}

double error_superquadric(double* x, double* y, int nLength, double a, double b, double n)
{
    double e, tmp;

    e = 0.0;
    for(int iLoop=0; iLoop<nLength; iLoop++)
    {
        tmp = pow(x[iLoop]/a, n) + pow(y[iLoop]/b, n) - 1.0;
         e = e + tmp*tmp;
    }

    return e;
}

/// Not used
void superquadric(std::vector<IPTPOINT>* pixel, double a, double b, double& cd, double& n)
{
    int nLength = pixel->size();
    double* x = new double[2*nLength];
    double* y = x + nLength;

	std::vector<IPTPOINT>::iterator iter = pixel->begin();
	for(int iIndex=0; iIndex<nLength; iIndex++)
	{
        x[iIndex] = fabs((double)(iter->x));
        y[iIndex] = fabs((double)(iter->y));

		++iter;
	}


    double eL, eR, e, nL, nR;
    eL=eR=e=nL=nR=0.0;
    nL=1.5;       eL=error_superquadric(x, y, nLength, a, b, nL);
    nR=4.0;     eR=error_superquadric(x, y, nLength, a, b, nR);

    double nmin=1.66;
    double emin = error_superquadric(x, y, nLength, a, b, nmin);
    for(n=1.68; n<3.00; n+=0.01)
    {
        e = error_superquadric(x, y, nLength, a, b, n);
        if(emin > e)
        {
            emin = e;
            nmin = n;
        }
    }

    n = nmin;
    delete [] x;
    return;



	while(true)
    {
        n=(nL+nR)/2;
        e=error_superquadric(x, y, nLength, a, b, n);

        if(eR<eL)
        {
            if(e<eR)
            {
                nL=n;
                eL=e;
            }
            else
            {
                nL=(n+nL)/2;
                eL=error_superquadric(x, y, nLength, a, b, nL);
            }
        }
        else
        {
            if(e<eL)
            {
                nR = (n+nR)/2;
                eR = error_superquadric(x, y, nLength, a, b, nR);
            }
            else
            {
                nR = n;
                eR = e;
            }

        }

        if((nR-nL)<0.01)
        {
            n = (nL+nR)/2.0;
            break;
        }
    }

	
	return;	
}

void superquadric(CPixelList * ppixel, double a, double b, double& cd, double& n)
{
    int nLength = ppixel->size();
    double* x = new double[2*nLength];
    double* y = x + nLength;

	CPixelList::iterator iter = ppixel->begin();
	for(int iIndex=0; iIndex<nLength; iIndex++)
	{
        x[iIndex] = fabs((double)(iter->x));
        y[iIndex] = fabs((double)(iter->y));

		++iter;
	}


    double eL, eR, e, nL, nR;
    eL=eR=e=nL=nR=0.0;
    nL=1.5;       eL=error_superquadric(x, y, nLength, a, b, nL);
    nR=4.0;     eR=error_superquadric(x, y, nLength, a, b, nR);

    double nmin=1.66;
    double emin = error_superquadric(x, y, nLength, a, b, nmin);
    for(n=1.68; n<3.00; n+=0.01)
    {
        e = error_superquadric(x, y, nLength, a, b, n);
        if(emin > e)
        {
            emin = e;
            nmin = n;
        }
    }

    n = nmin;

	while(true)
    {
        n=(nL+nR)/2;
        e=error_superquadric(x, y, nLength, a, b, n);

        if(eR<eL)
        {
            if(e<eR)
            {
                nL=n;
                eL=e;
            }
            else
            {
                nL=(n+nL)/2;
                eL=error_superquadric(x, y, nLength, a, b, nL);
            }
        }
        else
        {
            if(e<eL)
            {
                nR = (n+nR)/2;
                eR = error_superquadric(x, y, nLength, a, b, nR);
            }
            else
            {
                nR = n;
                eR = e;
            }

        }

        if((nR-nL)<0.01)
        {
            n = (nL+nR)/2.0;
            break;
        }
    }

	delete x;
	return;	
}

bool area_convex(std::vector<IPTPOINT>& pix, int& area)
{
	int nLength = pix.size();

	std::vector<IPTPOINT>::iterator itr, itr_end;
	itr=pix.begin();
	itr_end=pix.end();

	int ymin=itr->y;
	int ymax=itr->y;
	while(++itr != itr_end)
	{
		if(ymin>itr->y)
			ymin = itr->y;

		if(ymax<itr->y)
			ymax = itr->y;
	}

	area = 0;
	int xleft, xright;

	for(int y=ymin; y<=ymax; y++)
	{
		bool bclose;

		itr=pix.begin();

		while(itr!=itr_end && itr->y != y)
			++itr;

		if(itr==itr_end)
			return false;

		xleft = xright = itr->x;
		bclose= false;
		while(itr!=itr_end)
		{
			if(itr->y == y)
			{
				if(itr->x < xleft)
					xleft = itr->x;

				if(itr->x > xright)
					xright = itr->x;

				bclose=true;
			}

			++itr;
		}

		if(!bclose)
			return false;

		area += (xright-xleft+1);
	}
	
	return true;
}