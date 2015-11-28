/* sequentialCalcDiffImg.cpp */
//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <iostream>
#include <mutex>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include "sequentialCaptCurrBuffer.h"
#include "sequentialCalcDiffImg.h"

//-----------------------------------------------------------------------
// Global variable definition
//-----------------------------------------------------------------------
std::mutex diff_mtx;

//-----------------------------------------------------------------------
// using namespace
//-----------------------------------------------------------------------
using namespace std;

//-----------------------------------------------------------------------

//***********************************************************************
// Function : sequentialCalcDiffImg |
//***********************************************************************
void sequentialCalcDiffImg(cv::Mat& curr, cv::Mat& diff, bool& break_flag){
    cv::Mat gray_prev;
    cap_mtx.lock();
    cv::cvtColor( curr.clone(), gray_prev, CV_BGR2GRAY); // グレースケール
    cap_mtx.unlock();
    // blur( gray_prev, gray_prev, cv::Size(3,3) ); // 平滑化

    cv::Mat gray_save = gray_prev.clone();

    while( !break_flag ){
        cv::Mat gray_curr;
        cap_mtx.lock();
        cv::cvtColor( curr.clone(), gray_curr, CV_BGR2GRAY); // グレースケール
        cap_mtx.unlock();
        blur( gray_curr, gray_curr, cv::Size(3,3) ); // 平滑化

        // 画像差分の選出
        cv::Mat gray_diff1 = gray_prev - gray_curr;
        cv::Mat gray_diff2 = gray_save - gray_curr - cv::Mat1b::ones(gray_curr.size())*30;

        double maxVal1, maxVal2;
        cv::minMaxLoc( gray_diff1, NULL, &maxVal1, NULL, NULL );
        cv::minMaxLoc( gray_diff1, NULL, &maxVal2, NULL, NULL );

        cv::Mat diff_normal1, diff_normal2;
        if(maxVal1 < 50 || maxVal2 < 50){ // 動体なし
            continue;
        }

        cv::normalize( gray_diff2, diff_normal1, 0, 255, cv::NORM_MINMAX); // 正規化
        cv::normalize( gray_diff1, diff_normal2, 0, 255, cv::NORM_MINMAX); // 正規化
        cv::Mat diff_normal = diff_normal1 + diff_normal2;

        diff_mtx.lock();
        diff = diff_normal.clone();
        diff_mtx.unlock();

        cv::swap(gray_prev, gray_curr);
    };
}
