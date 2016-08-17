/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//			  Intel License Agreement
//		  For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
#include "_cv.h"

/* Creates new histogram */
CvHistogram *
cvCreateHist( int dims, int *sizes, CvHistType type, float** ranges, int uniform )
{
    CvHistogram *hist = 0;

    CV_FUNCNAME( "cvCreateHist" );
    __BEGIN__;

    if( (unsigned)dims > CV_MAX_DIM )
        CV_ERROR( CV_BadOrder, "Number of dimensions is out of range" );

    if( !sizes )
        CV_ERROR( CV_HeaderIsNull, "Null <sizes> pointer" );

    CV_CALL( hist = (CvHistogram *)cvAlloc( sizeof( CvHistogram )));

    hist->type = CV_HIST_MAGIC_VAL;
    hist->thresh2 = 0;
    hist->bins = 0;
    if( type == CV_HIST_ARRAY )
    {
        CV_CALL( hist->bins = cvInitMatNDHeader( &hist->mat, dims, sizes,
                                                 CV_HIST_DEFAULT_TYPE ));
        CV_CALL( cvCreateData( hist->bins ));
    }
    else if( type == CV_HIST_SPARSE )
    {
        CV_CALL( hist->bins = cvCreateSparseMat( dims, sizes, CV_HIST_DEFAULT_TYPE ));
    }
    else
    {
        CV_ERROR( CV_StsBadArg, "Invalid histogram type" );
    }

    if( ranges )
        CV_CALL( cvSetHistBinRanges( hist, ranges, uniform ));

    __END__;

    if( cvGetErrStatus() < 0 )
        cvReleaseHist( &hist );

    return hist;
}


/* Creates histogram wrapping header for given array */
CV_IMPL CvHistogram*
cvMakeHistHeaderForArray( int dims, int *sizes, CvHistogram *hist,
                          float *data, float **ranges, int uniform )
{
    CvHistogram* result = 0;
    
    CV_FUNCNAME( "cvMakeHistHeaderForArray" );

    __BEGIN__;

    if( !hist )
        CV_ERROR( CV_StsNullPtr, "Null histogram header pointer" );

    if( !data )
        CV_ERROR( CV_StsNullPtr, "Null data pointer" );

    hist->thresh2 = 0;
    hist->type = CV_HIST_MAGIC_VAL;
    CV_CALL( hist->bins = cvInitMatNDHeader( &hist->mat, dims, sizes,
                                             CV_HIST_DEFAULT_TYPE, data ));

    if( ranges )
    {
        if( !uniform )
            CV_ERROR( CV_StsBadArg, "Only uniform bin ranges can be used here "
                                    "(to avoid memory allocation)" );
        CV_CALL( cvSetHistBinRanges( hist, ranges, uniform ));
    }

    result = hist;

    __END__;

    if( cvGetErrStatus() < 0 && hist )
    {
        hist->type = 0;
        hist->bins = 0;
    }

    return result;
}


CV_IMPL void
cvReleaseHist( CvHistogram **hist )
{
    CV_FUNCNAME( "cvReleaseHist" );
    
    __BEGIN__;

    if( !hist )
        CV_ERROR( CV_StsNullPtr, "" );

    if( *hist )
    {
        CvHistogram* temp = *hist;

        if( !CV_IS_HIST(temp))
            CV_ERROR( CV_StsBadArg, "Invalid histogram header" );

        *hist = 0;

        if( CV_IS_SPARSE_HIST( temp ))
            cvReleaseSparseMat( (CvSparseMat**)&temp->bins );
        else
        {
            cvReleaseData( temp->bins );
            temp->bins = 0;
        }
        
        if( temp->thresh2 )
            cvFree( (void**)&temp->thresh2 );

        cvFree( (void**)&temp );
    }

    __END__;
}


CV_IMPL void
cvClearHist( CvHistogram *hist )
{
    CV_FUNCNAME( "cvClearHist" );
    
    __BEGIN__;

    if( !CV_IS_HIST(hist) )
        CV_ERROR( CV_StsBadArg, "Invalid histogram header" );

    cvZero( hist->bins );

    __END__;
}


// Clears histogram bins that are below than threshold
CV_IMPL void
cvThreshHist( CvHistogram* hist, double thresh )
{
    CV_FUNCNAME( "cvThreshHist" );

    __BEGIN__;

    if( !CV_IS_HIST(hist) )
        CV_ERROR( CV_StsBadArg, "Invalid histogram header" );

    if( !CV_IS_SPARSE_MAT(hist->bins) )
    {
        CvMat mat;
        CV_CALL( cvGetMat( hist->bins, &mat, 0, 1 ));
        CV_CALL( cvThreshold( &mat, &mat, thresh, 0, CV_THRESH_TOZERO ));
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)hist->bins;
        CvSparseMatIterator iterator;
        CvSparseNode *node;
        
        for( node = cvInitSparseMatIterator( mat, &iterator );
             node != 0; node = cvGetNextSparseNode( &iterator ))
        {
            float* val = (float*)CV_NODE_VAL( mat, node );
            if( *val <= thresh )
                *val = 0;
        }
    }
    
    __END__;
}


// Normalizes histogram (make sum of the histogram bins == factor)
CV_IMPL void
cvNormalizeHist( CvHistogram* hist, double factor )
{
    double sum = 0;

    CV_FUNCNAME( "cvNormalizeHist" );
    __BEGIN__;

    if( !CV_IS_HIST(hist) )
        CV_ERROR( CV_StsBadArg, "Invalid histogram header" );

    if( !CV_IS_SPARSE_HIST(hist) )
    {
        CvMat mat;
        CV_CALL( cvGetMat( hist->bins, &mat, 0, 1 ));
        CV_CALL( sum = cvSum( &mat ).val[0] );
        if( sum == 0 )
            sum = 1;
        CV_CALL( cvScale( &mat, &mat, factor/sum, 0 ));
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)hist->bins;
        CvSparseMatIterator iterator;
        CvSparseNode *node;
        float scale;
        
        for( node = cvInitSparseMatIterator( mat, &iterator );
             node != 0; node = cvGetNextSparseNode( &iterator ))
        {
            sum += *(float*)CV_NODE_VAL(mat,node);
        }

        if( sum == 0 )
            sum = 1;
        scale = (float)(factor/sum);

        for( node = cvInitSparseMatIterator( mat, &iterator );
             node != 0; node = cvGetNextSparseNode( &iterator ))
        {
            *(float*)CV_NODE_VAL(mat,node) *= scale;
        }
    }

    __END__;
}


