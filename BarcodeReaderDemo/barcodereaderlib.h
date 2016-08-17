
#include "../qvcore/framework.h"
#include <iostream>
#include <cmath>
#include <cfloat>
#include <algorithm>

using namespace std;



void overlay_result( const IplImage* src, const IplImage* ovr, IplImage* dst, int channel_id = 0 )
{
	assert( src );
	assert( ovr );
	assert( dst );
	assert( src->nChannels == 1 );
	assert( ovr->nChannels == 1 );
	assert( dst->nChannels == 3 );

	CvRect src_roi = src->roi != NULL ? cvRect( src->roi->xOffset, src->roi->yOffset, src->roi->width, src->roi->height ) 
									  : cvRect( 0, 0, src->width, src->height );
	CvRect ovr_roi = ovr->roi != NULL ? cvRect( ovr->roi->xOffset, ovr->roi->yOffset, ovr->roi->width, ovr->roi->height ) 
									  : cvRect( 0, 0, ovr->width, ovr->height );
	CvRect dst_roi = dst->roi != NULL ? cvRect( dst->roi->xOffset, dst->roi->yOffset, dst->roi->width, dst->roi->height ) 
									  : cvRect( 0, 0, dst->width, dst->height );
	assert( src_roi.width == ovr_roi.width && ovr_roi.width == dst_roi.width );
	assert( src_roi.height == ovr_roi.height && ovr_roi.height == dst_roi.height );

	for ( int y = 0; y < dst_roi.height; ++y )
	{
		int x = 0;

		unsigned char* pSrc = (unsigned char*) src->imageData + ( y + src_roi.y ) * src->widthStep + ( x + src_roi.x );
		unsigned char* pOvr = (unsigned char*) ovr->imageData + ( y + ovr_roi.y ) * ovr->widthStep + ( x + ovr_roi.x );
		unsigned char* pDst = (unsigned char*) dst->imageData + ( y + dst_roi.y ) * dst->widthStep + ( x + dst_roi.x );
		
		for ( ; x < dst_roi.width; ++x, ++pSrc, ++pOvr, pDst += 3 )
		{
			*pDst = *(pDst +1) = *(pDst +2) = *pSrc;
			if ( *pOvr != 0 )
			{
				switch( channel_id )
				{
				case 1:
					*(pDst) = *(pDst +2) = 0;
					*(pDst +1) = *pOvr;
					break;

				case 2:
					*(pDst +1) = *(pDst +2) = 0;
					*pDst = *pOvr;
					break;

				case 0:
				default:
					*pDst = *(pDst +1) = 0;
					*(pDst +2) = *pOvr;
				}
			}
		}
	}
}




template< class T >
ostream& operator << ( ostream& out, const vector< T >& vec )
{
	for ( int x = 0; x < vec.size(); ++x )
		out << "#" << x << ": \t" << vec[ x ] << endl;
	return out;
}



class BarcodeDecoder
{
protected:

	vector< int >	lengths_histogram;
	vector< int >	lengths_mapping;

public:


	static bool get_first_run_length( const string& binary_str, int& pos, char& chr, int& chr_run_length )
	{
		chr = binary_str[ 0 ];
		chr_run_length = 1;
		pos = 1;

		return get_next_run_length( binary_str, pos, chr, chr_run_length );
	}



	static bool get_next_run_length( const string& binary_str, int& pos, char& chr, int& chr_run_length )
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



	static int find_histogram_peaks( const vector< int >& histogram, vector< int >& peaks )
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



	void build_lengths_histogram( const string& binary_pixel_scan )
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
		///cout << "LEN HISTOGRAM: " << endl << lengths_histogram << endl << endl;
	}



	void convert_pixel_scan_to_barcode( const string& binary_pixel_scan, string& binary_barcode )
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

		///cout << binary_barcode << endl;
	}
};




class EAN13BarcodeDecoder : public BarcodeDecoder
{
private:

