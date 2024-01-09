// This example demonstrates the ESP RainMaker with a standard Switch device.
#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include "AppInsights.h"
#define DEFAULT_POWER_MODE false

// BLE名称
const char *service_name = "esp-32";
// BLE密钥
const char *pop = "123";

// GPIO for push button
#if CONFIG_IDF_TARGET_ESP32C3
static int gpio_0 = 9;
static int gpio_switch = 7;
#else
// GPIO for virtual device
// 控制引脚输出
static int gpio_0 = 0;
static int gpio_switch = 13;
#endif

/* Variable for reading pin status*/
// switch 状态
bool switch_state = true;

// The framework provides some standard device types like switch, lightbulb,
// fan, temperaturesensor.
// 为节点创建switch设备(开关)
static Switch *my_switch = NULL;

// WARNING: sysProvEvent is called from a separate FreeRTOS task (thread)!
// 连接配网回调函数
void sysProvEvent(arduino_event_t *sys_event)
{
    switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
#if CONFIG_IDF_TARGET_ESP32S2
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n",
                      service_name, pop);
        printQR(service_name, pop, "softap");
#else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n",
                      service_name, pop);
        // 打印连接二维码
        printQR(service_name, pop, "ble");
#endif
        break;
    case ARDUINO_EVENT_PROV_INIT:
        wifi_prov_mgr_disable_auto_stop(10000);
        break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
        wifi_prov_mgr_stop_provisioning();
        break;
    default:;
    }
}

// switch开关回调函数
void write_callback(Device *device, Param *param, const param_val_t val,
                    void *priv_data, write_ctx_t *ctx)
{
    // 设备名
    const char *device_name = device->getDeviceName();
    // 参数名
    const char *param_name = param->getParamName();

    // 参数名是否等于power
    if (strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n",
                      val.val.b ? "true" : "false", device_name, param_name);
        switch_state = val.val.b;

        // 根据switch状态给引脚写入高低电平
        (switch_state == false) ? digitalWrite(gpio_switch, LOW)
        : digitalWrite(gpio_switch, HIGH);
        // 更新参数值
        param->updateAndReport(val);
    }
}

void setup()
{
    // 设置波特率
    Serial.begin(115200);
    // 获取boot状态
    pinMode(gpio_0, INPUT);
    // 设置控制led引脚状态
    pinMode(gpio_switch, OUTPUT);
    
    digitalWrite(gpio_switch, DEFAULT_POWER_MODE);

    // 添加节点
    Node my_node;
    // 初始化节点
    my_node = RMaker.initNode("ESP RainMaker Node");

    // Initialize switch device
    // 初始化设备
    my_switch = new Switch("LED灯开关", &gpio_switch);
    // 如果初始化失败则终止setup函数
    if (!my_switch) {
        return;
    }
    // Standard switch device
    // switch回调函数写入
    my_switch->addCb(write_callback);

    // Add switch device to the node
    // 节点添加switch
    my_node.addDevice(*my_switch);

    // This is optional

    // 
    RMaker.enableOTA(OTA_USING_TOPICS);
    // If you want to enable scheduling, set time zone for your region using
    // setTimeZone(). The list of available values are provided here
    // https://rainmaker.espressif.com/docs/time-service.html
    //  RMaker.setTimeZone("Asia/Shanghai");
    //  Alternatively, enable the Timezone service and let the phone apps set the
    //  appropriate timezone
    // 设置时区
    RMaker.enableTZService();

    RMaker.enableSchedule();

    RMaker.enableScenes();
    // Enable ESP Insights. Insteads of using the default http transport, this function will
    // reuse the existing MQTT connection of Rainmaker, thereby saving memory space.
    // 启用ESP Insights。此函数将不使用默认的http传输
    //重用Rainmaker现有的MQTT连接，从而节省内存空间。
    initAppInsights();

    // 
    RMaker.enableSystemService(SYSTEM_SERV_FLAGS_ALL, 2, 2, 2);

    // rainmaker启动服务
    RMaker.start();

    // wifi配网
    WiFi.onEvent(sysProvEvent);  // Will call sysProvEvent() from another thread.
#if CONFIG_IDF_TARGET_ESP32S2
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE,
                            WIFI_PROV_SECURITY_1, pop, service_name);
#else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM,
                            WIFI_PROV_SECURITY_1, pop, service_name);
#endif
}

void loop()
{
    // boot按下电平为低 如果被按下
    if (digitalRead(gpio_0) == LOW) {  // Push button pressed

        // Key debounce handling
        // 按钮防抖
        delay(100);
        // 记录按下时间
        int startTime = millis();
        // 持续按下
        while (digitalRead(gpio_0) == LOW) {
            delay(50);
        }
        // 记录松开时间
        int endTime = millis();
        // 按下时间大于10秒回复出场设置
        if ((endTime - startTime) > 10000) {
            // If key pressed for more than 10secs, reset all
            Serial.printf("Reset to factory.\n");
            RMakerFactoryReset(2);
        } 
        // 按下时间大于3秒wifi重置
        else if ((endTime - startTime) > 3000) {
            Serial.printf("Reset Wi-Fi.\n");
            // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
            RMakerWiFiReset(2);
        } else {
            // 打印按钮按下状态 
            // Toggle device state
            switch_state = !switch_state;
            Serial.printf("Toggle State to %s.\n", switch_state ? "true" : "false");
            // 可以通过boot更新switch状态参数
            if (my_switch) {
                my_switch->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,
                                                switch_state);
            }
            // switch_state 写入高低电平
            (switch_state == false) ? digitalWrite(gpio_switch, LOW)
            : digitalWrite(gpio_switch, HIGH);
        }
    }
    delay(100);
}