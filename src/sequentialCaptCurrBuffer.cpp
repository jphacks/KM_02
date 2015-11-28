/* sequentialCaptCurrBuffer.cpp */
//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <mutex>
#include <opencv2/opencv.hpp>
#include "sequentialCaptCurrBuffer.h"

//-----------------------------------------------------------------------
// Global variable definition
//-----------------------------------------------------------------------
std::mutex cap_mtx;

//-----------------------------------------------------------------------
// using namespace
//-----------------------------------------------------------------------
using namespace std;

//-----------------------------------------------------------------------

//***********************************************************************
// Function : sequentialCaptCurrBuffer |
//***********************************************************************
void sequentialCaptCurrBuffer(cv::Mat& curr, bool& break_flag){
    cv::VideoCapture cap(0);
    if(!cap.isOpened()){
        break_flag = true;
    }

    while( !break_flag ){
        // 現フレームを取得
        cv::Mat curr_tmp;
        cap >> curr_tmp;

        cap_mtx.lock();
        cv::swap( curr, curr_tmp);
        cap_mtx.unlock();
    };
}
