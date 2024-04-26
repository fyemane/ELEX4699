// CPong.cpp
#include <algorithm>
#include <chrono>
#include <thread>
#include "stdafx.h"
#include "cvui.h"
#include "CPong.h"

const int PADDLE_WIDTH = 10;
const int PADDLE_HEIGHT = 80;

CPong::CPong(const cv::Size& canvasSize, int comPort) {
   _canvas = cv::Mat::zeros(canvasSize, CV_8UC3);
   persistentCanvas = cv::Mat::zeros(canvasSize, CV_8UC3);  // Initialize persistent canvas
   cvui::init("Pong");  // Initialize cvui here
   _control.init_com(comPort);

   gui_position = cv::Point(700, 10);
   resetFlag = false, exitFlag = false, _thread_exit = false, endGame = false;
   resetButtonPressed = 0;
   ballRadius = 10;
   ballSpeed = 100;
   joyThreshold = 2048;

   leftPaddle = cv::Rect(0, _canvas.rows / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT);
   rightPaddle = cv::Rect(_canvas.cols - PADDLE_WIDTH, _canvas.rows / 2 - PADDLE_HEIGHT / 2, PADDLE_WIDTH, PADDLE_HEIGHT);
   ballPos = cv::Point(_canvas.cols / 2, _canvas.rows / 2);
   ballVelocity = cv::Point(1, 1);
   fps = 30;
   delay = 27;

   playerScore = 0;
   computerScore = 0;

   lastUpdateTime = std::chrono::high_resolution_clock::now();
}


void CPong::update() {


   if (!endGame) {
      // Read X and Y position values as a percentage from CControl
      _control.get_analog(JOYSTICK_X, joystick.x);  // Assuming analog input channel 2 for X
      _control.get_analog(JOYSTICK_Y, joystick.y);  // Assuming analog input channel 26 for Y

      // Update player paddle position (simulate potentiometer input)
      updatePaddle(leftPaddle, joystick.y);
      updateCPaddle(rightPaddle);

      // Update ball position
      updateBall();

      // Check for game end condition
      if (playerScore >= 5 || computerScore >= 5) {
         // End game
         // Implement end game logic here
         endGame = true; // Set endGame flag to true to indicate game end
      }
   }

   // Read debounced pushbutton for reset
   int resetButtonValue = 0;
   _control.get_button(BUTTON1, resetButtonValue);
   if (!resetButtonValue && resetButtonPressed) {
      // Button is pressed, set reset flag only if the game has ended
      resetFlag = true;
      resetButtonPressed = 0;
   }
   if (resetButtonValue)
      resetButtonPressed = 1;

   // Check if reset flag is true
   if (resetFlag) {
      // Reset scores and paddle positions
      playerScore = 0;
      computerScore = 0;
      leftPaddle.y = _canvas.rows / 2 - PADDLE_HEIGHT / 2;
      rightPaddle.y = _canvas.rows / 2 - PADDLE_HEIGHT / 2;
      ballPos = cv::Point(_canvas.cols / 2, _canvas.rows / 2);
      resetFlag = false; // Reset the reset flag
      endGame = false; // Reset the endGame flag
   }
}


void CPong::draw() {
   auto calc_start = std::chrono::steady_clock::now();
   // Set update rate at 30 Hz (100 ms per loop)
   auto end_time = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);

   _canvas = persistentCanvas.clone();  // clear canvas

   // Draw "Game Over" text if the game has ended
   if (endGame) {
      cv::putText(_canvas, "Game Over", cv::Point(_canvas.cols / 2 - 95, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
   }

   // draw a line
   cv::line(_canvas, cv::Point(_canvas.cols / 2, 0), cv::Point(_canvas.cols / 2, _canvas.rows), cv::Scalar(255, 255, 255), 1);

   // Draw game elements
   drawPaddle(_canvas, leftPaddle);
   drawPaddle(_canvas, rightPaddle);
   drawBall(_canvas, ballPos);

   // Display the canvas image
   // Draw GUI elements in the top-left corner
   cvui::window(_canvas, gui_position.x, gui_position.y, 250, 220, "Pong (FPS = " + std::to_string(fps) + ")");
   cvui::text(_canvas, gui_position.x + 10, gui_position.y + 30, "Player: " + std::to_string(playerScore));
   cvui::text(_canvas, gui_position.x + 150, gui_position.y + 30, "Computer: " + std::to_string(computerScore));
   cvui::text(_canvas, gui_position.x + 100, gui_position.y + 60, "Radius");
   cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + 60, 220, &ballRadius, 5, 100);
   cvui::text(_canvas, gui_position.x + 100, gui_position.y + 120, "Speed");
   cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + 120, 220, &ballSpeed, 100, 400);

   if (cvui::button(_canvas, gui_position.x + 130, gui_position.y + 180, "Exit")) {
      std::exit(0);
   }

   if (cvui::button(_canvas, gui_position.x + 50, gui_position.y + 180, "Reset")) {
      // Reset button pressed, set reset flag
      resetFlag = true;
   }

   // Display the canvas on the window
   if (!exitFlag) {
      // Update the CVUI GUI system
      cvui::update();
      cv::imshow("Pong", _canvas);
   }

   // Sleep if time remaining
   std::this_thread::sleep_until(end_time);

   auto calc_end = std::chrono::steady_clock::now();
   auto calc_elapsed = std::chrono::duration<double>(calc_end - calc_start);
   fps = 1 / calc_elapsed.count();
   std::cout << "FPS: " << fps << std::endl;
}

