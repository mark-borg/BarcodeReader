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

#include <tcl.h>
#include <tk.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include "dialogs.h"
#include "cvcam.h"
#include "cvvidtypes.h"
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Toggle.h>
#include <stdlib.h>
#include "highgui.h"
////////////////////////////////////////////////////////////////////////////////
extern Widget cvvTopLevelWidget;
static int select_res = 1;
static int select_format = 0;

typedef struct
{
    __u16* value;
    Widget w;
} Tracker;

typedef struct
{
    int         n;
    VideoFormat vf;
    CameraDescription descr;
    Tracker     br;
    Tracker     con;
    Tracker     col;
    Tracker     hue;
    int         min_w;
    int         min_h;
    int         mid_w;
    int         mid_h;
    int         max_w;
    int         max_h;
    Widget dialog;
    Widget paned;

    Widget label_res;
    Widget paned_res;
    Widget label_format;
    Widget paned_format;
    Widget res_min;
    Widget res_middle;
    Widget res_max;

    Widget format_24;
    Widget format_32;
    Widget format_yuv420p;

    Widget label_brightness;
    Widget label_contrast;
    Widget label_color;
    Widget label_hue;
    Widget brightness_w;
    Widget contrast_w;
    Widget color_w;
    Widget hue_w;
    Widget br_w;
    Widget con_w;
    Widget col_w;
    Widget h_w;

    Widget buttons;
    Widget b_ok;
    Widget b_apply;
    Widget b_cancel;
} VideoWidget;

typedef struct
{
    int         n;
    CameraDescription descr;
    int         enable;
    int         render;

    Widget dialog;
    Widget label;
    Widget paned;

    Widget enable_w;
    Widget render_w;

    Widget buttons;
    Widget b_ok;
    Widget b_apply;
    Widget b_cancel;
} CameraWidget;

static void updatevalues(VideoWidget *vf);

static void on_apply( Widget w, VideoWidget* data )
{
    cvcamSetProperty(data->n, "set_video_format", &data->vf);
    XtVaSetValues( w, XtNstate, False, 0 );
    updatevalues(data);
}

static void on_cancel( Widget w, VideoWidget* data )
{
    XtDestroyWidget( data->dialog );
    free( data );
}

static void on_ok( Widget w, VideoWidget* data )
{
    on_apply( w, data );
    on_cancel( w, data );
}

static void on_track( Widget w, Tracker* data, float* percent )
{
    char s[100];
    __u16 value = (__u16)(*percent * 0xFFFF);
    *(data->value) = value;
    sprintf( s, "%d", value );
    XtVaSetValues( data->w, XtNlabel, s, 0 );
}

static void set_float( Widget w, char* name, float value )
{
    Arg   args[1];

    if (sizeof(float) > sizeof(XtArgVal))
        XtSetArg(args[0], name, &value);
    else
    {
        union {
            XtArgVal xt;
            float   fp;
        } foo;
        foo.fp = value;
        XtSetArg(args[0], name, foo.xt);
    }
    XtSetValues(w,args,1);
}

static void on_24( Widget w, VideoWidget* v )
{
    int state = 1;
    XtVaGetValues( w, XtNstate, &state, 0 );
    if( !state )
        return;
    v->vf.picture.palette = VIDEO_PALETTE_RGB24;
}

static void on_32( Widget w, VideoWidget* v )
{
    int state = 1;
    XtVaGetValues( w, XtNstate, &state, 0 );
    if( !state )
        return;
    v->vf.picture.palette = VIDEO_PALETTE_RGB32;
}

static void on_yuv420p( Widget w, VideoWidget* v )
{
    int state = 1;
    XtVaGetValues( w, XtNstate, &state, 0 );
    if( !state )
        return;
    v->vf.picture.palette = VIDEO_PALETTE_YUV420P;
}

static void on_min( Widget w, VideoWidget* v )
{
    int state = 1;
    XtVaGetValues( w, XtNstate, &state, 0 );
    if( !state )
        return;
    v->vf.width = v->min_w;
    v->vf.height = v->min_h;
}

static void on_mid( Widget w, VideoWidget* v )
{
    int state = 1;
    XtVaGetValues( w, XtNstate, &state, 0 );
    if( !state )
        return;
    v->vf.width = v->mid_w;
    v->vf.height = v->mid_h;
}

