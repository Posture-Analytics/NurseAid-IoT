#include "time.h"
#include <Adafruit_MLX90640.h>
#include <WiFi.h>
#include <Wire.h>

// Uncomment the following line if you are using the M5StickC Plus
#define M5STICKCPLUS

#ifdef M5STICKCPLUS
    #include <M5StickCPlus.h>
#endif

// DEBUG Flag, prints data to Serial instead of sending to the database
#define DEBUG

#include "Errors.h"
#include "Credentials.h"
#include "Network.h"
#include "Buffer.h"
#include "DataReader.h"
#include "Database.h"

// Create a errors object to handle them
Errors errorHandler;

// Create a buffer to store the data to be sent to the database
TemperatureDataBuffer dataBuffer;

// Create a DataReader object to read the data from the sensors
DataReader dataReader;

// Create a task to assign the data push to the database to Core 0
TaskHandle_t sendToDatabaseTask;

// Create a Database object to send the data to the database
Database database;

void setup() {

    #ifdef M5STICKCPLUS
        M5.begin();
    #endif

    Serial.begin(115200);

    // Setup the thermal camera
    Serial.print("Initializing thermal camera... ");
    if(!dataReader.setup(&dataBuffer)){
        Serial.println("MLX90640 not found!");
        while (1) delay(10);
    }
    Serial.println("MLX90640 online!");

    // Setup WiFi connection
    setupWiFi();

    // Sync the device's time with an NTP Server time
    syncWithNTPTime();
    
    // Setup the Firebase Database connection
    database.setup(getCurrentTime());

    // Assign the task of sending data to the database to Core 0
    xTaskCreatePinnedToCore(
        sendToDatabase,          // Task function
        "sendToDatabaseLoop",    // Name of task
        10000,                   // Stack size of task
        NULL,                    // Parameter of the task
        1,                       // Priority of the task
        &sendToDatabaseTask,     // Task handle to keep track of created task
        0);                      // Pin task to core 0

    // Sanity delay
    delay(100);

    errorHandler.showError(ErrorType::None);
}

void loop() {
    // Disable the watchdog of Core 0, avoiding reboots caused by
    // the working time of the sendToDatabase task
    disableCore0WDT();

    dataReader.fillBuffer(&dataBuffer);
}

// Task attached to core 0
void sendToDatabase(void* pvParameters) {
    // A loop that runs forever to keep sending data to the database
    while (true) {
        // If the buffer is empty, wait for the data collection task to fill it
        if (!dataBuffer.isBufferEmpty()) {
            database.sendData(&dataBuffer);
        } else {
            vTaskDelay(10);
            yield();
        }
    }
}