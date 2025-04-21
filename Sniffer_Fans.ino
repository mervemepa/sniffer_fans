#include <FastLED.h>

//  triangular  GPIO25
//  sinusoidal  GPIO26
//  square      GPIO26

#include <math.h>

// Ayarlar
int sinTableSize = 100; // Sinüs dalgası için örnek sayısı
int sinTable[100]; // Sinüs dalgası verileri
const int timerInterval = 100; // Mikro-saniye cinsinden zaman aralığı (Frekans ayarı)



const int freq = 1000; // Frequency in Hz
const int resolution = 8; // PWM resolution
// How many leds in your strip?
#define NUM_LEDS 12

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATAPIN 17
#define CLOCK_PIN 13


// Define the array of leds
CRGB leds[NUM_LEDS];
// Define the array of leds

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
int enable1Pin = 23;
#include <driver/dac.h> // DAC için gerekli kütüphane
#include <esp_timer.h> // Zamanlayıcı için gerekli kütüphane
#include <math.h>

int scanTime = 1; // Tarama süresi (saniye)
BLEScan* pBLEScan;
float rssiTotal = 0;
int deviceCount = 0;
const int fanPin = 23; // PWM çıkışı için kullanılan pin
// Setting PWM properties

const int pwmChannel = 0;

int dutyCycle = 200;


const int signalPin = 25; // Sinyal çıkışı için kullanılan DAC pin

float rssiAverage = 0; // Ortalama RSSI
esp_timer_handle_t signalTimer; // Zamanlayıcı nesnesi

int signalFrequency = 100; // Sinyal frekansı (Hz)
float signalAmplitude = 0; // Sinyal genliği

// Bluetooth cihazı bulunduğunda çağrılan callback
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      String deviceInfo = advertisedDevice.toString().c_str();
      int rssi = advertisedDevice.getRSSI();
      String Gname = advertisedDevice.getName().c_str();
      String Gadd =  advertisedDevice.getAddress().toString().c_str();

      String cihaz =  Gname + " MAC: " + Gadd + " RSSI: " + String(rssi);

//
//      Serial.print("Cihaz Bulundu: ");
//      Serial.println(deviceInfo);
//
//      Serial.print("RSSI: ");
//      Serial.println(rssi);

      // RSSI toplamını ve cihaz sayısını artır
      rssiTotal += rssi;
      deviceCount++;
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Bluetooth Cihaz Tarayıcı Başlatılıyor...");
  FastLED.addLeds<APA104, DATAPIN, RGB>(leds, NUM_LEDS);
  // Fan pinini çıkış olarak ayarla
  pinMode(fanPin, OUTPUT);


  ledcAttachPin(enable1Pin, 1);

  ledcSetup(1, 1000, 8); // 12 kHz PWM, 8-bit resolution

  // Bluetooth başlat
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // BLE tarayıcı oluştur
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Aktif tarama açık


}

void loop() {


  Serial.println("Cihazları tarıyor...");

  // Tarama başlat
  BLEScanResults scanResults = pBLEScan->start(scanTime, false);
  if (deviceCount > 0) {
    char resultCountText[50];
    sprintf(resultCountText, "Bulunan cihaz sayısı: %d", scanResults.getCount());
    float rssiAverage = rssiTotal / deviceCount;
    char rssiAverageText[50];
    sprintf(rssiAverageText, "RSSI Ortalaması: %.2f", rssiAverage);
    int pwmValue = map(rssiAverage, -100, 0, 50, 255); // RSSI değerini 0-255 arasına ölçekle
    pwmValue = constrain(pwmValue, 50, 255); // PWM değerini sınırla
          Serial.print("pwm Value ");
      Serial.println(pwmValue);
    ledcWrite(enable1Pin,  pwmValue);

    for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(pwmValue, pwmValue, pwmValue); 
  }

  FastLED.show();   
    
  }
  rssiTotal = 0;
  deviceCount = 0;

  //delay(1000); // 10 saniye bekle

}
