
#include <sstream>
#include <stdlib.h>
#include <stdint.h>

#include "MainBoard.hpp"

namespace tuum {
namespace hal {

  const char CMD_BALL_SENSE[] = "gbs";
  const char CMD_DRIBBLER[] = "sms";

  const char CMD_GET_THROW_ANGLE[] = "gng";
  const char CMD_SET_THROW_ANGLE[] = "sng";

  const char CMD_SWITCH[] = "sw";
  const char CMD_GETSWITCH[] = "gsw";

  const char CMD_CHRG[] = "chrg";
  const char CMD_KICK[] = "kick";

  MainBoard::MainBoard():
    m_coilCharged(false),
    m_throwSpeed(0)
  {
    id = 1;

    m_ballSensorState = 0;
    m_dribblerState = 0;
    m_coilKickActive = 0;
    m_switchState = 0;

    for(size_t i=0; i < 4; i++) {
      m_switchStates[i] = getSwitch(i);
    }

    m_coilCooldown.setPeriod(100);
    m_coilCooldown.start(5000);

    m_updateTimer.setPeriod(300);
    m_updateTimer.start();

    senseBall();
  }

  void MainBoard::init(RTX485::WriteHandle wHandle, RTX485::SignalService sigRegister) {
    RTX485::DeviceCallback cb = std::bind1st(std::mem_fun(&MainBoard::signal), this);
    RTX485::Device::init(wHandle, sigRegister, cb);

    RTXLOG("Ready.");
  }

  void MainBoard::signal(RTX485::Message m) {
    std::string buf;
    size_t ix = m.data.find(",");
    if(ix == -1) return;

    buf = m.data.substr(0, ix);

    if(buf == "bl") { //bl,1
      std::string value = m.data.substr(ix + 1, 1);
      m_ballSensorState = atoi(value.c_str());
    }

    if(buf == "sw") { //sw,3,1
      std::string sw = m.data.substr(ix+1, 1);
      std::string value = m.data.substr(ix+3, 1);
      m_switchStates[atoi(sw.c_str())] = atoi(value.c_str());
    }

    if(buf == "gsw") { //gsw,3,1
      std::string sw = m.data.substr(ix+1, 1);
      std::string value = m.data.substr(ix+3, 1);
      m_switchStates[atoi(sw.c_str())] = atoi(value.c_str());

    }
  }

  void MainBoard::run() {
    if(m_updateTimer.isTime()) {
      if(m_dribblerState) {
        startDribbler();
      }
      senseBall();
      m_updateTimer.start();
    }

    if(!m_coilCharged) {
      coilCharge();
    }

  }

  bool MainBoard::getSwitch(size_t ix) {
    send({id, CMD_GETSWITCH});
    return m_switchStates[ix];
  }

  bool MainBoard::getBallSensorState() {
    return m_ballSensorState;
  }

  void MainBoard::senseBall() {
    send({id, CMD_BALL_SENSE});
  }

  void MainBoard::coilCharge() {
    if(!m_coilCharged && m_coilCooldown.isTime()) {
      chargeCoil();
      m_coilCharged = true;
      m_coilCooldown.start(1000);
    }
  }

  void MainBoard::coilKick() {
    if(!m_coilCharged) {
      coilCharge();
      return;
    }

    if(m_coilCooldown.isTime()){
      releaseCoil();
      m_coilCharged = false;
      m_coilCooldown.start(100);
    }
  }

  void MainBoard::chargeCoil() {
    send({id, CMD_CHRG});
  }

  void MainBoard::releaseCoil() {
    send({id, CMD_KICK});
  }

  bool MainBoard::getDribblerState()
  {
    return false; //TODO
  }

  void MainBoard::startDribbler(float v)
  {

  }

  void MainBoard::stopDribbler()
  {

  }

  void MainBoard::setThrowSpeed(unsigned int percent) {
    if(percent >= 100) percent = 99;

    m_throwSpeed = percent;
    send({id, format("%s,%i", CMD_DRIBBLER, m_throwSpeed)});
  }

  void MainBoard::setThrowAngle(unsigned int percent) {
    if(percent >= 100) percent = 99;

    send({id, format("%s,%i", CMD_SET_THROW_ANGLE, percent)});
  }

  void MainBoard::initialState()
  {
    setThrowSpeed(0);
    setThrowAngle(0);
  }

  void MainBoard::pickupState()
  {
    // if(!mb->getDribblerState()) mb->startDribbler(0.4);

    if(m_throwSpeed < 10) setThrowSpeed(40);
  }

}}
