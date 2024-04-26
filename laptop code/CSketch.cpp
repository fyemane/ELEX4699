// CSketch.cpp
#pragma once
#include <algorithm>
#include "stdafx.h"
#include "cvui.h"
#include "CControl.h"
#include "CSketch.h"

int redLedChannel{ 39 }, greenLedChannel{ 38 }, blueLedChannel{ 37 };
int colorButtonChannel = 32;  // Adjust to the actual digital input channel for the pushbutton
int resetButtonChannel = 33;  // Adjust to the actual digital input channel for the reset button
int accZChannel = 25;

CSketch::CSketch(const cv::Size& canvasSize, int comPort) : colorIndex(0), resetFlag(false) {
   _canvas = cv::Mat::zeros(canvasSize, CV_8UC3);
   persistentCanvas = cv::Mat::zeros(canvasSize, CV_8UC3);  // Initialize persistent canvas
   cvui::init("Etch-A-Sketch");  // Initialize cvui here
   _control.init_com(comPort);

   prevCursorX = cursorX = _canvas.rows / 2;
   prevCursorY = cursorY = _canvas.cols / 2;

   _control.get_analog(2, joyX);  // Assuming analog input channel 2 for X
   _control.get_analog(26, joyY);  // Assuming analog input channel 26 for Y
   _control.get_analog(accZChannel, accZ);
   prevAccZ = accZ;
}

void CSketch::update() {
   // Read X and Y position values as a percentage from CControl
   _control.get_analog(2, joyX);  // Assuming analog input channel 2 for X
   _control.get_analog(26, joyY);  // Assuming analog input channel 26 for Y
   _control.get_analog(accZChannel, accZ);

   //// Calculate canvas coordinates based on percentages
   //cursorX = static_cast<int>((joyX / 4096.0) * _canvas.cols);
   //cursorY = _canvas.rows - static_cast<int>((joyY / 4096.0) * _canvas.rows);

   // Update previous values for the next iteration
   prevAccZ = accZ;
   prevTime = std::chrono::steady_clock::now();

   CSketch::move();

   // Read debounced pushbutton for color cycling
   int colorButtonValue = 0;
   _control.get_digital(colorButtonChannel, colorButtonValue);
   // Check if the button is pressed and was not pressed in the previous cycle
   if (!colorButtonValue && colorButtonPressed) {
      // Button is pressed, cycle through colors
      colorIndex = (colorIndex + 1) % 7;
      colorButtonPressed = 0;
   }
   // Update the flag for the next cycle
   if (colorButtonValue)
      colorButtonPressed = 1;


   // Read debounced pushbutton for reset
   int resetButtonValue = 0;
   _control.get_button(resetButtonChannel, resetButtonValue);
   if (!resetButtonValue && resetButtonPressed) {
      // Button is pressed, set reset flag
      resetFlag = true;
      resetButtonPressed = 0;
   }
   if (resetButtonValue)
      resetButtonPressed = 1;

   // Check for shake (hypothetical function, implement based on your accelerometer data)
   if (checkForShake()) {
      resetFlag = true;
   }
}

void CSketch::draw() {
   // Draw on the canvas based on cursorX and cursorY with the current color
   cv::Scalar currentColor;
   int ledChannel; // Adjust to the actual digital output channel for the multi-color LED
   std::string colors[7] = { "RED", "GREEN", "BLUE", "CYAN", "YELLOW", "MAGENTA", "WHITE"};
   int led_values[7][3] = { {0, 0, 1}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 1}, {1, 0, 1}, {1, 1, 1} };
   cv::Point gui_position = cv::Point(10, 10);

   // Check if colorIndex is within the valid range
   if (colorIndex >= 0 && colorIndex < 7) {
      // Set the current color based on the colorIndex
      currentColor = cv::Scalar(led_values[colorIndex][0] * 255,
      led_values[colorIndex][1] * 255,
      led_values[colorIndex][2] * 255);

      // Set the corresponding LED channels
      _control.set_digital(blueLedChannel, led_values[colorIndex][0]);
      _control.set_digital(greenLedChannel, led_values[colorIndex][1]);
      _control.set_digital(redLedChannel, led_values[colorIndex][2]);
   }

   // Set the main canvas to a white background
   _canvas = cv::Scalar(0, 0, 0); // Set to white background

   // Draw on the persistentCanvas based on cursorX and cursorY with the current color
   cv::circle(persistentCanvas, cv::Point(cursorX, cursorY), 0, currentColor, -1);

   // Draw a line between the previous and current cursor positions
   if (prevCursorX >= 0 && prevCursorY >= 0) {
      cv::line(persistentCanvas, cv::Point(prevCursorX, prevCursorY), cv::Point(cursorX, cursorY), currentColor, 2);
   }

   // Update the previous cursor position
   prevCursorX = cursorX;
   prevCursorY = cursorY;

   // Draw on the main canvas
   _canvas = persistentCanvas.clone();  // Copy persistent canvas to main canvas 

   // Display the canvas image
   // Draw GUI elements in the top-left corner
   cvui::window(_canvas, gui_position.x, gui_position.y, 150, 100, "Etch-A-Sketch");
   cvui::text(_canvas, gui_position.x + 10, gui_position.y + 30, "Color: " + colors[colorIndex]);
   
   if (cvui::button(_canvas, gui_position.x + 80, gui_position.y + 50, "Exit")) {
      std::exit(0);
   }

   if (cvui::button(_canvas, gui_position.x + 10, gui_position.y + 50, "Reset")) {
      // Reset button pressed, set reset flag
      resetFlag = true;
   }

   // Check and handle the reset flag
   if (resetFlag) { 
      persistentCanvas = cv::Mat::zeros(_canvas.size(), CV_8UC3);  // Clear the persistent canvas
      resetFlag = false;  // Reset the flag
   }

   // Display the canvas on the window
   if (!exitFlag) {
      // Update the CVUI GUI system
      cvui::update();
      cv::imshow("Etch-A-Sketch", _canvas);
   }
}


bool CSketch::checkForShake() {
   // Read current time
   auto currentTime = std::chrono::steady_clock::now();

   // Calculate time difference in seconds
   auto timeDiff = std::chrono::duration_cast<std::chrono::seconds>(currentTime - prevTime).count();

   // Assuming a short period is 1 second
   if (timeDiff < 1) {
      // Calculate the change in accelerometer Z-axis reading
      int accZ;
      _control.get_analog(accZChannel, accZ);
      int accZChange = std::abs(accZ - prevAccZ);

      // Assuming a significant change threshold is 100 (adjust based on your data)
      if (accZChange > 500) {
         return true;  // Shake detected
      }
   }

   return false;  // No shake detected
}

void CSketch::move() {
   // Assuming 4096 as the maximum value for joystick input
   const int joyThreshold = 2048;  // You may need to adjust this threshold based on your joystick's characteristics

   int incrementX = (joyX - 2048) / 200;
   int incrementY = -1 * (joyY - 2048) / 200;

   // Check if the joystick is moved up or down
   if (joyY < joyThreshold - 300 || joyY > joyThreshold + 300) {
      cursorY += incrementY;  // Move the cursor up
   }   
   // Check if the joystick is moved up or down
   if (joyX > joyThreshold + 300 || joyX < joyThreshold - 300) {
      cursorX += incrementX;  // Move the cursor right
   }

   // border checking
   if (cursorX < 0)
      cursorX = 0;
   if (cursorX > _canvas.cols)
      cursorX = _canvas.cols;
   if (cursorY < 0)
      cursorY = 0;
   if (cursorY > _canvas.rows)
      cursorY = _canvas.rows;
}
