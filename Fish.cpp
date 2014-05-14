#include "stdafx.h"
#include "Fish.h"
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

Fish::Fish(void):delta(0,0),boundingRectangle(0,0,0,0),IsInTrack(false)
{

}

Fish::Fish(Rect boundRect):delta(0,0),boundingRectangle(boundRect), IsInTrack(false)
{

}

Fish::~Fish(void)
{
}


Point2i Fish::GetMarkPoint() const
{
	return Point2i(boundingRectangle.x, boundingRectangle.y);
}


Point2i Fish::GetExpectedMarkPoint() const
{
	return Point2i(boundingRectangle.x + delta.x, boundingRectangle.y + delta.y);
}


void Fish::SetBoundingRect(cv::Rect boundingRect)
{
	boundingRectangle = boundingRect;
}

void Fish::UpdateBoundingRect(cv::Rect boundingRect)
{
	delta.x =  boundingRect.x - boundingRectangle.x;
	delta.y =  boundingRect.y - boundingRectangle.y;

	boundingRectangle.x = boundingRect.x;
	boundingRectangle.y = boundingRect.y;

	boundingRectangle.width = max(boundingRect.width, boundingRectangle.width);
	boundingRectangle.height = max(boundingRect.height, boundingRectangle.height);
}



bool Fish::IsTheFish(Fish fish) const
{
	if(norm(GetExpectedMarkPoint() - fish.GetMarkPoint()) < 30)
		return true;
	return false;
}

bool Fish::IsDanglingFish() const
{
	if(boundingRectangle.width <= 3
		|| boundingRectangle.height <= 3
		|| boundingRectangle.width > boundingRectangle.height 
		) return false;

	return true;
}

bool Fish::TrackFish(Fish fish)
{
	if(!fish.IsDanglingFish()) return false;
	if(IsTheFish(fish))
	{
		UpdateBoundingRect(fish.GetBoundingRect());
		return true;
	}
}
