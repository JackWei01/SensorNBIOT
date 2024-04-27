#include <WiFi.h>
#include <PubSubClient.h>

// WiFi参数
const char* ssid = "360wifi"; // 替换为你的WiFi名称
const char* password = "625625625"; // 替换为你的WiFi密码

// MQTT服务器参数
const char* mqttServer = "192.168.0.101";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup() {
  Serial.begin(115200);
  
  // 连接WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // 设置MQTT服务器
  client.setServer(mqttServer, mqttPort);

  // 连接MQTT（不使用用户名和密码）
  if (client.connect("ESP32Client")) {
    Serial.println("Connected to MQTT Server!");
  } else {
    Serial.print("failed with state ");
    Serial.print(client.state());
    delay(2000);
  }
}

void loop() {
  if (!client.connected()) {
    Serial.println("MQTT not connected. Trying to reconnect...");
    while (!client.connected()) {
      // 尝试重新连接（不使用用户名和密码）
      if (client.connect("ESP32Client")) {
        Serial.println("Reconnected to MQTT Server!");
        // 重新订阅
      } else {
        Serial.print("failed with state ");
        Serial.print(client.state());
        delay(2000);
      }
    }
  }
  client.loop();

  // 发送消息的间隔时间
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    // 创建消息
    value++;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    
    // 发送消息
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("test/topic", msg);
  }
}
