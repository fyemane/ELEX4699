// CAsteroidGame.cpp
// background image from https://wallpapercave.com/w/wp3493593
// music from https://www.youtube.com/watch?v=pAWXra5oKVA
#include "stdafx.h"
#include <algorithm>
#include <chrono>
#include <thread>
#include <time.h>
#include <random>
#include <iostream>
#include "cvui.h"

#include <Windows.h>
#include <mmsystem.h>

#include "Constants.h"
#include "CAsteroidGame.h"
#include "CGameObject.h"
#include "CShip.h"
#include "CAsteroid.h"
#include "CCamera.h"
#include "CMissile.h"
#include "CEnemy.h"
#include "CEnemyMissile.h"
#include "CItem.h"

#pragma comment(lib, "winmm.lib")


CAsteroidGame::CAsteroidGame(const cv::Size& canvasSize, int comPort)
   : player(cv::Point(canvasSize.width / 2, canvasSize.height / 2), "circle", "orange", m_missileObjects, m_allObjects) {
   // Initialize cvui here
   cvui::init("AsteroidGame");
   // Initialize control
   _control.init_com(comPort);

   // Initialize persistentCanvas and canvas
   persistentCanvas = cv::Mat::zeros(canvasSize, CV_8UC3);
   _canvas = cv::Mat::zeros(canvasSize, CV_8UC3);  // Initialize _canvas here
   backgroundImage = cv::imread("background_space.jpg");
   // Resize the background image to match the canvas size
   cv::resize(backgroundImage, backgroundImage, _canvas.size());

   m_canvasSize = canvasSize;
   bannerWidth = 300;
   gamecamera.set_camera(cv::Point(player.get_position().x, player.get_position().y));
   m_arena = cv::Rect(0 + 30, 0 + 30, m_canvasSize.width - bannerWidth - 60, canvasSize.height - 60);
   banner = cv::Rect(canvasSize.width - bannerWidth, 0, bannerWidth, canvasSize.height);

   gui_position = cv::Point(1250, 500);
   resetFlag = false, exitFlag = false, endGame = false, _thread_exit = false;
   thrustButtonPressed = 0;
   joyThreshold = 2048;

   buttonValue2 = 0;
   buttonValue1= 0;
   buttonPressed1 = 0;

   gameState = "intro";

   end_of_level = false;
   playerScore = 0;
   m_level = 1;

   srand(time(NULL));

   if (mciSendString("open Purplenum_Survival_Music.mp3 alias bgm", nullptr, 0, nullptr) != 0) {
      std::cerr << "Failed to open music file" << std::endl;
   }

   if (mciSendString("play bgm repeat", nullptr, 0, nullptr) != 0) {
      std::cerr << "Failed to play background music" << std::endl;
   }
}

CAsteroidGame::~CAsteroidGame() {
   // destructor
}

