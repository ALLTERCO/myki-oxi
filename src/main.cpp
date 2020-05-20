#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUUID.h>
#include <BLEUtils.h>

#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 500

PulseOximeter pox;
uint32_t tsLastReport = 0;

// https://www.bluetooth.com/specifications/gatt/
// https://www.bluetooth.com/specifications/gatt/services/
// https://www.bluetooth.com/specifications/gatt/characteristics/

// PLXS	Pulse Oximeter Service
static const BLEUUID blePLXServiceUUID(0x1822U);
static const BLEUUID blePLXContMeasUUID(0x2A5FU);
static const BLEUUID blePLXSpotCheckMeasUUID(0x2A5EU);

static BLEServer *bleServer;
static BLEService *bleService;
static BLECharacteristic *blePLXContMeasChar;

void setup_oximeter(void) {
  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
    Serial.println("FAILED");
    for (;;)
      ;
  } else {
    Serial.println("SUCCESS");
  }
}

void setup_ble_gatts(void) {
  BLEDevice::init("MyKi Oxi");
  bleServer = BLEDevice::createServer();

  bleService = bleServer->createService(blePLXServiceUUID);

  blePLXContMeasChar = bleService->createCharacteristic(
      blePLXContMeasUUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  blePLXContMeasChar->addDescriptor(new BLE2902());
  bleService->start();

  BLEAdvertising *bleAdv = BLEDevice::getAdvertising();
  bleAdv->addServiceUUID(blePLXServiceUUID);
  bleAdv->setScanResponse(true);
  
  // functions that help with iPhone connections issue
  bleAdv->setMinPreferred(0x06);
  bleAdv->setMinPreferred(0x12);
  
  BLEDevice::startAdvertising();
}

void update(float hrf, float spo2f) {
  uint16_t tmp;
  uint8_t plxValue[5];

  plxValue[0] = 0x00;

  tmp = (uint16_t)round(spo2f);
  plxValue[1] = tmp & 0xff;
  plxValue[2] = (tmp >> 8) & 0xff;

  tmp = (uint16_t)round(hrf);
  plxValue[3] = tmp & 0xff;
  plxValue[4] = (tmp >> 8) & 0xff;

  blePLXContMeasChar->setValue(plxValue, sizeof(plxValue));
  blePLXContMeasChar->notify();
}

void setup() {
  Serial.begin(115200);
  setup_ble_gatts();
  setup_oximeter();
}

void loop() {
  // Make sure to call update as fast as possible
  pox.update();

  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float hr = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    Serial.print("Heart rate:");
    Serial.print(hr);
    Serial.print("bpm / SpO2:");
    Serial.print(spo2);
    Serial.println("%");

    tsLastReport = millis();

    update(hr, spo2);
  }
}