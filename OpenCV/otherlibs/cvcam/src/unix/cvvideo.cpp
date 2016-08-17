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

#include <glob.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <malloc.h>
#include <errno.h>
#include <pthread.h>
#include "dialogs.h"


#include "cvcam.h"
#include "cvvidtypes.h"
#include "videodev.h"
#include "icvvideo.h"
#include "render.h"

//#define DEBUG_CVCAM 1
//#define DEBUG_THREADS 1

#ifdef DEBUG_THREADS
#define pthread_mutex_lock(x) printf("locking mutex "#x" from thread %p\n",pthread_self()),pthread_mutex_lock(x)
#define pthread_mutex_unlock(x) printf("unlocking mutex "#x" from thread %p\n",pthread_self()),pthread_mutex_unlock(x)
#define pthread_rwlock_rdlock(x) printf("rdlocking rwlock "#x" from thread %p\n",pthread_self()),pthread_rwlock_rdlock(x)
#define pthread_rwlock_wrlock(x) printf("wrlocking rwlock "#x" from thread %p\n",pthread_self()),pthread_rwlock_wrlock(x)
#define pthread_rwlock_unlock(x) printf("unlocking rwlock "#x" from thread %p\n",pthread_self()),pthread_rwlock_unlock(x)
#define pthread_create(a,b,c,d) printf("starting new thread from thread %p\n",pthread_self()),pthread_create(a,b,c,d)
#endif

const int FRAMES_FORMAT=0;
const int TIME_FORMAT = 1;

CvVideoCamera* cameras = NULL;
static int ncameras = 0;

static void icvVideoPicture2ImageBuffer(int camera);
static IplImage * icvVideoGetImage(int cameraid);

