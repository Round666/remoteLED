## ESP32+Arduino+Node实现网页端控制LED😛

### 实现效果
![8e49898e8321f70fbe60 -original-original](../pic/Readme/8e49898e8321f70fbe60 -original-original.gif)

### 硬件
🐼 esp32 wroom 32D

⏳ LED模块

🎈 杜邦线

### back
下位机部分使用Arduino框架进行开发,vscode配合platformio进行开发,需要注意platform进行安装时挂载可靠的梯子

### webController
上位机控制部分使用express框架静态挂载返回页面配合tailwindcss进行点击控制svg发起请求转发至express,express发起udp请求,udp请求esp32模块,esp32模块进行高低电平的输出做到对led的开灭动作。