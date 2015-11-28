/* main.cpp */
//-----------------------------------------------------------------------
//include files
//-----------------------------------------------------------------------
#include <iostream>
#include <oauth.h>
#include <thread>
#include <mutex>
#include <unistd.h>

#include <opencv2/opencv.hpp>
#include <opencv2/superres/optical_flow.hpp>

#include "sequentialCalcOptFlow.h"
#include "sequentialCaptCurrBuffer.h"
#include "optFlow2RGB.h"

#include "tweet.h"
#include "webclient.h"
// #include "mykey.h"
#include "jphacks_key.h"

//-----------------------------------------------------------------------
// using namespace
//-----------------------------------------------------------------------
using namespace std;

//-----------------------------------------------------------------------

//***********************************************************************
// Function : main |
//***********************************************************************
int main(void)
{
    // buffering off for debug on pty
    setvbuf( stdout, NULL, _IONBF, BUFSIZ );

    WebClient::initialize();
    TwitterClient tc(c_key, c_sec, t_key, t_sec);
    std::string filename("../../skeleton.png");
    cv::Mat src = cv::imread(filename);

    // current frame の連続取得(別スレッド)
    cv::Mat curr_tmp;
    bool break_flag = false;
    thread cap_th( sequentialCaptCurrBuffer, ref(curr_tmp), ref(break_flag));
    while( curr_tmp.empty() ){
        sleep(0);
    }

    // optical flow の連続算出(別スレッド)
    cv::Mat flow_tmp;
    thread optflow_th( sequentialCalcOptFlow, ref(curr_tmp), ref(flow_tmp), ref(break_flag));
    while( flow_tmp.empty() ){
        sleep(0);
    }

    while( cv::waitKey(1) != '\x1b' ) {
        cap_mtx.lock();
        cv::Mat curr = curr_tmp.clone();
        cap_mtx.unlock();

        opt_flow_mtx.lock();
        cv::Mat flow = flow_tmp.clone();
        opt_flow_mtx.unlock();

        cv::Mat flow_rgb;
        optFlow2RGB( flow, flow_rgb );

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
        std::vector< std::vector< cv::Point > > contours;

        //輪郭取得
        cv::findContours(bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        // 検出された輪郭線を緑で描画
        for (auto contour = contours.begin(); contour != contours.end(); contour++){
            //輪郭を直線近似する
            std::vector< cv::Point > approx;
            cv::approxPolyDP(cv::Mat(*contour), approx, 0.01 * cv::arcLength(*contour, true), true);
            // 近似の面積が一定以上なら取得
            double area = cv::contourArea(approx);

            if (area > 1000.0){
                cv::Rect brect = cv::boundingRect(cv::Mat(approx).reshape(2));

                // 外接矩形を描画
                cv::rectangle(curr, brect.tl(), brect.br(), cv::Scalar(255, 0, 0), 2, CV_AA);
                // cv::polylines(curr, *contour, true, cv::Scalar(0, 255, 0), 2);
            }
        }

        //全体を表示する場合
        cv::imshow("coun", curr);
    }

    // スレッドの終了
    break_flag = true;
    cap_th.join();
    optflow_th.join();

    //return ( tc.tweet(message, src) ) ? 0 : 1;
    return 0;
}
