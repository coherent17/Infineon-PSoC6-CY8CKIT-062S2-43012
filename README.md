# Infineon PSoC 62S2 Wifi BT Pioneer Kit
[TOC]
## Get Started:

https://github.com/infineon/training-modustoolbox-level1-getting-started

download software MTB account

或是直接在ECE22A software下載
!! 要以系統管理員的權限執行，

## modus-shell CLI 使用

```bash=
# compile code with multiple thread
$ make -j build   
```

```bash=
#program on the board and flash to the target device
$ make program -j TARGET=Hello_World 
```

```bash=
# program the code to the default target
$ make qprogram
```

```bash=
# check for the dependency for the code
$ make getlibs
```

```bash=
# to see the hardware config
$ make config
```

```bash=
# to explore the project in vscode
$ make vscode
```

## PSoC 6 Config
![](https://hackmd.io/_uploads/B1L9R5FPn.png)

## training on PSoC
https://github.com/Infineon/training-modustoolbox-level2-psoc

### How-To: Starting with a new application
#### Open App on vscode
*    make vscode / project creator to specify

![](https://hackmd.io/_uploads/SJSDRjtDh.png)

#### Compile on VScode
*    Terminal/Run Build Task/Build
![](https://hackmd.io/_uploads/Sy5Ik2tP3.png)
![](https://hackmd.io/_uploads/BkrDJ2YDh.png)

#### Flash on the board
![](https://hackmd.io/_uploads/HJwA12Fw3.png)

#### Do it on CLI

*    open modus-shell
```bash=
# to see the available board type
$ project-creator-cli --list-boards

# to see the template of our boards
$ project-creator-cli --list-apps CY8CKIT-062S2-43012

# to create a template project using cli
$ project-creator-cli -b CY8CKIT-062S2-43012 -a mtb-example-empty-app -n MyCliApp

# cd into MyCliApp
# to see how to use makefile
$ make help

#build the app
$ make build

#generate vscode-dep files
$ make vscode

# open vscode on CLI
$ code ./mtb-example-empty-app.code-workspace
```

### How-To: Blinking an LED with a GPIO

#### Device Configurator

To see the detail macro name of the pins of GPIO
![](https://hackmd.io/_uploads/SkitShFDn.png)

![](https://hackmd.io/_uploads/ByO2vnYvn.png)

```clike=
//blinkLED
#include "cyhal.h"    //Hardware Abstraction Layer
#include "cy_pdl.h"   //Peripheral Driver Library
#include "cybsp.h"

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    //init the gpio
    cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_ON);


    for (;;)
    {
    	cyhal_gpio_toggle(CYBSP_LED_RGB_BLUE);
    	Cy_GPIO_Inv(CYBSP_LED_RGB_GREEN_PORT, CYBSP_LED_RGB_GREEN_PIN);
    	//cyhal_system_delay_ms(500);
    	Cy_SysLib_Delay(500);
    }
}

```

program usage:
![](https://hackmd.io/_uploads/B1pHng5w3.png)
click MyBlinkLED Program (KitProg3_MiniProg4)

### How-To: Blinking an LED with a PWM

```clike=
//blink using PWM
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    cyhal_pwm_t MyHAL_PWM;
    cyhal_pwm_init_adv(&MyHAL_PWM, CYBSP_USER_LED, NC, CYHAL_PWM_LEFT_ALIGN, true, 0, false, NULL);
    cyhal_pwm_set_duty_cycle(&MyHAL_PWM, 50, 1);
    cyhal_pwm_start(&MyHAL_PWM);


    Cy_TCPWM_PWM_Init(MyPDL_PWM_HW, MyPDL_PWM_NUM, &MyPDL_PWM_config);
    Cy_TCPWM_PWM_Enable(MyPDL_PWM_HW, MyPDL_PWM_NUM);
    Cy_TCPWM_TriggerReloadOrIndex_Single(MyPDL_PWM_HW, MyPDL_PWM_NUM);

    for (;;)
    {
    	cyhal_syspm_sleep();
    }
}

```

### How-To: Using a GPIO interrupt
按板子上的按鈕讓LED亮
#### Device Configuration:
![](https://hackmd.io/_uploads/S1cmJWjDn.png)

#### Using HAL API
```clike=
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"


//global variable as the call back function flag
bool g_event_occurred = false;

//the call back function definition
void MyCallbackFunction(void *callback_arg, cyhal_gpio_event_t event){
	g_event_occurred = true;
}

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    //init the button and the led
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    //the data structure for the call back function
    cyhal_gpio_callback_data_t my_callback_data = {.callback = MyCallbackFunction};

    //set the call back function onto the specific pin (button here)
    cyhal_gpio_register_callback(CYBSP_USER_BTN, &my_callback_data);

    //when event is trigger, then execute the call back function
    //the signal is pulled high, when the user push the button, the signal will be low -> falling
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, CYHAL_ISR_PRIORITY_DEFAULT, true);


    for (;;)
    {
    	if(g_event_occurred){
    		cyhal_gpio_toggle(CYBSP_USER_LED);
    		g_event_occurred = false;
    	}
    	//place the device in low power mode, and wake up a system event if interrupted
    	cyhal_syspm_deepsleep();
    }
}

```

#### Using PDL API system level interrupt
```clike=
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

bool g_event_occurred = false;

static void MyGPIO_ISR(void){
	Cy_GPIO_ClearInterrupt(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_NUM);
	g_event_occurred = true;
}

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    cy_stc_sysint_t irqcfg = {CYBSP_USER_BTN_IRQ, 7};
    cy_SysInt_Init(&irqcfg, MyGPIO_ISR);
    INVIC_EnableIRQ(CYBSP_USER_IRQ);
     
    for (;;)
    {
    	if(g_event_occurred){
    		Cy_GPIO_Inv(CYBSP_USER_LED_PORT, CYBSP_USER_LED_NUM);
    		g_event_occurred = false;
    	}
    	Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
}

```

### UART
用library manager去加上retarget_io
#### 印出LED state
```clike=
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"


int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    /*BSP init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initialize retarget-io to use the debug UART port */
	result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
	/* retarget-io init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

    __enable_irq();

    for (;;)
    {
    	cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
    	printf("LED ON\n\r");
    	cyhal_system_delay_ms(250);
    	cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
    	printf("LED OFF\n\r");
    	cyhal_system_delay_ms(250);
    }
}

```

![](https://hackmd.io/_uploads/S1vjcGoP3.png)
```clike=
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

// UART object and configuration structure
cyhal_uart_t uart_obj;
const cyhal_uart_cfg_t uart_config =
{
	.data_bits = 8,
	.stop_bits = 1,
	.parity = CYHAL_UART_PARITY_NONE,
	.rx_buffer = NULL,
	.rx_buffer_size = 0
};

int main(void)
{
    cy_rslt_t result;
    uint8_t read_data;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    /*BSP init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initialize UART */
	result = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
	/* UART init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	// enable interrupts
    __enable_irq();

    for (;;)
     {
    	cyhal_uart_getc(&uart_obj, &read_data, 0);
		if(read_data == '0')
		{
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
		}
		else if(read_data == '1')
		{
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
		}
     }
 }

```

![](https://hackmd.io/_uploads/rkb9jQsw2.png)
![](https://hackmd.io/_uploads/rku5smsP2.png)
```clike=
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#define GPIO_INTERRUPT_PRIORITY (7u)

volatile bool pressFlag = false;

// UART object and configuration structure
cyhal_uart_t uart_obj;
const cyhal_uart_cfg_t uart_config =
{
	.data_bits = 8,
	.stop_bits = 1,
	.parity = CYHAL_UART_PARITY_NONE,
	.rx_buffer = NULL,
	.rx_buffer_size = 0
};

//Interrupt handler
static void button_isr(void *handler_arg, cyhal_gpio_event_t event)
{
	//set press flag and toggle led
	cyhal_gpio_toggle(CYBSP_USER_LED);
	pressFlag = true;
}

// GPIO callback initialization structure
cyhal_gpio_callback_data_t cb_data =
{
	.callback     = button_isr,
	.callback_arg = NULL
};

int main(void)
{
    cy_rslt_t result;
	uint8_t numberPresses = 0;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    /*BSP init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initialize the user button */
	result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);
	/* GPIO init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initialize UART */
	result = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
	/* UART init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Configure GPIO interrupt */
	cyhal_gpio_register_callback(CYBSP_USER_BTN, &cb_data);
	cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);

	// enable interrupts
    __enable_irq();

    for (;;)
    {
    	if(pressFlag)
		{
    		numberPresses++;
    		if(numberPresses > 9)
			{
    			numberPresses = 0;
    		}

    		//create char from int to send over UART
    		char printChar = numberPresses + '0';
    		cyhal_uart_putc(&uart_obj, printChar);
    		cyhal_uart_putc(&uart_obj, '\n');
    		cyhal_uart_putc(&uart_obj, '\r');
    		pressFlag = false;
    	}
    }
}
/* [] END OF FILE */

```

### Some OS Knowledge (Demo in pthread)
myturn每一秒執行一次，yourturn每2秒執行一次
```clike=
#include <stdio.h>
#include <unistd.h>

void myturn(void){
	for(int i = 0; i < 8; i++){
		sleep(1);
		printf("My Turn! %d\n", i);
	}
}

void yourturn(){
	for(int i = 0; i < 3; i++){
		sleep(2);
		printf("Your Turn! %d\n", i);
	}
}


int main(){
	myturn();
	yourturn();
	return 0;
}
```
結果:
![](https://hackmd.io/_uploads/HybWzX3Ph.png)
一個做完才繼續做下一個，但如果想要讓他並行concurrency，使用pthread，傳入function pointer。
```clike=
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *myturn(void *arg){
	for(int i = 0; i < 8; i++){
		sleep(1);
		printf("My Turn! %d\n", i);
	}
	return NULL;
}

void yourturn(){
	for(int i = 0; i < 3; i++){
		sleep(2);
		printf("Your Turn! %d\n", i);
	}
}


int main(){
	pthread_t newthread;

	pthread_create(&newthread, NULL, myturn, NULL);
	yourturn();

	//wait until the thread is done before we exit.
	pthread_join(newthread, NULL);
	return 0;
}
```

先安裝需要的套件
```bash=
sudo apt-get install glibc-doc
sudo apt-get install manpages-posix manpages-posix-dev
```

compile of the code:
要加上-lpthread link到pthread的library
```makefile=
CC = gcc
CFLAGS = -g -Wall
BIN = pthread

all: $(BIN)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ -lpthread

clean:
	rm -rf $(BIN)
```

在這邊，使用pthread_join傳入function pointer使其多加入一個thread，但是這邊myturn會執行得比在main中的yourturn久，假如沒有加上pthread_join，程式會在yourturn做完後便停止，不會繼續做完thread裡面的內容，因此加上pthread_join的其中一個功能是讓他可以等到thread裡面的job做完才結束程式

結果:
![](https://hackmd.io/_uploads/rkuWQX2Pn.png)
可以看到的是他們開始輪流出現，並且等到pthread裡面的myturn全部做完程式才結束。

#### 傳遞參數進入thread
```clike=
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *myturn(void *arg){
	//casting the passing argument from pthread_create
	int *vptr = (int *)arg;

	for(int i = 0; i < 8; i++){
		sleep(1);
		printf("My Turn! %d, v = %d\n", i, *vptr);
		(*vptr)++;
	}
	return NULL;
}

void yourturn(){
	for(int i = 0; i < 3; i++){
		sleep(2);
		printf("Your Turn! %d\n", i);
	}
}


int main(){
	pthread_t newthread;
	int v = 5;

	//passing the int v to the function by pointer
	pthread_create(&newthread, NULL, myturn, &v);
	yourturn();

	//wait until the thread is done before we exit.
	pthread_join(newthread, NULL);
	printf("thread is finished in v = %d\n", v);
	return 0;
}
```
透過pthread_create第四個arg將int的位址傳入thread，而後將void pointer type casting 到正確的type便可以正常使用

結果:
![](https://hackmd.io/_uploads/SkfSNQnD2.png)

#### 回傳參數from thread
```clike=
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *myturn(void *arg){
	int *ret = (int *)malloc(sizeof(int));
	*ret = 5;

	for(int i = 0; i < 8; i++){
		sleep(1);
		printf("My Turn! %d, result = %d\n", i, *ret);
		(*ret)++;
	}
	return ret;
}

void yourturn(){
	for(int i = 0; i < 3; i++){
		sleep(2);
		printf("Your Turn! %d\n", i);
	}
}


int main(){
	pthread_t newthread;
	int *result;

	//thread allocate memory and return to main
	pthread_create(&newthread, NULL, myturn, NULL);
	yourturn();

	//what thread join does?
	//1. wait until the thread is done before exit/
	//2. get the return value from thread
	pthread_join(newthread, (void *)&result);
	printf("thread is finished in result = %d\n", *result);
	return 0;
}
```
thread內部dynamic allocate memory然後傳出來給main，在pthread_join時將要存的變數放在第二個arg傳入即可，因此pthread_join有兩個重要的特性:
*    1. wait until the thread is done before exit
*    get the return value from thread
結果:
![](https://hackmd.io/_uploads/Hy5I872Ph.png)

#### Thread Safety Issue
基本上就是從0算到1000000000 兩遍。
```clike=
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

//bug number in unsigned long
#define BIG 1000000000UL
uint32_t counter = 0;

void count_to_big(){
	for(uint32_t i = 0; i < BIG; i++){
		counter++;
	}
}

int main(){
	count_to_big();
	count_to_big();
	printf("Done. Counter = %u\n", counter);
	return 0;
}
```
使用timer後的結果:
![](https://hackmd.io/_uploads/rkJt5X2Dh.png)
可以看到確實算了兩次，counter此時為2000000000，而所花的時間為6秒53。

那現在把一次分給其他thread算:
```clike=
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

//bug number in unsigned long
#define BIG 1000000000UL
uint32_t counter = 0;

void *count_to_big(void *arg){
	for(uint32_t i = 0; i < BIG; i++){
		counter++;
	}
	return NULL;
}

int main(){
	pthread_t t;
	pthread_create(&t, NULL, count_to_big, NULL);
	count_to_big(NULL);
	pthread_join(t, NULL);
	printf("Done. Counter = %u\n", counter);
	return 0;
}
```
結果:
![](https://hackmd.io/_uploads/BkQ7V43w2.png)
速度更慢，且算的結果也不對，原因來自race condition，兩個thread同時想要去更動到global variable counter。因此這個code 不能夠安全的執行平行化。(not thread safe)。

#### Mutex Lock
為了解決前面所提到的race conditon 的問題，可以使用mutex lock，將某個thread排除其他的thread，讓其他thread都必須要等他做完才可以繼續，強迫其他人停下來等他做完。

```clike=
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

//bug number in unsigned long
#define BIG 1000000000UL
uint32_t counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *count_to_big(void *arg){
	for(uint32_t i = 0; i < BIG; i++){
		//make things atomic
		pthread_mutex_lock(&lock);		//other thread will wait here until it can get the lock
		counter++;
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

//using mutex to solve the race condition issue
int main(){
	pthread_t t;
	pthread_create(&t, NULL, count_to_big, NULL);
	count_to_big(NULL);
	pthread_join(t, NULL);
	printf("Done. Counter = %u\n", counter);
	return 0;
}
```
結果:
![](https://hackmd.io/_uploads/Hk_suEhvh.png)
使用mutex解決race condition之後，現在算的結果對了，但是竟然跑了3分多鐘== 在這個例子中，因為兩個thread share memory，因此會產生很多的overhead，造成運算時間增加許多在不必要的mutex lock and unlock。

*    Parallelism:
    在miutl-core/co-core的system中，兩個thread是可以同時進行的
*    Concurrency:
    在假如只有一CPU時，如果使用者需要很多個thread，那系統實際上會是，跑thread1一下，在切換到thread2一下，在切換回來。只要換得夠快，使用者就不會察覺到。但是會跑很慢，因為這樣一直切換。
    
*    Issue in Parallelism:Memory sharing:
在這個例子中，global variable counter是在兩個thread中共享的memory，在兩個thread中都access了100000000次，因此machine會試著讓memory coherent，因此share memory會讓程式大幅減少可平行化的可能性。

#### Volatile Keyword
foo function等一秒後會將done global variable設為true，此時程式便會印出Keep Going然後結束。
```clike=
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

bool done = false;

void *foo(void *arg){
    sleep(1);
    done = true;
    return NULL;
}

int main(int argc, char **argv){
    
    pthread_t p;
    pthread_create(&p, NULL, foo, NULL);
    
    printf("Waiting ...\n");
    while(!done){};
    printf("Keep Going.\n");

    return 0;
}
```

```bash=
$    gcc -g -Wall main.c -o main -lpthread
```
結果:
![](https://hackmd.io/_uploads/r1aKbH3P3.png)
與我們預期的結果一樣，但是倘若在compile時加上compiler optimize:

```bash=
$    gcc -g -Wall -O2 main.c -o main -lpthread
```

結果:
![](https://hackmd.io/_uploads/ryseMHhvh.png)
會一直卡在Waiting這邊，因為compiler看到
```clike=
while(!done){};
```
會認為while loop內部沒有更動到done的機會，但事實上是另一個thread在控制done這個變數，因此compiler的heuristic會將while(!done){}改為while(1){}使其一一輩子結束不了。

為了保證不同compiler的heuristic都要正確地執行我們的code，不能存著僥倖的心態。解決辦法是加上volatile的keyword

```clike=
volatile bool done = false;
```

如此一來，便可以告訴compiler done這個變數可能是會改變的，儘管不是很明顯。可能改變是在其他thread上或是signal handler。

#### Semaphore
https://www.geeksforgeeks.org/semaphores-in-process-synchronization/
semaphore代表的是一個unsigned int(一般會被初始化為1)，也就是一個普通的大於0的變數。有兩種operation可以去改變他的值。


*    Move(P): move會減少semaphore的數值。
    
*    Signal(V): signal會增加semaphore的數值

當semaphore的數值為0的時候，任何執行move的thread將會需要等待其他的thread執行signal的動作，藉此來將atomic的critical section保持不會被其他thread影響。

寫成pseudo code如下:
```clike=
void P(Semaphore s){
    while(s == 0){};
    s = s - 1;
}

void V(Semaphore s){
    s = s + 1;
}
```
所以一個process的執行應該如下:
```clike=
//perform a process
Semaphore s;
P(s)
//the critical section
V(s);
```
基本上可以將semaphore視為是一個flag告訴其他的process/thread，目前是否能夠去執行critical section的code，可能是share memory的區域。

![](https://hackmd.io/_uploads/rJjby0pPn.png)

看一下這個semaphore的範例
```clike=
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t s;

void *foo(void *arg){

    //wait (P)
    sem_wait(&s);
    printf("Enter critical section\n");

    //critical section
    sleep(4);

    //signal (V)
    printf("Ready to Exiting\n");
    sem_post(&s);
    return NULL;
}

int main(){
    
    //init the semaphore
    sem_init(&s, 0, 1);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, foo, NULL);
    sleep(2);
    pthread_create(&t2, NULL, foo, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);;
    sem_destroy(&s);
    return 0;
}
```
結果為:
![](https://hackmd.io/_uploads/SyavMC6P3.png)
解釋:
創建了兩個thread，t2在t1後兩秒後創建，但是因為此時t1正在foo中的critical section，正在睡4秒，因此t2要等t1睡完4秒後才能進行t2的foo。

### RTOS

#### Semaphore
![](https://hackmd.io/_uploads/Hkhww06vn.png)

*    Use the Library Manager to add the FreeRTOS library to the application.
![](https://hackmd.io/_uploads/BJp33RpP3.png)


*    Edit the Makefile to add FREERTOS and RTOS_AWARE to the COMPONENTS variable.
![](https://hackmd.io/_uploads/B1jxY0awn.png)

*    Copy FreeRTOSConfig.h from the FreeRTOS library to the root directory of the application and edit it to remove the compiler warning.

```clike=
//FreeRTOSConfig.h
/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (C) 2019-2020 Cypress Semiconductor Corporation, or a subsidiary of
 * Cypress Semiconductor Corporation.  All Rights Reserved.
 *
 * Updated configuration to support PSoC 6 MCU.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 * http://www.cypress.com
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#include "cy_utils.h"
#include "cy_syslib.h"

/* Get the low power configuration parameters from
 * the ModusToolbox Device Configurator GeneratedSource:
 * CY_CFG_PWR_SYS_IDLE_MODE     - System Idle Power Mode
 * CY_CFG_PWR_DEEPSLEEP_LATENCY - Deep Sleep Latency (ms)
 */
#include "cycfg_system.h"

#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configCPU_CLOCK_HZ                      SystemCoreClock
#define configTICK_RATE_HZ                      1000u
#define configMAX_PRIORITIES                    7
#define configMINIMAL_STACK_SIZE                128
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configQUEUE_REGISTRY_SIZE               10
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIME_SLICING                  0
#define configENABLE_BACKWARD_COMPATIBILITY     0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   10240
#define configAPPLICATION_ALLOCATED_HEAP        0

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            1
#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         1

/* Software timer related definitions. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE

/*
Interrupt nesting behavior configuration.
This is explained here: http://www.freertos.org/a00110.html

Priorities are controlled by two macros:
- configKERNEL_INTERRUPT_PRIORITY determines the priority of the RTOS daemon task
- configMAX_API_CALL_INTERRUPT_PRIORITY dictates the priority of ISRs that make API calls

Notes:
1. Interrupts that do not call API functions should be >= configKERNEL_INTERRUPT_PRIORITY
   and will nest.
2. Interrupts that call API functions must have priority between KERNEL_INTERRUPT_PRIORITY
   and MAX_API_CALL_INTERRUPT_PRIORITY (inclusive).
3. Interrupts running above MAX_API_CALL_INTERRUPT_PRIORITY are never delayed by the OS.
*/
/*
PSoC 6 __NVIC_PRIO_BITS = 3

0 (high)
1           MAX_API_CALL_INTERRUPT_PRIORITY 001xxxxx (0x3F)
2
3
4
5
6
7 (low)     KERNEL_INTERRUPT_PRIORITY       111xxxxx (0xFF)

!!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html

*/

/* Put KERNEL_INTERRUPT_PRIORITY in top __NVIC_PRIO_BITS bits of CM4 register */
#define configKERNEL_INTERRUPT_PRIORITY         0xFF
/*
Put MAX_SYSCALL_INTERRUPT_PRIORITY in top __NVIC_PRIO_BITS bits of CM4 register
NOTE For IAR compiler make sure that changes of this macro is reflected in
file portable\TOOLCHAIN_IAR\COMPONENT_CM4\portasm.s in PendSV_Handler: routine
*/
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    0x3F
/* configMAX_API_CALL_INTERRUPT_PRIORITY is a new name for configMAX_SYSCALL_INTERRUPT_PRIORITY
 that is used by newer ports only. The two are equivalent. */
#define configMAX_API_CALL_INTERRUPT_PRIORITY   configMAX_SYSCALL_INTERRUPT_PRIORITY


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xTaskAbortDelay                 0
#define INCLUDE_xTaskGetHandle                  0
#define INCLUDE_xTaskResumeFromISR              1

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#if defined(NDEBUG)
#define configASSERT( x ) CY_UNUSED_PARAMETER( x )
#else
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); CY_HALT(); }
#endif

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names - or at least those used in the unmodified vector table. */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* Dynamic Memory Allocation Schemes */
#define HEAP_ALLOCATION_TYPE1                   (1)     /* heap_1.c*/
#define HEAP_ALLOCATION_TYPE2                   (2)     /* heap_2.c*/
#define HEAP_ALLOCATION_TYPE3                   (3)     /* heap_3.c*/
#define HEAP_ALLOCATION_TYPE4                   (4)     /* heap_4.c*/
#define HEAP_ALLOCATION_TYPE5                   (5)     /* heap_5.c*/
#define NO_HEAP_ALLOCATION                      (0)

#define configHEAP_ALLOCATION_SCHEME            (HEAP_ALLOCATION_TYPE3)

/* Check if the ModusToolbox Device Configurator Power personality parameter
 * "System Idle Power Mode" is set to either "CPU Sleep" or "System Deep Sleep".
 */
#if defined(CY_CFG_PWR_SYS_IDLE_MODE) && \
    ((CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_SLEEP) || \
     (CY_CFG_PWR_SYS_IDLE_MODE == CY_CFG_PWR_MODE_DEEPSLEEP))

/* Enable low power tickless functionality. The RTOS abstraction library
 * provides the compatible implementation of the vApplicationSleep hook:
 * https://github.com/cypresssemiconductorco/abstraction-rtos#freertos
 * The Low Power Assistant library provides additional portable configuration layer
 * for low-power features supported by the PSoC 6 devices:
 * https://github.com/cypresssemiconductorco/lpa
 */
extern void vApplicationSleep( uint32_t xExpectedIdleTime );
#define portSUPPRESS_TICKS_AND_SLEEP( xIdleTime ) vApplicationSleep( xIdleTime )
#define configUSE_TICKLESS_IDLE                 2

#else
#define configUSE_TICKLESS_IDLE                 0
#endif

/* Deep Sleep Latency Configuration */
#if( CY_CFG_PWR_DEEPSLEEP_LATENCY > 0 )
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   CY_CFG_PWR_DEEPSLEEP_LATENCY
#endif

/* Allocate newlib reeentrancy structures for each RTOS task.
 * The system behavior is toolchain-specific.
 *
 * GCC toolchain: the application must provide the implementation for the required
 * newlib hook functions: __malloc_lock, __malloc_unlock, __env_lock, __env_unlock.
 * FreeRTOS-compatible implementation is provided by the clib-support library:
 * https://github.com/cypresssemiconductorco/clib-support
 *
 * ARM/IAR toolchains: the application must provide the reent.h header to adapt
 * FreeRTOS's configUSE_NEWLIB_REENTRANT to work with the toolchain-specific C library.
 * The compatible implementations are also provided by the clib-support library.
 */
#define configUSE_NEWLIB_REENTRANT              1

#endif /* FREERTOS_CONFIG_H */

```

```clike=
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

volatile int uxTopUsedPriority;

static SemaphoreHandle_t ButtonSemaphore;

/* Button ISR */
void button_interrupt_handler(void* handler_arg, cyhal_gpio_event_t event)
{
    static BaseType_t xHigherPriorityTaskWoken;

    /* Give the semaphore */
    xSemaphoreGiveFromISR(ButtonSemaphore, &xHigherPriorityTaskWoken);

    /* Yield current task if a higher priority task is now unblocked */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* Structure for GPIO interrupt */
cyhal_gpio_callback_data_t button_interrupt_data =
{
    .callback     = button_interrupt_handler,
    .callback_arg = NULL
};

/* Task to handle the LED */
void led_task()
{
    while (1)
    {
        /* Wait for button press */
    	xSemaphoreTake(ButtonSemaphore, portMAX_DELAY);

    	/* Toggle LED */
        cyhal_gpio_toggle(CYBSP_USER_LED);
    }
}

int main(void)
{
    cy_rslt_t result;

    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                             CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* Initialize button with an interrupt of priority 3 */
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, 1);
	cyhal_gpio_register_callback(CYBSP_USER_BTN, &button_interrupt_data);
	cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, 3, true);

	/* Create Semaphore and take it so that it doesn't trigger an initial toggle of the LED */
     vSemaphoreCreateBinary(ButtonSemaphore);
     xSemaphoreTake(ButtonSemaphore, 0);

    /* Create task with a stack of 1024, no parameters, and a priority of 1 */
    xTaskCreate(led_task, (char *)"led_task", 1024, 0, 1, NULL);

    /* Start the RTOS scheduler */
    vTaskStartScheduler();
    CY_ASSERT(0);
}

```

#### Mutex
![](https://hackmd.io/_uploads/HkZEj0TP2.png)
一樣先加入freertos及header，然後更新makefile的COMPONENT

```clike=
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

volatile int uxTopUsedPriority;

//TODO create variable for mutex
static SemaphoreHandle_t LED_lock;

/* LED task 1 - blink at 2 Hz */
void led_task1()
{
    while (1)
    {
        //TODO lock the mutex
    	xSemaphoreTake(LED_lock, portMAX_DELAY);

        cyhal_gpio_toggle(CYBSP_USER_LED);

        //TODO unlock the mutex
    	xSemaphoreGive(LED_lock);

        vTaskDelay (1000/2); /* 2 Hz delay and give the other task a turn */
    }
}

/* LED task 2 - blink at 5 Hz when button is pressed */
void led_task2()
{
    while (1)
    {

        //TODO lock the mutex
    	xSemaphoreTake(LED_lock, portMAX_DELAY);

        while (CYBSP_BTN_PRESSED == cyhal_gpio_read(CYBSP_USER_BTN)) /* Button is pressed */
        {
        	cyhal_gpio_toggle(CYBSP_USER_LED);
        	vTaskDelay (1000/5);
        }

        //TODO unlock the mutex
        xSemaphoreGive(LED_lock);

        vTaskDelay (1);	/* Give the other task a turn when button is not pressed */
    }
}

int main(void)
{
    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /* Initialize the device and board peripherals */
	cybsp_init() ;
    __enable_irq();

    /* Initialize the User LED */
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* Initialize User Button */
    cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, 1);

	//TODO Create Mutex
    LED_lock = xSemaphoreCreateMutex();

    /* Create tasks with a stack of 1024, no parameters, and a priority of 1 */
    xTaskCreate(led_task1, (char *)"led_task1", 1024, 0, 1, NULL);
    xTaskCreate(led_task2, (char *)"led_task2", 1024, 0, 1, NULL);

    /* Start the RTOS scheduler */
    vTaskStartScheduler();
    CY_ASSERT(0);
}

```

## PSoC training - wifi application
https://github.com/Infineon/training-modustoolbox-level3-wifi/tree/master

![](https://hackmd.io/_uploads/BJqeZQ0vh.png)
*    Open the Library manager and add the wifi-core-freertos-lwip-mbed and retarget-io libraries

![](https://hackmd.io/_uploads/r1WpWXCP2.png)
![](https://hackmd.io/_uploads/SkQ1zm0P2.png)

![](https://hackmd.io/_uploads/HkA_pXCw3.png)

```clike=
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
/* FreeRTOS header files */
#include "FreeRTOS.h"
#include "task.h"
/* Configuration file for Wi-Fi */
#include "wifi_config.h"
/* Middleware libraries */
#include "cy_retarget_io.h"
#include "cy_wcm.h"

void wifi_connect(void *arg)
{
    cy_rslt_t result;
    cy_wcm_connect_params_t connect_param;
    cy_wcm_ip_address_t ip_address;
    uint32_t retry_count;

    /* Configure the interface as a Wi-Fi STA (i.e. Client). */
    cy_wcm_config_t config = {.interface = CY_WCM_INTERFACE_TYPE_STA};

    /* Initialize the Wi-Fi Connection Manager and return if the operation fails. */
    result = cy_wcm_init(&config);

    printf("\nWi-Fi Connection Manager initialized.\n\r");

    /* Configure the connection parameters for the Wi-Fi interface. */
    memset(&connect_param, 0, sizeof(cy_wcm_connect_params_t));
    memcpy(connect_param.ap_credentials.SSID, WIFI_SSID, sizeof(WIFI_SSID));
    memcpy(connect_param.ap_credentials.password, WIFI_PASSWORD, sizeof(WIFI_PASSWORD));
    connect_param.ap_credentials.security = WIFI_SECURITY;

    /* Connect to the Wi-Fi AP. */
    for (retry_count = 0; retry_count < MAX_WIFI_CONN_RETRIES; retry_count++)
    {
        printf("Connecting to Wi-Fi AP '%s'\n\r", connect_param.ap_credentials.SSID);
        result = cy_wcm_connect_ap(&connect_param, &ip_address);

        if (result == CY_RSLT_SUCCESS)
        {
            printf("Successfully connected to Wi-Fi network '%s'.\n\r",
                    connect_param.ap_credentials.SSID);
            break;
        }
    }

    for(;;)
    {
    	if(result == CY_RSLT_SUCCESS)
    	{
    		/* Turn on LED and exit */
    		cyhal_gpio_write(CYBSP_USER_LED,CYBSP_LED_STATE_ON);
    		vTaskDelete(NULL);
    	}
    	else
    	{
    		cyhal_gpio_toggle(CYBSP_USER_LED);
    		vTaskDelay(100);
    	}
    }
}

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize retarget-io to use the debug UART port. */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    /* Initaize LED pin */
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    __enable_irq();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen. */
	printf("\x1b[2J\x1b[;H");
	printf("============================================================\n\r");
	printf("ModusToolbox-Level3-WiFi - 2: Attach\n\r");
	printf("============================================================\n\n\r");

    /* Create the MQTT Client task. */
    xTaskCreate(wifi_connect, "wifi_connect_task", 1024, NULL, 5, NULL);

    /* Never Returns */
    vTaskStartScheduler();
}

```

### Socket

#### Server API
![](https://hackmd.io/_uploads/Byxpfqlu3.png)

#### Client API
![](https://hackmd.io/_uploads/B1Xy79l_n.png)

## PSoC training - Machine Learning

### Cyberon
https://github.com/Infineon/training-modustoolbox-level3-machine-learning

#### CM0與CM4的差別
![](https://hackmd.io/_uploads/r1I-7Oruh.png)

CM0的application就是在CM0的CPU上跑語音辨識，而在CM4的application上面留給user自己玩。

隨便燒一個image去板子開uart會看到板子的unique id，需要根據這個id去註冊liscense才可以使用這個software。

![](https://hackmd.io/_uploads/SkAjrOSdn.png)
到這個網站去拿到License:
https://license.cyberon.tw/InfineonDSpotterLicense/InfineonDSpotterLicense.php

![](https://hackmd.io/_uploads/By-NUuB_n.png)
就可以收到license寄到信箱。
![](https://hackmd.io/_uploads/SkbU8_H_n.png)

這個license是device dependent的，不同device間不能共用。

下載license之後改名為:CybLicense.bin放到data的資料夾底下。

==必須要先clean project再rebuild，不然license會吃不到==

#### Custom model
下載Cyberon software 在雲端了
帳號:infineon_trial@cyberon.com.tw
密碼:留白

存完之後，Trigger_and_command_pack_withTxt.bin in data directory with your custom model bin file

在cyberon main.c中需要注意的為這邊:
```clike=
while(1)
{
    if(pdm_pcm_flag)
    {
        pdm_pcm_flag = 0;
        cyberon_asr_process(pdm_pcm_buffer, FRAME_SIZE);
    }
}
```

而其中重要的部分為cyberon_asr_process這個function，要在這邊設定timeout及偵測到關鍵字後要做的事情。
```clike=
void cyberon_asr_process(short *lpsSample, int nNumSample)
{
	static INT nCount = 0;
	INT nErr;
	char pchBuf[16];
	char pchCommand[64];
	INT nCommandID;
	INT nMapID;
	INT nConfidenceScore;
	INT nVolumeEnergy;

	if(!g_hDSpotter)
		return;

	if(bEnableTimeout && (nCount += nNumSample) > TIMEOUT)
	{
		g_lpfnCallback(__func__, "##########Timeout##########", itoa(TIMEOUT, pchBuf, 10));
		cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
		bEnableTimeout = !bEnableTimeout;
		g_hDSpotter = DSpotter_Init_Multi((BYTE *)CybModelGetBase(g_hCybModel), (BYTE **)&g_lppbyGroup[0], 1, MAX_TIME, g_lpbyMemPool, g_nMemPool, NULL, 0, &nErr, (BYTE *)&__start_license_data);
#if ENABLE_AGC
		DSpotterAGC_Enable(g_hDSpotter);
#endif
	}

	if((nErr = DSpotter_AddSample(g_hDSpotter, lpsSample, nNumSample)) == DSPOTTER_SUCCESS)
	{
		nCommandID = DSpotter_GetResult(g_hDSpotter);
		DSpotter_GetResultScore(g_hDSpotter, &nConfidenceScore, NULL, NULL);
		nVolumeEnergy = DSpotter_GetCmdEnergy(g_hDSpotter);

		if(!bEnableTimeout)
		{
			CybModelGetCommandInfo(g_hCybModel, 0, nCommandID, pchCommand, 64, &nMapID, NULL);
			g_hDSpotter = DSpotter_Init_Multi((BYTE *)CybModelGetBase(g_hCybModel), (BYTE **)&g_lppbyGroup[1], 1, MAX_TIME, g_lpbyMemPool, g_nMemPool, NULL, 0, &nErr, (BYTE *)&__start_license_data);
#if ENABLE_AGC
			DSpotterAGC_Enable(g_hDSpotter);
#endif
			nCount = 0;
			bEnableTimeout = !bEnableTimeout;
		}
		else
		{
			CybModelGetCommandInfo(g_hCybModel, 1, nCommandID, pchCommand, 64, &nMapID, NULL);
			DSpotter_Continue(g_hDSpotter);
			nCount = 0;
		}

#if NOT_SHOW_MULTI_PRONUNCIATION
		if(strstr(pchCommand, " ^"))
			strstr(pchCommand, " ^")[0] = '\0';
#endif
		g_lpfnCallback(__func__, "**********Result**********", "++");
		g_lpfnCallback(__func__, "Command", pchCommand);
		g_lpfnCallback(__func__, "Command ID", itoa(nCommandID, pchBuf, 10));
		g_lpfnCallback(__func__, "Map ID", itoa(nMapID, pchBuf, 10));
		g_lpfnCallback(__func__, "Confidence Score", itoa(nConfidenceScore, pchBuf, 10));
		g_lpfnCallback(__func__, "Volume Energy", itoa(nVolumeEnergy, pchBuf, 10));
		g_lpfnCallback(__func__, "**************************", "--");

		switch(nMapID){
		case -1:
			//awake for the group 1 word
			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
			break;
		case 10:
			cyhal_gpio_write(CYBSP_USER_LED2, CYBSP_LED_STATE_ON);
			break;
		case 11:
			cyhal_gpio_write(CYBSP_USER_LED2, CYBSP_LED_STATE_OFF);
			break;
		default:
			break;
		}
	}
	else if(nErr == DSPOTTER_ERR_Expired)
	{
		g_lpfnCallback(__func__, "Upper limit of recognition times is reached", itoa(DSPOTTER_ERR_Expired, pchBuf, 10));
	}
}
```

## Wifi
幾個重點:
*    softAP全部刪掉，使用STA
*    更改WIFI_SSID WIFI_PASSWORD WIFI_SECURITY_TYPE (iphone是WPA3)

![](https://hackmd.io/_uploads/ByJSkOKdh.png)

try:CY_WCM_SECURITY_WPA2_AES_PSK



### Ping Google DNS server IP 8.8.8.8 port 53
```clike=
cy_wcm_ip_address_t addr_ping;
addr_ping.version = CY_WCM_IP_VER_V4;
addr_ping.ip.v4 = 0x08080808;
uint32_t t_elapsed;
result = cy_wcm_ping(CY_WCM_INTERFACE_TYPE_STA, &addr_ping, 1000, &t_elapsed);
if(result == CY_RSLT_SUCCESS){
    printf("Ping Completed! Elapsed time: %d\n", t_elapsed);
}
```
結果: 有成功ping到他
![](https://hackmd.io/_uploads/BkkWKRUd3.png)

## Integrate WiFi & Voice Recongnition
*    先看wifi需要甚麼library:
![](https://hackmd.io/_uploads/BJuRNguO3.png)
要多裝: wifi-core-freertos-lwip-mbed(從library manager)

*    多加入需要的file
        *    FreeRTOSConfig.h
        *    lwipipts.h
        *    mbedtls_user_config.h
        *    wifi_config.h

*    更改makefile

COMPONENTS=FREERTOS LWIP MBEDTLS
DEFINES+=MBEDTLS_USER_CONFIG_FILE='"mbedtls_user_config.h"'
DEFINES+=CYBSP_WIFI_CAPABLE

原本整合在一起時，因為EABLE了freertos，因此原本的語音辨識也需要包成task，與tcp_client一同進行task的排程。

*    將語音辨識與wifi連線+socket server連線包成task，使用scheduler去排程 [已完成]
*    需要研究的部分為: 因為task與task間為獨立的，如何讓task間溝通及兩個task間priority的設計。[關鍵的部分]

## RTOS 再次研究
基本上就是將function獨立出來，使用RTOS讓他協助排程或是觸發事件。

### Thread/Task:
*    thread有三種狀態: idle, halted, running.
*    Higher priority tasks will be prioritized to run before lower priority tasks.

### Queues:
*    可以安全的讓thread與thread間溝通。
*    當一個thread要讀東西，但是queue裡面是空的，那這個task將會等到有東西被放入queue後才可以繼續執行。

### Semaphores:
*    就是一個flag(unsigned int)
*    set/give a semaphore時，數值會增加
*    get/take a semaphore時，數值會減少
*    當數值為0時，thread會暫停直到semaphore被set
*    可用於不同thread中告訴別人某事已經完成
*    例子:
For instance, you could have a collectDataThread that reads data from a sensor and a sendData thread that sends the data up to the cloud. The sendData thread would "get" the semaphore which will suspend the thread UNTIL the collectDataThread "sets" the semaphore when it has new data available that needs to be sent.

### Mutexes:
基本上就是一個鎖，用來限制不同thread間隊共同資源的存取，以避免race condition的事情發生。

### 如何使用FreeRTOS in MTB
*    1. library manager安裝FreeRTOS library
*    2. 在makefile COMPONENT多加上 FREETOS RTOS_AWARE
*    3. copy FreeRTOSConfig.h到root dir
*    4. 移除#warning line
*    5. coding template

```clike=
//Add the required header files:
#include "FreeRTOS.h"
#include "task.h"

//Create functions for any tasks that your application requires.
void MyFunction(void *arg)
{
    (void)arg;
     /* Initialize hardware */
     for (;;)
     {
     /* Do something */
     MyActions();
 
     /* Allow other tasks to execute */
     vTaskDelay(500);
     }
}

//Create a task for each of your functions and start the RTOS scheduler
int main(void)
{
    /* Initialize the device and board peripherals */
    if (CY_RSLT_SUCCESS != cybsp_init())
     {
     /* Unable to initialize BSP so HALT */
     CY_ASSERT(0);
     }
     if (pdPASS == xTaskCreate(MyFunction, // Task function
        "Task Name", // Task name
        1024, // Task stack size        
        NULL, // Parameters passed to task
        5, // Task priority
        NULL) // Task handle
     )
     {
         vTaskStartScheduler();
     }
     /* vTaskStartScheduler should never return.
    If we get here, then there was a serious error */
    CY_ASSERT(0);
}
```

### Task:

prototype:
```clike=
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
    const char * const pcName,
    const configSTACK_DEPTH_TYPE usStackDepth,
    void * const pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t * const pxCreatedTask )
```

*    priority最低的為0，為idle task
*    有最高priority的task將會優先執行，除非task被block住，否則不會停止
*    如果兩個task有相同的priority且都沒有block住，則FreeRTOSConfig.h會share CPU

### 如何Block CPU?
*    vTaskDelay block住task，讓其他task可以被執行
*    xQueueSend 假如要send的queue已經滿了，那就會被block住直到可以順利send出去或是timeout掉了
*    xQueueReceive 如果要讀的queue是空的，則task將會被block住直到有東西進入queue或是timeout掉了
*    xSemaphoreTake 當semaphore沒有被give

### Queue Sample
可以透過queue來進行task間的溝通
```clike=
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"
// Global Queue Handle
static QueueHandle_t qh = 0;

void task_tx(void *p)
{
    int myInt = 0;
    while (1)
    {
        myInt++;
        if (!xQueueSend(qh, &myInt, pdMS_TO_TICKS(500)))
         {
             puts("Failed to send item to queue within 500ms");
         }
     vTaskDelay(1000);
     }
}

void task_rx(void *p)
{
    int myInt = 0;
    while (1)
    {
        if (!xQueueReceive(qh, &myInt, pdMS_TO_TICKS(10000)))
        {
            puts("Failed to receive item within 10000 ms");
        }
        else
        {
        printf("Received: %u\n", myInt);
        }
    }
}

int main(void)
{
    qh = xQueueCreate(1, sizeof(int));
    xTaskCreate(task_tx, (char *)"t1", 2048, 0, 1, 0);
    xTaskCreate(task_rx, (char *)"t2", 2048, 0, 1, 0);
    vTaskStartScheduler();
    CY_ASSERT(0); // Should never get here
}
```
幾個需要注意的是:
*    要在createTask前create Queue
*    在task_tx中，每一秒send一個東西，如果queue是滿的，則會block 500ms(timeout)
*    在task_rx中，不需要使用vtaskDelay，因為如果queue沒有東西就會自動block住了。

### Mutexes sample

```clike=
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
static SemaphoreHandle_t spi_bus_lock=0;
void task_one()
{
    while (1)
    {
        if (xSemaphoreTake(spi_bus_lock, 1000))
        {
        // Use Guarded Resource
        // Give Semaphore back:
        xSemaphoreGive(spi_bus_lock);
        }
    }
}

void task_two()
{
    while (1)
    {
        if (xSemaphoreTake(spi_bus_lock, 1000))
        {
        // Use Guarded Resource
        // Give Semaphore back:
        xSemaphoreGive(spi_bus_lock);
        }
    }
}

int main(void)
{
    spi_bus_lock = xSemaphoreCreateMutex();
    xTaskCreate(task_one, (char *)"t1", 2048, 0, 1, 0);
    xTaskCreate(task_two, (char *)"t2", 2048, 0, 1, 0);
    vTaskStartScheduler();
    CY_ASSERT(0);
}
```

mutex只會讓一個task去進行，當其他task也進行到被mutex所住的資源將會sleep。