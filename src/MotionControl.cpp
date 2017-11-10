
#include <iostream>
#include <cmath>

#include "tuum_logger.hpp"

#include "MotionControl.hpp"

namespace tuum {
namespace hal {

  const char CMD_MOTOR_SPEED[] = "sd";

  MotionControl::MotionControl():
    RTX485::Device::Device()
  {

  }

  void MotionControl::init(RTX485::WriteHandle wHandle) {
    int id_seq, ix;
    for(ix=0, id_seq=1; ix < MOTOR_COUNT; ix++, id_seq++) {
      m_motorIDs[ix] = id_seq;
    }

    RTX485::Device::init(wHandle);

    RTXLOG("Ready.");
  }

  std::string MotionControl::getSpeedCmd(int v) {
    std::stringstream out;
    out << CMD_MOTOR_SPEED;
    out << v;
    return out.str();
  }

  void MotionControl::omniDrive(float spd, float dir, float rot) {
    /**
    int spd1 = speed * sin(angle + M_PI / 4.0) + rot;
    int spd2 = speed * -sin(angle - M_PI / 4.0) + rot;
    int spd3 = speed * -sin(angle + M_PI / 4.0) + rot;
    int spd4 = speed * sin(angle - M_PI / 4.0) + rot;

    int speeds[4] = {spd1, spd2, spd3, spd4};
    for (int ix=0; ix < MOTOR_COUNT; ix++) {
      send({m_motorIDs[ix], getSpeedCmd(speeds[ix])});
      usleep(10);
    }*/

    // mm / s, radians, deg / s
    send({1, format("om,%.2f,%.2f,%.2f", spd, dir, rot)});
  }

  void MotionControl::stop() {
    omniDrive(0, 0, 0);
  }

  void MotionControl::aiMove(Vec2d mv) {
    RTXLOG(format("#TODO: (%.2f, %.2f)", mv.x, mv.y));
  }

}}
