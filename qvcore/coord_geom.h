
#ifndef __COORD_GEOM_H__
#define __COORD_GEOM_H__


#include "common.h"



//-----------------------------------------------------------------------------------------
//	Finds the angle between the two 2D lines.
//	Lines are represented by a 3-element vector: [ a b c ] which represents the
//	equation:
//					ax + by + c = 0
//	which can be viewed as the traditional line equation:
//					y = mx + k
//	with gradient  m = -a / b  and intercept  k = -c / a.
//	The returned angle (in radians) is the acute angle, restricted between 0 and PI / 2.
//-----------------------------------------------------------------------------------------
DLL_EXPORT
double angle_between_lines( CvPoint3D32f& line1, CvPoint3D32f& line2 );



//-----------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------
DLL_EXPORT 
bool fit_line_to_image_points(	IplImage* img, CvRect& roi, 
								CvPoint3D32f& line_eqn, CvPoint3D32f& line_centre_pt, 
								int threshold = 0, double* r = NULL );


DLL_EXPORT 
CvMat* convert_3x2_matrix_to_3x3( CvMat* M );



DLL_EXPORT 
CvMat* convert_3x3_matrix_to_3x2( CvMat* M );



//-----------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------
DLL_EXPORT 
bool fit_affine_transform( CvPoint2D32f* pt, CvPoint2D32f* ref_pt, int num_pts,
						   CvMat** affine_matrix, CvMat** inv_affine_matrix, 
						   bool return_2x3_matrix = false );



DLL_EXPORT 
CvPoint3D32f cross_product( const CvPoint3D32f& v1, const CvPoint3D32f& v2 );



DLL_EXPORT 
CvPoint3D32f hc_cross_product( const CvPoint3D32f& v1, const CvPoint3D32f& v2 );



DLL_EXPORT 
CvPoint3D32f hc_transform_point( const CvPoint3D32f& pt, const CvMat& M );



DLL_EXPORT 
CvMat* build_euclidean_transform_matrix( CvPoint3D32f displacement, double rotation,
										 CvPoint3D32f rotation_centre,
										 bool return_2x3_matrix = false );




#endif

