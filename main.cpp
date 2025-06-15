#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>

// Includes your project's headers
#include "ImuSensor.hpp"
#include "GnssSensor.hpp"
#include "ProcessingUnit.hpp"
#include "Fdir.hpp"
#include "Simulator.hpp"
#include "Logger.hpp"

// IMU Configuration
std::unordered_map<std::string, std::tuple<double, int, double>> imu_sensors_config = {
    {"imu1", {100.0, 1000, 0.01}},
    {"imu2", {100.0, 1000, 0.01}},
    {"imu3", {100.0, 1000, 0.01}}
};

// GNSS Configuration
std::unordered_map<std::string, std::tuple<double, int, double>> gnss_sensors_config = {
    {"gnss1", {20.0, 1000, 0.01}},
    {"gnss2", {20.0, 1000, 0.01}}
};

// FDIR and ProcessingUnit frequencies
const double processing_freq = 50.0; 
const double fdir_freq = 20.0; // TODO: set the minimum sensor frequency dynamically

// Fault injection configuration
const int injection_duration = 5; // Duration of fault injection in seconds

// Instantiate IMU sensors
void instantiateImuSensors(
    std::vector<std::shared_ptr<ImuSensor>>& imu_sensors, 
    const std::unordered_map<std::string, std::tuple<double, int, double>>& imu_sensors_config) 
{
    for (const auto& [name, params] : imu_sensors_config) {
        imu_sensors.push_back(std::make_shared<ImuSensor>(
            name, 
            std::get<0>(params), 
            std::get<1>(params), 
            std::get<2>(params))
        );
    }
}

// Instantiate GNSS sensors
void instantiateGnssSensors(
    std::vector<std::shared_ptr<GnssSensor>>& gnss_sensors, 
    const std::unordered_map<std::string, std::tuple<double, int, double>>& gnss_sensors_config) 
{
    for (const auto& [name, params] : gnss_sensors_config) {
        gnss_sensors.push_back(std::make_shared<GnssSensor>(
            name, 
            std::get<0>(params), 
            std::get<1>(params), 
            std::get<2>(params))
        );
    }
}

void instantiateSimulation(
    std::vector<std::shared_ptr<ImuSensor>>& imu_sensors,
    std::vector<std::shared_ptr<GnssSensor>>& gnss_sensors,
    std::shared_ptr<ProcessingUnit>& processing_unit,
    std::shared_ptr<Fdir>& fdir,
    std::unique_ptr<Simulator>& simulator
    )
{
    // Instanciate IMU sensors
    instantiateImuSensors(imu_sensors, imu_sensors_config);

    // Instanciate GNSS sensors
    instantiateGnssSensors(gnss_sensors, gnss_sensors_config);

    // Instanciate ProcessingUnit
    processing_unit = std::make_shared<ProcessingUnit>(
        imu_sensors, 
        gnss_sensors, 
        processing_freq
    );

    // Instanciate FDIR
    fdir = std::make_shared<Fdir>(processing_unit, fdir_freq);
    
    // Add IMU sensors to FDIR
    for (auto& imu_sensor : imu_sensors) {
        fdir->addSensor(imu_sensor); 
    }

    // Add GNSS sensors to FDIR
    for (auto& gnss_sensor : gnss_sensors) {
        fdir->addSensor(gnss_sensor); 
    }

    // Instantiate the simulator
    simulator = std::make_unique<Simulator>(imu_sensors, gnss_sensors, processing_unit, fdir);
}

// Main function
int main() {
    // Initialize the logger
    Logger::init();

    // Instantiate the simulation components
    std::vector<std::shared_ptr<ImuSensor>> imu_sensors;
    std::vector<std::shared_ptr<GnssSensor>> gnss_sensors;
    std::shared_ptr<ProcessingUnit> processing_unit;
    std::shared_ptr<Fdir> fdir;
    std::unique_ptr<Simulator> simulator;
    instantiateSimulation(imu_sensors, gnss_sensors, processing_unit, fdir, simulator);

    // Start the interactive command loop
    std::string command;
    std::string interface = R"(
        [Interface] Welcome to the Simulator!
        Available commands:
        0 - Show this help message
        1 - Start
        2 - Stop
        3 - Use Case 1 (Simulate 10 seconds)
        4 - Use Case 2 (inject IMU faults for 5 seconds)
        5 - Use Case 3 (inject GNSS faults for 5 seconds)
        6 - Exit
        )";
    std::cout << interface << std::endl;

    // Define a mutex to protect the command input
    std::mutex command_mutex;

    bool start = false;
    while (true) {
        std::cout << "[Interface] Digit the number (e.g. for 'Start' digit '1') >> ";
        std::cin >> command;
        
        if (start == true && command != "2" && command != "6") 
        {
            Logger::log(Logger::Level::Warning, "[Interface] You must stop the simulation before executing other commands.");
            continue;
        }
        
        // Check if the command is empty
        if (command.empty()) 
        {
            Logger::log(Logger::Level::Warning, "[Interface] Command cannot be empty.");
            continue;
        }

        // Check if the command is a number
        if (command == "0") 
        {
            std::cout << interface << std::endl;
            continue;
        }
        if (command == "1") 
        {
            simulator->start();
            start = true;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            Logger::log(Logger::Level::Info, "[Interface] Simulation running... Check CSV files to see the data");
        } 
        else if (command == "2") 
        {
            simulator->stop();
            start = false;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            Logger::log(Logger::Level::Info, "[Interface] Simulation stopped.");
        } 
        else if (command == "3") 
        {
            simulator->start(); // Start the simulator
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
            Logger::log(Logger::Level::Info, "[Interface] Simulating for 10 seconds... Check CSV files to see the data");
            std::this_thread::sleep_for(std::chrono::seconds(10));
            simulator->stop(); // Stop the simulator 
        } 
        else if (command == "4") 
        {
            simulator->start(); // Start the simulator
            simulator->injectImuFaults(true); // Inject faults in IMU sensors
            std::this_thread::sleep_for(std::chrono::seconds(injection_duration)); // Wait for N seconds
            simulator->injectImuFaults(false); // Stop injecting faults
            simulator->stop(); // Stop the simulator 
        } 
        else if (command == "5") 
        {
            simulator->start(); // Start the simulator
            simulator->injectGnssFaults(true); // Inject faults in GNSS sensors
            std::this_thread::sleep_for(std::chrono::seconds(injection_duration)); // Wait for N seconds
            simulator->injectGnssFaults(false); // Stop injecting faults
            simulator->stop(); // Stop the simulator 
        } 
        else if (command == "6") 
        {
            if (start) 
            {
                simulator->stop(); // Stop the simulator if it's running
            }

            // Destroy the simulator and stop all sensors
            Logger::log(Logger::Level::Info, "[Interface] Exiting the simulator. Check the log file for details.");
            break;
        } 
        else 
        {
            Logger::log(Logger::Level::Warning, "[Interface] Command not valid: " + command);
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    Logger::log(Logger::Level::Info, "[Interface] Bye!");

    return 0;
}
