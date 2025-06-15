#include "Simulator.hpp"
#include <iostream>

Simulator::Simulator(
    std::vector<std::shared_ptr<ImuSensor>> imu_sensors,
    std::vector<std::shared_ptr<GnssSensor>> gnss_sensors, 
    std::shared_ptr<ProcessingUnit> processing_unit, 
    std::shared_ptr<Fdir> fdir)
    : imu_sensors_(imu_sensors), 
      gnss_sensors_(gnss_sensors), 
      processing_unit_(processing_unit), 
      fdir_(fdir), 
      running_(false) 
{
}

void Simulator::start() 
{
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    Logger::log(Logger::Level::Info, "[Simulator] Starting simulation");

    // Check if the simulation is already running
    if (running_) {
        return; 
    }
    
    running_ = true;
    simulation_thread_ = std::thread(&Simulator::run, this);
}

void Simulator::stop() 
{
    std::lock_guard<std::mutex> lock(simulation_mutex_);
    Logger::log(Logger::Level::Info, "[Simulator] Stopping simulation");

    // Check if the simulation is running
    if (!running_) {
        Logger::log(Logger::Level::Warning, "[Simulator] Simulation is not running");
        return; // Not running
    }
    
    running_ = false;
    //if (simulation_thread_.joinable())
    //    simulation_thread_.join(); // Wait for the thread to finish
    
    // Stop all sensors 
    for (const auto& imu_sensor : imu_sensors_) {
        if (imu_sensor->isRunning()) 
            Logger::log(Logger::Level::Info, "[Simulator] Stopping IMU sensor: " + imu_sensor->getName());
            imu_sensor->stop();
    }

    for (const auto& gnss_sensor : gnss_sensors_) {
        if (gnss_sensor->isRunning()) 
            Logger::log(Logger::Level::Info, "[Simulator] Stopping GNSS sensor: " + gnss_sensor->getName());
            gnss_sensor->stop();
    }

    // Stop the processing unit and FDIR
    processing_unit_->stop();
    fdir_->stop();

    // Wait for thread to finish
    if (simulation_thread_.joinable())
        simulation_thread_.join();
}

// Inject faults into IMU sensors
void Simulator::injectImuFaults(bool enable) 
{
    Logger::log(Logger::Level::Info, "[Simulator] Injecting IMU faults: " + std::string(enable ? "Enabled" : "Disabled"));
    for (const auto& imu_sensor : imu_sensors_) {
        imu_sensor->injectFault(enable);
        if (enable)
            imu_sensor->stop();
    }
}

// Inject faults into GNSS sensors
void Simulator::injectGnssFaults(bool enable) 
{
    Logger::log(Logger::Level::Info, "[Simulator] Injecting GNSS faults: " + std::string(enable ? "Enabled" : "Disabled"));
    for (const auto& gnss_sensor : gnss_sensors_) {
        gnss_sensor->injectFault(enable);
        if (enable)
        {
            // Save the GNSS freq
            gnss_freq_[gnss_sensor->getName()] = gnss_sensor->getFrequency();
            
            // Set a different frequency to simulate fault
            gnss_sensor->setFrequency(2.0); 
        }
        else
            // Set the frequency back to the original one
            gnss_sensor->setFrequency(gnss_freq_[gnss_sensor->getName()]);
    }
}

void Simulator::run() 
{
    std::lock_guard<std::mutex> lock(simulation_mutex_);

    // Simulate sensor data generation
    Logger::log(Logger::Level::Info, "[Simulator] Starting IMU sensors");
    for (const auto& imu_sensor : imu_sensors_) { // IMU
        imu_sensor->start();
    }

    Logger::log(Logger::Level::Info, "[Simulator] Starting GNSS sensors");
    for (const auto& gnss_sensor : gnss_sensors_) { // GNSS
        gnss_sensor->start();
    }

    // Run the processing unit
    Logger::log(Logger::Level::Info, "[Simulator] Starting Processing Unit");
    processing_unit_->start();

    // Run the FDIR system
    Logger::log(Logger::Level::Info, "[Simulator] Starting FDIR");
    fdir_->start();
}