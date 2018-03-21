# GRL开发日志
----------
这是一个瞎搞的STM32F1固件库

## v0.0
2017/1/15
- GRGlobal： 通用函数，容器（双向链表）
- GRCore：系统内核相关库函数
- GRGpio：GPIO操作，支持整个Port操作
- GRUsart：串口123库函数，串口数据包格式解析工具
- GRTimer：通用定时器操作，PWM发生器库函数
- GRSpi：SPI123库函数
- GRLcd：抽象LCD类

## v0.1
2017/2/9
- 增加模拟IIC
- PID工具包

## v1.0
2017/2/16
- 修复若干BUG，架构正式形成

## v1.1 
2017/2/28
- 主时钟初始化改为自动进行，在user头文件中修改PLL；
- 主时钟初始化后进行IO初始化，故用户需要提供一个IO初始化函数，如果不需要请设置空函数；
- 外设的AFIO初始化默认不自动进行，因为实际使用中发现自动初始化容易导致IO冲突，AFIO的初始化函数应该自行在GRCore::SystemIoInit里提供；
- 综上，新版本初始化顺序为：
**主时钟初始化(GRCore::SystemClockInit)->IO初始化（GRCore::SystemIoInit）->静态变量初始化->main()函数执行。**
即用户需要提供PLL和IO初始化函数

## v1.2
2017/3/2
- 引入FatFs，并对FatFs进行二次封装，提供了少量文件系统接口
- 新增GRSdio接口（by openedv），使用STM32标准SDIO接口驱动SD卡
- 更正AFIO时钟没有初始化的问题

## v1.3
2017/3/24
- 加入GRAdc类，暂时只支持任意单通道单次转换
- 基于ADC1增加了内部温度传感器GRInnerTemperature类
- 基于ADC噪声增加了真随机发生器接口

### v1.3.1
2017-08-14
- 为链表封装了迭代器
- GRUsartPkg类随链表的封装结构改变做出微调
- 文档结构微调

### v1.3.2
2017-09-04
- 增加了GRRtc类，获取实时时钟
- 增加GRDateTime，提供日历运算，暂未支持日期加减运算