#pragma once // Avoid multiple inclusion
#include "../sensors/ImuSensor.hpp"
#include "../sensors/GnssSensor.hpp"
#include "../processing/ProcessingUnit.hpp"
#include "../fdir/Fdir.hpp"
#include "../logging/Logger.hpp"

// Simulator class
class Simulator 
{
    public:
        // Constructor
        Simulator(std::vector<std::shared_ptr<ImuSensor>> imu_sensors,
                  std::vector<std::shared_ptr<GnssSensor>> gnss_sensors, 
                  std::shared_ptr<ProcessingUnit> processing_unit, 
                  std::shared_ptr<Fdir> fdir);

        // Destructor 
        ~Simulator() = default;

        // Start the simulation
        void start();

        // Stop the simulation
        void stop();

        // IMU sensors fault injection
        void injectImuFaults(bool enable);

        // GNSS sensors fault injection
        void injectGnssFaults(bool enable);

    private:
        void run();
        std::vector<std::shared_ptr<ImuSensor>> imu_sensors_;   // Vector of IMU sensors
        std::vector<std::shared_ptr<GnssSensor>> gnss_sensors_; // Vector of GNSS sensors
        std::unordered_map<std::string, double> gnss_freq_;     // Map of GNSS sensor frequencies
        std::shared_ptr<ProcessingUnit> processing_unit_;       // Processing unit for data processing
        std::shared_ptr<Fdir> fdir_;                            // Fault detection, isolation, and recovery
        std::thread simulation_thread_;                         // Thread for running the simulation
        bool running_;                             // Flag to control the simulation state
        std::mutex simulation_mutex_;                           // Mutex for thread-safe access to simulation state
};