////////////////////////////////////////////////////////////////
// ELEX 4699  project for BCIT
// Created April 23, 2024 by Raghav Marwah and Faniel Yemane
// Last updated April 23, 2024
////////////////////////////////////////////////////////////////
// Add simple GUI elements
#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <unistd.h>

#include "CControlPi.h"
#include "CCar.h"

#define CANVAS_NAME "Display Image"

////////////////////////////////////////////////////////////////
// Generate ARUCO markers
///////////////////////// ///////////////////////////////////////
void generate_marks()
{
//  std::string str;
//  cv::Mat im;
//  int mark_size = 250;
//
//  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
//
//  for (int marker_id = 0; marker_id < 250; marker_id++)
//  {
//    cv::aruco::drawMarker(dictionary, marker_id, mark_size, im, 1);
//    str = "mark" + std::to_string(marker_id) + ".png";
//    cv::imwrite(str, im);
//  }
}


////////////////////////////////////////////////////////////////
// Display Image on screen
////////////////////////////////////////////////////////////////
void do_image()
{
  cv::Mat im;

  // initialize GUI system
  cvui::init(CANVAS_NAME);
  cv::Point gui_position;

  // Load test image
  im = cv::imread("BCIT.jpg");

  // Seed random number generator with time
  srand(time(0));

  // Draw 500 circles and dots on the image
  for (int i = 0; i < 500; i++)
  {
    gui_position = cv::Point(10, 10);
    cvui::window(im, gui_position.x, gui_position.y, 200, 40, "Image Test");
    gui_position += cv::Point(5, 25);
    cvui::text(im, gui_position.x, gui_position.y, "Number of Circles: " + std::to_string(i));

    float radius = 50 * rand() / RAND_MAX;
    cv::Point center = cv::Point(im.size().width*rand() / RAND_MAX, im.size().height*rand() / RAND_MAX);

    cv::circle(im, center, radius, cv::Scalar(200, 200, 200), 1, cv::LINE_AA);

    im.at<char>(i,i) = 255;

    // Update the CVUI GUI system
    cvui::update();

    // Show image and delay (all display image to update)
    cv::imshow(CANVAS_NAME, im);
    cv::waitKey(1);
  }
}

////////////////////////////////////////////////////////////////
// Display Video on screen
////////////////////////////////////////////////////////////////
void do_video()
{
  cv::VideoCapture vid;

  vid.open(0, cv::CAP_DSHOW);

  bool do_canny = true;
  bool do_aruco = false;
  int canny_thresh = 30;
  bool do_exit = false;


  // initialize GUI system
  cvui::init(CANVAS_NAME);
  cv::Point gui_position;

  std::vector<cv::Scalar> color_vec;
  color_vec.push_back(cv::Scalar(255, 255, 255));
  color_vec.push_back(cv::Scalar(255, 0, 0));
  color_vec.push_back(cv::Scalar(0, 255, 0));
  color_vec.push_back(cv::Scalar(0, 0, 255));
  int color_index = 0;

  if (vid.isOpened() == 1)
  {
    do
    {
      cv::Mat frame, edges;

      // Capture video frame
      vid >> frame;

      // Make sure video frame exists
      if (frame.empty() == false)
      {
        // ARUCO marker tracking


        // GUI Menu
        gui_position = cv::Point(10, 10);
        cvui::window(frame, gui_position.x, gui_position.y, 200, 190, "Video Test");
        gui_position += cv::Point(5, 25);
        gui_position += cv::Point(120, 0);
        if (cvui::button(frame, gui_position.x, gui_position.y, 50, 30, "Exit"))
        {
          do_exit = true;
        }

        // Update the CVUI GUI system
        cvui::update();
        cv::imshow(CANVAS_NAME, frame);
      }
    }
    while (cv::waitKey(1) != 'q' && do_exit == false);
  }
}

void lab10() {
    CCar mycar;
    mycar.run();
}

void print_menu()
{
	std::cout << "\n***********************************";
	std::cout << "\n* ELEX4618 Template Project";
	std::cout << "\n***********************************";
  std::cout << "\n(1) Lab 10 - User Input";
	std::cout << "\n(0) Exit";
	std::cout << "\nCMD> ";
}

int main()
{
  // generate_marks();

	int cmd = -1;
	do
	{
		print_menu();
		std::cin >> cmd;
		switch (cmd)
		{

      case 1: lab10(); break;
      }
	} while (cmd != 0);
}
