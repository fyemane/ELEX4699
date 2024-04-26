#include "stdafx.h"

// CBase4618.cpp
#include "CBase4618.h"

void CBase4618::run() {
   do {
      update();
      draw();
   } while (cv::waitKey(1) != 'q');
}