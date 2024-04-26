/**
 * @file CSketch.h
 * @brief Declaration of the CSketch class.
 *  
 * This file contains the declaration of the CSketch class for creating Etch-A-Sketch.
 * 
 * @author Faniel Yemane
 */

#pragma once

#include "CBase4618.h"

 /**
  * @class CSketch
  * @brief A class representing a simple sketch application.
  *
  * This class extends the CBase4618 class and adds functionality for sketching
  * based on user input and accelerometer readings.
  */
class CSketch : public CBase4618 {
private:
   int joyX, joyY;           ///< Joystick X and Y coordinates.
   int cursorX, cursorY;     ///< Cursor X and Y coordinates.
   int prevCursorX, prevCursorY;  ///< Previous cursor X and Y coordinates.
   int accZ;                 ///< Accelerometer Z-axis reading.
   int colorIndex = 0;       ///< Index representing the color used for drawing.
   bool resetFlag = false, exitFlag = false; ///< Flags for resetting and exiting the application.
   cv::Mat persistentCanvas; ///< Mat to store persistent drawing.
   int colorButtonPressed = 0;   ///< Counter for color button presses.
   int resetButtonPressed = 0;   ///< Counter for reset button presses.

   int prevAccZ = 0; ///< Store the previous accelerometer Z-axis reading.
   std::chrono::steady_clock::time_point prevTime; ///< Store the previous time.

public:
   /**
    * @brief Constructor for CSketch.
    * @param canvasSize The size of the canvas for drawing.
    * @param comPort The COM port for communication.
    */
   CSketch(const cv::Size& canvasSize, int comPort);

   /**
    * @brief Overridden function for updating the sketch state.
    */
   void update() override;

   /**
    * @brief Overridden function for rendering the sketch.
    */
   void draw() override;

   /**
    * @brief Checks for a shake gesture based on accelerometer readings.
    * @return True if a shake is detected, false otherwise.
    */
   bool checkForShake();

   /**
    * @brief Moves the cursor based on joystick input.
    */
   void move();
};
