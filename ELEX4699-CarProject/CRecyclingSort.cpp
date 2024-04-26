// CPong.cpp
#include <algorithm>
#include <chrono>
#include <thread>

#include "CControlPi.h"
#include "CRecyclingSort.h"
#include "Constants.h"
#include "cvui.h"
#include "server.h"

CRecyclingSort::CRecyclingSort(const cv::Size& canvasSize, int comPort) {
    _canvas = cv::Mat::zeros(canvasSize, CV_8UC3);
    persistentCanvas = cv::Mat::zeros(canvasSize, CV_8UC3);  // Initialize persistent canvas
    cvui::init("RecyclingSort");              // Initialize cvui here
    m_canvasSize = canvasSize;
    //   _control.init_com(comPort);

    gui_position = cv::Point(10, 10);
    resetFlag = false, exitFlag = false, _thread_exit = false, _mutex_thread_exit = false;
    joyThreshold = 512;

    fps = 30;
    delay = 27;

    buttonPressed1 = 0, buttonValue1 = 0;  ///< when button1 is pressed
    buttonPressed2 = 0, buttonValue2 = 0;  ///< when button2 is pressed
    buttonPressed3 = 0, buttonValue3 = 0;  ///< when button3 is pressed
    buttonPressed4 = 0, buttonValue4 = 0;  ///< when button4 is pressed
    buttonPressed5 = 0, buttonValue5 = 0;  ///< when button5 is pressed

   color1_lowerH = 16, color1_lowerS = 100, color1_lowerV = 100, color1_upperH = 42, color1_upperS = 255, color1_upperV = 255;
   color2_lowerH = 154, color2_lowerS = 100, color2_lowerV = 100, color2_upperH = 168, color2_upperS = 255, color2_upperV = 255;
   color3_lowerH = 40, color3_lowerS = 100, color3_lowerV = 100, color3_upperH = 75, color3_upperS = 255, color3_upperV = 255;
   color4_lowerH = 77, color4_lowerS = 100, color4_lowerV = 100, color4_upperH = 130, color4_upperS = 255, color4_upperV = 255;

    color1_lowerBound = cv::Scalar(color1_lowerH, color1_lowerS, color1_lowerV), color1_upperBound = cv::Scalar(color1_upperH, color1_upperS, color1_upperV);
    color2_lowerBound = cv::Scalar(color2_lowerH, color2_lowerS, color2_lowerV), color2_upperBound = cv::Scalar(color2_upperH, color2_upperS, color2_upperV);
    color3_lowerBound = cv::Scalar(color3_lowerH, color3_lowerS, color3_lowerV), color3_upperBound = cv::Scalar(color3_upperH, color3_upperS, color3_upperV);
    color4_lowerBound = cv::Scalar(color4_lowerH, color4_lowerS, color4_lowerV), color4_upperBound = cv::Scalar(color4_upperH, color4_upperS, color4_upperV);

    lowerBounds = std::vector<cv::Scalar> {color1_lowerBound, color2_lowerBound, color3_lowerBound, color4_lowerBound};
    upperBounds = std::vector<cv::Scalar> {color1_upperBound, color2_upperBound, color3_upperBound, color4_upperBound};

    color1BallCount = 0;
    color2BallCount = 0;
    color3BallCount = 0;
    color4BallCount = 0;
    min_area_threshold = 8000;

    color1BallDetected = false;
     color2BallDetected = false;
    color3BallDetected = false;
    color4BallDetected = false;
    anyBallDetected = false;
    autosort = true;

    gui_vertspace = 60;
    servorun = 0;

    servo1position = 1500;
    servo2position = 1500;
    servo1target = 720;
    servo2target1 = 500;
    servo2target2 = 1000;
    servo2target3 = 1500;
    servo2target4 = 2000;

    servoincrement = 3;


    vid.open(0, cv::CAP_V4L2);
    if (!vid.isOpened()) {
        std::cerr << "Error: Unable to open camera" << std::endl;
    }

    // Create window for displaying canvas
    cv::namedWindow("RecyclingSort", cv::WINDOW_AUTOSIZE);

}

