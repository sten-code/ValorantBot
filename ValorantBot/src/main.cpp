#include "screen.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>

#include <hidapi.h>
#include <opencv2/opencv.hpp>
#pragma comment (lib, "OneCore.lib") // Required for `GetCommPorts`
#include <Windows.h>

cv::Point FindEnemy(cv::Mat image);

static cv::Point s_WindowSize;
static cv::Point s_WindowLocation;
static cv::Point s_WindowCenter;
static cv::Point s_FOVSize;
static cv::Point s_FOVCenter;
static cv::Point s_CaptureLocation;
static RECT s_WindowRect;

#define MAX_STR 255

int main(int argc, char* argv[])
{
    HWND windowHandle = FindWindow(NULL, L"VALORANT");
    if (windowHandle) GetWindowRect(windowHandle, &s_WindowRect);
    else
    {
        // If valorant is closed, just use the entire primary screen
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        s_WindowRect = { 0, 0, screenWidth, screenHeight };
        std::cout << "Valorant is closed, using primary screen instead." << std::endl;
    }
    
    // Define the region to capture
    s_WindowSize = { s_WindowRect.right - s_WindowRect.left, s_WindowRect.bottom - s_WindowRect.top };
    s_WindowLocation = { s_WindowRect.left, s_WindowRect.top };
    s_WindowCenter = s_WindowLocation + s_WindowSize / 2.0f;
    s_FOVSize = { 300, 300 };
    s_FOVCenter = s_FOVSize / 2.0f;
    s_CaptureLocation = s_WindowCenter - s_FOVCenter;

    // Initialize a screen capture
    Screen screen(s_FOVSize.x, s_FOVSize.y);

    hid_device* handle = hid_open(0x1038, 0x1858, L"CHIDLC");
    
    while (true)
    {
        // Capture screen data in an array of rgba values
        unsigned char* imageData = screen.CaptureScreen(s_CaptureLocation.x, s_CaptureLocation.y);

        // Convert the captured screen data to an opencv image
        cv::Mat image(s_FOVSize.y, s_FOVSize.x, CV_8UC4, imageData);

        // Check if the image was successfully loaded
        if (image.empty())
        {
            std::cerr << "Error: Couldn't load image!" << std::endl;
            return EXIT_FAILURE;
        }

        // Calculate the targeted enemy, if there are no enemies just do nothing
        cv::Point centerHead = FindEnemy(image);
        if (centerHead.x != 0 && centerHead.y != 0)
        {
            // Render a circle in the preview
            cv::circle(image, centerHead, 5, { 255, 255, 255 }, 1);

            // If the alt key is pressed, activate aimbot
            if (GetAsyncKeyState(VK_MENU) & 0x8000) 
            {
                // Calculate the location of the enemy relative the the crosshair
                cv::Point target = (centerHead - s_FOVCenter) / 1.5f;
                if (target.x > 127) target.x = 127;
                if (target.x < -128) target.x = -128;
                if (target.y > 127) target.y = 127;
                if (target.y < -128) target.y = -128;

                double distance = std::sqrt(target.x * target.x + target.y + target.y);
                if (distance < 5) {
                    unsigned char buf[3];
                    buf[0] = 0;
                    buf[1] = 'C';
                    buf[2] = '1';
                    hid_write(handle, buf, sizeof(buf));
                }

                {
                    unsigned char buf[4];
                    buf[0] = 0;
                    buf[1] = 'M';
                    buf[2] = target.x;
                    buf[3] = target.y;
                    hid_write(handle, buf, sizeof(buf));
                }
                std::cout << "Move: " << target.x << ", " << target.y << std::endl;
            }
        }

        cv::imshow("Image", image);

        // This is required for opencv to keep rendering new frames
        cv::waitKey(1);
    }

    cv::destroyAllWindows();
}

cv::Point FindEnemy(cv::Mat image)
{
    // Define the lower and upper bounds for the mask
    static cv::Scalar lowerBound = { 140, 110, 150 };
    static cv::Scalar upperBound = { 160, 200, 255 };

    // Convert the image from rgb to hsv
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    // Create a mask that only shows hsv values between the lower and upper bound
    cv::Mat mask;
    cv::inRange(hsv, lowerBound, upperBound, mask);

    // Make the pixels bigger in the shape of a triangle so that the contouring works better
    cv::Mat dilated;
    cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 
        0, 0, 0,
        1, 1, 1,
        0, 1, 0);
    cv::dilate(mask, dilated, kernel, cv::Point(-1, -1), 5);
    cv::imshow("dilated", dilated);

    // Find all the contours in the image which will turn a lot of pixels into big blobs
    // which can be parsed and evaluated individually
    std::vector<cv::Mat> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dilated, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    // Render the contours on a new window
    cv::Mat contoursImage = cv::Mat::zeros(dilated.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++)
        cv::drawContours(contoursImage, contours, static_cast<int>(i), cv::Scalar(255, 255, 255), 2, cv::LINE_8, hierarchy, 0);
    cv::imshow("Contours", contoursImage);

    // Find the best contour to track
    double bestScore = 0.0f;
    cv::Mat bestContour;
    for (const cv::Mat& contour : contours) 
    {
        cv::Rect rect = cv::boundingRect(contour);
        cv::Point center = cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);

        // Calculate the distance from the crosshair and the center of the contour
        double distance = std::sqrt((center.x - s_FOVCenter.x) * (center.x - s_FOVCenter.x) + (center.y - s_FOVCenter.y) * (center.y - s_FOVCenter.y));

        // Give a score to the contour based on the the y value and the distance of the contour relative to the crosshair
        // Prioritize the y value more than the distance
        double score = (s_FOVSize.y - rect.y) * 2 + (1131 - distance);
        if (bestScore < score)
        {
            bestScore = score;
            bestContour = contour;
        }
    }

    //cv::Moments mouse = cv::moments(closestContour);
    //cv::Point pixel = { (int)(mouse.m10 / mouse.m00), (int)(mouse.m01 / mouse.m00) };
    //return pixel;

    // Calculate the best location to target within the contour
    cv::Rect rect = cv::boundingRect(bestContour);
    int centerX = rect.x + rect.width / 2;
    int centerY = rect.y + 20;

    return { centerX, centerY };
}
