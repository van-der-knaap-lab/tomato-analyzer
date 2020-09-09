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
#ifndef _IPT_IMAGECORE_H
#define _IPT_IMAGECORE_H

#include <stdlib.h>
#include <stdio.h>

#include "IptAssert.h"
#include "ImgTool.h"

namespace ipt {

template <class T>
class CIptMemBlock {
public:
    typedef T  Data_Type;

    #ifdef NDEBUG
        typedef T*          iterator;
        typedef const T*    const_iterator;

        typedef T* const *         blk_iterator;
        typedef const T * const *  const_blk_iterator;

        typedef T *         row_iterator;
        typedef const T*    const_row_iterator;
    #else

        class const_iterator;
        class row_iterator;
        class const_row_iterator;

        class iterator {
            friend class CIptMemBlock<T>;

            #ifdef _MSC_VER
                friend class const_iterator;
            #else
                friend class CIptMemBlock<T>::const_iterator;
            #endif

        public:
            /**
             * default constructor
             */
            inline iterator() : pos(0), m_MemBlk(0){
            };

            /**
             * copy constructor
             */
            inline iterator(const iterator& cp)
                : pos(cp.pos),m_MemBlk(cp.m_MemBlk) {
            };

            /**
             * copy member by operator =
             */
            inline iterator& operator=(const iterator& other) {
                pos = other.pos;
                m_MemBlk = other.m_MemBlk;
                return *this;
            };

            /**
             * get pointed data
             */
            inline T& operator*() {
                return m_MemBlk->at(pos);
            };

            /**
             * Immediately next, prefix
             */
            inline iterator& operator++() {
                ++pos; 
                return *this;
            };

            /**
             * Immediately previous, prefix
             */
            inline iterator& operator--() {
                --pos; 
                return *this;
            };

            /**
             * Advance n elements (L-value is returned)
             */
            inline iterator& operator+=(const int n) {
                pos+=n; 
                return *this;
            };

            /**
             * Back n element (L-value is returned)
             */
            inline iterator& operator-=(const int n) {
                pos-=n; 
                return *this;
            };

            /**
             * Advance n elements
             */
            inline iterator operator+(const int n) {
                return iterator(pos+n,m_MemBlk);
            };

            /**
             * Back n elements
             */
            inline iterator operator-(const int n) {
                return iterator(pos-n,m_MemBlk);
            };

            /**
             * Immediately next, postfix
             */
            inline iterator operator++(int) {
                iterator tmp(*this); 
                pos++; 
                return tmp;
            };

            /**
             * Immediately previous, postfix
             */
            inline iterator operator--(int) {
                iterator tmp(*this); 
                pos--; 
                return tmp;
            };

            /**
             * Check the euqality of position in memory block
             */
            inline bool operator==(const iterator& other) const {
                return pos==other.pos;
            };

            /**
             * Check the euqality of position in memory block
             */
            inline bool operator!=(const iterator& other) const {
                return pos!=other.pos;
            };

            /**
             * compare if the position of the first iterator is 
             * ahead of the second or not
             */
            inline bool operator<(const iterator& other) const {
                return pos<other.pos;
            };

            /**
             * compare if the position of the first iterator is 
             * behind the second or not
             */
            inline bool operator>(const iterator& other) const {
                return pos>other.pos;
            };

            /**
             * compare if the position of the first iterator is 
             * ahead of or equal to the position of the second
             */
            inline bool operator<=(const iterator& other) const {
                return pos<=other.pos;
            };

            /**
             * compare if the position of the first iterator is
             * behind or equal to the position of the second 
             */
            inline bool operator>=(const iterator& other) const {
                return pos>=other.pos;
            };

        protected:
            inline const int& getPos() const {
                return pos;
            };

            inline const CIptMemBlock<T>* getMemBlk() const {
                return m_MemBlk;
            };

            explicit iterator(const int& startPos, CIptMemBlock<T>* vct)
                : pos(startPos), m_MemBlk(vct) {};

        private:
            /**
             * actual data index
             */
            int pos;

            /**
             * pointer to the actual memory block
             */
            CIptMemBlock<T>* m_MemBlk;
        }; // class iterator

        
        /**
         * const iterator type (allows read-only operations).
         */
        class const_iterator {
            friend class CIptMemBlock<T>;
        