CRecyclingSort::~CRecyclingSort() { vid.release(); }

void CRecyclingSort::update() {
     // Check if 5 seconds have passed since the last update
    static auto lastUpdateTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastUpdateTime).count();

    // Check if the delay for sorting another ball has passed
    static auto lastSortTime = std::chrono::steady_clock::now();
    auto sortElapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSortTime).count();


    color1_lowerBound = cv::Scalar(color1_lowerH, color1_lowerS, color1_lowerV), color1_upperBound = cv::Scalar(color1_upperH, color1_upperS, color1_upperV);
    color2_lowerBound = cv::Scalar(color2_lowerH, color2_lowerS, color2_lowerV), color2_upperBound = cv::Scalar(color2_upperH, color2_upperS, color2_upperV);
    color3_lowerBound = cv::Scalar(color3_lowerH, color3_lowerS, color3_lowerV), color3_upperBound = cv::Scalar(color3_upperH, color3_upperS, color3_upperV);
    color4_lowerBound = cv::Scalar(color4_lowerH, color4_lowerS, color4_lowerV), color4_upperBound = cv::Scalar(color4_upperH, color4_upperS, color4_upperV);

    lowerBounds = std::vector<cv::Scalar> {color1_lowerBound, color2_lowerBound, color3_lowerBound, color4_lowerBound};
    upperBounds = std::vector<cv::Scalar> {color1_upperBound, color2_upperBound, color3_upperBound, color4_upperBound};

    vid >> vidFrame;
    resizeImage(vidFrame, 350, resizedFrame);

    processedFrame1 = processImage(resizedFrame, color1_lowerBound, color1_upperBound);  // Process the video frame
    processedFrame2 = processImage(resizedFrame, color2_lowerBound, color2_upperBound);  // Process the video frame
    processedFrame3 = processImage(resizedFrame, color3_lowerBound, color3_upperBound);  // Process the video frame
    processedFrame4 = processImage(resizedFrame, color4_lowerBound, color4_upperBound);  // Process the video frame

    if (!autosort) {
        _control.set_digital(L8_WHITELED, 0);
        servorun = 0;

        if (sortElapsedTime >= 2) {
            if (buttonValue1 && buttonPressed1) {
                // Button is pressed, set reset flag only if the game has ended
                color1BallCount++;
                colorToSort = 1;
                servorun = 1;
                buttonPressed1 = 0;
                lastSortTime = currentTime;

            }
            if (!buttonValue1) {
                buttonPressed1 = 1;
            }

            if (buttonValue2 && buttonPressed2) {
                // Button is pressed, set reset flag only if the game has ended
                color2BallCount++;
                colorToSort = 2;
                servorun = 1;
                buttonPressed2 = 0;
                lastSortTime = currentTime;

            }
            if (!buttonValue2) {
                buttonPressed2 = 1;
            }

            if (buttonValue3 && buttonPressed3) {
                // Button is pressed, set reset flag only if the game has ended
                color3BallCount++;
                colorToSort = 3;

                servorun = 1;

                buttonPressed3 = 0;
                lastSortTime = currentTime;
            }
            if (!buttonValue3) {
                buttonPressed3 = 1;
            }

            if (buttonValue4 && buttonPressed4) {
                // Button is pressed, set reset flag only if the game has ended
                color4BallCount++;
                colorToSort = 4;

                servorun = 1;
                buttonPressed4 = 0;
                lastSortTime = currentTime;
            }
            if (!buttonValue4) {
                buttonPressed4 = 1;
            }

        }

    } else {
        _control.set_digital(L8_WHITELED, 1);


          if (sortElapsedTime >= 2) {
            // Reset the timer
            lastSortTime = currentTime;
    // Check if any color ball is detected
            if (anyBallDetected) {
                // Increment the corresponding color ball count and turn on the LED
                if (color1BallDetected) {
                    colorToSort = 1;
                    servorun = 1;
                    color1BallCount++;
                } if (color2BallDetected) {
                    colorToSort = 2;
                    servorun = 1;
                    color2BallCount++;
                } if (color3BallDetected) {
                    colorToSort = 3;
                    servorun = 1;

                    color3BallCount++;
                } if (color4BallDetected) {
                    colorToSort = 4;

                    servorun = 1;

                    color4BallCount++;
                }
            } else {
                // If no ball is detected, turn off all LEDs
                colorToSort = 0;
            }

        }
    }
        if (buttonValue5 && buttonPressed5) {
            // Button is pressed, set reset flag only if the game has ended
            autosort = !autosort;
            buttonPressed5 = 0;
        }
        if (!buttonValue5) {
            buttonPressed5 = 1;
        }

    sort_server.get_cmd(commands_list);

    if (!commands_list.empty()) {
        for (const auto& str: commands_list) {
            std::cout << "Received: " << str << std::endl;
        }
        if (commands_list.at(0) == "S 0 1 \n") {
            autosort = 1;
        }
        if (commands_list.at(0) == "S 0 0 \n") {
            autosort = 0;
        }
        if (commands_list.at(0) == "S 1 0 \n") {
            if (!autosort) {
                colorToSort = 1;
                servorun = 1;
                color1BallCount++;
            }
        }
        if (commands_list.at(0) == "S 1 1 \n") {
            if (!autosort) {
                colorToSort = 2;
                servorun = 1;
                color2BallCount++;
            }
        }
        if (commands_list.at(0) == "S 1 2 \n") {
            if (!autosort) {
                colorToSort = 3;
                servorun = 1;
                color3BallCount++;
            }
        }
        if (commands_list.at(0) == "S 1 3 \n") {
            if (!autosort) {
                colorToSort = 4;
                servorun = 1;
                color4BallCount++;
            }
        }
        if (commands_list.at(0) == "G 0 \n") {
            sort_server.send_string(std::to_string(autosort));
        }
        if (commands_list.at(0) == "G 1 0 \n") {
            sort_server.send_string(std::to_string(color1BallCount));
        }
        if (commands_list.at(0) == "G 1 1 \n") {
            sort_server.send_string(std::to_string(color2BallCount));
        }
        if (commands_list.at(0) == "G 1 2 \n") {
            sort_server.send_string(std::to_string(color3BallCount));
        }
        if (commands_list.at(0) == "G 1 3 \n") {
            sort_server.send_string(std::to_string(color4BallCount));
        }
        if (commands_list.at(0) == "G 2 \n") {
        }

    }


}

