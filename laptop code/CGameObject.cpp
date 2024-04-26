// CGameObject.cpp
#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include "stdafx.h"

#include "Constants.h"
#include "CGameObject.h"

CGameObject::CGameObject(cv::Point2f position, std::string shape, std::string color) {
   m_position = position;
   m_velocity = cv::Point2f(0, 0);
   m_maxVelocity = cv::Point2f(5, 5);
   m_speed = 5;
   m_angleVelocity = 5;
   m_acceleration = 0.05;

   m_shape = shape;
   m_color = color;
   m_angle = 0;

   m_state = "active";
   m_objectType = "gameobject";
   m_behavior = "neutral";

   m_health = 100;
   m_maxHealth = 100;
   m_lives = 20;
   m_radius = 20;
   // Initialize last update time to the current time
   lastUpdateTime = std::chrono::high_resolution_clock::now();
}


CGameObject::~CGameObject() {
   // destructor
}

bool CGameObject::is_collision(CGameObject& other) {
   // check if hypotenuse is less than the sum of radius of objects
   double distance = hypotf(m_position.x - other.m_position.x, m_position.y - other.m_position.y);
   return distance < m_radius + other.m_radius;
}

bool CGameObject::is_border_collision(cv::Rect& canvasSize) {
   // Check if the game object's circle shape touches the edges of the canvas

   // Check left border
   if (m_position.x - m_radius < canvasSize.x) {
      return true;
   }
   // Check right border
   if (m_position.x + m_radius > canvasSize.x + canvasSize.width - 1) {
      return true;
   }

   // Check top border
   if (m_position.y - m_radius < canvasSize.y) {
      return true;
   }

   // Check bottom border
   if (m_position.y + m_radius > canvasSize.y + canvasSize.height - 1) {
      return true;
   }

   return false;
}

void CGameObject::border_bounce(cv::Rect& canvasSize) {
   // Check if the object hits the left or right wall
   if (m_position.x - m_radius < canvasSize.x) {
      m_position.x = canvasSize.x + m_radius;
      m_velocity.x *= -1; // Reverse the horizontal velocity
   }
   if (m_position.x + m_radius > canvasSize.x + canvasSize.width - 1) {
      m_position.x = canvasSize.x + canvasSize.width - 1 - m_radius;
      m_velocity.x *= -1; // Reverse the horizontal velocity
   }

   // Check if the object hits the top or bottom wall
   if (m_position.y - m_radius < canvasSize.y) {
      m_position.y = canvasSize.y + m_radius;
      m_velocity.y *= -1; // Reverse the vertical velocity
   }
   if (m_position.y + m_radius > canvasSize.y + canvasSize.height - 1) {
      m_position.y = canvasSize.y + canvasSize.height - 1 - m_radius;
      m_velocity.y *= -1; // Reverse the vertical velocity
   }
}





void CGameObject::bounce(CGameObject& other) {
   cv::Point tempVelocity = m_velocity;
   m_velocity = other.m_velocity;
   other.m_velocity = tempVelocity;
}

void CGameObject::update(cv::Rect& canvasSize) {
   // change direction
   m_angle += m_angleVelocity;

   // Check for border collision before updating the position
   if (!is_border_collision(canvasSize)) {
      // add to current Point coords
      m_position += m_velocity;

      // separate x and y components of velocity
      m_velocity.x += cos(m_angle * PI / 180) * m_acceleration;
      m_velocity.y += sin(m_angle * PI / 180) * m_acceleration;
   }
}

void CGameObject::draw(cv::Mat& im) {

}

void CGameObject::drawHealthBar(cv::Mat& im, cv::Point2f sprite_position) {
    // Define the dimensions and position of the health meter
    int meterWidth = 60;
    int meterHeight = 10;
    int meterPadding = 2;
    cv::Point2f meterPosition(sprite_position.x - meterWidth / 2, sprite_position.y + m_radius + meterPadding);

    // Calculate the remaining health percentage
    double healthPercentage = static_cast<double>(m_health) / m_maxHealth;

    // Calculate the width of the filled portion of the health meter
    int filledWidth = static_cast<int>(meterWidth * healthPercentage);

    // Determine the color of the health meter based on the player's health
    cv::Scalar meterColor;
    if (healthPercentage <= 0.1) {
        meterColor = cv::Scalar(0, 0, 255); // Red color when health is below 10%
    } else if (healthPercentage <= 0.5) {
        meterColor = cv::Scalar(0, 255, 255); // Yellow color when health is below 50%
    } else {
        meterColor = cv::Scalar(0, 255, 0); // Green color for normal health
    }

    // Draw the outline of the health meter
    cv::rectangle(im, meterPosition, cv::Point2f(meterPosition.x + meterWidth, meterPosition.y + meterHeight), cv::Scalar(255, 255, 255), 1);

    // Draw the filled portion of the health meter with the determined color
    cv::rectangle(im, meterPosition, cv::Point2f(meterPosition.x + filledWidth, meterPosition.y + meterHeight), meterColor, cv::FILLED);

}

void CGameObject::fire() {

}

void CGameObject::takes_damage(double damage) {
   m_health -= damage;
   if (m_health <= 0) {
      m_health = m_maxHealth;
      m_lives -= 1;
      if (m_lives <= 0) {
         m_state = "inactive";
         // Set health to 0 to prevent negative health values
         m_health = 0;
      }
   }
}

cv::Point2f CGameObject::rotatePoint(cv::Point2f pt, cv::Point2f center, double angle_rad) {
   double x = cos(angle_rad) * (pt.x - center.x) - sin(angle_rad) * (pt.y - center.y) + center.x;
   double y = sin(angle_rad) * (pt.x - center.x) + cos(angle_rad) * (pt.y - center.y) + center.y;
   return cv::Point2f(x, y);
}