	// EAN 13 encoding tables
	static string A[ 10 ];
	static string B[ 10 ];
	static string C[ 10 ];
	static string F[ 10 ];
	static string C1[ 4 ];
	static string C2[ 10 ];


public:


	int calculate_checksum( const string& barcode_value )
	{
		if ( barcode_value.length() < 12 )
			return -1;

		int sum_even_digits = 0, sum_odd_digits = 0;
		for ( int n = 0; n < 12; n +=2 )
		{
			sum_odd_digits += barcode_value[ n+1 ] - '0';
			sum_even_digits += barcode_value[ n ] - '0';
		}
		int z = sum_even_digits + 3 * sum_odd_digits;
		int m = z / 10 * 10 + ( z % 10 ? 10 : 0 );
		int chk = m - z;

		return chk;
	}



	bool parse_binary_barcode( const string& binary_barcode, string& barcode_value )
	{
		if ( binary_barcode.length() == 0 )
			return false;

		// analyse EAN-13 barcode
		const char* ps = binary_barcode.c_str();
		barcode_value = "?";	// The first digit can only be known after the next 6 
								// digits are read; we put a placeholder for it.
		string table_usage;		// Which conversion tables are used to decode the 6 digits, will 
								// help to indicate what the first digit is.
		
		// skip all until we reach first quiet zone + 101 (the barcode start marker).
		while( strncmp( ps, "00000101", 8 ) != 0 )
		{
			// skip the current bar/space
			char c = ps[0];
			while( *ps == c )
				++ps;

			if ( *ps == 0 )
			{
				cout << "ERROR READING BARCODE! - premature end of data. " << barcode_value << endl;
				return false;
			}
		}
		ps += 8;


		// now decode the first part of the barcode (i.e. the part before the middle separator).
		// this consists of 6 digits (+ the unknown first digit, which we will decode later).
		do
		{
			bool ok = false;

			for ( int n = 0; n < 10 && ! ok; ++n )
			{
				// for the first part, either table A or table B could be used - we try both
				if ( strncmp( ps, A[n].c_str(), 7 ) == 0 )
				{
					barcode_value += ( '0' + n );
					table_usage += "A";
					ok = true;
				}
				else if ( strncmp( ps, B[n].c_str(), 7 ) == 0 )
				{
					barcode_value += ( '0' + n );
					table_usage += "B";
					ok = true;
				}
			}
			
			if ( ok )
			{
				ps += 7;
			}
			else
			{
				barcode_value += "?";
				cout << "ERROR READING BARCODE! - unknown digit. " << barcode_value << endl;
				return false;
			}
		}
		while( barcode_value.length() < 7 );	// 6 digits + the initial '?'


		// middle separator
		if ( strncmp( ps, "01010", 5 ) == 0 )
			ps += 5;
		else
		{
			barcode_value += "?";
			cout << "ERROR READING BARCODE! - expecting the middle separator. " << barcode_value << endl;
			return false;
		}


		// determine the first digit by looking at what encoding tables were used to decode
		// the 6 digits we have already decoded.
		bool ok = false;
		for ( int n = 0; n < 10 && ! ok; ++n )
			if ( strncmp( table_usage.c_str(), F[n].c_str(), 6 ) == 0 )
			{
				barcode_value[0] = '0' + n;
				ok = true;
			}

		if ( ! ok )
		{
			cout << "ERROR READING BARCODE! - can not determine first digit. " << barcode_value << endl;
			return false;
		}


		// continue decoding the 2nd part of the barcode (i.e. after the middle separator).
		do
		{
			bool ok = false;

			// digits in the second part are all encoded by table C
			for ( n = 0; n < 10 && ! ok; ++n )
			{
				if ( strncmp( ps, C[n].c_str(), 7 ) == 0 )
				{
					barcode_value += ( '0' + n );
					ok = true;
				}
			}
			
			if ( ok )
			{
				ps += 7;
			}
			else
			{
				barcode_value += "?";
				cout << "ERROR READING BARCODE! - can not decode digit in 2nd part. " << barcode_value << endl;
				return false;
			}
		}
		while( barcode_value.length() < 13 );	

		cout << "BARCODE = " << barcode_value << endl;


		// final separator
		if ( strncmp( ps, "101", 3 ) == 0 )
			ps += 3;
		else
		{
			barcode_value += "?";
			cout << "ERROR READING BARCODE! - missing final separator. " << barcode_value << endl;
			return false;
		}

		// quiet zone
		if ( strncmp( ps, "00000", 5 ) == 0 )
			ps += 5;
		else
		{
			cout << "WARNING: NO QUIET ZONE AFTER BARCODE!" << endl;
			while ( ps[0] == '0' )
				++ps;
		}


		// validate the checksum
		int chk = calculate_checksum( barcode_value );
		if ( chk != barcode_value[ barcode_value.length() - 1 ] - '0' )
		{
			cout << "ERROR READING BARCODE! - checksum failure. " << barcode_value << endl;
			return false;
		}


		// check if there is a 2- or 5-digit "add-on"
		if ( strncmp( ps, "1011", 4 ) == 0 )
		{
			cout << "Possible presence of a 2- or 5-digit 'add-on'." << endl;
			ps += 4;

			table_usage = "";
			string add_on;
			bool ok = false;

			do
			{
				ok = false;

				for ( int n = 0; n < 10 && ! ok; ++n )
				{
					// for the first part, either table A or table B could be used - we try both
					if ( strncmp( ps, A[n].c_str(), 7 ) == 0 )
					{
						add_on += ( '0' + n );
						table_usage += "A";
						ok = true;
					}
					else if ( strncmp( ps, B[n].c_str(), 7 ) == 0 )
					{
						add_on += ( '0' + n );
						table_usage += "B";
						ok = true;
					}
				}
				
				if ( ok )
				{
					ps += 7;

					// now expect the separator
					if ( strncmp( ps, "01", 2 ) == 0 )
						ps += 2;
					else if ( strncmp( ps, "00000", 5 ) == 0 )
						break;
					else
						ok = false;
				}
			}
			while( ok && add_on.length() < 5 );	


			// validate check-sum of add-on
			if ( ok && add_on.length() == 2 )
			{
				int a = ( add_on[0] - '0' ) * 10 + ( add_on[1] - '0' ); 
				int m = a / 4 * 4;
				int chk = a - m;
				
				if ( chk >= 0 && chk <= 3 &&
					 strncmp( table_usage.c_str(), C1[ chk ].c_str(), 2 ) == 0 )
				{
					cout << "2-digit add-on checksum validated." << endl;
					ok = true;
				}
				else
				{
					cout << "2-digit add-on checksum failure." << endl;
					ok = false;
				}

			}
			else if ( ok && add_on.length() == 5 )
			{
				int x = ( add_on[1] - '0' ) + ( add_on[3] - '0' ); 
				int y = ( add_on[0] - '0' ) + ( add_on[2] - '0' ) + ( add_on[4] - '0' );
				int a = 9 * x + 3 * y;
				int m = a / 10 * 10;
				int chk = a - m;

				if ( chk >= 0 && chk <= 9 &&
					 strncmp( table_usage.c_str(), C2[ chk ].c_str(), 5 ) == 0 )
				{
					cout << "5-digit add-on checksum validated." << endl;
					ok = true;
				}
				else
				{
					cout << "5-digit add-on checksum failure." << endl;
					ok = false;
				}
			}
			else
			{
				cout << "Incorrect add-on length. " << barcode_value << endl;
			}

			if ( ok )
			{
				cout << "BARCODE ADD-ON = " << add_on << endl;
				barcode_value += add_on;
			}
		}


		return true;
	}