void CRecyclingSort::resizeImage(cv::Mat& image, int targetWidth, cv::Mat& resizedImage) {
    double aspectRatio = static_cast<double>(image.cols) / image.rows;
    int targetHeight = static_cast<int>(targetWidth / aspectRatio);
    cv::resize(image, resizedImage, cv::Size(targetWidth, targetHeight));
}


void CRecyclingSort::handleUserInput() {
    _control.get_digital(L8_BUTTON1, buttonValue1);
    _control.get_digital(L8_BUTTON2, buttonValue2);
    _control.get_digital(L8_BUTTON3, buttonValue3);
    _control.get_digital(L8_BUTTON4, buttonValue4);
    _control.get_digital(L8_BUTTON5, buttonValue5);
}

cv::Mat CRecyclingSort::processImage(cv::Mat& image, cv::Scalar lowerBound, cv::Scalar upperBound) {
    // Convert the image from BGR to HSV color space
    cv::Mat hsvImage;
    cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

    // Create a mask to isolate pixels within the specified color range
    cv::Mat mask;
    cv::inRange(hsvImage, lowerBound, upperBound, mask);

    // Apply morphological operations to remove noise and enhance regions
    cv::Mat morphKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, morphKernel);  // Opening to remove noise
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, morphKernel); // Closing to fill gaps

    // Find contours in the filtered image
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Sort contours based on area in descending order
    std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
        return cv::contourArea(a) > cv::contourArea(b);
    });

    // Create a blank canvas to draw the largest contour
    cv::Mat largestContourImage = cv::Mat::zeros(image.size(), CV_8UC3);

    // Draw the bounding boxes around the contours
    for (size_t i = 0; i < contours.size(); ++i) {
        cv::Rect bbox = cv::boundingRect(contours[i]);
        cv::rectangle(largestContourImage, bbox, cv::Scalar(.5 * (lowerBound[0] + upperBound[0]), 255, 255), 2); // Draw bounding box
    }

    // Draw the largest contour onto the canvas
    if (!contours.empty()) {
        cv::drawContours(largestContourImage, contours, 0, cv::Scalar(.5 * (lowerBound[0] + upperBound[0]), 255, 255), -1); // Draw filled contour
        // Get the area of the largest contour
        double largestContourArea = cv::contourArea(contours[0]);

        // Check if the area is large enough to trigger the sort bin function
        if (largestContourArea > min_area_threshold) { // Define MIN_AREA_THRESHOLD as the minimum area required to trigger the sort bin function
            // Call the sort bin function based on the color detected
            anyBallDetected = true;
            if (lowerBound == color1_lowerBound && upperBound == color1_upperBound) {
                // Call sort bin function for color 1
                color1BallDetected = true;
                color2BallDetected = false;
                color3BallDetected = false;
                color4BallDetected = false;
            } else if (lowerBound == color2_lowerBound && upperBound == color2_upperBound) {
                // Call sort bin function for color 2
                 color1BallDetected = false;
                color2BallDetected = true;
                color3BallDetected = false;
                color4BallDetected = false;
            } else if (lowerBound == color3_lowerBound && upperBound == color3_upperBound) {
                // Call sort bin function for color 3
                 color1BallDetected = false;
                color2BallDetected = false;
                color3BallDetected = true;
                color4BallDetected = false;
            } else if (lowerBound == color4_lowerBound && upperBound == color4_upperBound) {
                // Call sort bin function for color 4
                color1BallDetected = false;
                color2BallDetected = false;
                color3BallDetected = false;
                color4BallDetected = true;
            }
            // Add conditions for other colors if needed
        } else {
            anyBallDetected = false;
            colorToSort = 0;
        }
    }

    cv::cvtColor(largestContourImage, largestContourImage, cv::COLOR_HSV2BGR);

    return largestContourImage;
}

