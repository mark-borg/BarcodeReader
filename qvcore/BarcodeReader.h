
#ifndef __BARCODE_READER_H__
#define __BARCODE_READER_H__


#include "common.h"




class DLL_EXPORT BarcodeReader
{
private:

	CvRect		search_roi;
	int			max_scans;
	int			search_type;
	int			min_correct_scans;

	bool		debug;
	IplImage*	debug_image;


	std::vector< std::string > barcode_values;		// list of results of each scan

	CvRect		curr_search_roi;
	int			num_scans;
	int			scan_step;
	int			scan_step2;
	int			scan_y;


public:


	enum { SEARCH_LINEAR, SEARCH_SUB_DIVIDE };



	BarcodeReader();



	virtual ~BarcodeReader();



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


	bool run( IplImage* img, std::string& barcode_value );


protected:

	void init_search();
	
	
	bool next_search();

};



#endif

