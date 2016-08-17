
#include "coord_geom.h"
#include <iostream>
#include <cmath>
#include <cfloat>


using namespace std;



double angle_between_lines( CvPoint3D32f& line1, CvPoint3D32f& line2 )
{
	//	Finds the angle between the two 2D lines.
	//	Lines are represented by a 3-element vector: [ a b c ] which represents the
	//	equation:
	//					ax + by + c = 0
	//	which can be viewed as the traditional line equation:
	//					y = mx + k
	//	with gradient  m = -a / b  and intercept  k = -c / a.
	//	The returned angle (in radians) is the acute angle, restricted between 0 and PI / 2.
	//

	// we use the gradient = -a / b   for line  ax + by + c = 0
	double angle1 = CV_PI / 2.0, angle2 = CV_PI / 2.0;
	if ( line1.y != 0.0 )
	{
		//angle1 = atan( - line1.x / line1.y );
		angle1 = - line1.x / line1.y;
		angle1 = angle1 < 0.0 ? CV_PI + atan( angle1 ) : atan( angle1 ); 
	}
	if ( line2.y != 0.0 )
	{
		//angle2 = atan( - line2.x / line2.y );
		angle2 = - line2.x / line2.y;
		angle2 = angle2 < 0.0 ? CV_PI + atan( angle2 ) : atan( angle2 ); 
	}

	double diff = angle2 - angle1;

	// correct to acute angle, since a line can arbitrarily have one of 2 directions
	if ( fabs( diff ) > CV_PI / 2.0 )
		diff = ( fabs( diff ) - CV_PI ) * ( diff < 0.0 ? -1 : 1 );

	return diff;
}



bool fit_line_to_image_points(	IplImage* img, CvRect& roi, 
								CvPoint3D32f& line_eqn, CvPoint3D32f& line_centre_pt, 
								int threshold, double* r )
{
	//	Fits a line to the pixels in the given roi which have a value greater than threshold.
	//
	//	This method uses a least-squares regression method to fit the line to the data.
	//	The line is represented by the equation:
	//					ax + by + c = 0
	//	which can be written in vector form as:
	//					[ a b c ]
	//
	//	The equation  ax + by + c = 0  can be transformed into:
	//					y = m x + c
	//	where m is the gradient and c is the intercept.
	//	The solution is found by solving the set of simultaneous equations made up from all 
	//	the n points in the image roi:
	//			sum(y)  = m sum(x) + n(c)
	//			sum(xy) = m sum(x*x) + c sum(x)
	//	The above are called the 'normal equations' for the least-squares line.
	//
	//	This method returns false if no points are found in the roi, or only one point exists.
	//
	//	Note that for vertical lines, the line equation y = mx + c fails, because
	//	the gradient m is infinite. But this is transparent to the caller, since vertical
	//	lines don't suffer from this problem, if the homogenous vector form is used.
	//	
	//	The method also returns the line centre point (with respect to the given points). The
	//	fitted line always passes through the centroid of the given data points. The point
	//	is specified in homogeneous coordinates, i.e. [ x y 1 ].
	//
	//	If parameter r is not NULL, then this function calculates the linear correlation
	//	coefficient r. This gives the fraction of the total variation which is explained by the
	//	least-squares regression line. I.e. r measures how well the line fits the given data 
	//	points. If the total variation is all explained by the line, then r = +/-1, and one can
	//	say that there is a perfect linear correlation; if it is all unexplained by the line,
	//	then r is 0. Normally r lies between 0 and 1.
	//


	assert( img );
	assert( img->nChannels == 1 );
	
	double sum_x = 0.0, sum_y = 0.0, sum_x_sq = 0.0, sum_x_y = 0.0;
	long total_pts = 0;

	for ( int y = roi.y; y < ( roi.y + roi.height ); ++y )
	{
		int x = roi.x;

		unsigned char* ptr = (unsigned char*)( img->imageData + y * img->widthStep + x );

		for ( ; x < ( roi.x + roi.width ); ++x, ++ptr )
		{
			if ( *ptr > threshold )
			{
				++total_pts;
				sum_x += x;
				sum_y += y;
				sum_x_y += x * y;
				sum_x_sq += x * x;
			}
		}
	}

	if ( total_pts <= 1 )
		return false;			// no points or only 1 point found!


	double denom = total_pts * sum_x_sq - sum_x * sum_x; 

	// calculate line parameters  y = m x + c
	double m = ( total_pts * sum_x_y - sum_x * sum_y ) / denom;
	double c = ( sum_y * sum_x_sq - sum_x * sum_x_y ) / denom;

	// fitted line always passes through centroid of data
	line_centre_pt.x = sum_x / total_pts;
	line_centre_pt.y = sum_y / total_pts;
	line_centre_pt.z = 1.0;

	// build homogeneous form
	if ( _isnan( m ) )
	{
		line_eqn.x = -1.0;
		line_eqn.y = 0.0;
		line_eqn.z = line_centre_pt.x;
	}
	else
	{
		line_eqn.x = m;
		line_eqn.y = -1.0;
		line_eqn.z = c;
	}

	// calclate linear correlation coefficient (if needed)
	if ( r != NULL )
	{
		double err_est = 0.0, err_cy = 0.0;

		for ( int y = roi.y; y < ( roi.y + roi.height ); ++y )
		{
			int x = roi.x;

			unsigned char* ptr = (unsigned char*)( img->imageData + y * img->widthStep + x );

			for ( ; x < ( roi.x + roi.width ); ++x, ++ptr )
			{
				if ( *ptr > threshold )
				{
					double estimated_y = ( denom == 0.0 || fabs( m ) < 0.01 ) ? y : m * x + c;
					err_est += ( y - estimated_y ) * ( y - estimated_y );
					err_cy += ( y - line_centre_pt.y ) * ( y - line_centre_pt.y );
				}
			}
		}

		*r = sqrt( 1.0 - err_est / err_cy );
	}

	return true;
}



