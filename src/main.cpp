/* main.cpp */
//-----------------------------------------------------------------------
//include files
//-----------------------------------------------------------------------
#include <iostream>
#include <oauth.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <unistd.h>

#include <opencv2/opencv.hpp>
#include <opencv2/superres/optical_flow.hpp>

#include "sequentialCaptCurrBuffer.h"
#include "sequentialCalcDiffImg.h"
#include "sequentialCalcOptFlow.h"
#include "optFlow2RGB.h"
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

    // current frame の連続取得(別スレッド)
    cv::Mat curr_tmp;
    bool break_flag = false;
    std::chrono::milliseconds ms(250);
    cv::VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    thread cap_th( sequentialCaptCurrBuffer, ref(cap), ref(curr_tmp), ref(break_flag));
    cv::Mat src;      //twitter投稿用
    string message;   //twitter投稿用

    while( curr_tmp.empty() ){
        std::this_thread::sleep_for(ms);
    }

    // 画像差分の連続算出(別スレッド)
    cv::Mat diff_tmp;
    thread diff_th( sequentialCalcDiffImg, ref(curr_tmp), ref(diff_tmp), ref(break_flag));
    while( diff_tmp.empty() ){
        std::this_thread::sleep_for(ms);
    }

    // // optical flow の連続算出(別スレッド)
    // cv::Mat flow_tmp;
    // thread optflow_th( sequentialCalcOptFlow, ref(curr_tmp), ref(flow_tmp), ref(break_flag));
    // while( flow_tmp.empty() ){
    //     std::this_thread::sleep_for(ms);
    // }
    while(1) {
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

        vector<cv::Rect> detected_obj_diff, detected_obj_optflow;
        detectMotionObject(curr, diff, detected_obj_diff);
        // detectMotionObject(curr, gray, detected_obj_optflow);

        string filename = "./data_set/object2.xml";
        cv::FileStorage fs1(filename, cv::FileStorage::READ);
        int width_min, width_max,width;
        string text;

        fs1["width_min"] >> width_min;
        fs1["width_max"] >> width_max;
        fs1["text"] >> text;

        int enable_count = 0;
        int esc_key = 0;
        for(auto i: detected_obj_diff){
            cv::rectangle(curr, i.tl(), i.br(), cv::Scalar(255, 0, 0), 2, CV_AA);
            width = i.br().x - i.tl().x;
            cout << "width :" << width << endl;
            if((width_min < width) && (width < width_max)){
              enable_count++;
            }else{
              enable_count = 0;
            }
            if(enable_count == 3){
              src = curr;
              message = text;
              cout << "twitterに投稿しますか？"<< endl;
              cout << "する：'s' しない：'s'以外"<<endl;
              int key = cv::waitKey(0);
              if(key == 's'){
                esc_key='\x1b';
              }
            }
        }
        // for(auto i: detected_obj_optflow){
        //     cv::rectangle(curr, i.tl(), i.br(), cv::Scalar(0, 255, 0), 2, CV_AA);
        // }
        cv::imshow("dist", curr);
        int key = cv::waitKey(30);
        if((esc_key == '\x1b') || (key == '\x1b')){
          break;
        }
    }

    // スレッドの終了
    break_flag = true;
    cap_th.join();
    diff_th.join();
    // optflow_th.join();

    WebClient::initialize();
    TwitterClient tc(c_key, c_sec, t_key, t_sec);
    //cv::Mat src = diff;

    return ( tc.tweet(message, src) ) ? 0 : 1;
    //return 0;
}
