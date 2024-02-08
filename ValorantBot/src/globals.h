#pragma once

#include <opencv2/opencv.hpp>
#include <Windows.h>

namespace Globals
{
	static cv::Point s_WindowSize;
	static cv::Point s_WindowLocation;
	static cv::Point s_WindowCenter;
	static cv::Point s_FOVSize;
	static cv::Point s_FOVCenter;
	static cv::Point s_CaptureLocation;
	static RECT s_WindowRect;
}