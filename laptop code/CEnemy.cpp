// CGameObject.cpp
#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include "stdafx.h"

#include "Constants.h"
#include "CGameObject.h"
#include "CEnemy.h"
#include "CEnemyMissile.h"

void drawPolygon(cv::Mat& canvas, int numSides, double radius, cv::Point2f position, double angle, cv::Scalar& color, int thickness, bool isFilled);

CEnemy::CEnemy(cv::Point2f position, std::string shape, std::string color, std::string enemyType, std::vector<CGameObject*>& gameObjects, std::vector<CGameObject*>& enemyMissileObjects)
   : CGameObject(position, shape, color), m_enemyType(enemyType), m_gameObjects(gameObjects), m_enemyMissileObjects(enemyMissileObjects) {
   m_lives = 1;
   m_maxHealth = 10;
   m_health = m_maxHealth;

   m_radius = 30;
   m_angle = 90;
   m_angleVelocity = 0;
   m_velocity = cv::Point2f(5, 5);
   m_acceleration = 0;
   m_speed = 200;

   m_state = "active";
   m_behavior = "hostile";
   m_objectType = "enemy";
   m_enemyTypes = {"scout", "stalker", "sentry", "surveillance" };
   //m_enemyType = m_enemyTypes.at(rand() % m_enemyType.size());
   m_enemyType = "scout";

   detection_range = 300;
   shooting_range = 200;
   fire_frequency = 3;
   
}

CEnemy::~CEnemy() {

}

void CEnemy::update(cv::Rect& canvasSize) {
   if (m_state == "active") {
      // Get the current time
      auto currentTime = std::chrono::high_resolution_clock::now();

      // Calculate the time difference since the last update
      std::chrono::duration<double> timeDiff = currentTime - lastUpdateTime;
      double deltaTime = timeDiff.count(); // Delta time in seconds

      //std::cout << "Inside class: angle " << m_angle << "\t";

      for (auto it = m_gameObjects.begin(); it != m_gameObjects.end(); ) {
         //// Get the pointer to the current game object
         CGameObject* sprite = *it;
         if (sprite->get_objectType() == "ship" && objectWithinDetectionRange(*sprite)) {
            rotateTowardsPlayer(*sprite);
         }
         ++it;
      }

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

void CEnemy::rotateTowardsPlayer(const CGameObject& player) {
   // Calculate the direction vector from the enemy to the player
   cv::Point2f direction = player.get_position() - m_position;
   // Calculate the angle between the direction vector and the positive x-axis
   double angle = std::atan2(direction.y, direction.x) * 180 / PI;
   // Set the angle as the new rotation angle for the enemy
   m_angle = angle;
}

bool CEnemy::objectWithinDetectionRange(const CGameObject& object) const {
   // Calculate the distance between the enemy and the object
   double distance = cv::norm(object.get_position() - m_position);
   // Check if the distance is within the detection range for detecting the object
   return distance <= detection_range;
}

bool CEnemy::objectWithinShootingRange(const CGameObject& object) const {
   // Calculate the distance between the enemy and the object
   double distance = cv::norm(object.get_position() - m_position);
   // Check if the distance is within the shooting range
   return distance <= shooting_range;
}

void CEnemy::fireAtObject(const CGameObject& object, std::vector<CGameObject*>& missileObjects) {
   // Check if the object is within the shooting range
   if (objectWithinShootingRange(object)) {
      // Check if enough time has passed since the last fire
      auto currentTime = std::chrono::steady_clock::now();
      std::chrono::duration<double> timeDiff = currentTime - lastFireTime;
      double timeElapsed = timeDiff.count(); // Time elapsed since the last fire in seconds

      if (timeElapsed >= 1.0 / fire_frequency) { // Check if it's time to fire based on fire frequency
        
         CEnemyMissile* newEnemyMissile = new CEnemyMissile(m_position, "circle", "red");

         newEnemyMissile->fire(m_position, m_angle, m_velocity, cv::Rect(0, 0, 1200, 900));
         // Add the missile to the missile objects vector
         missileObjects.push_back(newEnemyMissile);
         // Update the last fire time
         lastFireTime = currentTime;
      }
   }
}

void CEnemy::draw(cv::Mat& canvas) {
   if (m_state == "active") {

      if (m_enemyType == "scout") {
         drawPolygon(canvas, 5, m_radius, m_position, m_angle, cv::Scalar(0, 150, 255), true, 2);
      }

      if (m_enemyType == "sentry") {
         drawPolygon(canvas, 3, m_radius, m_position, m_angle, cv::Scalar(255, 0, 255), true, 2);
      }
   }
   // Draw health bar
   drawHealthBar(canvas, m_position);
}

void drawPolygon(cv::Mat& canvas, int numSides, double radius, cv::Point2f position, double angle, cv::Scalar& color, int thickness, bool isFilled) {
   // Calculate angle increment between two consecutive vertices
   double angleIncrement = 2 * CV_PI / numSides;

   // Convert angle to radians
   double currentAngle = angle * CV_PI / 180;

   // Calculate center of the polygon
   cv::Point center(position.x, position.y);

   // Vector to store vertices of the polygon
   std::vector<cv::Point> polygonVertices;

   // Calculate vertices of the polygon
   for (int i = 0; i < numSides; ++i) {
      // Calculate vertex position
      int x = center.x + radius * cos(currentAngle);
      int y = center.y + radius * sin(currentAngle);
      polygonVertices.push_back(cv::Point(x, y));
      // Move to the next angle
      currentAngle += angleIncrement;
   }

   if (isFilled) {
      // Draw and fill the polygon on the canvas
      cv::fillConvexPoly(canvas, polygonVertices.data(), polygonVertices.size(), color);
   }
   else {
      // Draw the pentagon on the canvas
      cv::polylines(canvas, polygonVertices, true, color, thickness); // Change the color and thickness as needed
   }
}