        public:
            /**
             * default constructor
             */
            inline const_iterator() : pos(0), m_MemBlk(0) {
            };

            /**
             * copy constructor
             */
            inline const_iterator(const const_iterator& other) {
                (*this)=other;
            };

            /**
             * copy constructor
             */
            inline const_iterator(const iterator& other) {
                (*this)=other;
            };

            /**
             * copy member
             */
            inline const_iterator& operator=(const iterator& other) {
                pos = other.getPos();
                m_MemBlk = other.getMemBlk();
                return *this;
            };

            /**
             * copy member
             */
            inline const_iterator& operator=(const const_iterator& other) {
                pos = other.pos;
                m_MemBlk = other.m_MemBlk;
                return *this;
            };

            /**
             * Immediately next, prefix
             */
            inline const_iterator& operator++() {
                ++pos; 
                return *this;
            };

            /**
             * Immediately previous, prefix
             */
            inline const_iterator& operator--() {
                --pos; 
                return *this;
            };

            /**
             * advance to next item  -- postfix
             */
            inline const_iterator operator++(int) {
                const_iterator tmp(*this); 
                pos++; 
                return tmp;
            };

            /**
             * recede to previous item  -- postfix
             */
            inline const_iterator operator--(int) {
                const_iterator tmp(*this); 
                pos--; 
                return tmp;
            };

            /**
             * Advance n elements (L-value is returned)
             */
            inline const_iterator& operator+=(const int n) {
                pos+=n; 
                return *this;
            };

            /**
             * Back n elements (L-value is returned)
             */
            inline const_iterator& operator-=(const int n) {
                pos-=n; 
                return *this;
            };

            /**
             * Advance n elements
             */
            inline const_iterator operator+(const int n) {
                return const_iterator(pos+n, m_MemBlk);
            };

            /**
             * Back n elements
             */
            inline const_iterator operator-(const int n) {
                return const_iterator(pos-n, m_MemBlk);
            };

            /**
             * compare
             */
            inline bool operator==(const const_iterator& other) const {
                return (pos == other.pos);
            };

            /**
             * compare
             */
            inline bool operator!=(const const_iterator& other) const {
                return pos!=other.pos;
            };

            /**
             * compare
             */
            inline bool operator==(const iterator& other) const {
                return pos==other.getPos();
            };

            /**
             * compare
             */
            inline bool operator!=(const iterator& other) const {
                return pos!=other.getPos();
            };

            /**
             * compare if the position of the first iterator is smaller than
             * the position of the second iterator
             */
            inline bool operator<(const iterator& other) const {
                return pos<other.pos;
            };

            /**
             * compare if the position of the first iterator is greater than
             * the position of the second iterator
             */
            inline bool operator>(const iterator& other) const {
                return pos>other.pos;
            };

            /**
             * compare if the position of the first iterator is smaller or equal than
             * the position of the second iterator
             */
            inline bool operator<=(const iterator& other) const {
                return pos<=other.pos;
            };

            /**
             * compare if the position of the first iterator is greater or equal
             * than the position of the second iterator
             */
            inline bool operator>=(const iterator& other) const {
                return pos>=other.pos;
            };

            /**
             * compare if the position of the first iterator is smaller than
             * the position of the second iterator
             */
            inline bool operator<(const const_iterator& other) const {
                return pos<other.pos;
            };

            /**
             * compare if the position of the first iterator is greater than
             * the position of the second iterator
             */
            inline bool operator>(const const_iterator& other) const {
                return pos>other.pos;
            };

            /**
             * compare if the position of the first iterator is smaller or equal than
             * the position of the second iterator
             */
            inline bool operator<=(const const_iterator& other) const {
                return pos<=other.pos;
            };

            /**
             * compare if the position of the first iterator is greater or equal
             * than the position of the second iterator
             */
            inline bool operator>=(const const_iterator& other) const {
                return pos>=other.pos;
            };

            /**
             * get pointed data
             */
            const T& operator*() {
                return m_MemBlk->at(pos);
            };

            protected:
            explicit const_iterator(const int startPos, const CIptMemBlock<T>* vct)
                : pos(startPos), m_MemBlk(vct) {};

            private:
            /**
             * actual matrix index
             */
            int pos;

            /**
             * pointer to the actual matrix
             */
            const CIptMemBlock<T>* m_MemBlk;
        }; // class const_iterator


