// OpenCvApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <queue>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include "Fish.h"


using namespace cv;
using namespace std;

Mat _ReferenceFrame;
const static int SENSITIVITY_VALUE = 30;
const static int BLUR_SIZE = 10;


Mat _referenceGrayframe;


//int to string helper function
string intToString(int number){

	//this function has a number input and string output
	std::stringstream ss;
	ss << number;
	return ss.str();
}

bool HasPointInRect(Rect rect, vector<Point2f>& trackingPoints)
{
	for (auto point : trackingPoints)
	{
		if(rect.contains(point)) return true;
	}
	return false;
}



//void MarkTrackingPoints(Mat& frame)
//{
//	for(auto point : _trackingPoints)
//	{
//		 circle(frame, point, 8, Scalar(0,255,0), -1, 8);
//	}
//}

const int _checkLine = 200;
const Rect _CheckingWindow(100, 100, 400, 300);

Fish _TrackingFish;

bool HasObjects(Mat& thresholdImage){

	Mat temp;
	thresholdImage.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );// retrieves external contours

	if(contours.size()>0) return true;

	return false;
}


Mat _previousCheckFrame;

bool IsReferenceFrame(Mat Frame)
{
	Mat difference;
	absdiff(_previousCheckFrame, Frame, difference);

	cv::threshold(difference, difference, 30,255,THRESH_BINARY);

	cv::blur(difference, difference, cv::Size(BLUR_SIZE, BLUR_SIZE));

	cv::threshold(difference, difference, 30 ,255,THRESH_BINARY);

	Frame.copyTo(_previousCheckFrame);
	if(HasObjects(difference)) return false;

	return true;
}


int Fish::count = 0;

void CountFish(vector<vector<Point>>& contours)
{
	for (auto contour : contours)
	{
		Rect fishBoundingRect = boundingRect(contour);
		if(_TrackingFish.IsTheFish(fishBoundingRect))
		{
			if(fishBoundingRect.y <= _checkLine)
				Fish::count++;
			return;
		}
	} 
}




void UpdateTrackingFish(vector<vector<Point>>& contours)
{
	_TrackingFish.Initilize();
	for (auto contour : contours)
	{
		Rect fishRect = boundingRect(contour);	
		if(fishRect.width > fishRect.height) continue;
		if(fishRect.width <= 3 || fishRect.height <= 3) continue;
		int distance = fishRect.y - _checkLine;
		if(distance < 0) continue; 

		if(_TrackingFish.GetMarkPoint().y - fishRect.y > 0)
			_TrackingFish.SetBoundingRect(fishRect);
	}
}

void DetectObjects(Mat Frame)
{
	Mat difference;
	absdiff(_ReferenceFrame, Frame, difference);

	cv::threshold(difference, difference, 40,255,THRESH_BINARY);
	cv::blur(difference, difference, cv::Size(BLUR_SIZE, BLUR_SIZE));
	cv::threshold(difference, difference, 40 ,255,THRESH_BINARY);

	Mat temp;
	difference.copyTo(temp);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE );// retrieves external contours

//	cout << "contours0:" << contours.size() << endl;
	CountFish(contours);
	UpdateTrackingFish(contours);

	imshow("Objects", difference);
}




int _tmain(int argc, _TCHAR* argv[])
{

	//some boolean variables for added functionality
	bool objectDetected = false;
	//these two can be toggled by pressing 'd' or 't'
	bool debugMode = false;
	bool trackingEnabled = false;
	//pause and resume code
	bool pause = false;
	//set up the matrices that we will need
	//the two frames we will be comparing
	Mat frame1;
	//their grayscale images (needed for absdiff() function)
	Mat grayImage1;
	//resulting difference image
	Mat differenceImage;
	//thresholded difference image (for use in findContours() function)
	Mat thresholdImage;
	//video capture object.
	VideoCapture capture;

	while(1){

		capture.open("Motion1.avi");

		if(!capture.isOpened()){
			cout<<"ERROR ACQUIRING VIDEO FEED\n";
			getchar();
			return -1;
		}

		capture >> _ReferenceFrame;
		int count = 0;
		cv::cvtColor(_ReferenceFrame, _ReferenceFrame, COLOR_BGR2GRAY);
		_ReferenceFrame.copyTo(_previousCheckFrame);
		while(1){

			capture >> frame1;
			if(frame1.empty()) break;
			cv::cvtColor(frame1,grayImage1,COLOR_BGR2GRAY);

			if(count%4 == 0)
			{
				if(IsReferenceFrame(grayImage1))
				{
					imshow("Reference Frame", frame1);
					grayImage1.copyTo(_ReferenceFrame);
//					_TrackingFish.clear();
				}
				count = 0;
			}
			count++;

			DetectObjects(grayImage1);

			std::ostringstream stringStream;
			stringStream << "Fish Count: " << Fish::count;

			putText(frame1, stringStream.str(),Point2i(50, _checkLine), FONT_HERSHEY_COMPLEX_SMALL, 1.0, Scalar(0,0,255));
			line(frame1, Point2i(0,_checkLine), Point2i(640,_checkLine), Scalar(0,255,0));
			rectangle(frame1, _TrackingFish.GetBoundingRect(), Scalar(255,0,0));
			imshow("Frame1",frame1);

			switch(waitKey(30)){
				 
			case 27: //'esc' key has been pressed, exit program.
				return 0;
			case 116: //'t' has been pressed. this will toggle tracking
				trackingEnabled = !trackingEnabled;
				if(trackingEnabled == false) cout<<"Tracking disabled."<<endl;
				else cout<<"Tracking enabled."<<endl;
				break;
			case 100: //'d' has been pressed. this will debug mode
				debugMode = !debugMode;
				if(debugMode == false) cout<<"Debug mode disabled."<<endl;
				else cout<<"Debug mode enabled."<<endl;
				break;
			case 112: //'p' has been pressed. this will pause/resume the code.
				pause = !pause;
				if(pause == true){ cout<<"Code paused, press 'p' again to resume"<<endl;
				while (pause == true){
					//stay in this loop until 
					switch (waitKey()){
						//a switch statement inside a switch statement? Mind blown.
					case 112: 
						//change pause back to false
						pause = false;
						cout<<"Code Resumed"<<endl;
						break;
					}
				}
				}

			}
		}
		//release the capture before re-opening and looping again.
		capture.release();
	}

	return 0;

}

