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
void detectMotionObject( cv::Mat& curr, cv::Mat& gray, std::vector<cv::Rect>& detected_obj ){
    // 平滑化
    blur( gray, gray, cv::Size(3,3) );
    // cv::imshow("gray", gray);

    // 二値化
    cv::Mat bin;
    cv::threshold((gray.type() == CV_32FC1) ? cv::Mat1b(gray*255): gray, bin, 10, 255.0, CV_THRESH_BINARY);
    // cv::imshow("bin", bin);

    //輪郭の座標リスト
    std::vector<std::vector<cv::Point>> contours;

    //輪郭取得
    cv::findContours(bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    cv::Mat curr_disp = curr.clone();
    std::vector<cv::Rect> detected_obj_tmp;
    for (auto contour: contours){
        // 輪郭を直線近似する
        std::vector< cv::Point > approx;
        cv::approxPolyDP(cv::Mat(contour), approx, 0.01 * cv::arcLength(contour, true), true);
        // 近似の面積が一定以上なら取得
        double area = cv::contourArea(approx);

        if (area > 1000.0){
            // 検出された輪郭線を緑で描画
            // cv::polylines(curr_disp, *contour, true, cv::Scalar(0, 255, 0), 2);

            // 外接矩形を描画
            detected_obj_tmp.push_back( cv::boundingRect(cv::Mat(approx).reshape(2)) );
            // cv::rectangle(curr_disp, detected_obj.back().tl(), detected_obj.back().br(), cv::Scalar(255, 0, 0), 2, CV_AA);
        }
    }

    // 近接する矩形同士をマージ
//    cout << detected_obj_tmp.size() << ", ";
    if( detected_obj_tmp.size() > 1 ){
        for(auto& i: detected_obj_tmp){
            if(i.area() == 0){
                continue;
            }
            cv::Point2i i_centerP = (i.tl() + i.br())/2;
            int i_S = i.area();
            for(auto& j: detected_obj_tmp){
                if(i == j || j.area() == 0){
                    continue;
                }
                cv::Point2i j_centerP = (j.tl() + j.br())/2;
                int j_S = j.area();
                cv::Rect rect_tmp(i_centerP, j_centerP);
                int contain = 0;
                if(i_S > j_S){
                    contain += (i.contains(rect_tmp.tl()))?1:0;
                    contain += (i.contains(rect_tmp.tl() + cv::Point2i(rect_tmp.width,0)))?1:0;
                    contain += (i.contains(rect_tmp.tl() + cv::Point2i(0,rect_tmp.height)))?1:0;
                    contain += (i.contains(rect_tmp.br()))?1:0;
                } else {
                    contain += (j.contains(rect_tmp.tl()))?1:0;
                    contain += (j.contains(rect_tmp.tl() + cv::Point2i(rect_tmp.width,0)))?1:0;
                    contain += (j.contains(rect_tmp.tl() + cv::Point2i(0,rect_tmp.height)))?1:0;
                    contain += (j.contains(rect_tmp.br()))?1:0;
                }

                if(contain > 3){
                    i |= j;
                } else if( contain == 2){
                    if( rect_tmp.width > rect_tmp.height ){
                        if( 10 < rect_tmp.width - (i.width + j.width)/2 ){
                            i |= j;
                            j = cv::Rect();
                        }
                    } else {
                        if( 10 < rect_tmp.height - (i.height + j.height)/2 ){
                            i |= j;
                            j = cv::Rect();
                        }
                    }
                }
            }
        }
    }
    for(auto i: detected_obj_tmp){
        if(i.area() > 0){
            detected_obj.push_back(i);
        }
    }

//    cout << detected_obj.size() << "| ";
    //全体を表示する場合
    // cv::imshow("coun", curr_disp);
}
