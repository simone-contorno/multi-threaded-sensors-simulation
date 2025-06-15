#include "ProcessingUnit.hpp"
#include <deque>
#include <optional>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

// Constructor
ProcessingUnit::ProcessingUnit(
    std::vector<std::shared_ptr<ImuSensor>> imu_sensors, 
    std::vector<std::shared_ptr<GnssSensor>> gnss_sensors, 
    double frequency
) : imu_sensors_(imu_sensors), gnss_sensors_(gnss_sensors), frequency_(frequency), running_(true) 
{
    // Create timestamp for folder name
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time_t), "%Y%m%d_%H%M%S");
    
    // Create data directory
    data_directory_ = "../data/" + ss.str() + "_data";
    std::filesystem::create_directories(data_directory_);

    // Open and initialize CSV files
    imu_file_.open(data_directory_ + "/imu.csv");
    gnss_file_.open(data_directory_ + "/gnss.csv");

    // Write headers
    imu_file_ << "timestamp,attitude_rate_x,attitude_rate_y,attitude_rate_z,valid\n";
    gnss_file_ << "timestamp,pos_x,pos_y,pos_z,valid\n";
}

// Starts the simulation thread
void ProcessingUnit::start()
{
    Logger::log(Logger::Level::Info, "[ProcessingUnit] Start");
    running_ = true;
    thread_ = std::thread(&ProcessingUnit::run, this);
}

// Stops the simulation thread
void ProcessingUnit::stop()
{
    Logger::log(Logger::Level::Info, "[ProcessingUnit] Stop");
    running_ = false;

    // Wait for thread to finish
    if (thread_.joinable())
        thread_.join();
}

// Retrieve sensors data
ProcessingOutput ProcessingUnit::getSensorData() 
{
    std::vector<std::array<std::optional<double>, 3>> imu_data = {};
    std::vector<std::optional<Sensor::Timestamp>> imu_timestamps = {};
    for (auto& imu_sensor : imu_sensors_)
    {
        // Get IMU data
        std::deque<ImuData> imu_buffer = imu_sensor->getBuffer();
        if (!imu_buffer.empty())
        {
            // Get last IMU data
            std::array<std::optional<double>, 3> last_imu = 
            {
                imu_buffer.back().att_rate_x, 
                imu_buffer.back().att_rate_y, 
                imu_buffer.back().att_rate_z
            };
            auto& imu_timestamp_last = imu_buffer.back().timestamp;
            
            // Push IMU timestamps
            imu_timestamps.push_back({imu_timestamp_last}); //, imu_timestamp_second_last});
            
            // Push IMU data
            imu_data.push_back({last_imu}); //, second_last_imu});
        }
    }

    // Process IMU data
    std::array<std::optional<double>, 3> attitude_rate = getAttitudeRate(imu_data, imu_timestamps);

    // Verify IMU validity
    bool valid_imu = true;
    if (attitude_rate[0] == std::nullopt || attitude_rate[1] == std::nullopt || attitude_rate[2] == std::nullopt)
    {
        valid_imu = false;
        Logger::log(Logger::Level::Error, "[ProcessingUnit] No valid IMU data.");
    }

    // Get GNSS data
    std::array<std::optional<double>, 3> gnss_data = {std::nullopt, std::nullopt, std::nullopt};
    std::optional<Sensor::Timestamp> gnss_timestamp = std::nullopt;
    for (auto& gnss_sensor : gnss_sensors_)
    {
        std::deque<GnssData> gnss_buffer = gnss_sensor->getBuffer();
        if (!gnss_buffer.empty())
        {
            // If the current GNSS has more recent data, update
            std::optional<Sensor::Timestamp> gnss_timestamp_new = gnss_buffer.back().timestamp;
            if (gnss_timestamp_new != std::nullopt)
            {
                if (gnss_timestamp_new > gnss_timestamp)
                {
                    gnss_data = {gnss_buffer.back().pos_x, gnss_buffer.back().pos_y, gnss_buffer.back().pos_z};
                    gnss_timestamp = gnss_buffer.back().timestamp;
                }
            }      
        }
    }    

    // Verify GNSS validity
    bool valid_gnss = true;
    if (gnss_data[0] == std::nullopt || gnss_data[1] == std::nullopt || gnss_data[2] == std::nullopt)
    {
        valid_gnss = false;
        Logger::log(Logger::Level::Error, "[ProcessingUnit] No valid GNSS data.");
    }

    // Check the last measurement is not older than 1 second
    if (gnss_timestamp != std::nullopt)
    {
        auto time_now = std::chrono::steady_clock::now();
        auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(time_now - gnss_timestamp.value());
        if (time_diff.count() > 1.0)
        {
            valid_gnss = false;
            Logger::log(Logger::Level::Error, "[ProcessingUnit] GNSS data is older than 1 second.");
        }
    }

    return ProcessingOutput {
        std::chrono::steady_clock::now(),
        attitude_rate[0].value_or(0.0),
        attitude_rate[1].value_or(0.0),
        attitude_rate[2].value_or(0.0),
        gnss_data[0].value_or(0.0),
        gnss_data[1].value_or(0.0),
        gnss_data[2].value_or(0.0),
        valid_imu,
        valid_gnss
    };
}

