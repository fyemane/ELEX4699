/**
 * @file CMissile.h
 * @brief Declaration of the CMissile class.
 *
 * This file contains the declaration of the CMissile class, which represents a missile object in the game.
 * It extends the CGameObject class and adds functionality specific to missiles.
 *
 * @author Faniel Yemane
 */

#pragma once

#include "CGameObject.h"

 /**
  * @class CMissile
  * @brief A class representing a missile object in the game.
  *
  * This class extends the CGameObject class and adds functionality specific to missiles.
  */
class CMissile : public CGameObject {
private:
   int m_thrust; ///< The thrust of the missile.
   int m_maxFuel; ///< The maximum fuel capacity of the missile.
   int m_fuel; ///< The remaining fuel of the missile.

public:
   /**
    * @brief Constructor for CMissile.
    * @param position The position of the missile.
    * @param shape The shape of the missile.
    * @param color The color of the missile.
    */
   CMissile(cv::Point2f position, std::string shape, std::string color);

   /**
    * @brief Destructor for CMissile.
    */
   ~CMissile();

   /**
    * @brief Function to fire the missile.
    * @param player_position The position of the player.
    * @param player_angle The angle of the player.
    * @param player_velocity The velocity of the player.
    * @param canvasSize The size of the canvas.
    */
   void fire(cv::Point player_position, double player_angle, cv::Point player_velocity, cv::Rect& canvasSize);

   /**
    * @brief Function to update the state of the missile.
    * @param canvasSize The size of the canvas.
    */
   void update(cv::Rect& canvasSize) override;

   /**
    * @brief Function to reset the missile.
    */
   void reset();

   /**
    * @brief Function to draw the missile on the canvas.
    * @param canvas The canvas to draw on.
    */
   void draw(cv::Mat& canvas) override;
};
