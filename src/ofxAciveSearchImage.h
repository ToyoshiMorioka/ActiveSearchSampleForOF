//
//  ofxAciveSearchImage.h
//  ofxcvTest002
//
//  Created by MORIOKA TOYOSHI on 2013/02/27.
//
//

#ifndef __ofxcvTest002__ofxAciveSearchImage__
#define __ofxcvTest002__ofxAciveSearchImage__

#include "ofMain.h"
#include "ofxCv.h"

#define Q 16
#define MAX_SCALE 5.0
#define SCALE_FACTOR 1.25
//#define SCALE_FACTOR 2.0

using namespace cv;
using namespace ofxCv;

class ofxActiveSearchImage:ofBaseApp{
    
public:
    ofxActiveSearchImage();
    ~ofxActiveSearchImage();
    
    double debugHist(ofImage& src, ofImage& dst);
    void debugHist2(ofImage* src, CvHistogram* dst);
    
    double _minimum( const double v0, const double v1 );
    double _upper_bound( const size_t a_and_b, const size_t window_size, const double &similarity );
    void tone_down( ofImage &in, ofImage &out, const unsigned char base );
    void mk_histogram( ofImage &img, const size_t pi, const size_t pj, const size_t w_width, const size_t w_height, CvHistogram* hist );
    void _culling_out( const size_t pi, const size_t pj, const size_t w_width, const size_t w_height, const double &sim, const double &max_sim, vector<bool>& culling_table );
    double	histogram_intersection	( CvHistogram* histogram_a, CvHistogram* histogram_b );
    void active_search( ofImage &input,
                       ofImage &reference,
                       int &ret_x,
                       int &ret_y,
                       double &ret_scale );
    
    int inputWidth;
//private:
    
};

#endif /* defined(__ofxcvTest002__ofxAciveSearchImage__) */
