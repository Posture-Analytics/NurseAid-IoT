#include "Buffer.h"
#include "Errors.h"
#include "Debug.h"

bool TemperatureDataBuffer::isBufferEmpty() const {
    return bufferSize == 0;
}

bool TemperatureDataBuffer::isBufferFull() const {
    return bufferSize >= BUFFER_CAPACITY;
}

int TemperatureDataBuffer::getBufferCapacity() const {
    return BUFFER_CAPACITY;
}

int TemperatureDataBuffer::getBufferSize() const {
    return bufferSize;
}

int TemperatureDataBuffer::getReadIndex() const {
    return readIndex;
}

int TemperatureDataBuffer::getWriteIndex() const {
    return writeIndex;
}

void TemperatureDataBuffer::moveReadIndexForward() {
    readIndex = (readIndex + 1) % BUFFER_CAPACITY;
    bufferSize--;
}

void TemperatureDataBuffer::moveReadIndexBackward() {
    readIndex = (readIndex - 1 + BUFFER_CAPACITY) % BUFFER_CAPACITY;
    bufferSize++;
}

void TemperatureDataBuffer::moveWriteIndexForward() {
    writeIndex = (writeIndex + 1) % BUFFER_CAPACITY;
    bufferSize++;
}

void TemperatureDataBuffer::moveWriteIndexBackward() {
    writeIndex = (writeIndex - 1 + BUFFER_CAPACITY) % BUFFER_CAPACITY;
    bufferSize--;
}

time_t TemperatureDataBuffer::getCurrentSampleSeconds() const {
    return buffer[readIndex].timestampMillis / 1000ULL;
}

void TemperatureDataBuffer::computeCurrentSampleDate(char* sampleDate) {
    // Get the next sample from the buffer
    time_t sampleTimestampSec = getCurrentSampleSeconds();
    // Get the time information from the time_t object
    localtime_r(&sampleTimestampSec, &timeInfo);
    // Convert the struct tm to a formatted string like "YYYY-MM-DD\0"
    strftime(sampleDate, 11, "%F", &timeInfo);
}

void TemperatureDataBuffer::computeNextDaySeconds() {
    time_t sampleTimestampSec = getCurrentSampleSeconds();
    // Get the time info of the next day
    sampleTimestampSec += 24 * 60 * 60;
    localtime_r(&sampleTimestampSec, &timeInfo);
    // Set the time info to the start of the day
    timeInfo.tm_hour = 0;
    timeInfo.tm_min = 0;
    timeInfo.tm_sec = 0;

    // Convert back to time_t and save it
    nextDay = mktime(&timeInfo);
}

bool TemperatureDataBuffer::hasDateChanged() {
    // Compare the current timestamp with the timestamp of the next day
    return getCurrentSampleSeconds() >= nextDay;
}

bool TemperatureDataBuffer::isSampleNull(const temperatureData* sample) const {
    // Check if we have some non null data in the sample struct
    for (int i = 0; i < FRAME_SIZE; i++) {
        if (sample->temperaturePixels[i] != 0) {
            return false;
        }
    }

    // Return true if all the sample data is null
    return true;
}

const temperatureData* TemperatureDataBuffer::getSample() {
    // If the buffer is empty
    if (isBufferEmpty()) {
        // Return nullptr if the sample was not retrieved from the buffer
        return nullptr;
    }

    // Get the next sample from the buffer
    const temperatureData* ptrSample = &buffer[readIndex];

    // Move the read index to the next sample
    moveReadIndexForward();

    // Return the sample if it was retrieved from the buffer
    return ptrSample;
}

temperatureData* TemperatureDataBuffer::getNewSample() {
    // If the buffer is full
    if (isBufferFull()) {
        
        // Print an error message and restart the device
        errorHandler.showError(ErrorType::BufferFull, true);
        
        // Return nullptr if the sample was not added to the buffer
        return nullptr;
    }

    // Get the pointer to the next sample to be written
    temperatureData* ptrSample = &buffer[writeIndex];

    // Move the write index to the next sample
    moveWriteIndexForward();

    // Return the pointer to the next sample to be written
    return ptrSample;
}

void TemperatureDataBuffer::printBufferState() const {
    // If the buffer gets full
    if (isBufferFull()) {
        // Print an error message and restart the device
        errorHandler.showError(ErrorType::BufferFull, true);
    }

    // Prints the buffer state
    LogVerboseln("Buffer state: ", bufferSize, "/", BUFFER_CAPACITY);
}

void TemperatureDataBuffer::printBufferIndexes() const {
    LogVerboseln("Buffer index: R=", readIndex, " W=", writeIndex);
}

void TemperatureDataBuffer::dumpBufferContent(int start, int end) const {
    // Iterate over the buffer content on the desired range
    for (int i = start; i < end; i++) {
        // Get the current sample
        const temperatureData* sample = &buffer[i];

        // Print the sample timestamp
        LogDebug(sample->timestampMillis, " ");

        // Print the sample temperature sensor values
        for (int j = 0; j < FRAME_SIZE; j++) {
            LogDebug(sample->temperaturePixels[j], " ");
        }

        // Print the end of the line
        LogDebugln("\n");
    }
}
