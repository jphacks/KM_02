/* sequentialCalcOptFlow.cpp */
//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/superres/optical_flow.hpp>

#include "sequentialCalcOptFlow.h"
#include "sequentialCaptCurrBuffer.h"

//-----------------------------------------------------------------------
// Global variable definition
//-----------------------------------------------------------------------
std::mutex opt_flow_mtx;

//-----------------------------------------------------------------------
// using namespace
//-----------------------------------------------------------------------
using namespace std;

//-----------------------------------------------------------------------
//***********************************************************************
// Function : sequentialCalcOptFlow |
//***********************************************************************
void sequentialCalcOptFlow(cv::Mat& curr, cv::Mat& flow, bool& break_flag){
    // FarneBackアルゴリズムによるオプティカルフロー計算オブジェクトの生成
    auto opticalFlow = cv::superres::createOptFlow_Farneback();
    opticalFlow->setPyrScale(0.5);
    opticalFlow->setLevelsNumber(3);
    opticalFlow->setWindowSize(15);
    opticalFlow->setIterations(3);
    opticalFlow->setPolyN(5);
    opticalFlow->setPolySigma(1.2);
    opticalFlow->setFlags(0);

    cv::Mat prev = curr.clone();

    while( !break_flag ){
        cap_mtx.lock();
        cv::Mat curr_tmp = curr.clone();
        cap_mtx.unlock();

        // オプティカルフローの計算
        cv::Mat flow_tmp;
        opticalFlow->calc(prev, curr_tmp, flow_tmp);

        opt_flow_mtx.lock();
        flow = flow_tmp.clone();
        opt_flow_mtx.unlock();

        prev = curr_tmp;
    };
}
