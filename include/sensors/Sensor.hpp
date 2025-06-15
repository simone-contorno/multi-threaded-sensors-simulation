#pragma once // Avoid multiple inclusion
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include "../logging/Logger.hpp"

// Abstract base class for all sensors (IMU and GNSS)
class Sensor {
    public:
        // Timestamp alias using steady_clock 
        using Timestamp = std::chrono::steady_clock::time_point;

        // Constructor
        Sensor(const std::string& name, double frequency, int buffer_size, double noise) 
            : name_(name), frequency_(frequency), buffer_size_(buffer_size), noise_(noise), 
            running_(false), fault_injected_(false) {}

        // Destructor (default behaviour)
        virtual ~Sensor() = default;

        // Start sensor thread (it must be overrided)
        virtual void start() = 0;

        // Stop sensor thread (it must be overrided)
        virtual void stop() = 0;

        // Get sensor name
        std::string getName() const { return name_; }

        // Get sensor frequency in Hz
        double getFrequency() const { return frequency_; }

        // Get the last update timestamp
        Timestamp getLastUpdate()  
        { 
            std::lock_guard<std::mutex> last_update_lock(last_update_mutex_);
            return last_update_; 
        }
        
        // Get running status
        bool isRunning() const { return running_; }

        // Set the frequency of the sensor
        void setFrequency(double frequency) { frequency_ = frequency;}

    protected:
        // Sensor data generation loop
        virtual void run() = 0;

        Timestamp last_update_;
        std::string name_;                  // Sensor name
        double frequency_;                  // Frequency in Hz
        int buffer_size_;                   // Buffer size
        double noise_;                      // Sensor noise
        std::thread thread_;                // Simulation thread
        bool running_;                      // Thread control flag
        bool fault_injected_;               // injection fault simulation flag
        std::mutex buffer_mutex_;           // Mutex for thread-safe buffer access
        std::mutex last_update_mutex_;      // Mutex for thread-safe last update access
};