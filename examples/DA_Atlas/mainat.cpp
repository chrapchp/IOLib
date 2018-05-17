
#include <Arduino.h>
#include <Streaming.h>

#include <ArduinoUnit.h>


#include <DA_AtlasMgr.h>

int s1 = 7;
int s2 = 8;
int s3 = 9;


DA_AtlasMgr atlasSensorMgr = DA_AtlasMgr(Serial3, s1,s2,s3 );


void setup() {

  Serial.begin(9600);


  atlasSensorMgr.init();
}


void loop() {


}
