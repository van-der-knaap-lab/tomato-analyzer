#ifndef _IPT_GENERICIMG_HPP
#define _IPT_GENERICIMG_HPP

namespace ipt{

template <class T>
inline CIptImage<T>::CIptImage()
{
}


template <class T>
inline CIptImage<T>::CIptImage(const CIptMemBlock<T>& other)
         : CIptMemBlock<T>(other)
{
}


template <class T>
inline CIptImage<T>::CIptImage(const CIptImage<T>& other)
{
    copy(other);
}


template <class T>
inline CIptImage<T>& CIptImage<T>::operator=(const CIptImage<T>& other)
{
    copy(other);
    return *this;
}


template <class T>
inline CIptImage<T>& CIptImage<T>::copy(const CIptImage<T>& other)
{
    CIptMemBlock<T>::copy(other);
    return *this;
}


template <class T>
inline CIptImage<T>& CIptImage<T>::copy(const CIptMemBlock<T>& other)
{
    CIptMemBlock<T>::copy(other);
    return *this;
}


template <class T>
inline int CIptImage<T>::width() const
{
    return columns();
}


template <class T>
inline int CIptImage<T>::height() const
{
    return rows();
}


template <class T>
inline bool CIptImage<T>::createImg(int nWidth, int nHeight)
{
    return create(nHeight, nWidth);
}


template <class T>
inline void CIptImage<T>::crop(int nLeft, int nTop, int nRight, int nBottom)
{
    CIptMemBlock<T>::crop(nTop, nLeft, nBottom, nRight);
}


template <class T>
inline CIptImage<T>* CIptImage<T>::subImage(int nLeft, int nTop, int nRight, int nBottom)
{
    CIptImage<T>*  pImage = new CIptImage<T>;
    if(pImage)
    {
        pImage->crop(*this, nTop, nLeft, nBottom, nRight);
    }

    return pImage;
}


template <class T>
inline T& CIptImage<T>::neighbor(int iDir)
{
    return at(0);
}


template <class T>
inline const T CIptImage<T>::neighbor(int iDir) const
{
    return at(0);
}


template <class T>
inline CIptImage<T>& CIptImage<T>::operator & (const CIptImage<T>& other)
{
    iterator itr = begin();
    iterator itrEnd = end();
    const_iterator itrOther = other.begin();
    
    while(itr != itrEnd)
    {
        *itr &= *itrOther;

        ++itr;
        ++itrOther;
    }

    return *this;
}


template <class T>
inline CIptImage<T>& CIptImage<T>::operator | (const CIptImage<T>& other)
{
    iterator itr = begin();
    iterator itrEnd = end();
    const_iterator itrOther = other.begin();
    
    while(itr != itrEnd)
    {
        *itr |= *itrOther;

        ++itr;
        ++itrOther;
    }

    return *this;
}


template <class T>
inline CIptImage<T>& CIptImage<T>::operator ! ()
{
    iterator itr = begin();
    iterator itrEnd = end();
    
    while(itr != itrEnd)
    {
        *itr = !(*itr);
        ++itr;
    }

    return *this;
}


template <class T>
inline CIptImage<T>& CIptImage<T>::xor (const CIptImage<T>& A, const CIptImage<T>& B)
{
    assert(A.dim()==B.dim());
    
    if(create(A.rows(), A.columns()))
    {
        iterator iter = begin();
        iterator iter_end = end();
    
        const_iterator  iterA = A.begin();
        const_iterator  iterB = B.begin();
    
        while(iter != iter_end)
        {
            *iter = (*iterA) ^ (*iterB);

            ++iter;
            ++iterA;
            ++iterB;
        }
    }
    return *this;
}


} //namespace ipt

#endif //_IPT_GENERICIMG_HPP