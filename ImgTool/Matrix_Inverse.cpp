#include "stdafx.h"
#include "../Matrix.h"
#include <cmath>
#include <cfloat>

using namespace ipt;


CIptMatrix& CIptMatrix::inv(const CIptMatrix& X)
{
    ipt::CIptMatrix  Y;

    int N = X.rows();
    assert(N==X.columns());

   Y.create(N, 2*N);

    //if(blOk)
    {
        int  iRow, iCol;
        CIptMatrix::row_iterator    iterA, iterCur;
        CIptMatrix::const_iterator  iterX=X.begin();
        
        for(iRow=0; iRow<N; iRow++)
        {
            iterA = Y.row_begin(iRow);
            iterX = X.begin();

            for(iCol=0; iCol<N; iCol++)
            {
                iterA[iCol] = *iterX;

                ++iterX;
            }

            for(iCol=N; iCol<2*N; iCol++)
            {
                iterA[iCol] = 0.0;
            }

            iterA[iRow+N] = 1.0;
        }

        for(iRow=0; iRow<N; iRow++)
        {
            iterCur = Y.row_begin(iRow);

            double tmp = iterCur[iRow];
            iterCur[iRow]=1.0;
            for(iCol=iRow+1; iCol<N; iCol++)
            {
                iterCur[iCol] /= tmp;
            }

            for(int i=0; i<N; i++)
            {
                if(i == iRow)
                    continue;

                iterA = Y.row_begin(i);
                tmp = iterA[iRow];
                iterA[iRow] = 0.0;

                for(int j=iRow+1; j<2*N; j++)
                {
                    iterA[j] -= (iterCur[j] * tmp);
                }
            }
        }

        if(create(N, N))
        {
            CIptMatrix::iterator  iterThis=begin();
            for(int iRow=0; iRow<N; iRow++)
            {
                iterA = Y.row_begin(iRow);

                for(int iCol=N; iCol<2*N; iCol++)
                {
                    *iterThis = iterA[iCol];

                    ++iterThis;
                }
            }
        }
    }

    return *this;
}


bool inv(CIptMatrix& X, CIptMatrix& ret)
{
    CIptMatrix Y;

    int N = X.rows();
    int M = 2*N;

    int  iRow, iCol;
    
    if(Y.create(N, M))
    {
        CIptMatrix::row_iterator    iterA, iterCur;
        CIptMatrix::iterator  iterX = X.begin();
        
        for(iRow=0; iRow<N; iRow++)
        {
            iterA = Y.row_begin(iRow);
            
            for(iCol=0; iCol<N; iCol++)
            {
                iterA[iCol] = *iterX;
                
                ++iterX;
            }
            
            for(iCol=N; iCol<2*N; iCol++)
            {
                iterA[iCol] = 0.0;
            }
            
            iterA[iRow+N] = 1.0;
        }
        
        
        
        for(iRow=0; iRow<N; iRow++)
        {
            iterCur = Y.row_begin(iRow);
            
            double tmp = iterCur[iRow];
            if(fabs(tmp)<=FLT_EPSILON)
                return false;

            iterCur[iRow]=1.0;
            for(iCol=iRow+1; iCol<M; iCol++)
            {
                iterCur[iCol] /= tmp;
            }
            
            for(int i=0; i<N; i++)
            {
                if(i == iRow)
                    continue;
                
                iterA = Y.row_begin(i);
                tmp = iterA[iRow];
                iterA[iRow] = 0.0;
                
                for(int j=iRow+1; j<M; j++)
                {
                    iterA[j] -= (iterCur[j] * tmp);
                }
            }
        }
        
        if(ret.create(N, N))
        {
            CIptMatrix::iterator  iterThis=ret.begin();
            for(int iRow=0; iRow<N; iRow++)
            {
                iterA = Y.row_begin(iRow);
                
                for(int iCol=N; iCol<M; iCol++)
                {
                    *iterThis = iterA[iCol];
                    
                    ++iterThis;
                }
            }
        }

    }

    return true;
}