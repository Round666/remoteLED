#include <Arduino.h>
#include <WiFi.h>  //wifi功能需要的库
 
// 声明UDP对象
WiFiUDP Udp; 

// 存储Udp客户端发过来的数据
char incomingPacket[255];  
// 本地端口号                     
unsigned int localUdpPort = 123; 

void setup() {
  // 开启串口，波特率为115200
  Serial.begin(115200);    
  // 打开ESP32热点             
  WiFi.softAP("ESP32_Udp_server", "");  
  Serial.print("\n开发板IP地址为：");
  // 串口输出模块IP地址
  Serial.println(WiFi.softAPIP()); 
  // 启动UDP监听这个端口 
  Udp.begin(localUdpPort);      
  // 设置13pin为led控制引脚
  pinMode(13,OUTPUT); 
}
 
void loop() {
  /*接收发送过来的Udp数据*/
  // 获取接收的数据的长度
  int Data_length = Udp.parsePacket();
  // 如果有数据那么Data_length不为0，无数据Data_length为0  
  if (Data_length)                      
  {
    // 读取数据，将数据保存在数组incomingPacket中
    int len = Udp.read(incomingPacket, 255);  
    // 为了避免获取的数据后面乱码做的判断
    if (len > 0)                              
    {
      incomingPacket[len] = 0;
    }
    
    // Serial.println(incomingPacket);

    int ledState = digitalRead(13);

    if(String(incomingPacket) == "on"&&!ledState){
        Serial.println("开灯");
        digitalWrite(13,HIGH);
    }else if (String(incomingPacket) == "off"&&ledState)
    {
        /* code */
        Serial.println("关灯");
        digitalWrite(13,LOW);
    }
    

    /*将接受到的数据发送回去*/
    // 准备发送数据到目标IP和目标端口
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); 
    // 将数据receive data:放入发送的缓冲区 
    Udp.print("指令已下达");      
    // 将接收到的数据放入发送的缓冲区                   
    // Udp.println(incomingPacket); 
    // 向目标IP目标端口发送数据                      
    Udp.endPacket();                                    
  }
}