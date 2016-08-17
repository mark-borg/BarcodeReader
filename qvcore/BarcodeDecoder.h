
#ifndef __BARCODE_DECODER_H__
#define __BARCODE_DECODER_H__


#include "common.h"



class DLL_EXPORT BarcodeDecoder
{
protected:

	std::vector< int >	lengths_histogram;
	std::vector< int >	lengths_mapping;

	bool				debug;


public:



	BarcodeDecoder()
	{
		debug = false;
	}



	virtual ~BarcodeDecoder()
	{
	}



	void set_debug( bool debug = true )
	{
		this->debug = debug;
	}

	
	
	static bool get_first_run_length( const std::string& binary_str, 
									  int& pos, char& chr, int& chr_run_length );



	static bool get_next_run_length( const std::string& binary_str, 
									 int& pos, char& chr, int& chr_run_length );



	static int find_histogram_peaks( const std::vector< int >& histogram, 
									 std::vector< int >& peaks );



	void build_lengths_histogram( const std::string& binary_pixel_scan );



	void convert_pixel_scan_to_barcode( const std::string& binary_pixel_scan, 
										std::string& binary_barcode );

};



#endif

