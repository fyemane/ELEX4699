/**
 * @file CRecyclingSort.h
 * @brief Declaration of the CReCycling Sort class.
 *
 * This file contains the declaration of the CRecyclingSort classs.
 *
 * @author Faniel Yemane
 */

#pragma once

#include "CBase4618.h"
#include "server.h"

 /**
  * @class CRecyclingSort
  * @brief A class representing a simple recycling application.
  *
  * This class extends the CBase4618 class and adds functionality for sorting recycling
  */
class CRecyclingSort : public CBase4618 {
private:
   cv::Mat persistentCanvas; ///< Mat to store persistent drawing.
   cv::Point gui_position; ///< Position for graphical user interface.
   int joyThreshold; ///< Threshold for joystick input.

   cv::Mat vidFrame, resizedFrame;
   cv::Mat processedFrame1, processedFrame2, processedFrame3, processedFrame4;
   cv::VideoCapture vid;
   cv::Size m_canvasSize;

   cv::Scalar color1_upperBound, color1_lowerBound;
   cv::Scalar color2_upperBound, color2_lowerBound;
   cv::Scalar color3_upperBound, color3_lowerBound;
   cv::Scalar color4_upperBound, color4_lowerBound;

   std::vector<cv::Scalar> lowerBounds;
   std::vector<cv::Scalar> upperBounds;

   int color1_lowerH, color1_lowerS, color1_lowerV, color1_upperH, color1_upperS, color1_upperV;
   int color2_lowerH, color2_lowerS, color2_lowerV, color2_upperH, color2_upperS, color2_upperV;
   int color3_lowerH, color3_lowerS, color3_lowerV, color3_upperH, color3_upperS, color3_upperV;
   int color4_lowerH, color4_lowerS, color4_lowerV, color4_upperH, color4_upperS, color4_upperV;


   double fps; ///< Frames per second.
   int delay; ///< Delay for fps

   bool resetFlag, exitFlag; ///< Flags for resetting and exiting the application.
   bool _thread_exit, _mutex_thread_exit; ///< Flag for thread exit.
   std::mutex colorToSort_mutex;

   int buttonPressed1, buttonValue1;   ///< when button is pressed

   int buttonPressed2, buttonValue2;
   int buttonPressed3, buttonValue3;
   int buttonPressed4, buttonValue4;
   int buttonPressed5, buttonValue5;
   int servorun;

   int color1BallCount;
   int color2BallCount;
   int color3BallCount;
   int color4BallCount;

   bool color1BallDetected;
   bool color2BallDetected;
   bool color3BallDetected;
   bool color4BallDetected;
   bool anyBallDetected;
   bool autosort;

   int colorToSort;

   int servo1position, servo2position;
   int servoincrement;
   int servo1target;
   int servo2target1, servo2target2, servo2target3, servo2target4;

   int min_area_threshold;
   int gui_vertspace;

   CServer sort_server;
   std::vector<std::string> commands_list;


public:
   /**
    * @brief Constructor for CRecyclingSort.
    * @param canvasSize The size of the canvas for drawing.
    * @param comPort The COM port for communication.
    */
   CRecyclingSort(const cv::Size& canvasSize, int comPort);

   ~CRecyclingSort();

   /**
    * @brief Overridden function for updating the sketch state.
    */
   void update() override;

   /**
    * @brief Overridden function for rendering the sketch.
    */
   void draw() override;

   void handleUserInput();

   cv::Mat processImage(cv::Mat& image, cv::Scalar lowerBound, cv::Scalar upperBound);

   void drawBorder(cv::Mat& canvas, cv::Rect& imageRegion);

   void drawImage(cv::Mat& canvas, cv::Point position, cv::Mat& image, int color = 0);

   void resizeImage(cv::Mat& image, int targetWidth, cv::Mat& resizedImage);

   void sortColorBin();

   void servercommands();




   /**
    * @brief Thread function for updating the sketch state.
    * @param ptr Pointer to CPong object.
    */
   static void update_thread(CRecyclingSort* ptr);

   /**
    * @brief Thread function for rendering the sketch.
    * @param ptr Pointer to CRecyclingSort object.
    */
   static void draw_thread(CRecyclingSort* ptr);

   static void servo_thread(CRecyclingSort* ptr);

   static void serverstart_thread(CRecyclingSort* ptr);

   static void servercommands_thread(CRecyclingSort* ptr);

   /**
    * @brief Function to run the sketch.
    */
   void run();
};
