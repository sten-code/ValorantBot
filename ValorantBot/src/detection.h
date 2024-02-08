#pragma once

#include <opencv2/opencv.hpp>

typedef std::vector<cv::Point> Contour;

namespace Detection
{
	std::vector<Contour> FindContours(const cv::Mat& image, const cv::Scalar& lowerBound, const cv::Scalar& upperBound);
	Contour FindBestContour(const std::vector<Contour>& contours, cv::Point viewport);
	cv::Point FindTop(const Contour& contour);
}