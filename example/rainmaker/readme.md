# rainmaker控制LED灯熄灭
- ESP RainMaker 是一个轻量级的 IoT 云计算软件，与亚马逊云科技的无服务器架构高度集成。建立在无服务器架构上的 ESP RainMaker 在数据的存储空间和传输能力方面非常灵活，能够随着实际使用的数据流量向云端服务器分配任务，让您可以不再担心云端对于数据存储造成的压力。同时，您也无需担心在没有使用服务器时产生不必要的费用成本，因为在没有数据处理任务时，额外的服务器不会启用。在这种模式下，弹性的数据存储能力和低成本的费用模式可以帮助解决物联网情景下的运营问题。ESP RainMaker 不仅提供云端的数据通讯服务，还提供开源的标准化功能与方案，您可以参考 GitHub 获取开源信息。ESP RainMaker 还提供了一个开放API应用层，支持定制并满足不同环境的不同需求，此外还提供数据看板、第三方集成、远程 OTA 等功能模块。

- 通过github上example感受rainmaker能做什么事情

### arduino IDE

> 一开始无论通过platformio烧录还是arduino IDE烧录都会报flash空间不够,查找资料发现需要使用rainmaker格式引导分区,重新烧录成功!🥰

程序成功烧录之后,rainmaker无法正常运行,报错如下:
> E (9141) esp_rmaker_core: Node connected to Wi-Fi without Assisted claiming. Cannot proceed to MQTT connection.

> E (9142) esp_rmaker_core: Please update your phone apps and repeat Wi-Fi provisioning with BLE transport.

后在[issues中看到了回答](https://github.com/techiesms/ESP-RainMaker-Demo-Code/issues/2),按下主板下的boot键3秒重置wifi设置,rainmaker成功运行!使用移动端app`esp rainmaker`进行蓝牙连接配网,记得要打开地理位置授权,正常连接之后就能通过手机控制led灯了。

### platformio
烧录程序报错WiFiProv的error,platformio烧录需要在ini配置文件中配置项增加`board_build.partitions`,在项目根目录下增加rainmaker的csv文件作为配置项value值