
#include "EAN13BarcodeDecoder.h"
#include <iostream>
#include <algorithm>


using namespace std;



// decoding tables
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




int EAN13BarcodeDecoder::calculate_checksum( const string& barcode_value )
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



bool EAN13BarcodeDecoder::parse_binary_barcode( const string& binary_barcode, 
												string& barcode_value )
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
			trace_log << "ERROR READING BARCODE! - premature end of data. " << barcode_value << endl;
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
			trace_log << "ERROR READING BARCODE! - unknown digit. " << barcode_value << endl;
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
		trace_log << "ERROR READING BARCODE! - expecting the middle separator. " << barcode_value << endl;
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
		trace_log << "ERROR READING BARCODE! - can not determine first digit. " << barcode_value << endl;
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
			trace_log << "ERROR READING BARCODE! - can not decode digit in 2nd part. " << barcode_value << endl;
			return false;
		}
	}
	while( barcode_value.length() < 13 );	

	trace_log << "BARCODE = " << barcode_value << endl;


	// final separator
	if ( strncmp( ps, "101", 3 ) == 0 )
		ps += 3;
	else
	{
		barcode_value += "?";
		trace_log << "ERROR READING BARCODE! - missing final separator. " << barcode_value << endl;
		return false;
	}

	// quiet zone
	if ( strncmp( ps, "00000", 5 ) == 0 )
		ps += 5;
	else
	{
		trace_log << "WARNING: NO QUIET ZONE AFTER BARCODE!" << endl;
		while ( ps[0] == '0' )
			++ps;
	}


	// validate the checksum
	int chk = calculate_checksum( barcode_value );
	if ( chk != barcode_value[ barcode_value.length() - 1 ] - '0' )
	{
		trace_log << "ERROR READING BARCODE! - checksum failure. " << barcode_value << endl;
		return false;
	}


	// check if there is a 2- or 5-digit "add-on"
	if ( strncmp( ps, "1011", 4 ) == 0 )
	{
		trace_log << "Possible presence of a 2- or 5-digit 'add-on'." << endl;
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
				trace_log << "2-digit add-on checksum validated." << endl;
				ok = true;
			}
			else
			{
				trace_log << "2-digit add-on checksum failure." << endl;
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
				trace_log << "5-digit add-on checksum validated." << endl;
				ok = true;
			}
			else
			{
				trace_log << "5-digit add-on checksum failure." << endl;
				ok = false;
			}
		}
		else
		{
			trace_log << "Incorrect add-on length. " << barcode_value << endl;
		}

		if ( ok )
		{
			trace_log << "BARCODE ADD-ON = " << add_on << endl;
			barcode_value += add_on;
		}
	}


	return true;
}



bool EAN13BarcodeDecoder::parse_binary_pixel_scan( const string& binary_pixel_scan, 
												   string& barcode_value )
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
	
	if ( debug )
		trace_log << "PEAKS: " << endl << histogram_peaks << endl << endl;


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
	
	if ( debug )
		trace_log << "ADJ PEAKS: " << endl << histogram_peaks << endl << endl;


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
	
	if ( debug )
		trace_log << "LEN MAPPING: " << endl << lengths_mapping << endl << endl;


	// convert the raw barcode string to a binary barcode using the lengths mapping table
	string binary_barcode;
	convert_pixel_scan_to_barcode( binary_pixel_scan, binary_barcode );


	return parse_binary_barcode( binary_barcode, barcode_value ); 
}


