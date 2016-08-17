/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "_cv.h"

static CvStatus  icvCalcValues(float* Dx2Blured,
                        float* Dy2Blured,
                        float* DxyBlured,
                        int width,
                        int srcStep,
                        float* eigenvv,
                        int eigenStep,
                        int NumStr,
                        float factor)
{

    int i,j;
    eigenStep >>= 2;
    srcStep >>=2;

    for ( i = 0 ; i < NumStr; i++ )
    {
        for ( j = 0; j < width ; j++ )
        {
            /* finding eigenvalues of |a b|
                                      |b c| */

            float a = Dx2Blured[j]*factor;
            float b = DxyBlured[j]*factor;
            float c = Dy2Blured[j]*factor;
            float l1,l2;
            float det;
            float dmax  = MAX( a , c);
            float dmin  = MIN( a , c);

            dmax *= 0.01f;

            /* if singular matrix  - don't process it */
            if ( dmin < dmax )
            {
                memset(eigenvv + 6*j, 0, 24); continue;
            }

            if ( fabs(b) < dmax )
            {
                eigenvv[ 6 * j ] = a;
                eigenvv[ 6 * j + 1] = c;
                eigenvv[ 6 * j + 2] = 1.0f;
                eigenvv[ 6 * j + 3] = 0;
                eigenvv[ 6 * j + 4] = 0;
                eigenvv[ 6 * j + 5] = 1.0f;
                continue;
            }
            det = a * c - b * b;
            if ( det < dmax )
            {
                memset(eigenvv + 6*j, 0, 24); continue;
            }

            {
                float apc = a + c;
                float discr = apc * apc - 4 * det;
                float Sqrt  = (float)sqrt( (double)discr );
                float Inorm1,Inorm2,x1,x2,y1,y2;

                l1 = (apc + Sqrt)*0.5f;
                l2 = (apc - Sqrt)*0.5f;

                x1 = b;
                x2 = (-( a - l1 ));

                y1 = b;
                y2 = (-( a - l2 ));

                Inorm1 = 1.f/(float)sqrt((double)x1*x1 + (double)x2*x2);
                Inorm2 = 1.f/(float)sqrt((double)y1*y1 + (double)y2*y2);
                eigenvv[ 6 * j ] = l1;
                eigenvv[ 6 * j + 1] = l2;
                eigenvv[ 6 * j + 2] = x1 * Inorm1;
                eigenvv[ 6 * j + 3] = x2 * Inorm1;
                eigenvv[ 6 * j + 4] = y1 * Inorm2;
                eigenvv[ 6 * j + 5] = y2 * Inorm2;

            }
        }
        Dx2Blured += srcStep;
        Dy2Blured += srcStep;
        DxyBlured += srcStep;

        eigenvv += eigenStep;
    }
    return CV_NO_ERR;
}

static CvStatus  icvMulDBuffers(CvSize roi, int step,
                              float* BXX, float* BXY, float* BYY)
{
    int i, j;
    float* TBXX=BXX;
    float* TBXY=BXY;
    float* TBYY=BYY;
    int fstep = step>>2;

    for( i = 0; i < roi.height; i++ )
    {
        for (j= roi.width-1; j>=0; j--)
        {
            float x = ((short*)TBXX)[j];
            float y = ((short*)TBYY)[j];
            TBXX[j] = x*x;
            TBXY[j] = x*y;
            TBYY[j] = y*y;
        }

        TBXX += fstep;
        TBYY += fstep;
        TBXY += fstep;
    }

    return CV_NO_ERR;
}

static CvStatus  icvMulDBuffers32f( CvSize roi,
                                  float* BXX,
                                  float* BXY,
                                  float* BYY)
{
    int i, j;
    for( i = 0; i < roi.height; i++)
    {
        for (j= 0; j< roi.width; j++)
        {
            float x = BXX[i*roi.width+j];
            float y = BYY[i*roi.width+j];
            
            BXX[i*roi.width+j] = x*x;
            BXY[i*roi.width+j] = x*y;
            BYY[i*roi.width+j] = y*y;
        }
    }
    return CV_NO_ERR;
}

