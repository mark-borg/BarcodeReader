
#include "BarcodeReader.h"
#include "EAN13BarcodeDecoder.h"
#include <iostream>

using namespace std;




BarcodeReader::BarcodeReader()
{
	search_roi = cvRect( 0, 0, 0, 0 );		// whole image
	max_scans  = 25;
	min_correct_scans = 2;
	search_type = SEARCH_SUB_DIVIDE;

	debug = false;
	debug_image = NULL;
}



BarcodeReader::~BarcodeReader()
{
	if ( debug_image )
		cvReleaseImage( &debug_image );
}



void BarcodeReader::init_search()
{
	switch( search_type )
	{
	case SEARCH_SUB_DIVIDE:
		num_scans = 0;
		scan_step = 1;
		scan_step2 = 1;
		scan_y = curr_search_roi.y + ( curr_search_roi.height - 1 ) * scan_step / ( scan_step2 * 2.0 );
		break;

	case SEARCH_LINEAR:
	default:
		num_scans = 0;
		scan_step = ( curr_search_roi.height - 1 ) / ( max_scans - 1 );
		scan_y = curr_search_roi.y;
	};
}



bool BarcodeReader::next_search()
{
	switch( search_type )
	{
	case SEARCH_SUB_DIVIDE:
		++num_scans;
		if ( num_scans >= max_scans )
			return false;
		scan_step += 2;
		if ( scan_step > 2 * scan_step2 )
		{
			scan_step = 1;
			scan_step2 *= 2;
		}
		scan_y = curr_search_roi.y + ( curr_search_roi.height - 1 ) * scan_step / ( scan_step2 * 2.0 );
		break;

	case SEARCH_LINEAR:
	default:
		++num_scans;
		if ( num_scans >= max_scans )
			return false;
		scan_y += scan_step;
		assert( scan_y >= curr_search_roi.y && scan_y < ( curr_search_roi.y + curr_search_roi.height ) );
	};
	return true;
}



bool BarcodeReader::run( IplImage* img, std::string& barcode_value )
{
	assert( img );
	assert( img->nChannels == 1 );


	// set search area
	curr_search_roi = search_roi;
	if ( curr_search_roi.width == 0 )
	{
		curr_search_roi.x = 0;
		curr_search_roi.width = img->width;
	}
	if ( curr_search_roi.height == 0 )
	{
		curr_search_roi.y = 0;
		curr_search_roi.height = img->height;
	}


	if ( debug )
	{
		if ( debug_image != NULL )
			cvReleaseImage( &debug_image );
		debug_image = cvCloneImage( img );
		assert( debug_image );

		cvRectangle( debug_image, 
					 cvPoint( curr_search_roi.x - 1, curr_search_roi.y - 1 ), 
					 cvPoint( curr_search_roi.x + 1 + curr_search_roi.width, curr_search_roi.y + 1 + curr_search_roi.height ), 
					 CV_RGB( 128, 128, 128 ) );

		cvNamedWindow( "BarcodeReader-debug", CV_WINDOW_AUTOSIZE );
		cvShowImage( "BarcodeReader-debug", debug_image );
	}


	barcode_values.clear();
	barcode_values.resize( max_scans );

	init_search();

	int num_correct_scans = 0;

	do
	{
		if ( debug )
			trace_log << "Scan #" << num_scans << " along line: " << scan_y << endl;


		// find the maximum and minimum greyvalues along the scan line
		int x = curr_search_roi.x;
		unsigned char* ptr = (unsigned char*) img->imageData + scan_y * img->widthStep + x;
		int minv = 256, maxv = -1;

		for ( ; x < curr_search_roi.width; ++x, ++ptr )
		{
			if ( *ptr < minv )
				minv = *ptr;
			if ( *ptr > maxv )
				maxv = *ptr;
		}
		if ( debug )
			trace_log << "Min, Max along scan line: " << minv << "," << maxv << endl;


		// do optimal thresholding to find the threshold
		int T0 = -1, T = ( minv + maxv ) / 2;

		while ( T0 != T )
		{
			T0 = T;

			int nb = 0, nf = 0;
			double tb = 0, tf = 0;

			int x = curr_search_roi.x;
			unsigned char* ptr = (unsigned char*) img->imageData + scan_y * img->widthStep + x;
			for ( ; x < curr_search_roi.width; ++x, ++ptr )
			{
				if ( *ptr > T ) 
				{
					tb += *ptr;
					++nb;
				}
				else
				{
					tf += *ptr;
					++nf;
				}
			}

			double	mb = tb / nb,
					mf = tf / nf;

			T = ( mb + mf ) / 2.0;
		}

		if ( debug )
			trace_log << "Optimal threshold: " << T << endl;


		// threshold along the scan line & gather the barcode sample
		string barcode_sample;
		x = curr_search_roi.x;
		ptr = (unsigned char*) img->imageData + scan_y * img->widthStep + x;
		if ( debug )
		{
			unsigned char* ptrDebug = (unsigned char*) debug_image->imageData + scan_y * debug_image->widthStep + x;
			for ( ; x < curr_search_roi.width; ++x, ++ptr, ++ptrDebug )
			{
				if ( *ptr > T ) 
				{
					barcode_sample += "0";
					*ptrDebug = 255;
				}
				else if ( *ptr < T )
				{
					barcode_sample += "1";
					*ptrDebug = 0;
				}
			}
		}
		else
		{
			for ( ; x < curr_search_roi.width; ++x, ++ptr )
			{
				if ( *ptr > T ) 
					barcode_sample += "0";
				else if ( *ptr < T )
					barcode_sample += "1";
			}
		}
		barcode_sample += "1";	// finish off line

		if ( debug )
		{
			trace_log << barcode_sample << endl << endl;
			cvShowImage( "BarcodeReader-debug", debug_image );
		}


		// try decoding the barcode sample
		EAN13BarcodeDecoder barcode;
		if ( ! barcode.parse_binary_pixel_scan( barcode_sample, barcode_values[ num_scans ] ) )
		{
			if ( debug )
			{
				trace_log << "ERROR READING BARCODE!" << endl;
				trace_log << barcode_values[ num_scans ] << endl;
			}
			barcode_values[ num_scans ] = "";
		}
		else
		{
			if ( debug )
				trace_log << "Barcode OK!  "  << barcode_values[ num_scans ] << endl;
		}


		// keep track of how many successful reads done so far
		if ( barcode_values[ num_scans ].length() )
		{
			if ( barcode_value.length() == 0 )
				barcode_value = barcode_values[ num_scans ];
			else if ( barcode_values[ num_scans ].length() > barcode_value.length() && 
					  ( barcode_values[ num_scans ].length() == 15 || 
						barcode_values[ num_scans ].length() == 18 ) &&
					  barcode_value.length() == 13 )
				barcode_value = barcode_values[ num_scans ];

			if ( strncmp( barcode_value.c_str(), barcode_values[ num_scans ].c_str(), 
						  min( barcode_value.length(), barcode_values[ num_scans ].length() ) ) == 0 )
				++num_correct_scans;
		}


		if ( num_correct_scans >= min_correct_scans )
			break;
	}
	while( next_search() );


	if ( debug )
	{
		if ( num_correct_scans >= min_correct_scans )
			trace_log << "BARCODE = " << barcode_value << "\t\tsuccessful scan reads=" << num_correct_scans << " of " << num_scans+1 << endl;
		else
			trace_log << "ERROR READING BARCODE!" << endl;
		cvShowImage( "BarcodeReader-debug", debug_image );
	}

	return num_correct_scans >= min_correct_scans;
}


