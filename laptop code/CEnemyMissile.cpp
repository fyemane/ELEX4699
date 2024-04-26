// CGameObject.cpp
#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include "stdafx.h"

#include "Constants.h"
#include "CGameObject.h"
#include "CEnemyMissile.h"

CEnemyMissile::CEnemyMissile(cv::Point2f position, std::string shape, std::string color)
   : CGameObject(position, shape, color) {
   m_state = "active";
   m_objectType = "enemyMissile";

   m_speed = 200;
   m_maxFuel = 2000000;
   m_fuel = m_maxFuel;

   m_thrust = 0;

   m_lives = 1;
   m_maxHealth = 10;
   m_health = m_maxHealth;
   m_radius = 5;

}
CEnemyMissile::~CEnemyMissile() {

}

void CEnemyMissile::fire(cv::Point2f player_position, double player_angle, cv::Point player_velocity, cv::Rect& canvasSize) {
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

void CEnemyMissile::update(cv::Rect& canvasSize) {
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
         m_lives = 0;
         m_state = "inactive";
      }

      m_fuel -= 10;
      if (m_fuel == 0) {
         m_state = "inactive";
      }

      // Update last update time for the next iteration
      lastUpdateTime = currentTime;
   }
}



void CEnemyMissile::draw(cv::Mat& canvas) {
   if (m_state == "active") {
      cv::circle(canvas, m_position, m_radius, cv::Scalar(0, 255, 255), cv::FILLED);
   }
}