void CAsteroidGame::update() {
   // Read X and Y position values as a percentage from CControl
   //_control.get_analog(JOYSTICK_X, joystick.x);  // Assuming analog input channel 2 for X
   //_control.get_analog(JOYSTICK_Y, joystick.y);  // Assuming analog input channel 26 for Y
   if (gameState == "splash") {
      // Read debounced pushbutton for reset
      if (!buttonValue1 && buttonPressed1) {
         gameState = "playing";
         start_level(m_level);
         // Button is pressed
         buttonPressed1 = 0;
      }
      if (buttonValue1) {
         buttonPressed1 = 1;
      }
   }


   if (gameState == "playing") {
      rotatePlayer(player, joystick);
      //std::cout << player.get_angle() << "\t";
      //std::cout << player.get_velocity().x << "\t" << player.get_velocity().y << "\t";

      if (!endGame) {

         if (shouldSpawnEnemy())
            spawnEnemy();

         // Update enemy behavior
         for (auto& enemy : m_gameObjects) {
            if (enemy->get_objectType() == "enemy") {
               CEnemy* enemyObject = dynamic_cast<CEnemy*>(enemy);
               if (enemyObject) {
                  // Check if the player is within the detection range
                  if (enemyObject->objectWithinShootingRange(player)) {
                     // Fire at the player
                     enemyObject->fireAtObject(player, m_enemyMissileObjects);
                  }
               }
            }
         }

         for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); ) {
            // Get the pointer to the current game object
            CGameObject* sprite = *it;

            // Update the game object
            sprite->update(m_arena);

            if (sprite->get_objectBehavior() == "hostile" && sprite->get_state() == "active") {
               if (player.is_collision(*sprite) && sprite->get_objectType() == "asteroid") {
                  sprite->takes_damage(10);
                  player.takes_damage(10);
               }
               if (player.is_collision(*sprite) && sprite->get_objectType() == "enemy") {
                  sprite->takes_damage(40);
                  player.takes_damage(40);
               }
            }
            if (sprite->get_objectBehavior() == "passive" && sprite->get_state() == "active") {
               if (player.is_collision(*sprite) && sprite->get_objectType() == "plus") {
                  sprite->takes_damage(10);
                  player.set_health(player.get_maxHealth());
               }
               if (player.is_collision(*sprite) && sprite->get_objectType() == "heart") {
                  sprite->takes_damage(10);
                  player.set_lives(player.get_lives() + 1);
               }
            }

            // Check if the game object's lives are 0 or its state is "inactive"
            if ((sprite->get_lives() == 0 || sprite->get_state() == "inactive") && sprite->get_objectType() != "ship") {
               // Erase the game object from the vector and get the iterator to the next position
               it = m_gameObjects.erase(it);
               // Delete the game object from memory
               delete sprite;
            }
            else {
               // Move to the next position in the vector
               ++it;
            }
         }

         for (auto it = m_missileObjects.begin(); it != m_missileObjects.end(); ) {

            CGameObject* missile = *it;

            missile->update(m_arena);

            for (auto spriteIt = m_gameObjects.begin(); spriteIt != m_gameObjects.end(); ++spriteIt) {
               CGameObject* sprite = *spriteIt;

               // Check for collision between missile and asteroid
               if (sprite->get_objectBehavior() == "hostile" && missile->is_collision(*sprite)) {
                  // Handle collision
                  missile->takes_damage(10);
                  sprite->takes_damage(10);
                  if (sprite->get_lives() <= 0)
                     playerScore += 10;
               }
            }

            // Check if the game object's lives are 0 or its state is "inactive"
            if (missile->get_lives() == 0 || missile->get_state() == "inactive") {
               // Erase the game object from the vector and get the iterator to the next position
               it = m_missileObjects.erase(it);

               // Delete the game object from memory
               delete missile;
            }
            else {
               // Move to the next position in the vector
               ++it;
            }
         }

         for (auto it = m_enemyMissileObjects.begin(); it != m_enemyMissileObjects.end(); ) {
            CGameObject* enemyMissile = *it;

            enemyMissile->update(m_arena);

            for (auto spriteIt = m_gameObjects.begin(); spriteIt != m_gameObjects.end(); ++spriteIt) {
               CGameObject* sprite = *spriteIt;

               // Check for collision between missile and asteroid
               if (sprite->get_objectType() == "ship" && enemyMissile->is_collision(*sprite)) {
                  // Handle collision
                  enemyMissile->takes_damage(10);
                  sprite->takes_damage(10);

               }
            }

            // Check if the game object's lives are 0 or its state is "inactive"
            if (enemyMissile->get_lives() <= 0 || enemyMissile->get_state() == "inactive") {
               // Erase the game object from the vector and get the iterator to the next position
               it = m_enemyMissileObjects.erase(it);

               // Delete the game object from memory
               delete enemyMissile;
            }
            else {
               // Move to the next position in the vector
               ++it;
            }
         }

         end_of_level = true;
         // Check for end of level
         for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); ++it) {
            CGameObject* sprite = *it;
            if ((sprite->get_objectType() == "asteroid" || sprite->get_objectType() == "enemy") && sprite->get_state() == "active") {
               end_of_level = false;
               break;  // Break out of the loop as soon as an active asteroid is found
            }
         }
         if (end_of_level) {
            m_level += 1;
            start_level(m_level);
            end_of_level = false;
         }

         if (player.get_lives() == 0) {
            endGame = true;
         }
      }

      if (resetFlag) {
         playerScore = 0;
         m_level = 1; // Set to level 1
         // Other game-specific variables initialization for level 1...

         // Start level 1
         player.set_state("active");
         player.set_lives(10);
         player.set_health(player.get_maxHealth());
         start_level(m_level);
         resetFlag = false;
         endGame = false;
      }


      // Read debounced pushbutton for reset
      //_control.get_button(BUTTON1, fireButtonValue);
      if (!buttonValue1 && buttonPressed1) {
         player.fire();
         // Button is pressed
         buttonPressed1 = 0;
      }
      if (buttonValue1) {
         buttonPressed1 = 1;
      }

      // Read debounced pushbutton for reset
      //_control.get_button(BUTTON2, thrustButtonValue);
      if (!buttonValue2) {
         // Button is pressed
         player.accelerate();
      }
      if (buttonValue2) {
         player.deccelerate();
      }
   }
}


