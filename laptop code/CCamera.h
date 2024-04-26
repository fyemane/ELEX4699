/**
 * @file CCamera.h
 * @brief Declaration of the CCamera class.
 *
 * This file contains the declaration of the CCamera class, which represents a camera object in the game.
 */

#pragma once

 /**
  * @class CCamera
  * @brief A class representing a camera object in the game.
  *
  * This class manages the position of the camera.
  */
class CCamera {
private:
   cv::Point camerapos; ///< The position of the camera.

public:
   /**
    * @brief Constructor for CCamera.
    */
   CCamera() { ; }

   /**
    * @brief Function to get the position of the camera.
    * @return The position of the camera.
    */
   cv::Point get_camera() {
      return camerapos;
   }

   /**
    * @brief Function to set the position of the camera.
    * @param setcamera The position to set the camera to.
    */
   void set_camera(cv::Point& setcamera) { camerapos = setcamera; }
};
