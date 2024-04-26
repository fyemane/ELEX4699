/**
 * @file CSketch.h
 * @brief Declaration of the CSketch class.
 *
 * This file contains the declaration of the CPong class, which represents a simple sketch application.
 * It extends the CBase4618 class and adds functionality for sketching based on user input and accelerometer readings.
 *
 * @author Faniel Yemane
 */

#pragma once

#include "CBase4618.h"

 /**
  * @class CPong
  * @brief A class representing a simple sketch application.
  *
  * This class extends the CBase4618 class and adds functionality for sketching
  * based on user input and accelerometer readings.
  */
class CPong : public CBase4618 {
private:
   cv::Point joystick; ///< Current joystick position.
   bool resetFlag, exitFlag; ///< Flags for resetting and exiting the application.
   cv::Mat persistentCanvas; ///< Mat to store persistent drawing.
   int resetButtonPressed;   ///< Counter for reset button presses.
   cv::Point gui_position; ///< Position for graphical user interface.
   int joyThreshold; ///< Threshold for joystick input.

   cv::Rect leftPaddle; ///< Rectangle representing the left paddle.
   cv::Rect rightPaddle; ///< Rectangle representing the right paddle.
   cv::Point ballPos; ///< Position of the ball.
   cv::Point ballVelocity; ///< Velocity of the ball.
   int ballSpeed; ///< Speed of the ball.
   int ballRadius; ///< Radius of the ball.

   double fps; ///< Frames per second.
   int delay; ///< Delay for fps
   bool _thread_exit; ///< Flag for thread exit.
   bool endGame; ///< Flag indicating the end of the game.

   int playerScore; ///< Player's score.
   int computerScore; ///< Computer's score.

   std::chrono::high_resolution_clock::time_point lastUpdateTime; ///< Variable to store the last update time.


public:
   /**
    * @brief Constructor for CPong.
    * @param canvasSize The size of the canvas for drawing.
    * @param comPort The COM port for communication.
    */
   CPong(const cv::Size& canvasSize, int comPort);

   /**
    * @brief Overridden function for updating the sketch state.
    */
   void update() override;

   /**
    * @brief Overridden function for rendering the sketch.
    */
   void draw() override;

   /**
    * @brief Thread function for updating the sketch state.
    * @param ptr Pointer to CPong object.
    */
   static void update_thread(CPong* ptr);

   /**
    * @brief Thread function for rendering the sketch.
    * @param ptr Pointer to CPong object.
    */
   static void draw_thread(CPong* ptr);

   /**
    * @brief Function to run the sketch.
    */
   void run();

private:
   /**
    * @brief Update the paddle position based on joystick input.
    * @param paddle The paddle rectangle.
    * @param joyY The y-coordinate of the joystick.
    */
   void updatePaddle(cv::Rect& paddle, int joyY);

   /**
    * @brief Update the computer-controlled paddle position.
    * @param paddle The paddle rectangle.
    */
   void updateCPaddle(cv::Rect& paddle);

   /**
    * @brief Update the position of the ball.
    */
   void updateBall();

   /**
    * @brief Draw the paddle on the canvas.
    * @param canvas The canvas to draw on.
    * @param paddle The paddle rectangle.
    */
   void drawPaddle(cv::Mat& canvas, cv::Rect& paddle);

   /**
    * @brief Draw the ball on the canvas.
    * @param canvas The canvas to draw on.
    * @param ballPos The position of the ball.
    */
   void drawBall(cv::Mat& canvas, cv::Point& ballPos);
};
