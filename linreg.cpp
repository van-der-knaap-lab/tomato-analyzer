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

#include "linreg.h"
#include <math.h>
#include <float.h>


LinearRegression::LinearRegression()
{
	Reset();
}

void LinearRegression::addXY(const double * x, const double * y, int number)
{
	for(int i=0; i<number; i++)
	{
		n++;
		sumX += x[i];
		sumY += y[i];
		sumXsquared += x[i] * x[i];
		sumYsquared += y[i] * y[i];
		sumXY += x[i] * y[i];
	}
}

void LinearRegression::Reset()
{
	n=0;
	sumX=0;
	sumY=0;
	sumXsquared=0;
	sumYsquared=0;
	sumXY=0;
	Vertical=1.0;
	bVertical = false;
}

void LinearRegression::Calculate()
{
	if (fabs( double(n) * sumXsquared - sumX * sumX) 
		> FLT_EPSILON)	// This is not exactly correct but seems to work for cases encountered in Tomato Analyzer. 
						// Previously it was DBL_EPSILON, which was too small in some cases. It should probably 
						// depend on the numerator in the calculation of 'a' below, or something. 
	{
		a = ( double(n) * sumXY - sumY * sumX) /
			( double(n) * sumXsquared - sumX * sumX);
		b = (sumY - a * sumX) / double(n);
		
		double sx = a * (sumXY - sumX * sumY / double(n));
		double sy2 = sumYsquared - sumY * sumY / double(n);
		double sy = sy2 - sx;
		
		coefD = sx / sy2;
		coefC = sqrt(coefD);
		stdError = sqrt(sy / double(n - 2));
		bVertical = false;
	}
	else
	{
		a = b = coefD = coefC = stdError = 0.0;
		Vertical = sumX/double(n);
		bVertical =true;
	}
}

/** 
 * If you first call addXY(x, y, n) and Calculate(), then getRSquared(x, y, n) returns 
 * the R squared value for the calculated linear regression model. 
 */
double LinearRegression::getRSquared(const double * x, const double * y, int n) const
{
	double avgY = 0; 
	for (int i = 0; i < n; i++) {
		avgY += y[i]; 
	}
	avgY /= n; 

	double ssr = 0;		// sum of squared residuals--	y_i - f(x_i)
	double ssy = 0;		// sum of squared differences-- y_i - mean(y)
	for (int i = 0; i < n; i++) {
		ssr += pow( y[i] - (a * x[i] + b), 2 ); 
		ssy += pow( y[i] - avgY, 2 ); 
	}
	return 1 - ssr/ssy; 
}

