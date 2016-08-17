
#ifndef __EAN13_BARCODE_DECODER_H__
#define __EAN13_BARCODE_DECODER_H__


#include "common.h"
#include "BarcodeDecoder.h"



class DLL_EXPORT EAN13BarcodeDecoder : public BarcodeDecoder
{
private:

	// EAN 13 decoding tables
	static std::string A[ 10 ];
	static std::string B[ 10 ];
	static std::string C[ 10 ];
	static std::string F[ 10 ];
	static std::string C1[ 4 ];
	static std::string C2[ 10 ];


public:


	EAN13BarcodeDecoder()
	{
		debug = false;
	}



	virtual ~EAN13BarcodeDecoder()
	{
	}



	int calculate_checksum( const std::string& barcode_value );



	bool parse_binary_barcode( const std::string& binary_barcode, std::string& barcode_value );



	bool parse_binary_pixel_scan( const std::string& binary_pixel_scan, std::string& barcode_value );

};



#endif

