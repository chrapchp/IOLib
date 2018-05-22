
#include <Arduino.h>
#include <Streaming.h>
#include <avr/wdt.h>
#include <Ethernet.h>

#include <DA_NonBlockingDelay.h>
#include <DA_AtlasMgr.h>
#include <DA_RemoteIOServer.h>


#ifdef CONTROLLINO_BUILD
  # define CONTROLLINO_MAXI_AUTOMATION
  # include <Controllino.h>
  # define S1 CONTROLLINO_PIN_HEADER_DIGITAL_OUT_12
  # define S2 CONTROLLINO_PIN_HEADER_DIGITAL_OUT_13
  # define S3 CONTROLLINO_PIN_HEADER_DIGITAL_OUT_14
#else // ifdef CONTROLLINO_BUILD
  # define S1 7
  # define S2 8
  # define S3 9
#endif // ifdef CONTROLLINO_BUILD

uint8_t s1 = S1;
uint8_t s2 = S2;
uint8_t s3 = S3;

#define DEFAULT_IP_ADDRESS   192, 168, 1, 253 // 192.168.1.253
#define DEFAULT_GATEWAY    192, 168, 1, 1     // 192.168.1.1
#define DEFAULT_SUBNET_MASK  255, 255, 255, 0 // 255.255.255.0
#define DEFAULT_MAC_ADDRESS 0xDE, 0xAD, 0xBE, 0x0E, 0x94, 0xB5

IPAddress defaultIP(DEFAULT_IP_ADDRESS);
IPAddress defaultGateway(DEFAULT_GATEWAY);
IPAddress defaultSubnet(DEFAULT_SUBNET_MASK);
byte defaultMAC[] = { DEFAULT_MAC_ADDRESS };

char atlasrxBuff[DA_ATLAS_RX_BUF_SZ];

DA_AtlasMgr atlasSensorMgr = DA_AtlasMgr(Serial2, s1, s2, s3);
void onHeartBeat()
{
  atlasSensorMgr.serialize(&Serial, true);
}

DA_NonBlockingDelay KI_001 = DA_NonBlockingDelay(5000, onHeartBeat);
DA_RemoteIOServer remoteIOserver = DA_RemoteIOServer();
void setup() {

  Ethernet.begin(defaultMAC, defaultIP, defaultGateway, defaultSubnet);

  Serial.begin(9600);
  Serial2.begin(9600);

  atlasSensorMgr.init();
  atlasSensorMgr.setPollingInterval(10000); // ms
  // atlasSensorMgr.sendRaw(4, "i",atlasrxBuff);
  //  Serial << "4:" << atlasrxBuff << endl;
  atlasSensorMgr.setEnabled(true);
  remoteIOserver.init();
  remoteIOserver.setAtlasMgr(&atlasSensorMgr);
}

void loop() {

  remoteIOserver.refresh();



  //atlasSensorMgr.refresh();
  //KI_001.refresh();

  // Serial << millis() << " " << endl;
  //  atlasSensorMgr.sendRaw(0, "i",atlasrxBuff);
}
