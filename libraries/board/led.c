#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

#include "led.h"

/*
	- {pin_num: M13, pin_signal: GPIO_AD_B1_09, label: LED_G, identifier: LED_G}
	- {pin_num: K12, pin_signal: GPIO_AD_B1_05, label: LED_B, identifier: LED_B}
	- {pin_num: L13, pin_signal: GPIO_AD_B1_10, label: LED_R, identifier: LED_R}

	- {pin_num: L13, peripheral: GPIO1, signal: 'gpio_io, 26', pin_signal: GPIO_AD_B1_10, direction: OUTPUT, gpio_init_state: 'true', pull_up_down_config: Pull_Up_22K_Ohm}
	- {pin_num: M13, peripheral: GPIO1, signal: 'gpio_io, 25', pin_signal: GPIO_AD_B1_09, direction: OUTPUT, gpio_init_state: 'true', pull_up_down_config: Pull_Up_22K_Ohm}
	- {pin_num: K12, peripheral: GPIO1, signal: 'gpio_io, 21', pin_signal: GPIO_AD_B1_05, direction: OUTPUT, gpio_init_state: 'true', pull_up_down_config: Pull_Up_22K_Ohm}
*/

void LED_Init(void)
{
	gpio_pin_config_t led_config;
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_10_GPIO1_IO26,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_09_GPIO1_IO25,0U);
	IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_05_GPIO1_IO21,0U);                                   

	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_05_GPIO1_IO21,0xD0B0u);                               
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_09_GPIO1_IO25,0xD0B0u);
	IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_10_GPIO1_IO26,0xD0B0u);

	/* GPIO configuration of LED_R on GPIO_AD_B1_10 (pin L13) */
	led_config.direction=kGPIO_DigitalOutput;
	led_config.outputLogic= 0U;
	led_config.interruptMode=kGPIO_NoIntmode;
	GPIO_PinInit(GPIO1, 26U, &led_config);

	/* GPIO configuration of LED_G on GPIO_AD_B1_09 (pin M13) */
	led_config.direction=kGPIO_DigitalOutput;
	led_config.outputLogic= 0U;
	led_config.interruptMode=kGPIO_NoIntmode;
	GPIO_PinInit(GPIO1, 25U, &led_config);
		
	/* GPIO configuration of LED_B on GPIO_AD_B1_05 (pin K12) */
	led_config.direction=kGPIO_DigitalOutput;
	led_config.outputLogic= 0U;
	led_config.interruptMode=kGPIO_NoIntmode;
	GPIO_PinInit(GPIO1, 21U, &led_config);

	LED_R(0);
	LED_G(0);
	LED_B(0);
}


