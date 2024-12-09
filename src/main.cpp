//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define LORA_DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22 
#define OLED_RST -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;

struct __attribute__((__packed__)) dataStruct
{
  int grimmValues[31];
  int partectorNumber;  // Parts/cm³
  int partectorDiam;    // nm
  float partectorMass;  // µg/m³
  float temperature;    // °C
  float humidity;       // %
  float pressure;       // hPa
  float altitude;       // m
  uint16_t co2;         // ppm
}data;


void setup() { 
  //initialize Serial Monitor
  Serial.begin(115200);
  
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA RECEIVER ");
  display.display();

  Serial.println("LoRa Receiver Test");
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.println("LoRa Initializing OK!");
  display.display();  
  //set syncWord
  LoRa.setSyncWord(0xB2);
  LoRa.enableCrc();
}

void loop() {

  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    //Serial.print("Received packet ");

    //read packet
    while (LoRa.available()) {
      LoRa.readBytes((uint8_t*) &data, packetSize);
    }

    //print RSSI of packet
    int rssi = LoRa.packetRssi();
    //Serial.print(" with RSSI ");    
    //Serial.println(rssi);

    // Dsiplay information
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Partector No.:");
    display.setCursor(0,20);
    display.print(data.partectorNumber);
    display.setCursor(0,30);
    display.print("RSSI:");
    display.setCursor(30,30);
    display.print(rssi);
    display.setCursor(0,40);
    display.print("T: ");
    display.setCursor(30,40);
    display.print(data.temperature);
    display.setCursor(0,50);
    display.print("CO: ");
    display.setCursor(30,50);
    display.print(data.co2);
    display.display();   

    Serial.write('<');
    Serial.write((const char*) &data, sizeof(data));
    Serial.write('>');
  }
}
