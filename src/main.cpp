#include <iostream>
#include <oauth.h>
#include <opencv2/opencv.hpp>

#include <opencv2/opencv.hpp>
#include <opencv2/superres/optical_flow.hpp>

#include "tweet.h"
#include "webclient.h"
// #include "mykey.h"
#include "jphacks_key.h"

int main(void)
{
    // buffering off for debug on pty
    setvbuf( stdout, NULL, _IONBF, BUFSIZ );

    WebClient::initialize();
    TwitterClient tc(c_key, c_sec, t_key, t_sec);
    std::string filename("../../skeleton.png");
    cv::Mat src = cv::imread(filename);

    //return ( tc.tweet(message, src) ) ? 0 : 1;
    return 0;
}