static CvStatus CV_STDCALL
icvEigenValsVecsGetSize( int roiWidth,int apertureSize,
                         int avgWindow, int* bufferSize )
{
    if((roiWidth<=0)&&(apertureSize<=3)&&(avgWindow<=3))return CV_BADSIZE_ERR;
    if(!bufferSize) return CV_NULLPTR_ERR;
    (*bufferSize)  = 3 * (MAX(7,MAX(avgWindow, apertureSize))+1) * roiWidth*sizeof(float);
    return CV_NO_ERR;
}

/*F///////////////////////////////////////////////////////////////////////////////////////
//    Name:    p_cvCalcCornerEigenValsAndVecs8uC1R
//    Purpose:  Calculating eigenvalues and eigenvectors for matrix
//    Context:
//    Parameters:
//              src        - pointer to the source image
//              srcStep    - width of the full Src image in bytes
//              eigenvv    - array of 6-value vectors /see note/
//              eigenvvStep  - it's step in bytes
//              roi        - roi size in pixels
//              opSize     - Sobel operator aperture size - 1
//              blockSize  - size of block for summation
//    Returns:
//              CV_NO_ERR if all ok or error code
//    Notes:
//F*/
static CvStatus CV_STDCALL
icvEigenValsVecs_8u32f_C1R( const uchar* pSrc, int srcStep,
                            float* eigenvv, int eigenvvStep,
                            CvSize roi, int kerSize,
                            int blSize, void*  pBuffer )
{
    int RestToSobel = roi.height;
    CvSize curROI;
    int i;
    int HBuf = MAX(7,MAX(kerSize, blSize));
    /* multiplied derivatives buffers - used for bluring */
    float* flBufXX = (float*)pBuffer;
    float* flBufYY = flBufXX+(HBuf+1)*roi.width;
    float* flBufXY = flBufYY+(HBuf+1)*roi.width;
    float denom = 1;
    
    _CvConvState* stX;
    _CvConvState* stY;
    _CvConvState* stBX;
    _CvConvState* stBY;
    _CvConvState* stBXY;
    /* Step of all buffers in pixels */
    int ustep  = roi.width*sizeof(float);
    int Temp;
    unsigned char* src = (unsigned char*)pSrc;

    
    /* Check Bad Arguments */
    if((src == NULL) || (eigenvv == NULL))return CV_NULLPTR_ERR;
    if((srcStep <= 0)||(eigenvvStep <= 0))return CV_BADSIZE_ERR;
    if((roi.width <= 0)||(roi.height <= 0 )) return CV_BADSIZE_ERR;
    
    
    for(i = 0; i < kerSize-1;i++)denom *= 2;
    denom = denom*denom * 255*blSize*blSize;
    denom=1.0f/denom;
    curROI.width = roi.width;
    icvSobelInitAlloc(roi.width,cv8u,kerSize,CV_ORIGIN_TL,1,0,&stX);
    icvSobelInitAlloc(roi.width,cv8u,kerSize,CV_ORIGIN_TL,0,1,&stY);
    icvBlurInitAlloc(roi.width,cv32f,kerSize,&stBX);
    icvBlurInitAlloc(roi.width,cv32f,kerSize,&stBY);
    icvBlurInitAlloc(roi.width,cv32f,kerSize,&stBXY);

            
    /* Main Cycle */
    while ( RestToSobel)
    {
        int stage;
        if((RestToSobel == roi.height))
        {
            stage = CV_START;
            Temp = curROI.height = HBuf+kerSize/2; 
            
        }
        else if(RestToSobel+kerSize/2+blSize/2<=HBuf)
        {
            stage = CV_END;
            Temp = curROI.height = RestToSobel;
        }
        else
        {
            stage = CV_MIDDLE;
            curROI.height = Temp = (RestToSobel<= HBuf)?RestToSobel-1:HBuf; 
                             
        }
        RestToSobel-=Temp;
         

        icvSobel_8u16s_C1R( src, srcStep, (short*)(flBufXX+roi.width),
                          ustep, &curROI,stX, stage);

        curROI.height =Temp;
        icvSobel_8u16s_C1R( src, srcStep, (short*)(flBufYY+roi.width),
                          ustep, &curROI,stY, stage);
        
        src += Temp * srcStep;
                
/****************************************************************************************\
*                     Multy Buffers                                                      *
\****************************************************************************************/
        icvMulDBuffers(curROI,ustep,flBufXX+roi.width,flBufXY+roi.width,flBufYY+roi.width);
        Temp = curROI.height;
        icvBlur_32f_C1R(flBufXX+roi.width,ustep,flBufXX,ustep,&curROI,stBX,stage);
        curROI.height =Temp;
        icvBlur_32f_C1R(flBufXY+roi.width,ustep,flBufXY,ustep,&curROI,stBXY,stage);
        curROI.height =Temp;
        icvBlur_32f_C1R(flBufYY+roi.width,ustep,flBufYY,ustep,&curROI,stBY,stage);
        
        /* calc values */

        icvCalcValues( flBufXX, flBufYY, flBufXY, roi.width,ustep,
                     eigenvv, eigenvvStep, curROI.height,denom);
        eigenvv += curROI.height * eigenvvStep/4;

    }
   
    icvFilterFree(&stX);
    icvFilterFree(&stY);
    icvFilterFree(&stBX);
    icvFilterFree(&stBY);
    icvFilterFree(&stBXY);
    return CV_NO_ERR;
}


