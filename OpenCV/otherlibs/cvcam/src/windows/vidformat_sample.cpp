/**************************************************************************\
	How to set video resolution programatically
\**************************************************************************/ 
void callback(IplImage* image)
{

}

void ShowCamVideo(HWND hwnd, int width, int height)
{
	/*Init Video Format structure 
	typedef struct
	{
    		int width;
    		int height;
    		double framerate;
	}VidFormat;
	*/
	VidFormat vidFmt={320, 240, 30.0};

	int ncams = cvcamGetCamerasCount( );
	cvcamSetProperty(0, CVCAM_PROP_ENABLE, CVCAMTRUE);
	cvcamSetProperty(0, CVCAM_PROP_CALLBACK,callback);
	cvcamSetProperty(0, CVCAM_PROP_WINDOW, &hwnd);   

	//Set Video Format Property
	cvcamSetProperty(out[0], CVCAM_PROP_SETFORMAT, &vidFmt);

	cvcamInit( ); 
	cvcamStart( );
}

int main( int argc, char** argv )
{
	cvNamedWindow("cvcam window", CV_WINDOW_AUTOSIZE);
	ShowCamVideo((HWND)cvGetWindowHandle("cvcam window"), 320, 240);
	cvWaitKey(0);
	cvcamStop( );
	cvcamExit( );
}

/**************************************************************************\
                                   How To Play Media file by CvCam
\**************************************************************************/
void callback(IplImage* image)
{

}

void ShowAVIVideo(char* filename, HWND hwnd, int width, int height)
{
	cvcamAVIOpenFile(filename);
	cvcamAVISetWindow (0, hwnd);
	cvcamAVISetCallback(0, callback);
	cvcamAVISetSize(0, width, height);
	cvcamAVIRun(0);
	cvcamAVIWaitCompletion(0);
	cvcamAVIStop(0);
	cvcamAVICloseFile(0);	
}

int main( int argc, char** argv )
{
	cvNamedWindow("avi window", CV_WINDOW_AUTOSIZE);
	ShowAVIVideo("myavifile.avi", (HWND)cvGetWindowHandle("avi window"), 320, 240);
}
                                   