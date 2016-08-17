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

#ifndef __ICVVIDEO_H__
#define __ICVVIDEO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cvcam.h"
#include "cvvidtypes.h"
#include "cv.h"
#include "pthread.h"
#include "X11/Xlib.h"


/* main video cycle for single camera*/
void* icvVideoCaptureProc(void* arg);

int icvVideoPauseCamera(int camera);

int icvVideoResumeCamera(int camera);

int icvVideoInit(int CurrentCamera);

int icvVideoStart(int camera);

int icvVideoStop(int camera);

/* returns depth in bits, 0 for unsupported formats */
int icvVideoFormat2Depth(int format);

/* Initializes most fields of CvVideoCamera struct */
int icvVideoSetDefaultValues(int camera, int device, int channel,
     struct video_capability VidCap);

#define IF_GETTING_PROPERTY(someproperty) if(!strcmp(someproperty,property))
#define IF_SETTING_PROPERTY(someproperty) if(!strcmp(someproperty,property))
#define CAPTURING 0
#define STOPPING  1
#define FINISHED  2
#define READY     3
#define STARTING  4

typedef struct
{
int device;
int channel;
pthread_mutex_t ioctlmutex;     /* protects access to the unix device for that camera */
int enabled;
int rendered;
int initialised;
int paused;
pthread_mutex_t pausemutex;  /* to pause acquisition */
pthread_mutex_t capturestatemutex; /* protects the capturestate and renderstate fields (used in capture, main and render threads) */
pthread_cond_t capturestatecond;
volatile int capturestate;  /* State of main loop for camera can be CAPTURING,
                                             STOPPING, FINISHED or READY */
volatile int   renderstate;  /*non zero if rendering loop exists*/
void (*callback)(IplImage* frame);
CameraDescription description;
VideoFormat videopp;
struct video_mmap vid_mmap;
struct video_mbuf mbuf;
char* memorymap;
__u16 imagepalette;             /* output image palette */
pthread_rwlock_t *framelock; /* protect each frame (1 rwlock per frame)*/
pthread_mutex_t lastframemutex; /* protects frame number */
volatile int lastframe;
pthread_cond_t updatedcond;
pthread_mutex_t updatedmutex;
int updated;
pthread_t thread;
Window window;
int renderwidth;
int renderheight;
}CvVideoCamera;



#ifdef __cplusplus
}
#endif



#endif //__ICVVIDEO_H__