static CvStatus CV_STDCALL
icvEigenValsVecs_32f_C1R( const float* pSrc, int srcStep,
                          float* eigenvv, int eigenvvStep,
                          CvSize roi, int kerSize,
                          int blSize, void* pBuffer )
{
    int RestToSobel = roi.height;
    CvSize curROI;
    int HBuf = MAX(7,MAX(kerSize, blSize));
    /* multiplied derivatives buffers - used for bluring */
    float* flBufXX = (float*)pBuffer;
    float* flBufYY = flBufXX+(HBuf+1)*roi.width;
    float* flBufXY = flBufYY+(HBuf+1)*roi.width;
    
    _CvConvState* stX;
    _CvConvState* stY;
    _CvConvState* stBX;
    _CvConvState* stBY;
    _CvConvState* stBXY;
    /* Step of all buffers in pixels */
    int ustep  = roi.width*sizeof(float);
    int Temp;
    int i;
    float* src = (float*)pSrc;
    float denom = 1;
    

    
    /* Check Bad Arguments */
    if((src == NULL) || (eigenvv == NULL))return CV_NULLPTR_ERR;
    if((srcStep <= 0)||(eigenvvStep <= 0))return CV_BADSIZE_ERR;
    if((roi.width <= 0)||(roi.height <= 0 )) return CV_BADSIZE_ERR;
    for(i = 0; i < kerSize-1;i++)denom *= 2;
    denom = denom*denom * 255*blSize*blSize;
    denom=1.0f/denom;
    

    curROI.width = roi.width;
    icvSobelInitAlloc(roi.width,cv32f,kerSize,CV_ORIGIN_TL,1,0,&stX);
    icvSobelInitAlloc(roi.width,cv32f,kerSize,CV_ORIGIN_TL,0,1,&stY);
    icvBlurInitAlloc(roi.width,cv32f,blSize,&stBX);
    icvBlurInitAlloc(roi.width,cv32f,blSize,&stBY);
    icvBlurInitAlloc(roi.width,cv32f,blSize,&stBXY);

            
    /* Main Cycle */
    while ( RestToSobel)
    {
        int stage;
        if((RestToSobel == roi.height))
        {
            stage = CV_START;
            Temp = curROI.height = (RestToSobel<= HBuf+kerSize/2)?RestToSobel-1:HBuf+kerSize/2; 
            
        }
        else if(RestToSobel+kerSize/2+blSize/2<=HBuf)
        {
            stage = CV_END;
            Temp = curROI.height = RestToSobel;
        }
        else
        {
            stage = CV_MIDDLE;
            curROI.height = Temp = (RestToSobel<= HBuf)?RestToSobel-1:HBuf; 
                             
        }
        RestToSobel-=Temp;

         

        icvSobel_32f_C1R( src,srcStep,flBufXX+roi.width,
                          ustep, &curROI,stX, stage);
        curROI.height =Temp;
        icvSobel_32f_C1R( src,srcStep,flBufYY+roi.width,
                          ustep,&curROI,stY,stage);
        
        src += Temp * srcStep/4;
                
/****************************************************************************************\
*                     Multy Buffers                                                      *
\****************************************************************************************/
        icvMulDBuffers32f(curROI, flBufXX+roi.width,flBufXY+roi.width,flBufYY+roi.width);
        Temp = curROI.height;
        icvBlur_32f_C1R(flBufXX+roi.width,ustep,flBufXX,ustep,&curROI,stBX,stage);
        curROI.height =Temp;
        icvBlur_32f_C1R(flBufXY+roi.width,ustep,flBufXY,ustep,&curROI,stBXY,stage);
        curROI.height =Temp;
        icvBlur_32f_C1R(flBufYY+roi.width,ustep,flBufYY,ustep,&curROI,stBY,stage);
        
        /* calc values */

        icvCalcValues( flBufXX, flBufYY, flBufXY, roi.width,ustep,
                     eigenvv, eigenvvStep, curROI.height,denom);
        eigenvv += curROI.height * eigenvvStep/4;

    }
   
    icvFilterFree(&stX);
    icvFilterFree(&stY);
    icvFilterFree(&stBX);
    icvFilterFree(&stBY);
    icvFilterFree(&stBXY);
    return CV_NO_ERR;
}