// Retrieves histogram global min, max and their positions
CV_IMPL void
cvGetMinMaxHistValue( const CvHistogram* hist,
                      float *value_min, float* value_max,
                      int* idx_min, int* idx_max )
{
    double minVal, maxVal;
    int dims, size[CV_MAX_DIM];

    CV_FUNCNAME( "cvGetMinMaxHistValue" );

    __BEGIN__;

    if( !CV_IS_HIST(hist) )
        CV_ERROR( CV_StsBadArg, "Invalid histogram header" );

    dims = cvGetDims( hist->bins, size );

    if( !CV_IS_SPARSE_HIST(hist) )
    {
        CvMat mat;
        CvPoint minPt, maxPt;

        CV_CALL( cvGetMat( hist->bins, &mat, 0, 1 ));
        CV_CALL( cvMinMaxLoc( &mat, &minVal, &maxVal, &minPt, &maxPt ));

        if( dims == 1 )
        {
            if( idx_min )
                *idx_min = minPt.y + minPt.x;
            if( idx_max )
                *idx_max = maxPt.y + maxPt.x;
        }
        else if( dims == 2 )
        {
            if( idx_min )
                idx_min[0] = minPt.y, idx_min[1] = minPt.x;
            if( idx_max )
                idx_max[0] = maxPt.y, idx_max[1] = maxPt.x;
        }
        else if( idx_min || idx_max )
        {
            int imin = minPt.y*mat.cols + minPt.x;
            int imax = maxPt.y*mat.cols + maxPt.x;
            int i;
           
            for( i = dims - 1; i >= 0; i-- )
            {
                if( idx_min )
                {
                    int t = imin / size[i];
                    idx_min[i] = imin - t*size[i];
                    imin = t;
                }

                if( idx_max )
                {
                    int t = imax / size[i];
                    idx_max[i] = imax - t*size[i];
                    imax = t;
                }
            }
        }
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)hist->bins;
        CvSparseMatIterator iterator;
        CvSparseNode *node;
        int minv = CV_POS_INF;
        int maxv = CV_NEG_INF;
        CvSparseNode* minNode = 0;
        CvSparseNode* maxNode = 0;

        for( node = cvInitSparseMatIterator( mat, &iterator );
             node != 0; node = cvGetNextSparseNode( &iterator ))
        {
            int value = *(int*)CV_NODE_VAL(mat,node);
            value = CV_TOGGLE_FLT(value);
            if( value < minv )
            {
                minv = value;
                minNode = node;
            }

            if( value > maxv )
            {
                maxv = value;
                maxNode = node;
            }
        }

        if( !minNode )
        {
            assert( !maxNode );
            minVal = maxVal = 0;
            if( idx_min )
                memset( idx_min, -1, dims*sizeof(idx_min[0]));
            if( idx_max )
                memset( idx_max, -1, dims*sizeof(idx_max[0]));
        }
        else
        {
            assert( maxNode );
            minv = CV_TOGGLE_FLT(minv);
            maxv = CV_TOGGLE_FLT(maxv);
            minVal = (float&)minv;
            maxVal = (float&)maxv;
            if( idx_min )
                memcpy( idx_min, (uchar*)minNode+mat->idxoffset, dims*sizeof(idx_min[0]));
            if( idx_max )
                memcpy( idx_max, (uchar*)maxNode+mat->idxoffset, dims*sizeof(idx_max[0]));
        }
    }

    if( value_min )
        *value_min = (float)minVal;

    if( value_max )
        *value_max = (float)maxVal;

    __END__;
}


// Compares two histograms using one of a few methods
CV_IMPL double
cvCompareHist( const CvHistogram* hist1,
               const CvHistogram* hist2,
               int method )
{
    double _result = -1;
    
    CV_FUNCNAME( "cvCompareHist" );

    __BEGIN__;

    int i, dims1, dims2;
    int size1[CV_MAX_DIM], size2[CV_MAX_DIM], total = 1;
    double result = 0;
        
    if( !CV_IS_HIST(hist1) || !CV_IS_HIST(hist2) )
        CV_ERROR( CV_StsBadArg, "Invalid histogram header[s]" );

    if( CV_IS_SPARSE_MAT(hist1->bins) != CV_IS_SPARSE_MAT(hist2->bins))
        CV_ERROR(CV_StsUnmatchedFormats, "One of histograms is sparse and other is not");

    CV_CALL( dims1 = cvGetDims( hist1->bins, size1 ));
    CV_CALL( dims2 = cvGetDims( hist2->bins, size2 ));
    
    if( dims1 != dims2 )
        CV_ERROR( CV_StsUnmatchedSizes,
                  "The histograms have different numbers of dimensions" );

    for( i = 0; i < dims1; i++ )
    {
        if( size1[i] != size2[i] )
            CV_ERROR( CV_StsUnmatchedSizes, "The histograms have different sizes" );
        total *= size1[i];
    }


    if( !CV_IS_SPARSE_MAT(hist1->bins))
    {
        float *ptr1 = 0, *ptr2 = 0;
        CV_CALL( cvGetRawData( hist1->bins, (uchar**)&ptr1 ));
        CV_CALL( cvGetRawData( hist2->bins, (uchar**)&ptr2 ));

        switch( method )
        {
        case CV_COMP_CHISQR:
            for( i = 0; i < total; i++ )
            {
                double a = ptr1[i] - ptr2[i];
                double b = ptr1[i] + ptr2[i];
                if( b != 0 )
                    result += a*a/b;
            }
            break;
        case CV_COMP_CORREL:
            {
                double s1 = 0, s11 = 0;
                double s2 = 0, s22 = 0;
                double s12 = 0;
                double num, denom2, scale = 1./total;
                
                for( i = 0; i < total; i++ )
                {
                    double a = ptr1[i];
                    double b = ptr2[i];

                    s12 += a*b;
                    s1 += a;
                    s11 += a*a;
                    s2 += b;
                    s22 += b*b;
                }

                num = s12 - s1*s2*scale;
                denom2 = (s11 - s1*s1*scale)*(s22 - s2*s2*scale);
                result = denom2 != 0 ? num/sqrt(denom2) : 1;
            }
            break;
        case CV_COMP_INTERSECT:
            for( i = 0; i < total; i++ )
            {
                float a = ptr1[i];
                float b = ptr2[i];
                if( a <= b )
                    result += a;
                else
                    result += b;
            }
            break;
        default:
            CV_ERROR( CV_StsBadArg, "Unknown comparison method" );
        }
    }
    else
    {
        CvSparseMat* mat1 = (CvSparseMat*)(hist1->bins);
        CvSparseMat* mat2 = (CvSparseMat*)(hist2->bins);
        CvSparseMatIterator iterator;
        CvSparseNode *node1, *node2;

        if( mat1->heap->active_count > mat2->heap->active_count )
        {
            CvSparseMat* t;
            CV_SWAP( mat1, mat2, t );
        }

        switch( method )
        {
        case CV_COMP_CHISQR:
            for( node1 = cvInitSparseMatIterator( mat1, &iterator );
                 node1 != 0; node1 = cvGetNextSparseNode( &iterator ))
            {
                double v1 = *(float*)CV_NODE_VAL(mat1,node1);
                uchar* node2 = cvPtrND( mat2, CV_NODE_IDX(mat1,node1), 0, 0, &node1->hashval );
                if( !node2 )
                    result += v1;
                else
                {
                    double v2 = *(float*)node2;
                    double a = v1 - v2;
                    double b = v1 + v2;
                    if( b != 0 )
                        result += a*a/b;
                }
            }

            for( node2 = cvInitSparseMatIterator( mat2, &iterator );
                 node2 != 0; node2 = cvGetNextSparseNode( &iterator ))
            {
                double v2 = *(float*)CV_NODE_VAL(mat2,node2);
                if( !cvPtrND( mat1, CV_NODE_IDX(mat2,node2), 0, 0, &node2->hashval ))
                    result += v2;
            }
            break;
        case CV_COMP_CORREL:
            {
                double s1 = 0, s11 = 0;
                double s2 = 0, s22 = 0;
                double s12 = 0;
                double num, denom2, scale = 1./total;
                
                for( node1 = cvInitSparseMatIterator( mat1, &iterator );
                     node1 != 0; node1 = cvGetNextSparseNode( &iterator ))
                {
                    double v1 = *(float*)CV_NODE_VAL(mat1,node1);
                    uchar* node2 = cvPtrND( mat2, CV_NODE_IDX(mat1,node1), 0, 0, &node1->hashval );
                    if( node2 )
                    {
                        double v2 = *(float*)node2;
                        s12 += v1*v2;
                    }
                    s1 += v1;
                    s11 += v1*v1;
                }

                for( node2 = cvInitSparseMatIterator( mat2, &iterator );
                     node2 != 0; node2 = cvGetNextSparseNode( &iterator ))
                {
                    double v2 = *(float*)CV_NODE_VAL(mat2,node2);
                    s2 += v2;
                    s22 += v2*v2;
                }

                num = s12 - s1*s2*scale;
                denom2 = (s11 - s1*s1*scale)*(s22 - s2*s2*scale);
                result = denom2 != 0 ? num/sqrt(denom2) : 1;
            }
            break;
        case CV_COMP_INTERSECT:
            {
                for( node1 = cvInitSparseMatIterator( mat1, &iterator );
                     node1 != 0; node1 = cvGetNextSparseNode( &iterator ))
                {
                    float v1 = *(float*)CV_NODE_VAL(mat1,node1);
                    uchar* node2 = cvPtrND( mat2, CV_NODE_IDX(mat1,node1), 0, 0, &node1->hashval );
                    if( node2 )
                    {
                        float v2 = *(float*)node2;
                        if( v1 <= v2 )
                            result += v1;
                        else
                            result += v2;
                    }
                }
            }
            break;
        default:
            CV_ERROR( CV_StsBadArg, "Unknown comparison method" );
        }
    }

    _result = result;
    
    __END__;
    
    return _result;
}