static void on_max( Widget w, VideoWidget* v )
{
    int state = 1;
    XtVaGetValues( w, XtNstate, &state, 0 );
    if( !state )
        return;
    v->vf.width = v->max_w;
    v->vf.height = v->max_h;
}

static void updatevalues(VideoWidget *vf)
{
    int camera = vf->n;
    float percent;

    cvcamGetProperty(camera, "set_video_format", &vf->vf);
    if( vf->vf.width == vf->descr.minwidth )
        XtVaSetValues( vf->res_min, XtNstate, True, 0 );
    else if( vf->vf.width == vf->descr.maxwidth )
        XtVaSetValues( vf->res_max, XtNstate, True, 0 );
    else
        XtVaSetValues( vf->res_middle, XtNstate, True, 0 );

    switch( vf->vf.picture.palette )
    {
        case VIDEO_PALETTE_RGB24:
            XtVaSetValues( vf->format_24, XtNstate, True, 0 );
            break;
        case VIDEO_PALETTE_RGB32:
            XtVaSetValues( vf->format_32, XtNstate, True, 0 );
            break;
        case VIDEO_PALETTE_YUV420P:
            XtVaSetValues( vf->format_yuv420p, XtNstate, True, 0 );
            break;
        default:
            XtDestroyWidget( vf->dialog );
            free( vf );
            printf( "supported only RGB24, RGB32 & YUV420P formats\n" );
            return;
    }

    set_float( vf->br_w, XtNtopOfThumb, (float)vf->vf.picture.brightness / 0xFFFF );
    percent = (float)vf->vf.picture.brightness / 0xFFFF;
    vf->br.value = &vf->vf.picture.brightness;
    on_track(vf->br_w, &vf->br, &percent );

    set_float( vf->con_w, XtNtopOfThumb, (float)vf->vf.picture.contrast / 0xFFFF );
    percent = (float)vf->vf.picture.contrast / 0xFFFF;
    vf->con.value = &vf->vf.picture.contrast;
    on_track(vf->con_w, &vf->con, &percent );

    set_float( vf->col_w, XtNtopOfThumb, (float)vf->vf.picture.colour / 0xFFFF );
    percent = (float)vf->vf.picture.colour / 0xFFFF;
    vf->col.value = &vf->vf.picture.colour;
    on_track(vf->col_w, &vf->col, &percent );

    set_float( vf->h_w, XtNtopOfThumb, (float)vf->vf.picture.hue / 0xFFFF );
    percent = (float)vf->vf.picture.hue / 0xFFFF;
    vf->hue.value = &vf->vf.picture.hue;
    on_track(vf->h_w, &vf->hue, &percent );
}

