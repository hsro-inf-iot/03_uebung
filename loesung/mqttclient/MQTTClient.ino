#include <AZ3166WiFi.h>
#include "MQTTClient.h"
#include "MQTTNetwork.h"
#include "HTS221Sensor.h"
#include "parson.h"

// Debug flag
#define DEBUG 1

// Sampling interval for sending data
#define INTERVAL 2000
// maximal length of massage lenght
#define MESSAGE_MAX_LEN 256

// define topics for pub and sub 
const char* pub_topic = "iotro/pub";
const char* sub_topic = "iotro/sub";

int status = WL_IDLE_STATUS;
int arrivedcount = 0;
bool hasWifi = false;

DevI2C *i2c;
HTS221Sensor *sensor;

const char* mqttServer = "iot.eclipse.org";
int port = 1883;

MQTTNetwork mqttNetwork;
MQTT::Client<MQTTNetwork, Countdown> client = MQTT::Client<MQTTNetwork, Countdown>(mqttNetwork);


bool mqttConnected = false;

/*
  Init the sensor
*/
void SensorInit()
{
    i2c = new DevI2C(D14, D15);
    sensor = new HTS221Sensor(*i2c);
    sensor->init(NULL);
}

/*
  Read temparature from sensor
*/
float readTemperature()
{
    sensor->reset();

    float temperature = 0;
    sensor->getTemperature(&temperature);

    return temperature;
}

/*
  Read humidity from sensor
*/
float readHumidity()
{
    sensor->reset();

    float humidity = 0;
    sensor->getHumidity(&humidity);

    return humidity;
}

/*
  Simple logging to console id DEGUB is set
*/
void log(String msg)
{
  if (DEBUG==1)
  {
    Serial.println(msg);
  }
}

/*
  Process the message for sending to server as JSON
*/
void sendMessage(int messageId, char *payload)
{
    log("--> sendMessage");

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;

    json_object_set_number(root_object, "messageId", messageId);

    float t = readTemperature();
    float h = readHumidity();

    json_object_set_number(root_object, "temperature", t);
    json_object_set_number(root_object, "humidity", h);

    serialized_string = json_serialize_to_string_pretty(root_value);

    // this write data to buf as payload which gets send to server
    snprintf(payload, MESSAGE_MAX_LEN, "%s", serialized_string);
    log(serialized_string);

    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    log("<-- sendMessage");
}

/*
  Initialize WiFi
*/
void initWifi() {
  Screen.print(0, "Wi-Fi Connecting");
  Serial.print("Attempting to connect to Wi-Fi");
  
  if (WiFi.begin() == WL_CONNECTED) {
    IPAddress ip = WiFi.localIP();
    Screen.print(0, "Wi-Fi Connected");
    Screen.print(1, ip.get_address());
    hasWifi = true;
    Screen.print(2, "Running... \r\n");
  } else {
    Screen.print(1, "No Wi-Fi\r\n ");
  }
}

/*
  Callback for incoming messages
*/
void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;

    char msgInfo[60];
    sprintf(msgInfo, "Message arrived: qos %d, retained %d, dup %d, packetid %d", message.qos, message.retained, message.dup, message.id);
    Serial.println(msgInfo);

    sprintf(msgInfo, "Payload: %s", (char*)message.payload);
    Serial.println(msgInfo);
    ++arrivedcount;
}

/*
  Init the MQTT client
*/
void initMqttClient()
{

  arrivedcount = 0;
  
  char msgBuf[100];
  sprintf(msgBuf, "Connecting to MQTT server %s:%d", mqttServer, port);
  Serial.println(msgBuf);

  int rc = mqttNetwork.connect(mqttServer, port);
  if (rc != 0) {
    Serial.println("Connected to MQTT server failed");
  } else {
    Serial.println("Connected to MQTT server successfully");
    mqttConnected = true;
  }

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.MQTTVersion = 3;
  data.clientID.cstring = (char*)"iotro";
  data.username.cstring = (char*)"testuser";
  data.password.cstring = (char*)"testpassword";
  
  if ((rc = client.connect(data)) != 0) {
      Serial.println("MQTT client connect to server failed");
  }
  
  if ((rc = client.subscribe(sub_topic, MQTT::QOS2, messageArrived)) != 0) {
      Serial.println("MQTT client subscribe from server failed");
  }
}

void disconnect()
{
  if (mqttConnected)
  {
    int rc;
    if ((rc = client.unsubscribe(sub_topic)) != 0) {
        Serial.println("MQTT client unsubscribe from server failed");
    }
    
    if ((rc = client.disconnect()) != 0) {
        Serial.println("MQTT client disconnect from server failed");
    }
    mqttNetwork.disconnect();
  }
}

int runMqtt() {
   MQTT::Message message;

  // Send message
  char buf[MESSAGE_MAX_LEN];
  sendMessage(1, buf);
  message.qos = MQTT::QOS0;
  message.retained = false;
  message.dup = false;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf)+1;
  int rc = client.publish(pub_topic, message);
  return rc;
}

void setup() {
  //Initialize serial and Wi-Fi:
  Serial.begin(115200);
  SensorInit();
  initWifi();
  initMqttClient();
}

void loop() {
  Serial.println("\r\n>>>Enter Loop");

  if (hasWifi && mqttConnected) {
    runMqtt();
    client.yield(100);
  }

  delay(INTERVAL);
}

