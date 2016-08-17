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

#include "render.h"
#include "convert.h"

#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define RENDER_FRAMERATE 1

#define Assert(exp)                                                 \
    if( !(exp) )                                                    \
    {                                                               \
        printf("Assertion: %s  %s: %d\n", #exp, __FILE__, __LINE__);\
        assert(exp);                                                \
    }

static void icvvResizeImage( int src_w, int src_h, int src_s, char* src,
                             int dst_w, int dst_h, int dst_s, char* dst,
                             int depth )
{

    int x, y;

    int* x_array = (int*)malloc(sizeof(int) * dst_w);
    int* y_array = (int*)malloc(sizeof(int) * dst_h);
    float x_step = (float)(src_w - 1) / (dst_w - 1);
    float y_step = (float)(src_h - 1) / (dst_h - 1);
    char* _src;

    Assert( src );
    Assert( dst );
    Assert( src_w > 0 && src_h > 0 && src_s >= src_w * depth / 8 );
    Assert( dst_w > 0 && dst_h > 0 && dst_s >= dst_w * depth / 8 );

    if( src_w == dst_w && src_h == dst_h && src_s == dst_s )
    {
        memcpy( dst, src, src_s * src_h );
        return;
    }


    for( x = 0; x < dst_w; x++ )
        x_array[x] = (int)(x_step * x + 0.5);
    for( y = 0; y < dst_h; y++ )
        y_array[y] = (int)(y_step * y + 0.5);

    Assert(x_array[dst_w - 1] == src_w - 1);
    Assert(y_array[dst_h - 1] == src_h - 1);

    switch( depth )
    {
        case 8:
            for( y = 0; y < dst_h; y++, dst += dst_s )
            {
                _src = src + y_array[y] * src_s;
                for( x = 0; x < dst_w; x++ )
                {
                    int offset = x_array[x];
                    dst[x] = _src[offset];
                }
            }
            break;
         case 16:
            for( y = 0; y < dst_h; y++, dst += dst_s )
            {
                _src = src + y_array[y] * src_s;
                for( x = 0; x < dst_w; x++ )
                {
                    int offset = x_array[x];
                    ((short*)dst)[x] = ((short*)_src)[offset];
                }
            }
            break;
        case 24:
            for( y = 0; y < dst_h; y++, dst += dst_s )
            {
                _src = src + y_array[y] * src_s;
                for( x = 0; x < dst_w; x++ )
                {
                    int offset = x_array[x] * 3;
                    dst[x * 3]     = _src[offset];
                    dst[x * 3 + 1] = _src[offset + 1];
                    dst[x * 3 + 2] = _src[offset + 2];
                }
            }
            break;
         case 32:
            for( y = 0; y < dst_h; y++, dst += dst_s )
            {
                _src = src + y_array[y] * src_s;
                for( x = 0; x < dst_w; x++ )
                {
                    int offset = x_array[x];
                    ((int*)dst)[x] = ((int*)_src)[offset];
                }
            }
            break;
    }

    free(x_array);
    free(y_array);
}
int icvVideoRenderStart(int cameraid)
{
    pthread_t   thread;

    if( !cameras[cameraid].rendered)
        return 0;

    if( !cameras[cameraid].window)
        return 0;

    if(pthread_create(&thread, NULL, icvVideoRender, (void*)cameraid))
    {
        fprintf(stderr, "icvVideoRenderStart: failed create thread for rendering");
        return 0;
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////////////
void* icvVideoRender(void* data)
{
    int cameraid = (int)data;
    CvVideoCamera *const camera = &(cameras[cameraid]);
    Display* display;
    int screen_num;
    GC gc;
    char* display_name = NULL;
    Window window = camera->window;
    XWindowAttributes windowattr;
    Visual* visual;
    int windowdepth;
    XImage* image;
    XShmSegmentInfo xshmseg;
    int width  = (camera->renderwidth>0)?camera->renderwidth:camera->videopp.width;
    int height = camera->renderheight?camera->renderheight:camera->videopp.height;
    int picturedepth = camera->videopp.picture.depth;
    int pixelsize;
    XGCValues values;
    IplImage* iplimage;
    time_t start, now;
    int frames = 0;
    float rate = 0;
    Status XShm;
    uchar* imgdata = NULL;
    uchar* tmpbuff = NULL;

    pthread_mutex_lock(&(camera->capturestatemutex));
    if(camera->capturestate != CAPTURING)
    {
        pthread_mutex_unlock(&(camera->capturestatemutex));
        pthread_exit( NULL );
    }
    camera->renderstate=1;
    pthread_cond_signal(&(camera->capturestatecond));
    pthread_mutex_unlock(&(camera->capturestatemutex));
    XInitThreads();

    if ( (display=XOpenDisplay(display_name)) == NULL )

    {
        fprintf( stderr, "cvVideo: cannot connect to X server %s\n",
            XDisplayName(display_name));
        pthread_exit( NULL );
    }

    screen_num = DefaultScreen(display);

    if (XGetWindowAttributes(display, window,
        &windowattr) == 0)
    {
        fprintf(stderr, "icvVideoRender: failed to get window attributes.\n" );
        pthread_exit(NULL);
    }

    if(windowattr.map_state == IsUnmapped)
    {
        fprintf(stderr, "icvVideoRender: window is not mapped \n" );
        pthread_exit(NULL);
    }

    windowdepth = windowattr.depth;
    visual      = windowattr.visual;

    pixelsize = icvVideoWindowPixelsize(windowdepth);

    XShm = XShmQueryExtension(display);
    if(XShm)
    {
        image = XShmCreateImage(display, visual, windowdepth, ZPixmap, NULL,
            &xshmseg, width, height );

        assert(image);

        xshmseg.shmid = shmget (IPC_PRIVATE,
            width*height*pixelsize, IPC_CREAT|0777);

        assert(xshmseg.shmid != -1);
        xshmseg.shmaddr = image->data=(char*)shmat (xshmseg.shmid, 0, 0) ;

        xshmseg.readOnly = False;

        XShmAttach (display, &xshmseg);
    }
    else
    {
        imgdata = (uchar*)malloc(width*height*icvVideoWindowPixelsize(windowdepth)) ;
        image = XCreateImage(display, visual, windowdepth, ZPixmap, 0,
            (char*)imgdata, width,
            height, 8,
            icvVideoWindowPixelsize(windowdepth)
            *width);

        assert(image);
        XInitImage(image);
    }

    gc = XCreateGC(display,window,0, &values );
#ifdef RENDER_FRAMERATE
    start = time(NULL);
#endif

    pthread_mutex_lock(&(camera->capturestatemutex));
    while((camera->capturestate == CAPTURING) && (camera->rendered))
    {
        pthread_mutex_unlock(&(camera->capturestatemutex));
        pthread_mutex_lock(&(camera->updatedmutex));
        while(camera->updated == 0)
            pthread_cond_wait(&(camera->updatedcond), &(camera->updatedmutex));
        camera->updated = 0;
        pthread_mutex_unlock(&(camera->updatedmutex));
        if(cvcamGetProperty(cameraid, "raw_image",&iplimage ))
        {
            assert(image->data);
            if(camera->callback != NULL)
                camera->callback(iplimage);
            if((width==camera->videopp.width)&&
               (height==camera->videopp.height))
            {
                icvvConvert(width, height, width*picturedepth/8, picturedepth,
                            iplimage->imageData, width*pixelsize, pixelsize*8, image->data
                    );
                cvReleaseImage(&iplimage);
            }
            else
            {
                tmpbuff = (uchar*)malloc(camera->videopp.width*camera->videopp.height*
                                         pixelsize) ;
                
                icvvConvert(camera->videopp.width, camera->videopp.height,
                            camera->videopp.width*picturedepth/8, picturedepth,
                            iplimage->imageData, camera->videopp.width*pixelsize,
                            pixelsize*8, (char*)tmpbuff);
                cvReleaseImage(&iplimage);
                
                icvvResizeImage(camera->videopp.width,
                                camera->videopp.height,
                                (camera->videopp.width)*pixelsize, (char*)tmpbuff,
                                width, height,width*pixelsize, image->data, pixelsize*8);
                
                free(tmpbuff);
                
            }
            
            //fprintf(stdout, "cvVideoRendering:image converted!!!!\n");
            
            if(XShm)
            {
                XShmPutImage(display, window, gc,
                             image,0,0,0,0, width,
                             height, False);
            }
            else
            {
                XPutImage(display, window, gc,
                          image,0,0,0,0, width,
                          height);
            }
            
            XSync(display, False);
#ifdef RENDER_FRAMERATE            
            now = time(NULL);
            frames++;
            if (now-start)
                rate = frames/(float)(now-start);
            if((frames%30) == 0)
                fprintf(stdout, "camera %d fps = %f\n", cameraid, rate);
#endif
        }//if(cvcamGetProperty(CAMERA, "raw_image",&image ))

        // stop here if we're paused
        pthread_mutex_lock(&(camera->pausemutex));
        pthread_mutex_unlock(&(camera->pausemutex));
        pthread_mutex_lock(&(camera->capturestatemutex));
    }//while (camera->state == CAPTURING && camera->rendered)
    pthread_mutex_unlock(&(camera->capturestatemutex));

    pthread_mutex_lock(&(camera->capturestatemutex));
#if 0
    if(camera->state != CAPTURING) {
        // we ended because the camera is not capturing anymore
        while (camera->capturestate != FINISHED )
        {
            pthread_cond_wait(&(camera->capturestatecond),&(camera->capturestatemutex));
        }
    }
#endif
    camera->renderstate=0;
    pthread_cond_signal(&(camera->capturestatecond));
    pthread_mutex_unlock(&(camera->capturestatemutex));

    XShmDetach (display, &xshmseg);
    XDestroyImage (image);
    XFreeGC(display,gc);
    shmdt (xshmseg.shmaddr);
    shmctl (xshmseg.shmid, IPC_RMID, 0);
    if(imgdata)
        free(imgdata);  
    pthread_exit(NULL);
}

////////////////////////////////////////////////////////////////////////////////
int icvVideoWindowPixelsize(int depth)
{
    switch (depth)
    {
    case 32:
        return 4;
        
    case 24:
        return 4;
        
    case 16:
        return 2;
        
    case 8:
        return 1;
        
    default:
        return 0;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
