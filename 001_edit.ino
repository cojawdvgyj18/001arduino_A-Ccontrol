//標頭檔
#include <OneWire.h>
#include <DallasTemperature.h>

//定義接腳
#define PowerButton 2
  //啟動按鈕接腳
#define one_wire_pin 3
  //one_wire通訊腳
#define Compressor 4
  //壓縮機relay接腳
#define Heater 5
  //加熱器relay接腳
#define Fan 6
  //風扇控制器接腳
#define ComHeater 7
  //壓縮機管路加熱器relay接腳
#define error_light 8
  //報錯燈接腳

//啟動功能
OneWire oneWire(one_wire_pin);
DallasTemperature DS18B20(&oneWire);

//設定用定義
#define relay_type 1
  //設定繼電器類型，1=低態作動，0=高太作動
#define DS18B20_sensors 3
  //設定所接傳感器數量
#define error_timecap 5
  //設定錯誤容許
#define HeaterOFF (25)
#define CompressorOFF (-30)
#define CompressorSTART (15)
#define CompressorRESTART (20)
  //設定各控制點溫度

float sensor_temperature[DS18B20_sensors];
float temperature;
int sensor_error ;
int error_times ;

//取溫
float gettempereature () {
  int _tempereature;
  if (error_times > error_timecap) {
    int _blink = 10;
    Serial.println("error , system off.");
    digitalWrite(Compressor,(0^relay_type));
    digitalWrite(Heater,(0^relay_type));
    digitalWrite(Fan,(0^relay_type));
    digitalWrite(ComHeater,(0^relay_type));
    while (_blink > 0) {
      digitalWrite(error_light,!(digitalRead(error_light)));
      delay(200);
    }
    break;
  }
  sensor_error = 0;
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
    _temperature = sensor_temperature[(_sensors)] + _temperature;
  }
  if (sensor_error >= DS18B20_sensors){
    Serial.print(",");
    Serial.println("all sensors error");
    error_times = error_times +1;
    for(int _errordelay=0;_errordelay<10;_errordelay=_errordelay-1){
      digitalWrite(error_light,!(digitalRead(error_light)));
      delay(100);
    }
  }
  else {
    _temperature = _temperature / ( DS18B20_sensors - sensor_error );
    return _temperature
  }
}
