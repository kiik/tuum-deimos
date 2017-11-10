{
  'includes': [
    '../libtuum/hal/hal-iface.gypi'
  ],
  'cflags': [
    '--std=c++11'
  ],
  'include_dirs': [
    'inc'
  ],
  'dependencies': [
    'libenv',
    'librtxmath',
  ],
  'sources': [
    'src/Hardware.cpp',
    'src/MainBoard.cpp',
    'src/Camera.cpp',
    'src/MotionControl.cpp',
    'src/MotorDriver.cpp',
    'src/RefereeListener.cpp',
    'src/RTX485.cpp',
    'src/SerialPort.cpp',
    'src/tuum_hal.cpp',
    'src/V4L2Camera.cpp',
  ],

  'all_dependent_settings': {
    'include_dirs': ['inc'],
  },
}
