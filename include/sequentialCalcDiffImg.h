/* sequentialCalcDiffImg.h */
#ifndef INCLUDE_SEQUENTIALCALCDIFFIMG_H
#define INCLUDE_SEQUENTIALCALCDIFFIMG_H

//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <mutex>
#include <opencv2/opencv.hpp>

//-----------------------------------------------------------------------
// grobal variable declaration
//-----------------------------------------------------------------------
extern std::mutex diff_mtx;

//-----------------------------------------------------------------------
// function prototype
//-----------------------------------------------------------------------
void sequentialCalcDiffImg(cv::Mat& curr, cv::Mat& diff, bool& break_flag);

//-----------------------------------------------------------------------
#endif // INCLUDE_SEQUENTIALCALCDIFFIMG_H
