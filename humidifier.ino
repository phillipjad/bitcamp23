//Includes for default arduino libraries to interface
//with digital humidity & temperature sensor (DHT11)
// and SG90 servo
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//defined pins for sensors
#define LED_SIGNAL 6    //pin for led to signal that humdifier is heating
#define RELAY_SIGNAL 8  //pin to activate relay
#define HUM_VCC 7       //pin for Humidity sensor voltage
#define HUM_DATA 3      //pin for Humidity sensor data
#define THERMO_CS 10    //pin for Thermistor chip select
#define THERMO_MISO 11  //pin that receives signals from thermistors
#define THERMO_VCC 12   //pin that powers thermistors
#define THERMO_SCK 13   //pin that syncs data exchange


//One-time initialization code before main looop drives device
void setup() {
  //relay setup and LED signal setup
  pinMode(RELAY_SIGNAL, OUTPUT);    //sets relay signal pin to output
  pinMode(LED_SIGNAL, OUTPUT);      //sets led signal pin to output
  //Humidity Sensor pin setup
  pinMode(HUM_VCC, OUTPUT);         //sets humidifiers voltage pin to output
  digitalWrite(HUM_VCC, HIGH);      //passes voltage through pin
  //Thermistor pin setup
  pinMode(THERMO_CS, OUTPUT);     //sets chip select pin to output
  digitalWrite(THERMO_CS, HIGH);  //passes voltage through pin
  pinMode(THERMO_MISO, INPUT);             //sets THERMO_MISO pin to input
  pinMode(THERMO_VCC, OUTPUT);      //sets humidifiers voltage pin to output
  digitalWrite(THERMO_VCC, HIGH);   //passes voltage through pin
  pinMode(THERMO_SCK, OUTPUT);      //sets humidifiers voltage pin to output
  Serial.begin(9600);               //Opens 9600 baud for output
}

//helper function to read signal from thermistor
uint8_t readSig() {
  uint8_t temp = 0;
  for (int i = 7; i >= 0; i--) {
    digitalWrite(THERMO_SCK, HIGH);  //passes voltage through pin
    if (digitalRead(THERMO_MISO)) {
      //if thermistor is attempting to pass signal it is
      //read into temp at proper bit
      temp |= (1 << i);
    }
    digitalWrite(THERMO_SCK, LOW);  //stops voltage through pin
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
  digitalWrite(THERMO_CS, LOW);
  delay(10);
  //lines 60-62 read signals from thermistor into signal variable
  signal = readSig();
  signal <<= 8;
  signal |= readSig();
  //sets CS state of thermistor to high voltage, signaling end of data read
  digitalWrite(THERMO_CS, HIGH);
  delay(10);
  //assigns useful bits of signal variable to temp variable
  //and applies resolution.
  temp = (signal >> 3) * 0.25;
  //Converts celsius temp reading to farenheit
  //Written in American :P.
  temp = temp * 9.0 / 5.0 + 32;
  hum = dht.readHumidity();
  Serial.println("tempF: " + String(temp));
  Serial.println("Humidity: " + String(hum) + "%\n");
  //Runs humidifier heating elemnt and state led
  //when suboptimal humidity is observed and 
  if (temp < 165 && hum < 60){
    digitalWrite(RELAY_SIGNAL, HIGH);
    digitalWrite(LED_SIGNAL, HIGH);
  }
  //Checks for invalid humidity sensor reading 
  //in order to prevent unwanted cooldown period.
  else if(String(hum).equals(" NAN")){;}
  else{
    //Deactivates humidifier in case of extraordinarily high temperatures
    //or humidity levels that are too high.
    Serial.println("Cooling!!!");
    digitalWrite(RELAY_SIGNAL, LOW);
    digitalWrite(LED_SIGNAL, LOW);
    delay(6000000);
  }
  delay(250);
}
