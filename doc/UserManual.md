# GRL's User Manual #

2017/1/10

----------

## gr_user.h ##

> 用户宏设置头文件

- **GR\_USE_NAMESPACE：**
	可通过宏定义，控制是否使用GRL命名空间，命名空间的名字也可以由用户修改

- **GRMarco：**
	- XTAL：
	通过XTAL设置晶振频率，单位为MHz  
	- SYSTICK\_RELOAD：
	设置滴答时钟的重载值

-----------

## GRGlobal ##

> 引用了官方固件库中提供的STM32F1的寄存器映射及中断向量表，同时也应引用了gr_user.h，载入用户的设置

- **GRCFunc：**
	使用此宏定义修饰的函数，可使编译器采用C方式编译该函数。

- **grRound：**
	对对浮点数进行四舍五入运算。

- **grSrand：**
	设置随机数种子，默认使用滴答时钟的值。

- **grRand：**
	获取随机数，范围是0~RAND\_MAX。

- **BITBAND：**
	将地址和对应位变换为位寻址的地址

- **BIT\_ADDR：**
	将地址值转化为指针，并且返回，主函数中使用解除引用即可实现位寻址
  
	`*BIT_ADDR((vu32)GPIOA->ODR,2)=1 //PA输出寄存器第二位变为1`

- **grSwap：**
	模板函数，将两个数据类型的对象进行交换，注意深度复制问题。

- **grMax：**
	模板函数，比较两个数并获取较大数的引用，相等时返回第一个数。

- **grMin：**
	模板函数，比较两个数并获取较小数的引用，相等时返回第一个数。


----------

## GRCore ##

> 引用了gr_global.h，参考原子程序，封装了众多底层的功能，包括系统时钟控制、延时函数的实现、NVIC相关函数、JTAG功能设置函数、汇编函数的支持等，该类带有纯虚析构函数，不允许生成实例。

- **SystemClockInit：**
	设置主时钟倍频

- **SystemPll：**
	获取倍频值

- **SystemFreq：**
	获取主频

- **RccDeInit：**
	复位所有时钟寄存器

- **SleepUs：**
	微秒级精确延时，10us以上的延时效果良好

- **SleepMs：**
	毫秒级精确延时，误差较小

- **SleepSec：**
	秒级延时，可以传入浮点数，延时很长时间，误差为10ms

- **SleepNop：**
	粗略延时，执行n*SysFreq个空循环，传入100时，延时为700us左右

- **SleepInt：**
	粗略延时，与SleepNop中一样，但请只在中断中使用该函数

### 类中断 ###

> 类中断（Class-based Interrupt Request）是在OOP下实现中断处理的机制，GRL中的外设类带有一个虚的CIRQ()方法，通过类的继承，可以重载这些CIRQ方法，同时通过CIRQ向量注册，外设中断发生时就能跳转到重载的CIRQ方法中，保持了GRL的OOP特性。




- **类中断的实现**
    &emsp;在GRCore的实现文件中，定义了一个名为CIRQ_table的指针数组，该数组为**CIRQ向量表**，该向量表按照一定顺序，记录了需要执行该通道CIRQ的对象指针，称为**CIRQ指针**。例如，CIRQ_table[0]记录了需要执行Usart1通道CIRQ的对象指针，若CIRQ_table[0]的值为某个GRUsart类的对象指针，当Usart1发生了硬件层面的中断时，GRL将会在硬件中断服务函数中根据CIRQ向量表第[0]元素的指示，执行GRUsart类或其派生类所实现的CIRQ函数，由此便建立了硬件中断和CIRQ的对应关系，且由于CIRQ是一个虚函数，故可以通过继承重构该函数，以实现不同的CIRQ服务函数。
	&emsp;数组的大小由CIRQ通道的CIRQChannel类型枚举元素个数决定：

    	enum CIRQChannel
		{
			Usart1,Usart2,Usart3,
			Tim1,Tim2,Tim3,Tim4,
			Tim5,Tim6,Tim7,Tim8,
			…
		CH_CNT};

	CH_CNT为枚举的最后一个元素，它的数值反映出了CIRQ通道的个数，即为CIRQ_table[]的大小。
	

- **类中断的注册**
	&emsp;通过将数组中对应的元素赋值，使CIRQ指针指向对应的外设对象，该过程称为**CIRQ注册**。
	&emsp;外设类的对象被构造时，如果使用CIRQ，将默认将本对象的指针（this）注册为当前对应CIRQ通道的CIRQ指针，当占用同通道的外设对象被多次生成时，该通道的CIRQ指针会发生更新覆盖，以至于发生外设中断时，只跳入最后生成的对象的CIRQ方法中。如果需要做出调整，可自行调用CIRQ注册函数：

		GRCore::CIRQReg(CIRQChannel ch,void* ptr);

		/*Example:*/
		GRCore::CIRQReg(GRCore::Usart1,this);//将本对象注册为Usart1通道CIRQ对象
		

