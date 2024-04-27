#include <Arduino.h>

const int mq2AnalogPin = 34; // MQ2的A0端口连接到ESP32的GPIO34

void setup() {
  Serial.begin(115200); // 初始化串行通信
  analogReadResolution(12); // ESP32支持默认12位模拟读取分辨率
}

void loop() {
  int mq2Value = analogRead(mq2AnalogPin); // 读取MQ2传感器的模拟值
  
  // 将读取的模拟值转换为对应的电压值(如果ESP32供电为3.3V)
  // ESP32模拟读取为12位分辨率，范围从0到4095
  float voltage = mq2Value / 4095.0 * 3.3;

  Serial.print("MQ2 Analog Value: "); // 在串行监视器中打印读数
  Serial.print(mq2Value);
  Serial.print(" => Voltage: ");
  Serial.println(voltage, 3); // 保留3位小数

  delay(1000); // 等待1秒钟
}
