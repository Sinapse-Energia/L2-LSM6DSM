#ifndef __SOUTHBOUNDEC_h
#define __SOUTHBOUNDEC_H


#include "stm32f2xx_hal.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* User can use this section to tailor TIMx instance used and associated
   resources */
/* Definition for TIMx clock resources */
#define PERIOD_PWM 1082


#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM3_CLK_ENABLE()

/* Definition for TIMx Channel Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define TIMx_GPIO_PORT_CHANNEL1        GPIOB
#define TIMx_GPIO_PORT_CHANNEL2        GPIOB
#define TIMx_GPIO_PORT_CHANNEL3        GPIOB
#define TIMx_GPIO_PORT_CHANNEL4        GPIOB
#define TIMx_GPIO_PIN_CHANNEL1         GPIO_PIN_4
#define TIMx_GPIO_PIN_CHANNEL2         GPIO_PIN_5
#define TIMx_GPIO_PIN_CHANNEL3         GPIO_PIN_0
#define TIMx_GPIO_PIN_CHANNEL4         GPIO_PIN_1
#define TIMx_GPIO_AF_CHANNEL1          GPIO_AF1_TIM3
#define TIMx_GPIO_AF_CHANNEL2          GPIO_AF1_TIM3
#define TIMx_GPIO_AF_CHANNEL3          GPIO_AF1_TIM3
#define TIMx_GPIO_AF_CHANNEL4          GPIO_AF1_TIM3
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#include "stm32f2xx_hal.h"


// void relayActivation(
//		GPIO_TypeDef* gpio_PORT,
//		uint16_t gpio_PIN
// 		)
// Input parameters:
// ----> uint16_t gpio_PIN: Is the position in the port in STM32Fxx where relay is connected
// Output parameters: NONE
// Modified parameters:
// ----> GPIO_TypeDef* gpio_PORT: It is the handler to port in STM32Fxx microcontroller where relay is connected
// Type of routine: GENERIC (non dependent of device)
// Dependencies: NOTE
// Description:
// This function is used to activate one relay connected to gpio_PIN unto gpio_PORT of stm32fxx microcontroller.
// NOTE: For using this function, the GPIO must be initialized
// Example: relayActivation(GPIOX2_GPIO_Port,GPIOX2_Pin);
void relayActivation(GPIO_TypeDef* gpio_PORT, uint16_t gpio_PIN);


// void relayDeactivation(
//		GPIO_TypeDef* gpio_PORT,
//		uint16_t gpio_PIN
// 		)
// Input parameters:
// ----> uint16_t gpio_PIN: Is the position in the port in STM32Fxx where relay is connected
// Output parameters: NONE
// Modified parameters:
// ----> GPIO_TypeDef* gpio_PORT: It is the handler to port in STM32Fxx microcontroller where relay is connected
// Type of routine: GENERIC (non dependent of device)
// Dependencies: NOTE
// Description:
// This function is used to deactivate one relay connected to gpio_PIN unto gpio_PORT of stm32fxx microcontroller.
// NOTE: For using this function, the GPIO must be initialized
// Example: relayDeactivation(GPIOX2_GPIO_Port,GPIOX2_Pin);
void relayDeactivation(GPIO_TypeDef* gpio_PORT, uint16_t gpio_PIN);

/// HAL funtion to configure timer as PWM mode
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);

/// Initialize clocks and duty cycle for PWM in timer3
/// #define PERIOD_PWM must have one value for period, in general 1082
void initializePWM(void);

/// Function that does one regulation over 1-10V output. If regulation is one value from 0 to 100,
/// Output is (regulation/10) Volts.
int dimming(int regulation);


//// Flash functions.

// Public Functions


//#define ORIGIN_SECTOR    ((uint32_t)0x08000000) /* first flash sector 16k */

#define ORIGIN_SECTOR    ((uint32_t)0x080E0000) /* last 128k */

HAL_StatusTypeDef FlashNVM_EraseSector(uint8_t sector);
HAL_StatusTypeDef FlashNVM_Read(uint32_t start_address, uint8_t* data_out, uint32_t size);
HAL_StatusTypeDef FlashNVM_Write(uint32_t start_address, const uint8_t* data_in, uint32_t size);

int MIC_Flash_Memory_Write(const uint8_t *data_in, uint32_t size);
int MIC_Flash_Memory_Read(const uint8_t *data_out, uint32_t size);


// If _SOFTWARE is chosen, it is needed:
// A) to uncomment NSS_SOFTWARE and define PIN and PORT below.
// B) You must include into MX_GPIO_INIT() the configuration as output for NSS pin.
// C) You must configure HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) in order to define which pins for SPI1, SPI2 function
// is going to use.
// D) You must configure HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) in order to know what pins in HW SPI are going to be
// disabled.


/// SPI modes STM32Fxx

//        |PINSPACK1              |PINSPACK2              |PINSPACK3             |PINS PACK 4
//SPIX    |MOSI   MISO    SCK     |MOSI   MISO    SCK     |MOSI   MISO    SCK    |MOSI   MISO    SCK
//        |
//SPI1    |PA7    PA6     PA5     |PB5    PB4     PB3     |                      |
//SPI2    |PC3    PC2     PB10    |PB15   PB14    PB13    |PI3    PI2     PI0    |PB15   PB14    PI1
//SPI3    |PB5    PB4     PB3     |PC12   PC11    PC10    |                      |
//SPI4    |PE6    PE5     PE2     |PE14   PE13    PE12    |                      |
//SPI5    |PF9    PF8     PF7     |PF11   PH7     PH6     |                      |
//SPI6    |PG14   PG12    PG13    |




typedef struct {

	SPI_HandleTypeDef *spi;
	GPIO_TypeDef * portNSS;
	uint16_t pinNSS;
} spi_lines;


typedef enum {
	TM_SPI_PinsPack_1 = 0x00, /*!< Select PinsPack1 from Pinout table for specific SPI */
	TM_SPI_PinsPack_2,        /*!< Select PinsPack2 from Pinout table for specific SPI */
	TM_SPI_PinsPack_3,        /*!< Select PinsPack3 from Pinout table for specific SPI */
	TM_SPI_PinsPack_4        /*!< Select PinsPack4 from Pinout table for specific SPI */

} TM_SPI_PinsPack_t;

typedef enum {
	SPI1_interface=0x00,
	SPI2_interface,
	SPI3_interface

} SPIx_interface;


typedef enum
{
	MASTER_FULLDUPLEX_HARDWARE=0,
	MASTER_HALFDUPLEX_HARDWARE,
	MASTER_FULLDUPLEX_SOFTWARE, //only is implemented SOFTWARE mode
	MASTER_HALFDUPLEX_SOFTWARE,


} SPImode;

typedef enum
{
	baud_13000_26MHzHSE=0,
	baud_6500_26MHzHSE,
	baud_3250_26MHzHSE,
	baud_1625_26MHzHSE,
	baud_812_26MHzHSE,
	baud_406_26MHzHSE,
	baud_203_26MHzHSE,
	baud_101_26MHzHSE

} SPIbaudrate;



#define SPI_TIMEOUT 3000 // milliseconds
#define BUFFER_SPI 256

int  config_spi(spi_lines *hspi,SPIx_interface spi, TM_SPI_PinsPack_t PinsPack,GPIO_TypeDef * portNSS, uint16_t pinNSS, SPImode mode, SPIbaudrate baudRate);
int  write_spi_message(spi_lines *hspi, uint8_t *reg, uint8_t *payload, int size_payload);
int read_spi(spi_lines *hspi, uint8_t * reg, uint8_t * spi_message, int spi_bytes_read);

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif


#endif
