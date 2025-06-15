#pragma once
#include "Sensor.hpp"
#include <deque>

// IMU data structure: angular velocities
struct ImuData 
{
    Sensor::Timestamp timestamp;
    double att_rate_x;
    double att_rate_y;
    double att_rate_z;
};

// IMU Sensor Class
class ImuSensor : public Sensor 
{
    public:
        // Constructor
        ImuSensor(const std::string& name, double frequency, int buffer_size, double noise);
        
        // Start IMU thread
        void start() override;
        
        // Stop IMU thread
        void stop() override;
        
        // Enable or disbale fault injection
        void injectFault(bool enable);

        // Get IMU data
        std::deque<ImuData> getBuffer();

    private:
        // IMU sensor data generation loop
        void run() override;

        // IMU data
        ImuData generateSample();
        std::deque<ImuData> buffer_;    // Circular data buffer
};