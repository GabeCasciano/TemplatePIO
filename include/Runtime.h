#ifndef RUNTIME_H_
#define RUNTIME_H_

#include "IPAddress.h"
#include "Logger.h"
#include <Arduino.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>

#ifndef ENET_CS
#define ENET_CS (10)
#endif

#ifndef MQTT_PORT
#define MQTT_PORT (1883)
#endif

#ifndef RETRY
#define RETRY (5)
#endif

#ifndef AN0
#define AN0 (0)
#endif

#ifndef AN1
#define AN1 (1)
#endif

#ifndef DIO0
#define DIO0 (0)
#endif

#ifndef DIO1
#define DIO1 (1)
#endif

#define IP_PRINTER(x) x[0], x[1], x[2], x[3]

namespace Runtime {

struct NetworkStruct {
  byte mac[6] = {0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED};
  IPAddress local_ip;
  IPAddress broker_ip;
  String client_name = "client";
  EthernetClient eth_client;
  PubSubClient *mqtt_client = nullptr;

  NetworkStruct(IPAddress local, IPAddress broker)
      : local_ip(local), broker_ip(broker) {
    mqtt_client = new PubSubClient(eth_client);

    LOGGER(INFO,
           "Setting up network with CS: %d, \n local ip: %d.%d.%d.%d broker "
           "ip: %d.%d.%d.%d:%d",
           ENET_CS, IP_PRINTER(local_ip), IP_PRINTER(broker_ip), MQTT_PORT);

    Ethernet.init(ENET_CS);
    Ethernet.begin(mac, local_ip);

    if (Ethernet.hardwareStatus() == EthernetNoHardware)
      LOGGER(ERROR, "No ethernet hardware");

    if (Ethernet.linkStatus() == LinkOFF)
      LOGGER(ERROR, "No Link");

    mqtt_client->setServer(broker_ip, MQTT_PORT);
  }
  ~NetworkStruct() {
    mqtt_client->disconnect();
    if (mqtt_client != nullptr)
      delete mqtt_client;
  }
};

NetworkStruct *network;

inline void Setup() {
  Serial.begin(9600);
  network = new NetworkStruct(IPAddress(1, 1, 1, 1), IPAddress(1, 1, 1, 2));
}

inline void Loop() {

  // runtime stuff first

  // Checking we are connceted
  if (!network->mqtt_client->connected()) {
    LOGGER(WARNING, "Reconnect");
    if (Ethernet.linkStatus() == LinkOFF) {
      LOGGER(WARNING, "Lost link");
      return;
    }
    if (network->mqtt_client->connect(network->client_name.c_str())) {
      LOGGER(WARNING, "Could not connect");
      return;
    }
  } else {

    // pubing
    network->mqtt_client->loop();
  }
}

} // namespace Runtime

#endif
