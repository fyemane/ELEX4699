/**
 * @file CControlPi.h
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

#define BUTTON1 17
#define BUTTON2 22

#define JOYSTICK_X 0
#define JOYSTICK_Y 1

#include <string>

enum CommandType { DIGITAL = 0, ANALOG, SERVO };


 /**
  * @class CControl
  * @brief Provides control functionalities for interacting with a microcontroller.
  */
class CControlPi {
private:
public:
   /**
    * @brief Constructor for CControl class.
    */
   CControlPi();

   /**
    * @brief Destructor for CControl class.
    */
   ~CControlPi();

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
   void get_data(int type, int channel, int& result);

   /**
    * @brief Writes data to the microcontroller.
    * @param type Type of data (DIGITAL, ANALOG, SERVO).
    * @param channel Channel number for the data.
    * @param val Value to be written.
    * @return True if successful, false otherwise.
    */
   void set_data(int type, int channel, int val);

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
   void get_digital(int channel, int& result);

   /**
    * @brief Writes digital data to the microcontroller.
    * @param channel Channel number for the digital output.
    * @param val Value to be written (0 or 1).
    * @return True if successful, false otherwise.
    */
   void set_digital(int channel, int val);

   /**
    * @brief Get button state with debounce.
    * @param channel The channel number.
    * @param val Reference to store the debounced button state.
    * @return True if the operation is successful, false otherwise.
    */
   bool get_button(int channel, int& val);

   /**
    * @brief Reads the position ofs a servo motor from the microcontroller.
    * @param channel Channel number for the servo motor.
    * @param result Reference to store the servo position.
    * @return True if successful, false otherwise.
    */
   void get_servo(int channel, int& result);

   /**
    * @brief Sets the position of a servo motor on the microcontroller.
    * @param channel Channel number for the servo motor.
    * @param position Position to be set for the servo motor.
    * @return True if successful, false otherwise.
    */
   void set_servo(int channel, int position);

   /**
    * @brief Automatically detects the correct serial port for the microcontroller.
    * @return True if successful, false otherwise.
    */
   void auto_detect_microcontroller();

private:
   /**
    * @brief Reads response from the microcontroller.
    * @return Response string from the microcontroller.
    */
   std::string read_response();
};
