/*
    DataReader.h

    * This module handle the MLX90640 and the data collection.
    * It setup the sensors and the constants related to them
    * It also handle the routine to collect data from the sensors and store it on the buffer.
*/

#ifndef DataReader_H_
#define DataReader_H_

#include <Adafruit_MLX90640.h>

#include "Buffer.h"

// Mode of the MLX90640: MLX90640_INTERLEAVED or MLX90640_CHESS
#define MLX90640_MODE MLX90640_CHESS

// Resolution of the MLX90640: MLX90640_ADC_16BIT, MLX90640_ADC_17BIT, MLX90640_ADC_18BIT or MLX90640_ADC_19BIT
#define MLX90640_RESOLUTION MLX90640_ADC_18BIT

// Refresh rate of the MLX90640 (in Hz): 0.5, 1, 2, 4, 8, 16, 32, 64
#define MLX90640_REFRESH_RATE MLX90640_2_HZ


/**
 * This class handle the sensors and the data collection from them.
 * It setup the sensors and the constants related to them
 * It also handle the routine to collect data from the sensors and store it on the buffer.
*/
class DataReader {

private:
    // MLX90640 object
    Adafruit_MLX90640 mlx;

    // Frame buffer to store the next frame of temperatures
    temperatureData* newSample = nullptr;

public:

    /**
     * Setup the MLX90640 and its parameters
     * 
     * @param dataBuffer: Pointer to the buffer where the data will be stored
     * 
     * @return true if everything went well, false otherwise
     */
    bool setup(TemperatureDataBuffer* dataBuffer);

    /**
     * Fill buffer if the frame is ready
     * 
     * @param dataBuffer: Pointer to the buffer where the data will be stored
     */
    void fillBuffer(TemperatureDataBuffer* dataBuffer);
};

#endif  // DataReader_H_