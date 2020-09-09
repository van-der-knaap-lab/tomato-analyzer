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
/*  linreg.h */
#ifndef _LINREG_H_
#define _LINREG_H_


class LinearRegression
{
    public:
		double Vertical;

		LinearRegression();
		void Reset();
		void addXY(const double * x, const double * y, int number);
        void Calculate();   // calculate coefficients

		long items() const { return n; }
        double getA() const { return a; }
		double getB() const { return b; }
		double getsumX() const { return sumX; }
		double getsumY() const { return sumY; }
		double getsumXsquared() const { return sumXsquared; }
		double getsumYsquared() const { return sumYsquared; }
		double getRSquared(const double * x, const double * y, int number) const; 

		double getCoefDeterm() const { return coefD; }
		double getCoefCorrel() const { return coefC; }
		double getStdErrorEst() const{ return stdError; }
		double getVertical() const {return Vertical;}
		bool   IsVertical() const {return bVertical;}

    protected:
		bool   bVertical;
        long n;             // number of data points input
        double sumX, sumY;  // sums of x and y
        double sumXsquared, // sum of x squares
               sumYsquared; // sum y squares
        double sumXY;       // sum of x*y

        double a, b;        // coefficients of f(x) = a*x + b
        double coefD,       // coefficient of determination
               coefC,       // coefficient of correlation
               stdError;    // standard error of estimate
};

#endif                      // end of linreg.h


