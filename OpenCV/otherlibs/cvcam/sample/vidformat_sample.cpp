#include "cvcam.h"
#include "highgui.h"

/**************************************************************************\
            How to set video resolution programatically
\**************************************************************************/ 
void callback(IplImage* image)
{

}

#if 1
int ShowCamVideo(HWND hwnd, int width, int height)
{
    VidFormat vidFmt={ width, height, 30.0};

    int ncams = cvcamGetCamerasCount( );
    cvcamSetProperty(0, CVCAM_PROP_ENABLE, CVCAMTRUE);
    cvcamSetProperty(0, CVCAM_PROP_CALLBACK,callback);
    cvcamSetProperty(0, CVCAM_PROP_WINDOW, &hwnd);   

    //Set Video Format Property
    cvcamSetProperty( 0, CVCAM_PROP_SETFORMAT, &vidFmt);

    if( !cvcamInit() )
        return 0;
    cvcamStart();
    return 1;
}

int main( int argc, char** argv )
{
    cvNamedWindow("cvcam window", CV_WINDOW_AUTOSIZE);
    if( ShowCamVideo((HWND)cvGetWindowHandle("cvcam window"), 640, 480) )
    {
        cvWaitKey(0);
        cvcamStop();
    }
    cvcamExit();
}

#else
/**************************************************************************\
                       How To Play Media file by CvCam
\**************************************************************************/

void ShowAVIVideo(char* filename, HWND hwnd, int width, int height)
{
    cvcamAVIFILE f = cvcamAVIOpenFile(filename);
    if( (int)f < 0 )
        return;
    cvcamAVISetWindow (f, hwnd);
    cvcamAVISetCallback(f, callback);
    cvcamAVISetSize(f, width, height);
    cvcamAVIRun(f);
    cvcamAVIWaitCompletion(f);
    cvcamAVIStop(f);
    cvcamAVICloseFile(f);
}


int main( int argc, char** argv )
{
    if( argc > 1 )
    {
        cvNamedWindow("avi window", CV_WINDOW_AUTOSIZE);
        ShowAVIVideo( argv[1], (HWND)cvGetWindowHandle("avi window"), 320, 240);
    }
}
#endif
