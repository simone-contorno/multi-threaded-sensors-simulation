multi-threaded-sensors-simulation/
├── CMakeLists.txt
├── README.md
├── STRUCT.md
├── .gitignore
├── main.cpp
├── include/
│   ├── fdir/
│   │   └── Fdir.hpp
│   ├── logging/
│   │   └── Logger.hpp
│   ├── processing/
│   │   └── ProcessingUnit.hpp
│   ├── sensors/
│   │   ├── GnssSensor.hpp
│   │   ├── ImuSensor.hpp
│   │   └── Sensor.hpp
│   └── simulator/
│       └── Simulator.hpp
├── scripts/
│   └── plot_sensor_data.py
├── src/
│   ├── fdir/
│   │   └── Fdir.cpp
│   ├── logging/
│   │   └── Logger.cpp
│   ├── processing/
│   │   └── ProcessingUnit.cpp
│   ├── sensors/
│   │   ├── GnssSensor.cpp
│   │   └── ImuSensor.cpp
│   └── simulator/
│       └── Simulator.cpp
├── flowcharts/
│   ├── fdir/
│   │   └── Fdir.svg
│   ├── processing/
│   │   └── Processing.svg
│   ├── sensors/
│   │   └── Sensors.svg
│   └── simulator/
│       └── Simulator.svg
├── uml/
│   ├── uml.puml
│   └── uml.svg
├── log/
│   └── log_YYYYMMDD_HHMMSS.log
└── data/
    └── YYYYMMDD_HHMMSS_data/
        ├── imu.csv
        ├── gnss.csv
        ├── imu.png
        └── gnss.png
