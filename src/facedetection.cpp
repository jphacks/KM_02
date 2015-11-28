#include "main.h"
#include "facedetection.h"

using namespace std;
using namespace cv;

int enable_count = 0;

Mat detectFaceInImage(Mat &image,string &cascade_file){
	CascadeClassifier cascade;
	cascade.load(cascade_file);
	vector<Rect> faces;
	cascade.detectMultiScale(image, faces, 1.1,3,0,Size(20,20));
	
	for (int i = 0; i < faces.size(); i++){
		rectangle(image, Point(faces[i].x,faces[i].y),Point(faces[i].x + faces[i].width,faces[i].y + faces[i].height),Scalar(0,200,0),3,CV_AA);
		if((300 < faces[i].width) && (faces[i].width < 310) ){
			enable_count ++;
		}else{
			enable_count = 0;
		}
		if(enable_count == 3){
			imwrite("../../image/test.png", image);
			ofstream ofs("../../comment/test.txt");
			ofs << "誰か来ました！" << endl;
			cout << "保存しました！" << endl;
			enable_count = 0;
		}
		cout << "矩形の大きさ：" << faces[i].width << "×" << faces[i].height << endl;
	}
	 
	return image;
}

