#include <opencv2/opencv.hpp>
#include <ntcore.h>
#include <networktables/NetworkTable.h>

using namespace cv;

void debugMode();

int lowH = 11;
int highH = 86;

int lowS = 69;
int highS = 255;

int lowV = 113;
int highV = 255;

int cannyThresh = 0;

int main(int argc, char **argv)
{

  int isDebugging = 1;

  VideoCapture capture(0);
  Size size;
  Mat rawFrame;
  Mat hsvFrame;
  Mat thresholdFrame;
  Mat contourFrame;

  Scalar hsvMin(lowH, lowS, lowV);
  Scalar hsvMax(highH, highS, highV);

  //grab frame from the Camera and store its size
  capture >> rawFrame;
  int height = rawFrame.size().height;
  int width = rawFrame.size().width;
  size = Size(width, height);

  hsvFrame = Mat(size, CV_8UC3);
  thresholdFrame = Mat(size, CV_8UC1);
  contourFrame = Mat(size, CV_8UC1);

  namedWindow("contour");

  if (isDebugging)
  {
    debugMode();
  }

  while (true)
  {
    capture >> rawFrame;

    cvtColor(rawFrame, hsvFrame, CV_BGR2HSV);
    inRange(hsvFrame, hsvMin, hsvMax, thresholdFrame);

    //open the image (scale down and then back up) this removes small noise and smaller yellow reflections
    morphologyEx(thresholdFrame, thresholdFrame, 2, getStructuringElement(MORPH_RECT, Size(10, 10), Point(-1, -1)));

    //find contours in the image
    vector<vector<Point>> initContours;
    vector<Vec4i> hierarchy;
    Canny(thresholdFrame, contourFrame, cannyThresh, cannyThresh * 3, 3);
    findContours(contourFrame, initContours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    //filter out bad contours to find only the largest one on screen, store its index in latestIndex varible
    int largestSize = -1;
    int latestIndex = -1;

    for (int i = 0; i < initContours.size(); i++)
    {
      int size = contourArea(initContours[i]);
      if (size > largestSize)
      {
        largestSize = size;
        latestIndex = i;
      }
    }

    //if we have a contour that satisfies requirements, draw a green outline around it
    if (latestIndex != -1)
      drawContours(rawFrame, initContours, latestIndex, Scalar(0, 255, 0), 3);

    //update min and max values from sliders
    hsvMin = Scalar(lowH, lowS, lowV);
    hsvMax = Scalar(highH, highS, highV);

    //display images
    imshow("raw", rawFrame);
    if (isDebugging)
    {
      imshow("contour", contourFrame);
    }

    //check if ESC is pressed to exit the program;
    if ((cvWaitKey(10) & 255) == 27)
      break;
  }

  return 0;
}

void debugMode()
{
  namedWindow("Control", WINDOW_AUTOSIZE);

  //make trackbars to control the HSV min max values
  createTrackbar("lowH", "Control", &lowH, 360);
  createTrackbar("highH", "Control", &highH, 360);
  createTrackbar("lowS", "Control", &lowS, 255);
  createTrackbar("highS", "Control", &highS, 255);
  createTrackbar("lowV", "Control", &lowV, 255);
  createTrackbar("highV", "Control", &highV, 255);
  createTrackbar("Canny", "Control", &cannyThresh, 255);
}
