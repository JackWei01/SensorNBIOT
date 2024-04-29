#include <HardwareSerial.h>

  // 定义ESP32的硬件串口，通常用第二个硬件串口作为RS485的通信口
  HardwareSerial RS485Serial(2); // RX, TX

  // 485烟感器询问帧
  uint8_t queryFrame[] = {0x01, 0x03, 0x00, 0x10, 0x00, 0x01, 0x85, 0xCF};

  void setup() {
    // 初始化串口通信
    Serial.begin(115200); // 用于调试的串口
    RS485Serial.begin(9600, SERIAL_8N1, 16, 17); // ESP32的RX，TX引脚可以根据实际情况更改

    // 发送询问帧到烟感器
    RS485Serial.write(queryFrame, sizeof(queryFrame));
  }



void loop() {
  static unsigned long previousMillis = 0;  // 上一次发送询问帧的时间
  unsigned long currentMillis = millis();   // 当前时间

  // 检查是否已经超过3秒
  if (currentMillis - previousMillis >= 3000) {
    // 发送询问帧到烟感器
    RS485Serial.write(queryFrame, sizeof(queryFrame));

    // 更新上一次发送时间
    previousMillis = currentMillis;
  }

  // 如果串口有数据可读
  if (RS485Serial.available()) {
    // 读取缓冲区的所有数据
    while (RS485Serial.available()) {
      uint8_t byte = RS485Serial.read();
      Serial.print("0x");
      // 以十六进制的形式打印每个字节
      if (byte < 0x10) Serial.print("0"); // 为了美观，如果数字小于0x10，打印前面的0
      Serial.print(byte, HEX);
      Serial.print(" "); // 在字节之间打印空格以分隔
    }
    Serial.println(); // 每读完一帧后换行
  }

  // 不需要添加delay，使用非阻塞延时
}