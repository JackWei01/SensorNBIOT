#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

HardwareSerial RS485Serial(2); // RX, TX
WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* ssid = "360wifi"; // 请替换为你的WiFi名
const char* password = "625625625"; // 请替换为你的WiFi密码
const char* mqtt_server = "192.168.0.101"; // MQTT服务器地址
const char* mqtt_topic = "mqtt/yangan"; // MQTT主题

// 485烟感器询问帧
uint8_t queryFrame[] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x01, 0x85, 0xCF};

// 正常响应
// 0x01 
// 0x03 
// 0x02 
// 0x00 
// 0x00 
// 0xB8 
// 0x44 

// 报警响应
// 0x01 
// 0x03 
// 0x02 
// 0x00 
// 0x01 
// 0x79 
// 0x84 
uint8_t correctResponse[] = {0x01, 0x03, 0x02, 0x00, 0x01, 0x79, 0x84};

void setup() {
  Serial.begin(115200);
  RS485Serial.begin(9600, SERIAL_8N1, 16, 17); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  mqttClient.setServer(mqtt_server, 1883);

  while (!mqttClient.connected()) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }

  sendMQTTMessage(false); // 发送初始信息
}

void loop() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  // 每3秒发送一次询问帧
  if (currentMillis - previousMillis >= 3000) {
    RS485Serial.write(queryFrame, sizeof(queryFrame));
    previousMillis = currentMillis;
  }

  // 检查RS485串口是否有数据可读
  if (RS485Serial.available()) {
    size_t expectedResponseSize = 7; // 预期的响应数据长度
    uint8_t receivedBytes[expectedResponseSize];
    size_t index = 0;

    // 读取等待时间，以确保数据完整
    unsigned long startMillis = millis();
    while (millis() - startMillis < 1000 && index < expectedResponseSize) {
      if (RS485Serial.available()) {
        receivedBytes[index++] = RS485Serial.read();
        Serial.print(receivedBytes[index-1], HEX); // 打印读取的字节的十六进制形式
        Serial.print(" "); // 为了可读性在每个字节后面加一个空格
      }
    }
    Serial.println(); // 每次读取后换行

    // 检查是否接收到的数据长度正确
    if (index == expectedResponseSize) {
      // 检查是否收到报警响应
      if (receivedBytes[3] == 0x00 && receivedBytes[4] == 0x01) {
        sendMQTTMessage(true); // 如果是报警状态，立即发送MQTT报警消息
      }
    }
  }

  // 每30秒发送一次MQTT消息
  static unsigned long lastMQTTSend = 0;
  if (currentMillis - lastMQTTSend >= 30000) {
    sendMQTTMessage(false); // 发送非报警状态的MQTT消息
    lastMQTTSend = currentMillis;
  }

  mqttClient.loop(); // 维持MQTT连接
}


void sendMQTTMessage(bool alarm) {
  StaticJsonDocument<200> doc;

  // 填充JSON文档
  doc["time"] = millis();
  doc["device_id"] = "YG625"; // 替换为你的设备ID
  doc["alarm"] = alarm;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // 将JSON文档序列化为字符数组

  mqttClient.publish(mqtt_topic, jsonBuffer); //todo 改为 Qos1 防止消息未发出
}