	bool parse_binary_pixel_scan( const string& binary_pixel_scan, string& barcode_value )
	{
		barcode_value = "";

		// The overall length of an EAN 13 barcode is 95 'modules', where a 'module' is
		// the width of the slimmest bar (0.33mm according to the spec.)
		// In our raw binary mask, it is expected that at least one pixel represents
		// a module, so the length of the mask must be greater than 95.
		if ( binary_pixel_scan.length() < 95 )
			return false;

		
		// build histogram of different lengths of this raw barcode sample
		build_lengths_histogram( binary_pixel_scan );

				
		// find peaks in the lengths histogram 
		vector< int > histogram_peaks;
		if ( ! find_histogram_peaks( lengths_histogram, histogram_peaks ) )
			return false;
		///cout << "PEAKS: " << endl << histogram_peaks << endl << endl;


		// take the first (smallest length) peak as the 'module' length
		sort( histogram_peaks.begin(), histogram_peaks.end() );
		int module_size = histogram_peaks[ 0 ];
		

		// search for the next peaks, which should have size in multiples of 'module';
		// allowing variation due to noise up to half a 'module'.
		// And if not found (e.g. no digit in barcode has such a bar size), add these peaks.
		bool found_peak = false;
		int next_module_size = module_size;

		for ( int peak = 2; peak < 5; ++peak )
		{
			found_peak = false;
			next_module_size += module_size;
			for ( int k = 0; k < histogram_peaks.size() && ! found_peak; ++k )
				if ( abs( histogram_peaks[ k ] - next_module_size ) <= ( module_size / 2 ) )
				{
					found_peak = true;
					next_module_size = histogram_peaks[ k ];
				}
			
			if ( ! found_peak )
			{
				histogram_peaks.push_back( next_module_size );
			}
		}

		// add a final peak after the 4th peak to handle other non-bar sizes
		// such as the quiet zone, etc.
		next_module_size += 2 * module_size;
		histogram_peaks.push_back( next_module_size < lengths_histogram.size() ? next_module_size : lengths_histogram.size() - 1 );
	
		// do a final sort
		sort( histogram_peaks.begin(), histogram_peaks.end() );
		///cout << "ADJ PEAKS: " << endl << histogram_peaks << endl << endl;


		// now create the mapping table
		lengths_mapping.clear();
		lengths_mapping.resize( lengths_histogram.size() );

		for ( int k = 0; k < lengths_histogram.size(); ++k )
		{
			// which is the nearest peak?
			int min_d = 999999;
			int min_peak = -1;

			for ( int peak = 0; peak < histogram_peaks.size(); ++peak )
			{
				int d = abs( histogram_peaks[ peak ] - k );
				if ( d < min_d )
				{
					min_d = d;
					min_peak = peak;
				}
			}

			lengths_mapping[ k ] = min_peak + 1;
			if ( lengths_mapping[ k ] > 5 )
				lengths_mapping[ k ] = 5;
		}
		///cout << "LEN MAPPING: " << endl << lengths_mapping << endl << endl;


		// convert the raw barcode string to a binary barcode using the lengths mapping table
		string binary_barcode;
		convert_pixel_scan_to_barcode( binary_pixel_scan, binary_barcode );


		return parse_binary_barcode( binary_barcode, barcode_value ); 
	}

};



