/*
    Database.h

    * This module handles the database connection and provides functions to send data
    to the database.
    * It uses the FirebaseESP32 library to connect and send data directly to the
    Firebase Realtime Database. It also provides a function to structure the collected
    data into JSON formatted batches to be sent to the database.
    * It also logs the device's boot, useful to analyze crashes, stability, reboots...
*/

#ifndef Database_H_
#define Database_H_

#include <FirebaseESP32.h>

#include "Buffer.h"
#include "Credentials.h"

// Send Rate of the data sending, in hertz (Hz)
const int SEND_RATE = 2;

/**
 * Database class to handle the database connection and data sending 
 * to the Firebase Realtime Database
 * 
 * This class uses the FirebaseESP32 library to connect and send data directly to the
 * Firebase Realtime Database. It also provides a function to structure the collected
 * data into JSON formatted batches to be sent to the database.
 * 
 * It also logs the device's boot, useful to analyze crashes, stability, reboots...
 */
class Database {
    static const int JSON_BATCH_SIZE = 2;

    // Define the Firebase Data, Authentication and Configuration objects
    FirebaseData fbdo;
    FirebaseAuth auth;
    FirebaseConfig config;
    // Create a JSON object to hold and organize the data before sending it to the database
    FirebaseJson jsonBuffer;

    // Create a counter to help to fill the JSON object until a certain size
    int jsonSize = 0;

    // Store the current date
    char sampleDate[12];

    // Set the database where the json will be pushed to
    String DATABASE_BASE_PATH = "/thermal_cam/";

    // Set the data path on the database where the sensor data will be stored
    String fullDataPath;

    // Store whether or not the last sample from the sensors was valid (non-zero)
    bool last_was_valid;

    // Set the interval between data send, in microseconds (us)
    const int dataSendIntervalMicros = 1e6 / SEND_RATE;
    // Save the time of the last data send, in microseconds (us)
    unsigned long dataPrevSendingMicros = 0;
    // Save the current time, in microseconds (us)
    unsigned long currentMicros = 0;

    // Update the current time variable
    void updateCurrentTime();

    // Store the base64 encoded value
    char encodedValue[3];

    // Store the concatenated base64 encoded values
    String payload = "";

public:
    /**
     * Constructor for the Database class
    */
    Database();

    /**
     * Log the device's boot. Useful to analyze crashes, stability, reboots...
     * Prints all the relevant information to the Serial Monitor
     */
    void bootLog();

    /** 
     * Setup the database connection 
     * @param timestampUnix The timestamp of the device's boot, in Unix time
     */
    void setup(time_t timestampUnix);

    /**
     * Append sensor data into the JSON object
     * @param data The sensor data to be appended
    */
    void appendDataToJSON(const temperatureData* data);

    /**
     * Send the JSON object to the database, update the node asynchronously
     * @return Whether or not the data was successfully sent to the database
     */
    bool pushData();

    /**
     * Track the incoming data and fill the json buffer to be sent to the database
     * @param dataBuffer The buffer containing the sensor data
     */
    void sendData(TemperatureDataBuffer* dataBuffer);

    /**
     * Encode a float value into a base64 string
     * @param input The float value to be encoded
     * @param output The string where the encoded value will be stored
     * @param offset The offset to be applied to the input value
     * @param scale The scale to be applied to the input value
     * @return Whether or not the value was successfully encoded
     */
    bool encodeBase64(float input, char* output, float offset, float scale);
};

#endif
