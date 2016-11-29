
#include <sstream>
#include <stdlib.h>
#include <stdint.h>

#include "MainBoard.hpp"

namespace tuum { namespace hal {

  const char CMD_BALL_SENSE[] = "bl";
  const char CMD_DRIBBLER[] = "dr";
  const char CMD_CHRG[] = "chrg";
  const char CMD_KICK[] = "kick";

  MainBoard::MainBoard() {
    id = 1;

    m_ballSensorState = 0;
    m_dribblerState = 0;
    m_coilKickActive = 0;

    m_coilKickCharge.setPeriod(300);
    m_coilKickCooldown.setPeriod(1500);

    m_updateTimer.setPeriod(300);
    m_updateTimer.start();

    senseBall();
  }

  void MainBoard::init(RTX485::WriteHandle wHandle, RTX485::SignalService sigRegister) {
    RTX485::DeviceCallback cb = std::bind1st(std::mem_fun(&MainBoard::signal), this);
    RTX485::Device::init(wHandle, sigRegister, cb);
  }

  void MainBoard::signal(RTX485::Message m) {
    std::string buf;
    size_t ix = m.data.find(",");
    if(ix == -1) return;

    buf = m.data.substr(0, ix);

    if(buf == "bl") {
      std::string value = m.data.substr(ix + 1, 1);
      m_ballSensorState = atoi(value.c_str());
    }
  }

  void MainBoard::run() {
    if(m_updateTimer.isTime()) {
      if(m_dribblerState) {
        startDribbler();
      }
      m_updateTimer.start();
    }

    if(m_coilKickActive && m_coilKickCharge.isTime()) {
      if(m_coilChargeLevel > 4) {
        m_coilKickActive = false;
	m_coilKickCooldown.start();
      } else {
      	chargeCoil();
        m_coilChargeLevel++;
	if(m_coilChargeLevel == 1) releaseCoil();
	else chargeCoil();
	m_coilKickCharge.start();
      }

    }
  }

  bool MainBoard::getBallSensorState() {
    return m_ballSensorState;
  }

  void MainBoard::senseBall() {
    send({id, CMD_BALL_SENSE});
  }

  void MainBoard::coilKick() {
    if(!m_coilKickActive && m_coilKickCooldown.isTime()) {
      senseBall();
      chargeCoil();
      m_coilKickActive = true;
      m_coilChargeLevel = 0;
      m_coilKickCharge.start();
    }
    if(m_coilKickActive && m_coilKickCharge.isTime()){
      releaseCoil();
      m_coilKickActive = false;
      m_coilKickCooldown.start();

    }
  }

  void MainBoard::chargeCoil() {
    send({id, CMD_CHRG});
  }

  void MainBoard::releaseCoil() {
    send({id, CMD_KICK});
  }

  void MainBoard::startDribbler(float v) {
    m_dribblerState = 1;
    send({1, format("dr,%.2f", v)});
  }

  void MainBoard::stopDribbler() {
    m_dribblerState = 0;
    send({id, format("dr,%.2f", 0)});
  }



}}
