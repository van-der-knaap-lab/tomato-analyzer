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
#ifndef _IPT_IMAGEIO_H
#define _IPT_IMAGEIO_H

namespace ipt {

    class CIptImageFileHandler {
    public:
        bool loadImageFile(const char * szFileName);
        bool saveImageFile(const CIptColorImage * pImage, const char * szFileName);
        bool saveImageFile(const char * szSrcFileName, const char * szDstFileName);
    }; // class CIptImageFileHandler


    class CIptImageIO {
    public:
        CIptImageIO();
        virtual ~CIptImageIO();

        virtual bool LoadImage(const char * szFileName);
        virtual bool LoadImage(FILE * pFile);

        virtual bool SaveImage(CIptColorImage * pImage, const char * szFileName);

        virtual CIptColorImage *    CreateIptColorImage(void);
    }; // class CIptImageIO 

}; // namespace ipt

#endif // _IPT_IMAGEIO_H