CvMat* convert_3x2_matrix_to_3x3( CvMat* M )
{
	assert( M );
	assert( M->cols == 3 && M->rows == 2 );
	

	CvMat* MA = cvCreateMat( 3, 3, CV_32FC1 );

	for ( int y = 0; y < 2; ++y )
		for ( int x = 0; x < 3; ++x )
			cvmSet( MA, y, x, cvmGet( M, y, x ) );

	cvmSet( MA, 2, 0, 0.0 );
	cvmSet( MA, 2, 1, 0.0 );
	cvmSet( MA, 2, 2, 1.0 );

	cvReleaseMat( &M );

	return MA;
}



CvMat* convert_3x3_matrix_to_3x2( CvMat* M )
{
	assert( M );
	assert( M->cols == 3 && M->rows == 3 );
	

	CvMat* MA = cvCreateMat( 2, 3, CV_32FC1 );

	for ( int y = 0; y < 2; ++y )
		for ( int x = 0; x < 3; ++x )
			cvmSet( MA, y, x, cvmGet( M, y, x ) );

	cvReleaseMat( &M );

	return MA;
}



bool fit_affine_transform( CvPoint2D32f* pt, CvPoint2D32f* ref_pt, int num_pts,
						   CvMat** affine_matrix, CvMat** inv_affine_matrix, 
						   bool return_2x3_matrix )
{
	//	Determines the affine transform from the given points pt to the reference points ref_pt
	//
	//	Affine matrix M =	[ a b u ]
	//						[ c d v ]
	//						[ 0 0 1 ]
	//
	//	Given a point p from pt, where p is represented in 
	//	homogeneous form p =	[ px ]
	//							[ py ]
	//							[ 1  ]
	//		Mp = r
	//
	//		where r =	[ a(px) + b(py) + u ],  r in ref_pt
	//					[ c(px) + d(py) + v ]
	//					[ 1                 ]
	//
	//	We solve a set of linear equations to get the affine transform parameters 
	//	a, b, c, d, u, v. Parameters a, b, c, d encode the rotaton, scaling and shear
	//  part of the affine transform, while parameters u, v encode the translation.
	//	
	//	A minimum of 3 points is required (6 linear equations).
	//
	//	Writing the linear equations as:
	//		a(p1x) + b(p1y) + 0c     + 0d     +  u + 0v  = r1x
	//		0a     + 0b     + c(p1x) + d(p1y) + 0u +  v  = r1y
	//		...etc.
	//
	//	Which can be written in matrix form:
	//		Ax = r
	//	or:
	//		[ p1x  p1y  0    0    u  0 ] [ a ]   [ rx1 ]
	//		[ 0    0	p1x	 p1y  0  v ] [ b ]   [ ry1 ]
	//		[ p2x  p2y  0    0    u  0 ] [ c ]   [ rx2 ]
	//		[ 0    0	p2x	 p2y  0  v ] [ d ] = [ ry2 ]
	//		[ p3x  p3y  0    0    u  0 ] [ u ]   [ rx3 ]
	//		[ 0    0	p3x	 p3y  0  v ] [ v ]   [ ry3 ]
	//
	//	More than 3 points can be given for better accuracy. SVD is used to solve the
	//	set of linear equations.
	//	Returns false if the matrix is singular, i.e. no solution can be found.
	//
	//	If the parameter return_2x3_matrix is set to true, then the returned matrix M is 
	//	of the form:
	//					M =	[ a b u ]
	//						[ c d v ]
	//	


	assert( affine_matrix != NULL );
	assert( inv_affine_matrix != NULL );
	assert( *affine_matrix == NULL );
	assert( *inv_affine_matrix == NULL );
	assert( num_pts >= 3 );


	CvMat* A = cvCreateMat( num_pts * 2, 6, CV_32FC1 );	// the equations matrix
	assert( A );

	CvMat* X = cvCreateMat( 6, 1, CV_32FC1 );			// the affine parameters (solution)
	assert( X );
	
	CvMat* R = cvCreateMat( num_pts * 2, 1, CV_32FC1 );	// the rhs
	assert( R );
	
	for ( int k = 0; k < num_pts; ++k )
	{
		cvmSet( A, k*2, 0, pt[k].x );
		cvmSet( A, k*2, 1, pt[k].y );
		cvmSet( A, k*2, 2, 1.0 );
		cvmSet( A, k*2, 3, 0.0 );
		cvmSet( A, k*2, 4, 0.0 );
		cvmSet( A, k*2, 5, 0.0 );

		cvmSet( A, k*2+1, 0, 0.0 );
		cvmSet( A, k*2+1, 1, 0.0 );
		cvmSet( A, k*2+1, 2, 0.0 );
		cvmSet( A, k*2+1, 3, pt[k].x );
		cvmSet( A, k*2+1, 4, pt[k].y );
		cvmSet( A, k*2+1, 5, 1.0 );

		cvmSet( R, k*2,	  0, ref_pt[k].x );
		cvmSet( R, k*2+1, 0, ref_pt[k].y );
	}

	bool success = true;

	if ( cvSolve( A, R, X, CV_SVD ) )
	{
		// the affine matrix
		CvMat* A = cvCreateMat( 3, 3, CV_32FC1 );	// the affine matrix
		assert( A );
	
		cvmSet( A, 0, 0, cvmGet( X, 0, 0 ) );
		cvmSet( A, 0, 1, cvmGet( X, 1, 0 ) );
		cvmSet( A, 0, 2, cvmGet( X, 2, 0 ) );
		cvmSet( A, 1, 0, cvmGet( X, 3, 0 ) );
		cvmSet( A, 1, 1, cvmGet( X, 4, 0 ) );
		cvmSet( A, 1, 2, cvmGet( X, 5, 0 ) );
		cvmSet( A, 2, 0, 0.0 );
		cvmSet( A, 2, 1, 0.0 );
		cvmSet( A, 2, 2, 1.0 );

		// compute also its inverse, i.e. transfrom from ref_pt to pt
		*inv_affine_matrix = cvCreateMat( 3, 3, CV_32FC1 );	// inverse of affine matrix
		assert( *inv_affine_matrix );

		if ( ! cvInvert( A, *inv_affine_matrix ) )
		{
			trace_log << "Matrix inverse failed!" <<endl;
			success = false;
			cvReleaseMat( inv_affine_matrix );
			*inv_affine_matrix = NULL;
		}

		if ( return_2x3_matrix )
		{
			*affine_matrix = cvCreateMat( 2, 3, CV_32FC1 );	// the affine matrix
			assert( *affine_matrix );

			cvmSet( *affine_matrix, 0, 0, cvmGet( A, 0, 0 ) );
			cvmSet( *affine_matrix, 0, 1, cvmGet( A, 0, 1 ) );
			cvmSet( *affine_matrix, 0, 2, cvmGet( A, 0, 2 ) );
			cvmSet( *affine_matrix, 1, 0, cvmGet( A, 1, 0 ) );
			cvmSet( *affine_matrix, 1, 1, cvmGet( A, 1, 1 ) );
			cvmSet( *affine_matrix, 1, 2, cvmGet( A, 1, 2 ) );
		}
		else
		{
			*affine_matrix = A;
		}
	}
	else
	{
		trace_log << "Matrix is singular!" << endl;
		success = false;
		*affine_matrix = NULL;
	}

	cvReleaseMat( &A );
	cvReleaseMat( &R );
	cvReleaseMat( &X );

	return success;
}