// copies one histogram to another
CV_IMPL void
cvCopyHist( const CvHistogram* src, CvHistogram** _dst )
{
    CV_FUNCNAME( "cvCopyHist" );

    __BEGIN__;

    int eq = 0;
    int is_sparse;
    int i, dims1, dims2;
    int size1[CV_MAX_DIM], size2[CV_MAX_DIM], total = 1;
    float* ranges[CV_MAX_DIM];
    float** thresh = 0;
    CvHistogram* dst;
    
    if( !_dst )
        CV_ERROR( CV_StsNullPtr, "Destination double pointer is NULL" );

    dst = *_dst;

    if( !CV_IS_HIST(src) || (dst && !CV_IS_HIST(dst)) )
        CV_ERROR( CV_StsBadArg, "Invalid histogram header[s]" );

    is_sparse = CV_IS_SPARSE_MAT(src->bins);
    CV_CALL( dims1 = cvGetDims( src->bins, size1 ));
    for( i = 0; i < dims1; i++ )
        total *= size1[i];

    if( dst && is_sparse == CV_IS_SPARSE_MAT(dst->bins))
    {
        CV_CALL( dims2 = cvGetDims( dst->bins, size2 ));
    
        if( dims1 == dims2 )
        {
            for( i = 0; i < dims1; i++ )
                if( size1[i] != size2[i] )
                    break;
        }

        eq = i == dims1;
    }

    if( !eq )
    {
        cvReleaseHist( _dst );
        CV_CALL( dst = cvCreateHist( dims1, size1,
                 !is_sparse ? CV_HIST_ARRAY : CV_HIST_SPARSE, 0, 0 ));
        *_dst = dst;
    }

    if( CV_HIST_HAS_RANGES( src ))
    {
        if( CV_IS_UNIFORM_HIST( src ))
        {
            for( i = 0; i < dims1; i++ )
                ranges[i] = (float*)src->thresh[i];
            thresh = ranges;
        }
        else
            thresh = src->thresh2;
        CV_CALL( cvSetHistBinRanges( dst, thresh, CV_IS_UNIFORM_HIST(src)));
    }

    CV_CALL( cvCopy( src->bins, dst->bins ));
    
    __END__;
}


// Sets a value range for every histogram bin
CV_IMPL void
cvSetHistBinRanges( CvHistogram* hist, float** ranges, int uniform )
{
    CV_FUNCNAME( "cvSetHistBinRanges" );
    
    __BEGIN__;

    int dims, size[CV_MAX_DIM], total = 0;
    int i, j;
    
    if( !ranges )
        CV_ERROR( CV_StsNullPtr, "NULL ranges pointer" );

    if( !CV_IS_HIST(hist) )
        CV_ERROR( CV_StsBadArg, "Invalid histogram header" );

    CV_CALL( dims = cvGetDims( hist->bins, size ));
    for( i = 0; i < dims; i++ )
        total += size[i]+1;
    
    if( uniform )
    {
        for( i = 0; i < dims; i++ )
        {
            if( !ranges[i] )
                CV_ERROR( CV_StsNullPtr, "One of <ranges> elements is NULL" );
            hist->thresh[i][0] = ranges[i][0];
            hist->thresh[i][1] = ranges[i][1];
        }

        hist->type |= CV_HIST_UNIFORM_FLAG + CV_HIST_RANGES_FLAG;
    }
    else
    {
        float* dim_ranges;

        if( !hist->thresh2 )
        {
            CV_CALL( hist->thresh2 = (float**)cvAlloc(
                        dims*sizeof(hist->thresh2[0])+
                        total*sizeof(hist->thresh2[0][0])));
        }
        dim_ranges = (float*)(hist->thresh2 + dims);

        for( i = 0; i < dims; i++ )
        {
            float val0 = -FLT_MAX;

            if( !ranges[i] )
                CV_ERROR( CV_StsNullPtr, "One of <ranges> elements is NULL" );
            
            for( j = 0; j <= size[i]; j++ )
            {
                float val = ranges[i][j];
                if( val <= val0 )
                    CV_ERROR(CV_StsOutOfRange, "Bin ranges should go in ascenting order");
                val0 = dim_ranges[j] = val;
            }

            hist->thresh2[i] = dim_ranges;
            dim_ranges += size[i] + 1;
        }

        hist->type |= CV_HIST_RANGES_FLAG;
        hist->type &= ~CV_HIST_UNIFORM_FLAG;
    }

    __END__;
}


#define  ICV_HIST_DUMMY_IDX  (INT_MIN/3)

static CvStatus
icvCalcHistLookupTables8x( const CvHistogram* hist, int lo, int hi,
                           int dims, int* size, int* tab )
{
    int is_sparse = CV_IS_SPARSE_HIST( hist );
    int have_range = CV_HIST_HAS_RANGES(hist);
    int i, j;
    
    if( !have_range || CV_IS_UNIFORM_HIST(hist))
    {
        for( i = 0; i < dims; i++ )
        {
            double a = have_range ? hist->thresh[i][0] : 0;
            double b = have_range ? hist->thresh[i][1] : 256;
            int sz = size[i];
            double scale = sz/(b - a), cur = (lo - a)*scale;
            int step = 1;

            if( !is_sparse )
                step = ((CvMatND*)(hist->bins))->dim[i].step/sizeof(float);

            for( j = lo; j < hi; j++ )
            {
                int idx = cvFloor(cur);
                if( (unsigned)idx < (unsigned)sz )
                    idx *= step;
                else
                    idx = ICV_HIST_DUMMY_IDX;

                tab[i*(hi - lo) + j - lo] = idx;
                cur += scale;
            }
        }
    }
    else
    {
        for( i = 0; i < dims; i++ )
        {
            double limit = hist->thresh2[i][0];
            int idx = -1, write_idx = ICV_HIST_DUMMY_IDX, sz = size[i];
            int step = 1;

            if( !is_sparse )
                step = ((CvMatND*)(hist->bins))->dim[i].step/sizeof(float);

            if( limit > hi )
                limit = hi;
            
            j = lo;
            for(;;)
            {
                for( ; j < limit; j++ )
                    tab[i*(hi - lo) + j - lo] = write_idx;

                if( (unsigned)(++idx) < (unsigned)sz )
                {
                    limit = hist->thresh2[i][idx+1];
                    if( limit > hi )
                        limit = hi;
                    write_idx = idx*step;
                }
                else
                {
                    for( ; j < hi; j++ )
                        tab[i*(hi - lo) + j - lo] = ICV_HIST_DUMMY_IDX;
                    break;
                }
            }
        }
    }

    return CV_OK;
}


/***************************** C A L C   H I S T O G R A M *************************/

