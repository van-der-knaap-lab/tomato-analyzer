#include "stdafx.h"
#include "../Matrix.h"

using namespace ipt;

CIptMatrix::CIptMatrix()
    : CIptMemBlock<double>()
{
}

CIptMatrix::CIptMatrix(const CIptMatrix& other)
    : CIptMemBlock<double>(other)
{
}


CIptMatrix::CIptMatrix(int nRows, int nCols)
    : CIptMemBlock<double>(nRows, nCols)
{
}


CIptMatrix::CIptMatrix(int nRows, int nCols, double val)
    : CIptMemBlock<double>(nRows, nCols, val)
{
}

CIptMatrix::CIptMatrix(const char * szSpecialMatrix)
{
}

CIptMatrix& CIptMatrix::operator = (const CIptMatrix& other)
{
    copy(other);

    return *this;
}


CIptMatrix::~CIptMatrix()
{
}


CIptMatrix& CIptMatrix::transpose()
{
    int nRows = rows();
    int nCols = columns();

    for(int i=0; i<nRows; i++)
    {
        row_iterator riter;
        
        riter = row_begin(i);
        for(int j=i+1; j<nCols; j++)
        {
            double   tmp;

            tmp = at(j, i);
            at(j, i) = riter[j];
            riter[j] = tmp;
        } //for(int j=i+1; j<nCols; j++)

    } //for(int i=0; i<nRows; i++)

    resize(columns(), rows());
    return *this;
}


CIptMatrix& CIptMatrix::transpose(const CIptMatrix& other)
{
    copy(other);
    transpose();

    return *this;
}


CIptMatrix& CIptMatrix::operator += (const CIptMatrix& A)
{
    add(A);
    
    return *this;
}


CIptMatrix& CIptMatrix::operator -= (const CIptMatrix& A)
{
    subtract(A);

    return *this;
}


CIptMatrix& CIptMatrix::operator *= (const CIptMatrix& A)
{
    multiply(A);

    return *this;
}


CIptMatrix& CIptMatrix::operator /= (const CIptMatrix& A)
{
    devide(A);
    
    return *this;
}


CIptMatrix& CIptMatrix::operator += (double scalor)
{
    add(scalor);

    return *this;
}


CIptMatrix& CIptMatrix::operator *= (double scalor)
{
    multiply(scalor);

    return *this;
}


CIptMatrix& CIptMatrix::operator -= (double scalor)
{
    add(-scalor);

    return *this;
}


CIptMatrix& CIptMatrix::operator /= (double scalor)
{
    devide(scalor);

    return *this;
}


CIptMatrix CIptMatrix::operator -()
{
    CIptMatrix  ret(rows(), columns());

    iterator iter, iter_end, iterThis;
    iter = ret.begin();
    iter_end = ret.end();
    iterThis = begin();
    while(iter!=iter_end)
    {
        *iter = -(*iterThis);

        iter++;
        iterThis++;
    }

    return ret;
}


CIptMatrix CIptMatrix::operator+(const CIptMatrix& A)
{
    CIptMatrix mat;
    mat.add(*this, A);

    return mat;
}


CIptMatrix CIptMatrix::operator+(double scalor)
{
    CIptMatrix mat;
    mat.add(*this, scalor);

    return mat;
}


CIptMatrix CIptMatrix::operator-(const CIptMatrix& A)
{
    CIptMatrix mat;
    mat.subtract(*this, A);

    return mat;
}


CIptMatrix CIptMatrix::operator-(double scalor)
{
    CIptMatrix mat;
    mat.subtract(*this, scalor);

    return mat;
}


CIptMatrix CIptMatrix::operator*(const CIptMatrix& A)
{
    CIptMatrix mat;
    mat.multiply(*this, A);

    return mat;
}


CIptMatrix CIptMatrix::operator*(double scalor)
{
    CIptMatrix mat;
    mat.multiply(*this, scalor);

    return mat;
}


CIptMatrix CIptMatrix::operator/(const CIptMatrix& A)
{
    CIptMatrix mat;
    mat.devide(*this, A);

    return mat;
}


CIptMatrix CIptMatrix::operator/(double scalor)
{
    CIptMatrix mat;
    mat.devide(*this, scalor);

    return mat;
}


