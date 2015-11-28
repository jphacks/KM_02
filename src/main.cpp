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

#include "sequentialCaptCurrBuffer.h"
#include "sequentialCalcDiffImg.h"
#include "sequentialCalcOptFlow.h"
#include "detectMotionObject.h"

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

    // 画像差分の連続算出(別スレッド)
    cv::Mat diff_tmp;
    thread diff_th( sequentialCalcDiffImg, ref(curr_tmp), ref(diff_tmp), ref(break_flag));
    while( diff_tmp.empty() ){
        sleep(0);
    }

    // // optical flow の連続算出(別スレッド)
    // cv::Mat flow_tmp;
    // thread optflow_th( sequentialCalcOptFlow, ref(curr_tmp), ref(flow_tmp), ref(break_flag));
    // while( flow_tmp.empty() ){
    //     sleep(0);
    // }

    while( cv::waitKey(1) != '\x1b' ) {
        cap_mtx.lock();
        cv::Mat curr = curr_tmp.clone();
        cap_mtx.unlock();

        diff_mtx.lock();
        cv::Mat diff = diff_tmp.clone();
        diff_mtx.unlock();
        cv::imshow("diff", diff);

        // opt_flow_mtx.lock();
        // cv::Mat flow = flow_tmp.clone();
        // opt_flow_mtx.unlock();

        // // optical flow to RGB
        // cv::Mat flow_rgb;
        // optFlow2RGB( flow, flow_rgb );

        // // 表示
        // cv::imshow("optical flow", flow_rgb);

        // // グレースケール
        // cv::Mat gray;
        // cv::cvtColor( flow_rgb, gray, CV_BGR2GRAY);

        vector<cv::Rect> detected_obj;
        detectMotionObject(curr, diff, detected_obj);
    }

    // スレッドの終了
    break_flag = true;
    cap_th.join();
    diff_th.join();
    // optflow_th.join();

    //return ( tc.tweet(message, src) ) ? 0 : 1;
    return 0;
}