// Retrieve attitude rate from IMU data
std::array<std::optional<double>, 3> ProcessingUnit::getAttitudeRate(
            std::vector<std::array<std::optional<double>, 3>> imu_data, 
            std::vector<std::optional<Sensor::Timestamp>> imu_timestamps)
{
    // Compute the sum of all IMU data
    std::vector<std::optional<double>> imu_timestamp_diff = {}; 
    std::array<std::optional<double>, 3> sum_imu_last = {std::nullopt, std::nullopt, std::nullopt};
    std::array<std::optional<double>, 3> sum_imu_second_last = {std::nullopt, std::nullopt, std::nullopt};
    int valid_timestamp_count = 0;
    int valid_imu_count = 0;

    for (size_t i = 0; i < imu_data.size(); i++)
    {
        auto imu = imu_data[i];

        // Assumption: the IMU values are valid only if all values are provided (otherwise the sensor is not correctly working)
        if (imu[0] != std::nullopt && imu[1] != std::nullopt && imu[2] != std::nullopt)
        {
            valid_imu_count++;

            // Initialize the avg value for the last IMU data if not already done
            if (sum_imu_last[0] == std::nullopt)
            {
                sum_imu_last[0] = 0.0;
                sum_imu_last[1] = 0.0;
                sum_imu_last[2] = 0.0;
            }
            else
            {
                sum_imu_last[0].value() += imu[0].value();
                sum_imu_last[1].value() += imu[1].value();
                sum_imu_last[2].value() += imu[2].value();
            }
        }
    }

    // Sum the timestamps values
    double sum_timestamp = 0.0;
    for (auto timestamp : imu_timestamps)
    {
        if (timestamp != std::nullopt)
        {
            valid_timestamp_count++;
            sum_timestamp += std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.value().time_since_epoch()).count();
        }
    }

    // Compute the timestamp average
    std::optional<double> imu_time_avg = std::nullopt;
    if (sum_timestamp != 0.0)
    {
        imu_time_avg = sum_timestamp / valid_timestamp_count;
    }

    // Compute the average of the IMU data
    std::array<std::optional<double>, 3> avg_imu = {std::nullopt, std::nullopt, std::nullopt};
    if (sum_imu_last[0] != std::nullopt)
    {
        avg_imu[0] = sum_imu_last[0].value() / valid_imu_count;
        avg_imu[1] = sum_imu_last[1].value() / valid_imu_count;
        avg_imu[2] = sum_imu_last[2].value() / valid_imu_count;
    }

    return avg_imu;
}

void ProcessingUnit::run()
{
    while(running_)
    {
        {
            // Get Sensors data
            std::lock_guard<std::mutex> lock(output_mutex_);
            ProcessingOutput output = getSensorData();

            // Save the data in csv files
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                output.timestamp.time_since_epoch()).count();

            // Write IMU data
            imu_file_ << timestamp << ","
                    << output.attitude_rate_x << ","
                    << output.attitude_rate_y << ","
                    << output.attitude_rate_z << ","
                    << output.valid_imu << "\n";
            imu_file_.flush();

            // Write GNSS data
            gnss_file_ << timestamp << ","
                    << output.last_pos_x << ","
                    << output.last_pos_y << ","
                    << output.last_pos_z << ","
                    << output.valid_gnss << "\n";
            gnss_file_.flush();

            // Update last output
            last_output_ = output;
        }

        int sleep_time_ms = static_cast<int>(1000 / frequency_);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
    }
}