// Calculates histogram for one or more 8u arrays
static CvStatus CV_STDCALL
    icvCalcHist_8u_C1R( uchar** img, int step, uchar* mask, int maskStep,
                        CvSize size, CvHistogram* hist )
{
    int* tab;
    int is_sparse = CV_IS_SPARSE_HIST(hist);
    int dims, histsize[CV_MAX_DIM];
    int i, x;
    CvStatus status;

    dims = cvGetDims( hist->bins, histsize );

    tab = (int*)cvStackAlloc( dims*256*sizeof(int));
    status = icvCalcHistLookupTables8x( hist, 0, 256, dims,
                                        histsize, tab );

    if( status < 0 )
        return status;

    if( !is_sparse )
    {
        int total = 1;
        int* bins = ((CvMatND*)(hist->bins))->data.i;

        for( i = 0; i < dims; i++ )
            total *= histsize[i];

        if( dims <= 3 && total >= -ICV_HIST_DUMMY_IDX )
            return CV_BADSIZE_ERR; // too big histogram

        switch( dims )
        {
        case 1:
            {
            int tab1d[256];
            memset( tab1d, 0, sizeof(tab1d));

            for( ; size.height--; img[0] += step )
            {
                uchar* ptr = img[0];
                if( !mask )
                {
                    for( x = 0; x <= size.width - 4; x += 4 )
                    {
                        int v0 = ptr[x];
                        int v1 = ptr[x+1];

                        tab1d[v0]++;
                        tab1d[v1]++;

                        v0 = ptr[x+2];
                        v1 = ptr[x+3];

                        tab1d[v0]++;
                        tab1d[v1]++;
                    }

                    for( ; x < size.width; x++ )
                        tab1d[ptr[x]]++;
                }
                else
                {
                    for( x = 0; x < size.width; x++ )
                        if( mask[x] )
                            tab1d[ptr[x]]++;
                    mask += maskStep;
                }
            }

            for( i = 0; i < 256; i++ )
            {
                int idx = tab[i];
                if( idx >= 0 )
                    bins[idx] += tab1d[i];
            }
            }
            break;
        case 2:
            for( ; size.height--; img[0] += step, img[1] += step )
            {
                uchar* ptr0 = img[0];
                uchar* ptr1 = img[1];
                if( !mask )
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        int v0 = ptr0[x];
                        int v1 = ptr1[x];
                        int idx = tab[v0] + tab[256+v1];

                        if( idx >= 0 )
                            bins[idx]++;
                    }
                }
                else
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        if( mask[x] )
                        {
                            int v0 = ptr0[x];
                            int v1 = ptr1[x];

                            int idx = tab[v0] + tab[256+v1];

                            if( idx >= 0 )
                                bins[idx]++;
                        }
                    }
                    mask += maskStep;
                }
            }
            break;
        case 3:
            for( ; size.height--; img[0] += step, img[1] += step, img[2] += step )
            {
                uchar* ptr0 = img[0];
                uchar* ptr1 = img[1];
                uchar* ptr2 = img[2];
                if( !mask )
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        int v0 = ptr0[x];
                        int v1 = ptr1[x];
                        int v2 = ptr2[x];
                        int idx = tab[v0] + tab[256+v1] + tab[512+v2];

                        if( idx >= 0 )
                            bins[idx]++;
                    }
                }
                else
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        if( mask[x] )
                        {
                            int v0 = ptr0[x];
                            int v1 = ptr1[x];
                            int v2 = ptr2[x];
                            int idx = tab[v0] + tab[256+v1] + tab[512+v2];

                            if( idx >= 0 )
                                bins[idx]++;
                        }
                    }
                    mask += maskStep;
                }
            }
            break;
        default:
            for( ; size.height--; )
            {
                if( !mask )
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        int* binptr = bins;
                        for( i = 0; i < dims; i++ )
                        {
                            int idx = tab[i*256 + img[i][x]];
                            if( idx < 0 )
                                break;
                            binptr += idx;
                        }
                        if( i == dims )
                            binptr[0]++;
                    }
                }
                else
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        if( mask[x] )
                        {
                            int* binptr = bins;
                            for( i = 0; i < dims; i++ )
                            {
                                int idx = tab[i*256 + img[i][x]];
                                if( idx < 0 )
                                    break;
                                binptr += idx;
                            }
                            if( i == dims )
                                binptr[0]++;
                        }
                    }
                    mask += maskStep;
                }

                for( i = 0; i < dims; i++ )
                    img[i] += step;
            }
        }
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)(hist->bins);
        int node_idx[CV_MAX_DIM];

        for( ; size.height--; )
        {
            if( !mask )
            {
                for( x = 0; x < size.width; x++ )
                {
                    for( i = 0; i < dims; i++ )
                    {
                        int idx = tab[i*256 + img[i][x]];
                        if( idx < 0 )
                            break;
                        node_idx[i] = idx;
                    }
                    if( i == dims )
                    {
                        int* bin = (int*)cvPtrND( mat, node_idx, 0, 1 );
                        bin[0]++;
                    }
                }
            }
            else
            {
                for( x = 0; x < size.width; x++ )
                {
                    if( mask[x] )
                    {
                        for( i = 0; i < dims; i++ )
                        {
                            int idx = tab[i*256 + img[i][x]];
                            if( idx < 0 )
                                break;
                            node_idx[i] = idx;
                        }
                        if( i == dims )
                        {
                            int* bin = (int*)cvPtrND( mat, node_idx, 0, 1, 0 );
                            bin[0]++;
                        }
                    }
                }
                mask += maskStep;
            }

            for( i = 0; i < dims; i++ )
                img[i] += step;
        }
    }

    return CV_OK;
}