        /**
         * block iterator
         */
        class blk_iterator {
            friend class CIptMemBlock<T>;

        public:
            /**
             * default constructor
             */
            inline blk_iterator() : m_rows(0), m_pRowIterator(0){
            };

            /**
             * destructor
             */
            ~blk_iterator(){
                if(0 != m_pRowIterator)
                    delete [] m_pRowIterator;
            };

            /**
             * copy constructor
             */
            inline blk_iterator(const blk_iterator& cp)
                : m_rows(cp.m_rows),m_pRowIterator(cp.m_pRowIterator) {
            };

            /**
             * copy member by operator =
             */
            inline blk_iterator& operator=(const blk_iterator& other) {
                m_rows = other.m_rows;
                m_pRowIterator = other.m_pRowIterator;
                return *this;
            };

       public:
            inline row_iterator& operator[](int iN) const {
                assert(iN<m_rows && iN>=0);
                
                return m_pRowIterator[iN];
            };

            int getRowNumber(void) const {
                return m_rows;
            };

        private:
            explicit inline blk_iterator(CIptMemBlock<T> * pBlk){
                m_rows = pBlk->rows();
                m_pRowIterator = new row_iterator[m_rows];
                if(0 != m_pRowIterator)
                {
                    for(int iRow=0; iRow<m_rows; iRow++)
                        m_pRowIterator[iRow].set(iRow, pBlk);
                }
            };

        private:
            int m_rows;
            row_iterator * m_pRowIterator;
        }; // class blk_iterator


        class const_blk_iterator {
            friend class CIptMemBlock<T>;

        public:
            inline const_row_iterator& operator[](int iN){
                assert(iN<m_rows && iN>=0);
                
                return m_pRowIterator[iN];
            };

            /**
             * default constructor
             */
            inline const_blk_iterator(){
                m_rows = 0;
                m_pRowIterator = 0;
            };

            /**
             * destructor
             */
            ~const_blk_iterator(){
                if(0 != m_pRowIterator)
                    delete [] m_pRowIterator;
            };

            /**
             * Copy member
             */
            inline const_blk_iterator(const_blk_iterator& cpy){
                (*this) = cpy;
            };

            /**
             * Copy member
             */
            inline const_blk_iterator(const blk_iterator& cpy){
                (*this) = cpy;
            };

            /**
             * Copy member
             */
            inline const_blk_iterator& operator=(const_blk_iterator& other){
                m_rows = other.m_rows;
                m_pRowIterator = new const_row_iterator[m_rows];
                if(0 != m_pRowIterator)
                {
                    for(int iRow=0; iRow<m_rows; iRow++)
                        m_pRowIterator[iRow] = (other.m_pRowIterator)[iRow];
                }
            };

            /**
             * Copy member
             */
            inline const_blk_iterator& operator=(const blk_iterator& other){
                m_rows = other.getRowNumber();
                m_pRowIterator = new const_row_iterator[m_rows];
                if(0 != m_pRowIterator)
                {
                    for(int iRow=0; iRow<m_rows; iRow++)
                        m_pRowIterator[iRow] = other[iRow];
                }

                return *this;
            };

        private:
            explicit inline const_blk_iterator(CIptMemBlock<T> * pBlk){
                m_rows = pBlk->getNumOfRows();
                m_pRowIterator = new const_row_iterator[m_rows];
                if(0 != m_pRowIterator)
                {
                    for(int iRow=0; iRow<m_rows; iRow++)
                        m_pRowIterator[i].set(iRow, pBlk);
                }
            };

        private:
            int m_rows;
            const_row_iterator * m_pRowIterator;
        }; // class const_blk_iterator

        
        /**
         * row iterator
         */
        class row_iterator {
            friend class CIptMemBlock<T>;
            friend class blk_iterator;

        public:
            /**
             * default constructor
             */
            inline row_iterator() : m_iRow(0), m_pMemBlk(0) {
            };

            inline T& operator[](int iCol){
                return m_pMemBlk->at(m_iRow, iCol);
            };

            inline int getRowIndex(void) const {
                return m_iRow;
            };

            inline CIptMemBlock<T> *  getMemBlk(void) const {
                return m_pMemBlk;
            };

        private:
            inline row_iterator(int iRow, CIptMemBlock<T> * pMemBlk)
                : m_iRow(iRow), m_pMemBlk(pMemBlk) {
            };

