/**
 * @file CAsteroid.h
 * @brief Declaration of the CAsteroid class.
 *
 * This file contains the declaration of the CAsteroid class, which represents an asteroid object in the game.
 * It extends the CGameObject class and adds functionality specific to asteroids.
 */

#pragma once

#include "CGameObject.h"

 /**
  * @class CAsteroid
  * @brief A class representing an asteroid object in the game.
  *
  * This class extends the CGameObject class and adds functionality specific to asteroids.
  */
class CAsteroid : public CGameObject {
private:
   int m_scalarColor; ///< Scalar color value for the asteroid.

public:
   /**
    * @brief Constructor for CAsteroid.
    * @param position The position of the asteroid.
    * @param shape The shape of the asteroid.
    * @param color The color of the asteroid.
    */
   CAsteroid(cv::Point2f position, std::string shape, std::string color);

   /**
    * @brief Destructor for CAsteroid.
    */
   ~CAsteroid();

   /**
    * @brief Function to update the asteroid's state.
    * @param canvasSize The size of the canvas.
    */
   void update(cv::Rect& canvasSize) override;

   /**
    * @brief Function to draw the asteroid on the canvas.
    * @param canvas The canvas to draw on.
    */
   void draw(cv::Mat& canvas) override;
};

