/* detectMotionObject.cpp */
//-----------------------------------------------------------------------
// include files
//-----------------------------------------------------------------------
#include <vector>
#include <opencv2/opencv.hpp>
#include "detectMotionObject.h"
#include "optFlow2RGB.h"

//-----------------------------------------------------------------------
// using namespace
//-----------------------------------------------------------------------
using namespace std;

//-----------------------------------------------------------------------

//***********************************************************************
// Function : detectMotionObject |
//***********************************************************************
void detectMotionObject( cv::Mat& curr, cv::Mat& flow32FC2, std::vector<cv::Rect> detected_obj ){
    // optical flow to RGB
    cv::Mat flow_rgb;
    optFlow2RGB( flow32FC2, flow_rgb );

    // 表示
    cv::imshow("optical flow", flow_rgb);

    // グレースケール
    cv::Mat gray, bin;
    cv::cvtColor( flow_rgb, gray, CV_BGR2GRAY);

    // 平滑化
    blur( gray, gray, cv::Size(3,3) );
    cv::imshow("gray", gray);

    // 二値化
    cv::threshold(cv::Mat1b(gray*255), bin, 10, 255.0, CV_THRESH_BINARY);
    cv::imshow("bin", bin);

    //輪郭の座標リスト
    std::vector<std::vector<cv::Point>> contours;

    //輪郭取得
    cv::findContours(bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    cv::Mat curr_disp = curr.clone();
    for (auto contour = contours.begin(); contour != contours.end(); contour++){
        //輪郭を直線近似する
        std::vector< cv::Point > approx;
        cv::approxPolyDP(cv::Mat(*contour), approx, 0.01 * cv::arcLength(*contour, true), true);
        // 近似の面積が一定以上なら取得
        double area = cv::contourArea(approx);

        if (area > 1000.0){
            // 検出された輪郭線を緑で描画
            // cv::polylines(curr, *contour, true, cv::Scalar(0, 255, 0), 2);

            // 外接矩形を描画
            detected_obj.push_back( cv::boundingRect(cv::Mat(approx).reshape(2)) );
            cv::rectangle(curr_disp, detected_obj.back().tl(), detected_obj.back().br(), cv::Scalar(255, 0, 0), 2, CV_AA);
        }
    }

    //全体を表示する場合
    cv::imshow("coun", curr_disp);
}
