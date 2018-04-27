#include <opencv2/opencv.hpp>

using namespace cv;

void debugMode();

int lowH = 11;
int highH = 86;

int lowS = 148;
int highS = 255;

int lowV = 113;
int highV = 255;

int main(int argc, char **argv)
{

  int isDebugging = 0;
  
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
  
  if(isDebugging)
  {
    debugMode();
  }
  
  while(true)
  {
    capture >> rawFrame;
    
    //Change into HLS colorspace for better filtering
    cvtColor(rawFrame, hsvFrame, CV_BGR2HSV);
    inRange(hsvFrame, hsvMin, hsvMax, thresholdFrame);

    //find contours in the image
    vector<vector<Point>> initContours;
    
    vector<Vec4i> hierarchy;
    Canny(thresholdFrame, contourFrame, 255, 255, 3);
    findContours(contourFrame, initContours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));

    int largestSize = 10;
    int latestIndex = -1;
    
    for(int i = 0;i < initContours.size(); i++)
    {
      int size = contourArea(initContours[i]);
      if(size > largestSize)
      {
	largestSize = size;
	latestIndex = i;
      }
    }

    if(latestIndex != -1) 
      drawContours(rawFrame, initContours, latestIndex, Scalar(0, 255, 0), 3);
    
    hsvMin = Scalar(lowH, lowS, lowV);
    hsvMax = Scalar(highH, highS, highV);
    
    imshow("raw", rawFrame);
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
  createTrackbar("lowH", "Control", &lowH, lowH);
  createTrackbar("highH", "Control", &highH, highH);
  createTrackbar("lowS", "Control", &lowS, lowS);
  createTrackbar("highS", "Control", &highS, highS);
  createTrackbar("lowV", "Control", &lowV, lowV);
  createTrackbar("highV", "Control", &highV, highV);
}
