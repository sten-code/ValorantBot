#include <fstream>
#include <iostream>
#pragma comment (lib, "OneCore.lib")
#include <Windows.h>
#include <chrono>
#include <limits>

#include "screen.h"
#include "serialport.h"

#include <opencv2/opencv.hpp>

bool ShowComPorts();
cv::Point FindEnemy(cv::Mat image);

void moveMouseRelative(int x, int y)
{
    // SetCursorPos ((int)point.x, (int)point.y);

    INPUT mouseInput;

    mouseInput.type = INPUT_MOUSE;
    mouseInput.mi.dx = x;
    mouseInput.mi.dy = y;
    mouseInput.mi.mouseData = 0;
    mouseInput.mi.time = 0;
    mouseInput.mi.dwFlags = MOUSEEVENTF_MOVE;
    mouseInput.mi.dwExtraInfo = 0;
    SendInput(1, &mouseInput, sizeof(mouseInput));
}

static cv::Point s_ScreenSize;
static cv::Point s_CenterScreen;
static cv::Point s_FOVSize;
static cv::Point s_CenterFOV;
static cv::Point s_CaptureLocation;

int main(int argc, char* argv[])
{
    // Define the region to capture
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    s_ScreenSize = { screenWidth, screenHeight };
    s_CenterScreen = s_ScreenSize / 2.0f;
    s_FOVSize = { 200, 200 };
    s_CenterFOV = s_FOVSize / 2.0f;
    s_CaptureLocation = s_CenterScreen - s_CenterFOV;

    Screen screen(s_FOVSize.x, s_FOVSize.y);
    SerialPort port;
    port.Open("COM3", 115200);

    while (true)
    {
        unsigned char* imageData = screen.CaptureScreen(s_CaptureLocation.x, s_CaptureLocation.y);

        cv::Mat image(s_FOVSize.y, s_FOVSize.x, CV_8UC4, imageData);

        // Check if the image was successfully loaded
        if (image.empty())
        {
            std::cerr << "Error: Couldn't load image!" << std::endl;
            return EXIT_FAILURE;
        }

        cv::Point centerHead = FindEnemy(image);
        if (centerHead.x != 0 && centerHead.y != 0)
        {
            if (GetAsyncKeyState(VK_MENU) & 0x8000) 
            {
                cv::Point target = centerHead - s_CenterFOV;
                double distance = std::sqrt(target.x * target.x + target.y * target.y);
                std::cout << "Distance: " << distance << std::endl;
                if (distance < 20)
                {
                    //port.Write("FIRE\n");
                    //std::cout << "FIRE" << std::endl;
                }
                std::stringstream ss;
                ss << (target.x / 2.0f) << "," << (target.y / 2.0f) << std::endl;
                port.Write(ss.str());
                std::cout << ss.str();
            }
            cv::circle(image, centerHead, 10, { 255, 255, 255 }, 3);
        }

        cv::imshow("Image", image);
        cv::waitKey(1);
    }

    cv::destroyAllWindows();
}

cv::Point FindEnemy(cv::Mat image)
{
    // Define the lower and upper bounds for the mask
    static cv::Scalar lowerBound = { 140, 110, 150 };
    static cv::Scalar upperBound = { 150, 195, 255 };

    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    cv::Mat mask;
    cv::inRange(hsv, lowerBound, upperBound, mask);

    cv::Mat dilated;
    cv::Mat kernel = (cv::Mat_<uchar>(3, 3) << 
        0, 0, 0,
        1, 1, 1,
        0, 1, 0);
    cv::dilate(mask, dilated, kernel, cv::Point(-1, -1), 5);

    cv::Mat thresh;
    cv::threshold(dilated, thresh, 60, 255, cv::THRESH_BINARY);

    std::vector<cv::Mat> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(thresh, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    // Create a blank image to draw contours
    cv::Mat contoursImage = cv::Mat::zeros(thresh.size(), CV_8UC3);

    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::drawContours(contoursImage, contours, static_cast<int>(i), cv::Scalar(255, 255, 255), 2, cv::LINE_8, hierarchy, 0);
    }

    cv::imshow("Dilated", dilated);
    cv::imshow("Contours", contoursImage);

    //double maxArea = 0.0;
    //cv::Mat contour;
    //for (const cv::Mat& mat : contours)
    //{
    //    double area = cv::contourArea(mat);
    //    if (maxArea < area)
    //    {
    //        contour = mat;
    //        maxArea = area;
    //    }
    //}

    //double minDist = std::numeric_limits<double>::max();
    //cv::Mat closestContour;
    //for (const cv::Mat& contour : contours) 
    //{
    //    cv::Rect rect = cv::boundingRect(contour);
    //    cv::Point center = cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
    //    double distance = std::sqrt((center.x - s_CenterFOV.x) * (center.x - s_CenterFOV.x) + (center.y - s_CenterFOV.y) * (center.y - s_CenterFOV.y));
    //    if (distance < minDist) {
    //        minDist = distance;
    //        closestContour = contour;
    //    }
    //}

    double bestScore = 0.0f;
    cv::Mat closestContour;
    for (const cv::Mat& contour : contours) 
    {
        cv::Rect rect = cv::boundingRect(contour);
        cv::Point center = cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);

        double distance = std::sqrt((center.x - s_CenterFOV.x) * (center.x - s_CenterFOV.x) + (center.y - s_CenterFOV.y) * (center.y - s_CenterFOV.y));
        double score = (s_FOVSize.y - rect.y) + (282 - distance);
        if (bestScore < score)
        {
            bestScore = score;
            closestContour = contour;
        }
    }

    //cv::Moments mouse = cv::moments(closestContour);
    //cv::Point pixel = { (int)(mouse.m10 / mouse.m00), (int)(mouse.m01 / mouse.m00) };
    //return pixel;

    cv::Rect rect = cv::boundingRect(closestContour);
    int centerX = rect.x + rect.width / 2;
    int centerY = rect.y + 10;

    return { centerX, centerY };
}

bool ShowComPorts()
{
    ULONG portNumbers[100];
    ULONG portsFound;
    ULONG status = GetCommPorts(portNumbers, 100, &portsFound);

    if (status == ERROR_SUCCESS)
    {
        std::cout << "Found " << portsFound << " port(s)." << std::endl;
        for (auto i = 0; i < portsFound; i++) {
            std::cout << "COM" << portNumbers[i] << std::endl;;
        }
        return true;
    }
    else if (status == ERROR_FILE_NOT_FOUND)
    {
        std::cout << "There are no COM ports connected!" << std::endl;
    }
    else
    {
        std::cout << "An error occurred!" << std::endl;
    }
    return false;
}
