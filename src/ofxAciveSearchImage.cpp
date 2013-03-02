//
//  ofxAciveSearchImage.cpp
//  ofxcvTest002
//
//  Created by MORIOKA TOYOSHI on 2013/02/27.
//
//

#include "ofxAciveSearchImage.h"

ofxActiveSearchImage::ofxActiveSearchImage(){
    
}
ofxActiveSearchImage::~ofxActiveSearchImage(){
    
}

double ofxActiveSearchImage::_minimum( const double v0, const double v1 )
{
	return ( v0 < v1 ) ? v0 : v1;
}

double ofxActiveSearchImage::_upper_bound( const size_t a_and_b, const size_t window_size, const double &similarity )
{
	return ( _minimum( similarity * window_size, a_and_b ) + window_size - a_and_b ) / window_size;
}

void ofxActiveSearchImage::_culling_out( const size_t pi, const size_t pj, const size_t w_width, const size_t w_height,
                  const double &sim, const double &max_sim,
                  vector< bool >& culling_table )
{
	const size_t w_size = w_width * w_height;
	for( size_t j = 0; j < w_height ; j ++ )
	{
		if( _upper_bound( w_width * ( w_height - j ), w_size, sim ) >= max_sim )
		{
			break;
		}
		for( size_t i = 0 ; i < w_width ; i ++ )
		{
			if( _upper_bound( ( w_width - i ) * ( w_height - j ), w_size, sim ) < max_sim )
			{
				culling_table[ (pi + i) + inputWidth * (pj + j) ] = true;
				if( i < pi )
				{
					culling_table[ (pi - i) + inputWidth * ( pj + j )] = true;
				}
			}
			else
			{
				break;
			}
		}
	}
}

void ofxActiveSearchImage::tone_down( ofImage &in,
               ofImage &out,
               const unsigned char base )
{
    out.allocate(in.width, in.height, OF_IMAGE_COLOR);
    
    int totalSize = out.width * out.height * 3;
    unsigned char *pIn = in.getPixels();
    unsigned char *pOut = out.getPixels();
	for( size_t i = 0 ; i < totalSize; i ++ )
	{
		pOut[ i ]  = pIn[ i ] / base;
		pOut[ i + 1]  = pIn[ i + 1] / base;
		pOut[ i + 2]  = pIn[ i + 2] / base;
	}
}

void ofxActiveSearchImage::mk_histogram( ofImage &img,
                                        const size_t pi, const size_t pj,
                                        const size_t w_width, const size_t w_height,
                                        CvHistogram* hist )
{
	Mat tempImg = toCv(img);
    IplImage tempSrc = tempImg;
    IplImage *src1 = &tempSrc;
    IplImage *dst1[4] = {0, 0, 0, 0};
	
	int sch1 = 0;
    
	sch1 = src1->nChannels;
    
    if (img.width != w_width || img.height != w_height) {
        cvSetImageROI(src1, cvRect(pi, pj, w_width, w_height));
    }
    
    
    for(int i = 0; i < sch1; i++){
        dst1[i] = cvCreateImage (cvSize (w_width, w_height), src1->depth, 1);
    }
    
	//入力画像がマルチチャンネルの場合，画像をチャンネル毎に分割
    
	if (sch1 == 1) {
		cvCopy (src1, dst1[0], NULL);
	}else{
		cvSplit (src1, dst1[0], dst1[1], dst1[2], dst1[3]);
	}
	//ヒストグラムの計算
	for(int i = 0; i < sch1; i++){
		cvCalcHist (&dst1[i], hist, 0, NULL);
		cvNormalizeHist(hist, 1.0);
	}
    
    for(int i = 0; i < sch1; i++){
        cvReleaseImage(&dst1[i]);
    }
    
    
    if (img.width != w_width || img.height != w_height) {
        cvResetImageROI(src1);
    }
    
    return;
}

double ofxActiveSearchImage::histogram_intersection( CvHistogram* histogram_a,
                                                    CvHistogram* histogram_b)
{
	double intersection = 0.0;
    intersection = cvCompareHist( histogram_a, histogram_b, CV_COMP_INTERSECT);
	return intersection;
}

