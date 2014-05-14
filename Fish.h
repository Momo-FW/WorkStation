#pragma once

#include <opencv2/highgui/highgui.hpp>


class Fish
{
public:
	Fish(void);
	Fish(cv::Rect boundRect);
	~Fish(void);

	cv::Rect GetBoundingRect() const { return boundingRectangle;} 
	cv::Point2i GetMarkPoint() const;
	cv::Point2i GetExpectedMarkPoint() const;
	cv::Point2i GetDelta() const { return delta;};

	void SetBoundingRect(cv::Rect boundingRect);
	void UpdateBoundingRect(cv::Rect boundingRect);

	bool IsTheFish(Fish fish) const;
	bool TrackFish(Fish fish);

	bool IsDanglingFish() const;
	bool IsInTracking() const {return IsInTrack; }
	void SetTracKing() { IsInTrack = true;}
	void Initilize() {boundingRectangle.x = 10000;  boundingRectangle.y = 1000; }
	static int count;
	bool IsInTrack;
private:
	cv::Rect boundingRectangle;
	cv::Point2i delta;
};

