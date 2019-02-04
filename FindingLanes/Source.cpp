#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat canny(Mat img)
{
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	/*imshow("test", gray);
	waitKey();*/

	Mat blur;
	GaussianBlur(gray, blur, Size(5, 5), 0);
	/*imshow("test", blur);
	waitKey();*/

	Mat canny;
	Canny(blur, canny, 50, 150);
	/*imshow("test", canny);
	waitKey();*/

	return canny;
}

Mat regionOfInterest(Mat img)
{
	int width = img.cols;//getWindowImageRect("img").width;//img
	int height = img.rows;//getWindowImageRect("img").height;//img
	
	Mat mask(Size(width, height), CV_8UC1, Scalar(0));
	
	/*imshow("test", mask);
	waitKey();*/

	int x1 = 200, y1 = height,
		x2 = 1100, y2 = height,
		x3 = 550, y3 = 250;

	/*vector<Point> polygon;
	polygon.push_back(Point(x1, y1));
	polygon.push_back(Point(x2, y2));
	polygon.push_back(Point(x3, y3));*/

	Point points[1][3];
	points[0][0] = Point(x1, y1);
	points[0][1] = Point(x2, y2);
	points[0][2] = Point(x3, y3);

	const int size = 3;
	const Point *p[1] = { points[0] };	

	fillPoly(mask, p, &size, 1, Scalar(255));
	/*imshow("test", mask);
	waitKey();*/

 	bitwise_and(img, mask, mask);
	/*imshow("test", mask);
	waitKey();*/
	
	return mask;
}

Mat houghLine(Mat imgOrg, Mat cannyImg)
{
	vector<Vec4i> lines; //vector<cv::Vec<int, 4>> houghlines
	HoughLinesP(cannyImg, lines, 1, CV_PI / 180, 100, 40, 50);//Treshold:100 - MinlineLength:40 - MaxlineGap:10
														 //100,10,50		50,40,10
	for (int i=0; i<lines.size(); i++)
	{
		Point a = Point(lines[i][0], lines[i][1]);
		Point b = Point(lines[i][2], lines[i][3]);
		line(imgOrg, a, b, Scalar(0, 0, 255));
	}

	return imgOrg;
}

int main()
{
	Mat img = imread("den5.jpg");

	if (!img.empty())
		imshow("img", img);
	else
		cout << "Resim yuklenemedi!" << endl;

	waitKey();

	Mat laneimg = img.clone();

	Mat cannyimg = canny(laneimg);

	Mat croppedimage = regionOfInterest(cannyimg);
	
	imshow("resultCanny", croppedimage);
	waitKey();

	Mat linesImg = houghLine(img, croppedimage);

	imshow("resultHough", linesImg);
	waitKey();


	//Video---------------------------------------------
	VideoCapture video;
	Mat videoFrame;

	if (!video.open("testVideo.mp4"))
	{
		cout << "Video acilamadi!";
		return 0;
	}

	namedWindow("video");

	while (video.read(videoFrame))
	{
		/*imshow("test", videoFrame);
		waitKey();*/

		Mat laneimg = videoFrame.clone();
		Mat cannyimg = canny(laneimg);
		Mat croppedimage = regionOfInterest(cannyimg);
		Mat linesImg = houghLine(videoFrame, croppedimage);

		imshow("video", linesImg);

		if (waitKey(1) != -1)
			break;
	}

	video.release();
	destroyAllWindows();
	return 0;
}