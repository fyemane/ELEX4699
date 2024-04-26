/**
 * @file CControl.h
 *
 * @brief Header file for the CControl class, providing control functionalities.
 *
 * This file contains the declaration of the CControl class, which encapsulates
 * communication and control functionalities for digital/analog inputs, outputs,
 * buttons, servos, and microcontroller auto-detection.
 *
 * @author Faniel Yemane
 */
#pragma once
#include "Serial.h"

#define RGBLED_RED_PIN 39
#define RGBLED_GRN_PIN 38
#define RGBLED_BLU_PIN 37

#define BUTTON1 33
#define BUTTON2 32

#define JOYSTICK_X 2
#define JOYSTICK_Y 26
#define JOYSTICK_SEL 5

#define ACC_X 23
#define ACC_Y 24
#define ACC_Z 25

#define SERVO_PORT0 0
#define SERVO_PORT1 1
#define SERVO_PORT2 2
#define SERVO_PORT3 3

enum CommandType { DIGITAL = 0, ANALOG, SERVO };


 /**
  * @class CControl
  * @brief Provides control functionalities for interacting with a microcontroller.
  */
class CControl {
private:
   Serial _com;           ///< Serial communication object.
public:
   /**
    * @brief Constructor for CControl class.
    */
   CControl();

   /**
    * @brief Destructor for CControl class.
    */
   ~CControl();

   /**
    * @brief Initializes the communication with the microcontroller.
    * @param comport The COM port number for communication.
    */
   void init_com(int comport);

   /**
    * @brief Reads data from the microcontroller.
    * @param type Type of data (DIGITAL, ANALOG, SERVO).
    * @param channel Channel number for the data.
    * @param result Reference to store the result.
    * @return True if successful, false otherwise.
    */
   bool get_data(int type, int channel, int& result);

   /**
    * @brief Writes data to the microcontroller.
    * @param type Type of data (DIGITAL, ANALOG, SERVO).
    * @param channel Channel number for the data.
    * @param val Value to be written.
    * @return True if successful, false otherwise.
    */
   bool set_data(int type, int channel, int val);

   /**
    * @brief Reads analog data from the microcontroller.
    * @param channel Channel number for the analog input.
    * @param result Reference to store the result.
    * @return True if successful, false otherwise.
    */
   bool get_analog(int channel, int& result);

   /**
    * @brief Reads digital data from the microcontroller.
    * @param channel Channel number for the digital input.
    * @param result Reference to store the result.
    * @return True if successful, false otherwise.
    */
   bool get_digital(int channel, int& result);

   /**
    * @brief Writes digital data to the microcontroller.
    * @param channel Channel number for the digital output.
    * @param val Value to be written (0 or 1).
    * @return True if successful, false otherwise.
    */
   bool set_digital(int channel, int val);

   /**
    * @brief Get button state with debounce.
    * @param channel The channel number.
    * @param val Reference to store the debounced button state.
    * @return True if the operation is successful, false otherwise.
    */
   bool get_button(int channel, int& val);

   /**
    * @brief Reads the position of a servo motor from the microcontroller.
    * @param channel Channel number for the servo motor.
    * @param result Reference to store the servo position.
    * @return True if successful, false otherwise.
    */
   bool get_servo(int channel, int& result);

   /**
    * @brief Sets the position of a servo motor on the microcontroller.
    * @param channel Channel number for the servo motor.
    * @param position Position to be set for the servo motor.
    * @return True if successful, false otherwise.
    */
   bool set_servo(int channel, int position);

   /**
    * @brief Automatically detects the correct serial port for the microcontroller.
    * @return True if successful, false otherwise.
    */
   bool auto_detect_microcontroller();

private:
   /**
    * @brief Reads response from the microcontroller.
    * @return Response string from the microcontroller.
    */
   std::string read_response();
};
