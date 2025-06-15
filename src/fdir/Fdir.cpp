#include "Fdir.hpp"
#include <iostream>

// Constructor
Fdir::Fdir(std::shared_ptr<ProcessingUnit> processing_unit, double frequency) : 
    processing_unit_(processing_unit), frequency_(frequency) 
{
}

// Start the FDIR thread
void Fdir::start() 
{
    //std::cout << "[Fdir] Start" << std::endl;
    Logger::log(Logger::Level::Info, "[Fdir] Start");
    running_ = true;
    thread_ = std::thread(&Fdir::run, this);
}

// Stop the FDIR thread
void Fdir::stop() 
{
    Logger::log(Logger::Level::Info, "[Fdir] Stop");
    running_ = false;
    
    // Wait for thread to finish
    if (thread_.joinable())
        thread_.join();
}

// Add a sensor
void Fdir::addSensor(std::shared_ptr<Sensor> sensor) 
{
    Logger::log(Logger::Level::Info, "[Fdir] Adding sensor: " + sensor->getName());
    std::lock_guard<std::mutex> lock(fdir_mutex_);
    sensors_[sensor->getName()] = {sensor, 0, sensor->getFrequency()}; // Add the sensor to the map
}

// Remove a sensor
void Fdir::removeSensor(const std::string& name) 
{
    Logger::log(Logger::Level::Info, "[Fdir] Removing sensor: " + name);
    std::lock_guard<std::mutex> lock(fdir_mutex_);
    sensors_.erase(name); // Remove the sensor from the map
}

// Processing unit loop
void Fdir::run() 
{
    while (running_) 
    {
        {
            std::lock_guard<std::mutex> lock(fdir_mutex_);

            // Check the sensors status
            checkSensors();

            // Check the processing unit status
            checkProcessingUnit();
        }
        
        int sleep_time_ms = static_cast<int>(1000 / frequency_);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
    }
}

// Check the sensors status
void Fdir::checkSensors() 
{
    for (auto& [name, sensor_info] : sensors_) 
    {
        auto& [sensor, counter, measurement_frequency] = sensor_info;
        
        // Check if the sensor is running
        if (!sensor->isRunning()) 
        {
            //std::cerr << "[Fdir] Sensor " << name << " is not running." << std::endl;
            continue; // Skip to the next sensor
        } 

        auto time_step = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - sensor->getLastUpdate()).count();
        if (time_step > 1000 / measurement_frequency) 
        {
            counter++;
        }
        else 
        {
            counter = 0; // Reset the counter if the sensor is working properly
        }

        // Check if the sensor has failed
        if (counter >= 3) 
        {
            Logger::log(Logger::Level::Error, "[Fdir] Sensor " + name + " did not provide any output for three consecutive nominal measurement intervals");
        }
    }
}

// Check the processing unit status
void Fdir::checkProcessingUnit() 
{
    if (!processing_unit_->getLastOutput().valid_imu || !processing_unit_->getLastOutput().valid_gnss) 
    {
        if (!valid_data_) 
        {
            Logger::log(Logger::Level::Error, "[Fdir] Processing unit has invalid data.");
            valid_data_ = true; // Set the flag to true to avoid multiple messages
        }
    }
    else 
    {
        if (valid_data_) 
        {
            Logger::log(Logger::Level::Info, "[Fdir] Processing unit has valid data.");
            valid_data_ = false; // Reset the flag when data is valid
        }
    }
}