#define ICV_DEF_INIT_TAB_DETECTION( FUNCNAME )              \
static void icvInit##FUNCNAME##Table( CvFuncTable* table )  \
{                                                           \
    table->fn_2d[CV_8U] = (void*)icv##FUNCNAME##_8u32f_C1R; \
    table->fn_2d[CV_8S] = 0;                                \
    table->fn_2d[CV_32F] = (void*)icv##FUNCNAME##_32f_C1R;  \
}


ICV_DEF_INIT_TAB_DETECTION( EigenValsVecs )

typedef CvStatus (CV_STDCALL * CvEigFunc)( const void* src, int srcstep,
                                           void* dst, int dststep,
                                           CvSize size, int aperture_size,
                                           int block_size, void* buffer );

CV_IMPL void
cvCornerEigenValsAndVecs( const void* srcarr, void* eigenvarr,
                          int block_size, int aperture_size )
{
    static CvFuncTable eig_tab;
    static int inittab = 0;
    void *buffer = 0;

    CV_FUNCNAME( "cvCornerEigenValsAndVecs" );

    __BEGIN__;

    CvSize src_size;
    int buf_size = 0;
    CvEigFunc func = 0;

    CvMat stub, *src = (CvMat*)srcarr;
    CvMat eigstub, *eigenv = (CvMat*)eigenvarr;

    if( !inittab )
    {
        icvInitEigenValsVecsTable( &eig_tab );
        inittab = 1;
    }

    CV_CALL( src = cvGetMat( srcarr, &stub ));
    CV_CALL( eigenv = cvGetMat( eigenv, &eigstub ));

    if( CV_MAT_CN(src->type) != 1 )
        CV_ERROR(CV_StsBadArg, "Source image has more than 1 channel");

    if( CV_MAT_CN(eigenv->type)*eigenv->width != src->width*6 )
        CV_ERROR(CV_StsBadArg, "Eigen-vals&vecs image should be 6 times "
                               "wider than the source image");

    if( src->height != eigenv->height )
        CV_ERROR( CV_StsUnmatchedSizes, "" );

    if( CV_MAT_DEPTH(eigenv->type) != CV_32F )
        CV_ERROR( CV_BadDepth, "Eigen-vals&vecs image does not have IPL_DEPTH_32F depth" );

    func = (CvEigFunc)(eig_tab.fn_2d[CV_MAT_DEPTH(src->type)]);
    if( !func )
        CV_ERROR( CV_StsUnsupportedFormat, "" );

    src_size = cvGetMatSize( src );

    IPPI_CALL( icvEigenValsVecsGetSize( src_size.width, aperture_size,
                                        block_size, &buf_size ));
    CV_CALL( buffer = cvAlloc( buf_size ));

    IPPI_CALL( func( src->data.ptr, src->step, eigenv->data.ptr, eigenv->step,
                     src_size, aperture_size, block_size, buffer ));

    __END__;

    cvFree( &buffer );
}


/* End of file */

