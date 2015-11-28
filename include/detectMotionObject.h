/* detectMotionObject.h */
#ifndef INCLUDE_DETECTMOTIONOBJECT_H
#define INCLUDE_DETECTMOTIONOBJECT_H

//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <vector>
#include <opencv2/opencv.hpp>

//-----------------------------------------------------------------------
// function prototype
//-----------------------------------------------------------------------
void detectMotionObject( cv::Mat& curr, cv::Mat& flow32FC2, std::vector<cv::Rect> detected_obj );

//-----------------------------------------------------------------------
#endif // INCLUDE_DETECTMOTIONOBJECT_H