            inline void set(int iRow, CIptMemBlock<T> * pMemBlk){
                m_iRow = iRow;
                m_pMemBlk = pMemBlk;
            };

        private:
            int m_iRow;
            CIptMemBlock<T> *   m_pMemBlk;
        }; // class row_iterator


       /**
        * const row iterator
        */
       class const_row_iterator {
            friend class CIptMemBlock<T>;
            friend class const_blk_iterator;

       public:
            /**
             * Default constructor
             */
            inline const_row_iterator(){
                m_iRow = 0;
                m_pMemBlk = 0;
            };

            /**
             * Copy member
             */
            inline const_row_iterator(const row_iterator& cpy){
                (*this) = cpy;
            };

            /**
             * Copy member
             */
            inline const_row_iterator(const_row_iterator& cpy){
                (*this) = cpy;
            };

            /**
             * Copy member
             */
            inline const_row_iterator& operator = (const_row_iterator& other){
                m_iRow = other.m_iRow;
                m_pMemBlk = other.m_pMemBlk;

                return *this;
            };

            /**
             * Copy member
             */
            inline const_row_iterator& operator = (const row_iterator& other){
                m_iRow = other.getRowIndex();
                m_pMemBlk = other.getMemBlk();

                return *this;
            };

       public:
            inline const T& operator[](int iCol){
                return m_pMemBlk->at(m_iRow, iCol);
            };
            
       private:
            explicit const_row_iterator(int iRow, const CIptMemBlock<T> * pMemBlk)
                : m_iRow(iRow), m_pMemBlk(pMemBlk) {
            };

            inline void set(int iRow, const CIptMemBlock<T> * pMemBlk){
                m_iRow = iRow;
                m_pMemBlk = pMemBlk;
            };

        private:
            int m_iRow;
            const CIptMemBlock<T> *   m_pMemBlk;
        }; // class row_iterator

    #endif // NDEBUG

public:
    /**
     * constructors
     */
    CIptMemBlock();
    CIptMemBlock(int nRows, int nCols);
    CIptMemBlock(int nRows, int nCols, T tinit);
    CIptMemBlock(int nRows, int nCols, const T * pData);

    /**
     * Destructor
     */
    virtual ~CIptMemBlock();

    /**
     * Copy member functions
     */
    CIptMemBlock(const CIptMemBlock& other);
    CIptMemBlock<T>& operator=(const CIptMemBlock& other);

    /**
     * copy function
     */
    CIptMemBlock<T>&  copy(const CIptMemBlock& other);

    /**
     * attach memory
     * the memory would not be deleted in the destructor
     */
    void attach(T* pMem, int nRows, int nCols);
    void attach(const CIptMemBlock<T>& pIptMemBlk);

    /**
     * set each uint's value to be the specified input
     */
    void fill(T  value);

    /**
     * get the dimentionality of the memory block
     */
    int  size(void)    const;
    int  rows(void)    const;
    int  columns(void) const;
    IPTPIXEL dim(void) const;

    /**
     * resize the memory space
     * the new size should be the same as the old one's
     */
    bool resize(int nNewRows, int nNewCols);

    /**
     * create a continous memory block for use,
     * initialized if second function is called
     */
    bool create(int nRows, int nCols);
    bool create(int nRows, int nCols, T vinit);

    /**
     * release memory block
     * reset the "attached" to be false
     */
    void clear(void);

    /**
     * obtaining value of the speicified unit 
     */
    Data_Type&          at(const int pos);
    Data_Type&          at(const IPTPOINT& pt);
    Data_Type&          at(const IPTPIXEL& pxl);
    Data_Type&          at(const int iRow, const int iCol);
    const Data_Type&    at(const int pos) const;
    const Data_Type&    at(const IPTPOINT& pt) const;
    const Data_Type&    at(const IPTPIXEL& pxl) const;
    const Data_Type&    at(const int iRow, const int iCol) const;

    Data_Type&  operator[](int pos);
   
    /**
     * validation of index
     */
    bool isValid(int iRow, int iCol)  const;
    bool isValid(const IPTPIXEL& pxl) const;

    
    /**
     * make this memblock contain only a subblock of its orinigal spaces
     */
    bool  crop(int rowTop, int colLeft, int rowBottom, int colRight);
    
