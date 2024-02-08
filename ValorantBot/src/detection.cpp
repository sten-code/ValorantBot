#include "detection.h"

std::vector<Contour> Detection::FindContours(const cv::Mat& image, const cv::Scalar& lowerBound, const cv::Scalar& upperBound)
{
    // Convert the image from rgb to hsv
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    // Create a mask that only shows hsv values between the lower and upper bound
    cv::Mat mask;
    cv::inRange(hsv, lowerBound, upperBound, mask);

    // Make the pixels bigger in the shape of a triangle so that the contouring works better
    cv::Mat dilated;
    cv::Mat kernel = (cv::Mat_<uchar>(3, 3) <<
        1, 1, 1,
        1, 1, 1,
        0, 1, 0);
    cv::dilate(mask, dilated, kernel, cv::Point(-1, -1), 5);

    // Find all the contours in the image which will turn a lot of pixels into big blobs
    // which can be parsed and evaluated individually
    std::vector<Contour> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dilated, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    return contours;
}

Contour Detection::FindBestContour(const std::vector<Contour>& contours, cv::Point viewport)
{
    // Find the best contour to track
    double bestScore = 0.0f;
    Contour bestContour;
    cv::Point centerViewport = viewport / 2;
    for (const Contour& contour : contours)
    {
        cv::Rect rect = cv::boundingRect(contour);
        cv::Point center = cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);

        // Calculate the distance from the crosshair and the center of the contour
        double dist2 = (center.x - centerViewport.x) * (center.x - centerViewport.x) +
                       (center.y - centerViewport.y) * (center.y - centerViewport.y);

        // Give a score to the contour based on the the y value and the distance of the contour relative to the crosshair
        // Prioritize the y value more than the distance
        double score = (viewport.y - rect.y) + (424*424 - dist2) / 300.0f;
        if (bestScore < score)
        {
            bestScore = score;
            bestContour = contour;
        }
    }

    return bestContour;
}

cv::Point Detection::FindTop(const Contour& contour)
{
    // Calculate the best location to target within the contour
    if (contour.size() > 0)
    {
        cv::Point extTop = *std::min_element(contour.begin(), contour.end(),
            [](const cv::Point& lhs, const cv::Point& rhs) {
                return lhs.y < rhs.y;
            });

        return { extTop.x, extTop.y };
    }
    return { 0, 0 };
}