/* Returns the actual number of currently available cameras */
int cvcamGetCamerasCount()
{
    glob_t               pglob;
    int                     count, count1;
    int                     CamerasCount =  0;
    int*                    descriptors;
    
    struct video_capability VidCap;
    struct video_channel    Channel;
    
#ifdef DEBUG_CVCAM
    printf("cvcamGetCamerasCount(), ncameras=%d\n",ncameras);
#endif
    if(ncameras)
        return ncameras;
    
    if(glob("/dev/video*",GLOB_MARK,NULL, &pglob))
    {
        fprintf(stderr, "cvcamGetCamerasCount:no video devices /dev/video(n)"
                        "found\n");
        return 0;
    }

    descriptors = (int*)malloc(pglob.gl_pathc*sizeof(int));
    
    for(count = 0; count < pglob.gl_pathc; count++)
    {
        descriptors[count] = open(pglob.gl_pathv[count],O_RDWR);
        if (descriptors[count]  != -1 )
        {
            if(ioctl(descriptors[count], VIDIOCGCAP, &VidCap)==-1)
            {
                close(descriptors[count]);
                break;
            }
            
            for(count1=0; count1<VidCap.channels; count1++)
                
            {
                Channel.channel = count1;
                
                if((ioctl(descriptors[count], VIDIOCGCHAN, &Channel) != -1))
                {
                    
                    Channel.norm = 1;
                    Channel.channel = count1;
                    
                    if((ioctl(descriptors[count],VIDIOCSCHAN, &Channel) != -1)&&
                        (ioctl(descriptors[count], VIDIOCGCAP, &VidCap)    != -1))
                    {
                        if(Channel.type&VIDEO_TYPE_CAMERA)
                        {
                            
                            cameras =
                                (CvVideoCamera*)realloc(cameras,
                                (CamerasCount+1)*sizeof(CvVideoCamera));
                            if(!icvVideoSetDefaultValues(CamerasCount, 
                                descriptors[count],
                                count1, VidCap))
                            {
                                printf("icvVideoSetDefaultValues failed for camera %d, channel %d\n",count,count1);
                                break; 
                            }
                            
                            strcpy(cameras[CamerasCount].description.device,
                                pglob.gl_pathv[count]);
                            
                            strcpy(cameras[CamerasCount].description.DeviceDescription,
                                VidCap.name);
                            
                            strcpy(cameras[CamerasCount].description.ChannelDescription,
                                Channel.name);
                            
                            CamerasCount++;
                            break;
                            
                        }//if(Channel.type&VIDEO_TYPE_CAMERA)
                    }//if((ioctl(descriptors[count],VIDIOCSCHAN, &Channel) != -1)&&
                    //(ioctl(descriptors[count], VIDIOCGCAP, &VidCap)!=-1))
                }//if(ioctl(fd, VIDIOCSCHAN, &Channel) != -1)
            }//for(count1=0; count1<VidCap.channels; count1++)
            if(count1 == VidCap.channels) // no valid channel found
            {
                printf("closing  descriptors[%d]=%d\n",count,descriptors[count]);
                close(descriptors[count]);
            }
        }//if (descriptors[count] = open(pglob.gl_pathv[count],O_RDWR)  != -1 )
    }//for (count = 0; count < pglob.gl_pathc; count++)
    globfree(&pglob);
    free(descriptors);
    ncameras = CamerasCount;
    XInitThreads();
#ifdef DEBUG_CVCAM
    printf("cvcamGetCamerasCount() found %d cameras\n",ncameras);
#endif
    return CamerasCount;
}
///////////////////////////////////////////////////////////////////////////////
/* get/set the property of the camera. returns 0 if the property is not supported */
int cvcamGetProperty(int cameraid, const char* property, void* value)
{
    CvVideoCamera *const camera = &(cameras[cameraid]);

#ifdef DEBUG_CVCAM
    printf("cvcamGetProperty(camera=%d,property=%s)\n",cameraid, property);
#endif
    if(!cameras||cameraid>ncameras-1||cameraid<0)
    {
        fprintf(stderr,"cvcamGetProperty:no such camera\n");
        return 0;
    }//if(!cameras||camera>ncameras-1||camera<0)

    IF_GETTING_PROPERTY("description")
    {
        strcpy(((CameraDescription*)value)->DeviceDescription,camera->description.DeviceDescription);
        strcpy(((CameraDescription*)value)->device, camera->description.device);
        strcpy(((CameraDescription*)value)->ChannelDescription,camera->description.ChannelDescription);
        
        ((CameraDescription*)value)->channel = camera->description.channel;
        
        ((CameraDescription*)value)->maxwidth = camera->description.maxwidth;
        ((CameraDescription*)value)->maxheight = camera->description.maxheight;
        ((CameraDescription*)value)->minwidth = camera->description.minwidth;
        ((CameraDescription*)value)->minheight = camera->description.minheight;
        
        return 1;
    }//IF_GETTING_PROPERTY("description")
    
    IF_GETTING_PROPERTY("enable")
    {
        *(int*)value = camera->enabled;
        return 1;
    }//IF_GETTING_PROPERTY("enable")
    
    IF_GETTING_PROPERTY("render")
    {
        *(int*)value = camera->rendered;
        //TODO: real call to rendering ioctl(if after initialisation)
        
        return 1;
    }//IF_GETTING_PROPERTY("render")
    
    IF_GETTING_PROPERTY("window")
    {
        *(Window*)value = camera->window;
        return 1;
    }//IF_GETTING_PROPERTY("window")
    
    IF_GETTING_PROPERTY("callback")
    {
        fprintf(stdout,"The property \"callback\" is set-only\n");
        return 0;
    }//IF_GETTING_PROPERTY("callback")
    
    IF_GETTING_PROPERTY("camera_pp")
    {
        icvVideoCameraPPDialog(cameraid);
        return 1;
    }//IF_GETTING_PROPERTY("camera_pp")
    
    IF_GETTING_PROPERTY("set_video_format")
    {
        /*
        if(!camera->initialised)
        {
        fprintf(stderr, "cvcamGetProperty, property = video_pp, camera\
        isn't initialised");
        return 0;
        }
        */
        ((VideoFormat*)value)->width = camera->videopp.width;
        ((VideoFormat*)value)->height = camera->videopp.height;
        
        ((VideoFormat*)value)->picture.brightness =
            camera->videopp.picture.brightness;
        
        ((VideoFormat*)value)->picture.hue =
            camera->videopp.picture.hue;
        
        ((VideoFormat*)value)->picture.colour =
            camera->videopp.picture.colour;
        
        ((VideoFormat*)value)->picture.contrast =
            camera->videopp.picture.contrast;
        
        ((VideoFormat*)value)->picture.whiteness   =
            camera->videopp.picture.whiteness;
        
        ((VideoFormat*)value)->picture.depth =
            camera->videopp.picture.depth;
        
        ((VideoFormat*)value)->picture.palette =
            camera->videopp.picture.palette;
        
        return 1;
    }//IF_GETTING_PROPERTY("set_video_format")
    
    IF_GETTING_PROPERTY("rectify_parameters")
    {
        fprintf(stdout,"TODO get prop rectify_parameters\n");
        return 1;
    }//IF_GETTING_PROPERTY("rectify_parameters")
    
    IF_GETTING_PROPERTY("stereo_parameters")
    {
        fprintf(stdout,"TODO get prop stereo_parameters\n");
        return 1;
    }//IF_GETTING_PROPERTY("stereo_parameters")
    
    IF_GETTING_PROPERTY("raw_image")
    {
        *((IplImage**)value) = icvVideoGetImage(cameraid);
        if(*((IplImage**)value) != NULL)
            return 1;
        else
            return 0;
    }//IF_GETTING_PROPERTY("raw_image")
    
    IF_GETTING_PROPERTY("disparity")
    {
        fprintf(stdout,"TODO get prop disparity\n");
        return 1;
    }//IF_GETTING_PROPERTY("disparity")
    
    IF_GETTING_PROPERTY("stereo_image")
    {
        fprintf(stdout,"TODO get prop stereo_image\n");
        return 1;
    }//IF_GETTING_PROPERTY("stereo_image")
    
    IF_GETTING_PROPERTY("video_pp")
    {
        icvVideoVideoPPDialog(cameraid);
        return 1;
    }//IF_GETTING_PROPERTY("video_pp")

    IF_GETTING_PROPERTY(CVCAM_RNDWIDTH)
    {
        *(int*)value = camera->renderwidth;
        return 1;
    }//IF_GETTING_PROPERTY (CVCAM_RNDWIDTH)

    IF_GETTING_PROPERTY(CVCAM_RNDHEIGHT)
    {
     *(int*)value = camera->renderheight;
        return 1;
    }//IF_GETTING_PROPERTY("CVCAM_RNDHEIGHT")


    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////


/* gets all property names. the actual number of properties is returned. */
int cvcamGetPropertiesList(int camera, const char** properties, int count)
{
#ifdef DEBUG_CVCAM
    printf("cvcamGetPropertiesList()\n");
#endif
    assert(properties);
    
    properties[0] = "description" ;
    properties[1] = "enable";
    properties[2] = "render";
    properties[3] = "window";
    properties[4] = "callback";
    properties[5] = "camera_pp";
    properties[6] = "video_pp" ;
    properties[7] = "raw_image";
    properties[8] = "set_video_format" ;
    
    return 9;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


int cvcamSetProperty(int cameraid, const char* property, void* value)
{
    int k;
    CvVideoCamera *const camera = &(cameras[cameraid]);
    
#ifdef DEBUG_CVCAM
    printf("cvcamSetProperty(camera=%d,property=%s)\n",cameraid, property);
#endif
    if(!cameras||cameraid>ncameras-1||cameraid<0)
    {
        fprintf(stderr,"cvcamSetProperty: no such camera\n");
        return 0;
    }//if(!cameras||camera>ncameras-1)
    
    IF_SETTING_PROPERTY("description")
    {
        fprintf(stdout, "cvcamSetProperty: The property \"description\" cannot be "
                        "set\n");
        return 0;
    }//IF_SETTING_PROPERTY("description")
    
    IF_SETTING_PROPERTY("enable")
    {
        camera->enabled = (int)value;
        return 1;
    }//IF_SETTING_PROPERTY("enable")
    
    IF_SETTING_PROPERTY("render")
    {
        camera->rendered = (int)value;
        return 1;
    }//IF_SETTING_PROPERTY("render")
    
    IF_SETTING_PROPERTY("window")
    {
        camera->window = *(Window*)value;
        
        //fprintf(stdout,"TODO get prop window\n");
        return 1;
    }//IF_SETTING_PROPERTY("window")
    
    IF_SETTING_PROPERTY("callback")
    {
        camera->callback = ( void (*)(IplImage*) )value;
        return 1;
    }//IF_SETTING_PROPERTY("callback")

    IF_SETTING_PROPERTY("camera_pp")
    {
        fprintf(stdout,"TODO get prop camera_pp\n");
        return 1;
    }//IF_SETTING_PROPERTY("camera_pp")
    
    IF_SETTING_PROPERTY("description")
    {
        fprintf(stdout,"TODO get prop camera_pp\n");
        return 1;
    }//IF_SETTING_PROPERTY("camera_pp")
    
    IF_SETTING_PROPERTY("set_video_format")
    {
        k = icvVideoStop(cameraid);
        if(
            (((VideoFormat*)value)->picture.brightness>0xffff)||
            (((VideoFormat*)value)->picture.hue>0xffff)||
            (((VideoFormat*)value)->picture.colour>0xffff)||
            (((VideoFormat*)value)->picture.contrast>0xffff)||
            (((VideoFormat*)value)->picture.whiteness>0xffff)||
            (((VideoFormat*)value)->width > camera->description.maxwidth)||
            (((VideoFormat*)value)->height > camera->description.maxheight)||
            (((VideoFormat*)value)->width < camera->description.minwidth)||
            (((VideoFormat*)value)->height < camera->description.minheight)
            )
        {
#ifdef DEBUG_CVCAM
            printf("cvcamSetProperty(camera=%d,property=%s): values out of bounds\n",cameraid, property);
#endif
            if(k)
                icvVideoStart(cameraid);
            return 0;
        }
        
        if(
            ((((VideoFormat*)value)->width == camera->description.maxwidth)&&
            (((VideoFormat*)value)->height == camera->description.maxheight))||
            ((((VideoFormat*)value)->width == camera->description.minwidth)&&
            (((VideoFormat*)value)->height == camera->description.minheight))||
            ((((VideoFormat*)value)->width == 320)&&                
            (((VideoFormat*)value)->height == 240))
            )
        {
            camera->videopp.width = ((VideoFormat*)value)->width;
            camera->videopp.height= ((VideoFormat*)value)->height;
        }
        else
        {
            if(
                ((((VideoFormat*)value)->width > 320)&&
                ((VideoFormat*)value)->height > 240)
                )
            {
                camera->videopp.width = 320;
                camera->videopp.height= 240;
            }else
            {
                camera->videopp.width=
                    camera->description.minwidth;
                
                camera->videopp.height=
                    camera->description.minheight;
            }
        }
        
        camera->videopp.picture.brightness=
            ((VideoFormat*)value)->picture.brightness;
        
        camera->videopp.picture.hue=
            ((VideoFormat*)value)->picture.hue;
        
        camera->videopp.picture.colour=
            ((VideoFormat*)value)->picture.colour;
        
        camera->videopp.picture.contrast=
            ((VideoFormat*)value)->picture.contrast ;
        
        camera->videopp.picture.whiteness=
            ((VideoFormat*)value)->picture.whiteness;
        
        camera->videopp.picture.palette=
            ((VideoFormat*)value)->picture.palette;
        if(camera->videopp.picture.palette == VIDEO_PALETTE_YUV420P)
            camera->imagepalette = VIDEO_PALETTE_RGB24;

        camera->videopp.picture.depth = icvVideoFormat2Depth(camera->imagepalette);
        
        if(!camera->videopp.picture.depth)
            return 0;


        //cvcamInit();

        if(k)
            icvVideoStart(cameraid);
        //fprintf(stdout,"TODO get prop video_pp\n");
        return 1;
    }//IF_SETTING_PROPERTY("set_video_format")
    
    IF_SETTING_PROPERTY("rectify_parameters")
    {
        fprintf(stdout,"TODO get prop rectify_parameters\n");
        return 1;
    }//IF_SETTING_PROPERTY("rectify_parameters")
    
    IF_SETTING_PROPERTY("stereo_parameters")
    {
        fprintf(stdout,"TODO get prop stereo_parameters\n");
        return 1;
    }//IF_SETTING_PROPERTY("stereo_parameters")
    
    IF_SETTING_PROPERTY("raw_image")
    {
        fprintf(stdout,"TODO get prop raw_image\n");
        return 1;
    }//IF_SETTING_PROPERTY("raw_image")
    
    IF_SETTING_PROPERTY("disparity")
    {
        fprintf(stdout,"TODO get prop disparity\n");
        return 1;
    }//IF_SETTING_PROPERTY("disparity")
    
    IF_SETTING_PROPERTY("stereo_image")
    {
        fprintf(stdout,"TODO get prop stereo_image\n");
        return 1;
    }//IF_SETTING_PROPERTY("stereo_image")

    IF_SETTING_PROPERTY("video_pp")
    {
        fprintf(stdout,"TODO get prop set_video_format\n");
        return 1;
    }//IF_SETTING_PROPERTY( "video_pp" )


    IF_SETTING_PROPERTY(CVCAM_RNDWIDTH)
    {
        k=icvVideoStop(cameraid);
        camera->renderwidth = *(int*)value;
        if(k)
            icvVideoStart(cameraid);
        return 1;

    }//IF_SETTING_PROPERTY(CVCAM_RNDWIDTH)

    IF_SETTING_PROPERTY(CVCAM_RNDHEIGHT)
    {
        k=icvVideoStop(cameraid);
        camera->renderheight = *(int*)value;
        if(k)
            icvVideoStart(cameraid);
        return 1;

    }//IF_SETTING_PROPERTY(CVCAM_RNDHEIGHT)

    return 0;
}
///////////////////////////////////////////////////////////////////////////////

/* Prepares the currently enabled cameras for work */
int cvcamInit()
{
    int camera;
#ifdef DEBUG_CVCAM
    printf("cvcamInit()\n");
#endif
    for(camera=0; camera<ncameras; camera++)
    {
        icvVideoInit(camera);
    }//for(camera=0; camera<ncameras; camera++)

    return 1;
}

///////////////////////////////////////////////////////////////////////////////
/* Start the video */
int cvcamStart()
{
    int cameraid;
    
#ifdef DEBUG_CVCAM
    printf("cvcamStart()\n");
#endif
    for(cameraid=0; cameraid<ncameras; cameraid++)
    {
        if(cameras[cameraid].enabled)
        {
            if(!icvVideoStart(cameraid))
            {
                return 0;
            }
        }
    }//for(cameraid=0; cameraid<ncameras; cameraid++)

    return 1;
}

int icvVideoStart(int cameraid)
{
    CvVideoCamera *const camera = &(cameras[cameraid]);
    int frame;

    pthread_mutex_lock(&(camera->capturestatemutex));
    if(camera->capturestate != READY)
    {
        fprintf(stderr,"icvVideoStart: camera %d not ready\n",camera);
        pthread_mutex_unlock(&(camera->capturestatemutex));
        return 0;
    }
    camera->capturestate = STARTING;
    pthread_cond_signal(&(camera->capturestatecond));
    pthread_mutex_unlock(&(camera->capturestatemutex));

    pthread_mutex_lock(&(camera->ioctlmutex));
    if(ioctl(camera->device, VIDIOCGMBUF,
             &(camera->mbuf)) == -1)
    {
        pthread_mutex_unlock(&(camera->ioctlmutex));
        return 0;
    }
    pthread_mutex_unlock(&(camera->ioctlmutex));
#ifdef DEBUG_CVCAM
    printf("mbuf: size=%d, frames=%d, offsets=",camera->mbuf.size,
           camera->mbuf.frames);
#endif
    camera->memorymap = (char*)mmap(0,
                    camera->mbuf.size,
                    PROT_READ|PROT_WRITE, MAP_SHARED,
                    camera->device,
                    0);
        
    if((int)camera->memorymap == -1)
        {
        fprintf(stderr, "icvVideoStart: mmap failed");
        return 0;
    }

    // acquire first frame
    camera->vid_mmap.frame  = 0;
    camera->vid_mmap.width  = camera->videopp.width;
    camera->vid_mmap.height = camera->videopp.height;
    camera->vid_mmap.format = camera->videopp.picture.palette;
    camera->lastframe = 0;

    pthread_mutex_lock(&(camera->ioctlmutex));
    
    if(ioctl(camera->device, VIDIOCMCAPTURE, &(camera->vid_mmap)) < 0)
    {
        fprintf(stderr, "icvVideoInit: ioctl VIDIOCMCAPTURE failed \n");
        return 0;
    }
    if(ioctl(camera->device, VIDIOCSYNC, &(camera->vid_mmap.frame) ) < 0)
    {
        fprintf(stderr, "icvVideoInit: ioctl VIDIOCSYNC failed \n");
        return 0;
    }//if ioctl(..., VIDIOCSYNK, ...)
    pthread_mutex_unlock(&(camera->ioctlmutex));
        
    assert(camera->videopp.picture.palette);
    assert(icvVideoFormat2Depth(camera->imagepalette));
    
    pthread_mutex_lock(&(camera->capturestatemutex));
    camera->capturestate = CAPTURING;
    pthread_cond_signal(&(camera->capturestatecond));
    pthread_mutex_unlock(&(camera->capturestatemutex));

    // initialize frame locks.
    pthread_mutex_init(&(camera->lastframemutex), NULL);
    camera->framelock = (pthread_rwlock_t *)malloc(camera->mbuf.frames *
                                                   sizeof(pthread_rwlock_t));
    for(frame = 0; frame < camera->mbuf.frames; frame++)
    {
        pthread_rwlock_init(&(camera->framelock[frame]),NULL);
    }

    camera->updated = 1;

    // start capture thread
    if(pthread_create((&(camera->thread)),NULL,
                      icvVideoCaptureProc, (void*)cameraid))
    {
        fprintf(stderr, "icvVideoStart: couldn't start thread for "
                "camera %d\n", camera);
        return 0;
    }//if pthread_create ...
#ifdef DEBUG_THREADS
    printf("started icvVideoCaptureProc thread %p from thread %p\n",
           camera->thread, pthread_self());
#endif
    
    if(camera->rendered)
    {
#ifdef DEBUG_CVCAM
        printf("icvVideoStart: start rendering\n");
#endif
        icvVideoRenderStart(cameraid);
    }
    return 1;
}


////////////////////////////////////////////////////////////////////////////////
void* icvVideoCaptureProc(void* arg)
{
    int cameraid = (int) arg;
    CvVideoCamera* const camera = &(cameras[cameraid]);
    
    assert(cameras != NULL);
    assert(cameraid>=0 && cameraid <ncameras);
    assert(camera->mbuf.frames>0);
    pthread_mutex_lock(&(camera->capturestatemutex));
    while(camera->capturestate == CAPTURING)
    {
        pthread_mutex_unlock(&(camera->capturestatemutex));
#ifdef DEBUG_CVCAM
        printf("*");
        fflush(stdout);
#endif
        // I don't need to rdlock lastframe, since I'm the one who writes it
        //pthread_mutex_lock(&(camera->lastframemutex));
        camera->vid_mmap.frame  = (camera->lastframe + 1)%camera->mbuf.frames;
        //pthread_mutex_unlock(&(camera->lastframemutex));

        pthread_rwlock_wrlock(&(camera->framelock[camera->vid_mmap.frame]));
        pthread_mutex_lock(&(camera->ioctlmutex));
        if(ioctl(camera->device, VIDIOCMCAPTURE, &(camera->vid_mmap)) < 0)
        {
            fprintf(stderr, "icvVideoCaptureProc: ioctl VIDIOCMCAPTURE failed \n");
            pthread_mutex_lock(&(camera->capturestatemutex));
            camera->capturestate = FINISHED;
            pthread_cond_signal(&(camera->capturestatecond));
            pthread_mutex_unlock(&(camera->capturestatemutex));
        }
        if(ioctl(camera->device, VIDIOCSYNC, &(camera->vid_mmap.frame) ) < 0)
        {
            fprintf(stderr, "icvVideoCaptureProc: ioctl VIDIOCSYNC failed \n");
            pthread_mutex_lock(&(camera->capturestatemutex));
            camera->capturestate = FINISHED;
            pthread_cond_signal(&(camera->capturestatecond));
            pthread_mutex_unlock(&(camera->capturestatemutex));
        }//if ioctl(..., VIDIOCSYNK, ...)
        pthread_mutex_unlock(&(camera->ioctlmutex));
        pthread_rwlock_unlock(&(camera->framelock[camera->vid_mmap.frame]));
        pthread_mutex_lock(&(camera->lastframemutex));
        camera->lastframe = camera->vid_mmap.frame;
        pthread_mutex_unlock(&(camera->lastframemutex));
        // if the camera is not rendered, call the callback myself
        if (!camera->rendered && (camera->callback != NULL))
        {
            IplImage *image = icvVideoGetImage(cameraid);
            if(image != NULL) {
                camera->callback(image);
                cvReleaseImage(&image);
            } else {
                fprintf(stderr, "icvVideoCaptureProc: icvVideoGetImage() returned NULL\n");
            }
        }
        // signal that the rendering should be updated, since the image changed
        if (camera->rendered)
        {
            pthread_mutex_lock(&(camera->updatedmutex));
            if(camera->updated == 0)
            {
                pthread_cond_signal(&(camera->updatedcond));
                camera->updated = 1;
            }
            pthread_mutex_unlock(&(camera->updatedmutex));
        }
        // stop here if we're paused
        pthread_mutex_lock(&(camera->pausemutex));
        pthread_mutex_unlock(&(camera->pausemutex));
        pthread_mutex_lock(&(camera->capturestatemutex));
    }
    pthread_mutex_unlock(&(camera->capturestatemutex));
    pthread_mutex_lock(&(camera->capturestatemutex));
    camera->capturestate = FINISHED;
    pthread_cond_signal(&(camera->capturestatecond));
    pthread_mutex_unlock(&(camera->capturestatemutex));
    pthread_exit(NULL);
}
////////////////////////////////////////////////////////////////////////////////

/* Frees all resources */
int cvcamExit()
{
    int cameraid, frame;

#ifdef DEBUG_CVCAM
    printf("cvcamExit(), ncameras = %d\n",ncameras);
#endif
    cvcamStop();
    for(cameraid=0; cameraid<ncameras; cameraid++) {
        CvVideoCamera *const camera = &(cameras[cameraid]);
        close(camera->device);
        pthread_mutex_destroy(&(camera->ioctlmutex));
        pthread_mutex_destroy(&(camera->pausemutex));
        pthread_mutex_destroy(&(camera->capturestatemutex));
        pthread_cond_destroy(&(camera->capturestatecond));
        for(frame=0; frame< camera->mbuf.frames; frame++)
        {
            pthread_rwlock_destroy(&(camera->framelock[frame]));
        }
        free(camera->framelock);
        pthread_mutex_destroy(&(camera->lastframemutex));
        pthread_cond_destroy(&(camera->updatedcond));
        pthread_mutex_destroy(&(camera->updatedmutex));
        camera->initialised = 0;
    }
    free(cameras);
    cameras = NULL;
    ncameras = 0;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////

int icvVideoPauseCamera(int cameraid)
{
    CvVideoCamera *const camera = &(cameras[cameraid]);

    pthread_mutex_lock(&(camera->capturestatemutex));
    if(!(camera->capturestate==CAPTURING))
    {
        pthread_mutex_unlock(&(camera->capturestatemutex));
        return 1;
    }
    pthread_mutex_unlock(&(camera->capturestatemutex));
    // taking this lock will stop grabbing
    if(!camera->paused) {
        pthread_mutex_lock(&(camera->pausemutex));
        camera->paused = 1;
    }
    return 0;
}
////////////////////////////////////////////////////////////////////////////////

int icvVideoResumeCamera(int cameraid)
{
    CvVideoCamera *const camera = &(cameras[cameraid]);

    pthread_mutex_lock(&(camera->capturestatemutex));
    if(!(camera->capturestate==CAPTURING))
    {
        pthread_mutex_unlock(&(camera->capturestatemutex));
        return 1;
    }
    pthread_mutex_unlock(&(camera->capturestatemutex));
    if(camera->paused) {
        camera->paused = 0;
        pthread_mutex_unlock(&(camera->pausemutex));
    }
    return 0;
}
////////////////////////////////////////////////////////////////////////////////

/* Pause the video; should be used for preventing data changes during frame reading 
	using "frame" and other properties */
int cvcamPause()
{
    int i;
    
#ifdef DEBUG_CVCAM
    printf("cvcamPause()\n");
#endif
    for(i=0; i<ncameras; i++)
    {
        icvVideoPauseCamera(i);
    }
    
    
    return 0;

}
////////////////////////////////////////////////////////////////////////////////

/* Resume the video */
int cvcamResume()
{
    int i;
    
#ifdef DEBUG_CVCAM
    printf("cvcamResume()\n");
#endif
    for(i=0; i<ncameras; i++)
    {
        icvVideoResumeCamera(i);
    }
    
    return 0;
    
}
////////////////////////////////////////////////////////////////////////////////

int icvVideoInit(int cameraid)
{
    struct video_channel Channel;
    struct video_capability VidCap;
    CvVideoCamera *const camera = &(cameras[cameraid]);

#ifdef DEBUG_CVCAM
    printf("icvVideoInit(%d)\n",cameraid);
#endif
    char *rgbframe;

    if(camera->initialised)
    {
    /*
    fprintf(stderr, "icvVideoInit:camera %d is already "
                        "initialized\n",camera);*/
        return 0;
    }
    
    if(camera->enabled)
    {
        pthread_mutex_lock(&(camera->ioctlmutex));
        if(ioctl(camera->device,VIDIOCSPICT,
            &(camera->videopp.picture) )==-1)
        {
            pthread_mutex_unlock(&(camera->ioctlmutex));
            fprintf(stdout, "icvVideoInit: ioctl VIDIOCSPICT failed\n");
            fprintf(stdout,"camera=%d,brightness=%d,hue=%d,colour=%d,contrast=%d,whiteness=%d,depth=%d,palette=%d\n",
                    camera,
                    camera->videopp.picture.brightness,
                    camera->videopp.picture.hue,
                    camera->videopp.picture.colour,
                    camera->videopp.picture.contrast,camera->videopp.picture.whiteness,camera->videopp.picture.depth,camera->videopp.picture.palette);
            return 0;
        }
        pthread_mutex_unlock(&(camera->ioctlmutex));
        
        Channel.norm = 1;
        Channel.channel = camera->channel;
        pthread_mutex_lock(&(camera->ioctlmutex));
        if((ioctl(camera->device,VIDIOCSCHAN, &Channel) == -1)||
            (ioctl(camera->device, VIDIOCGCAP, &VidCap)==-1))
        {
            fprintf(stderr,
                "icvVideoInit: ioctl VIDIOCSCHAN or VIDIOCGCAP failed\n");
            pthread_mutex_unlock(&(camera->ioctlmutex));
            return 0;
        }
        pthread_mutex_unlock(&(camera->ioctlmutex));
        
        pthread_mutex_lock(&(camera->ioctlmutex));
        if(ioctl(camera->device, VIDIOCGMBUF,
            &(camera->mbuf)) == -1)
        {
            pthread_mutex_unlock(&(camera->ioctlmutex));
            fprintf(stdout, "icvVideoInit: ioctl VIDIOCGMBUF failed\n");
            return 0;
        }
        pthread_mutex_unlock(&(camera->ioctlmutex));
        
        pthread_mutex_lock(&(camera->ioctlmutex));
        if(ioctl(camera->device,
            VIDIOCSCHAN, &Channel)==-1)
        {
            fprintf(stderr, "icvVideoInit: couldn't set channel %d",
                Channel.channel);
            pthread_mutex_unlock(&(camera->ioctlmutex));
            return 0;
        }
        pthread_mutex_unlock(&(camera->ioctlmutex));
        
        camera->initialised = 1;
        pthread_mutex_lock(&(camera->capturestatemutex));
        camera->capturestate = READY;
        pthread_cond_signal(&(camera->capturestatecond));
        pthread_mutex_unlock(&(camera->capturestatemutex));
        
   }//if(camera->IsEnabled)
   return 1;
}
////////////////////////////////////////////////////////////

int icvVideoStop(int cameraid)
{
    int i, frame;
    CvVideoCamera *const camera = &(cameras[cameraid]);

#ifdef DEBUG_CVCAM
    printf("icvVideoStop(%d)\n",cameraid);
#endif
    pthread_mutex_lock(&(camera->capturestatemutex));
    if(camera->capturestate != CAPTURING)
    {
#ifdef DEBUG_CVCAM
        printf("icvVideoStop: not capturing (state=%d), returning 0\n",camera->capturestate);
#endif
        pthread_mutex_unlock(&(camera->capturestatemutex));
        return 0;
    }
    camera->capturestate = STOPPING;
    pthread_cond_signal(&(camera->capturestatecond));
    pthread_mutex_unlock(&(camera->capturestatemutex));
    if(camera->paused)
        icvVideoResumeCamera(cameraid);

    pthread_mutex_lock(&(camera->capturestatemutex));
    // wait for the end of capture proc
    while (camera->capturestate != FINISHED )
    {
        pthread_cond_wait(&(camera->capturestatecond),&(camera->capturestatemutex));
    }
    // wait for the end of rendering
    while (camera->renderstate)
    {
        // the rendering loop may be waiting for an update
        pthread_mutex_lock(&(camera->updatedmutex));
        camera->updated = 1;
        pthread_cond_signal(&(camera->updatedcond));
        pthread_mutex_unlock(&(camera->updatedmutex));
        pthread_cond_wait(&(camera->capturestatecond),&(camera->capturestatemutex));
    }
    pthread_mutex_unlock(&(camera->capturestatemutex));

    munmap(camera->memorymap,
        camera->mbuf.size);
    
    pthread_mutex_lock(&(camera->capturestatemutex));
    camera->capturestate = READY;
    pthread_cond_signal(&(camera->capturestatecond));
    pthread_mutex_unlock(&(camera->capturestatemutex));
#ifdef DEBUG_CVCAM
        printf("icvVideoStop: returning 1\n",camera->capturestate);
#endif
    return 1;
}
//////////////////////////////////////////////////////////////////

/* Stop the video */
int cvcamStop()
{
    int camera;
#ifdef DEBUG_CVCAM
    printf("cvcamStop()\n");
#endif
    for(camera=0; camera<ncameras; camera++)
    {
        icvVideoStop(camera);
    }//for(camera=0; camera<ncameras; camera++)
    return 1;
}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
int icvVideoFormat2Depth(int format)
{
    switch (format)
    {
    case VIDEO_PALETTE_RGB24:
        return 24;
        
    case VIDEO_PALETTE_RGB32:
        return 32;
        
    case VIDEO_PALETTE_RGB565:
        return 16;
        
    case VIDEO_PALETTE_RGB555:
        return 16;
        
    default:
        return 0;
    }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int icvVideoSetDefaultValues(int cameraid, int device, int channel, 
     struct video_capability VidCap)
{
    CvVideoCamera *const camera = &(cameras[cameraid]);

    camera->device                = device;
    camera->channel               = channel;
    camera->description.channel   = channel;
    pthread_mutex_init(&(camera->ioctlmutex), NULL);
    camera->enabled               = 0;
    camera->rendered              = 0;
    camera->initialised           = 0;
    camera->paused                = 0;
    pthread_mutex_init(&(camera->pausemutex), NULL);
    pthread_mutex_init(&(camera->capturestatemutex), NULL);
    pthread_cond_init(&(camera->capturestatecond), NULL);
    camera->capturestate          = FINISHED;
    camera->memorymap             = NULL;
    camera->renderstate           = 0;
    pthread_mutex_init(&(camera->updatedmutex), NULL);
    pthread_cond_init(&(camera->updatedcond), NULL);
    camera->updated               = 0;
    camera->videopp.picture.palette = VIDEO_PALETTE_RGB24;
    camera->imagepalette = VIDEO_PALETTE_RGB24;
    
    camera->description.maxwidth    = VidCap.maxwidth;
    
    camera->description.maxheight   = VidCap.maxheight;
    
    camera->description.minwidth    = VidCap.minwidth;

    if(camera->description.minwidth<160)
        camera->description.minwidth = 160;
    
    camera->description.minheight   = VidCap.minheight;
    
    if(camera->description.minheight<120)
        camera->description.minheight = 120;
    
    if(ioctl(device,VIDIOCGPICT,
        &(camera->videopp.picture) )==-1)
        return 0;   
    
    camera->videopp.picture.palette   = VIDEO_PALETTE_RGB24;
    camera->imagepalette = VIDEO_PALETTE_RGB24;
    camera->videopp.width             = camera->description.maxwidth;
    
    camera->videopp.height            = camera->description.maxheight;
    camera->videopp.picture.depth      = 24;
    camera->videopp.picture.brightness = 30000;
    camera->videopp.picture.hue        = 30000;
    camera->videopp.picture.colour     = 30000;
    camera->videopp.picture.contrast   = 30000;
    camera->videopp.picture.whiteness  = 30000;
    camera->callback                   = NULL;

    camera->renderwidth                = 0;
    camera->renderheight               = 0;
    
    if(ioctl(device,VIDIOCSPICT,
        &(camera->videopp.picture) )==-1)
    {
#ifdef DEBUG_CVCAM
        fprintf(stdout, "icvSetDefaultValues: ioctl VIDIOCSPICT failed, trying YUV420P format\n");
#endif
        camera->videopp.picture.palette   = VIDEO_PALETTE_YUV420P;
        if(ioctl(device,VIDIOCSPICT,
                 &(camera->videopp.picture) )==-1)
        {
            fprintf(stdout, "icvSetDefaultValues: ioctl VIDIOCSPICT failed even in YUV420P format\n");
#ifdef DEBUG_CVCAM
            fprintf(stdout,"camera=%d,brightness=%d,hue=%d,colour=%d,contrast=%d,whiteness=%d,depth=%d,palette=%d\n",
                    camera,
                    camera->videopp.picture.brightness,
                    camera->videopp.picture.hue,
                    camera->videopp.picture.colour,
                    camera->videopp.picture.contrast,
                    camera->videopp.picture.whiteness,
                    camera->videopp.picture.depth,
                    camera->videopp.picture.palette);
#endif
            return 0;
        }
    }
#ifdef DEBUG_CVCAM
    fprintf(stdout,"OK! camera=%d,brightness=%d,hue=%d,colour=%d,contrast=%d,whiteness=%d,depth=%d,palette=%d\n",
            camera,
                    camera->videopp.picture.brightness,
                    camera->videopp.picture.hue,
                    camera->videopp.picture.colour,
                    camera->videopp.picture.contrast,camera->videopp.picture.whiteness,camera->videopp.picture.depth,camera->videopp.picture.palette);
#endif
    return 1;
}

/**********************************************************************
 *
 * Color correction functions
 *
 **********************************************************************/

/*
 * Turn a YUV4:2:0 block into an RGB block
 *
 * Video4Linux seems to use the blue, green, red channel
 * order convention-- rgb[0] is blue, rgb[1] is green, rgb[2] is red.
 *
 * Color space conversion coefficients taken from the excellent
 * http://www.inforamp.net/~poynton/ColorFAQ.html
 * In his terminology, this is a CCIR 601.1 YCbCr -> RGB.
 * Y values are given for all 4 pixels, but the U (Pb)
 * and V (Pr) are assumed constant over the 2x2 block.
 *
 * To avoid floating point arithmetic, the color conversion
 * coefficients are scaled into 16.16 fixed-point integers.
 * They were determined as follows:
 *
 *	double brightness = 1.0;  (0->black; 1->full scale) 
 *	double saturation = 1.0;  (0->greyscale; 1->full color)
 *	double fixScale = brightness * 256 * 256;
 *	int rvScale = (int)(1.402 * saturation * fixScale);
 *	int guScale = (int)(-0.344136 * saturation * fixScale);
 *	int gvScale = (int)(-0.714136 * saturation * fixScale);
 *	int buScale = (int)(1.772 * saturation * fixScale);
 *	int yScale = (int)(fixScale);	
 */

/* LIMIT: convert a 16.16 fixed-point value to a byte, with clipping. */
#define LIMIT(x) ((x)>0xffffff?0xff: ((x)<=0xffff?0:((x)>>16)))

static inline void
move_420_block(int yTL, int yTR, int yBL, int yBR, int u, int v, 
	       int rowPixels, unsigned char * rgb, int bits)
{
	const int rvScale = 91881;
	const int guScale = -22553;
	const int gvScale = -46801;
	const int buScale = 116129;
	const int yScale  = 65536;
	int r, g, b;

	g = guScale * u + gvScale * v;
//	if (force_rgb) {
//		r = buScale * u;
//		b = rvScale * v;
//	} else {
		r = rvScale * v;
		b = buScale * u;
//	}

	yTL *= yScale; yTR *= yScale;
	yBL *= yScale; yBR *= yScale;

	if (bits == 24) {
		/* Write out top two pixels */
		rgb[0] = LIMIT(b+yTL); rgb[1] = LIMIT(g+yTL);
		rgb[2] = LIMIT(r+yTL);

		rgb[3] = LIMIT(b+yTR); rgb[4] = LIMIT(g+yTR);
		rgb[5] = LIMIT(r+yTR);

		/* Skip down to next line to write out bottom two pixels */
		rgb += 3 * rowPixels;
		rgb[0] = LIMIT(b+yBL); rgb[1] = LIMIT(g+yBL);
		rgb[2] = LIMIT(r+yBL);

		rgb[3] = LIMIT(b+yBR); rgb[4] = LIMIT(g+yBR);
		rgb[5] = LIMIT(r+yBR);
	} else if (bits == 16) {
		/* Write out top two pixels */
		rgb[0] = ((LIMIT(b+yTL) >> 3) & 0x1F) 
			| ((LIMIT(g+yTL) << 3) & 0xE0);
		rgb[1] = ((LIMIT(g+yTL) >> 5) & 0x07)
			| (LIMIT(r+yTL) & 0xF8);

		rgb[2] = ((LIMIT(b+yTR) >> 3) & 0x1F) 
			| ((LIMIT(g+yTR) << 3) & 0xE0);
		rgb[3] = ((LIMIT(g+yTR) >> 5) & 0x07) 
			| (LIMIT(r+yTR) & 0xF8);

		/* Skip down to next line to write out bottom two pixels */
		rgb += 2 * rowPixels;

		rgb[0] = ((LIMIT(b+yBL) >> 3) & 0x1F)
			| ((LIMIT(g+yBL) << 3) & 0xE0);
		rgb[1] = ((LIMIT(g+yBL) >> 5) & 0x07)
			| (LIMIT(r+yBL) & 0xF8);

		rgb[2] = ((LIMIT(b+yBR) >> 3) & 0x1F)
			| ((LIMIT(g+yBR) << 3) & 0xE0);
		rgb[3] = ((LIMIT(g+yBR) >> 5) & 0x07)
			| (LIMIT(r+yBR) & 0xF8);
	}
}

/* Converts from planar YUV420 to RGB24. */
static void 
yuv420p_to_rgb(int width, int height,
	       unsigned char *pIn0, unsigned char *pOut0, int bits)
{
	const int numpix = width * height;
	const int bytes = bits >> 3;
	int i, j, y00, y01, y10, y11, u, v;
	unsigned char *pY = pIn0;
	unsigned char *pU = pY + numpix;
	unsigned char *pV = pU + numpix / 4;
	unsigned char *pOut = pOut0;

	for (j = 0; j <= height - 2; j += 2) {
		for (i = 0; i <= width - 2; i += 2) {
			y00 = *pY;
			y01 = *(pY + 1);
			y10 = *(pY + width);
			y11 = *(pY + width + 1);
			u = (*pU++) - 128;
			v = (*pV++) - 128;

			move_420_block(y00, y01, y10, y11, u, v,
				       width, pOut, bits);
	
			pY += 2;
			pOut += 2 * bytes;

		}
		pY += width;
		pOut += width * bytes;
	}
}


// allocates a nes iplimage containing the last grabbed image.
// Can be called from the main thread or the render thread, via
//   GetProperty("raw_image"), or from the capturing thread, via
//   the callback
static IplImage *
icvVideoGetImage(int cameraid)
{
    CvSize size;
    CvVideoCamera *const camera = &(cameras[cameraid]);
    IplImage *image;
    void *data;
    int lastframe;
    int depth = icvVideoFormat2Depth(camera->imagepalette);

    pthread_mutex_lock(&(camera->capturestatemutex));
    if(!(camera->capturestate==CAPTURING))
    {
        pthread_mutex_unlock(&(camera->capturestatemutex));
        return NULL;
    }
    pthread_mutex_unlock(&(camera->capturestatemutex));
    size.width = camera->videopp.width;
    size.height = camera->videopp.height;
    
    assert(camera->videopp.picture.palette);
    assert(icvVideoFormat2Depth(camera->imagepalette));

    image = cvCreateImageHeader(size,IPL_DEPTH_8U,depth/8);
    pthread_mutex_lock(&(camera->lastframemutex));
    lastframe = camera->lastframe;
    pthread_mutex_unlock(&(camera->lastframemutex));
    pthread_rwlock_rdlock(&(camera->framelock[lastframe]));
    if (camera->imagepalette == camera->videopp.picture.palette)
    {
        IplImage *buffer = cvCreateImageHeader(size,IPL_DEPTH_8U,depth/8);
        cvSetImageData(buffer,
                       (void*)(camera->memorymap+
                       camera->mbuf.offsets[camera->lastframe]),
                       size.width*(depth/8));
        cvCreateImageData(image);
        cvCopyImage(buffer, image);
        pthread_rwlock_unlock(&(camera->framelock[lastframe]));
        cvReleaseImageHeader(&buffer);
        return image;
    }
    // only one conversion is implemented, for now
    assert(camera->videopp.picture.palette == VIDEO_PALETTE_YUV420P);
    assert((camera->imagepalette == VIDEO_PALETTE_RGB24) ||
           (camera->imagepalette == VIDEO_PALETTE_RGB565));
    
    data = (void*)cvAlloc(size.width*size.height*(depth/8));
    cvSetImageData(image, data, size.width*(depth/8));
    yuv420p_to_rgb(camera->videopp.width,
                   camera->videopp.height,
                   (unsigned char*)(camera->memorymap+
                   camera->mbuf.offsets[camera->lastframe]),
                   (unsigned char*)(data),
                   depth);
    pthread_rwlock_unlock(&(camera->framelock[lastframe]));
    return image;
}

//Stubs for avi files

/*Plays a specified avi file into a specified window
if file is NULL, file browser is opened. if window is 0,
it is created. width and height mean output size's 0 means
those of avi file are used. __cdecl (*callback)(IplImage*) would be
called on every frame. NULL means no callback*/
CVCAM_API int cvcamPlayAVI(const char* file,
                           void* window,
                           int width,
                           int height,
                           void* callback)
{

return -1;
}


/*Advanced API for dealing with AVI files*/




/*Opens a given file or pops up a dialog if file is NULL
returns a handle to the file opened for success or -1 for failure*/
CVCAM_API cvcamAVIFILE cvcamAVIOpenFile(char* file)
{
    return (cvcamAVIFILE)-1;
}

/*The next functions just do what they say and return 0
for success, anything other for failure*/

CVCAM_API int cvcamAVICloseFile(cvcamAVIFILE file)
{
    return -1;
}

CVCAM_API int cvcamAVISetWindow(cvcamAVIFILE file, void* window)
{
    return -1;
}

CVCAM_API int cvcamAVISetCallback(cvcamAVIFILE file, void* callback)
{
    return -1;
}

CVCAM_API int cvcamAVISetSize(cvcamAVIFILE file, int width, int height)
{
    return -1;
}

CVCAM_API int cvcamAVIRun(cvcamAVIFILE file)
{
    return -1;
}

CVCAM_API int cvcamAVIStop(cvcamAVIFILE file)
{
    return -1;
}

CVCAM_API int cvcamAVIPause(cvcamAVIFILE file)
{
    return -1;
}

CVCAM_API int cvcamAVIResume(cvcamAVIFILE file)
{
    return -1;
}

CVCAM_API int cvcamAVIWaitCompletion(cvcamAVIFILE file)
{
    return -1;
}

CVCAM_API int cvcamAVIIsRunning(cvcamAVIFILE file)
{
    return -1;
}

