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

#include "_highgui.h"

#ifndef WIN32

#include "gtk/gtk.h"
#include "gdk/gdkkeysyms.h"
#include <stdio.h>

/*#if _MSC_VER >= 1200
#pragma warning( disable: 4505 )
#pragma comment(lib,"gtk-win32-2.0.lib")
#pragma comment(lib,"glib-2.0.lib")
#pragma comment(lib,"gobject-2.0.lib")
#pragma comment(lib,"gdk-win32-2.0.lib")
#pragma comment(lib,"gdk_pixbuf-2.0.lib")
#endif*/

struct CvWindow;

typedef struct CvTrackbar
{
    int signature;
    GtkWidget* widget;
    char* name;
    CvTrackbar* next;
    CvWindow* parent;
    int* data;
    int pos;
    int maxval;
    CvTrackbarCallback notify;
}
CvTrackbar;


typedef struct CvWindow
{
    int signature;
    GtkWidget* area;
    GtkWidget* frame;
    GtkWidget* paned;
    char* name;
    CvWindow* prev;
    CvWindow* next;
    
    CvMat* image;
    CvMat* dst_image;
    int converted;
    int last_key;
    int flags;

    CvMouseCallback on_mouse;

    struct
    {
        int pos;
        int rows;
        CvTrackbar* first;
    }
    toolbar;
}
CvWindow;