void CPong::updatePaddle(cv::Rect& paddle, int joyY) {
   // Get the current time
   auto currentTime = std::chrono::high_resolution_clock::now();

   // Calculate the time difference since the last update
   std::chrono::duration<double> timeDiff = currentTime - lastUpdateTime;
   double deltaTime = timeDiff.count(); // Delta time in seconds

   // Update paddle position
   // Check if the joystick is moved up or down
   if (joyY < joyThreshold - .2 * joyThreshold)
      paddle.y += 10;
   if (joyY > joyThreshold + .2 * joyThreshold)
      paddle.y -= 10;

   // Ensure paddle stays within bounds
   if (paddle.y < 0) {
      paddle.y = 0;
   }
   else if (paddle.y + PADDLE_HEIGHT > _canvas.rows) {
      paddle.y = _canvas.rows - PADDLE_HEIGHT;
   }
}

void CPong::updateCPaddle(cv::Rect& paddle) {
   // Update paddle position
   // Check if the joystick is moved up or down
   if (paddle.y + PADDLE_HEIGHT / 2 <= ballPos.y)
      paddle.y += 10;
   if (paddle.y + PADDLE_HEIGHT / 2 > ballPos.y)
      paddle.y -= 10;

   // Ensure paddle stays within bounds
   if (paddle.y < 0) {
      paddle.y = 0;
   }
   else if (paddle.y + PADDLE_HEIGHT > _canvas.rows) {
      paddle.y = _canvas.rows - PADDLE_HEIGHT;
   }
}

void CPong::updateBall() {
   // Update ball position based on elapsed time and velocity
   ballPos.x += ballVelocity.x * ballSpeed / fps;
   ballPos.y += ballVelocity.y * ballSpeed / fps;

   // Check for collisions with top wall
   if (ballPos.y <= ballRadius) {
      ballPos.y = ballRadius;
      ballVelocity.y = -ballVelocity.y; // Reflect ball off top wall
   }

   // Check for collisions with bottom wall
   if (ballPos.y >= _canvas.rows - ballRadius) {
      ballPos.y = _canvas.rows - ballRadius;
      ballVelocity.y = -ballVelocity.y; // Reflect ball off bottom wall
   }


   // Check for collisions with left paddle
   if (ballPos.x - ballRadius < leftPaddle.x + PADDLE_WIDTH && ballPos.y + ballRadius >= leftPaddle.y && ballPos.y - ballRadius <= leftPaddle.y + PADDLE_HEIGHT) {
      ballPos.x = leftPaddle.x + PADDLE_WIDTH + ballRadius;
      ballVelocity.x = -ballVelocity.x; // Reflect ball off left 

   }
   
  
   if (ballPos.x + ballRadius >= rightPaddle.x && ballPos.y >= rightPaddle.y && ballPos.y <= rightPaddle.y + PADDLE_HEIGHT) {
      ballPos.x = rightPaddle.x - ballRadius;
      ballVelocity.x = -ballVelocity.x; // Reflect ball off right paddle

   }

   // Check for scoring
   if (ballPos.x - ballRadius <= 0) {
      // Computer scores
      computerScore++;
      ballPos = cv::Point(_canvas.cols / 2, _canvas.rows / 2); // Reset ball position
   }
   else if (ballPos.x + ballRadius >= _canvas.cols) {
      // Player scores
      playerScore++;
      ballPos = cv::Point(_canvas.cols / 2, _canvas.rows / 2); // Reset ball position
   }
}

void CPong::drawPaddle(cv::Mat& canvas, cv::Rect& paddle) {
   cv::rectangle(canvas, paddle, cv::Scalar(255, 255, 255), cv::FILLED);
}

void CPong::drawBall(cv::Mat& canvas, cv::Point& ballPos) {
   cv::circle(canvas, ballPos, ballRadius, cv::Scalar(255, 255, 255), cv::FILLED);
}

void CPong::run()
{
   std::thread t1 (&CPong::update_thread, this);
   std::thread t2 (&CPong::draw_thread, this);
   t1.detach();
   t2.detach();
   while (cv::waitKey(1) != 'q') {
      cv::waitKey(1);
   }
   _thread_exit = true;
}

void CPong::update_thread(CPong* ptr)
{
   while (ptr->_thread_exit == false)
   {
      ptr->update();
   }
}
void CPong::draw_thread(CPong* ptr)
{
   while (ptr->_thread_exit == false)
   {
      ptr->draw();
   }
}
