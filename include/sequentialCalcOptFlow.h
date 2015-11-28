/* sequentialCalcOptFlow.h */
#ifndef INCLUDE_SEQUENTIALCALCOPTFLOW_H
#define INCLUDE_SEQUENTIALCALCOPTFLOW_H

//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <mutex>
#include <opencv2/opencv.hpp>

//-----------------------------------------------------------------------
// grobal variable declaration
//-----------------------------------------------------------------------
extern std::mutex opt_flow_mtx;

//-----------------------------------------------------------------------
// function prototype
//-----------------------------------------------------------------------
void sequentialCalcOptFlow(cv::Mat& curr,
                           cv::Mat& flow,
                           bool& break_flag);

//-----------------------------------------------------------------------
#endif // INCLUDE_SEQUENTIALCALCOPTFLOW_H
