#!/usr/bin/env python
import time

import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

server = "iot.eclipse.org"

client = mqtt.Client()
client.on_connect = on_connect

client.connect(server, 1883, 60)

client.loop_start()

while True:
    time.sleep(5)
    client.publish("iotro/sub", "hello")