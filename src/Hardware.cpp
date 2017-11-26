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

  bool motor_test = true, pitcher_auto = true;
  time_ms_t mt_t0;

  Hardware::Hardware():
    m_frontCamera(nullptr),
    m_backCamera(nullptr),
    m_clk_30Hz(1000 / 30),
    m_clk_50Hz(1000 / 50),
    m_clk_dbg(1000),
    m_pitchSpeed(0), m_pitchAngle(0)
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

    m_cvCamera = new cv::VideoCapture(gC.getInt("Vision.FirstCameraIndex"));
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
    pitcherSet(0, 10);
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

    if(pitcher_auto && m_clk_dbg.tick(t))
    {
      if(m_fps_dbg_en) printf("Hardware::Stats == %s\n", readStats().toString().c_str());

      if(t - mt_t0 >= 3000) {
	mt_t0 = t;
        if(motor_test)
	{
          motor_test = false;
	  HWBus.sendCommand(1, "sms,20");
	}
	else
	{
	  HWBus.sendCommand(1, "sms,0");
	}
      }
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

  cv::Mat imgBuf;

  int Hardware::readFrame(cv::Mat& out)
  {
    m_cvCamera->retrieve(imgBuf);
    // out = cv::imread("./basketball_demo_00.png");
    // imgL = imgRGB[0:800,48:1328,: ]
    // imgR = imgRGB[0:800,1328:2608,: ]

    cv::Size sz = imgBuf.size();
    cv::Rect gArea(1328, 0, 2608 - 1328, 800);

    if(sz.width == 3448)
      out = imgBuf(gArea);
    else
      out = imgBuf;

    return 0;
  }

  void Hardware::pitcherSet(uint8_t relSpeed, uint8_t relAngle)
  {
    if(pitcher_auto) pitcher_auto = false;

    int v = relSpeed < 99 ? relSpeed : 99;
    int a = relAngle < 99 ? relAngle : 99;

    if(a > 0 && a < 10) a = 10;

    printf("pitcherSet(v=%i, a=%i) / %s\n", v, a, tuum::format("sng,%u", a).c_str());

    // 2 argument - "sng,{0}".format(relAngle)
    HWBus.sendCommand(1, tuum::format("sng,%i", a));
    HWBus.sendCommand(1, tuum::format("sms,%i", v));

    v += 5;
    HWBus.sendCommand(1, tuum::format("sng,%i", a));
    HWBus.sendCommand(1, tuum::format("sms,%i", v));

    m_pitchSpeed = v;
    m_pitchAngle = a;
  }

  Vec2i Hardware::pitcherGet()
  {
    return Vec2i(m_pitchSpeed, m_pitchSpeed);
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