CIptMatrix& CIptMatrix::add(double scalor)
{
    iterator iter = begin();
    iterator iter_end = end();

    while(iter != iter_end)
    {
        *iter += scalor;

        ++iter;
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::add(const CIptMatrix& A)
{
    assert(dim()==A.dim());

    iterator iter = begin();
    iterator iter_end = end();
    const_iterator iterA = A.begin();

    while(iter!=iter_end)
    {
        *iter += *iterA;

        ++iter;
        ++iterA;
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::subtract(double val)
{
    add(val);

    return *this;
}


CIptMatrix& CIptMatrix::subtract(const CIptMatrix& A)
{
    assert(dim()==A.dim());

    iterator iter = begin();
    iterator iter_end = end();
    const_iterator iterA = A.begin();

    while(iter!=iter_end)
    {
        *iter -= *iterA;

        ++iter;
        ++iterA;
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::add(const CIptMatrix& A, double scalor)
{
    if(create(A.rows(), A.columns()))
    {
        iterator iter, iter_end;
        const_iterator iterA = A.begin();

        iter = begin();
        iter_end = end();
        iterA = A.begin();
        while(iter!=iter_end)
        {
            *iter = *iterA + scalor;

            ++iter;
            ++iterA;
        }
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::add(const CIptMatrix& A, const CIptMatrix& B)
{
    assert(A.dim() == B.dim());

    if(create(A.rows(), A.columns()))
    {
        iterator iter, iter_end;
        const_iterator iterA, iterB;

        iter = begin();
        iter_end = end();
        iterA = A.begin();
        iterB = B.begin();
        while(iter!=iter_end)
        {
            *iter = *iterA + *iterB;

            ++iter;
            ++iterA;
            ++iterB;
        }
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::subtract(const CIptMatrix& A, double scalor)
{
    add(A, -scalor);

    return *this;
}


CIptMatrix& CIptMatrix::subtract(const CIptMatrix& A, const CIptMatrix& B)
{
    assert(A.dim() == B.dim());

    if(create(A.rows(), A.columns()))
    {
        iterator iter, iter_end;
        const_iterator iterA, iterB;

        iter = begin();
        iter_end = end();
        iterA = A.begin();
        iterB = B.begin();
        while(iter!=iter_end)
        {
            *iter = *iterA - *iterB;

            ++iter;
            ++iterA;
            ++iterB;
        }
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::dotmultiply(const CIptMatrix& A, const CIptMatrix& B)
{
    assert(A.dim() == B.dim());

    if(create(A.rows(), A.columns()))
    {
        iterator iter, iter_end;
        const_iterator iterA, iterB;

        iter = begin();
        iter_end = end();
        iterA = A.begin();
        iterB = B.begin();
        while(iter!=iter_end)
        {
            *iter = (*iterA) * (*iterB);

            ++iter;
            ++iterA;
            ++iterB;
        }
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::dotmultiply(const CIptMatrix& A)
{
    assert(dim() == A.dim());

    iterator iter, iter_end;
    const_iterator iterA;

    iter = begin();
    iter_end = end();
    iterA = A.begin();
    while(iter!=iter_end)
    {
        (*iter) *= (*iterA);

        ++iter;
        ++iterA;
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::dotdevide(const CIptMatrix& A)
{
    assert(dim() == A.dim());

    iterator iter, iter_end;
    const_iterator iterA;

    iter = begin();
    iter_end = end();
    iterA = A.begin();
    while(iter!=iter_end)
    {
        (*iter) /= (*iterA);

        ++iter;
        ++iterA;
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::dotdevide(const CIptMatrix& A, const CIptMatrix& B)
{
    assert(A.dim() == B.dim());

    if(create(A.rows(), A.columns()))
    {
        iterator iter, iter_end;
        const_iterator iterA, iterB;

        iter = begin();
        iter_end = end();
        iterA = A.begin();
        iterB = B.begin();
        while(iter!=iter_end)
        {
            *iter = (*iterA) / (*iterB);

            ++iter;
            ++iterA;
            ++iterB;
        }
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::multiply(double scalor)
{
    iterator iter = begin();
    iterator iter_end = end();

    while(iter != iter_end)
    {
        *iter *= scalor;

        ++iter;
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::multiply(const CIptMatrix& A)
{
    CIptMatrix B;
    B.multiply(*this, A);
    copy(B);

    return *this;
}


CIptMatrix& CIptMatrix::multiply(const CIptMatrix& A, double scalor)
{
    if(create(A.rows(), A.columns()))
    {
        iterator iter, iter_end;
        const_iterator iterA = A.begin();

        iter = begin();
        iter_end = end();
        iterA = A.begin();
        while(iter!=iter_end)
        {
            *iter = (*iterA) * scalor;

            ++iter;
            ++iterA;
        }
    }
    
    return *this;
}


CIptMatrix& CIptMatrix::multiply(const CIptMatrix& A, const CIptMatrix& B)
{
    int a,b,c,d;
    a=A.rows();
    b=A.columns();
    c=B.rows();
    d=B.columns();
    assert(B.rows()==A.columns());

    CIptMatrix::const_row_iterator  riter;

    int nRows = A.rows();
    int nCols = B.columns();
    
    if(create(nRows, nCols))
    {
        for(int iRow=0; iRow<nRows; iRow++)
        {
            riter = A.row_begin(iRow);
            double tmp;
            for(int iCol=0; iCol<nCols; iCol++)
            {
                tmp = 0.0;
                for(int k=0; k<A.columns(); k++)
                {
                    tmp += (riter[k] * B.at(k, iCol));
                }
                
                at(iRow, iCol) = tmp;
            }
        }
    }

    return *this;
}


CIptMatrix& CIptMatrix::devide(double scalor)
{
    assert(scalor!=0.0);
    multiply(scalor);

    return *this;
}


CIptMatrix& CIptMatrix::devide(const CIptMatrix& A)
{
    CIptMatrix mat;

    multiply(mat.inv(A));

    return *this;
}


CIptMatrix& CIptMatrix::devide(const CIptMatrix& A, double scalor)
{
    assert(scalor!=0.0);
    multiply(A, 1/scalor);

    return *this;
}


CIptMatrix& CIptMatrix::devide(const CIptMatrix& A, const CIptMatrix& B)
{
    CIptMatrix mat;

    multiply(A, mat.inv(B));

    return *this;
}


double CIptMatrix::sum() const
{
    const_iterator iter = begin();
    const_iterator iter_end = end();

    double ret=0.0;
    while(iter != iter_end)
    {
        ret += *iter;

        ++iter;
    }
        
    return ret;
}


CIptMatrix& CIptMatrix::sum(const CIptMatrix& mat, int iType)
{
    assert(iType==1 || iType==2);

    if(1 == iType)
    {
        if(create(mat.columns(),1))
        {
            double tmp;
            const_row_iterator  rowitr;
            
            int nRow = mat.rows();
            int nCol = mat.columns();

            for(int i=0; i<nRow; i++)
            {
                tmp = 0.0;
                rowitr = row_begin(i);
                for(int j=0; j<nCol; j++)
                {
                    tmp += rowitr[j];
                }

                at(i) = tmp;
            } //for(int i=0; i<nRow; i++)
        } //if(create(mat.columns(),1))
    } //if(1 == iType)

    if(2 == iType)
    {
        if(create(1, mat.rows()))
        {
            double tmp;
         
            int nRow = mat.rows();
            int nCol = mat.columns();

            for(int j=0; j<nCol; j++)
            {
                tmp = 0.0;
                for(int i=0; i<nRow; i++)
                {
                    tmp += mat.at(i, j);
                }

                at(j) = tmp;
            } //for(int j=0; j<nCol; i++)
        } //if(create(1, mat.rows()))
    } //if(2 == iType)

    return *this;
}


double CIptMatrix::trace() const
{
    int nRows = rows();
    int nCols = columns();
    
    int nMin = (nRows>=nCols)? nCols : nRows;

    double ret=0.0;
    for(int i=0; i<nMin; i++)
    {
        ret += at(i, i);
    }

    return ret;
}


double CIptMatrix::det() const
{
    assert(rows()==columns());

    return 0.0;
}