void CAsteroidGame::handleUserInput() {
   _control.get_analog(JOYSTICK_X, joystick.x);  // Assuming analog input channel 2 for X
   _control.get_analog(JOYSTICK_Y, joystick.y);  // Assuming analog input channel 26 for Y
   _control.get_button(BUTTON1, buttonValue1);
   _control.get_button(BUTTON2, buttonValue2);
}

void CAsteroidGame::draw_intro(cv::Mat& canvas) {
   static int introCounter = 0;
   static int introDurations[] = { 800, 1700, 1700, 1700, 1700 }; // Durations for each intro item in milliseconds

   if (introCounter == 0) {
      // Draw first intro item
      cv::putText(_canvas, "Yeah Man! Studios", cv::Point(100, 400), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(255, 255, 255), 2);
      std::this_thread::sleep_for(std::chrono::milliseconds(introDurations[introCounter]));
   }
   else if (introCounter == 1) {
      // Draw second intro item
      cv::putText(_canvas, "from BCIT headquarters", cv::Point(100, 500), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(150, 150, 255), 2);
      std::this_thread::sleep_for(std::chrono::milliseconds(introDurations[introCounter]));

   }
   else if (introCounter == 2) {
      // Draw third intro item
      cv::putText(_canvas, "for ELEX 4618 Lab 6 Asteroids XD ", cv::Point(100, 600), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(100, 100, 255), 2);
      std::this_thread::sleep_for(std::chrono::milliseconds(introDurations[introCounter]));
   }
   else if (introCounter == 3) {
      // Draw third intro item
      cv::putText(_canvas, "Presents...", cv::Point(100, 700), cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(50, 50, 255), 2);
      std::this_thread::sleep_for(std::chrono::milliseconds(introDurations[introCounter]));
   }
   else {
      // Intro sequence finished, start the game
      std::this_thread::sleep_for(std::chrono::milliseconds(introDurations[introCounter]));
      gameState = "splash";
   }
   introCounter++;
}

