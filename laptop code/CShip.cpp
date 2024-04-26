// CGameObject.cpp
#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include "stdafx.h"

#include "Constants.h"
#include "CGameObject.h"
#include "CShip.h"
#include "CMissile.h"   

// constructor
CShip::CShip(cv::Point position, std::string shape, std::string color, std::vector<CGameObject*>& missileObjects, std::vector<CGameObject*>& allObjects)
   : CGameObject(position, shape, color), gameobjects(gameobjects), missileobjects(missileObjects), allobjects(allObjects) {
   m_lives = 10;
   m_maxHealth = 100;
   m_health = m_maxHealth;
   m_angle = 90;
   m_angleVelocity = 0;
   m_acceleration = 0;
   m_missileCount = 0;
   m_missileStack = 20;
   m_objectType = "ship";
}

// destructor
CShip::~CShip() {
   // destructor 
}


void CShip::rotate_left() {
   m_angleVelocity = -700;
}
void CShip::rotate_right() {
   m_angleVelocity = 700;
}

void CShip::stop_rotation() {
   m_angleVelocity = 0;
}

void CShip::accelerate() {
   m_acceleration = 500;
}
void CShip::deccelerate() {
   m_acceleration = 0;
}

void CShip::update(cv::Rect& canvasSize) {
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

void CShip::draw(cv::Mat& canvas) {
   // Calculate the points of the triangle
   double angleRad = m_angle * PI / 180;
   double arrowLength = m_radius; // Set arrow length to match the radius of the circle
   double arrowWidth = m_radius * 0.4; // Adjust arrow width as desired

   cv::Point tip(
      m_position.x + arrowLength * cos(angleRad),
      m_position.y + arrowLength * sin(angleRad)
   );

   cv::Point leftWing(
      m_position.x + arrowWidth * cos(angleRad + PI / 2),
      m_position.y + arrowWidth * sin(angleRad + PI / 2)
   );

   cv::Point rightWing(
      m_position.x + arrowWidth * cos(angleRad - PI / 2),
      m_position.y + arrowWidth * sin(angleRad - PI / 2)
   );

   // Draw the arrowhead in white color
   cv::line(canvas, tip, leftWing, cv::Scalar(255, 255, 255), 2);
   cv::line(canvas, leftWing, rightWing, cv::Scalar(255, 255, 255), 2);
   cv::line(canvas, rightWing, tip, cv::Scalar(255, 255, 255), 2);

   // Draw the health bar
   drawHealthBar(canvas, m_position);

   // Draw ejecting lines if acceleration is not zero
   if (m_acceleration != 0) {
      // Calculate the position of the back of the ship
      cv::Point back(
         m_position.x - arrowLength * cos(angleRad),
         m_position.y - arrowLength * sin(angleRad)
      );

      // Draw red and yellow lines ejecting out from the back
      cv::line(canvas, back, leftWing, cv::Scalar(0, 0, 255), 2); // Red line
      cv::line(canvas, back, rightWing, cv::Scalar(0, 255, 255), 2); // Yellow line
   }

}




void CShip::fire() {

   for (auto i: {-10, 0, 10}) {
      CMissile* newMissile = new CMissile(m_position, "shape", "color");

      // Set the missile's initial position, angle, velocity, and canvas size (if needed)
      // Fire the missile
      newMissile->fire(m_position, m_angle + i, m_velocity, cv::Rect(0, 0, 1200, 900));

      // Append the new missile object to the missile object vector
      missileobjects.push_back(newMissile);
   }
}