/**
 * @file CItem.h
 * @brief Declaration of the CItem class.
 *
 * This file contains the declaration of the CItem class, which represents an item object in the game.
 * It extends the CGameObject class and adds functionality specific to items.
 *
 * @author Faniel Yemane
 */

#pragma once

#include "CGameObject.h"

 /**
  * @class CItem
  * @brief A class representing an item object in the game.
  *
  * This class extends the CGameObject class and adds functionality specific to items.
  */
class CItem : public CGameObject {
private:
   std::vector<std::string> m_itemTypes; ///< Vector of possible item types.

public:
   /**
    * @brief Constructor for CItem.
    * @param position The position of the item.
    * @param shape The shape of the item.
    * @param color The color of the item.
    */
   CItem(cv::Point position, std::string shape, std::string color);

   /**
    * @brief Destructor for CItem.
    */
   ~CItem();

   /**
    * @brief Function to update the state of the item.
    * @param canvasSize The size of the canvas.
    */
   void update(cv::Rect& canvasSize) override;

   /**
    * @brief Function to draw the item on the canvas.
    * @param canvas The canvas to draw on.
    */
   void draw(cv::Mat& canvas) override;
};
