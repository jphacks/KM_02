/* optFlow2RGB.cpp */
//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <opencv2/opencv.hpp>
#include "optFlow2RGB.h"

//-----------------------------------------------------------------------
// using namespace
//-----------------------------------------------------------------------
using namespace std;

//-----------------------------------------------------------------------

//***********************************************************************
// Function : optFlow2RGB |
//***********************************************************************
void optFlow2RGB(cv::Mat& flow32FC2, cv::Mat& rgb32FC3){
    cv::Mat flowXY[2];
    split(flow32FC2, flowXY);

    //optical flow to HSV
    cv::Mat magnitude, angle; //  オプティカルフローを極座標に変換 (角度は[deg])
    cv::cartToPolar(flowXY[0], flowXY[1], magnitude, angle, true);

    cv::Mat hsvPlanes[3];
    double maxVal;
    cv::minMaxLoc(magnitude, NULL, &maxVal, NULL, NULL);

    if(maxVal > 10){
        cv::normalize(magnitude, hsvPlanes[2], 0, 1, cv::NORM_MINMAX); // 正規化
    } else { // 動体なし
        hsvPlanes[2] = cv::Mat1f::zeros(magnitude.size());
    }

    hsvPlanes[0] = angle;
    hsvPlanes[1] = cv::Mat1f::ones(magnitude.size());

    //  HSVを合成して一枚の画像にする
    cv::Mat hsv;
    merge(hsvPlanes, 3, hsv);

    //  HSVからBGRに変換
    cvtColor(hsv, rgb32FC3, cv::COLOR_HSV2BGR);
}
