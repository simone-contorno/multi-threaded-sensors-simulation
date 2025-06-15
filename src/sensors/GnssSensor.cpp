#include "GnssSensor.hpp"
#include <chrono>
#include <random>
#include <iostream>

// Constructor: initializes member variables
GnssSensor::GnssSensor(const std::string& name, double frequency, int buffer_size, double noise) 
    : Sensor(name, frequency, buffer_size, noise) 
{
}

// Starts the simulation thread
void GnssSensor::start()
{
    Logger::log(Logger::Level::Info, "[GnssSensor] Starting GNSS sensor: " + name_);
    running_ = true;
    thread_ = std::thread(&GnssSensor::run, this);
}

// Stops the simulation thread
void GnssSensor::stop()
{
    std::lock_guard<std::mutex> lock(buffer_mutex_); // Lock the mutex

    Logger::log(Logger::Level::Info, "[GnssSensor] Stopping GNSS sensor: " + name_);
    running_ = false;
    
    // Clear the buffer
    buffer_.clear(); // Clear the buffer

    // Wait for thread to finish
    if (thread_.joinable())
        thread_.join();
}

// Enables or disables fault injection
void GnssSensor::injectFault(bool enable)
{
    Logger::log(Logger::Level::Info, "[GnssSensor] Fault injection " + std::string(enable ? "enabled" : "disabled") + " for GNSS sensor: " + name_);
    fault_injected_ = enable;
    
    // Clear the buffer if fault injection is enabled
    if (enable) {
        std::lock_guard<std::mutex> lock(buffer_mutex_); // Lock the mutex
        buffer_.clear(); // Clear the buffer
    }
}

// Returns a thread-safe copy of the IMU data buffer
std::deque<GnssData> GnssSensor::getBuffer()
{
    std::lock_guard<std::mutex> lock(buffer_mutex_); // Lock the mutex
    return buffer_;
} // Release the mutex

// Sensor loop
void GnssSensor::run()
{
    while(running_)
    {
        // Check if fault injection is enabled
        if (!fault_injected_) 
        {
            GnssData sample = generateSample();

            std::lock_guard<std::mutex> lock(buffer_mutex_); // Lock the mutex

            std::lock_guard<std::mutex> last_update_lock(last_update_mutex_);
            last_update_ = sample.timestamp;

            buffer_.push_back(sample);

            // Limit buffer size for memory
            if (buffer_.size() > buffer_size_) {
                buffer_.pop_front();
            }
        } // Release the mutex
        
        int sleep_time_ms = static_cast<int>(1000 / frequency_);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
    }
}

// Generate random GNSS data 
GnssData GnssSensor::generateSample() 
{
    static std::default_random_engine generator(std::random_device{}()); // Random seed with portable engine
    static std::normal_distribution<double> noise(0.0, noise_);  // Generate noise (not bias)

    return GnssData {
        std::chrono::steady_clock::now(),
        1.0 + noise(generator),
        1.0 + noise(generator),
        1.0 + noise(generator)
    };
}