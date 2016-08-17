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

#ifndef __HIGHGUI_H_
#define __HIGHGUI_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>

///MB:#include "highgui.h"
#include "/mdev/visual_algorithmics/OpenCV/otherlibs/highgui/highgui.h"
///MB:#include "cxmisc.h"
#include "/mdev/visual_algorithmics/OpenCV/cxcore/include/cxmisc.h"

#ifndef WIN32
#include "cvconfig.h"
#else
void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin );
#endif

/* Errors */
#define HG_OK          0 /* Don't bet on it! */
#define HG_BADNAME    -1 /* Bad window or file name */
#define HG_INITFAILED -2 /* Can't initialize HigHGUI. Possibly, can't find vlgrfmts.dll */
#define HG_WCFAILED   -3 /* Can't create a window */
#define HG_NULLPTR    -4 /* The null pointer where it should not appear */
#define HG_BADPARAM   -5 

#define CV_WINDOW_MAGIC_VAL     0x00420042
#define CV_TRACKBAR_MAGIC_VAL   0x00420043

#ifdef WIN32
#undef HAVE_MIL
#define HAVE_MIL 0 /* change 0 to 1 to turn on MIL support */
#endif

/***************************** CvCapture structure ******************************/

#define CV_CAPTURE_BASE_API_COUNT 6

typedef void (CV_CDECL* CvCaptureCloseFunc)( CvCapture* capture );
typedef int (CV_CDECL* CvCaptureGrabFrameFunc)( CvCapture* capture );
typedef IplImage* (CV_CDECL* CvCaptureRetrieveFrameFunc)( CvCapture* capture );
typedef double (CV_CDECL* CvCaptureGetPropertyFunc)( CvCapture* capture, int id );
typedef int (CV_CDECL* CvCaptureSetPropertyFunc)( CvCapture* capture,
                                                  int id, double value );
typedef const char* (CV_CDECL* CvCaptureGetDescriptionFunc)( CvCapture* capture );

typedef struct CvCaptureVTable
{
    int     count;
    CvCaptureCloseFunc close;
    CvCaptureGrabFrameFunc grab_frame;
    CvCaptureRetrieveFrameFunc retrieve_frame;
    CvCaptureGetPropertyFunc get_property;
    CvCaptureSetPropertyFunc set_property;
    CvCaptureGetDescriptionFunc get_description;
}
CvCaptureVTable;

typedef struct CvCapture
{
    CvCaptureVTable* vtable;
}
CvCapture;

#ifndef WIN32
#ifdef HAVE_CAMV4L
CvCapture* icvOpenCAM_V4L( int wIndex );
#endif
#ifdef HAVE_DC1394
CvCapture* icvOpenCAM_DC1394( int wIndex );
#endif
#endif

#endif /* __HIGHGUI_H_ */

