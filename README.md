# Übung - 03: MQTT and CoAP

## Ziel der Übung

In dieser Übung soll ein wenig Erfahrung mit den MQTT und CoAP Protokollen gesammelt werden. Hierzu sollen CoAP-Client, MQTT Publisher und MQTT Subscriber implementiert werden.

Als MQTT Server/Broker verwenden wir den öffentlich zugänglicher IOT Server vom Paho Projekt von der Eclipse Foundation: 

 - Eclipse IoT projects available at iot.eclipse.org
 - port 1883 

Alternativ können Sie auch den Inf-IOT MQTT Broker verwenden:

  - inf-iot.westeurope.azurecontainer.io
  - port 1883

Oder Sie hosten eine eigene Container-Instanz

  - ```docker pull eclipse-mosquitto```
  - ```docker run -it -p 1883:1883  eclipse-mosquitto```

---

## Aufgabe 1

### a)

Implementieren Sie eine Subscriber und einen Publisher in der Programmiersprache ihrer Wahl. Gute und einfache Anleitungen finden sich im Internet.

- Java: https://www.baeldung.com/java-mqtt-client
- Python: https://www.dinotools.de/2015/04/12/mqtt-mit-python-nutzen/
- JavaScript/Node : https://github.com/mqttjs/MQTT.js
- Elixir: https://github.com/gausby/tortoise

NOTE: Achten sie darauf welches Topic sie verwenden!

b)

Verwenden Sie ihre Implementierung aus a und verfeinern diese, indem Sie _Temperatur_ und _CO2_ Werte aus verschiedenen Räumen _Labor_, _Office_ und _Lager_ publisher. Ihr Subscriber soll Wildcards verwenden, um dann _Temperatur_ und _CO2_ aus dem gesamten Gebäude auszulesen.

- Wie sehen Ihre Topics aus?
- Wie sehen die Wildcards aus?

## Aufgabe 2

Schreiben Sie eine CoAP-Client in der Programmiersprache ihrer Wahl - aber **nicht** in Python!

Verwenden Sie den Server ```coap.me:5683``` nud path ```test````.

Was bekommen Sie als response?