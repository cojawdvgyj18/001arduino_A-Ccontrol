#include <OneWire.h>
#include <DallasTemperature.h>
#define Compressor 4
#define Heater 5
#define Fan 6
#define ComHeater 7
#define PowerButton 2
#define DS18B20_sensors 3
#define one_wire_pin 3
#define error_light 8
#define relay_type 1
#define error_timecap 5
OneWire oneWire(one_wire_pin);
DallasTemperature DS18B20(&oneWire);
float sensor_temperature[DS18B20_sensors];
float temperature;
int sensor_error = 0;
int error_times = 0;
int HeaterON (-25);
int HeaterOFF (30);
int CompressorOFF (-130);
int CompressorRESTART (15);

void setup(){
  Serial.begin(9600);
  DS18B20.begin();
  pinMode(PowerButton,INPUT_PULLUP);
  pinMode(Compressor,OUTPUT);
  pinMode(Heater,OUTPUT);
  pinMode(Fan,OUTPUT);
  pinMode(ComHeater,OUTPUT);
  pinMode(error_light,OUTPUT);
  digitalWrite(error_light,LOW);
  digitalWrite(Compressor,(0^relay_type));
  digitalWrite(Heater,(0^relay_type));
  digitalWrite(Fan,(0^relay_type));
  digitalWrite(ComHeater,(0^relay_type));
  while (digitalRead(PowerButton)== HIGH){
    Serial.println("system no start, PLS press down botton");
  }
  while (digitalRead(PowerButton)== LOW){
    Serial.println("press down botton , system ready.");
  }
  delay(100);
}

void loop()
{
  gettemperature();
  Serial.print(temperature);
  Serial.println(" , system no start.");
  if ((temperature > CompressorRESTART) and (digitalRead(Heater) == HIGH)){
    CompressorON();
  }
}

void gettemperature()
{
  delay(150);
  while (error_times > error_timecap){
    digitalWrite(Compressor,(0^relay_type));
    digitalWrite(Heater,(0^relay_type));
    digitalWrite(Fan,(0^relay_type));
    digitalWrite(ComHeater,(0^relay_type));
    Serial.println("error , system off.");
    digitalWrite(error_light,!(digitalRead(error_light)));
    delay(200);
  }
  sensor_error = 0;
  temperature = 0;
  DS18B20.requestTemperatures();
  for(int _sensors = 0; _sensors<DS18B20_sensors;_sensors=_sensors+1){
    sensor_temperature[(_sensors)] = DS18B20.getTempCByIndex(_sensors);
    Serial.println(sensor_temperature[(_sensors)]);
    if (sensor_temperature[(_sensors)] != (-127)){
      Serial.print(",");
      Serial.print(sensor_temperature[(_sensors)]);
    }
    if (sensor_temperature[(_sensors)] == (-127)){
      sensor_error = sensor_error + 1;
      Serial.print(",");
      Serial.print("No. ");
      Serial.print(_sensors);
      Serial.print("sensor error");
      sensor_temperature[(_sensors)] =0;
    }
    temperature = sensor_temperature[(_sensors)] + temperature;
  }
  if (sensor_error >= DS18B20_sensors){
    Serial.print(",");
    Serial.println("all sensors error");
    error_times = error_times +1;
    for(int _errordelay=0;_errordelay<100;_errordelay=_errordelay-1){
      digitalWrite(error_light,!(digitalRead(error_light)));
      delay(_errordelay+20);
    }
  }
  else {
    temperature = temperature / ( DS18B20_sensors - sensor_error );
  }
}

void CompressorON()
{
  Serial.println("system start.");
  digitalWrite(Compressor,(1^relay_type));
  delay(1000);
  FanPOWER ();
  while (temperature >= CompressorOFF){
    gettemperature();
    Serial.print(temperature);
    Serial.println(" ,Compressor start.");
  }
  while (temperature < CompressorOFF){
    gettemperature();
    Serial.print(temperature);
    Serial.println(" ,Achieve goals");
    digitalWrite(Compressor,(0^relay_type));
    HeaterPOWER();
  }
  
}

void HeaterPOWER(){
  FanPOWER ();
  digitalWrite(Heater,(1^relay_type));
  int timer = 300 ;
  while ((temperature < CompressorRESTART) or (timer >=0)){
    gettemperature();
    Serial.print(temperature);
    Serial.println(" ,Heater working.");
    if(temperature >= CompressorRESTART){
      digitalWrite(Heater,(0^relay_type));
    }
    timer = timer-1;
  }
  digitalWrite(Heater,(0^relay_type));
  CompressorON();
}

void FanPOWER(){
  if (digitalRead(Compressor) == (1^relay_type)) {
    digitalWrite(Fan,(1^relay_type));
    Serial.println(" Fan power on.");
  }
  else{
    digitalWrite(Fan,(0^relay_type));
    Serial.println(" Fan power off.");
  }
}
