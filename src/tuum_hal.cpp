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

    senseTimer.setPeriod(3000);
    senseTimer.start();
  }


  void process() {

    if(senseTimer.isTime()) {
      //hw.getMainBoard()->senseBall();
      printf("SWITCH 4: '%i'\n",hw.getMainBoard()->getSwitch(3));
      senseTimer.start();
    }

    hw.process();
  }

}}
