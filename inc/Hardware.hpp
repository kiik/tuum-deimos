/** @file Hardware.hpp
 *  Platform Hardware class for consolidating hardware control.
 *
 *  @authors Ants-Oskar Mäesalu, Kristjan Kanarbik, Meelik Kiik
 *  @version 0.1
 *  @date 4 December 2015
 */

#ifndef HAL_HARDWARE_H
#define HAL_HARDWARE_H

#include <opencv2/opencv.hpp>

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


  struct fps_t
  {
    time_ms_t t0, dt, dt_avg;
    double p;

    fps_t():
      t0(millis()), dt(0)
    {
      p = 0.02;
    }

    void tick(time_ms_t t = 0)
    {
      if(t == 0) t = millis();

      dt = t - t0;

      // dt_new -> n projection
      //p = cv::max((double)(dt / 1000), (double)0.005);

      if(dt_avg == 0) dt_avg = dt;
      dt_avg = dt_avg * (1 - p) + dt * p;

      t0 = t;
    }
  };

  class Hardware {
    public:
      struct Stats {
        time_ms_t frameDelta, frameDeltaAvg, hwDelta, hwDeltaAvg;
        float frameDeltaFilter, hwDeltaFilter;

        std::string toString()
        {
          return tuum::format(
            "{.frameDelta = {%lums, ~%luHz, f=%.3f}, .hwDelta = {%lums, ~%luHz, f=%.3f} }",
            frameDelta, (1000 / frameDeltaAvg), frameDeltaFilter,
            hwDelta, (1000 / hwDeltaAvg), hwDeltaFilter
          );
        }
      };

    public:
      Hardware();

      void init();
      void setup();
      void process();

      Camera* getCamera();
      Camera* getFrontCamera();
      Camera* getBackCamera();

      int readFrame(cv::Mat&);

      MotionControl* getMotionControl();

      // Deprecated
      MotionControl* getMotorControl();

      MainBoard* getMainBoard();

      RefereeListener* getRefListener();

      Stats readStats();

      void pitcherSet(uint8_t, uint8_t);
      Vec2i pitcherGet();

    private:
      MotionControl m_motCtl;

      Camera *m_frontCamera;
      Camera *m_backCamera;

      cv::VideoCapture *m_cvCamera;

      int m_dribbler;
      int m_coilGun;
      int m_ballSensor;
      int m_switches;

      uint8_t m_pitchSpeed, m_pitchAngle;

      MainBoard m_mainBoard;

      RefereeListener m_refereeListener;

      soft_clk_t m_clk_50Hz, m_clk_30Hz, m_clk_dbg;
      fps_t m_fps_vs, m_fps_hw;
      bool m_fps_dbg_en;
  };

}}

#endif // HAL_HARDWARE_H
