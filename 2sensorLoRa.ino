#include "DFRobot_PH.h" //library untuk sensor ph
#include <OneWire.h>  //library untuk pembacaan sensor ds18b20
#include <DallasTemperature.h>  //library untk sensor ds18b20
#include <SPI.h>
#include <LoRa.h> 
#define sensor_ds18b20 3 //pada pin 3 untuk sensor ds18b20
 
OneWire oneWire(sensor_ds18b20);
 
DallasTemperature sensors(&oneWire); //penggunaan interface 1 wire untuk pengaksesan
 
#define sensor_ph A0 //pembacaan sensor ph menggunakan ADC
 
float tegangan,nilaiph,suhu;
DFRobot_PH ph;
int counter = 0;
uint8_t datasend[72];

char suhu_1[8]={"\0"},nilaiph_1[8]={"\0"};

void setup()  {
   Serial.begin(9600);  //serial monitor
   while (!Serial);

   Serial.println("LoRa Node 1");
    
   if (!LoRa.begin(8681E5)) {
 
    Serial.println("Starting LoRa failed!");
    while (1);
    }
  
}



void monitoring()
{
  
  
   ph.begin(); //mulai untuk pembacaan sensor ph meter
   sensors.begin(); //mulai untuk pembacaan sensor ds18b20

    sensors.requestTemperatures(); 
  static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        tegangan = analogRead(sensor_ph)/1024.0*5000; //mengubah tegangan analog menjadi digital dan menjadi tegangan
        suhu = sensors.getTempCByIndex(00);
        nilaiph = ph.readPH(tegangan,suhu);  //konversi tegangan menjadi ph meter dengan kompensasi suhu
        //Serial.print("Nilai Suhu (^C) : ");
        //Serial.print(suhu,1); //nilai pembacaan suhu
        //Serial.println("^C \n");
        //Serial.print (" Nilai pH       : ");
        //Serial.println(nilaiph,2); //nilai pembacaan ph meter

        Serial.println(F("Nilai Suhu dan pH pada Sungai :"));
        Serial.print("[");
        Serial.print(suhu,1);
        Serial.print("℃");
        Serial.print(" , ");
        Serial.print(nilaiph,2);
        Serial.print("]");
        Serial.println("");
        
                
        delay(2000);
            }
  }



void monitoringWrite()
{
        char data[70] = "\0";

        //monitoringstrf(nilaiph,0,1,suhu_1);
        //monitoringstrf(suhu,0,1,nilaiph_1);
        
        dtostrf(nilaiph, 0, 1, suhu_1);
        dtostrf(suhu, 0, 1, nilaiph_1);

        strcat(data,"Nilai Suhu dan pH pada Sungai :");
        strcat(data,"[");
        strcat(data,nilaiph_1);
        strcat(data," ℃");
        strcat(data,",");
        strcat(data,suhu_1);
        strcat(data,"]");
        strcpy((char *)datasend,data);
  } 


void loop()
{

        
        Serial.print("Semding packet: ");
        Serial.println(counter);
        monitoring();
        monitoringWrite();
        //send packet
        LoRa.beginPacket();
        LoRa.print((char*)datasend);
        //LoRa.print(counter);
        LoRa.endPacket();

        counter++;

        delay(5000);
         
}
