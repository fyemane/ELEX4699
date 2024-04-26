#include "stdafx.h"
#include "CControl.h"

#include "Serial.h"
#include <conio.h>


CControl::CControl() {
   // Constructor
}

CControl::~CControl() {
   // Destructor
}

void CControl::init_com(int comport) {
   // Initialize the serial port
  /* std::string comPortName = "COM" + std::to_string(comport);
   _com.open(comPortName.c_str());*/
   auto_detect_microcontroller();
}

bool CControl::get_data(int type, int channel, int& result) {
   // Implement the serial communication protocol for GET
   std::string command = "G " + std::to_string(type) + " " + std::to_string(channel) + "\n";

   _com.write(command.c_str(), command.length());

   std::string response = read_response();

   // Parse the response and update the result
   std::istringstream iss(response);
   std::string cmd;
   int receivedType, receivedChannel, receivedValue;
   iss >> cmd >> receivedType >> receivedChannel >> receivedValue;

   if (cmd == "A" && receivedType == type && receivedChannel == channel) {
      result = receivedValue;
      return true; // Command succeeded
   }

   return false; // Invalid response
}

bool CControl::set_data(int type, int channel, int val) {
   // Implement the serial communication protocol for SET
   std::string command = "S " + std::to_string(type) + " " + std::to_string(channel) + " " + std::to_string(val) + "\n";
   _com.write(command.c_str(), command.length());

   std::string response = read_response();

   // Check if the response is valid
   if (!response.empty()) {
      // Parse the response and check for success
      std::istringstream iss(response);
      std::string cmd;
      int receivedType, receivedChannel, receivedValue;
      iss >> cmd >> receivedType >> receivedChannel >> receivedValue;

      if (cmd == "S" && receivedType == type && receivedChannel == channel && receivedValue == val) {
         return true; // Command succeeded
      }
   }

   return false;
}


bool CControl::get_analog(int channel, int& result) {
   // Helper function to get analog input as a percentage of full scale
   return get_data(ANALOG, channel, result);
}


bool CControl::get_digital(int channel, int& result) {
   // Function to read a digital input without debounce
   return get_data(DIGITAL, channel, result);
}

bool CControl::set_digital(int channel, int val) {
   return set_data(DIGITAL, channel, val);
}

bool CControl::get_servo(int channel, int& result) {
   // Function to get the current position of the servo
   return get_data(SERVO, channel, result);
}

bool CControl::set_servo(int channel, int position) {
   // Function to set the position of the servo
   return set_data(SERVO, channel, position);
}

bool CControl::get_button(int channel, int& val) {
   // Function to read a digital input and debounce it with a 1-second timeout
   int result;
   if (get_data(DIGITAL, channel, result)) {
      // Initial read
      bool lastState = result;
      bool currentState = lastState;

      // Start time for debounce timeout
      double startTime = cv::getTickCount();

      // Debounce loop
      while ((cv::getTickCount() - startTime) / cv::getTickFrequency() < 1.0) {
         if (get_data(DIGITAL, channel, result)) {
            currentState = result;

            // Check for a stable state for at least 1 second
            if (currentState == lastState) {
               // Delay for debounce
               cv::waitKey(1);  // Adjust the delay time as needed
               val = currentState;
               return true; // Return if executed
            }
         }
         // Update last state
         lastState = currentState;
      }
   }
   return false; // Command timed out or unstable state
}


// Auto-detect the correct serial port
bool CControl::auto_detect_microcontroller() {
   const int max_ports_to_scan = 9;  // Adjust as needed
   int i, temp = 0;
   std::string comPortName;

   _com.flush();
   while (true) {
      for (i = 1; i <= max_ports_to_scan; i++) {
         comPortName = "COM" + std::to_string(i);
         _com.open(comPortName.c_str());

         if (_com.is_open()) {
            std::cout << "COM" << i << " is open for communication" << std::endl;
            return true;
         }
         else {
            std::cout << "COM" << i << " is in use, check to communication" << std::endl;
         }
      }
      //std::cout << "Microcontroller not found. retrying..." << std::endl;
   }
}

std::string CControl::read_response() {
   // Helper function to read response from the serial port
   std::string response;
   char buffer[2];

   // Read until newline character or timeout (1 second)
   double start_time = cv::getTickCount();
   while (buffer[0] != '\n' && (cv::getTickCount() - start_time) / cv::getTickFrequency() < 1.0) {
      if (_com.read(buffer, 1) > 0) {
         response += buffer[0];
      }
   }

   if (response.empty() || response.find(' ') == std::string::npos) {
      auto_detect_microcontroller();
   }
   return response;
}

