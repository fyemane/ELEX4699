/**
 * @file CShip.h
 * @brief Declaration of the CShip class.
 *
 * This file contains the declaration of the CShip class, which represents a ship object in the game.
 * It extends the CGameObject class and adds functionality specific to the ship.
 * 
 * @author Faniel Yemane
 */

#pragma once

#include "CGameObject.h"

 /**
  * @class CShip
  * @brief A class representing a ship object in the game.
  *
  * This class extends the CGameObject class and adds functionality specific to the ship.
  */
class CShip : public CGameObject {
private:
   int m_missileCount; ///< The count of missiles of the ship.
   int m_missileStack; ///< The stack of missiles of the ship.

   std::vector<CGameObject*>& gameobjects; ///< Reference to the vector of all game objects.
   std::vector<CGameObject*>& missileobjects; ///< Reference to the vector of missile objects.
   std::vector<CGameObject*>& allobjects; ///< Reference to the vector of all objects.

public:
   /**
    * @brief Constructor for CShip.
    * @param position The position of the ship.
    * @param shape The shape of the ship.
    * @param color The color of the ship.
    * @param missileObjects Reference to the vector of missile objects.
    * @param allObjects Reference to the vector of all objects.
    */
   CShip(cv::Point position, std::string shape, std::string color, std::vector<CGameObject*>& missileObjects, std::vector<CGameObject*>& allObjects);

   /**
    * @brief Destructor for CShip.
    */
   ~CShip();

   /**
    * @brief Function to rotate the ship left.
    */
   void rotate_left();

   /**
    * @brief Function to rotate the ship right.
    */
   void rotate_right();

   /**
    * @brief Function to stop the rotation of the ship.
    */
   void stop_rotation();

   /**
    * @brief Function to accelerate the ship.
    */
   void accelerate();

   /**
    * @brief Function to decelerate the ship.
    */
   void deccelerate();

   /**
    * @brief Function to update the state of the ship.
    * @param canvasSize The size of the canvas.
    */
   void update(cv::Rect& canvasSize) override;

   /**
    * @brief Function to draw the ship on the canvas.
    * @param canvas The canvas to draw on.
    */
   void draw(cv::Mat& canvas) override;

   /**
    * @brief Function to fire a missile from the ship.
    */
   void fire();

   /**
    * @brief Function to set the vector of game objects.
    * @param gameObjects Reference to the vector of game objects.
    */
   void setGameObjects(std::vector<CGameObject*>& gameObjects) {
      gameobjects = gameObjects;
   }

   /**
    * @brief Function to set the vector of missile objects.
    * @param missileObjects Reference to the vector of missile objects.
    */
   void setMissileObjects(std::vector<CGameObject*>& missileObjects) {
      missileobjects = missileObjects;
   }
};