// Calculates histogram for one or more 32f arrays
static CvStatus CV_STDCALL
    icvCalcHist_32f_C1R( float** img, int step, uchar* mask, int maskStep,
                         CvSize size, CvHistogram* hist )
{
    int is_sparse = CV_IS_SPARSE_HIST(hist);
    int uniform = CV_IS_UNIFORM_HIST(hist);
    int dims, histsize[CV_MAX_DIM];
    float uni_range[CV_MAX_DIM][2];
    int i, x;

    dims = cvGetDims( hist->bins, histsize );
    step /= sizeof(img[0][0]);

    if( uniform )
    {
        for( i = 0; i < dims; i++ )
        {
            float t = ((float)histsize[i])/(hist->thresh[i][1] - hist->thresh[i][0]);
            uni_range[i][0] = t;
            uni_range[i][1] = (float)(-t*hist->thresh[i][0]);
        }
    }

    if( !is_sparse )
    {
        CvMatND* mat = (CvMatND*)(hist->bins);
        int* bins = mat->data.i;

        if( uniform )
        {
            switch( dims )
            {
            case 1:
                {
                float a = uni_range[0][0], b = uni_range[0][1];
                int sz = histsize[0];

                for( ; size.height--; img[0] += step )
                {
                    float* ptr = img[0];

                    if( !mask )
                    {
                        for( x = 0; x <= size.width - 4; x += 4 )
                        {
                            int v0 = cvFloor(ptr[x]*a + b);
                            int v1 = cvFloor(ptr[x+1]*a + b);

                            if( (unsigned)v0 < (unsigned)sz )
                                bins[v0]++;
                            if( (unsigned)v1 < (unsigned)sz )
                                bins[v1]++;

                            v0 = cvFloor(ptr[x+2]*a + b);
                            v1 = cvFloor(ptr[x+3]*a + b);

                            if( (unsigned)v0 < (unsigned)sz )
                                bins[v0]++;
                            if( (unsigned)v1 < (unsigned)sz )
                                bins[v1]++;
                        }

                        for( ; x < size.width; x++ )
                        {
                            int v0 = cvFloor(ptr[x]*a + b);
                            if( (unsigned)v0 < (unsigned)sz )
                                bins[v0]++;
                        }
                    }
                    else
                    {
                        for( x = 0; x < size.width; x++ )
                            if( mask[x] )
                            {
                                int v0 = cvFloor(ptr[x]*a + b);
                                if( (unsigned)v0 < (unsigned)sz )
                                    bins[v0]++;
                            }
                        mask += maskStep;
                    }
                }
                }
                break;
            case 2:
                {
                float  a0 = uni_range[0][0], b0 = uni_range[0][1];
                float  a1 = uni_range[1][0], b1 = uni_range[1][1];
                int sz0 = histsize[0], sz1 = histsize[1];
                int step0 = ((CvMatND*)(hist->bins))->dim[0].step/sizeof(float);

                for( ; size.height--; img[0] += step, img[1] += step )
                {
                    float* ptr0 = img[0];
                    float* ptr1 = img[1];

                    if( !mask )
                    {
                        for( x = 0; x < size.width; x++ )
                        {
                            int v0 = cvFloor( ptr0[x]*a0 + b0 );
                            int v1 = cvFloor( ptr1[x]*a1 + b1 );

                            if( (unsigned)v0 < (unsigned)sz0 &&
                                (unsigned)v1 < (unsigned)sz1 )
                                bins[v0*step0 + v1]++;
                        }
                    }
                    else
                    {
                        for( x = 0; x < size.width; x++ )
                        {
                            if( mask[x] )
                            {
                                int v0 = cvFloor( ptr0[x]*a0 + b0 );
                                int v1 = cvFloor( ptr1[x]*a1 + b1 );

                                if( (unsigned)v0 < (unsigned)sz0 &&
                                    (unsigned)v1 < (unsigned)sz1 )
                                    bins[v0*step0 + v1]++;
                            }
                        }
                        mask += maskStep;
                    }
                }
                }
                break;
            default:
                for( ; size.height--; )
                {
                    if( !mask )
                    {
                        for( x = 0; x < size.width; x++ )
                        {
                            int* binptr = bins;
                            for( i = 0; i < dims; i++ )
                            {
                                int idx = cvFloor(img[i][x]*uni_range[i][0]
                                                 + uni_range[i][1]);
                                if( (unsigned)idx >= (unsigned)histsize[i] )
                                    break;
                                binptr += idx*(mat->dim[i].step/sizeof(float));
                            }
                            if( i == dims )
                                binptr[0]++;
                        }
                    }
                    else
                    {
                        for( x = 0; x < size.width; x++ )
                        {
                            if( mask[x] )
                            {
                                int* binptr = bins;
                                for( i = 0; i < dims; i++ )
                                {
                                    int idx = cvFloor(img[i][x]*uni_range[i][0]
                                                     + uni_range[i][1]);
                                    if( (unsigned)idx >= (unsigned)histsize[i] )
                                        break;
                                    binptr += idx*(mat->dim[i].step/sizeof(float));
                                }
                                if( i == dims )
                                    binptr[0]++;
                            }
                        }
                        mask += maskStep;
                    }

                    for( i = 0; i < dims; i++ )
                        img[i] += step;
                }
            }
        }
        else
        {
            for( ; size.height--; )
            {
                for( x = 0; x < size.width; x++ )
                {
                    if( !mask || mask[x] )
                    {
                        int* binptr = bins;
                        for( i = 0; i < dims; i++ )
                        {
                            float v = img[i][x];
                            float* thresh = hist->thresh2[i];
                            int idx = -1, sz = histsize[i];

                            while( v >= thresh[idx+1] && ++idx < sz )
                                /* nop */;

                            if( (unsigned)idx >= (unsigned)sz )
                                break;

                            binptr += idx*(mat->dim[i].step/sizeof(float));
                        }
                        if( i == dims )
                            binptr[0]++;
                    }
                }

                for( i = 0; i < dims; i++ )
                    img[i] += step;
                if( mask )
                    mask += maskStep;
            }
        }
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)(hist->bins);
        int node_idx[CV_MAX_DIM];

        for( ; size.height--; )
        {
            if( uniform )
            {
                for( x = 0; x < size.width; x++ )
                {
                    if( !mask || mask[x] )
                    {
                        for( i = 0; i < dims; i++ )
                        {
                            int idx = cvFloor(img[i][x]*uni_range[i][0]
                                             + uni_range[i][1]);
                            if( (unsigned)idx >= (unsigned)histsize[i] )
                                break;
                            node_idx[i] = idx;
                        }
                        if( i == dims )
                        {
                            int* bin = (int*)cvPtrND( mat, node_idx, 0, 1, 0 );
                            bin[0]++;
                        }
                    }
                }
            }
            else
            {
                for( x = 0; x < size.width; x++ )
                {
                    if( !mask || mask[x] )
                    {
                        for( i = 0; i < dims; i++ )
                        {
                            float v = img[i][x];
                            float* thresh = hist->thresh2[i];
                            int idx = -1, sz = histsize[i];

                            while( v >= thresh[idx+1] && ++idx < sz )
                                /* nop */;

                            if( (unsigned)idx >= (unsigned)sz )
                                break;

                            node_idx[i] = idx;
                        }
                        if( i == dims )
                        {
                            int* bin = (int*)cvPtrND( mat, node_idx, 0, 1, 0 );
                            bin[0]++;
                        }
                    }
                }
            }

            for( i = 0; i < dims; i++ )
                img[i] += step;

            if( mask )
                mask += maskStep;
        }
    }

    return CV_OK;
}


CV_IMPL void
cvCalcArrHist( CvArr** img, CvHistogram* hist,
               int do_not_clear, const CvArr* mask )
{
    CV_FUNCNAME( "cvCalcHist" );

    __BEGIN__;

    uchar* ptr[CV_MAX_DIM];
    uchar* maskptr = 0;
    int maskstep = 0, step = 0;
    int i, dims;
    int cont_flag = -1;
    CvMat stub0, *mat0 = 0;
    CvMatND dense;
    CvSize size;

    if( !CV_IS_HIST(hist))
        CV_ERROR( CV_StsBadArg, "Bad histogram pointer" );

    if( !img )
        CV_ERROR( CV_StsNullPtr, "Null double array pointer" );

    CV_CALL( dims = cvGetDims( hist->bins ));
    
    for( i = 0; i < dims; i++ )
    {
        CvMat stub, *mat = (CvMat*)img[i];
        CV_CALL( mat = cvGetMat( mat, i == 0 ? &stub0 : &stub, 0, 1 ));

        if( CV_MAT_CN( mat->type ) != 1 )
            CV_ERROR( CV_BadNumChannels, "Only 1-channel arrays are allowed here" );

        if( i == 0 )
        {
            mat0 = mat;
            step = mat0->step;
        }
        else
        {
            if( !CV_ARE_SIZES_EQ( mat0, mat ))
                CV_ERROR( CV_StsUnmatchedSizes, "Not all the planes have equal sizes" );

            if( mat0->step != mat->step )
                CV_ERROR( CV_StsUnmatchedSizes, "Not all the planes have equal steps" );

            if( !CV_ARE_TYPES_EQ( mat0, mat ))
                CV_ERROR( CV_StsUnmatchedFormats, "Not all the planes have equal types" );
        }

        cont_flag &= mat->type;
        ptr[i] = mat->data.ptr;
    }

    if( mask )
    {
        CvMat stub, *mat = (CvMat*)mask;
        CV_CALL( mat = cvGetMat( mat, &stub, 0, 1 ));

        if( !CV_IS_MASK_ARR(mat))
            CV_ERROR( CV_StsBadMask, "Bad mask array" );

        if( !CV_ARE_SIZES_EQ( mat0, mat ))
            CV_ERROR( CV_StsUnmatchedSizes,
                "Mask size does not match to other arrays\' size" );
        maskptr = mat->data.ptr;
        maskstep = mat->step;
        cont_flag &= mat->type;
    }

    size = cvGetMatSize(mat0);
    if( CV_IS_MAT_CONT( cont_flag ))
    {
        size.width *= size.height;
        size.height = 1;
        maskstep = step = CV_AUTOSTEP;
    }

    if( !CV_IS_SPARSE_HIST(hist))
    {
        dense = *(CvMatND*)hist->bins;
        dense.type = (dense.type & ~CV_MAT_TYPE_MASK) | CV_32SC1;
    }

    if( !do_not_clear )
    {
        CV_CALL( cvZero( hist->bins ));
    }
    else if( !CV_IS_SPARSE_HIST(hist))
    {
        CV_CALL( cvConvert( (CvMatND*)hist->bins, &dense ));
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)(hist->bins);
        CvSparseMatIterator iterator;
        CvSparseNode* node;

        for( node = cvInitSparseMatIterator( mat, &iterator );
             node != 0; node = cvGetNextSparseNode( &iterator ))
        {
            int& val = *(int*)CV_NODE_VAL( mat, node );
            val = cvRound( (float&)val );
        }
    }

    if( CV_MAT_DEPTH(mat0->type) > CV_8S && !CV_HIST_HAS_RANGES(hist))
        CV_ERROR( CV_StsBadArg, "histogram ranges must be set (via cvSetHistBinRanges) "
                                "before calling the function" );

    switch( CV_MAT_DEPTH(mat0->type) )
    {
    case CV_8U:
        IPPI_CALL( icvCalcHist_8u_C1R( ptr, step, maskptr, maskstep, size, hist ));
	    break;
    case CV_32F:
	    IPPI_CALL( icvCalcHist_32f_C1R( (float**)ptr, step, maskptr, maskstep, size, hist ));
	    break;
    default:
        CV_ERROR( CV_StsUnsupportedFormat, "Unsupported array type" );
    }

    if( !CV_IS_SPARSE_HIST(hist))
    {
        CV_CALL( cvConvert( &dense, (CvMatND*)hist->bins ));
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)(hist->bins);
        CvSparseMatIterator iterator;
        CvSparseNode* node;

        for( node = cvInitSparseMatIterator( mat, &iterator );
             node != 0; node = cvGetNextSparseNode( &iterator ))
        {
            int& val = *(int*)CV_NODE_VAL( mat, node );
            (float&)val = (float)val;
        }
    }
    
    __END__;
}


