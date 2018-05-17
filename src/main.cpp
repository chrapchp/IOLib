
#include <Arduino.h>
#include <Streaming.h>

#include <DA_NonBlockingDelay.h>
#include <DA_AtlasMgr.h>

int s1 = 7;
int s2 = 8;
int s3 = 9;

char atlasrxBuff[DA_ATLAS_RX_BUF_SZ];

DA_AtlasMgr atlasSensorMgr = DA_AtlasMgr(Serial3, s1, s2, s3);
void onHeartBeat()
{
  atlasSensorMgr.serialize(&Serial, true);
}

DA_NonBlockingDelay KI_001 = DA_NonBlockingDelay(1000, onHeartBeat);

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);

  atlasSensorMgr.init();
  atlasSensorMgr.setPollingInterval(3000); // ms
  // atlasSensorMgr.sendRaw(4, "i",atlasrxBuff);
  //  Serial << "4:" << atlasrxBuff << endl;
  atlasSensorMgr.setEnabled(true);

}

void loop() {
  atlasSensorMgr.refresh();
  KI_001.refresh();

  // Serial << millis() << " " << endl;
  //  atlasSensorMgr.sendRaw(0, "i",atlasrxBuff);
}
