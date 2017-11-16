/** @file Hardware.cpp
 *  Platform Hardware class implementation.
 *
 *  @authors Ants-Oskar Mäesalu, Kristjan Kanarbik, Meelik Kiik
 *  @version 0.1
 *  @date 5 December 2015
 */

#include <iostream>
#include <fstream>

#include "tuum_platform.hpp"

#include "Hardware.hpp"

namespace tuum { namespace hal {

  const RTX485::DeviceID RTX_MAIN_BOARD_ID = 0;

  Hardware::Hardware():
    m_frontCamera(nullptr),
    m_backCamera(nullptr),
    m_clk_30Hz(1000 / 30),
    m_clk_50Hz(1000 / 50),
    m_clk_dbg(1000)
  {

  }

  void Hardware::init() {
    RTXLOG("Loading video streams...\n");
    std::ifstream frontCameraDevice(gC.getStr("Vision.FirstCamera"));

    /*
    if (frontCameraDevice.good())
      m_frontCamera = new Camera(gC.getStr("Vision.FirstCamera"), CAMERA_WIDTH, CAMERA_HEIGHT);
    if(m_frontCamera != nullptr) m_frontCamera->setup();
    */

    /*
    std::ifstream backCameraDevice(gC.getStr("Vision.SecondCamera"));
    if (backCameraDevice.good())
      m_backCamera = new Camera(gC.getStr("Vision.SecondCamera"), CAMERA_WIDTH, CAMERA_HEIGHT);
    */

    m_cvCamera = new cv::VideoCapture(1);
    m_cvCamera->grab();

    if(gC.getStr("HW.Active") == "Y") {
      RTXLOG(format("Loading modules... (%s, %i)\n", gC.getStr("HWBus.Port").c_str(), gC.getInt("HWBus.Baud")));
      HWBus.init(gC.getStr("HWBus.Port").c_str(), gC.getInt("HWBus.Baud"));
      m_refereeListener.init(gC.getStr("RefModule.Port").c_str(), gC.getInt("RefModule.Baud"));

      m_motCtl.init(hw_bus_write);
      m_mainBoard.init(hw_bus_write, hw_bus_register);
    } else {
      RTXLOG("Hardware not active.\n", LOG_WARN);
    }
  }

  void Hardware::setup() {

  }

  void Hardware::process() {
    time_ms_t t = millis();

    if(m_clk_30Hz.tick(t))
    {
      m_fps_vs.tick(t);

      if(m_frontCamera != nullptr) m_frontCamera->process();
      if(m_cvCamera != nullptr) m_cvCamera->grab();
    }

    if(m_clk_50Hz.tick(t))
    {
      m_fps_hw.tick(t);

      m_mainBoard.run();
    }

    if(m_clk_dbg.tick(t))
    {
      if(m_fps_dbg_en) printf("Hardware::Stats == %s\n", readStats().toString().c_str());
    }
  }

  Hardware::Stats Hardware::readStats()
  {
    Hardware::Stats out;

    out.frameDelta = m_fps_vs.dt;
    out.frameDeltaAvg = m_fps_vs.dt_avg;
    out.frameDeltaFilter = m_fps_vs.p;

    out.hwDelta = m_fps_hw.dt;
    out.hwDeltaAvg = m_fps_hw.dt_avg;
    out.hwDeltaFilter = m_fps_hw.p;

    return out;
  }

  int Hardware::readFrame(cv::Mat& out)
  {
    m_cvCamera->retrieve(out);

    cv::Rect gArea(48, 0, 1280, 800);
    out = out(gArea);

    return 0;
  }

  Camera* Hardware::getCamera() {
    return m_frontCamera;
  }

  Camera* Hardware::getFrontCamera() {
    return m_frontCamera;
  }

  Camera* Hardware::getBackCamera() {
    return nullptr;//m_backCamera;
  }

  MainBoard* Hardware::getMainBoard() {
    return &m_mainBoard;
  }

  MotionControl* Hardware::getMotionControl() {
    return &m_motCtl;
  }

  MotionControl* Hardware::getMotorControl() {
    return &m_motCtl;
  }

  RefereeListener* Hardware::getRefListener() {
    return &m_refereeListener;
  }

}}
