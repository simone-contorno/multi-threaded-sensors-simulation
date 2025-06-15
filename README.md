# Multi-threaded sensors simulation

A multi-threaded simulation for synchronized sensor data generation, fusion, logging, and fault detection & isolation (FDIR).

## Table of Contents
- [Overview](#overview)
- [Architecture](#architecture)
  - [Components](#components)
  - [Communication](#communication)
- [Implementation Details](#implementation-details)
  - [Sensor Components](#sensor-components)
  - [Processing Unit](#processing-unit)
  - [FDIR System](#fdir-system)
  - [Logging](#logging)
- [Building the Project](#building-the-project)
- [Running the Simulation](#running-the-simulation)
- [Use Cases](#use-cases)
  - [Case 1: Nominal Operation](#case-1-nominal-operation)
  - [Case 2: IMU Failure](#case-2-imu-failure)
  - [Case 3: GNSS Failure](#case-3-gnss-failure)
- [Data Generation and Visualization](#data-generation-and-visualization)
- [Possible Improvements](#possible-improvements)
- [License](#license)

## Overview

This project simulates a sensor fusion system using parallel multi-threading and synchronization among IMU and GNSS sensors, a processing unit, and an FDIR component. It demonstrates:
- Multi-threaded sensor simulation (IMU & GNSS)
- Real-time data fusion and processing
- Fault Detection, Isolation, and Recovery (FDIR) system
- Synchronized data logging and visualization

## Architecture
### Components
- 3x IMU Sensors (100Hz)
- 2x GNSS Sensors (20Hz)
- 1x Processing Unit (50Hz)
- 1x FDIR Component (20Hz)

The FDIR component rate has been choosen with the lowest sensor rate value.

### Communication
Components communicate through thread-safe data structures and mutexes. The system uses a publisher-subscriber pattern where:
- Sensors publish data at their specified frequencies
- Processing Unit subscribes to sensor data
- FDIR monitors both sensors and processing unit

## Implementation Details

### Sensor Components
- IMU sensors generate attitude rate data with configurable noise
- GNSS sensors produce position data with configurable noise
- Both implement fault injection capabilities for testing

### Processing Unit
- Averages valid IMU measurements
- Uses latest GNSS measurement
- Implements data validation and aging checks
- Logs filtered output data to CSV files

### FDIR System
- Monitors sensor health
- Detects missing data conditions
- Raises alarms for component failures

### Logging
- The simulator uses a thread-safe `Logger` class to record events, warnings, errors, and debug information
- All log messages are written to a log file
- Log levels include: Debug, Info, Warning, and Error
- Logging is initialized at startup and can be used by all components for diagnostics and traceability

## Building the Project
```bash
mkdir build && cd build
cmake ..
make
```

## Running the Simulation
Execute the binary:
```bash
./multi-threaded-sensors-simulation
```

Available commands:

0. Help - Show commands
1. Start - Begins simulation
2. Stop - Stops simulation
3. Use Case 1 - Runs nominal case for 10 seconds
4. Use Case 2 - Simulates IMU failures
5. Use Case 3 - Simulates GNSS failures
6. Exit - Terminates program

## Use Cases

### Case 1: Nominal Operation
- Duration: 10 seconds
- All sensors operating normally
- Demonstrates normal data flow and processing

### Case 2: IMU Failure
- Duration: 5 seconds
- Simulates sequential IMU sensor failures
- Validate IMU missing data detection

### Case 3: GNSS Failure
- Duration: 5 seconds
- Simulates GNSS data dropout
- Validates position data aging detection

## Data Generation and Visualization
### Sensor Data
- Each simulation run creates a timestamped folder in `data/`
- IMU and GNSS data are stored in CSV format
- Data includes timestamps, measurements, and validity flags

### Data Visualization
The `plot_sensor_data.py` script generates:
- 3D visualization of IMU attitude rates
- 3D visualization of GNSS positions
- Plots are saved as PNG files in the data folder
- Color gradients indicate temporal progression

### Logging System
- Logs are stored in the `log/` directory
- Each run creates a timestamped log file

### UML Documentation
- System architecture is documented in PlantUML format
- Class diagram shows component relationships
- Generated diagram available in `uml`

## Possible Improvements
Potential Improvements are:
- Run the interface in a separate terminal in order to guarantee a better readability
- Improve thread synchronization and thread-safe rules
- Implement configuration file (e.g. JSON) to better configure the simulation

## License

See the [MIT](LICENSE) license for details.
