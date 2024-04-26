/**
 * @file CBase4618.h
 * @brief Declaration of the CBase4618 class.
 * 
 * This file contains the declaration of the CBase4618 base class for creating apps.
 * 
 * @author Faniel Yemane
 */

#pragma once

#include "CControl.h"

 /**
  * @class CBase4618
  * @brief A base class for a simple graphics application.
  *
  * This class provides a basic framework for a graphics application, including a control
  * object for user input and a canvas for drawing.
  */
class CBase4618 {
protected:
   CControl _control; ///< The control object for user input.
   cv::Mat _canvas;   ///< The canvas for drawing.

public:
   /**
    * @brief Destructor for CBase4618.
    */
   virtual ~CBase4618() {}

   /**
    * @brief Pure virtual function for updating the application state.
    */
   virtual void update() = 0;

   /**
    * @brief Pure virtual function for rendering the application.
    */
   virtual void draw() = 0;

   /**
    * @brief Runs the graphics application.
    *
    * This function initializes the application and enters the main loop.
    */
   void run();
};
