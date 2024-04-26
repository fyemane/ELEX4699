/**
 * @file CAsteroids.h
 * @brief Declaration of the CAsteroids class.
 *
 * This file contains the declaration of the CAsteroids class, which represents a simple sketch application.
 * It extends the CBase4618 class and adds functionality for sketching based on user input and accelerometer readings.
 *
 * @author Faniel Yemane
 */

#pragma once

#include "CBase4618.h"
#include "CShip.h"
#include "CCamera.h"

 /**
  * @class CAsteroidGame
  * @brief A class representing a simple sketch application.
  *
  * This class extends the CBase4618 class and adds functionality for sketching
  * based on user input and accelerometer readings.
  */
class CAsteroidGame : public CBase4618 {
private:
   cv::Point joystick; ///< Current joystick position.
   bool resetFlag; ///< Flag to indicate if the game should be reset.
   bool exitFlag; ///< Flag to indicate if the game should exit.
   bool _thread_exit; ///< Flag for thread exit.
   cv::Mat persistentCanvas; ///< Mat to store persistent drawing.
   int thrustButtonPressed; ///< Counter for reset button presses.
   cv::Point gui_position; ///< Position for graphical user interface.
   int joyThreshold; ///< Threshold for joystick input.

   bool endGame; ///< Flag indicating the end of the game.
   int buttonValue2; ///< Value of button 2.
   int buttonValue1; ///< Value of button 1.
   int buttonPressed1; ///< Counter for button 1 presses.

   cv::Size m_canvasSize; ///< Size of the canvas for drawing.
   cv::Rect m_arena; ///< Rectangle representing the game arena.
   cv::Rect banner; ///< Rectangle representing the game banner.
   int bannerWidth; ///< Width of the game banner.

   int playerScore; ///< Player's score.
   int m_level; ///< Current level of the game.

   bool end_of_level; ///< Flag indicating the end of a level.
   std::string gameState; ///< Current state of the game.

   std::vector<CGameObject*> m_gameObjects; ///< Vector containing game objects.
   std::vector<CGameObject*> m_missileObjects; ///< Vector containing missile objects.
   std::vector<CGameObject*> m_enemyMissileObjects; ///< Vector containing enemy missile objects.

   std::vector<CGameObject*> m_allObjects; ///< Vector containing all game objects.
   std::vector<CGameObject*> m_levelObjects; ///< Vector containing objects specific to the current level.

   CShip player; ///< Player's ship object.
   CCamera gamecamera; ///< Game camera object.
   double fps; ///< Frames per second.

   cv::Mat backgroundImage; ///< Background image for the game.

   std::chrono::steady_clock::time_point m_levelStartTime; // Track the start time of the level
   std::chrono::milliseconds m_enemySpawnInterval; ///< Interval between enemy spawns.
   int m_maxEnemiesPerLevel; ///< Maximum number of enemies to spawn per level.

public:
   /**
    * @brief Constructor for CAsteroidGame.
    * @param canvasSize The size of the canvas for drawing.
    * @param comPort The COM port for communication.
    */
   CAsteroidGame(const cv::Size& canvasSize, int comPort);

   /**
    * @brief Destructor for CAsteroidGame.
    */
   ~CAsteroidGame();

   /**
    * @brief Overridden function for updating the sketch state.
    */
   void update() override;

   /**
    * @brief Overridden function for rendering the sketch.
    */
   void draw() override;

   /**
    * @brief Function to rotate the player's ship based on joystick input.
    * @param player_ship Reference to the player's ship object.
    * @param jytk Current joystick position.
    */
   void rotatePlayer(CShip& player_ship, cv::Point jytk) const;

   /**
    * @brief Function to start a new game level.
    * @param level The level to start.
    */
   void start_level(int level);

   /**
    * @brief Function to draw the border of the game arena.
    * @param canvas The canvas to draw on.
    * @param gameBorder The border rectangle.
    */
   void draw_border(cv::Mat& canvas, cv::Rect& gameBorder);

   /**
    * @brief Function to draw the game banner.
    * @param canvas The canvas to draw on.
    */
   void draw_banner(cv::Mat& canvas);

   /**
    * @brief Getter for game object vector.
    * @return Reference to the vector of game objects.
    */
   std::vector<CGameObject*>& getGameObjects() { return m_gameObjects; }

   /**
    * @brief Getter for missile object vector.
    * @return Reference to the vector of missile objects.
    */
   std::vector<CGameObject*>& getMissileObjects() { return m_missileObjects; }

   /**
    * @brief Getter for all game object vector.
    * @return Reference to the vector of all game objects.
    */
   std::vector<CGameObject*>& getallObjects() { return m_allObjects; }

   /**
    * @brief Function to check if an enemy should be spawned.
    * @return True if an enemy should be spawned, false otherwise.
    */
   bool shouldSpawnEnemy();

   /**
    * @brief Function to spawn an enemy.
    */
   void spawnEnemy();

   /**
    * @brief Function to handle user input.
    */
   void handleUserInput();

   /**
    * @brief Function to draw the introduction screen.
    * @param canvas The canvas to draw on.
    */
   void draw_intro(cv::Mat& canvas);

   /**
    * @brief Thread function for updating the sketch state.
    * @param ptr Pointer to CAsteroidGame object.
    */
   static void update_thread(CAsteroidGame* ptr);

   /**
    * @brief Thread function for rendering the sketch.
    * @param ptr Pointer to CAsteroidGame object.
    */
   static void draw_thread(CAsteroidGame* ptr);

   /**
    * @brief Thread function for handling user input.
    * @param ptr Pointer to CAsteroidGame object.
    */
   static void handleUserInput_thread(CAsteroidGame* ptr);

   /**
    * @brief Function to run the game.
    */
   void run();

};
