#include <Adafruit_MLX90640.h>

// Uncomment the following line if you are using the M5StickC Plus
#define M5STICKCPLUS

#ifdef M5STICKCPLUS
    #include <M5StickCPlus.h>
#endif

// Thermal camera object
Adafruit_MLX90640 mlx;

// Frame buffer to store one frame of temperatures
float frame[32*24];

void setup() {

    #ifdef M5STICKCPLUS
        M5.begin();
    #endif

    Serial.begin(115200);

    // Initialize the thermal camera
    Serial.print("Initializing thermal camera... ");
    if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
        Serial.println("MLX90640 not found!");
        while (1) delay(10);
    }
    Serial.println("MLX90640 online!");

    // Print the camera details
    Serial.print("Serial number: ");
    Serial.print(mlx.serialNumber[0], HEX);
    Serial.print(mlx.serialNumber[1], HEX);
    Serial.println(mlx.serialNumber[2], HEX);
    
    // Switch modes (Interleaved/Chess)
    //mlx.setMode(MLX90640_INTERLEAVED);
    mlx.setMode(MLX90640_CHESS);
    Serial.print("Current mode: ");
    if (mlx.getMode() == MLX90640_CHESS) {
        Serial.println("Chess");
    } else {
        Serial.println("Interleave");    
    }

    mlx.setResolution(MLX90640_ADC_18BIT);
    Serial.print("Current resolution: ");
    mlx90640_resolution_t res = mlx.getResolution();
    switch (res) {
        case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
        case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
        case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
        case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
    }

    mlx.setRefreshRate(MLX90640_2_HZ);
    Serial.print("Current frame rate: ");
    mlx90640_refreshrate_t rate = mlx.getRefreshRate();
    switch (rate) {
        case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
        case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
        case MLX90640_2_HZ: Serial.println("2 Hz"); break;
        case MLX90640_4_HZ: Serial.println("4 Hz"); break;
        case MLX90640_8_HZ: Serial.println("8 Hz"); break;
        case MLX90640_16_HZ: Serial.println("16 Hz"); break;
        case MLX90640_32_HZ: Serial.println("32 Hz"); break;
        case MLX90640_64_HZ: Serial.println("64 Hz"); break;
    }
}


void loop() {
    delay(500);

    // Get a frame of temperatures
    if (mlx.getFrame(frame) != 0) {
        Serial.println("'getFrame()' failed");
        return;
    }

    Serial.println();
    Serial.println();
    for (uint8_t h=0; h<24; h++) {
        for (uint8_t w=0; w<32; w++) {
            float t = frame[h*32 + w];
            Serial.print(t, 1);
            Serial.print(", ");
        }
    }
    Serial.println();
}