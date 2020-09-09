#include "GrayImage.h"
#include "ColorImage.h"
#include "Bitmap.h"

using namespace ipt;

void main()
{
    printf("ok\n");
 //   CIptGrayImage im;
 //   bool blRet = im.create(128, 256);

    //BYTE * pd = im.getData();
    //CIptMemBlock<BYTE>::iterator iter=im.begin();
//    CIptGrayImage::Pixel_Iterator iter=im.begin();
 //   CIptGrayImage::iterator iter=im.begin();


//    int  nH = im.getWidth();

    CIptBitmap  cbmp;
    cbmp.LoadImage("C:\\test.bmp");
    cbmp.CreateIptColorImage();

}