void CRecyclingSort::sortColorBin() {
    if (colorToSort == 0) {
        _control.set_digital(L8_RGBLED_YEL, 0);
        _control.set_digital(L8_RGBLED_RED, 0);
        _control.set_digital(L8_RGBLED_GRN, 0);
        _control.set_digital(L8_RGBLED_BLU, 0);
    }


    if (colorToSort == 1) {
        _control.set_digital(L8_RGBLED_YEL, 1);
        _control.set_digital(L8_RGBLED_RED, 0);
        _control.set_digital(L8_RGBLED_GRN, 0);
        _control.set_digital(L8_RGBLED_BLU, 0);
    //
        if (servorun) {
            while (servo2position <= servo2target1) {
                servo2position += servoincrement;
                _control.set_servo(L8_SERVO2,servo2position);
            }
            while (servo2position > servo2target1) {
                servo2position -= servoincrement;
                _control.set_servo(L8_SERVO2, servo2position);
            }
            for (servo1position = 510; servo1position < servo1target; servo1position += servoincrement) {
                _control.set_servo(L8_SERVO1, servo1position);
            }
            for (servo1position = servo1target; servo1position > 510; servo1position -= servoincrement) {
                _control.set_servo(L8_SERVO1, servo1position);

            }

                }
            servorun = 0;

            }

    if (colorToSort == 2) {
        _control.set_digital(L8_RGBLED_YEL, 0);
        _control.set_digital(L8_RGBLED_RED, 1);
        _control.set_digital(L8_RGBLED_GRN, 0);
        _control.set_digital(L8_RGBLED_BLU, 0);

        if (servorun) {
            while (servo2position <= servo2target2) {
                servo2position += servoincrement;
                _control.set_servo(L8_SERVO2, servo2position);
            }
            while (servo2position > servo2target2) {
                servo2position -= servoincrement;
                _control.set_servo(L8_SERVO2, servo2position);
            }

            for (servo1position = 500; servo1position < servo1target; servo1position += servoincrement) {
                _control.set_servo(L8_SERVO1, servo1position);
            }
            for (servo1position = servo1target; servo1position > 500; servo1position -= servoincrement) {
                _control.set_servo(L8_SERVO1, servo1position);

            }



            servorun = 0;

        }

    }
    if (colorToSort == 3) {
        _control.set_digital(L8_RGBLED_YEL, 0);
        _control.set_digital(L8_RGBLED_RED, 0);
        _control.set_digital(L8_RGBLED_GRN, 1);
        _control.set_digital(L8_RGBLED_BLU, 0);

        if (servorun) {

            while (servo2position <= servo2target3) {
                servo2position += servoincrement;
                _control.set_servo(L8_SERVO2, servo2position);
            }
            while (servo2position > servo2target3) {
                servo2position -= servoincrement;
                _control.set_servo(L8_SERVO2, servo2position);
            }
            for (servo1position = 500; servo1position < servo1target; servo1position += servoincrement) {
                _control.set_servo(L8_SERVO1, servo1position);
            }
            for (servo1position = servo1target; servo1position > 500; servo1position -= servoincrement) {
                _control.set_servo(L8_SERVO1, servo1position);

            }


            servorun = 0;

        }


    }
    if (colorToSort == 4) {
        _control.set_digital(L8_RGBLED_YEL, 0);
        _control.set_digital(L8_RGBLED_RED, 0);
        _control.set_digital(L8_RGBLED_GRN, 0);
        _control.set_digital(L8_RGBLED_BLU, 1);

        if (servorun) {
            while (servo2position <= servo2target4) {
                servo2position += servoincrement;
                _control.set_servo(L8_SERVO2, servo2position);
            }
            while (servo2position > servo2target4) {
                servo2position -= servoincrement;
                _control.set_servo(L8_SERVO2, servo2position);
            }
            for (servo1position = 500; servo1position < servo1target; servo1position += servoincrement) {
                _control.set_servo(L8_SERVO1, servo1position);
            }
            for (servo1position = servo1target; servo1position > 500; servo1position -= servoincrement) {
                _control.set_servo(L8_SERVO1, servo1position);

            }



            servorun = 0;

        }

    }

}



