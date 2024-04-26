// CGameObject.cpp
#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include <random>
#include "stdafx.h"

#include "Constants.h"
#include "CGameObject.h"
#include "Citem.h" 

// constructor
CItem::CItem(cv::Point position, std::string shape, std::string color)
   : CGameObject(position, shape, color) {
   m_lives = 1;
   m_maxHealth = 1;
   m_health = m_maxHealth;

   m_velocity = cv::Point2f(0, 0);
   m_acceleration = 0;
   m_angle = 0;
   m_angleVelocity = 0;

   m_state = "active";
   m_behavior = "passive";
   m_itemTypes = { "plus" , "heart"};
   m_objectType = m_itemTypes.at(rand() % 2);
}

// destructor
CItem::~CItem() {
   // destructor 
}



void CItem::update(cv::Rect& canvasSize) {
   if (m_state == "active") {
      // Get the current time
      auto currentTime = std::chrono::high_resolution_clock::now();

      // Calculate the time difference since the last update
      std::chrono::duration<double> timeDiff = currentTime - lastUpdateTime;
      double deltaTime = timeDiff.count(); // Delta time in seconds

      // change direction
      m_angle += m_angleVelocity * deltaTime;

      //std::cout << "Inside class: angle " << m_angle << "\t";

      // Check for border collision before updating the position
      if (!is_border_collision(canvasSize)) {
         // separate x and y components of velocity
         m_position.x += m_velocity.x * deltaTime + 0.5 * cos(m_angle * PI / 180) * m_acceleration * pow(deltaTime, 2);
         m_position.y += m_velocity.y * deltaTime + 0.5 * sin(m_angle * PI / 180) * m_acceleration * pow(deltaTime, 2);

         // Update velocity based on acceleration
         m_velocity.x += cos(m_angle * PI / 180) * m_acceleration * deltaTime;
         m_velocity.y += sin(m_angle * PI / 180) * m_acceleration * deltaTime;
      }

      border_bounce(canvasSize);
      // Update last update time for the next iteration
      lastUpdateTime = currentTime;
   }
}

void CItem::draw(cv::Mat& canvas) {
   if (m_state == "active") {
      // Define the size and thickness of the plus sign
      int size = 20;
      int thickness = 5;

      // Convert the angle to radians
      double angle_rad = m_angle * PI / 180.0;

      // Calculate the endpoints of the horizontal line
      cv::Point2f p1(m_position.x - std::min(static_cast<int>(size / 2), static_cast<int>(m_radius)), m_position.y);
      cv::Point2f p2(m_position.x + std::min(static_cast<int>(size / 2), static_cast<int>(m_radius)), m_position.y);

      // Calculate the endpoints of the vertical line
      cv::Point2f p3(m_position.x, m_position.y - std::min(static_cast<int>(size / 2), static_cast<int>(m_radius)));
      cv::Point2f p4(m_position.x, m_position.y + std::min(static_cast<int>(size / 2), static_cast<int>(m_radius)));

      // Rotate the points around the position
      cv::Point2f center = m_position;
      p1 = rotatePoint(p1, center, angle_rad);
      p2 = rotatePoint(p2, center, angle_rad);
      p3 = rotatePoint(p3, center, angle_rad);
      p4 = rotatePoint(p4, center, angle_rad);

      if (m_objectType == "plus") {
         // Draw the horizontal and vertical lines
         cv::line(canvas, p1, p2, cv::Scalar(0, 255, 0), thickness);
         cv::line(canvas, p3, p4, cv::Scalar(0, 255, 0), thickness);
      }
      else if (m_objectType == "heart") {
         cv::line(canvas, p1, p2, cv::Scalar(0, 0, 255), thickness);
         cv::line(canvas, p3, p4, cv::Scalar(0, 0, 255), thickness);
      }
   }
}