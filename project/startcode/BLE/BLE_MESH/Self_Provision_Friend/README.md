# 概述
这是一个通用朋友例子

## 关于MESH库
在这个例子中，使用BLE_MESH_FRN.LIB作为默认库。
BLE_MESH_FRN提供了基本的朋友功能。

## 关于自配网
选择自配网的原因，是为了快了解其工作机制。
建议在了解其机制后，可结合通用开关模型进行
修改为通用配网模式。

|功能|使能|
|-|-|
|转发|Y|
|朋友|Y|
|低功耗|N|
|代理|N|
|配置通过广播|Y|
|配置通过连接|N|
|配置者|N|
|客户端模型|Y|

提供本例的目的是以最基本的功能出发，快速了解其工作机制。

本例子需要与低功耗例子结合使用，注意节点地址必须唯一（本例中：0x0005）