void icvVideoVideoPPDialog(int camera)
{

    char title[100];

    VideoWidget* vf = (VideoWidget*)malloc( sizeof(*vf) );
    vf->n = camera;
    cvcamGetProperty(camera, "description", &vf->descr);

    snprintf( title, 100, "%s, %s (%s): Video format", vf->descr.device, vf->descr.DeviceDescription, vf->descr.ChannelDescription );
//    snprintf( title, 100, "%d", camera);
    
    cvvInitSystem(0, 0);
    /* Creating main dialog window */
    vf->dialog = XtVaAppCreateShell( title, "video", topLevelShellWidgetClass,
                                XtDisplay( /*(void*)*/cvvTopLevelWidget ),
                                XtNminWidth, 10, XtNminHeight, 10,
                                XtNinput, 1, 0 );
    /* Creating main paned window... It will parent for all smaller elements */
    vf->paned = XtVaCreateManagedWidget( "CvVisual_paned", panedWidgetClass, vf->dialog,
                                     0 );
    /* Resolution elements */
    vf->label_res = XtVaCreateManagedWidget( "resolution", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNlabel, "camera resolution",
                                     XtNjustify, XtJustifyCenter, 0 );

    vf->paned_res = XtVaCreateManagedWidget( "resolution_res", panedWidgetClass, vf->paned,
                                         XtNshowGrip, False,
                                         XtNorientation, XtorientHorizontal,
                                         0 );
    
    sprintf( title, "%dx%d", vf->descr.minwidth, vf->descr.minheight );
    vf->res_min = XtVaCreateManagedWidget( "res min", toggleWidgetClass, vf->paned_res,
                                       XtNlabel, title,
                                       XtNshowGrip, False,
                                       XtNradioData, &select_res,
                                       0 );
    XtAddCallback( vf->res_min, "callback", (XtCallbackProc)on_min, vf );
    vf->min_w = vf->descr.minwidth;
    vf->min_h = vf->descr.minheight;

    vf->res_middle = XtVaCreateManagedWidget( "res middle", toggleWidgetClass, vf->paned_res,
                                       XtNlabel, "320x240",
                                       XtNshowGrip, False,
                                       XtNradioData, &select_res,
                                       XtNradioGroup, vf->res_min,
                                       0 );
    XtAddCallback( vf->res_middle, "callback", (XtCallbackProc)on_mid, vf );
    vf->mid_w = 320;
    vf->mid_h = 240;

    sprintf( title, "%dx%d", vf->descr.maxwidth, vf->descr.maxheight );
    vf->res_max = XtVaCreateManagedWidget( "res max", toggleWidgetClass, vf->paned_res,
                                       XtNlabel, title,
                                       XtNshowGrip, False,
                                       XtNradioData, &select_res,
                                       XtNradioGroup, vf->res_min,
                                       0 );
    XtAddCallback( vf->res_max, "callback", (XtCallbackProc)on_max, vf );
    vf->max_w = vf->descr.maxwidth;
    vf->max_h = vf->descr.maxheight;

    /* Video format elements */
    vf->label_format = XtVaCreateManagedWidget( "format", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNlabel, "data format",
                                     XtNjustify, XtJustifyCenter, 0 );

    vf->paned_format = XtVaCreateManagedWidget( "format", panedWidgetClass, vf->paned,
                                         XtNshowGrip, False,
                                         XtNorientation, XtorientHorizontal,
                                         0 );

    vf->format_24 = XtVaCreateManagedWidget( "bpp24", toggleWidgetClass, vf->paned_format,
                                         XtNlabel, "RGB24",
                                         XtNshowGrip, False,
                                         XtNradioData, &select_format,
                                         0 );
    XtAddCallback( vf->format_24, "callback", (XtCallbackProc)on_24, vf );

    vf->format_32 = XtVaCreateManagedWidget( "bpp32", toggleWidgetClass, vf->paned_format,
                                       XtNlabel, "RGB32",
                                       XtNshowGrip, False,
                                       XtNradioData, &select_format,
                                       XtNradioGroup, vf->format_24,
                                       0 );
    XtAddCallback( vf->format_32, "callback", (XtCallbackProc)on_32, vf );

    vf->format_yuv420p = XtVaCreateManagedWidget( "yuv420p", toggleWidgetClass, vf->paned_format,
                                         XtNlabel, "YUV420P",
                                         XtNshowGrip, False,
                                         XtNradioData, &select_format,
                                         XtNradioGroup, vf->format_24,
                                         0 );
    XtAddCallback( vf->format_yuv420p, "callback", (XtCallbackProc)on_yuv420p, vf );

    /* Brightness, contrast, color & hue elements */
    /* brightness */
    vf->label_brightness = XtVaCreateManagedWidget( "brigthness", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNlabel, "brigthness",
                                     XtNjustify, XtJustifyCenter, 0 );
    vf->brightness_w = XtVaCreateManagedWidget( "brigthness", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNjustify, XtJustifyCenter, 0 );
    vf->br.w = vf->brightness_w;
    vf->br_w = XtVaCreateManagedWidget( "br", scrollbarWidgetClass, vf->paned,
                                  XtNshowGrip, False,
                                  XtNorientation, XtorientHorizontal, 0 );
    XtAddCallback( vf->br_w, "jumpProc", (XtCallbackProc)on_track, &vf->br );

    /* contrast */
    vf->label_contrast = XtVaCreateManagedWidget( "contrast", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNlabel, "contrast",
                                     XtNjustify, XtJustifyCenter, 0 );
    vf->contrast_w = XtVaCreateManagedWidget( "brigthness", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNjustify, XtJustifyCenter, 0 );
    vf->con.w = vf->contrast_w;
    vf->con_w = XtVaCreateManagedWidget( "con", scrollbarWidgetClass, vf->paned,
                                  XtNshowGrip, False,
                                  XtNorientation, XtorientHorizontal, 0 );
    XtAddCallback( vf->con_w, "jumpProc", (XtCallbackProc)on_track, &vf->con );

    /* colour */
    vf->label_color = XtVaCreateManagedWidget( "color", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNlabel, "color",
                                     XtNjustify, XtJustifyCenter, 0 );
    vf->color_w = XtVaCreateManagedWidget( "brigthness", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNjustify, XtJustifyCenter, 0 );
    vf->col.w = vf->color_w;
    vf->col_w = XtVaCreateManagedWidget( "col", scrollbarWidgetClass, vf->paned,
                                  XtNshowGrip, False,
                                  XtNorientation, XtorientHorizontal, 0 );
    XtAddCallback( vf->col_w, "jumpProc", (XtCallbackProc)on_track, &vf->col );

    /* hue */
    vf->label_hue = XtVaCreateManagedWidget( "hue", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNlabel, "hue",
                                     XtNjustify, XtJustifyCenter, 0 );
    vf->hue_w = XtVaCreateManagedWidget( "brigthness", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNjustify, XtJustifyCenter, 0 );
    vf->hue.w = vf->hue_w;
    vf->h_w = XtVaCreateManagedWidget( "h", scrollbarWidgetClass, vf->paned,
                                  XtNshowGrip, False,
                                  XtNorientation, XtorientHorizontal, 0 );
    XtAddCallback( vf->h_w, "jumpProc", (XtCallbackProc)on_track, &vf->hue );

    updatevalues(vf);

    /* Buttons */
    vf->buttons = XtVaCreateManagedWidget( "buttons", panedWidgetClass, vf->paned,
                                         XtNshowGrip, False,
                                         XtNorientation, XtorientHorizontal,
                                         0 );

    vf->b_ok = XtVaCreateManagedWidget( "b_ok", toggleWidgetClass, vf->buttons,
                                       XtNlabel, "Ok",
                                       XtNshowGrip, False,
                                       XtNstate, False,
                                       0 );
    XtAddCallback( vf->b_ok, XtNcallback, (XtCallbackProc)on_ok, (void*)vf );

    vf->b_apply = XtVaCreateManagedWidget( "b_apply", toggleWidgetClass, vf->buttons,
                                       XtNlabel, "Apply",
                                       XtNshowGrip, False,
                                       XtNstate, False,
                                       0 );
    XtAddCallback( vf->b_apply, XtNcallback, (XtCallbackProc)on_apply, (void*)vf );

    vf->b_cancel = XtVaCreateManagedWidget( "b_cancel", toggleWidgetClass, vf->buttons,
                                       XtNlabel, "Cancel",
                                       XtNshowGrip, False,
                                       XtNstate, False,
                                       0 );
    XtAddCallback( vf->b_cancel, XtNcallback, (XtCallbackProc)on_cancel, (void*)vf );
    vf->n = camera;

    XtRealizeWidget( vf->dialog );

    {
        /* fixating window size */
        int width = 10;
        int height = 10;
        XtVaGetValues( vf->dialog, XtNwidth, &width, XtNheight, &height, 0 );
        XtVaSetValues( vf->dialog, XtNminWidth, width, XtNminHeight, height,
                               XtNmaxWidth, width, XtNmaxHeight, height, 0 );
    }
}