void CAsteroidGame::draw() {
   auto calc_start = std::chrono::steady_clock::now();
   // Set update rate at 30 Hz (100 ms per loop)
   // Set update rate at 30 Hz (100 ms per loop)
   auto end_time = std::chrono::system_clock::now() + std::chrono::milliseconds(23);

   _canvas = persistentCanvas.clone();  // clear canvas

   if (gameState == "intro") {
      draw_intro(_canvas);
   }

   if (gameState == "splash") {
      // Display splash screen
      _canvas = persistentCanvas.clone();  // clear canva
      cv::putText(_canvas, "C PLUS ", cv::Point(100, 250), cv::FONT_HERSHEY_COMPLEX, 6, cv::Scalar(255, 255, 255), 2);
      cv::putText(_canvas, "DESTROY!", cv::Point(100, 450), cv::FONT_HERSHEY_COMPLEX, 6, cv::Scalar(0, 0, 255), 2);
      //cv::putText(_canvas, "Press any key to start...", cv::Point(100, 300), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
      cv::putText(_canvas, "Created by Faniel Yemane" , cv::Point(100, 700), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
      cv::putText(_canvas, "Press button 1 to start", cv::Point(100, 750), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(255, 255, 255), 2);

   }

   if (gameState == "playing") {

      // Overlay the background image on the canvas
      backgroundImage.copyTo(_canvas);

      if (endGame) {
         cv::putText(_canvas, "GAME", cv::Point(350, 400), cv::FONT_HERSHEY_COMPLEX, 6, cv::Scalar(255, 255, 255), 2);
         cv::putText(_canvas, "OVER", cv::Point(350, 600), cv::FONT_HERSHEY_COMPLEX, 6, cv::Scalar(255, 255, 255), 2);
      }

      for (auto& missile : m_missileObjects) {
         missile->draw(_canvas);
      }

      for (auto& enemyMissile : m_enemyMissileObjects) {
         enemyMissile->draw(_canvas);
      }

      for (auto& sprite : m_gameObjects) {
         sprite->draw(_canvas);
      }

      draw_border(_canvas, m_arena);
      draw_banner(_canvas);

      // Display the canvas image
      // Draw GUI elements in the top-left corner
      cvui::window(_canvas, gui_position.x, gui_position.y, 150, 70, "AsteroidGame FPS: " + std::to_string(static_cast<int>(fps)));

      if (cvui::button(_canvas, gui_position.x + 80, gui_position.y + 30, "Exit")) {
         std::exit(0);
      }

      if (cvui::button(_canvas, gui_position.x + 10, gui_position.y + 30, "Reset")) {
         // Reset button pressed, set reset flag
         resetFlag = true;
      }
   }
   // Display the canvas on the window
   if (!exitFlag) {
      // Update the CVUI GUI system
      cvui::update();
      cv::imshow("AsteroidGame", _canvas);
   }

   // Sleep if time remaining
   std::this_thread::sleep_until(end_time);

   auto calc_end = std::chrono::steady_clock::now();
   auto calc_elapsed = std::chrono::duration<double>(calc_end - calc_start);
   fps = 1 / calc_elapsed.count();

}

void CAsteroidGame::rotatePlayer(CShip& player_ship, cv::Point jytk) const {
   if (jytk.x < joyThreshold - 0.15 * joyThreshold) {
      player_ship.rotate_left();
      //std::cout << "Rotating left" << std::endl;
   }
   else if (jytk.x > joyThreshold + 0.15 * joyThreshold) {
      player_ship.rotate_right();
      //std::cout << "Rotating right" << std::endl;
   }
   else {
      player_ship.stop_rotation();
      //std::cout << "no rotation" << std::endl;
   }
} 

void CAsteroidGame::start_level(int level) {
   // Clear the game objects list
   m_gameObjects.clear();
   //m_missileObjects.clear();
   m_allObjects.clear();
   m_levelObjects.clear();

   // Add the player ship to the game objects list
   m_gameObjects.push_back(&player);

   // Set initial values for time-based spawning
   m_levelStartTime = std::chrono::steady_clock::now();
   m_enemySpawnInterval = std::chrono::milliseconds(1000); // Example: Spawn an enemy every 1 second
   m_maxEnemiesPerLevel = 10; // Example: Maximum 10 enemies per level

   // Randomize asteroid position and velocity
   int radius = rand() % (20 - 10) + 10;
   double x = rand() % (m_arena.x + m_arena.width - radius - (m_arena.x + radius)) + m_arena.x + radius;
   double y = rand() % (m_arena.y + m_arena.height - radius - (m_arena.y + radius)) + m_arena.y + radius;
   double vx = rand() % (50 - -50) + -50;
   double vy = rand() % (50 - -50) + -50;

   // Create a new asteroid object
   CAsteroid* asteroid = new CAsteroid(cv::Point(x, y), "circle", "gray");
   asteroid->set_velocity(cv::Point(vx, vy));
   asteroid->set_radius(radius);

   // Add the asteroid to the game objects list
   m_gameObjects.push_back(asteroid);

   // Add asteroids
   for (int i = 0; i <= level; i++) {
      // Randomize asteroid position and velocity
      int radius;
      double x, y, vx, vy;

      radius = rand() % (100 - 10) + 10;
      x = rand() % (m_arena.x + m_arena.width - radius - (m_arena.x + radius)) + m_arena.x + radius;
      y = rand() % (m_arena.y + m_arena.height - radius - (m_arena.y + radius)) + m_arena.y + radius;
      do {
         vx = rand() % (200 - -200) + -200;
         vy = rand() % (200 - -200) + -200;
      } while (vx != 0 && vy != 0);

      // Cr eate a new asteroid object
      CAsteroid* asteroid = new CAsteroid(cv::Point(x, y), "circle", "gray");
      asteroid->set_velocity(cv::Point2f(vx, vy));
      asteroid->set_radius(radius);

      // Add the asteroid to the game objects list
      m_levelObjects.push_back(asteroid);
   }

   if (m_level >= 3) {
      // Add enemies
      for (int i = 0; i <= level; i++) {
         // Randomize enemy position and velocity
         int radius = rand() % (15 - 5) + 5;
         double x = rand() % (m_arena.x + m_arena.width - radius - (m_arena.x + radius)) + m_arena.x + radius;
         double y = rand() % (m_arena.y + m_arena.height - radius - (m_arena.y + radius)) + m_arena.y + radius;
         double vx = rand() % (200 - -200) + -200;
         double vy = rand() % (200 - -200) + -200;

         // Create a new enemy object
         CEnemy* enemy = new CEnemy(cv::Point2f(x, y), "circle", "gray", "stalker", m_gameObjects, m_enemyMissileObjects);
         enemy->set_velocity(cv::Point2f(vx, vy));
         enemy->set_radius(radius);

         // Add the asteroid to the game objects list
         m_levelObjects.push_back(enemy);
      }
   }

   if (m_level > 0) {
      for (int i = 0; i < 1; i++) {
         // Randomize asteroid position and velocity
         int radius = 20;
         double x = rand() % (m_arena.x + m_arena.width - radius - (m_arena.x + radius)) + m_arena.x + radius;
         double y = rand() % (m_arena.y + m_arena.height - radius - (m_arena.y + radius)) + m_arena.y + radius;

         // Create a new enemy object
         CItem* item = new CItem(cv::Point2f(x, y), "circle", "gray");
         item->set_radius(radius);

         // Add the asteroid to the game objects list
         m_levelObjects.push_back(item);
      }
   }
}

void CAsteroidGame::draw_border(cv::Mat& canvas, cv::Rect& gameBorder) {
   cv::rectangle(canvas, gameBorder, cv::Scalar(255, 255, 255), 2);
}


void CAsteroidGame::draw_banner(cv::Mat& canvas) {
   cv::rectangle(_canvas, banner, cv::Scalar(100, 100, 100), cv::FILLED);
   cv::line(_canvas, cv::Point(_canvas.cols - 300, 0), cv::Point(_canvas.cols - 300, _canvas.rows), cv::Scalar(255, 255, 255), 3);

   // Write the title "C and Destroy" on top of the banner
   int title_x = canvas.cols - 270;
   int title_y = 80;

   cv::putText(canvas, "C PLUS", cv::Point(title_x, title_y), cv::FONT_HERSHEY_COMPLEX, 1.5, cv::Scalar(255, 255, 255), 2);
   cv::putText(canvas, "DESTROY", cv::Point(title_x, title_y + 70), cv::FONT_HERSHEY_COMPLEX, 1.5, cv::Scalar(255, 255, 255), 2);

   // Write the score, enemies, lives, level, and health inside the banner
   int x_offset = canvas.cols - 270;
   int y_offset = 250;
   int line_spacing = 40;

   cv::putText(canvas, "SCORE: " + std::to_string(playerScore), cv::Point(x_offset, y_offset), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1);
   cv::putText(canvas, "LEVEL: " + std::to_string(m_level), cv::Point(x_offset, y_offset + 1 * line_spacing), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1);
   cv::putText(canvas, "ENTITIES: " + std::to_string(m_gameObjects.size() - 1), cv::Point(x_offset, y_offset + 2 * line_spacing), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1);
   // Assuming player lives and health are member variables of the player object
   cv::putText(canvas, "LIVES: " + std::to_string(player.get_lives()), cv::Point(x_offset, y_offset + 3 * line_spacing), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1);
   cv::putText(canvas, "HEALTH: " + std::to_string(player.get_health()), cv::Point(x_offset, y_offset + 4 * line_spacing), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1);
   cv::putText(canvas, "MISSILES: " + std::to_string(m_missileObjects.size()), cv::Point(x_offset, y_offset + 5 * line_spacing), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1);
   cv::putText(canvas, "E-MISSILES: " + std::to_string(m_enemyMissileObjects.size()), cv::Point(x_offset, y_offset + 6 * line_spacing), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 1);

}


bool CAsteroidGame::shouldSpawnEnemy() {
   // Get the current time
   auto currentTime = std::chrono::steady_clock::now();

   // Calculate elapsed time since the level started
   auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_levelStartTime);

   // Check if enough time has passed to spawn a new enemy and if there are any objects left in the levelObjects vector
   return elapsedTime >= m_enemySpawnInterval && !m_levelObjects.empty();
}

