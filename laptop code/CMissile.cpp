// CGameObject.cpp
#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include "stdafx.h"

#include "Constants.h"
#include "CGameObject.h"
#include "CMissile.h"

CMissile::CMissile(cv::Point2f position, std::string shape, std::string color)
   : CGameObject(position, shape, color) {
   m_state = "active";
   m_objectType = "missile";

   m_speed = 200;
   m_maxFuel = 2000000;
   m_fuel = m_maxFuel;

   m_lives = 1;
   m_health = 1;
   m_radius = 5;
}
CMissile::~CMissile() {

}

void CMissile::fire(cv::Point player_position, double player_angle, cv::Point player_velocity, cv::Rect& canvasSize) {
   if (m_state == "active") {
      // Get the current time
      auto currentTime = std::chrono::high_resolution_clock::now();

      // Calculate the time difference since the last update
      std::chrono::duration<double> timeDiff = currentTime - lastUpdateTime;
      double deltaTime = timeDiff.count(); // Delta time in seconds

      // Set the missile's position to the player's position
      m_position = player_position;

      // Calculate the missile's velocity based on the player's angle and the missile's speed
      m_velocity.x = player_velocity.x + cos(player_angle * PI / 180) * m_speed;
      m_velocity.y = player_velocity.y + sin(player_angle * PI / 180) * m_speed;

      // Update last update time for the next iteration
      lastUpdateTime = currentTime;
   }
}

void CMissile::update(cv::Rect& canvasSize) {
   if (m_state == "active") 
   {
      // Get the current time
      auto currentTime = std::chrono::high_resolution_clock::now();

      // Calculate the time difference since the last update
      std::chrono::duration<double> timeDiff = currentTime - lastUpdateTime;
      double deltaTime = timeDiff.count(); // Delta time in seconds

      if (!is_border_collision(canvasSize)) {
         // separate x and y components of velocity
         m_position.x += m_velocity.x * deltaTime;
         m_position.y += m_velocity.y * deltaTime;
      }
      else {
         m_state = "inactive";
         m_lives = 0;
      }

      // Check if fuel has run out
      if (m_fuel <= 0) {
         m_state = "inactive"; // Set state to inactive
         m_lives = 0;
      }

      //m_fuel -= 1 * deltaTime;

      // Update last update time for the next iteration
      lastUpdateTime = currentTime;
   }
}



void CMissile::draw(cv::Mat& canvas) {
   if (m_state == "active") {
      cv::circle(canvas, m_position, m_radius, cv::Scalar(255, 255, 0), cv::FILLED);
   }
}