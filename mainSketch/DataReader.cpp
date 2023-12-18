#include <Adafruit_MLX90640.h>

#include "DataReader.h"
#include "Network.h"
#include "Buffer.h"
#include "Errors.h"
#include "Debug.h"

bool DataReader::setup(TemperatureDataBuffer* dataBuffer) {
    // Setup the MLX90640
    if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
        errorHandler.showError(ErrorType::MLX906040InitFailure, true);
        return false;
    }

    // Set the mode of the MLX90640
    // mlx.setMode(MLX90640_INTERLEAVED);
    mlx.setMode(MLX90640_MODE);

    // Set the resolution of the MLX90640
    mlx.setResolution(MLX90640_RESOLUTION);

    // Set the refresh rate of the MLX90640
    mlx.setRefreshRate(MLX90640_REFRESH_RATE);

    // Point the frame pointer to the next sample to be written
    newSample = dataBuffer->getNewSample();

    // If everything went well, return true
    return true;
}

void DataReader::fillBuffer(TemperatureDataBuffer* dataBuffer) {

    LogVerboseln("Filling buffer");

    // If the frame is ready, read it
    if (mlx.getFrame(newSample->temperaturePixels) == 0) {

        LogVerboseln("Frame ready");

        // Fill the buffer with current timestamp (in milliseconds)
        newSample->timestampMillis = getCurrentMillisTimestamp();

        // Pointer to the next sample to be written
        newSample = dataBuffer->getNewSample();
    } else {
        LogErrorln("Waiting for frame from MLX90640");
    }
}
