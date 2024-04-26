/**
 * @file CEnemyMissile.h
 * @brief Declaration of the CEnemyMissile class.
 *
 * This file contains the declaration of the CEnemyMissile class, which represents an enemy missile object in the game.
 * It extends the CGameObject class and adds functionality specific to enemy missiles.
 *
 * @author Faniel Yemane
 */

#pragma once

#include "CGameObject.h"

 /**
  * @class CEnemyMissile
  * @brief A class representing an enemy missile object in the game.
  *
  * This class extends the CGameObject class and adds functionality specific to enemy missiles.
  */
class CEnemyMissile : public CGameObject {
private:
   int m_thrust; ///< The thrust of the enemy missile.
   int m_fuel; ///< The remaining fuel of the enemy missile.
   int m_maxFuel; ///< The maximum fuel capacity of the enemy missile.

public:
   /**
    * @brief Constructor for CEnemyMissile.
    * @param position The position of the enemy missile.
    * @param shape The shape of the enemy missile.
    * @param color The color of the enemy missile.
    */
   CEnemyMissile(cv::Point2f position, std::string shape, std::string color);

   /**
    * @brief Destructor for CEnemyMissile.
    */
   ~CEnemyMissile();

   /**
    * @brief Function to fire the enemy missile towards the player.
    * @param player_position The position of the player.
    * @param player_angle The angle of the player.
    * @param player_velocity The velocity of the player.
    * @param canvasSize The size of the canvas.
    */
   void fire(cv::Point2f player_position, double player_angle, cv::Point player_velocity, cv::Rect& canvasSize);

   /**
    * @brief Function to update the state of the enemy missile.
    * @param canvasSize The size of the canvas.
    */
   void update(cv::Rect& canvasSize) override;

   /**
    * @brief Function to draw the enemy missile on the canvas.
    * @param canvas The canvas to draw on.
    */
   void draw(cv::Mat& canvas) override;
};
