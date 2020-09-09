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

#pragma warning(disable:4099)

#ifndef _CVIP_DECLS_H_
#define _CVIP_DECLS_H_

// File: CVIPdecls.h
// Author: Yusaku Sako
// Date: 07/11/99

// These declarations are needed to use CVIP tools under C++.

/* Modified by Jaymie Strecker, 4/23/2010 to work with updated CVIP tools. 
 * Most declarations in here are now redundant, being defined in other CVIP
 * header files. 
 */

#include <ColorMap.h>

extern "C" {
/*
   #define MAX(a, b)  (((a) > (b)) ? (a) : (b)) 
   #define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 
 

 
	
   typedef enum {CVIP_NO, CVIP_YES} CVIP_BOOLEAN;
   typedef enum {REAL, COMPLEX} FORMAT;
   typedef enum {OFF, ON} STATE;
   typedef enum {CVIP_BYTE, CVIP_SHORT, CVIP_INTEGER, CVIP_FLOAT, CVIP_DOUBLE} CVIP_TYPE;

// Matrix
   typedef struct {
      CVIP_TYPE data_type;
      FORMAT data_format;
      unsigned int rows;
      unsigned int cols;
      char **rptr;
      char **iptr;
   } MATRIX;
*/
   typedef MATRIX Matrix;
   /* macros used to extract information from the Image structure */
   #define getNoOfRows_Matrix(matrix) ((matrix)->rows)
   #define getNoOfCols_Matrix(matrix) ((matrix)->cols)
   #define getDataType_Matrix(matrix) ((matrix)->data_type)
   #define getDataFormat_Matrix(matrix) ((matrix)->data_format)

   #define getRealData_Matrix(matrix) ((void *)((matrix)->rptr))
   #define getData_Matrix(matrix) getRealData_Matrix(matrix)
   #define getImagData_Matrix(matrix) ((void *)((matrix)->iptr))


   #define getRealRow_Matrix(matrix,row) ((void *)((matrix)->rptr[row]))
   #define getRow_Matrix(matrix,row) getRealRow_Matrix(matrix,row)
   #define getImagRow_Matrix(matrix,row) ((void *)((matrix)->iptr[row]))

   /* Type Declarations for functions contained in CVIPtools matrix
      library - libmatrix.a ( Prototype form ) */
      
   #define matrix_allocate(a,b,c,d) new_Matrix(a,b,c,d) 
   #define matrix_makecomplex(a,b) makeComplex_Matrix(a,b)
   #define matrix_makereal(a,b) makeReal_Matrix(a,b)
   #define matrix_free(a)  delete_Matrix(a)
   #define matrix_print(a) print_Matrix(a)
   #define matrix_scale(a,b) scale_Matrix(a,b)
   #define matrix_crop(a,b,c,d,e) crop_Matrix(a,b,c,d,e)
   #define matrix_add(a,b) add_Matrix(a,b)
   #define matrix_sub(a,b)  sub_Matrix(a,b)
   #define matrix_mult_pwise(a,b) multPWise_Matrix(a,b)
   #define matrix_mult(a,b) mult_Matrix(a,b)
   #define matrix_transpose(a)  transpose_Matrix(a)
   #define matrix_invert(a) invert_Matrix(a)
   #define rect2pol(a,b)  rect2pol_Matrix(a,b)
   #define matrix_det(a) det_Matrix(a)
   #define matrix_copy(a,b,c) copy_Matrix(a,b,c)
   #define matrix_fcopy(a,b) fastCopy_Matrix(a,b)
   #define matrix_dup(a,b) duplicate_Matrix(a,b)
/*
   Matrix *new_Matrix(unsigned int rows, unsigned int cols, CVIP_TYPE data_type, FORMAT data_format);
   CVIP_BOOLEAN makeComplex_Matrix(Matrix *real_Matrix, CVIP_BOOLEAN verbose);
   CVIP_BOOLEAN makeReal_Matrix(Matrix *complex_Matrix, CVIP_BOOLEAN verbose);
   void delete_Matrix(Matrix *A);
   void print_Matrix(const Matrix *A);
   Matrix *scale_Matrix(const Matrix *a, double s);
   Matrix *crop_Matrix(const Matrix *a, unsigned int, unsigned int, unsigned int, unsigned int);
   Matrix *add_Matrix(const Matrix *A,const Matrix *B);
   Matrix *sub_Matrix(const Matrix *A,const Matrix *B);
   Matrix *multPWise_Matrix(const Matrix *A, const Matrix *B);
   Matrix *mult_Matrix(const Matrix *A,const Matrix *B);
   Matrix *transpose_Matrix(const Matrix *A);
   Matrix *invert_Matrix(const Matrix *A);
   Matrix *rect2pol_Matrix(Matrix *A, int dir);
   double det_Matrix(const Matrix *A);
   CVIP_BOOLEAN copy_Matrix(const Matrix *A, Matrix *B);
   CVIP_BOOLEAN fastCopy_Matrix(const Matrix *A, Matrix *B);
   Matrix *duplicate_Matrix(const Matrix *A, FORMAT df);
   Matrix *and_Matrix( const Matrix *A, const Matrix *B);
   int recip_Matrix(Matrix **rP, const Matrix *aP, const Matrix *bP);
   int divide_Matrix(Matrix **dP, const Matrix *aP, const Matrix	*bP, const Matrix *cP);
   int abs_Matrix(Matrix	*matP);
   Matrix *clone_Matrix(const Matrix *matP, CVIP_TYPE dt);
   Matrix *mag_Matrix(Matrix *in);
   Matrix *square_mag_Matrix(const Matrix *in);
   CVIP_BOOLEAN conj_Matrix(Matrix *in);
   Matrix *sqrt_Matrix(Matrix *in);
   Matrix *cbrt_Matrix(Matrix *in);
   Matrix *convolve_Matrix(Matrix *, Matrix *, int handle, const int center);

//


typedef enum {PBM, PGM, PPM, EPS, TIF, GIF, RAS, ITX,IRIS, CCC, BIN, VIP, GLR, BTC, BRC, HUF, ZVL, ARITH, BTC2, BTC3, DPC, ZON, ZON2, SAFVR, JPG, WVQ, FRA, VQ} IMAGE_FORMAT;
 typedef enum {BINARY, GRAY_SCALE, RGB, HSL, HSV, SCT, CCT, LUV, LAB, XYZ} COLOR_FORMAT;
 typedef enum {
	T_WORD, 
	T_BAR, 
	T_AMP, 
	T_SEMI, 
	T_GT, 
	T_GTGT, 
	T_LT, 
	T_NL, 
	T_EOF
} TOKEN;

 typedef enum {
	INTRO, 
	HELP, 
	I, 
	II1, 
	II2, 
	II3, 
	II4, 
	II5, 
	II6,
	II7,
	II8,
	II9,
	II1_1, 
	II1_2,
	II1_3, 
	II1_4, 
	II1_1_1,
	II1_1_2, 
	II1_1_3, 
	II1_1_4, 
	II1_1_5, 
	II1_3_8,
	II1_4_7, 
        II1_1_5_7,
	II2_1,
	II2_2,
        II2_3,
        II2_4,
	II2_1_2,
	II2_3_8,
	II3_1,
	II3_2,
	II3_4,
	II3_5,
	II4_1,
	II4_2,
	II4_4,
	II4_5,
        II5_1,
	II5_2,
	II5_3,
	II5_4,
	II6_1,
	II6_2,
        II6_3,
	II6_4,
 	II6_5,
        II6_1_4,
        II6_1_5,
	II6_2_1,
	II6_2_2, 
        II6_2_4,
	II6_5_1,
	II6_5_2,
        II8_7,
	II9_7
} MENU;

 typedef enum {

	HISTEQ,
	HISTSPEC,
	LOCAL_HISTEQ,
	SMOOTH,
	SHARP,
	EDGE_LAPLACIAN, 
	EDGE_ROBERTS, 
	EDGE_SOBEL, 
	EDGE_PREWITT,
	EDGE_KIRSCH, 
	EDGE_ROBINSON,
	EDGE_PYRAMID,
	EDGE_FREI, 
	EDGE_LINK,
	HOUGH,
	CONVERT,  
	MORPHO,

// Arithmetic Logic 

	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	AND,
	NOT,
	OR,
	XOR,


// Band Utilities

	EXTRACT_BAND,
	ASSEMBLE_BANDS,


// Segmentation

	MULTIRES,
	SPLITNMERGE,
	FUZZYC,
	SCTSPLIT,
	HIST_THRESH,
	MEDIANCUT,

// Color

	COLORXFORM, 
	LUMINANCE,
	subPCT, 
	I_subPCT, 
	PCT, 
	I_PCT,

// Transforms

	DCTXFORM, 
	I_DCTXFORM, 
	FFT, 
	I_FFT, 
	WALSH, 
	I_WALSH, 
	HADAMARD, 
	I_HADAMARD, 
	HAAR, 
	I_HAAR, 
	WAVELET, 
	I_WAVELET,

// Noise Utilities

	GAMMA_NOISE,
	GAUSSIAN_NOISE,
	NEGATIVE_NOISE,
	RAYLEIGH_NOISE,
	SALT_NOISE,
	UNIFORM_NOISE,

// Histogram Modification

	HIST_SHRINK,
	HIST_SLIDE,
	HIST_SPEC,
	HIST_STRETCH,
	GRAY_LIN_MOD,
	GRAY_NOLIN_MOD,

// Compression

	Block_Trunc_Coding, 
	GRAY_RLC, 
	BITPLANE_RLC, 
	HUFFMAN,
	JPEG_COMPRESS,
	ARITHMETIC_CODE,
	LZW_COMPRESS,
	BTC_ALG2,
	BTC_ALG3,
	DPCM,
	UNIX_PACK,
	UNIX_COMPRESS,
	UNIX_GZIP,

// MORPHO

	MORPH_OPEN,
	MORPH_CLOSE,
	MORPH_DILATE,
	MORPH_ERODE,

// Geometry

	SCALE,
	ROTATE,
	ZOOM,
	ENLARGE,
	TRANSLATE,
	CUT_PASTE,
	CROP,
	GRAY_QUANT,
	SPATIAL_QUANT,

// Utility Image Creation

	CHECKBOARD,
	CIRCLE,
	LINE,
	SQUARE,
	SINE_WAVE,
	COSINE_WAVE,
	SQUARE_WAVE,

// Object Properties

	LABEL,
	PROPS,
	BINTHRESH,
	EULER,
	PERIMETER,
	THINNESS,
	IRREGULARITY,

// Warping

	BILINEAR,
	CREATE_MESH,
	DISPLAY_MESH,
	WARPING,
	KEYBOARD_MESH,
	AREA_POINTS,

// Linear Filters

	BLUR_SPATIAL,
	LOWPASS_SPATIAL,
	LAPLACIAN_SPATIAL,
	HIGHPASS_SPATIAL,
 	DIFFERENCE_SPATIAL,
	SPECIFY_SPATIAL,
	UNSHARP_SPATIAL,
	RASTER_SCAN,
	SINGLE_FILTER,

// Transform-Domain Filtering

	XFORMFILTER,
	INVERSEFILTER,
	LEAST_SQUARES,
	SIMPLE_WIENER,
	WIENER_FILTER,
	PARAMETRIC_WIENER,
	GEOMETRIC_MEAN,
	POWER_SPECT_EQUALIZE,
	HOMOMORFILTER,
	NOTCHFILTER,
	


// Spatial-Domain Filtering

	MINIMUM_FILTER,
	MEAN_FILTER,
	ALPHA_FILTER,
	CONTRA_FILTER,
	GEOMETRIC_FILTER,
	HARMONIC_FILTER,
	YP_MEAN_FILTER,
	MEDIAN_FILTER,
	MIDPOINT_FILTER,
	MAXIMUM_FILTER,

// Environment Options

	CVIP_IMGPATH,
	CVIP_DISPLAY,

// Other

	ZERO_PAD,
	DATA_REMAP,
	MOUSE_COORDS,

// Applications

	DERMVIS,
	FEATURE,
	WVQ_APP,
	DEFORM_TEMPLATE,
	NIGHTVISION,
	COPTER_APP1

} PROGRAMS;
*/
/*
 typedef struct packet PACKET;

 struct packet {
      CVIP_TYPE *dtype;
      unsigned int dsize;
      void **dptr; 
   };

 typedef struct history *HISTORY;

 struct history {
      PROGRAMS ftag;
      PACKET *packetP;
      HISTORY next;
   };

 typedef struct {
      IMAGE_FORMAT image_format;
      COLOR_FORMAT color_space;
      int bands;
      MATRIX **image_ptr;
      HISTORY story;
   } Image;

    #define bandP image_ptr
*/
 struct ObjPropType {
      float eig_ratio;
      float orientation;
      float h_cog;
      float v_cog;
      float area;
   };

 typedef struct ObjPropType ObjProp;
/*
// Color
   struct ColorType {
      byte r, g, b;
   };

   typedef struct ColorType Color;

   #define getRed_Color(p) ((p).r)
   #define getGrn_Color(p) ((p).g)
   #define getBlu_Color(p) ((p).b)

   #define assign_Color(p,red,grn,blu) do { (p).r = (red); (p).g = (grn); (p).b = (blu); } while ( 0 )

   #define equal_Color(p,q) ( (p).r == (q).r && (p).g == (q).g && (p).b == (q).b )
//
*/
 struct ObjectType
   {
      int label;
      Color pixel;
      long x_min;
      long y_min;
      long x_max;
      long y_max;
      ObjProp prop;
   };

 typedef struct ObjectType Object;
/*
    struct DoubleLinkType {
      struct DoubleLinkType *nextP;
      struct DoubleLinkType *previousP;
      void *contentsP;
   };

   typedef struct DoubleLinkType dlink;

 struct SingleLinkType {
      struct SingleLinkType *nextP;
      void *contentsP;
   };

 typedef struct SingleLinkType slink;

 struct SinglyLinkedListType {
      slink *clP;
      slink *headP;
      slink *tailP;
      int listlength;
   };

 typedef struct SinglyLinkedListType linked_list;
*/
 /*

    linked_list *new_LL(void);
    void removenext_LL(linked_list *llP);
    void removehead_LL(linked_list *llP);
    int size_LL(linked_list *llP);
    void head_LL(linked_list *llP);
    void tail_LL(linked_list *llP);
    int istail_LL(linked_list *llP);
    int ishead_LL(linked_list *llP);
    void next_LL(linked_list *llP);
    void previous_LL(linked_list *llP);
    void *retrieve_LL(linked_list *llP);
    void *retrieveNext_LL( linked_list *llP );
    void addhead_LL(linked_list *llP, void *newobjectP);
    void addnext_LL(linked_list *llP, void *newobjectP);
    void promote_LL(linked_list *llP);
    void print_LL(linked_list *llP, void (*print)(void *objectP,FILE *fileP), FILE *fileP);
    void delete_LL(linked_list *llP, void (*objfree)(void *objectP) );
    void replace_LL(linked_list *llP, void *newobjectP);
    int find_LL(linked_list *llP, int (*match)(void *objectP, void *lookforP), void *lookforP);
    int findNext_LL( linked_list *llP, int (*match)(void *objectP, void *lookforP), void *lookforP );
    int isempty_LL( linked_list *llP );

    slink *new_Link( void *newcontentsP );
    void delete_Link( slink *linkP );
*/
 /*
// Hash table
   #define setKey_HT(h,k) ((h)->key = (k))
   #define size_HT(h) ((h)->size)

   struct HashTableType {
      linked_list **table;
      unsigned key;

      unsigned size;
   };

   typedef struct HashTableType HashTable;

   HashTable *new_HT(size_t size);
   void delete_HT(HashTable *htP, void (*objfree)(void *objectP));
   void addObject_HT(HashTable *htP, void *objectP);
   int findObject_HT(HashTable *htP, int (*match)(void *objectP, void *lookforP), void *lookforP);
   int findNextObject_HT(HashTable *htP, int (*match)(void *objectP, void *lookforP), void *lookforP);
   void *getNextObject_HT(HashTable *htP);
   void *getObject_HT(HashTable *htP);
   void removeNextObject_HT(HashTable *htP);
*/
//
/*
// Stack
   	typedef linked_list *Stack;

	Stack new_Stack(void);
	void push_Stack(Stack sP, void *objectP);
	void *pop_Stack(Stack sP);
	int isempty_Stack(Stack sP);

	#define delete_Stack(stackP,objfreeP) delete_LL(stackP,objfreeP)
//

// ROI
	   struct ROIType {

      unsigned x;     // Horizontal offset of ROI from pixel location (0,0) 
      unsigned y;     // Vertical offset of ROI from pixel location (0,0) 
      unsigned dx;    // Horizontal size of the ROI 
      unsigned dy;    // Vertical size of the ROI 

      Image *dataP;
   };

   typedef struct ROIType ROI;

   static int DataSize[5] = {sizeof(byte), sizeof(short), sizeof(int), sizeof(float), sizeof(double)};

   ROI *new_ROI(void);
   void delete_ROI(ROI *roiP);
   void asgnImage_ROI(ROI *roiP, Image *imageP, unsigned x, unsigned y,  
   unsigned dx, unsigned dy);
*/
   #define asgnFullImage_ROI(roiP,imageP) \
   asgnImage_ROI(roiP,imageP,0,0,getNoOfCols_Image(imageP),\
   getNoOfRows_Image(imageP))

   /* Macros */

   #define getHorOffset_ROI(roiP) ((roiP)->x)
   #define getVerOffset_ROI(roiP) ((roiP)->y)
   #define getHorSize_ROI(roiP) ((roiP)->dx)
   #define getVerSize_ROI(roiP) ((roiP)->dy)
   #define getNoOfRows_ROI(roiP) getVerSize_ROI(roiP)
   #define getNoOfCols_ROI(roiP) getHorSize_ROI(roiP)
   #define getNoOfBands_ROI(roiP) (((roiP)->dataP)->bands)
   #define getDataFormat_ROI(roiP) ((*((roiP)->dataP)->bandP)->data_format)
   #define getDataType_ROI(roiP) ((*((roiP)->dataP)->bandP)->data_type)

   #define getRealRow_ROI(roiP,y,band_no) \
   ((byte *)getRealRow_Image((roiP)->dataP,(y)+getVerOffset_ROI(roiP), \
    band_no) + DataSize[getDataType_ROI(roiP)] * getHorOffset_ROI(roiP))
   #define getRow_ROI(roiP,y,band_no) getRealRow_ROI(roiP,y,band_no)
   #define getImagRow_ROI(roiP,y,band_no) \
   ((byte *)getImagRow_Image((roiP)->dataP,(y)+getVerOffset_ROI(roiP), \
   band_no) + DataSize[getDataType_ROI(roiP)] * getHorOffset_ROI(roiP))

//

// ColorHist
   #define RED 0
   #define GRN 1
   #define BLU 2
/*
   struct ColorHistObjectType {
      Color pixel;
      unsigned value;
   };

   typedef struct ColorHistObjectType ColorHistObject;

   struct ColorHistogramType {
      ColorHistObject *histogram;
      unsigned no_of_colors;
   };

   typedef struct ColorHistogramType ColorHistogram;

   typedef ColorHistObject *ColorMap;

   typedef HashTable ColorHashTable;
   ColorHistogram *new_ColorHist(void);
   void delete_ColorHist(ColorHistogram *chP);
   void compute_ColorHist(ColorHistogram *chP, Image *imageP, unsigned maxval);
   void print_ColorHist( ColorHistogram *chP );
   int dropColor_ColorHist( ColorHistogram *chP, Color pixel, unsigned *no_pixels);

   #define new_ColorHT() new_HT((size_t)20023)
   #define setKey_ColorHT(h,k) setKey_HT(h,k)

   void addObject_ColorHT(HashTable *chtP, Color pixel, int index);
   ColorHashTable *hist2Hash_ColorHT(ColorHistogram *chP);
   int lookUpColor_ColorHT( ColorHashTable *chtP, Color pixel );
   void delete_ColorHT(ColorHashTable *chtP);
*/
//

 typedef linked_list *ObjectList;

   struct ObjectVectorType {
      Object *oP;
      unsigned no_of_objects;
   };

   typedef struct ObjectVectorType ObjectVector;

/***	a Vector is one link in a simple chain that follows
	the edge of a contour from sample point to sample point	***/
/*
struct vector
{
	short		dir;
	struct vector *	next;
};

typedef struct vector	Vector;

struct ChainCodeType 
{
	int start_y;
	int start_x;
	int elev;
	int no_of_vectors;

	Vector *chain;
};
*/

typedef struct ChainCodeType ChainCode;


    /* macros used to extract information from the Image structure */
   #define getNoOfRows_Image(image) ((image)->bandP[0]->rows)
   #define getNoOfCols_Image(image) ((image)->bandP[0]->cols)
   #define getDataType_Image(image) ((image)->bandP[0]->data_type)

	float * getDataRange_Image( Image *imageP);

   #define getDataFormat_Image(image) ((image)->bandP[0]->data_format)
   #define getFileFormat_Image(image) ((image)->image_format)
   #define getColorSpace_Image(image) ((image)->color_space)
   #define getNoOfBands_Image(image) ((image)->bands)
   #define setNoOfBands_Image(image,bands) \
   (getNoOfBands_Image(image) = (bands))
   #define setFileFormat_Image(image,format) \
   (getFileFormat_Image(image) = (format))
   #define setColorSpace_Image(image,space) \
   (getColorSpace_Image(image) = (space))

   #define getBand_Image(image,band_no) ((image)->bandP[band_no])
   #define setBand_Image(image,band,band_no) ((image)->bandP[band_no]=(band)) 

   #define getRealData_Image(image,band_no) \
   (getRealData_Matrix(getBand_Image(image,band_no)))
   #define getData_Image(image,band_no) getRealData_Image(image,band_no)
   #define getImagData_Image(image,band_no) \
   (getImagData_Matrix(getBand_Image(image,band_no)))

   #define getRealRow_Image(image,row,band_no) \
   (getRealRow_Matrix(getBand_Image(image,band_no),row))
   #define getRow_Image(image,row,band_no) getRealRow_Image(image,row,band_no)
   #define getImagRow_Image(image,row,band_no) \
   (getImagRow_Matrix(getBand_Image(image,band_no),row))

	#define getRealPixel_Image(image,type,x,y,band_no) \
	(getRealVal_Matrix(getBand_Image(image,band_no),type,x,y))
	#define getPixel_Image(image,type,x,y,band_no) \
	getRealPixel_Image(image,type,x,y,band_no)
	#define setRealPixel_Image(image,type,x,y,band_no,pixel) \
	(getPixel_Image(image,type,x,y,band_no) = (pixel))
	#define setPixel_Image(image,type,x,y,band_no,pixel) \
	setRealPixel_Image(image,type,x,y,band_no,pixel)
	#define getImagPixel_Image(image,type,x,y,band_no) \
	(getImagVal_Matrix(getBand_Image(image,band_no),type,x,y))
	#define setImagPixel_Image(image,type,x,y,band_no,pixel) \
	(getImagPixel_Image(image,type,x,y,band_no) = (pixel))

	Image *new_Image(IMAGE_FORMAT image_format, COLOR_FORMAT color_space,
	int bands, int heigth, int width, CVIP_TYPE data_type,
	FORMAT data_format);     
	Image* read_Image(char*, int);
//	int cast_Image(const Image *, CVIP_TYPE);
	IMAGE_FORMAT getFormat_CVIP(char*);
	void write_Image(Image*, char*, CVIP_BOOLEAN, CVIP_BOOLEAN, IMAGE_FORMAT, CVIP_BOOLEAN);
	Image *hist_thresh_segment(Image *imgP);
	Image *hist_thresh_gray(Image *imgP);
	Image *shrink( Image *input_Image, float factor );
    Image *bilinear_interp(Image *inImg, float factor);
    Image * spatial_quant(Image *cvipImage, int  row,  int  col,int method);


	Image *threshold_segment(Image *inputImage, unsigned int threshval,
	CVIP_BOOLEAN thresh_inbyte);
	/*
	<inputImage> - pointer to Image structure
	<threshval> - threshold value
	<thresh_inbyte>  
	  - CVIP_NO apply threshval directly to image data;
	  - CVIP_YES  threshval is CVIP_BYTE range; remap to image
			  data range before thresholding.
	*/

	Image  *crop(Image  *imgP,  unsigned  row_offset,   unsigned
     col_offset, unsigned rows, unsigned cols);
	Image* edge_detect_setup(Image*, int);
	Image *MorphOpen(Image *inputImage, int k_type,  int  ksize,
						int height, int width);
	Image *MorphClose(Image *inputImage, int k_type,  int  ksize,
	int height, int width);
	Image *MorphDilate(Image *inputImage, int k_type, int ksize,
	int height, int width);
	Image *MorphErode(Image *inputImage, int k_type, int ksize,
	int height, int width);
	Image *and_Image(Image *inputImage1,Image *inputImage2);
	Image *duplicate_Image(const Image *a);
	void delete_Image(Image *A);
	Image *gray_quant_segment(Image *cvipImage, int num_bits);
	Image *CVIPluminance(Image   *cvip_Image,   int    maxval,
	CVIP_BOOLEAN retain_image, CVIP_BOOLEAN verbose);
	Image *median_filter(Image *inputImage, int size);

	void print_Object( void *thisP, FILE *fileP);
	int read_Object( void *thisP, FILE *fileP);
	Object *new_Object(int label_count, Color pixel, int y_pos, int x_pos);
	void delete_Object(void *objectP);
	int match_Object(void *objectP, void *labelP);

	ObjectList label_Objects(Image *imageP, Image **labelP, unsigned background);
	void drawBB_Objects(Image *imageP, ObjectList listP);     
	ObjectVector *listToVector_Objects(ObjectList listP);
	void getProp_Objects(ObjectList listP, Image *labelP);
	int trimList_Objects(ObjectList listP, unsigned width, unsigned height,
	float p_area_min, float p_area_max, float eig_r_min, float eig_r_max,
	float angle_min, float angle_max, float p_width, float p_height,
	CVIP_BOOLEAN no_edge);
	int perimeter(Image * labeledImage, int r, int c);

	ChainCode *new_ChainCode( int start_r, int start_c, int elev );
	void delete_ChainCode(ChainCode *ccP);
	void draw_ChainCode(ChainCode *ccP, Image *imageP);
	int print_ChainCode(ChainCode* ccP, const char *name);
	Image  *create_rectangle(int  im_width,  int  im_height,int
	start_c,int start_r,int rect_width, int rect_height);
	int shootRay( Image *imageP, int elev, int *ray_x, int *ray_y, int x1, int y1, int x2, int y2);
	void getXY_ChainCode(ChainCode *ccP, int **cP, int **rP);
	Image *CVIPhalftone(Image  *cvip_Image,  int  halftone,  int
	maxval,   float   fthreshval,   CVIP_BOOLEAN   retain_image,
	CVIP_BOOLEAN verbose);

	int MatchInt(void* content, void* lookforP);

	Image* colorxform(const Image* rgbImage, COLOR_FORMAT newcspace,
		float* norm, float* refwhite, int dir);

	Image *histeq(Image *in, int band);
//	Image *remap_Image(const  Image  *imageP,  CVIP_TYPE  dtype, unsigned dmin, unsigned dmax);
      
}  // end extern "C"

extern short build_LineChainCode(ChainCode *ccP, Image *imageP,
	int c_min, int r_min, int c_max, int r_max);
     
#endif