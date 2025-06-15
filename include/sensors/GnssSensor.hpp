#pragma once
#include "Sensor.hpp"
#include <deque>

// GNSS data structure representing position
struct GnssData 
{
    Sensor::Timestamp timestamp;
    double pos_x;
    double pos_y;
    double pos_z;
};

// GNSS Sensor class
class GnssSensor : public Sensor
{
    public:
        // Constructor
        GnssSensor (const std::string& name, double frequency, int buffer_size, double noise);

        // Start GNSS thread
        void start() override;
        
        // Stop GNSS thread
        void stop() override;
        
        // Enable or disbale fault injection
        void injectFault(bool enable);

        // Get GNSS data
        std::deque<GnssData> getBuffer();

    private:
        // GNSS sensor data generation loop
        void run() override;

        // GNSS data
        GnssData generateSample();
        std::deque<GnssData> buffer_;   // Circular data buffer
};