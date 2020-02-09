
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

// terminal
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <math.h>

#include "HoleFinder.hpp"
#include "TOF10120.hpp"

// https://github.com/RoboticsBrno/ServoESP32 instead of standard servo in ESP32

// Objects declaration

// Servo

Servo myservo;  // create servo object to control a servo

// OLED declaration

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);


// HoleFinder object

HoleFinder holeFinder(1000);

// TOF10120

TOF10120 lidar;

// Servo // EK20508 0.12 sec/60° -> 5 graus / 10ms

#define ANGLEOFFSET 8
#define ANGLEINI 20
#define ANGLEEND 180-ANGLEINI
#define ANGLEINCR 4
#define DELAYTIME 10
#define DEFAULTDIST 500

unsigned long lastTime=0;

int angle = ANGLEINI;
bool increasing = true;
bool increasingOld = false;
int angleUpdate = ANGLEINCR;
bool reversing = true;

int distLidar = 0;
int distLidarFilteredOld = 0;
int distLidarFiltered = 0;

bool filteringOneRead = false;
int distLidarOld = 0;


void setup() {
    Serial.begin(115200);

    // I2C

    Wire.begin(); 
    
    // Servo

    myservo.attach(27);

    // Setup OLED

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
        // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    display.clearDisplay();
}


void loop() {
    
    myservo.write(angle + ANGLEOFFSET);

    lidar.update();

    if ((millis() - lastTime) > DELAYTIME) {

        distLidar = lidar.getDistance();  //   ReadDistance();

        // Filtering if one Pulse is wrong. As negative effect, it creates a one delay in the transitions.

        if (!filteringOneRead && abs(distLidar - distLidarOld)>1500) {
            distLidarFiltered = distLidarOld;
            filteringOneRead = true;
        } else {
            distLidarFiltered = distLidar;
            filteringOneRead = false;
        }
        distLidarOld = distLidar;

        //distLidarFiltered = distLidar;

        // Display OLED

        display.fillRect(0, 0, 128, 16, BLACK);
        display.setTextSize(1);      // Normal 1:1 pixel scale
        display.setTextColor(WHITE); // Draw white text
        display.setCursor(0, 0);     // Start at top-left corner
        display.cp437(true);        // Use full 256 char 'Code Page 437' font        
        display.print(distLidarFiltered); // "%04i", 

        int angleRadar = angle;
        
        if (increasing) angleRadar -= ANGLEINCR*3;
        else angleRadar +=ANGLEINCR*4;

        int xLine = map(angleRadar, ANGLEINI, ANGLEEND, 0, 127);
        int yLine = map(distLidarFiltered, 0, 2000, 16, 62);

        display.drawLine(xLine, 16, xLine, yLine, WHITE);
        display.drawLine(xLine, yLine + 1, xLine, display.height(), BLACK);

        display.display();

        // Update calculation

        if (increasing && angle<ANGLEEND) angleUpdate = ANGLEINCR;
        else if (increasing && angle>=ANGLEEND) { 
            increasing = false; 
            angleUpdate = -1 * ANGLEINCR;
        }
        else if (!increasing && angle>ANGLEINI) angleUpdate= -1 * ANGLEINCR;
        else if (!increasing && angle<=ANGLEINI) {
            increasing = true;
            angleUpdate = ANGLEINCR;
        }

        reversing = increasing != increasingOld;

        if (reversing) Serial.println("Rerversing!!");

        // Hole calculation

        holeFinder.update(angleRadar, distLidarFiltered, reversing);

        Serial.print(holeFinder.getTurn());
        Serial.print(" ");
        Serial.println(holeFinder.getAngleCenterHole());

        increasingOld = increasing;

        // Update Angle

        angle += angleUpdate;

        lastTime = millis();
    }

}

