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

        vector<cv::Rect> detected_obj;
        detectMotionObject(curr, flow, detected_obj);
    }

    // スレッドの終了
    break_flag = true;
    cap_th.join();
    optflow_th.join();

    //return ( tc.tweet(message, src) ) ? 0 : 1;
    return 0;
}
