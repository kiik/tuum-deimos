/** @file tuum_hal.hpp
 *  Harware platform module declaration.
 *
 *  @authors Meelik Kiik
 *  @version 0.1
 */

#include "hal.hpp"

#include "MotorDriver.hpp"
#include "MotorControl.hpp"
#include "MainBoard.hpp"
#include "Hardware.hpp"

namespace tuum { namespace hal {

  extern Hardware hw;

  void setup();
  void process();

  bool MotorTest, DribblerTest, CoilTest;

}}

#endif