void ofxActiveSearchImage::active_search( ofImage &input,
                   ofImage &reference,
                   int &ret_x, int &ret_y,
                   double &ret_scale )
{
    
    ofImage degraded_input = input;
    ofImage degraded_reference = reference;
    inputWidth = input.width;
    int hist_size = 16;
    //int hist_size = 256;
	float range_0[] = { 0, 256 };
	float *ranges[] = { range_0 };
    
    CvHistogram *input_histogram, *reference_histogram;
    
    input_histogram = cvCreateHist (1, &hist_size, CV_HIST_ARRAY, ranges, 1);
    reference_histogram = cvCreateHist (1, &hist_size, CV_HIST_ARRAY, ranges, 1);
    
	mk_histogram( degraded_reference, 0, 0, reference.width, reference.height, reference_histogram );
    
	double scale;
	if( input.width * reference.height <  reference.width * input.height )
	{
		scale = static_cast< double >( reference.width ) / input.width;
	}
	else
	{
		scale = static_cast< double >( reference.height ) / input.height;
	}
	
	size_t window_width = static_cast< size_t >( reference.width / scale );
	size_t window_height = static_cast< size_t >( reference.height / scale );
	size_t window_size = window_width * window_height;
    
	vector< bool > culling_table( input.width * input.height );
    
	double max_similarity = 0;
	double similarity;
    
	while( scale < MAX_SCALE )
	{
        for (int i = 0; i < culling_table.size(); i++) {
            culling_table[i] = false;
        }
        
		for( size_t j = 0 ; j < input.height - window_height + 1 ; j ++ )
		{
			for( size_t i = 0 ; i < input.width - window_width + 1 ; i ++ )
			{
                
				if( !culling_table[i + input.width * j] )
				{
					mk_histogram( degraded_input, i, j, window_width, window_height, input_histogram );
					similarity = histogram_intersection( input_histogram, reference_histogram );
                    
					if( similarity > max_similarity )
					{
						max_similarity = similarity;
						ret_scale = scale;
						ret_x = static_cast< unsigned int >( i * ret_scale );
						ret_y = static_cast< unsigned int >( j * ret_scale );
					}
					
					_culling_out( i, j, window_width, window_height, similarity, max_similarity, culling_table );
				}
			}
		}
		
		scale *= SCALE_FACTOR;
		window_width = static_cast< size_t >( reference.width / scale );
		window_height = static_cast< size_t >( reference.height / scale );
		window_size = window_width * window_height;
	}
    
    cvReleaseHist(&input_histogram);
    cvReleaseHist(&reference_histogram);
}

double  ofxActiveSearchImage::debugHist(ofImage& src, ofImage& dst){
    Mat tempImg = toCv(src);
    Mat tempImg2 = toCv(dst);
    double resultdata;
    
    IplImage tempSrc = tempImg;
    IplImage tempSrc2 = tempImg2;
    
    IplImage *src1 = &tempSrc, *src2 = &tempSrc2;
    
	IplImage *dst1[4] = {0, 0, 0, 0}, *dst2[4] = {0, 0, 0, 0};
	CvHistogram *hist1, *hist2;
	int sch1 = 0, sch2 = 0;
	int hist_size = 256;
	float range_0[] = { 0, 256 };
	float *ranges[] = { range_0 };
	
	if(src1 == 0 || src2 == 0){
		return -1;
	}
    
	sch1 = src1->nChannels;
	sch2 = src2->nChannels;
	if(sch1 != sch2){
		return -1;
	}
    
	for(int i=0;i<sch1;i++) {
		dst1[i] = cvCreateImage (cvSize (src1->width, src1->height), src1->depth, 1);
		dst2[i] = cvCreateImage (cvSize (src2->width, src2->height), src2->depth, 1);
	}
    
	//ヒストグラム構造体を確保
	hist1 = cvCreateHist (1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	hist2 = cvCreateHist (1, &hist_size, CV_HIST_ARRAY, ranges, 1);
    
	//入力画像がマルチチャンネルの場合，画像をチャンネル毎に分割
	if (sch1 == 1) {
		cvCopy (src1, dst1[0], NULL);
		cvCopy (src2, dst2[0], NULL);
	}else{
		cvSplit (src1, dst1[0], dst1[1], dst1[2], dst1[3]);
		cvSplit (src2, dst2[0], dst2[1], dst2[2], dst2[3]);
	}
	//ヒストグラムの計算
	for(int i= 0;i<sch1;i++){
		cvCalcHist (&dst1[i], hist1, 0, NULL);
		cvCalcHist (&dst2[i], hist2, 0, NULL);
		cvNormalizeHist(hist1, 1.0);
		cvNormalizeHist(hist2, 1.0);
	}
    //	printf("ヒストグラムの比較\n");
    resultdata = cvCompareHist( hist1, hist2, CV_COMP_INTERSECT);
    
    for(int i= 0;i<sch1;i++){
		cvReleaseImage(&dst1[i]);
        cvReleaseImage(&dst2[i]);
	}
    
    cvReleaseHist(&hist1);
    cvReleaseHist(&hist2);
    return resultdata;
}

void ofxActiveSearchImage::debugHist2(ofImage* src, CvHistogram* dst){
    Mat tempImg = toCv(*src);
    IplImage tempSrc = tempImg;
	IplImage *dst1[4] = {0, 0, 0, 0};
	
	int sch1 = 0;
	
	if(&tempSrc == 0){
		return;
	}
    
	sch1 = tempSrc.nChannels;
    
	for(int i=0;i<sch1;i++) {
		dst1[i] = cvCreateImage (cvSize (tempSrc.width, tempSrc.height), tempSrc.depth, 1);
	}
    	
	//入力画像がマルチチャンネルの場合，画像をチャンネル毎に分割
	if (sch1 == 1) {
		cvCopy (&tempSrc, dst1[0], NULL);
		
	}else{
		cvSplit (&tempSrc, dst1[0], dst1[1], dst1[2], dst1[3]);
		
	}
	//ヒストグラムの計算
	for(int i= 0;i<sch1;i++){
		cvCalcHist (&dst1[i], dst, 0, NULL);
		cvNormalizeHist(dst, 1.0);
		
	}
    
    for(int i= 0;i<sch1;i++){
		cvReleaseImage(&dst1[i]);
	}

    return;
}
