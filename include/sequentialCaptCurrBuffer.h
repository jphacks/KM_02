/* sequentialCaptCurrBuffer.h */
#ifndef INCLUDE_SEQUENTIALCAPTCURRBUFFER_H
#define INCLUDE_SEQUENTIALCAPTCURRBUFFER_H

//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <mutex>
#include <opencv2/opencv.hpp>

//-----------------------------------------------------------------------
// grobal variable declaration
//-----------------------------------------------------------------------
extern std::mutex cap_mtx;

//-----------------------------------------------------------------------
// function prototype
//-----------------------------------------------------------------------
void sequentialCaptCurrBuffer(cv::Mat& curr_buffer,
                              bool& canceled);

//-----------------------------------------------------------------------
#endif // INCLUDE_SEQUENTIALCAPTCURRBUFFER_H