void CRecyclingSort::drawBorder(cv::Mat& canvas, cv::Rect& imageRegion) {
    cv::rectangle(canvas, cv::Rect(imageRegion.x - 1, imageRegion.y - 1, imageRegion.width + 2, imageRegion.height + 2), cv::Scalar(255, 255, 255), cv::FILLED);
}

void CRecyclingSort::drawImage(cv::Mat& canvas, cv::Point position, cv::Mat& image, int color) {
    cv::Rect imageRegion(position.x, position.y, image.cols, image.rows);
    drawBorder(canvas, imageRegion);
    image.copyTo(canvas(imageRegion));
    if (color != 0) {
        if (color == 1)
            cv::putText(_canvas, "Color " + std::to_string(color) + ": " + std::to_string(color1_lowerH) + "-" + std::to_string(color1_upperH), cv::Point(position.x, position.y + imageRegion.height - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
        else if (color == 2)
            cv::putText(_canvas, "Color " + std::to_string(color) + ": " + std::to_string(color2_lowerH) + "-" + std::to_string(color2_upperH), cv::Point(position.x, position.y + imageRegion.height - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
        else if (color == 3)
            cv::putText(_canvas, "Color " + std::to_string(color) + ": " + std::to_string(color3_lowerH) + "-" + std::to_string(color3_upperH), cv::Point(position.x, position.y + imageRegion.height - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
        else
            cv::putText(_canvas, "Color " + std::to_string(color) + ": " + std::to_string(color4_lowerH) + "-" + std::to_string(color4_upperH), cv::Point(position.x, position.y + imageRegion.height - 5), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

        }
    }




void CRecyclingSort::draw() {
    auto calc_start = std::chrono::steady_clock::now();
    // Set update rate at 30 Hz (100 ms per loop)
    auto end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay);
    _canvas = persistentCanvas.clone();  // clear canvas


    if (anyBallDetected) {
        if (color1BallDetected) {
            cv::putText(_canvas, "Color 1 Detected", cv::Point(m_canvasSize.width - 2 * resizedFrame.cols - 3, resizedFrame.rows - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

        }
        if (color2BallDetected) {
            cv::putText(_canvas, "Color 2 Detected", cv::Point(m_canvasSize.width - 2 * resizedFrame.cols - 3, resizedFrame.rows - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

        }
        if (color3BallDetected) {
            cv::putText(_canvas, "Color 3 Detected", cv::Point(m_canvasSize.width - 2 * resizedFrame.cols - 3, resizedFrame.rows - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

        }
        if (color4BallDetected) {
            cv::putText(_canvas, "Color 4 Detected", cv::Point(m_canvasSize.width - 2 * resizedFrame.cols - 3, resizedFrame.rows - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

        }
    } else {
            cv::putText(_canvas, "No color detected", cv::Point(m_canvasSize.width - 2 * resizedFrame.cols - 3, resizedFrame.rows - 10), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);

    }






    // video frame
    drawImage(_canvas, cv::Point(m_canvasSize.width - resizedFrame.cols, 0), resizedFrame);
    // color 1, top left
    drawImage(_canvas, cv::Point(m_canvasSize.width - 2 * resizedFrame.cols - 3, resizedFrame.rows), processedFrame1, 1);
    // color 2 top right
    drawImage(_canvas, cv::Point(m_canvasSize.width - 1 * resizedFrame.cols - 3, resizedFrame.rows), processedFrame2, 2);
    // color 3 bottom left
    drawImage(_canvas, cv::Point(m_canvasSize.width - 2 * resizedFrame.cols - 3, resizedFrame.rows * 2), processedFrame3, 3);
    // color 3 bottom right
    drawImage(_canvas, cv::Point(m_canvasSize.width - 1 * resizedFrame.cols - 3, resizedFrame.rows * 2), processedFrame4, 4);

////


    // Display the canvas image
    // Draw GUI elements in the top-left corner
    cvui::window(_canvas, gui_position.x, gui_position.y, 700, 850,
                 "RecyclingSort");

    // autosort
    cvui::checkbox(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 5, "Autosort", &autosort);

    if (!autosort) {


    if (cvui::button(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 6, "Sort Bin 1")) {
        colorToSort = 1;
        servorun = 1;
        color1BallCount++;
    }
    if (cvui::button(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 7, "Sort Bin 2")) {
            colorToSort = 2;
        servorun = 1;
        color2BallCount++;
    }
    if (cvui::button(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 8, "Sort Bin 3")) {
            colorToSort = 3;

        servorun = 1;
        color3BallCount++;
    }
    if (cvui::button(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 9, "Sort Bin 4")) {
            colorToSort = 4;

        servorun = 1;
        color4BallCount++;
    }

    }






    cvui::text(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 1, "Color1 Ball Count: " + std::to_string(color1BallCount));
    cvui::text(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 2, "Color2 Ball Count: " + std::to_string(color2BallCount));
    cvui::text(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 3, "Color3 Ball Count: " + std::to_string(color3BallCount));
    cvui::text(_canvas, gui_position.x + 470, gui_position.y + gui_vertspace * 4, "Color4 Ball Count: " + std::to_string(color4BallCount));


    if (cvui::button(_canvas, gui_position.x + 430, gui_position.y + 750, "Exit")) {
        std::exit(0);
    }
//
   if (cvui::button(_canvas, gui_position.x + 530, gui_position.y + 750, "Reset")) {
        // Reset button pressed, set reset flag
        color1BallCount = 0;
        color2BallCount = 0;
        color3BallCount = 0;
        color4BallCount = 0;
    }





    // Color 1 trackbars
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace, "Color 1 Lower H");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 1, 200, &color1_lowerH, 0, 180);
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 2, "Color 1 Lower S");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 2, 200, &color1_lowerS, 0, 255);
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 3, "Color 1 Lower V");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 3, 200, &color1_lowerV, 0, 255);

    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace, "Color 1 Upper H");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 1, 200, &color1_upperH, 0, 180);
    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 2, "Color 1 Upper S");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 2, 200, &color1_upperS, 0, 255);
    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 3, "Color 1 Upper V");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 3, 200, &color1_upperV, 0, 255);

    // Color 2 trackbars
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 4, "Color 2 Lower H");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 4, 200, &color2_lowerH, 0, 180);
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 5, "Color 2 Lower S");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 5, 200, &color2_lowerS, 0, 255);
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 6, "Color 2 Lower V");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 6, 200, &color2_lowerV, 0, 255);

    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 4, "Color 2 Upper H");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 4, 200, &color2_upperH, 0, 180);
    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 5, "Color 2 Upper S");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 5, 200, &color2_upperS, 0, 255);
    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 6, "Color 2 Upper V");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 6, 200, &color2_upperV, 0, 255);

    // Color 3 trackbars
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 7, "Color 3 Lower H");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 7, 200, &color3_lowerH, 0, 180);
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 8, "Color 3 Lower S");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 8, 200, &color3_lowerS, 0, 255);
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 9, "Color 3 Lower V");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 9, 200, &color3_lowerV, 0, 255);

    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 7, "Color 3 Upper H");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 7, 200, &color3_upperH, 0, 180);
    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 8, "Color 3 Upper S");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 8, 200, &color3_upperS, 0, 255);
    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 9, "Color 3 Upper V");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 9, 200, &color3_upperV, 0, 255);

    // Color 4 trackbars
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 10, "Color 4 Lower H");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 10, 200, &color4_lowerH, 0, 180);
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 11, "Color 4 Lower S");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 11, 200, &color4_lowerS, 0, 255);
    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 12, "Color 4 Lower V");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 12, 200, &color4_lowerV, 0, 255);

    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 10, "Color 4 Upper H");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 10, 200, &color4_upperH, 0, 180);
    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 11, "Color 4 Upper S");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 11, 200, &color4_upperS, 0, 255);
    cvui::text(_canvas, gui_position.x + 270, gui_position.y + gui_vertspace * 12, "Color 4 Upper V");
    cvui::trackbar(_canvas, gui_position.x + 210, gui_position.y + gui_vertspace * 12, 200, &color4_upperV, 0, 255);

    cvui::text(_canvas, gui_position.x + 70, gui_position.y + gui_vertspace * 13, "Min Area");
    cvui::trackbar(_canvas, gui_position.x + 10, gui_position.y + gui_vertspace * 13, 200, &min_area_threshold, 0, 20000);

    cv::imshow("vidframe", resizedFrame);
    sort_server.set_txim(resizedFrame);



    // Display the canvas on the window
    if (!exitFlag) {
        // Update the CVUI GUI system
        cvui::update();
        cv::imshow("RecyclingSort", _canvas);
    }

    // Sleep if time remaining
    std::this_thread::sleep_until(end_time);

    auto calc_end = std::chrono::steady_clock::now();
    auto calc_elapsed = std::chrono::duration<double>(calc_end - calc_start);
    fps = 1 / calc_elapsed.count();
//    std::cout << "FPS: " << fps << std::endl;
}

void CRecyclingSort::run() {

    std::thread serverlisten(&CRecyclingSort::serverstart_thread, this);
    std::thread t1(&CRecyclingSort::servo_thread, this);
    serverlisten.detach();
    t1.detach();
    do {
        update();
        draw();
        handleUserInput();
    } while (cv::waitKey(1) != 'q');

   _thread_exit = true;
    sort_server.stop();
   t1.join();
   serverlisten.join();
}


void CRecyclingSort::servo_thread(CRecyclingSort* ptr) {
    while (ptr->_thread_exit == false) {
        ptr->sortColorBin();
    }
}


void CRecyclingSort::serverstart_thread(CRecyclingSort* ptr) {
    while (ptr-> _thread_exit == false) {
        ptr->sort_server.start(4618);
    }
}
