/** @file Hardware.hpp
 *  Platform Hardware class for consolidating hardware control.
 *
 *  @authors Ants-Oskar Mäesalu, Kristjan Kanarbik, Meelik Kiik
 *  @version 0.1
 *  @date 4 December 2015
 */

#ifndef HAL_HARDWARE_H
#define HAL_HARDWARE_H

#include "tuum_platform.hpp"

#include "RTX485.hpp"

#include "Camera.hpp"
#include "MotionControl.hpp"
#include "MainBoard.hpp"
#include "RefereeListener.hpp"

namespace tuum {
namespace hal {

  // Hardware controllers RS485 bus communication handle.
  static RTX485 HWBus;

  inline void hw_bus_write(RTX485::Message msg) {
    HWBus.sendCommand(msg.id, msg.data);
  }

  inline void hw_bus_register(RTX485::SignalParams sigInfo) {
    HWBus.registerDevice(sigInfo);
  }

  class Hardware {
    public:
      Hardware();

      void init();
      void setup();
      void process();

      Camera* getCamera();
      Camera* getFrontCamera();
      Camera* getBackCamera();

      MotionControl* getMotionControl();

      // Deprecated
      MotionControl* getMotorControl();

      MainBoard* getMainBoard();

      RefereeListener* getRefListener();

    private:
      MotionControl m_motCtl;

      Camera *m_frontCamera;
      Camera *m_backCamera;

      int m_dribbler;
      int m_coilGun;
      int m_ballSensor;
      int m_switches;

      MainBoard m_mainBoard;

      RefereeListener m_refereeListener;

  };

}}

#endif // HAL_HARDWARE_H