//#ifndef NDEBUG
#define Assert(exp)                                             \
if( !(exp) )                                                    \
{                                                               \
    printf("Assertion: %s  %s: %d\n", #exp, __FILE__, __LINE__);\
    assert(exp);                                                \
}

static void icvPutImage( CvWindow* window );
static gboolean icvOnClose( GtkWidget* widget, GdkEvent* event, gpointer user_data );
static gboolean icvOnExpose( GtkWidget* widget,
                GdkEventExpose *event, gpointer user_data );
static gboolean icvOnKeyPress( GtkWidget* widget, GdkEventKey* event, gpointer user_data );
static void icvOnTrackbar( GtkWidget* widget, gpointer user_data );
static gboolean icvOnMouse( GtkWidget *widget, GdkEvent *event, gpointer user_data );

static int             last_key = -1;
GtkWidget*             cvTopLevelWidget = 0;

static CvWindow* hg_windows = 0;

CV_IMPL int cvInitSystem( int argc, char** argv )
{
    static int wasInitialized = 0;    

    // check initialization status
    if( !wasInitialized )
    {
        hg_windows = 0;

        gtk_init( &argc, &argv );
        wasInitialized = 1;
    }

    return 0;
}

static CvWindow* icvFindWindowByName( const char* name )
{
    CvWindow* window = hg_windows;
    while( window != 0 && strcmp(name, window->name) != 0 )
        window = window->next;

    return window;
}

static CvWindow* icvWindowByWidget( GtkWidget* widget )
{
    CvWindow* window = hg_windows;

    while( window != 0 && window->area != widget &&
           window->frame != widget && window->paned != widget )
        window = window->next;

    return window;
}


static void
icvUpdateWindowSize( const CvWindow* window )
{
    int width = 0, height = 0;
    CvTrackbar* t;

    if( !window->frame )
        return;
    gtk_window_get_size( GTK_WINDOW(window->frame), &width, &height );
    if( window->image )
    {
        width = MAX(window->image->width,width);
        height = window->image->height;
    }

    //gtk_widget_translate_coordinates( window->area, window->paned, 0, 0, &dw, &dh );
    //height += dh;

    for( t = window->toolbar.first; t != 0; t = t->next )
        height += 50;
    
    //if( window->flags & CV_WINDOW_AUTOSIZE )
    //    gtk_window_set_resizable( GTK_WINDOW(window->frame), TRUE );
    gtk_window_resize( GTK_WINDOW(window->frame), width, height );
    //if( window->flags & CV_WINDOW_AUTOSIZE )
    //    gtk_window_set_resizable( GTK_WINDOW(window->frame), FALSE );
}


CV_IMPL int cvNamedWindow( const char* name, int flags )
{
    int result = 0;
    CV_FUNCNAME( "cvNamedWindow" );

    __BEGIN__;

    CvWindow* window;
    int len;

    cvInitSystem(0,0);
    if( !name )
        CV_ERROR( CV_StsNullPtr, "NULL name string" );

    // Check the name in the storage
    if( icvFindWindowByName( name ) != 0 )
    {
        result = 1;
        EXIT;
    }

    len = strlen(name);
    CV_CALL( window = (CvWindow*)cvAlloc(sizeof(CvWindow) + len + 1));
    memset( window, 0, sizeof(*window));
    window->name = (char*)(window + 1);
    memcpy( window->name, name, len + 1 );
    window->flags = flags;
    window->signature = CV_WINDOW_MAGIC_VAL;
    window->image = 0;
    window->last_key = 0;
    window->on_mouse = 0;
    memset( &window->toolbar, 0, sizeof(window->toolbar));
    window->next = hg_windows;
    window->prev = 0;
    if( hg_windows )
        hg_windows->prev = window;
    hg_windows = window;

    window->frame = gtk_window_new( GTK_WINDOW_TOPLEVEL );
    window->paned = gtk_vbox_new( FALSE, 0 );
    //window->trackbars = 0;
    window->area = gtk_drawing_area_new();
    gtk_box_pack_end( GTK_BOX(window->paned), window->area, TRUE, TRUE, 0 );
    gtk_widget_show( window->area );
    gtk_container_add( GTK_CONTAINER(window->frame), window->paned );
    gtk_widget_show( window->paned );
    gtk_widget_set_size_request( window->frame, 100, 100 );
    //gtk_window_set_transient_for( window->area, GTK_WINDOW(window->frame) );
    gtk_signal_connect( GTK_OBJECT(window->area), "expose-event",
                        GTK_SIGNAL_FUNC(icvOnExpose), window );
    gtk_signal_connect( GTK_OBJECT(window->frame), "key-press-event",
                        GTK_SIGNAL_FUNC(icvOnKeyPress), window );
    gtk_signal_connect( GTK_OBJECT(window->area), "button-press-event",
                        GTK_SIGNAL_FUNC(icvOnMouse), window );
    gtk_signal_connect( GTK_OBJECT(window->area), "button-release-event",
                        GTK_SIGNAL_FUNC(icvOnMouse), window );
    gtk_signal_connect( GTK_OBJECT(window->area), "motion-notify-event",
                        GTK_SIGNAL_FUNC(icvOnMouse), window );
    gtk_signal_connect( GTK_OBJECT(window->frame), "delete-event",
                        GTK_SIGNAL_FUNC(icvOnClose), window );
    gtk_widget_set_events( window->area, GDK_EXPOSURE_MASK | GDK_BUTTON_RELEASE_MASK |
                                         GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK );

    gtk_widget_show( window->frame );
    gtk_window_set_title( GTK_WINDOW(window->frame), name );
    //gtk_window_set_resizable( GTK_WINDOW(window->frame), (flags & CV_WINDOW_AUTOSIZE) == 0 );

    result = 1;
    __END__;

    return result;
}


static void icvDeleteWindow( CvWindow* window )
{
    CvTrackbar* trackbar;
    
    if( window->prev )
        window->prev->next = window->next;
    else
        hg_windows = window->next;

    if( window->next )
        window->next->prev = window->prev;

    window->prev = window->next = 0;

    cvReleaseMat( &window->image );
    cvReleaseMat( &window->dst_image );

    gtk_widget_destroy( window->frame );
    
    for( trackbar = window->toolbar.first; trackbar != 0; )
    {
        CvTrackbar* next = trackbar->next;
        cvFree( (void**)&trackbar );
        trackbar = next;
    }

    cvFree( (void**)&window );
}


CV_IMPL void cvDestroyWindow( const char* name )
{
    CV_FUNCNAME( "cvDestroyWindow" );
    
    __BEGIN__;

    CvWindow* window;

    if(!name)
        CV_ERROR( CV_StsNullPtr, "NULL name string" );

    window = icvFindWindowByName( name );
    if( !window )
        EXIT;

    icvDeleteWindow( window );

    __END__;
}


CV_IMPL void
cvDestroyAllWindows( void )
{
    while( hg_windows )
    {
        CvWindow* window = hg_windows;
        icvDeleteWindow( window );
    }
}


CV_IMPL void
cvShowImage( const char* name, const CvArr* arr )
{
    CV_FUNCNAME( "cvShowImage" );

    __BEGIN__;
    
    CvWindow* window;
    int origin = 0;
    CvMat stub, *image;

    if( !name )
        CV_ERROR( CV_StsNullPtr, "NULL name" );

    window = icvFindWindowByName(name);
    if( !window || !arr )
        EXIT; // keep silence here.

    if( CV_IS_IMAGE_HDR( arr ))
        origin = ((IplImage*)arr)->origin;
    
    CV_CALL( image = cvGetMat( arr, &stub ));

    if( !window->image )
        cvResizeWindow( name, image->cols, image->rows );

    if( window->image &&
        !CV_ARE_SIZES_EQ(window->image, image) )
        cvReleaseMat( &window->image );

    if( !window->image )
		window->image = cvCreateMat( image->rows, image->cols, CV_8UC3 );

    cvConvertImage( image, window->image,
        (origin != 0 ? CV_CVTIMG_FLIP) + CV_CVTIMG_SWAP_RB );
    icvPutImage( window );
    icvUpdateWindowSize( window );

    __END__;
}


CV_IMPL void cvResizeWindow(const char* name, int width, int height )
{
    CV_FUNCNAME( "cvResizeWindow" );

    __BEGIN__;
    
    CvWindow* window;
    //CvTrackbar* trackbar;

    if( !name )
        CV_ERROR( CV_StsNullPtr, "NULL name" );

    window = icvFindWindowByName(name);
    if(!window)
        EXIT;

    // TODO: take into account frame borders and trackbars
    gtk_window_resize( GTK_WINDOW(window->frame), width, height );
    //gtk_widget_set_size_request( window->area, width, height );

    __END__;
}


CV_IMPL void cvMoveWindow( const char* name, int x, int y )
{
    CV_FUNCNAME( "cvMoveWindow" );

    __BEGIN__;

    CvWindow* window;

    if( !name )
        CV_ERROR( CV_StsNullPtr, "NULL name" );

    window = icvFindWindowByName(name);
    if(!window)
        EXIT;
    
    gtk_window_move( GTK_WINDOW(window->frame), x, y );

    __END__;
}


static CvTrackbar*
icvFindTrackbarByName( const CvWindow* window, const char* name )
{
    CvTrackbar* trackbar = window->toolbar.first;

    for( ; trackbar != 0 && strcmp( trackbar->name, name ) != 0; trackbar = trackbar->next )
        ;

    return trackbar;
}


CV_IMPL int
cvCreateTrackbar( const char* trackbar_name, const char* window_name,
                  int* val, int count, CvTrackbarCallback on_notify )
{
    int result = 0;

    CV_FUNCNAME( "cvCreateTrackbar" );

    __BEGIN__;
    
    /*char slider_name[32];*/
    CvWindow* window = 0;
    CvTrackbar* trackbar = 0;

    if( !window_name || !trackbar_name )
        CV_ERROR( CV_StsNullPtr, "NULL window or trackbar name" );

    if( count <= 0 )
        CV_ERROR( CV_StsOutOfRange, "Bad trackbar maximal value" );

    window = icvFindWindowByName(window_name);
    if( !window )
        EXIT;

    trackbar = icvFindTrackbarByName(window,trackbar_name);
    if( !trackbar )
    {
        int len = strlen(trackbar_name);
        trackbar = (CvTrackbar*)cvAlloc(sizeof(CvTrackbar) + len + 1);
        memset( trackbar, 0, sizeof(*trackbar));
        trackbar->signature = CV_TRACKBAR_MAGIC_VAL;
        trackbar->name = (char*)(trackbar+1);
        memcpy( trackbar->name, trackbar_name, len + 1 );
        trackbar->parent = window;
        trackbar->next = window->toolbar.first;
        window->toolbar.first = trackbar;
        
        GtkWidget* hscale_box = gtk_hbox_new( FALSE, 10 );
        GtkWidget* hscale_label = gtk_label_new( trackbar_name );
        GtkWidget* hscale = gtk_hscale_new_with_range( 0, count, 1 );
        gtk_range_set_update_policy( GTK_RANGE(hscale), GTK_UPDATE_CONTINUOUS );
        gtk_scale_set_digits( GTK_SCALE(hscale), 0 );
        //gtk_scale_set_value_pos( hscale, GTK_POS_TOP );
        gtk_scale_set_draw_value( GTK_SCALE(hscale), TRUE );

        trackbar->widget = hscale;
        gtk_box_pack_start( GTK_BOX(hscale_box), hscale_label, FALSE, FALSE, 5 );
        gtk_widget_show( hscale_label );
        gtk_box_pack_start( GTK_BOX(hscale_box), hscale, TRUE, TRUE, 5 );
        gtk_widget_show( hscale );
        gtk_box_pack_start( GTK_BOX(window->paned), hscale_box, FALSE, FALSE, 5 );
        gtk_widget_show( hscale_box );

        icvUpdateWindowSize( window );
    }
        
    if( val )
    {
        int value = *val;
        if( value < 0 )
            value = 0;
        if( value > count )
            value = count;
        gtk_range_set_value( GTK_RANGE(trackbar->widget), value );
        trackbar->pos = value;
        trackbar->data = val;
    }
        
    trackbar->maxval = count;
    trackbar->notify = on_notify;
    gtk_signal_connect( GTK_OBJECT(trackbar->widget), "value-changed",
                        GTK_SIGNAL_FUNC(icvOnTrackbar), trackbar );

    if( (window->flags & CV_WINDOW_AUTOSIZE) && (window->image) )
        gtk_widget_set_size_request( window->frame, window->image->width, window->image->height );

    result = 1;

    __END__;

    return result;
}


CV_IMPL void
cvSetMouseCallback( const char* window_name, CvMouseCallback on_mouse )
{
    CV_FUNCNAME( "cvSetMouseCallback" );

    __BEGIN__;
    
    CvWindow* window = 0;

    if( !window_name )
        CV_ERROR( CV_StsNullPtr, "NULL window name" );

    window = icvFindWindowByName(window_name);
    if( !window )
        EXIT;

    window->on_mouse = on_mouse;

    __END__;
}


CV_IMPL int cvGetTrackbarPos( const char* trackbar_name, const char* window_name )
{
    int pos = -1;
    
    CV_FUNCNAME( "cvGetTrackbarPos" );

    __BEGIN__;

    CvWindow* window;
    CvTrackbar* trackbar = 0;

    if( trackbar_name == 0 || window_name == 0 )
        CV_ERROR( CV_StsNullPtr, "NULL trackbar or window name" );

    window = icvFindWindowByName( window_name );
    if( window )
        trackbar = icvFindTrackbarByName( window, trackbar_name );

    if( trackbar )
        pos = trackbar->pos;

    __END__;

    return pos;
}


CV_IMPL void cvSetTrackbarPos( const char* trackbar_name, const char* window_name, int pos )
{
    CV_FUNCNAME( "cvSetTrackbarPos" );

    __BEGIN__;

    CvWindow* window;
    CvTrackbar* trackbar = 0;

    if( trackbar_name == 0 || window_name == 0 )
        CV_ERROR( CV_StsNullPtr, "NULL trackbar or window name" );

    window = icvFindWindowByName( window_name );
    if( window )
        trackbar = icvFindTrackbarByName( window, trackbar_name );

    if( trackbar )
    {
        if( pos < 0 )
            pos = 0;

        if( pos > trackbar->maxval )
            pos = trackbar->maxval;
    }
    gtk_range_set_value( GTK_RANGE(trackbar->widget), pos );

    __END__;
}


CV_IMPL void* cvGetWindowHandle( const char* window_name )
{
    void* widget = 0;
    
    CV_FUNCNAME( "cvGetWindowHandle" );

    __BEGIN__;

    CvWindow* window;

    if( window_name == 0 )
        CV_ERROR( CV_StsNullPtr, "NULL window name" );

    window = icvFindWindowByName( window_name );
    if( window )
        widget = (void*)window->area;

    __END__;

    return widget;
}
    

CV_IMPL const char* cvGetWindowName( void* window_handle )
{
    const char* window_name = "";
    
    CV_FUNCNAME( "cvGetWindowName" );

    __BEGIN__;

    CvWindow* window;

    if( window_handle == 0 )
        CV_ERROR( CV_StsNullPtr, "NULL window" );

    window = icvWindowByWidget( (GtkWidget*)window_handle );
    if( window )
        window_name = window->name;

    __END__;

    return window_name;
}


/* draw image to frame */
static void icvPutImage( CvWindow* window )
{
    Assert( window != 0 );
    if( window->image == 0 )
        return;

    gdk_draw_rgb_image( window->area->window, window->area->style->fg_gc[GTK_STATE_NORMAL],
		                0, 0, window->image->cols, window->image->rows,
                        GDK_RGB_DITHER_MAX, window->image->data.ptr, window->image->step );
}                                                   
                                                    

static gboolean icvOnExpose( GtkWidget *widget,
                GdkEventExpose* /*event*/, gpointer user_data )
{
    CvWindow* window = (CvWindow*)user_data;
    if( window->signature == CV_WINDOW_MAGIC_VAL &&
        window->area == widget && window->image != 0 )
    {
        icvPutImage( window );
        return TRUE;
    }

    return FALSE;
}


static gboolean icvOnKeyPress( GtkWidget * /*widget*/,
                GdkEventKey* event, gpointer /*user_data*/ )
{
    int code = 0;
    
    switch( event->keyval )
    {
    case GDK_Escape:
        code = 27;
        break;
    case GDK_Return:
    case GDK_Linefeed:
        code = '\n';
        break;
    case GDK_Tab:
        code = '\t';
    default:
        code = event->keyval;
    }

    code |= event->state << 16;
    last_key = code;
    return FALSE;
}


static void icvOnTrackbar( GtkWidget* widget, gpointer user_data )
{
    int pos = cvRound( gtk_range_get_value(GTK_RANGE(widget)));
    CvTrackbar* trackbar = (CvTrackbar*)user_data;

    if( trackbar && trackbar->signature == CV_TRACKBAR_MAGIC_VAL &&
        trackbar->widget == widget )
    {
        trackbar->pos = pos;
        if( trackbar->data )
            *trackbar->data = pos;
        if( trackbar->notify )
            trackbar->notify(pos);
    }
}

static gboolean icvOnClose( GtkWidget* widget, GdkEvent* /*event*/, gpointer user_data )
{
    CvWindow* window = (CvWindow*)user_data;
    if( window->signature == CV_WINDOW_MAGIC_VAL &&
        window->frame == widget )
        icvDeleteWindow(window);
    return TRUE;
}


static gboolean icvOnMouse( GtkWidget *widget, GdkEvent *event, gpointer user_data )
{
    CvWindow* window = (CvWindow*)user_data;
    CvPoint pt = {-1,-1};
    int cv_event = -1, state = 0;

    if( window->signature != CV_WINDOW_MAGIC_VAL ||
        window->area != widget || !window->image || !window->on_mouse )
        return FALSE;

    if( event->type == GDK_MOTION_NOTIFY )
    {
        GdkEventMotion* event_motion = (GdkEventMotion*)event;
        
        cv_event = CV_EVENT_MOUSEMOVE;
        pt.x = cvRound(event_motion->x);
        pt.y = cvRound(event_motion->y);
        state = event_motion->state;
    }
    else if( event->type == GDK_BUTTON_PRESS ||
             event->type == GDK_BUTTON_RELEASE ||
             event->type == GDK_2BUTTON_PRESS )
    {
        GdkEventButton* event_button = (GdkEventButton*)event;
        
        pt.x = cvRound(event_button->x);
        pt.y = cvRound(event_button->y);

        if( event_button->type == GDK_BUTTON_PRESS )
        {
            cv_event = event_button->button == 1 ? CV_EVENT_LBUTTONDOWN :
                       event_button->button == 2 ? CV_EVENT_MBUTTONDOWN :
                       event_button->button == 3 ? CV_EVENT_RBUTTONDOWN : 0;
        }
        else if( event_button->type == GDK_BUTTON_RELEASE )
        {
            cv_event = event_button->button == 1 ? CV_EVENT_LBUTTONUP :
                       event_button->button == 2 ? CV_EVENT_MBUTTONUP :
                       event_button->button == 3 ? CV_EVENT_RBUTTONUP : 0;
        }
        else if( event_button->type == GDK_2BUTTON_PRESS )
        {
            cv_event = event_button->button == 1 ? CV_EVENT_LBUTTONDBLCLK :
                       event_button->button == 2 ? CV_EVENT_MBUTTONDBLCLK :
                       event_button->button == 3 ? CV_EVENT_RBUTTONDBLCLK : 0;
        }
        state = event_button->state;
    }

    if( cv_event >= 0 &&
        (unsigned)pt.x < (unsigned)(window->image->width) &&
        (unsigned)pt.y < (unsigned)(window->image->height) )
    {
        int flags = (state & GDK_SHIFT_MASK ? CV_EVENT_FLAG_SHIFTKEY : 0) |
                    (state & GDK_CONTROL_MASK ? CV_EVENT_FLAG_CTRLKEY : 0) |
                    (state & (GDK_MOD1_MASK|GDK_MOD2_MASK) ? CV_EVENT_FLAG_ALTKEY : 0) |
                    (state & GDK_BUTTON1_MASK ? CV_EVENT_FLAG_LBUTTON : 0) |
                    (state & GDK_BUTTON2_MASK ? CV_EVENT_FLAG_MBUTTON : 0) |
                    (state & GDK_BUTTON3_MASK ? CV_EVENT_FLAG_RBUTTON : 0);
        window->on_mouse( cv_event, pt.x, pt.y, flags );
    }
    
    return FALSE;    
}

static gboolean icvOnMouseMotion( GtkWidget *widget, GdkEventMotion *event, gpointer user_data )
{
    CvWindow* window = (CvWindow*)user_data;
    if( window->signature == CV_WINDOW_MAGIC_VAL &&
        window->area == widget && window->image && window->on_mouse )
    {
        CvPoint pt;
        pt.x = cvRound(event->x);
        pt.y = cvRound(event->y);
        
        int flags = (event->state & GDK_SHIFT_MASK ? CV_EVENT_FLAG_SHIFTKEY : 0) |
                    (event->state & GDK_CONTROL_MASK ? CV_EVENT_FLAG_CTRLKEY : 0) |
                    (event->state & (GDK_MOD1_MASK|GDK_MOD2_MASK) ? CV_EVENT_FLAG_ALTKEY : 0) |
                    (event->state & GDK_BUTTON1_MASK ? CV_EVENT_FLAG_LBUTTON : 0) |
                    (event->state & GDK_BUTTON2_MASK ? CV_EVENT_FLAG_MBUTTON : 0) |
                    (event->state & GDK_BUTTON3_MASK ? CV_EVENT_FLAG_RBUTTON : 0);
        //gtk_widget_translate_coordinates( window->area, window->frame,
        //                                  pt.x, pt.y, &pt.x, &pt.y );
        window->on_mouse( CV_EVENT_MOUSEMOVE, pt.x, pt.y, flags );
    }

    return FALSE;    
}


static gboolean icvAlarm( gpointer user_data )
{
    *(int*)user_data = 1;
    return FALSE;
}


CV_IMPL int cvWaitKey( int delay )
{
    int expired = 0;
    guint timer = 0;
    
    last_key = -1;
    if( delay > 0 )
        timer = g_timeout_add( delay, icvAlarm, &expired );
    
    while( gtk_main_iteration_do(TRUE) && last_key < 0 && !expired && hg_windows != 0 )
        ;
    
    if( delay > 0 && !expired )
        g_source_remove(timer);
    
    return last_key;
}

#endif

/* End of file. */