    /**
     * make this memblock contain the same data as the specified subblock in MemBlkSrc
     */
    bool  crop(const CIptMemBlock<T>& MemBlkSrc, int rowTop, int colLeft, int rowBottom, int colRight);
    
    /**
     * getting different iterators
     */
    iterator             begin();
    const_iterator       begin() const;
    iterator             end();
    const_iterator       end() const;

    blk_iterator         blk_begin();
    const_blk_iterator   blk_begin() const;

    row_iterator         row_begin(int iRow);
    const_row_iterator   row_begin(int iRow) const;

    Data_Type * getData(void) const;
    Data_Type * getDataByRow(int iRow) const;

private:
    void init(void);
    Data_Type * const getRowAddress(int iRow) const;
    CIptMemBlock<T>*  subBlock(int posLeft, int posTop, int posRight, int posBottom);

private:
    /**
     * image width and height
     */
    int  m_nRows, m_nCols;

    /**
     * contained image data
     */
    Data_Type *  m_pMemData;
    Data_Type ** m_blkiter;

    /**
     * indicate whether the data is deleted or not with 
     * the existance of class instance
     * if the data are obtained via attach member function
     * the data would be left intact
     * otherwise, the image data space will be reclaimed
     */
    bool m_blAttached;
}; // class CIptMemBlock


/*------------------------------------------------------------------------------*/
template <class T>
class CIptImage : public CIptMemBlock<T> {
public:
    CIptImage();
    CIptImage(const CIptImage<T>& other);
    CIptImage(const CIptMemBlock<T>& other);

    CIptImage<T>&  operator=(const CIptImage<T>& other);

    CIptImage<T>&  copy(const CIptMemBlock<T>& other);
    CIptImage<T>&  copy(const CIptImage<T>& other);

    int width(void)  const;
    int height(void) const;
    
public:
    bool createImg(int nWidth, int nHeight);


    T&  neighbor(int);
    const T neighbor(int) const;

    /**
     * loggical operation
     */
    CIptImage<T>& operator & (const CIptImage<T>& other);
    CIptImage<T>& operator | (const CIptImage<T>& other);
    CIptImage<T>& operator ! ();
    CIptImage<T>& operator ^ (const CIptImage<T>& other);
    CIptImage<T>& xor (const CIptImage<T>& A, const CIptImage<T>& B);


    void rotate(double dbAngle);
    void resize(int nNewWidth, int nNewHeight, const char * szMethod);
    void resize(double dbScale, const char * szMethod);
    void crop(int nLeft, int nTop, int nRight, int nBottom);

    CIptImage<T>* subImage(int nLeft, int nTop, int nRight, int nBottom);
    CIptImage<T>* resizeNew(int nWidth, int nHeight, const char * szMethod);
    CIptImage<T>* resizeNew(double dbScale, const char * szMethod);
}; // class CIptImage


//----------------------------------------------------------------------------------
template <class T, int N>
    class CIptMulImage {
    public:
        CIptMulImage();

        bool create(int nWidth, int nHeight);

        int  getWidth(int iChannel=0)  const;
        int  getHeight(int iChannel=0) const;
        int  getSize(int iChannel=0)   const;

        typename T::iterator        begin(int iChannel=0);
        typename T::const_iterator  const_begin(int iChannel=0);
        typename T::iterator        end(int iChannel=0);
        typename T::const_iterator  const_end(int iChannel=0);

        typename T::blk_iterator        blk_begin(int iChannel=0);
        typename T::const_blk_iterator  const_blk_begin(int iChannel=0);

        T*    getSingleChannelImage(int iChannel);

    private:
        void clear();

    private:
        T  m_Image[N];
}; // class CIptMulImage 


typedef CIptMemBlock<BYTE>::iterator            Pixel_iterator;
typedef CIptMemBlock<BYTE>::blk_iterator        Pixel_blk_iterator;
typedef CIptMemBlock<BYTE>::row_iterator        Pixel_row_iterator;
typedef CIptMemBlock<BYTE>::const_iterator      Const_Pixel_iterator;
typedef CIptMemBlock<BYTE>::const_row_iterator  Const_Pixel_row_iterator;
typedef CIptMemBlock<BYTE>::const_blk_iterator  Const_Pixel_blk_iterator;



} // namespace ipt

#include "GenericImg.hpp"
#include "ImageCore.hpp"

#endif //_IPT_IMAGECORE_H