// encoding tables
string EAN13BarcodeDecoder::A[] = {	"0001101", 
									"0011001", 
									"0010011", 
									"0111101", 
									"0100011",
									"0110001", 
									"0101111", 
									"0111011", 
									"0110111", 
									"0001011" };

string EAN13BarcodeDecoder::B[] = {	"0100111", 
									"0110011", 
									"0011011", 
									"0100001", 
									"0011101", 
									"0111001", 
									"0000101", 
									"0010001", 
									"0001001", 
									"0010111", };

string EAN13BarcodeDecoder::C[] = {	"1110010", 
									"1100110",
									"1101100",
									"1000010",
									"1011100",
									"1001110",
									"1010000",
									"1000100",
									"1001000",
									"1110100", };

string EAN13BarcodeDecoder::F[] = {	"AAAAAA",	
									"AABABB",	
									"AABBAB",	
									"AABBBA",	
									"ABAABB", 
									"ABBAAB",	
									"ABBBAA",	
									"ABABAB",	
									"ABABBA",	
									"ABBABA" };

string EAN13BarcodeDecoder::C1[] ={	"AA", 
									"AB",
									"BA",
									"BB", };

string EAN13BarcodeDecoder::C2[] ={	"BBAAA", 
									"BABAA",
									"BAABA",
									"BAAAB",
									"ABBAA",
									"AABBA",
									"AAABB",
									"ABABA",
									"ABAAB",
									"AABAB", };



