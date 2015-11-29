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
    cout << "sequentialCaptCurrBufferに入ってる" << endl;
		cv::VideoCapture cap(0);
		cout << "VideoCaptureの宣言" << endl; 
		cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		if(!cap.isOpened()){
			cout<<"カメラが起動できていません"<<endl;			  
			break_flag = true;
    }

    while( !break_flag ){
			cout <<"while(!break_flag)に入った！"<<endl;
			// 現フレームを取得
        cv::Mat curr_tmp;
        cap >> curr_tmp;

				cout << curr_tmp << endl;

        cap_mtx.lock();
        cv::swap( curr, curr_tmp);
        cap_mtx.unlock();
    };
}