void CAsteroidGame::spawnEnemy() {
   m_levelStartTime = std::chrono::steady_clock::now(); // Set the current time as the next enemy spawn time
   // Implement enemy spawning logic...

   int randint = rand() % m_levelObjects.size();

   // Check if there are any objects left in the levelObjects vector
   if (!m_levelObjects.empty()) {
      // Move the first item from levelObjects to gameObjects
      m_gameObjects.push_back((m_levelObjects.at(randint)));
      // Erase the first item from levelObjects
      m_levelObjects.erase(m_levelObjects.begin() + randint);
   }
}


void CAsteroidGame::update_thread(CAsteroidGame* ptr)
{
   while (ptr->_thread_exit == false)
   {
      ptr->update();
   }
}
void CAsteroidGame::draw_thread(CAsteroidGame* ptr)
{
   while (ptr->_thread_exit == false)
   {
      ptr->draw();
   }
}

void CAsteroidGame::handleUserInput_thread(CAsteroidGame* ptr)
{
   while (ptr->_thread_exit == false)
   {
      ptr->handleUserInput();
   }
}


void CAsteroidGame::run()
{
   std::thread t1(&CAsteroidGame::update_thread, this);
   std::thread t2(&CAsteroidGame::draw_thread, this);
   std::thread t3(&CAsteroidGame::handleUserInput_thread, this);
   t1.detach();
   t2.detach();
   t3.detach();
   while (cv::waitKey(1) != 'q') {
      cv::waitKey(1);
   }
   _thread_exit = true;
}