class BarcodeReader
{
private:

	CvRect		search_roi;
	int			max_scans;
	int			search_type;
	int			min_correct_scans;

	bool		debug;
	IplImage*	debug_image;


	vector< string > barcode_values;		// list of results of each scan

	CvRect		curr_search_roi;
	int			num_scans;
	int			scan_step;
	int			scan_step2;
	int			scan_y;


public:


	enum { SEARCH_LINEAR, SEARCH_SUB_DIVIDE };



	BarcodeReader()
	{
		search_roi = cvRect( 0, 0, 0, 0 );		// whole image
		max_scans  = 25;
		min_correct_scans = 2;
		search_type = SEARCH_SUB_DIVIDE;

		debug = false;
		debug_image = NULL;
	}



	virtual ~BarcodeReader()
	{
		if ( debug_image )
			cvReleaseImage( &debug_image );
	}



	void set_search_area( const CvRect& area )
	{
		search_roi = area;
	}



	CvRect get_search_area() const
	{
		return search_roi;
	}



	void set_max_scans( int max_scans )
	{
		assert( max_scans > 0 );
		this->max_scans = max_scans;
	}



	int get_max_scans() const
	{
		return max_scans;
	}



	void set_search_type( int search_type )
	{
		this->search_type = search_type;
	}



	int get_search_type() const
	{
		return search_type;
	}



	void set_min_correct_scans( int scans )
	{
		assert( scans > 0 );
		min_correct_scans = scans;
	}



	int get_min_correct_scans() const
	{
		return min_correct_scans;
	}



	void set_debug( bool debug = true )
	{
		this->debug = debug;
	}

	
	
	void init_search()
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

	
	
	bool next_search()
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



	bool run( IplImage* img, std::string& barcode_value )
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
				cout << "Scan #" << num_scans << " along line: " << scan_y << endl;


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
				cout << "Min, Max along scan line: " << minv << "," << maxv << endl;


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
				cout << "Optimal threshold: " << T << endl;


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
				cout << barcode_sample << endl << endl;
				cvShowImage( "BarcodeReader-debug", debug_image );
			}


			// try decoding the barcode sample
			EAN13BarcodeDecoder barcode;
			if ( ! barcode.parse_binary_pixel_scan( barcode_sample, barcode_values[ num_scans ] ) )
			{
				if ( debug )
				{
					cout << "ERROR READING BARCODE!" << endl;
					cout << barcode_values[ num_scans ] << endl;
				}
				barcode_values[ num_scans ] = "";
			}
			else
			{
				if ( debug )
					cout << "Barcode OK!  "  << barcode_values[ num_scans ] << endl;
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
				cout << "BARCODE = " << barcode_value << "\t\tsuccessful scan reads=" << num_correct_scans << " of " << num_scans+1 << endl;
			else
				cout << "ERROR READING BARCODE!" << endl;
			cvShowImage( "BarcodeReader-debug", debug_image );
		}

		return num_correct_scans >= min_correct_scans;
	}
};





