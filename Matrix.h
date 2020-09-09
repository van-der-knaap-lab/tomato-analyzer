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
#ifndef _IPT_MATRIX_H
#define _IPT_MATRIX_H

#include "ImageCore.h"

namespace ipt {

class CIptMatrix : public CIptMemBlock<double> {
public:
    /**
     * constructors
     */
    CIptMatrix();
    CIptMatrix(const CIptMatrix& other);
    CIptMatrix(int nRows, int nCols);
    CIptMatrix(int nRows, int nCols, double val);
    CIptMatrix(const char * szSpecialMatrix);
    CIptMatrix& operator = (const CIptMatrix& other);

    virtual ~CIptMatrix();
    
public:
    /**
     * add scalor to all elements in this matrix 
     */
    CIptMatrix& add(double scalor);

    /**
     * add another matrix A to this matrix 
     */
    CIptMatrix& add(const CIptMatrix& A);

    /**
     * add matrix A and a scalor, then set this matrix to be the sum
     */
    CIptMatrix& add(const CIptMatrix& A, double scalor);
    
    /**
     * add matrices A and B, then set this matrix to be the sum
     */
    CIptMatrix& add(const CIptMatrix& A, const CIptMatrix& B);

    /**
     * sum of all the elements in this matrix 
     */
    double sum(void) const;

    /**
     * sum of row vector or column vectors
     * same as sum(mat, type) in Matlab language
     */
    CIptMatrix& sum(const CIptMatrix& mat, int type);

    /**
     * subtract each element in this matrix by scalor
     */
    CIptMatrix& subtract(double scalor);

    /**
     * subtract this matrix by other one
     */
    CIptMatrix& subtract(const CIptMatrix& A);

    /**
     * this matrix = A-scalor
     */
    CIptMatrix& subtract(const CIptMatrix& A, double scalor);
    
    /**
     * this matrix = A - B
     */
    CIptMatrix& subtract(const CIptMatrix& A, const CIptMatrix& B);
    
    /**
     * element-by-element products of A and B (A.*B in Matlab)
     * and set this matrix to be the product
     */
    CIptMatrix& dotmultiply(const CIptMatrix& A, const CIptMatrix& B);

    /**
     * element-by-element products of A and this matrix
     * and update this matrix to be the product
     */
    CIptMatrix& CIptMatrix::dotmultiply(const CIptMatrix& A);

    /**
     * element-by-element divisions of A and B (A./B in Matlab)
     * and set this matrix to be the result
     */
    CIptMatrix& dotdevide(const CIptMatrix& A, const CIptMatrix& B);

    /**
     * element-by-element divisions of A and this matrix (this./A in Matlab)
     * and update this matrix to be the result
     */
    CIptMatrix& dotdevide(const CIptMatrix& A);

    /**
     * matrix multiplication of A and B
     * (denoted as A*B in Matlab)
     * and set this matrix to be the product
     */
    CIptMatrix& multiply(const CIptMatrix& A, const CIptMatrix& B);

    /**
     * multiplication of matrix A and a scalor
     * and set this matrix to be the product
     */
    CIptMatrix& multiply(const CIptMatrix& A, double scalor);

    /**
     * multiply this matrix by ther other one
     */    
    CIptMatrix& multiply(const CIptMatrix& A);

    /**
     * multiply each element in this matrix by scalor
     */
    CIptMatrix& multiply(double scalor);

    /**
     * matrix division of A and B, which is same as A*inv(B)
     * denoted as A/B in Matlab
     */
    CIptMatrix& devide(const CIptMatrix& A, const CIptMatrix& B);

    /**
     * division of matrix A and a scalor
     */
    CIptMatrix& devide(const CIptMatrix& A, double scalor);

    /**
     * devide this matrix by the other one
     * and update this matrix
     */
    CIptMatrix& devide(const CIptMatrix& A);

    /**
     * devide this matrix by a scalor
     * and update this matrix
     */
    CIptMatrix& devide(double scalor);


    /*
     * overloaded operators
     */
    CIptMatrix& operator += (const CIptMatrix& other);
    CIptMatrix& operator -= (const CIptMatrix& other);
    CIptMatrix& operator *= (const CIptMatrix& other);
    CIptMatrix& operator /= (const CIptMatrix& other);

    CIptMatrix& operator += (double scalor);
    CIptMatrix& operator -= (double scalor);
    CIptMatrix& operator *= (double scalor);
    CIptMatrix& operator /= (double scalor);

    CIptMatrix operator + (const CIptMatrix& other);
    CIptMatrix operator - (const CIptMatrix& other);
    CIptMatrix operator * (const CIptMatrix& other);
    CIptMatrix operator / (const CIptMatrix& other);

    CIptMatrix operator + (double scalor);
    CIptMatrix operator - (double scalor);
    CIptMatrix operator * (double scalor);
    CIptMatrix operator / (double scalor);


    /**
     * unary minus operation
     */
    CIptMatrix operator -(void);

    /**
     * inversion of this matrix
     */
    CIptMatrix& inv(CIptMatrix& Y);

    /**
     * inversion of input matrix A
     */
    CIptMatrix& inv(const CIptMatrix& A);

    /**
     * matrix determinant
     */
    double det(void) const;

    /**
     * rank of determination
     */
    int rank() const;

    /**
     * trace
     */
    double trace(void) const;

    /**
     * eigenvalue and eigenvector
     */
    std::vector<double>*   eig(CIptMatrix& vec);
    
    
    /**
     * array transpose of matrix, same as operator .' in Matlab
     */
    CIptMatrix& transpose(void);

    /**
     * set this matrix to be the transpose of the other one
     */
    CIptMatrix& transpose(const CIptMatrix& other);


}; // class CIptMatrix


} // namespace ipt

bool inv(ipt::CIptMatrix& Y, ipt::CIptMatrix& ret);

#endif // _IPT_MATRIX_H