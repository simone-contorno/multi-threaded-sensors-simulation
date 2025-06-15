#pragma once
#include <string>  
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <optional>
#include <fstream>
#include "../sensors/ImuSensor.hpp"
#include "../sensors/GnssSensor.hpp"
#include "../logging/Logger.hpp"

// Output data structure (timestamp, IMU, GNSS, validity)
struct ProcessingOutput 
{
    Sensor::Timestamp timestamp;
    double attitude_rate_x;
    double attitude_rate_y;
    double attitude_rate_z;
    double last_pos_x;
    double last_pos_y;
    double last_pos_z;
    bool valid_imu;
    bool valid_gnss;
};

class ProcessingUnit 
{
    public:
        // Constructor
        ProcessingUnit(
            std::vector<std::shared_ptr<ImuSensor>> imu_sensors, 
            std::vector<std::shared_ptr<GnssSensor>> gnss_sensors, 
            double frequency
        );

        // Destructor (default behaviour)
        ~ProcessingUnit() = default;

        // Start processing unit
        void start();

        // Stop processing unit
        void stop();
        
        // Retrieve last data from sensors
        ProcessingOutput getSensorData();

        // Get last output
        ProcessingOutput getLastOutput() const { return last_output_; };

    private:
        // Processing unit loop
        void run();

        // Retrieve attitude rate from IMU data
        std::array<std::optional<double>, 3> getAttitudeRate(
            std::vector<std::array<std::optional<double>, 3>> imu_data, 
            std::vector<std::optional<Sensor::Timestamp>> imu_timestamps
        );

        std::vector<std::shared_ptr<ImuSensor>> imu_sensors_;       // IMU sensors
        std::vector<std::shared_ptr<GnssSensor>> gnss_sensors_;     // GNSS sensor
        double frequency_;                                          // Processing frequency
        std::thread thread_;                                        // Simulation thread
        bool running_;                                 // Thread control flag
        std::mutex output_mutex_;                                   // Output mutex
        ProcessingOutput last_output_;                              // Last processed output
        std::string data_directory_;                                // Data directory path
        std::ofstream imu_file_;                                    // IMU CSV file stream
        std::ofstream gnss_file_;                                   // GNSS CSV file stream
};