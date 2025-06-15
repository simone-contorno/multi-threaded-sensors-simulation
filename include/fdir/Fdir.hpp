#pragma once

#include "../sensors/Sensor.hpp"
#include "../processing/ProcessingUnit.hpp"
#include "../logging/Logger.hpp"
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

class Fdir 
{
    public:
        // Constructor
        Fdir(std::shared_ptr<ProcessingUnit> processing_unit, double frequency);

        // Destructor
        ~Fdir() = default;

        // Start the FDIR thread
        void start();

        // Stop the FDIR thread
        void stop();

        // Add a sensor
        void addSensor(std::shared_ptr<Sensor> sensor);

        // Remove a sensor
        void removeSensor(const std::string& name);

    private:
        // Processing unit loop
        void run();

        // Check the sensors status
        void checkSensors();

        // Check the processing unit status
        void checkProcessingUnit();

        double frequency_;
        std::shared_ptr<ProcessingUnit> processing_unit_; // Processing unit instance
        std::unordered_map<std::string, std::tuple<std::shared_ptr<Sensor>, int, double>> sensors_; // Sensor name : [Sensor pointer, counter, measurement frequency]
        std::mutex fdir_mutex_;
        bool running_;
        std::thread thread_;
        bool valid_data_ = false; // Flag to indicate if the Processing Unit data is valid
};
