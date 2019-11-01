# [CH579 软件工程](https://github.com/sochub/CH579)

## 使用说明

IO工程文件中，需要将对应文件夹下的user文件夹下的各个项目main.c文件放置到keil工程文件中，就可以编译相应的例程。

*  IO 驱动工程
    *  ADC：控制AD
    *  FLASH：FLASH读写
    *  SPI0：SPI通信
    *  UART1：串口通信配置
    *  USB：USB通信
    *  PWM：PWM输出      
*  NET 网络工程
    *  DHCP_Client：DHCP功能例程，获取IP地址
    *  DNS：DNS例程，用域名解析出目标IP地址
    *  FTP_Client：FTP客户端例程
    *  FTP_Server：FTP服务器例程
    *  IP_Raw：IP_Raw例程，IP层建立连接、进行数据收发
    *  MQTT
        *  MQTT_SRC：MQTT协议源文件
        *  MQTT_Pub：MQTT-Publish例程
        *  MQTT_Sub：MQTT-Subscibe例程
    *  TCP_Client
    *  TCP_MultipleClients：TCP多客户端例程，建立4个TCP客户端
    *  TCP_SingleClient：TCP单客户端例程，建立1个TCP客户端
        *  TCP_Server: TCP_Server例程，建立TCP服务器
        *  UDP_Client：UDP_Client例程，通过UDP协议发送报文
        *  UDP_Server：UDP_Server例程，接受来自广播IP的指定端口报文，并将接受到报文的地址作为目标地址发送报文
        *  CH57xNET协议栈库说明.pdf：以太网协议栈库说明  
* BLE 无线工程
    * Broadcaster：广播者角色例程，处于广播态一直广播
    * CyclingSensor：骑行传感器例程，连接主机后定时上传速度和踏频
    * GlucoseSensor：葡萄糖传感器例程，连接主机后定时上传葡萄糖浓度
    * HeartRate：心率计例程，连接主机后定时上传心率
    * Peripheral：外设从机角色例程，自定义包含五种不同属性的服务，包含可读、可写、通知、可读可写、安全可读
    * RunningSensor：跑步传感器例程，连接主机后定时上传速度
    * HID_Keyboard：蓝牙键盘例程，模拟键盘设备，连接主机后定时上传键值
    * HID_Mouse：蓝牙鼠标例程，模拟鼠标设备，连接主机后定时上传键值
    * HID_Consumer：蓝牙拍照器例程，模拟用户控制设备，连接主机后定时上传音量键下键
    * HID_Touch：蓝牙触摸例程，模拟触摸笔设备，连接主机后定时上传触摸值
    * Observer：观察者角色例程，定时扫描，如果扫描结果不为空，则打印扫描到的广播地址
    * DirectTest：直接测试例程，测试指定通信频道发送数据包
    * OTA：无线升级例程
    * HAL：例程共用的硬件相关文件
    * LIB：BLE协议栈库文件及其头文件
    * CH57xBLE协议栈库说明.pdf：蓝牙协议栈库说明

###  [SoC资源平台](http://www.qitas.cn)