CvPoint3D32f cross_product( const CvPoint3D32f& v1, const CvPoint3D32f& v2 )
{
	CvPoint3D32f v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;

	return v;
}




CvPoint3D32f hc_cross_product( const CvPoint3D32f& v1, const CvPoint3D32f& v2 )
{
	CvPoint3D32f v = cross_product( v1, v2 );

	if ( v.z != 0.0 )
	{
		v.x /= v.z;
		v.y /= v.z;
		v.z /= v.z;		// normalise homogeneous coords
	}

	return v;
}




CvPoint3D32f hc_transform_point( const CvPoint3D32f& pt, const CvMat& M )
{
	CvPoint3D32f rpt;
	
	rpt.x = pt.x * cvmGet( &M, 0, 0 ) +
			pt.y * cvmGet( &M, 0, 1 ) +
			pt.z * cvmGet( &M, 0, 2 );
	rpt.y =	pt.x * cvmGet( &M, 1, 0 ) +
			pt.y * cvmGet( &M, 1, 1 ) +
			pt.z * cvmGet( &M, 1, 2 );
	rpt.z = pt.x * cvmGet( &M, 2, 0 ) +
			pt.y * cvmGet( &M, 2, 1 ) +
			pt.z * cvmGet( &M, 2, 2 );

	// normalise homogeneous coords
	if ( rpt.z != 0.0 )
	{
		rpt.x /= rpt.z;
		rpt.y /= rpt.z;
		rpt.z /= rpt.z;
	}

	return rpt;
}



