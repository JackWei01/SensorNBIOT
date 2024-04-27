#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi参数
const char* ssid = "360wifi"; // 替换为你的WiFi名称
const char* password = "625625625"; // 替换为你的WiFi密码

// MQTT服务器参数
const char* mqttServer = "192.168.0.101";
const int mqttPort = 1883; // 你的MQTT端口（通常是1883）

// 初始化WiFi和MQTT客户端
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// MQ2传感器的模拟输入引脚
const int mq2AnalogPin = 34; // 将MQ2的A0端口连接到ESP32的GPIO34

void setup() {
  Serial.begin(115200);
  // 连接WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // 设置MQTT服务器和端口
  mqttClient.setServer(mqttServer, mqttPort);

  // 连接到MQTT服务器
  if (mqttClient.connect("ESP32Client")) {
    Serial.println("Connected to MQTT Server!");
  } else {
    Serial.print("failed with state ");
    Serial.print(mqttClient.state());
    delay(2000);
  }

  // ESP32支持12位模拟读取分辨率
  analogReadResolution(12);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // 读取MQ2传感器数据
  int mq2Value = analogRead(mq2AnalogPin); 
  float voltage = mq2Value / 4095.0 * 3.3; // 计算电压值

  // 生成要发布的消息
  char msg[50];
  snprintf(msg, 50, "MQ2 Voltage: %.3fV", voltage);

  // 每秒发布一次消息
  Serial.print("Publish message: ");
  Serial.println(msg);
  mqttClient.publish("sensor/mq2", msg);

  delay(1000);
}

void reconnect() {
  // 在这里，我们重新连接到服务器
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // 尝试连接
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("connected");
      // 一旦连接上，我们可能需要重新订阅
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // 等待5秒钟后重试
      delay(5000);
    }
  }
}
