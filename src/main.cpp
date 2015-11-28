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

        cv::Mat flowXY[2];
        split(flow, flowXY);

        // オプティカルフローの可視化（色符号化）
        //  オプティカルフローを極座標に変換（角度は[deg]）
        cv::Mat magnitude, angle;
        cv::cartToPolar(flowXY[0], flowXY[1], magnitude, angle, true);

        double maxVal;
        cv::minMaxLoc(magnitude, NULL, &maxVal, NULL, NULL);
        // std::cout << maxVal << "\n";
        //  色相（H）はオプティカルフローの角度
        //  彩度（S）は0～1に正規化したオプティカルフローの大きさ
        //  明度（V）は1
        cv::Mat hsvPlanes[3];
        hsvPlanes[0] = angle;
        if(maxVal > 10){
            cv::normalize(magnitude, hsvPlanes[2], 0, 1, cv::NORM_MINMAX); // 正規化
        } else {
            hsvPlanes[2] = cv::Mat::zeros(magnitude.size(), CV_32F);
        }

        hsvPlanes[1] = cv::Mat::ones(magnitude.size(), CV_32F);

        //  HSVを合成して一枚の画像にする
        cv::Mat hsv;
        merge(hsvPlanes, 3, hsv);

        //  HSVからBGRに変換
        cv::Mat flowBgr;
        cvtColor(hsv, flowBgr, cv::COLOR_HSV2BGR);

        // 表示
        cv::imshow("optical flow", flowBgr);

        // グレースケール
        cv::Mat gray, bin;
        cv::cvtColor( flowBgr, gray, CV_BGR2GRAY);
        cv::minMaxLoc(flowBgr, NULL, &maxVal, NULL, NULL);

        // 平滑化
        blur( gray, gray, cv::Size(3,3) );
        cv::imshow("gray", gray);

        // cv::threshold(cv::Mat1b(gray*255), bin, 0.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);
        cv::threshold(cv::Mat1b(gray*255), bin, 10, 255.0, CV_THRESH_BINARY);
        cv::imshow("bin", bin);

        //輪郭の座標リスト
        std::vector< std::vector< cv::Point > > contours;

        //輪郭取得
        cv::findContours(bin, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
        //cv::findContours(bin, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

        // 検出された輪郭線を緑で描画
        for (auto contour = contours.begin(); contour != contours.end(); contour++){
            // cv::polylines(prev, *contour, true, cv::Scalar(0, 255, 0), 2);

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