/***************************** B A C K   P R O J E C T *****************************/

// Calculates back project for one or more 8u arrays
static CvStatus CV_STDCALL
    icvCalcBackProject_8u_C1R( uchar** img, int step, uchar* dst, int dstStep,
                               CvSize size, const CvHistogram* hist )
{
    const int small_hist_size = 1<<12;
    int* tab = 0;
    int is_sparse = CV_IS_SPARSE_HIST(hist);
    int dims, histsize[CV_MAX_DIM];
    int i, x;
    CvStatus status;

    dims = cvGetDims( hist->bins, histsize );

    tab = (int*)cvStackAlloc( dims*256*sizeof(int));
    status = icvCalcHistLookupTables8x( hist, 0, 256, dims, histsize, tab );
    if( status < 0 )
        return status;

    if( !is_sparse )
    {
        int total = 1;
        CvMatND* mat = (CvMatND*)(hist->bins);
        float* bins = mat->data.fl;
        uchar* buffer = 0;

        for( i = 0; i < dims; i++ )
            total *= histsize[i];

        if( dims <= 3 && total >= -ICV_HIST_DUMMY_IDX )
            return CV_BADSIZE_ERR; // too big histogram

        if( dims > 1 && total <= small_hist_size && CV_IS_MAT_CONT(mat->type))
        {
            buffer = (uchar*)cvAlloc(total);
            if( !buffer )
                return CV_OUTOFMEM_ERR;
            for( i = 0; i < total; i++ )
            {
                int v = cvRound(bins[i]);
                buffer[i] = CV_CAST_8U(v);
            }
        }

        switch( dims )
        {
        case 1:
            {
            uchar tab1d[256];
            for( i = 0; i < 256; i++ )
            {
                int idx = tab[i];
                if( idx >= 0 )
                {
                    int v = cvRound(bins[idx]);
                    tab1d[i] = CV_CAST_8U(v);
                }
                else
                    tab1d[i] = 0;
            }

            for( ; size.height--; img[0] += step, dst += dstStep )
            {
                uchar* ptr = img[0];
                for( x = 0; x <= size.width - 4; x += 4 )
                {
                    uchar v0 = tab1d[ptr[x]];
                    uchar v1 = tab1d[ptr[x+1]];

                    dst[x] = v0;
                    dst[x+1] = v1;

                    v0 = tab1d[ptr[x+2]];
                    v1 = tab1d[ptr[x+3]];

                    dst[x+2] = v0;
                    dst[x+3] = v1;
                }

                for( ; x < size.width; x++ )
                    dst[x] = tab1d[ptr[x]];
            }
            }
            break;
        case 2:
            for( ; size.height--; img[0] += step, img[1] += step, dst += dstStep )
            {
                uchar* ptr0 = img[0];
                uchar* ptr1 = img[1];

                if( buffer )
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        int v0 = ptr0[x];
                        int v1 = ptr1[x];
                        int idx = tab[v0] + tab[256+v1];
                        int v = 0;

                        if( idx >= 0 )
                            v = buffer[idx];

                        dst[x] = (uchar)v;
                    }
                }
                else
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        int v0 = ptr0[x];
                        int v1 = ptr1[x];
                        int idx = tab[v0] + tab[256+v1];
                        int v = 0;

                        if( idx >= 0 )
                        {
                            v = cvRound(bins[idx]);
                            v = CV_CAST_8U(v);
                        }

                        dst[x] = (uchar)v;
                    }
                }
            }
            break;
        case 3:
            for( ; size.height--; img[0] += step, img[1] += step,
                                  img[2] += step, dst += dstStep )
            {
                uchar* ptr0 = img[0];
                uchar* ptr1 = img[1];
                uchar* ptr2 = img[2];

                if( buffer )
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        int v0 = ptr0[x];
                        int v1 = ptr1[x];
                        int v2 = ptr2[x];
                        int idx = tab[v0] + tab[256+v1] + tab[512+v2];
                        int v = 0;

                        if( idx >= 0 )
                            v = buffer[idx];

                        dst[x] = (uchar)v;
                    }
                }
                else
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        int v0 = ptr0[x];
                        int v1 = ptr1[x];
                        int v2 = ptr2[x];
                        int idx = tab[v0] + tab[256+v1] + tab[512+v2];
                        int v = 0;

                        if( idx >= 0 )
                        {
                            v = cvRound(bins[idx]);
                            v = CV_CAST_8U(v);
                        }
                        dst[x] = (uchar)v;
                    }
                }
            }
            break;
        default:
            for( ; size.height--; dst += dstStep )
            {
                if( buffer )
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        uchar* binptr = buffer;
                        int v = 0;

                        for( i = 0; i < dims; i++ )
                        {
                            int idx = tab[i*256 + img[i][x]];
                            if( idx < 0 )
                                break;
                            binptr += idx;
                        }
                        
                        if( i == dims )
                            v = binptr[0];

                        dst[x] = (uchar)v;
                    }
                }
                else
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        float* binptr = bins;
                        int v = 0;

                        for( i = 0; i < dims; i++ )
                        {
                            int idx = tab[i*256 + img[i][x]];
                            if( idx < 0 )
                                break;
                            binptr += idx;
                        }

                        if( i == dims )
                        {
                            v = cvRound( binptr[0] );
                            v = CV_CAST_8U(v);
                        }

                        dst[x] = (uchar)v;
                    }
                }

                for( i = 0; i < dims; i++ )
                    img[i] += step;
            }
        }

        cvFree( (void**)&buffer );
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)(hist->bins);
        int node_idx[CV_MAX_DIM];

        for( ; size.height--; dst += dstStep )
        {
            for( x = 0; x < size.width; x++ )
            {
                int v = 0;

                for( i = 0; i < dims; i++ )
                {
                    int idx = tab[i*256 + img[i][x]];
                    if( idx < 0 )
                        break;
                    node_idx[i] = idx;
                }
                if( i == dims )
                {
                    float* bin = (float*)cvPtrND( mat, node_idx, 0, 1, 0 );
                    v = cvRound(bin[0]);
                    v = CV_CAST_8U(v);
                }

                dst[x] = (uchar)v;
            }

            for( i = 0; i < dims; i++ )
                img[i] += step;
        }
    }

    return CV_OK;
}


