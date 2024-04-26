/**
 * @file CEnemy.h
 * @brief Declaration of the CEnemy class.
 *
 * This file contains the declaration of the CEnemy class, which represents an enemy object in the game.
 * It extends the CGameObject class and adds functionality specific to enemies.
 */

#pragma once

#include "CGameObject.h"

 /**
  * @class CEnemy
  * @brief A class representing an enemy object in the game.
  *
  * This class extends the CGameObject class and adds functionality specific to enemies.
  */
class CEnemy :
   public CGameObject {
private:
   std::vector<std::string> m_enemyTypes; ///< Vector of possible enemy types.
   std::string m_enemyType; ///< The type of the enemy.

   double detection_range; ///< The detection range of the enemy.
   double shooting_range; ///< The shooting range of the enemy.
   double fire_frequency; ///< The frequency at which the enemy fires (e.g., shots per second).
   std::chrono::steady_clock::time_point lastFireTime; ///< Time point of the last fire.

   std::vector<CGameObject*>& m_gameObjects; ///< Reference to the vector of all game objects.
   std::vector<CGameObject*>& m_enemyMissileObjects; ///< Reference to the vector of enemy missile objects.

public:
   /**
    * @brief Constructor for CEnemy.
    * @param position The position of the enemy.
    * @param shape The shape of the enemy.
    * @param color The color of the enemy.
    * @param enemyType The type of the enemy.
    * @param gameObjects Reference to the vector of all game objects.
    * @param enemyMissileObjects Reference to the vector of enemy missile objects.
    */
   CEnemy(cv::Point2f position, std::string shape, std::string color, std::string enemyType, std::vector<CGameObject*>& gameObjects, std::vector<CGameObject*>& enemyMissileObjects);

   /**
    * @brief Destructor for CEnemy.
    */
   ~CEnemy();

   /**
    * @brief Getter for the type of the enemy.
    * @return The type of the enemy.
    */
   inline std::string get_enemyType() const { return m_enemyType; }

   /**
    * @brief Function to rotate the enemy towards the player.
    * @param player Reference to the player object.
    */
   void rotateTowardsPlayer(const CGameObject& player);

   /**
    * @brief Method to check if a game object is within the detection range for detecting.
    * @param object Reference to the game object to check.
    * @return True if the object is within the detection range, false otherwise.
    */
   bool objectWithinDetectionRange(const CGameObject& object) const;

   /**
    * @brief Method to check if a game object is within the shooting range.
    * @param object Reference to the game object to check.
    * @return True if the object is within the shooting range, false otherwise.
    */
   bool objectWithinShootingRange(const CGameObject& object) const;

   /**
    * @brief Method to make the enemy fire at a game object.
    * @param object Reference to the game object to fire at.
    * @param missileObjects Reference to the vector of missile objects.
    */
   void fireAtObject(const CGameObject& object, std::vector<CGameObject*>& missileObjects);

   /**
    * @brief Function to update the state of the enemy.
    * @param canvasSize The size of the canvas.
    */
   void update(cv::Rect& canvasSize) override;

   /**
    * @brief Function to draw the enemy on the canvas.
    * @param canvas The canvas to draw on.
    */
   void draw(cv::Mat& canvas) override;
};
