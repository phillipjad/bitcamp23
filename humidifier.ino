//Includes for default arduino libraries to interface
//with digital humidity & temperature sensor (DHT11)
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


//defined pins for sensors
#define HUM_VCC 7 //pin for Humidity sensor voltage
#define HUM_DATA 3 //pin for Humidity sensor data 
#define CHIP_SELECT 10 //pin for Thermistor chip select
#define MISO 11 //pin that receives signals from thermistors
#define THERMO_VCC 12 //pin that powers thermistors
#define THERMO_SCK 13 //pin that syncs data exchange


//One-time initialization code before main looop drives device
void setup() {
  //Humidity Sensor pin setup
  pinMode(HUM_VCC, OUTPUT); //sets humidifiers voltage pin to output
  digitalWrite(HUM_VCC, HIGH); //passes voltage through pin
  //Thermistor pin setup 
  pinMode(CHIP_SELECT, OUTPUT); //sets chip select pin to output
  digitalWrite(CHIP_SELECT, HIGH); //passes voltage through pin
  pinMode(MISO, INPUT); //sets MISO pin to input
  pinMode(THERMO_VCC, OUTPUT); //sets humidifiers voltage pin to output
  digitalWrite(THERMO_VCC, HIGH); //passes voltage through pin
  pinMode(THERMO_SCK, OUTPUT); //sets humidifiers voltage pin to output
  Serial.begin(9600); //Opens 9600 baud for output
}

//helper function to read signal from thermistor
uint8_t readSig(){
  uint8_t temp = 0;
  for (int i = 7; i >= 0; i--){
    digitalWrite(THERMO_SCK, HIGH); //passes voltage through pin
    if (digitalRead(MISO)){
      //if thermistor is attempting to pass signal it is
      //read into temp at proper bit
      temp |= (1 << i);
    }
    digitalWrite(THERMO_SCK, LOW); //stops voltage through pin
  }
  return temp;
}

//main driver function
void loop() {
  //raw signal from thermistor
  uint16_t signal = 0;
  //values for water temperature and air humidity
  float temp = 0, hum = 0;
  //Initializes DHT11 sensor for usage
  DHT dht(HUM_DATA, DHT11);
  dht.begin();
  //prepares to read data from thermistor
  digitalWrite(CHIP_SELECT, LOW);
  delay(10);
  //lines 60-62 read signals from thermistor into signal variable
  signal = readSig();
  signal <<= 8;
  signal |= readSig();
  //returns CS state of transistor to high voltage
  digitalWrite(CHIP_SELECT, HIGH);
  delay(10);
  //assigns useful bits of signal variable to temp variable
  //and applies resolution
  temp = (signal >> 3) * 0.25;
  Serial.println("tempC: " + String(temp));
  temp = temp * 9.0/5.0 + 32;
  Serial.println("tempF: " + String(temp));
  hum = dht.readHumidity();
  Serial.println("Humidity: " + String(hum) + "%");
  delay(5000);
}