// Calculates back project for one or more 32f arrays
static CvStatus CV_STDCALL
    icvCalcBackProject_32f_C1R( float** img, int step, float* dst, int dstStep,
                                CvSize size, const CvHistogram* hist )
{
    int is_sparse = CV_IS_SPARSE_HIST(hist);
    int uniform = CV_IS_UNIFORM_HIST(hist);
    int dims, histsize[CV_MAX_DIM];
    float uni_range[CV_MAX_DIM][2];
    int i, x;

    dims = cvGetDims( hist->bins, histsize );
    step /= sizeof(img[0][0]);
    dstStep /= sizeof(dst[0]);

    if( uniform )
    {
        for( i = 0; i < dims; i++ )
        {
            float t = ((float)histsize[i])/(hist->thresh[i][1] - hist->thresh[i][0]);
            uni_range[i][0] = t;
            uni_range[i][1] = (float)(-t*hist->thresh[i][0]);
        }
    }

    if( !is_sparse )
    {
        CvMatND* mat = (CvMatND*)(hist->bins);
        float* bins = mat->data.fl;

        if( uniform )
        {
            switch( dims )
            {
            case 1:
                {
                float a = uni_range[0][0], b = uni_range[0][1];
                int sz = histsize[0];

                for( ; size.height--; img[0] += step, dst += dstStep )
                {
                    float* ptr = img[0];

                    for( x = 0; x <= size.width - 4; x += 4 )
                    {
                        int v0 = cvFloor(ptr[x]*a + b);
                        int v1 = cvFloor(ptr[x+1]*a + b);

                        if( (unsigned)v0 < (unsigned)sz )
                            dst[x] = bins[v0];
                        else
                            dst[x] = 0;

                        if( (unsigned)v1 < (unsigned)sz )
                            dst[x+1] = bins[v1];
                        else
                            dst[x+1] = 0;

                        v0 = cvFloor(ptr[x+2]*a + b);
                        v1 = cvFloor(ptr[x+3]*a + b);

                        if( (unsigned)v0 < (unsigned)sz )
                            dst[x+2] = bins[v0];
                        else
                            dst[x+2] = 0;

                        if( (unsigned)v1 < (unsigned)sz )
                            dst[x+3] = bins[v1];
                        else
                            dst[x+3] = 0;
                    }

                    for( ; x < size.width; x++ )
                    {
                        int v0 = cvFloor(ptr[x]*a + b);
                        if( (unsigned)v0 < (unsigned)sz )
                            dst[x] = bins[v0];
                        else
                            dst[x] = 0;
                    }
                }
                }
                break;
            case 2:
                {
                float  a0 = uni_range[0][0], b0 = uni_range[0][1];
                float  a1 = uni_range[1][0], b1 = uni_range[1][1];
                int sz0 = histsize[0], sz1 = histsize[1];
                int step0 = ((CvMatND*)(hist->bins))->dim[0].step/sizeof(float);

                for( ; size.height--; img[0] += step, img[1] += step, dst += dstStep )
                {
                    float* ptr0 = img[0];
                    float* ptr1 = img[1];

                    for( x = 0; x < size.width; x++ )
                    {
                        int v0 = cvFloor( ptr0[x]*a0 + b0 );
                        int v1 = cvFloor( ptr1[x]*a1 + b1 );

                        if( (unsigned)v0 < (unsigned)sz0 &&
                            (unsigned)v1 < (unsigned)sz1 )
                            dst[x] = bins[v0*step0 + v1];
                        else
                            dst[x] = 0;
                    }
                }
                }
                break;
            default:
                for( ; size.height--; dst += dstStep )
                {
                    for( x = 0; x < size.width; x++ )
                    {
                        float* binptr = bins;

                        for( i = 0; i < dims; i++ )
                        {
                            int idx = cvFloor(img[i][x]*uni_range[i][0]
                                             + uni_range[i][1]);
                            if( (unsigned)idx >= (unsigned)histsize[i] )
                                break;
                            binptr += idx*(mat->dim[i].step/sizeof(float));
                        }
                        if( i == dims )
                            dst[x] = binptr[0];
                        else
                            dst[x] = 0;
                    }
                }

                for( i = 0; i < dims; i++ )
                    img[i] += step;
            }
        }
        else
        {
            for( ; size.height--; dst += dstStep )
            {
                for( x = 0; x < size.width; x++ )
                {
                    float* binptr = bins;
                    for( i = 0; i < dims; i++ )
                    {
                        float v = img[i][x];
                        float* thresh = hist->thresh2[i];
                        int idx = -1, sz = histsize[i];

                        while( v >= thresh[idx+1] && ++idx < sz )
                            /* nop */;

                        if( (unsigned)idx >= (unsigned)sz )
                            break;

                        binptr += idx*(mat->dim[i].step/sizeof(float));
                    }
                    if( i == dims )
                        dst[x] = binptr[0];
                    else
                        dst[x] = 0;
                }

                for( i = 0; i < dims; i++ )
                    img[i] += step;
            }
        }
    }
    else
    {
        CvSparseMat* mat = (CvSparseMat*)(hist->bins);
        int node_idx[CV_MAX_DIM];

        for( ; size.height--; dst += dstStep )
        {
            if( uniform )
            {
                for( x = 0; x < size.width; x++ )
                {
                    for( i = 0; i < dims; i++ )
                    {
                        int idx = cvFloor(img[i][x]*uni_range[i][0]
                                         + uni_range[i][1]);
                        if( (unsigned)idx >= (unsigned)histsize[i] )
                            break;
                        node_idx[i] = idx;
                    }
                    if( i == dims )
                    {
                        float* bin = (float*)cvPtrND( mat, node_idx, 0, 1, 0 );
                        dst[x] = bin[0];
                    }
                    else
                        dst[x] = 0;
                }
            }
            else
            {
                for( x = 0; x < size.width; x++ )
                {
                    for( i = 0; i < dims; i++ )
                    {
                        float v = img[i][x];
                        float* thresh = hist->thresh2[i];
                        int idx = -1, sz = histsize[i];

                        while( v >= thresh[idx+1] && ++idx < sz )
                            /* nop */;

                        if( (unsigned)idx >= (unsigned)sz )
                            break;

                        node_idx[i] = idx;
                    }
                    if( i == dims )
                    {
                        float* bin = (float*)cvPtrND( mat, node_idx, 0, 1, 0 );
                        dst[x] = bin[0];
                    }
                    else
                        dst[x] = 0;
                }
            }

            for( i = 0; i < dims; i++ )
                img[i] += step;
        }
    }

    return CV_OK;
}


