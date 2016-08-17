
#include "BarcodeDecoder.h"
#include <iostream>


using namespace std;




bool BarcodeDecoder::get_first_run_length( const string& binary_str, int& pos, char& chr, int& chr_run_length )
{
	chr = binary_str[ 0 ];
	chr_run_length = 1;
	pos = 1;

	return get_next_run_length( binary_str, pos, chr, chr_run_length );
}



bool BarcodeDecoder::get_next_run_length( const string& binary_str, int& pos, char& chr, int& chr_run_length )
{
	chr = binary_str[ pos ];
	chr_run_length = 1;

	for ( ; pos < binary_str.length(); ++pos )
	{
		if ( binary_str[ pos ] == chr )
		{
			++chr_run_length;
		}
		else
		{
			return true;
		}
	}

	chr_run_length = 0;
	return false;
}



int BarcodeDecoder::find_histogram_peaks( const vector< int >& histogram, vector< int >& peaks )
{
	// given a histogram, it returns the peaks in the histogram. Peaks are determined
	// using a 3-bin neighbourhood, i.e. a peak is greater than the bin just before it
	// and the bin just after it.
	peaks.clear();

	vector< bool > wrk;
	wrk.resize( histogram.size() );

	int maxv, maxk;
	do
	{
		maxv = 0;
		maxk = -1;
		
		for ( int k = 1; k < histogram.size() - 1; ++k )
		{
			if ( histogram[ k ] > maxv &&		// largest bin so far
				 ! wrk[ k ] && ! wrk[ k-1 ] && ! wrk[ k+1 ] &&		// not near an already-found peak
				 histogram[ k ] >= histogram[ k-1 ] && histogram[ k ] >= histogram[ k+1 ] )	// is a peak
			{
				maxv = histogram[ k ];
				maxk = k;
			}
		}

		// found peak?
		if ( maxk != -1 )
		{
			wrk[ maxk ] = true;
			peaks.push_back( maxk );
		}
	}
	while( maxk != -1 );

	return peaks.size();
}



void BarcodeDecoder::build_lengths_histogram( const string& binary_pixel_scan )
{
	// build histogram of different lengths of this raw barcode sample
	lengths_histogram.clear();
	lengths_histogram.resize( 50 );

	int pos, chr_run_len;
	char chr;
	get_first_run_length( binary_pixel_scan, pos, chr, chr_run_len );

	while( chr_run_len > 0 )
	{
		if ( chr_run_len < lengths_histogram.size() )
			++lengths_histogram[ chr_run_len ];
		else
			++lengths_histogram[ lengths_histogram.size() - 1 ];

		get_next_run_length( binary_pixel_scan, pos, chr, chr_run_len );
	}
	
	if ( debug )
	{
		trace_log << "LEN HISTOGRAM: " << endl << lengths_histogram << endl << endl;
	}
}



void BarcodeDecoder::convert_pixel_scan_to_barcode( const string& binary_pixel_scan, string& binary_barcode )
{
	// convert the binary pixel scan string to a binary barcode using the lengths mapping table
	binary_barcode = "";

	int pos, chr_run_len;
	char chr;
	get_first_run_length( binary_pixel_scan, pos, chr, chr_run_len );

	while( chr_run_len > 0 )
	{
		if ( chr_run_len >= lengths_histogram.size() )
			chr_run_len = lengths_histogram.size() - 1;

		int bar_size = lengths_mapping[ chr_run_len ];

		for ( int b = 0; b < bar_size; ++b )
			binary_barcode += chr;
		
		get_next_run_length( binary_pixel_scan, pos, chr, chr_run_len );
	}

	if ( debug )
	{
		trace_log << binary_barcode << endl;
	}
}


