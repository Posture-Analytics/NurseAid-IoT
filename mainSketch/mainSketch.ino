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
// #include "Database.h"

// Create a errors object to handle them
Errors errorHandler;

// Create a buffer to store the data to be sent to the database
TemperatureDataBuffer dataBuffer;

// Create a DataReader object to read the data from the sensors
DataReader dataReader;

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
    
    // Sanity delay
    delay(100);

    errorHandler.showError(ErrorType::None);
}


int i = 0;

void loop() {
    dataReader.fillBuffer(&dataBuffer);

    i++;

    if (i == 2) {
        i = 0;
        Serial.println("Dumping buffer");
        dataBuffer.dumpBufferContent();
    }

    delay(10);
}