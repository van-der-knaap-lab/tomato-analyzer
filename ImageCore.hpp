#ifndef _IPT_IMAGECORE_HPP
#define _IPT_IMAGECORE_HPP

namespace ipt {

template <class T>
inline CIptMemBlock<T>::CIptMemBlock()
{
    init();
}


template <class T>
inline CIptMemBlock<T>::CIptMemBlock(int nRows, int nCols)
{
    init();
    create(nRows, nCols);
}


template <class T>
inline CIptMemBlock<T>::CIptMemBlock(int nRows, int nCols, T vinit)
{
    init();
    create(nRows, nCols, vinit);
}


template <class T>
inline CIptMemBlock<T>::~CIptMemBlock()
{
    clear();
}


template <class T>
inline CIptMemBlock<T>::CIptMemBlock(const CIptMemBlock<T>& other)
{
    init();
    copy(other);    
}


template <class T>
inline CIptMemBlock<T>& CIptMemBlock<T>::operator=(const CIptMemBlock<T>& other)
{
    copy(other);
    return *this;
}


template <class T>
inline CIptMemBlock<T>&  CIptMemBlock<T>::copy(const CIptMemBlock& other)
{
    if(create(other.rows(), other.columns()))
    {
        const_iterator   iterSrc    = other.begin();
        const_iterator   iterEndSrc = other.end();

        iterator    iterDst = begin();
        while(iterSrc != iterEndSrc)
        {
            *iterDst = *iterSrc;

            ++iterDst;
            ++iterSrc;
        } //while(iterSrc != iterEndSrc)

    } //if(create(other.rows(), other.columns()))

    return *this;
}


template <class T>
inline int CIptMemBlock<T>::size() const 
{
    return m_nRows * m_nCols;
}


template <class T>
inline int CIptMemBlock<T>::rows() const 
{
    return m_nRows;
}


template <class T>
inline int CIptMemBlock<T>::columns() const 
{
    return m_nCols;
}


template <class T>
inline IPTPIXEL CIptMemBlock<T>::dim() const 
{
    return IPTPIXEL(m_nRows, m_nCols);
}


template <class T>
inline bool CIptMemBlock<T>::resize(int nNewRows, int nNewCols) 
{
    if(nNewRows*nNewCols == m_nRows*m_nCols)
    {
        m_nRows = nNewRows;
        m_nCols = nNewCols;

        return true;
    }

    return false;
}


template <class T>
inline bool CIptMemBlock<T>::create(int nRows, int nCols)
{
    assert(nRows>0 && nCols>0);
    
    if(m_nRows*m_nCols != nRows*nCols || m_blAttached)
    {
        clear();
        m_pMemData = new Data_Type[nRows * nCols];
    }

    if(0 != m_blkiter)
    {
        delete m_blkiter;
        m_blkiter = 0;
    }

    if(0 != m_pMemData)
    {
        m_nRows = nRows;
        m_nCols = nCols;

        return true;
    }

    return false;
}


template <class T>
inline bool CIptMemBlock<T>::create(int nRows, int nCols, T val)
{
    if(create(nRows, nCols))
    {
        fill(val);

        return true;
    }

    return false;
}


template <class T>
inline void CIptMemBlock<T>::fill(T val)
{
    iterator iter = begin();
    iterator iter_end = end();
    while(iter != iter_end)
    {
        *iter = val;
        ++iter;
    }

    return;
}


template <class T>
inline void CIptMemBlock<T>::attach(T* pMem, int nRows, int nCols)
{
    clear();

    m_pMemData   = pMEm;
    m_nRows      = nRows;
    m_nCols      = nCols;
    m_blAttached = true;
}


template <class T>
inline void CIptMemBlock<T>::attach(const CIptMemBlock<T>& pIptMemBlk)
{
    attach(IptMemBlk.m_pMemData, IptMemBlk.m_nRows, IptMemBlk.nCols);
}


template <class T>
inline T&  CIptMemBlock<T>::at(const int pos)
{
    assert(pos>=0);
    assert(pos<(m_nRows * m_nCols));
    
    return m_pMemData[pos];
}


template <class T>
inline const T&  CIptMemBlock<T>::at(const int pos) const
{
    assert(pos>=0);
    assert(pos<(m_nRows * m_nCols));
    
    return m_pMemData[pos];
}


template <class T>
inline T&  CIptMemBlock<T>::at(const IPTPIXEL& pxl)
{
    return at(pxl.iRow, pxl.iCol);
}


template <class T>
inline const T&  CIptMemBlock<T>::at(const IPTPIXEL& pxl) const
{
    return at(pxl.iRow, pxl.iCol);
}


template <class T>
inline T&  CIptMemBlock<T>::at(const IPTPOINT& pt)
{
    return at(pt.y, pt.x);
}


template <class T>
inline const T&  CIptMemBlock<T>::at(const IPTPOINT& pt) const
{
    return at(pt.y, pt.x);
}


template <class T>
inline T&  CIptMemBlock<T>::at(const int iRow, const int iCol)
{
//    assert(iRow>=0 && iRow<m_nRows);
//    assert(iCol>=0 && iCol<m_nCols);

    return m_pMemData[iRow*m_nCols + iCol];
}


template <class T>
inline const T&  CIptMemBlock<T>::at(const int iRow, const int iCol) const
{
    assert(iRow>=0 && iRow<m_nRows);
    assert(iCol>=0 && iCol<m_nCols);

    return m_pMemData[iRow * m_nCols + iCol];
}


template <class T>
inline T&  CIptMemBlock<T>::operator[](int pos)
{
    return at(pos);
}


template <class T>
inline bool CIptMemBlock<T>::isValid(int iRow, int iCol)  const
{
    return (iRow>=0 && iRow<m_nRows && iCol>=0 && iCol<m_nCols);
}


template <class T>
inline bool CIptMemBlock<T>::isValid(const IPTPIXEL& pxl)  const
{
    return isValid(pxl.iRow, pxl.iCol);
}


template <class T>
inline T * const CIptMemBlock<T>::getRowAddress(int iRow) const
{
    assert(iRow<m_nRows && iRow>=0);

    return m_pMemData+iRow*m_nCols;
}


template <class T>
inline typename CIptMemBlock<T>::iterator CIptMemBlock<T>::begin()
{
#ifdef NDEBUG
    return iterator(m_pMemData);
#else
    return iterator(0, this);
#endif
}


template <class T>
inline typename CIptMemBlock<T>::iterator CIptMemBlock<T>::end()
{
#ifdef NDEBUG
    return iterator(m_pMemData + m_nRows*m_nCols);
#else
    return iterator(m_nRows*m_nCols, this);
#endif
}


template <class T>
inline typename CIptMemBlock<T>::const_iterator CIptMemBlock<T>::begin() const
{
#ifdef NDEBUG
    return const_iterator(m_pMemData);
#else
    return const_iterator(0, this);
#endif
}


template <class T>
inline typename CIptMemBlock<T>::const_iterator CIptMemBlock<T>::end() const
{
#ifdef NDEBUG
    return const_iterator(m_pMemData + m_nRows*m_nCols);
#else
    return const_iterator(m_nRows*m_nCols, this);
#endif
}


template <class T>
inline typename CIptMemBlock<T>::blk_iterator CIptMemBlock<T>::blk_begin()
{
#ifdef NDEBUG
    if(0 == m_blkiter)
    {
        m_blkiter = new T*[m_nRows];
        assert(m_blkiter);

        for(int iRow=0; iRow<m_nRows; iRow++)
        {
            m_blkiter[iRow] = getRowAddress(iRow);
        }

    }    
    return m_blkiter;

#else
    return blk_iterator(this);
#endif // NDEBUG 
}


template <class T>
inline typename CIptMemBlock<T>::const_blk_iterator CIptMemBlock<T>::blk_begin() const
{
#ifdef NDEBUG
    const T ** blkiter = new T*[m_nRows];
    for(int iRow=0; iRow<m_nRows; iRow++)
    {
        blkiter[iRow] = getRowAddress(iRow);
    }
    return blkiter;
#else
    return const_blk_iterator(this);
#endif // NDEBUG 
}


template <class T>
inline typename CIptMemBlock<T>::row_iterator CIptMemBlock<T>::row_begin(int iRow)
{
#ifdef NDEBUG
    return getRowAddress(iRow);
#else
    return row_iterator(iRow, this);
#endif // NDEBUG 
}


template <class T>
inline typename CIptMemBlock<T>::const_row_iterator CIptMemBlock<T>::row_begin(int iRow) const
{
#ifdef NDEBUG
    return getRowAddress(iRow);
#else
    return const_row_iterator(iRow, this);
#endif // NDEBUG 
}


template <class T>
inline bool CIptMemBlock<T>::crop(int rowTop, int colLeft, int rowBottom, int colRight)
{
    CIptMemBlock<T>* pMem = subBlock(rowTop, colLeft, rowBottom, colRight);
    if(0 != pMem)
    {
        delete m_pMemData;

        m_pMemData = pMem;
        m_nRows = rowBottom - rowTop + 1;
        m_nCols = colRight - colLeft + 1;

        return true;
    }

    return false;
}


template <class T>
inline bool CIptMemBlock<T>::crop(const CIptMemBlock<T>& MemBlkSrc, int rowTop, int colLeft, int rowBottom, int colRight)
{
    CIptMemBlock<T>* pMem = MemBlkSrc.subBlock(rowTop, colLeft, rowBottom, colRight);
    if(0 != pMem)
    {
        clear();

        m_pMemData = pMem;
        m_nRows = rowBottom - rowTop + 1;
        m_nCols = colRight - colLeft + 1;

        return true;
    }

    return false;
}


template <class T>
inline CIptMemBlock<T>*  CIptMemBlock<T>::subBlock(int rowTop, int colLeft, int rowBottom, int colRight)
{
    assert(rowTop>=0 && colLeft>=0);
    assert(colRight<m_nCols && rowBottom<m_nRows);

    int nNewRows = rowBottom - rowTop + 1;
    int nNewCols = colRight - colLeft + 1;
    assert(nNewRows>0 && nNewCols>0);

    CIptMemBlock<T> *   pMemBlk = new CIptMemBlock<T>;
    if(0 != pMemBlk)
    {
        if(pMemBlk->create(nNewRows, nNewCols))
        {
            CIptMemBlock<T>::iterator   iterDest = pMemBlk->begin();
            for(int iRow=rowTop; iRow<=rowBottom; iRow++)
            {
                CIptMemBlock<T>::const_row_iterator   iterSrce = const_row_begin(iRow);

                for(int iCol=colLeft; iCol<=colRight; iCol++)
                    *(iterDest++) = iterSrce[iCol];
                
            } //for(int iRow=rowTop; iRow<=rowBottom; iRow++)
        } //pMemBlk->create(nNewRows, nNewCols)
    }

    return pMemBlk;
}


template <class T>
inline void CIptMemBlock<T>::clear()
{
    if(!m_blAttached && 0!=m_pMemData)
    {
        delete m_pMemData;
    }

    if(0 != m_blkiter)
    {
        delete m_blkiter;
        m_blkiter = 0;
    }

    init();
}


/**
 * private memebers are firstly initilized when class is constructed
 */
template <class T>
inline void CIptMemBlock<T>::init()
{
    m_blkiter  = 0;
    m_pMemData = 0;
    m_nRows    = 0;
    m_nCols    = 0;
    m_blAttached = false;
}

//--------------------------------------------------------------------------
template <class T, int N>
inline CIptMulImage<T, N>::CIptMulImage()
{
}


template <class T, int N>
inline bool CIptMulImage<T, N>::create(int nWidth, int nHeight)
{
    bool bRet = true;

    for(int i=0; i<N; i++)
    {
        bRet |= m_Image[i].create(nWidth, nHeight);
    }

    return bRet;
}


template <class T, int N>
inline int CIptMulImage<T, N>::getWidth(int iChannel) const
{
    assert(iChannel<N && iChannel>=0);

    return (m_Image[iChannel]).getWidth();
}


template <class T, int N>
inline int CIptMulImage<T, N>::getHeight(int iChannel) const
{
    assert(iChannel<N && iChannel>=0);

    return m_Image[iChannel].getHeight();
}


template <class T, int N>
inline int CIptMulImage<T, N>::getSize(int iChannel) const
{
    assert(iChannel<N && iChannel>=0);

    return m_Image[iChannel].getSize();
}


template <class T, int N>
inline void CIptMulImage<T, N>::clear()
{
    for(int i=0; i<N; i++)
        m_Image[i].clear();

    return;
}


template <class T, int N>
inline typename T::iterator CIptMulImage<T, N>::begin(int iChannel)
{
    assert(iChannel>=0 && iChannel<N);

    return m_Image[iChannel].begin();
}


template <class T, int N>
inline typename T::const_iterator CIptMulImage<T, N>::const_begin(int iChannel)
{
    assert(iChannel>=0 && iChannel<N);

    return m_Image[iChannel].const_begin();
}


template <class T, int N>
inline typename T::iterator CIptMulImage<T, N>::end(int iChannel)
{
    assert(iChannel>=0 && iChannel<N);

    return m_Image[iChannel].end();
}


template <class T, int N>
inline typename T::const_iterator CIptMulImage<T, N>::const_end(int iChannel)
{
    assert(iChannel>=0 && iChannel<N);

    return m_Image[iChannel].const_end();
}


template <class T, int N>
inline typename T::blk_iterator CIptMulImage<T, N>::blk_begin(int iChannel)
{
    assert(iChannel>=0 && iChannel<N);

    return m_Image[iChannel].blk_begin();
}


template <class T, int N>
inline typename T::const_blk_iterator CIptMulImage<T, N>::const_blk_begin(int iChannel)
{
    assert(iChannel>=0 && iChannel<N);

    return m_Image[iChannel].const_blk_begin();
}

template <class T, int N>
inline T* CIptMulImage<T, N>::getSingleChannelImage(int iChannel)
{
    assert(iChannel>=0 && iChannel<N);

    return m_Image+iChannel;
}

} // namespace ipt

#endif //ifndef _IPT_IMAGECORE_HPP