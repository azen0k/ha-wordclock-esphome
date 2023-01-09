#include "esphome.h"
#include <FastLED.h>

#define NUM_LEDS 125
#define DATA_PIN D5

// esphome dependencies:
// needs: esphome time --> id: current_time
// needs: esphome fastled --> id: fastledlight

// Layout of Clock (X not used chars) - PIN Layout
// ILNESTODEUX    1.......11 LIGNE 1
// QUATRETROIS    22......12 LIGNE 2
// NEUFUNESEPT    23......33 LIGNE 3
// HUITSIXCINQ    44......34 LIGNE 4
// MIDIXMINUIT    45......55 LIGNE 5
// ONZERHEURES    46......56 LIGNE 6
// MOINSOLEDIX    67......57 LIGNE 7
// ETRQUARTPMD    68......78 LIGNE 8
// VINGT-CINQU    89......79 LIGNE 9
// ETSDEMIEPAM    109.......119 LIGNE 10
// MINUTES        124.......120

int leds_time_it_is[] = {0, 1, 3, 4, 5}; // IL EST
int leds_minutes[] = {124, 123, 122, 121}; // Minutes LEDS
int leds_skiped[] = {110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120}; // Check Line 101
int leds_time_minutes[][15] = {
    { 51,  52,  53,  54,  55,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // HEURE
    { 80,  81,  82,  83,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // CINQ
    { 57,  58,  59,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // DIX
    {108, 109,  71,  72,  73,  74,  75,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // ET QUART
    { 85,  86,  87,  88,  89,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // VINGT
    { 89,  88,  87,  86,  85,  84,  83,  82,  81,  80,  -1,  -1,  -1,  -1,  -1}, // VINGT CINQ
    {109, 108, 106, 105, 104, 103, 102,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // ET DEMIE
    { 85,  86,  87,  88,  89,  84,  83,  82,  81,  80,  67,  66,  65,  64,  63}, // MOINS VINGT CINQ
    { 85,  86,  87,  88,  89,  67,  66,  65,  64,  63,  -1,  -1,  -1,  -1,  -1}, // MOINS VING
    { 67,  66,  65,  64,  63,  60,  61,  71,  72,  73,  74,  75,  -1,  -1,  -1}, // MOINS LE QUART
    { 85,  86,  87,  88,  89,  57,  58,  59,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // MOINS DIX
    { 85,  86,  87,  88,  89,  80,  81,  82,  83,  -1,  -1,  -1,  -1,  -1,  -1}  // MOINS CINQ
};

int leds_time_hours[][6] = {
    { 45,  46,  47,  48,  -1,  -1}, // MIDI
    { 27,  28,  29,  -1,  -1,  -1}, // UNE
    {  8,   9,  10,  11,  -1,  -1}, // DEUX
    { 12,  13,  14,  15,  16,  -1}, // TROIS
    { 17,  18,  19,  20,  21,  22}, // QUATRE
    { 34,  35,  36,  37,  -1,  -1}, // CINQ
    { 38,  39,  40,  -1,  -1,  -1}, // SIX
    { 30,  31,  32,  33,  -1,  -1}, // SEPT
    { 41,  42,  43,  44,  -1,  -1}, // HUIT
    { 23,  24,  25,  26,  -1,  -1}, // NEUF
    { 47,  48,  49,  -1,  -1,  -1}, // DIX
    { 46,  47,  48,  49,  -1,  -1}  // ONZE
    { 50,  51,  52,  53,  54,  56}  // MINUIT
};

CRGB leds[NUM_LEDS];
int hour = -1;
int minute = -1;
int red = 124;
int green = 124;
int blue = 124;
int brightness = 50;

class Wordclock : public Component, public CustomAPIDevice {
    public:
        void setup() override {
            FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
            FastLED.setBrightness(brightness);
            // Start all LED with on and default color and brightness to check if everything is working...
            for(int i = 0; i < NUM_LEDS; i++) { leds[i].setRGB(red, 0, 0); FastLED.show(); delay(10); }
            for(int i = 0; i < NUM_LEDS; i++) { leds[i].setRGB(0, green, 0); FastLED.show(); delay(10); }
            for(int i = 0; i < NUM_LEDS; i++) { leds[i].setRGB(0, 0, blue); FastLED.show(); delay(10); }
            for(int i = 0; i < NUM_LEDS; i++) { leds[i].setRGB(0, 0, 0); }
            FastLED.show();
            register_service(&Wordclock::on_setled, "setled", {"number","red", "blue", "green"});
        }
        void on_setled(int number, int red, int blue, int green) {
            if (number < NUM_LEDS || number > 0) {
                ESP_LOGD("setled", "Setting led number %d to color %i %i %i", number, red, green, blue );
                leds[number].setRGB(red, green, blue);
                FastLED.show();
                }
            else { ESP_LOGE("setled", "Not a valid LED Number - out of range"); }
        }
        void loop() override {
            auto time = id(current_time).now();
            int h = time.hour;
            int m = time.minute;
            // https://www.esphome.io/api/classesphome_1_1light_1_1_light_color_values.html LightColorValues Class
            auto fastledlight2 = id(fastledlight).current_values;
            //convert float 0.0 till 1.0 into int 0 till 255
            red = (int)(fastledlight2.get_red()*255);
            green = (int)(fastledlight2.get_green()*255);
            blue = (int)(fastledlight2.get_blue()*255);
            brightness = 0;
            //check if light is on and set brightness
            if (fastledlight2.get_state() > 0 ) { brightness = (int)(fastledlight2.get_brightness()*255); }
            else { ESP_LOGD("loop", "fastledlight state off - b: %i rgb %i %i %i", brightness, red, green, blue); delay(500);}
            FastLED.setBrightness(brightness);
            //check if valid time. Blink red,green,blue until valid time is present
            if (time.is_valid() == false) {
                ESP_LOGE("loop", "Got invalid time from current_time Time: %i:%i", h, m );
                leds[0].setRGB(255, 0, 0); FastLED.show(); delay(250);
                leds[0].setRGB(0, 255, 0); FastLED.show(); delay(250);
                leds[0].setRGB(0, 0, 255); FastLED.show(); delay(250);
                leds[0].setRGB(0, 0, 0);   FastLED.show();
            }
            else {
                // only update once in a Minute
                if(h != hour || m != minute) {
                   //ESP_LOGD("loop", "Using b: %i rgb %i %i %i", brightness, red, green, blue);
                    hour = h;
                    minute = m;
                    if (hour >= 0 && time.is_valid() == true){
                        int tmp_hour = hour;
                        int tmp_minute = (minute - (minute % 5));
                        if(tmp_minute >= 25) { tmp_hour += 1; }
                        tmp_minute = tmp_minute / 5;
                        tmp_hour = tmp_hour % 12;
                        int minutessum = minute % 5;
                        // Reset all LED, but skip LED 110 till 120
                        for(int i = 0; i < NUM_LEDS; i++) {     if(i < 110 || i > 120) leds[i].setRGB(0, 0, 0); }
                        for(int i = 0; i < 5; i++) {            leds[leds_time_it_is[i]].setRGB(red, green, blue); }
                        for(int i = 0; i < 15; i++) {           if(leds_time_minutes[tmp_minute][i] >= 0) { leds[leds_time_minutes[tmp_minute][i]].setRGB(red, green, blue); } }
                        for(int i = 0; i < 6; i++) {            if(leds_time_hours[tmp_hour][i] >= 0) { leds[leds_time_hours[tmp_hour][i]].setRGB(red, green, blue); } }
                        for(int i = 0; i < minutessum; i++) {   leds[leds_minutes[i]].setRGB(red, green, blue);}
                        FastLED.show();
                        ESP_LOGD("loop", "Update Time: %i:%i  Brightness: %i RGB: %i-%i-%i", hour, minute, brightness, red, green, blue);
                        ESP_LOGD("loop", "Using tmp_hour: %i tmp_minute: %i minutessum: %i", tmp_hour, tmp_minute, minutessum);
                    }
                }
            }
        }
};
