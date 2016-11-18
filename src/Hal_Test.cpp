/** @file tuum_hal.cpp
 *  Hardware platform module.
 *
 *  @authors Meelik Kiik
 *  @version 0.1
 */

#include "tuum_hal.hpp"

namespace tuum { namespace hal {

  Hardware hw;

  Timer senseTimer;

  void setup() {
    hw.init();
    hw.setup();

    TestDone = false;
  }


  void process() {
    if(!TestDone) {
      hw.getMotorControl()->omniDrive(25, 0, 0);
      usleep(1000);
      hw.getMotorControl()->omniDrive(25, 90, 0);
      hw.getMotorControl()->omniDrive(0, 0, 0);

      hw.getMainBoard()->startDribbler(0.1);
      hw.getMainBoard()->startDribbler(0.15);
      hw.getMainBoard()->stopDribbler();

      hw.getMainBoard()->coilKick();

      TestDone = true;
    }

    RTXLOG("Test complete.");

    hw.process();
  }

}}
