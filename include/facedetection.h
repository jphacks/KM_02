#include <fstream>
#include <string>

using namespace std;
using namespace cv;

//顔検出のため関数の宣言
Mat detectFaceInImage(Mat &image, string &cascade_file);