CvMat* build_euclidean_transform_matrix( CvPoint3D32f displacement, double rotation,
										 CvPoint3D32f rotation_centre,
										 bool return_2x3_matrix )
{
	CvMat* M = cvCreateMat( return_2x3_matrix ? 2 : 3, 3, CV_32FC1 );	
	
	// normalise homogeneous coords
	if ( displacement.z != 1.0 )
	{
		displacement.x /= displacement.z;
		displacement.y /= displacement.z;
		displacement.z /= displacement.z;
	}
	if ( rotation_centre.z != 1.0 )
	{
		rotation_centre.x /= rotation_centre.z;
		rotation_centre.y /= rotation_centre.z;
		rotation_centre.z /= rotation_centre.z;
	}

	register double c = cos( rotation ), s = sin( rotation );

	// rotation around the origin?
	if ( rotation_centre.x == 0 && rotation_centre.y == 0 )
	{
		cvmSet( M, 0, 0, c );
		cvmSet( M, 0, 1, -s );
		cvmSet( M, 0, 2, displacement.x );
		cvmSet( M, 1, 0, s );
		cvmSet( M, 1, 1, c );
		cvmSet( M, 1, 2, displacement.y );
	}
	else
	{
		// rotation about an arbitrary point
		cvmSet( M, 0, 0, c );
		cvmSet( M, 0, 1, -s );
		cvmSet( M, 0, 2, ( 1 - c ) * rotation_centre.x + s * rotation_centre.y + displacement.x );
		cvmSet( M, 1, 0, s );
		cvmSet( M, 1, 1, c );
		cvmSet( M, 1, 2, s * rotation_centre.x - ( 1 - c ) * rotation_centre.y + displacement.y );
	}

	if ( ! return_2x3_matrix )
	{
		cvmSet( M, 2, 0, 0.0 );
		cvmSet( M, 2, 1, 0.0 );
		cvmSet( M, 2, 2, 1.0 );
	}

	return M;
}

