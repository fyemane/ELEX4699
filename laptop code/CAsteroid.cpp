// CGameObject.cpp
#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include "stdafx.h"

#include "Constants.h"
#include "CGameObject.h"
#include "CAsteroid.h"

CAsteroid::CAsteroid(cv::Point2f position, std::string shape, std::string color)
   : CGameObject(position, shape, color) {
   m_velocity = cv::Point(static_cast<double>(rand() % (30 - 10) + 10),
                          static_cast<double>(rand() % (30 - 10) + 10));
   m_angle = 0;
   m_angleVelocity = 0;
   m_acceleration = 0;
   m_radius = 20;

   m_objectType = "asteroid";
   m_state = "active";
   m_behavior = "hostile";

   m_health = 1;
   m_maxHealth = 1;
   m_lives = 1;
   m_scalarColor = rand() % (150 - 50) + 50;
}

CAsteroid::~CAsteroid() {
   // destructor
}

void CAsteroid::update(cv::Rect& canvasSize) {
   if (m_state == "active") {
      // Get the current time
      auto currentTime = std::chrono::high_resolution_clock::now();

      // Calculate the time difference since the last update
      std::chrono::duration<double> timeDiff = currentTime - lastUpdateTime;
      double deltaTime = timeDiff.count(); // Delta time in seconds

      // change direction
      m_angle += m_angleVelocity;

      // Check for border collision before updating the position
      if (!is_border_collision(canvasSize)) {
         // separate x and y components of velocity
         m_position.x += m_velocity.x * deltaTime + 0.5 * cos(m_angle * PI / 180) * m_acceleration * pow(deltaTime, 2);
         m_position.y += m_velocity.y * deltaTime + 0.5 * sin(m_angle * PI / 180) * m_acceleration * pow(deltaTime, 2);

         // Update velocity based on acceleration
         m_velocity.x += cos(m_angle * PI / 180) * m_acceleration * deltaTime;
         m_velocity.y += sin(m_angle * PI / 180) * m_acceleration * deltaTime;
      }
      else
         m_state = "inactive";
      //border_bounce(canvasSize);

      // Update last update time for the next iteration
      lastUpdateTime = currentTime;
   }
}

void CAsteroid::draw(cv::Mat& canvas) {
   // Draw the blue circle representing the ship
   cv::circle(canvas, m_position, m_radius, cv::Scalar(255, 255, 255), cv::FILLED);
   cv::circle(canvas, m_position, m_radius - 2, cv::Scalar(m_scalarColor, m_scalarColor, m_scalarColor), cv::FILLED);
 }