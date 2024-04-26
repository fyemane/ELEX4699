/**
 * @file CGameObject.h
 * @brief Declaration of the CGameObject class.
 *
 * This file contains the declaration of the CGameObject class, which represents a generic game object.
 * 
 * @author Faniel Yemane
 */

#pragma once

#include "stdafx.h"

 /**
  * @class CGameObject
  * @brief A class representing a generic game object.
  *
  * This class serves as the base class for all game objects in the game.
  */
class CGameObject {
protected:
   cv::Point2f m_position; ///< The position of the game object.
   cv::Point2f m_velocity; ///< The velocity of the game object.
   cv::Point2f m_maxVelocity; ///< The maximum velocity of the game object.

   double m_speed; ///< The speed of the game object.
   double m_acceleration; ///< The acceleration of the game object.
   double m_angle; ///< The angle of the game object.
   double m_angleVelocity; ///< The angular velocity of the game object.

   std::string m_shape; ///< The shape of the game object.
   std::string m_color; ///< The color of the game object.
   std::string m_state; ///< The state of the game object.
   std::string m_objectType; ///< The type of the game object.
   std::string m_behavior; ///< The behavior of the game object.

   double m_radius; ///< The radius of the game object.
   int m_health; ///< The health of the game object.
   int m_maxHealth; ///< The maximum health of the game object.
   int m_lives; ///< The number of lives of the game object.

   std::chrono::high_resolution_clock::time_point lastUpdateTime; ///< Variable to store the last update time.

public:
   /**
    * @brief Constructor for CGameObject.
    * @param position The position of the game object.
    * @param shape The shape of the game object.
    * @param color The color of the game object.
    */
   CGameObject(cv::Point2f position, std::string shape, std::string color);

   /**
    * @brief Destructor for CGameObject.
    */
   virtual ~CGameObject();

   /**
    * @brief Function to check if there is a collision with another game object.
    * @param other The other game object to check collision with.
    * @return True if there is a collision, false otherwise.
    */
   bool is_collision(CGameObject& other);

   /**
    * @brief Function to check if there is a collision with the border of the canvas.
    * @param canvasSize The size of the canvas.
    * @return True if there is a collision with the border, false otherwise.
    */
   bool is_border_collision(cv::Rect& canvasSize);

   /**
    * @brief Function to handle bouncing off another game object.
    * @param other The other game object to bounce off.
    */
   void bounce(CGameObject& other);

   /**
    * @brief Function to handle bouncing off the border of the canvas.
    * @param canvasSize The size of the canvas.
    */
   void border_bounce(cv::Rect& canvasSize);

   /**
    * @brief Function to draw the game object on the canvas.
    * @param canvas The canvas to draw on.
    */
   virtual void draw(cv::Mat& canvas);

   /**
    * @brief Function to draw the health bar of the game object.
    * @param canvas The canvas to draw on.
    * @param sprite_position The position of the sprite.
    */
   virtual void drawHealthBar(cv::Mat& canvas, cv::Point2f sprite_position);

   /**
    * @brief Function to update the state of the game object.
    * @param canvasSize The size of the canvas.
    */
   virtual void update(cv::Rect& canvasSize) = 0;

   /**
    * @brief Function to handle firing action of the game object.
    */
   virtual void fire();

   /**
    * @brief Function to handle damage taken by the game object.
    * @param damage The amount of damage taken.
    */
   virtual void takes_damage(double damage);

   /**
    * @brief Function to rotate a point around a center point by an angle.
    * @param pt The point to rotate.
    * @param center The center point to rotate around.
    * @param angle_rad The angle in radians to rotate by.
    * @return The rotated point.
    */
   cv::Point2f rotatePoint(cv::Point2f pt, cv::Point2f center, double angle_rad);

   // Getters
   inline cv::Point2f get_position() const { return m_position; } ///< Get the position of the game object.
   inline cv::Point2f get_velocity() const { return m_velocity; } ///< Get the velocity of the game object.
   inline double get_angle() const { return m_angle; } ///< Get the angle of the game object.
   inline std::string get_state() const { return m_state; } ///< Get the state of the game object.
   inline std::string get_objectType() const { return m_objectType; } ///< Get the type of the game object.
   inline std::string get_objectBehavior() const { return m_behavior; } ///< Get the behavior of the game object.
   inline std::string get_shape() const { return m_shape; } ///< Get the shape of the game object.
   inline std::string get_color() const { return m_color; } ///< Get the color of the game object.
   inline int get_lives() const { return m_lives; } ///< Get the number of lives of the game object.
   inline int get_health() const { return m_health; } ///< Get the health of the game object.
   inline int get_maxHealth() const { return m_maxHealth; } ///< Get the maximum health of the game object.
   inline int get_radius() const { return m_radius; } ///< Get the radius of the game object.

   // Setters
   inline void set_position(cv::Point2f position) { m_position = position; } ///< Set the position of the game object.
   inline void set_velocity(cv::Point2f velocity) { m_velocity = velocity; } ///< Set the velocity of the game object.
   inline void set_angle(double angle) { m_angle = angle; } ///< Set the angle of the game object.
   inline void set_radius(double radius) { m_radius = radius; } ///< Set the radius of the game object.

   inline void set_shape(std::string shape) { m_shape = shape; } ///< Set the shape of the game object.
   inline void set_color(std::string color) { m_color = color; } ///< Set the color of the game object.
   inline void set_state(std::string new_state) { m_state = new_state; } ///< Set the state of the game object.
   inline void set_lives(int new_lives) { m_lives = new_lives; } ///< Set the number of lives of the game object.
   inline void set_health(int new_health) { m_health = new_health; } ///< Set the health of the game object.
   inline void set_maxHealth(int new_maxHealth) { m_maxHealth = new_maxHealth; } ///< Set the maximum health of the game object.
};