CV_IMPL void
cvCalcArrBackProject( CvArr** img, CvArr* dst, const CvHistogram* hist )
{
    CV_FUNCNAME( "cvCalcArrBackProject" );

    __BEGIN__;

    uchar* ptr[CV_MAX_DIM];
    uchar* dstptr = 0;
    int dststep = 0, step = 0;
    int i, dims;
    int cont_flag = -1;
    CvMat stub0, *mat0 = 0;
    CvSize size;

    if( !CV_IS_HIST(hist))
        CV_ERROR( CV_StsBadArg, "Bad histogram pointer" );

    if( !img )
        CV_ERROR( CV_StsNullPtr, "Null double array pointer" );

    CV_CALL( dims = cvGetDims( hist->bins ));
    
    for( i = 0; i <= dims; i++ )
    {
        CvMat stub, *mat = (CvMat*)(i < dims ? img[i] : dst);
        CV_CALL( mat = cvGetMat( mat, i == 0 ? &stub0 : &stub, 0, 1 ));

        if( CV_MAT_CN( mat->type ) != 1 )
            CV_ERROR( CV_BadNumChannels, "Only 1-channel arrays are allowed here" );

        if( i == 0 )
        {
            mat0 = mat;
            step = mat0->step;
        }
        else
        {
            if( !CV_ARE_SIZES_EQ( mat0, mat ))
                CV_ERROR( CV_StsUnmatchedSizes, "Not all the planes have equal sizes" );

            if( mat0->step != mat->step )
                CV_ERROR( CV_StsUnmatchedSizes, "Not all the planes have equal steps" );

            if( !CV_ARE_TYPES_EQ( mat0, mat ))
                CV_ERROR( CV_StsUnmatchedFormats, "Not all the planes have equal types" );
        }

        cont_flag &= mat->type;
        if( i < dims )
            ptr[i] = mat->data.ptr;
        else
        {
            dstptr = mat->data.ptr;
            dststep = mat->step;
        }
    }

    size = cvGetMatSize(mat0);
    if( CV_IS_MAT_CONT( cont_flag ))
    {
        size.width *= size.height;
        size.height = 1;
        dststep = step = CV_AUTOSTEP;
    }

    if( CV_MAT_DEPTH(mat0->type) > CV_8S && !CV_HIST_HAS_RANGES(hist))
        CV_ERROR( CV_StsBadArg, "histogram ranges must be set (via cvSetHistBinRanges) "
                                "before calling the function" );

    switch( CV_MAT_DEPTH(mat0->type) )
    {
    case CV_8U:
        IPPI_CALL( icvCalcBackProject_8u_C1R( ptr, step, dstptr, dststep, size, hist ));
	    break;
    case CV_32F:
	    IPPI_CALL( icvCalcBackProject_32f_C1R( (float**)ptr, step,
                                (float*)dstptr, dststep, size, hist ));
	    break;
    default:
        CV_ERROR( CV_StsUnsupportedFormat, "Unsupported array type" );
    }

    __END__;
}


////////////////////// B A C K   P R O J E C T   P A T C H /////////////////////////

CV_IMPL void
cvCalcArrBackProjectPatch( CvArr** arr, CvArr* dst, CvSize range, CvHistogram* hist,
                           int method, double norm_factor )
{
    CvHistogram* model = 0;
    
    CV_FUNCNAME( "cvCalcArrBackProjectPatch" );

    __BEGIN__;

    IplImage imgstub[CV_MAX_DIM], *img[CV_MAX_DIM];
    IplROI roi;
    CvMat dststub, *dstmat;
    int i, dims;
    int x, y;
    CvSize size;

    if( !CV_IS_HIST(hist))
        CV_ERROR( CV_StsBadArg, "Bad histogram pointer" );

    if( !img )
        CV_ERROR( CV_StsNullPtr, "Null double array pointer" );

    if( norm_factor <= 0 )
        CV_ERROR( CV_StsOutOfRange,
                  "Bad normalization factor (set it to 1.0 if unsure)" );

    CV_CALL( dims = cvGetDims( hist->bins ));
    CV_CALL( cvCopyHist( hist, &model ));
    CV_CALL( cvNormalizeHist( hist, norm_factor ));

    for( i = 0; i < dims; i++ )
    {
        CvMat stub, *mat;
        CV_CALL( mat = cvGetMat( arr[i], &stub, 0, 0 ));
        CV_CALL( img[i] = cvGetImage( mat, &imgstub[i] ));
        img[i]->roi = &roi;
    }

    CV_CALL( dstmat = cvGetMat( dst, &dststub, 0, 0 ));
    if( CV_MAT_TYPE( dstmat->type ) != CV_32FC1 )
        CV_ERROR( CV_StsUnsupportedFormat, "Resultant image must have 32fC1 type" );

    size = cvGetMatSize(dstmat);
    roi.coi = 0;

    for( y = 0; y < size.height; y++ )
    {
        roi.yOffset = y - range.height;
        roi.height = range.height*2 + 1;
        if( roi.yOffset < 0 )
        {
            roi.height += roi.yOffset;
            roi.yOffset = 0;
        }
        if( roi.yOffset + roi.height > size.height )
            roi.height = size.height - roi.yOffset;
        
        for( x = 0; x < size.width; x++ )
        {
            double result;
            
            roi.xOffset = x - range.width;
            roi.width = range.width*2 + 1;
            if( roi.xOffset < 0 )
            {
                roi.width += roi.xOffset;
                roi.xOffset = 0;
            }
            if( roi.xOffset + roi.width > size.width )
                roi.width = size.width - roi.xOffset;

            CV_CALL( cvCalcHist( img, model ));
            CV_CALL( cvNormalizeHist( model, norm_factor ));
            CV_CALL( result = cvCompareHist( model, hist, method ));
            CV_MAT_ELEM( *dstmat, float, y, x ) = (float)result;
        }
    }

    __END__;

    cvReleaseHist( &model );
}


// Calculates Bayes probabilistic histograms
CV_IMPL void
cvCalcBayesianProb( CvHistogram** src, int count, CvHistogram** dst )
{
    CV_FUNCNAME( "cvCalcBayesianProb" );
    
    __BEGIN__;

    int i;
    
    if( !src || !dst )
        CV_ERROR( CV_StsNullPtr, "NULL histogram array pointer" );

    if( count < 2 )
        CV_ERROR( CV_StsOutOfRange, "Too small number of histograms" );
    
    for( i = 0; i < count; i++ )
    {
        if( !CV_IS_HIST(src[i]) || !CV_IS_HIST(dst[i]) )
            CV_ERROR( CV_StsBadArg, "Invalid histogram header" );

        if( !CV_IS_MATND(src[i]->bins) || !CV_IS_MATND(dst[i]->bins) )
            CV_ERROR( CV_StsBadArg, "The function supports dense histograms only" );
    }
    
    cvZero( dst[0]->bins );
    // dst[0] = src[0] + ... + src[count-1]
    for( i = 0; i < count; i++ )
        CV_CALL( cvAdd( src[i]->bins, dst[0]->bins, dst[0]->bins ));

    CV_CALL( cvDiv( 0, dst[0]->bins, dst[0]->bins ));

    // dst[i] = src[i]*(1/dst[0])
    for( i = count - 1; i >= 0; i-- )
        CV_CALL( cvMul( src[i]->bins, dst[0]->bins, dst[i]->bins ));
    
    __END__;
}


CV_IMPL void
cvCalcProbDensity( const CvHistogram* hist, const CvHistogram* hist_mask,
                   CvHistogram* hist_dens, double scale )
{
    CV_FUNCNAME( "cvCalcProbDensity" );

    __BEGIN__;

    if( scale <= 0 )
        CV_ERROR( CV_StsOutOfRange, "scale must be positive" );

    if( !CV_IS_HIST(hist) || !CV_IS_HIST(hist_mask) || !CV_IS_HIST(hist_dens) )
        CV_ERROR( CV_StsBadArg, "Invalid histogram pointer[s]" );

    {
        CvArr* arrs[] = { hist->bins, hist_mask->bins, hist_dens->bins };
        CvMatND stubs[3];
        CvNArrayIterator iterator;

        CV_CALL( cvInitNArrayIterator( 3, arrs, 0, stubs, &iterator ));

        if( CV_MAT_TYPE(iterator.hdr[0]->type) != CV_32FC1 )
            CV_ERROR( CV_StsUnsupportedFormat, "All histograms must have 32fC1 type" );

        do
        {
            const float* srcdata = (const float*)(iterator.ptr[0]);
            const float* maskdata = (const float*)(iterator.ptr[1]);
            float* dstdata = (float*)(iterator.ptr[2]);
            int i;

            for( i = 0; i < iterator.size.width; i++ )
            {
                float s = srcdata[i];
                float m = maskdata[i];
                if( s > FLT_EPSILON )
                    if( m <= s )
                        dstdata[i] = (float)(m*scale/s);
                    else
                        dstdata[i] = (float)scale;
                else
                    dstdata[i] = (float)0;
            }
        }
        while( cvNextNArraySlice( &iterator ));
    }

    __END__;
}

/* End of file. */

