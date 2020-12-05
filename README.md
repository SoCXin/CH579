# [CH579](https://github.com/SoCXin/CH579)

[![sites](http://182.61.61.133/link/resources/SoC.png)](http://www.SoC.Xin)

#### [Vendor](https://github.com/SoCXin/Vendor)：[WCH](http://www.wch.cn/)
#### [Core](https://github.com/SoCXin/Cortex)：[Cortex M0](https://github.com/SoCXin/CM0)
#### [Level](https://github.com/SoCXin/Level)：40MHz

## [CH579描述](https://github.com/SoCXin/CH579/wiki)

[CH579](https://github.com/SoCXin/CH579) 采用40MHz的Cortex M0内核，片上集成低功耗蓝牙BLE(4.2)通讯模块、以太网控制器及收发器、全速USB主机和设备控制器及收发器、段式LCD驱动模块、ADC、触摸按键检测模块、RTC等外设。

32K SRAM，250KB，2KB DataFlash, 4KB BootLoader支持ICP、ISP、IAP、OTA

支持USB2.0全速和低速主机或设备，支持控制/批量/中断同步传输，支持USB type-C主从/电流检测,提供14通道12位ADC模数转换器，支持14通道触摸按键,提供4组26位定时器，支持捕捉/采样，支持12路PWM输出。

多种低功耗模式：Idle，Halt，Sleep，Shutdown，内置电池电压低压监控，最低电流0.2uA，0dBm发送功率时电流6mA

[![sites](docs/CH579.png)](http://www.wch.cn/products/CH579.html)

#### 关键特性

* 双模：BLE兼容4.2规范，Zigbee兼容IEEE 802.15.4规范，单端RF接口无需外部电感
* 支持3.3V和2.5V电源，范围2.1V～3.6V, 内置DC/DC转换
* 提供10M以太网接口，内置PHY
* SPI x 2 + UART x 4 (16C550/6Mbps)
* 内置实时时钟RTC
* 内置温度传感器
* 内置AES-128加解密单元
* 封装(QFN28_4X4/QFN48_5X5)

### [资源收录](https://github.com/SoCXin)

* [参考文档](docs/)
* [参考资源](src/)
* [参考工程](project/)

### [选型建议](https://github.com/SoCXin)

[CH579](https://github.com/SoCXin/CH579)的片上资源相对独特，集成以太网通信和无线通信，若稳定必成爆款

###  [SoC芯平台](http://www.SoC.Xin)