////////////////////////////////////////////////////////////////////////////////
static void on_enable( Widget w, CameraWidget* data )
{
    int status = 1;
    XtVaGetValues( w, XtNstate, &status, 0 );
    data->enable = status;
}

static void on_render( Widget w, CameraWidget* data )
{
    int status = 1;
    XtVaGetValues( w, XtNstate, &status, 0 );
    data->render = status;
}

static void on_cam_apply( Widget w, CameraWidget* data )
{
    cvcamSetProperty( data->n, "enable", &data->enable );
    cvcamSetProperty( data->n, "render", &data->render );
    XtVaSetValues( w, XtNstate, False, 0 );
}

static void on_cam_cancel( Widget w, CameraWidget* data )
{
    XtDestroyWidget( data->dialog );
    free( data );
}

static void on_cam_ok( Widget w, CameraWidget* data )
{
    on_cam_apply( w, data );
    on_cam_cancel( w, data );
}

void icvVideoCameraPPDialog(int camera)
{
    int    res;

    char title[100];

    CameraWidget* vf = (CameraWidget*)malloc( sizeof(*vf) );
    cvcamGetProperty(camera, "description", &vf->descr);
    snprintf( title, 100, "%s, %s (%s): Camera options", vf->descr.device, vf->descr.DeviceDescription, vf->descr.ChannelDescription );
    
    cvvInitSystem(0, 0);
    /* Creating main dialog window */
    vf->dialog = XtVaAppCreateShell( title, "camera", topLevelShellWidgetClass,
                                XtDisplay( cvvTopLevelWidget ),
                                XtNminWidth, 10, XtNminHeight, 10,
                                XtNinput, 1, 0 );
    /* Creating main paned window... It will parent for all smaller elements */
    vf->paned = XtVaCreateManagedWidget( "CvCamera_paned", panedWidgetClass, vf->dialog,
                                     0 );
    /* Resolution elements */
    sprintf( title, "%s on %s", vf->descr.DeviceDescription, vf->descr.device );
    vf->label = XtVaCreateManagedWidget( "desriprion", labelWidgetClass, vf->paned,
                                     XtNshowGrip, False,
                                     XtNlabel, title,
                                     XtNjustify, XtJustifyCenter, 0 );

    cvcamGetProperty(camera, "enable", &res);
    vf->enable_w = XtVaCreateManagedWidget( "enable", toggleWidgetClass, vf->paned,
                                      XtNlabel, "enable",
                                      XtNshowGrip, False,
                                      XtNradioData, &select_res,
                                      XtNstate, res,
                                      0 );
    XtAddCallback( vf->enable_w, "callback", (XtCallbackProc)on_enable, vf );

    cvcamGetProperty(camera, "render", &res);
    vf->render_w = XtVaCreateManagedWidget( "render", toggleWidgetClass, vf->paned,
                                      XtNlabel, "render",
                                      XtNshowGrip, False,
                                      XtNradioData, &select_res,
                                      XtNstate, res,
                                      0 );
    XtAddCallback( vf->render_w, "callback", (XtCallbackProc)on_render, vf );

    /* Buttons */
    vf->buttons = XtVaCreateManagedWidget( "buttons", panedWidgetClass, vf->paned,
                                         XtNshowGrip, False,
                                         XtNorientation, XtorientHorizontal,
                                         0 );

    vf->b_ok = XtVaCreateManagedWidget( "b_ok", toggleWidgetClass, vf->buttons,
                                     XtNlabel, "Ok",
                                     XtNshowGrip, False,
                                     XtNstate, False,
                                     0 );
    XtAddCallback( vf->b_ok, XtNcallback, (XtCallbackProc)on_cam_ok, (void*)vf );

    vf->b_apply = XtVaCreateManagedWidget( "b_apply", toggleWidgetClass, vf->buttons,
                                       XtNlabel, "Apply",
                                       XtNshowGrip, False,
                                       XtNstate, False,
                                       0 );
    XtAddCallback( vf->b_apply, XtNcallback, (XtCallbackProc)on_cam_apply, (void*)vf );

    vf->b_cancel = XtVaCreateManagedWidget( "b_cancel", toggleWidgetClass, vf->buttons,
                                         XtNlabel, "Cancel",
                                         XtNshowGrip, False,
                                         XtNstate, False,
                                         0 );
    XtAddCallback( vf->b_cancel, XtNcallback, (XtCallbackProc)on_cam_cancel, (void*)vf );
    XtRealizeWidget( vf->dialog );
    
    vf->n = camera;

    {
        /* fixating window size */
        int width = 10;
        int height = 10;
        XtVaGetValues( vf->dialog, XtNwidth, &width, XtNheight, &height, 0 );
        XtVaSetValues( vf->dialog, XtNminWidth, width, XtNminHeight, height,
                               XtNmaxWidth, width, XtNmaxHeight, height, 0 );
    }
}

