// M64 Modem Simple Test

#include <HardwareSerial.h>
#include "T4_WL_M64.h"

#define BUILT_IN_LED  13

struct M64_MODEM_INST modem;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial3.begin(115200);
  pinMode(BUILT_IN_LED, OUTPUT);
  if(M64_setup(&modem, M64_CHANNEL(1), M64_ROLE_CONTROLLER)){
    digitalWriteFast(BUILT_IN_LED, HIGH);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    char a = Serial.read();
    if(a) digitalToggle(BUILT_IN_LED);
    
    switch(a){
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
        Serial.println(a);
        M64_setup(&modem, M64_CHANNEL(a), modem.role);
        break;

      case 'a':
        Serial.printf("Set to bus controller\n");
        M64_setup(&modem, modem.channel, M64_ROLE_CONTROLLER);
      break;

      case 'b':
        Serial.printf("Set to bus listener\n");
        M64_setup(&modem, modem.channel, M64_ROLE_LISTENER);
      break;

      case 'c':
        Serial.printf("Requesting config...\n");
        M64_request(&modem, M64_CONFIG_GET);
      break;

      case 'v':
        Serial.printf("Requesting Version...\n");
        M64_request(&modem, M64_VERSION);
      break;

      case 'l':
        Serial.printf("Requesting TX queue length...\n");
        M64_request(&modem, M64_QUEUE_LEN);
      break;

      case 'f':
        Serial.printf("Requesting TX queue flush...\n");
        M64_request(&modem, M64_QUEUE_FLUSH);
      break;

      case 's':
        uint8_t packet_data[9] = "HeyyyYa!";
        Serial.printf("Sending: %s\n", packet_data);
        M64_send_packet(&modem, modem.supported_payload_size, packet_data);
      break;

      
    }
  }

  
  
 // char a = M64_parser(&modem);
  char a = M64_Events(&modem);

  switch(a){
    case M64_PACKET_RX:
      Serial.printf("RX Data: ");
      for(int n = 0; n < modem.supported_payload_size; n++){
        Serial.printf("%c", modem.rx_q[n]);
      }
      Serial.printf("\n");
    break;

    case M64_CONFIG_GET:
      Serial.printf("Current Role:\t%c\n", modem.role);
      Serial.printf("Current Channel: %c\n", modem.channel);
    break;

    case M64_CONFIG_SET:
      Serial.printf("Config Set Response: %s\n", (modem.last_set_okay == M64_ACK) ? "ACK" : "NACK");
    break;

    case M64_PACKET_TX:
      Serial.printf("TX Packet Response: %s\n", (modem.last_set_okay == M64_ACK) ? "ACK" : "NACK");
    break;

    case M64_QUEUE_FLUSH:
      Serial.printf("TX Queue Flush: %s\n", (modem.last_set_okay == M64_ACK) ? "SUCCESS" : "FAILED");
    break;

    case M64_QUEUE_LEN:
      Serial.printf("Current TX Queue Length: %6u bytes\n", modem.tx_q_len);
    break;

    default:
      if(a) Serial.printf("%c -> 0x%02X\n", a, a);
    break;
  }
}
