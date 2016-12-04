
#ifndef TUUM_MAINBOARD_H
#define TUUM_MAINBOARD_H

#include "platform.hpp"

#include "RTX485.hpp"

namespace tuum { namespace hal {

  class MainBoard : public RTX485::Device {
  private:
    bool m_ballSensorState;
    bool m_dribblerState;
    bool m_coilKickActive;
    bool m_switchState;
    bool m_switchStates[4];
    bool m_coilCharged;

    Timer m_coilCooldown;

    Timer m_updateTimer;

  public:
    MainBoard();

    void init(RTX485::WriteHandle wHandle, RTX485::SignalService sigRegister);
    void signal(RTX485::Message);

    void run();

    bool getBallSensorState();
    bool getSwitch(size_t);

    void senseBall();

    void chargeCoil();
    void releaseCoil();

    // These functions manage charging/kicking cycle.
    void coilCharge();
    void coilRelease();
    void startDribbler(float = 0.2);
    void stopDribbler();

    void coilKick();
  };

}}

#endif // RTX_MAINBOARD_H
