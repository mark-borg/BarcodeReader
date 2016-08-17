
#include "../qvcore/framework.h"
#include <iostream>

using namespace std;




#define TEST_1
//#define TEST_2



void main()
{

#ifdef TEST_1
#	define VIDEO_PATH	"../data/ean13/"
#	define TEST_FILE_MASK	"%s/%03.03d.jpg"

#elif defined( TEST_2 )
#	define VIDEO_PATH	"../data/upc-a/"
#	define TEST_FILE_MASK	"%s/%03.03d.jpg"

#else
#	error	"Ooops!"
#endif



	NumberedFileCamera camera( VIDEO_PATH, TEST_FILE_MASK );
	camera.set_output_greyscale();
	if ( ! camera.open_source() )
		exit(1);

	IplImage* img = NULL;
	IplImage* res = NULL;
	IplImage* res3 = NULL;

	CvFont font;
	cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 1.0f, 1.0f, 0, 3 );
			
	cvNamedWindow( "Result", CV_WINDOW_AUTOSIZE );

	BarcodeReader barcode;
	///barcode.set_debug();
	barcode.set_max_scans( 70 );
	barcode.set_min_correct_scans( 1 );


	while( ( img = camera.acquire_image() ) != NULL )
	{
		cvShowImage( "Result", img );

		
		string value;
		bool ok = barcode.run( img, value );


		res = cvCloneImage( img );
		res3 = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 3 );
		cvSet( res, CV_RGB( 0,0,0 ) );
		if ( ok )
		{
			cvPutText( res, ( "OK  " + value ).c_str(), cvPoint( 10, 50 ), &font, CV_RGB( 255,255,255 ) );
			overlay_result( img, res, res3, 1 );
		}
		else
		{
			cvLine( res, cvPoint( 0, 0 ), cvPoint( res->width-1, res->height-1 ), CV_RGB( 255,255,255 ), 10 );
			cvLine( res, cvPoint( 0, res->height-1 ), cvPoint( res->width-1, 0 ), CV_RGB( 255,255,255 ), 10 );
			overlay_result( img, res, res3, 0 );
		}
		cvShowImage( "Result", res3 );

		cvWaitKey( 0 );
		cvReleaseImage( &res );
		cvReleaseImage( &res3 );
		cvReleaseImage( &img );
		trace_log << endl;
	}

	camera.close_source();
}
