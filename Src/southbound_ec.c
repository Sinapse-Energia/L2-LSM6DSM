#include "southbound_ec.h"

extern TIM_HandleTypeDef    htim3;
extern TIM_OC_InitTypeDef sConfig;

 SPI_HandleTypeDef hspi1;
 SPI_HandleTypeDef hspi2;
 int continue_INT_SPI=0;

static void MX_TIM3_Init(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

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
void relayActivation(GPIO_TypeDef* gpio_PORT, uint16_t gpio_PIN)
{
	/* Check the parameters */
	 assert_param(IS_GPIO_ALL_INSTANCE(gpio_PORT));
	 assert_param(IS_GPIO_PIN(gpio_PIN));

	HAL_GPIO_WritePin(gpio_PORT, gpio_PIN, GPIO_PIN_SET);


}

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
void relayDeactivation(GPIO_TypeDef* gpio_PORT, uint16_t gpio_PIN)
{
	/* Check the parameters */
	assert_param(IS_GPIO_ALL_INSTANCE(gpio_PORT));
	assert_param(IS_GPIO_PIN(gpio_PIN));

	HAL_GPIO_WritePin(gpio_PORT, gpio_PIN, GPIO_PIN_RESET);

}



////////////////////
/////////////////////  PWM

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;


  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = PERIOD_PWM;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfig.OCMode = TIM_OCMODE_PWM1;
  sConfig.Pulse = 0;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfig, TIM_CHANNEL_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim3);

}


void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim_pwm)
{

  if(htim_pwm->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }

}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspPostInit 0 */

  /* USER CODE END TIM3_MspPostInit 0 */

    /**TIM3 GPIO Configuration
    PC8     ------> TIM3_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_MspPostInit 1 */

  /* USER CODE END TIM3_MspPostInit 1 */
  }

}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* htim_pwm)
{

  if(htim_pwm->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }

}



void initializePWM(void)
{

	MX_TIM3_Init();

}





/// Function that does one regulation over 1-10V output. If regulation is one value from 0 to 100,
/// Output is (regulation/10) Volts.
int dimming(int regulation)
{

	/// Francis TO REVIEW HW PWM for STM32F215RE

	 /* Set the pulse value for channel 2 */
	float temp=0.0;
	if (regulation>100) return -1;




		temp= (PERIOD_PWM/100.0)*(100-(regulation*0.9));   /// 100 - () because PWM inverts., regulation*0.9 to adapt scale.
		sConfig.Pulse = (uint32_t)temp;
		  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfig, TIM_CHANNEL_3) != HAL_OK)
		  {
		    /* Configuration Error */
		   Error_Handler();
		   return -1;
		  }


		  /* Start channel 3 */
		  if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3) != HAL_OK)
		  {
		    /* PWM Generation Error */
		   Error_Handler();
		   return -1;
		  }

		  return 1;

}

///////////////// Flash functions.



/**
  * @brief  erase Application or its copy FLASH
  * @param  fl_bank: flash area (application or its copy bank)
  * @retval true if OK,  otherwise return false
  */
HAL_StatusTypeDef FlashNVM_EraseSector(uint8_t sector)
{
	HAL_StatusTypeDef status;
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError = 0;
	uint8_t sector_start, sectors_n;



	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);


	EraseInitStruct.Sector = sector;
	EraseInitStruct.TypeErase = TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.NbSectors = 1;

	status = HAL_BUSY;
	while (status == HAL_BUSY) {
		status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	}
	HAL_FLASH_Lock();

	return status;
}





/**
  * @brief  Read a binary array from FLASH
  * @param  address: FLASH relative address to read
  * @param  data_out: output data array pointer
  * @param  size: array length
  * @retval operation status
  */
HAL_StatusTypeDef FlashNVM_Read(uint32_t start_address, uint8_t* data_out, uint32_t size)
{
	/// from M0 m2m2
    //uint32_t sizeCounter = 0;

	//while (sizeCounter < size) {
	//    *data_out = (*(__IO uint8_t*)start_address);
	//    data_out++;
	//    start_address++;
	//    sizeCounter++;
	//}
    //return HAL_OK;

	//from new m2m as cortex m4
	uint32_t sizeCounter = 0;

		// Check input data
	    if (!IS_FLASH_ADDRESS(start_address)) {
	        // It's not Flash's address
	    	return HAL_ERROR;
		}

		while (sizeCounter < size) {
		    *data_out = (*(__IO uint32_t*)start_address);
		    data_out++;
		    start_address++;
		    sizeCounter++;
		}
	    return 1;
}


/**
  * @brief  write data array to PREVIOSLY ERISED FLASH memory
  * @param  fl_bank: flash area (application or its copy bank)
  * @retval true if OK,  otherwise return false
  */
HAL_StatusTypeDef FlashNVM_Write(uint32_t start_address, const uint8_t* data_in, uint32_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t i;
	uint16_t *integerPointer;
	integerPointer = (uint16_t *)data_in;

	// remove for M0
	// Check input data
    //if (!IS_FLASH_ADDRESS(start_address)) {
        // It's not Flash's address
    //	return HAL_ERROR;
	//}

	// for new M2M
	// Check input data
	 if (!IS_FLASH_ADDRESS(start_address)) {
	      // It's not Flash's address
	    	return HAL_ERROR;
	 }

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	                           FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	//__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR );



	// Write data
	  for (i = 0; i < size; i++) {
	   	status = HAL_BUSY;
	   	while (status == HAL_BUSY) {
	   		status = HAL_FLASH_Program(TYPEPROGRAM_BYTE, start_address + i, data_in[i]);
	   	}
	   	if ( status != HAL_OK) {
	   		break;
	   	}
   }

	HAL_FLASH_Lock();

	return status;
}

int MIC_Flash_Memory_Write(const uint8_t *data_in, uint32_t size)
{


	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t sizeReceived=0;


	//status=  FlashNVM_EraseSector(FLASH_SECTOR_0);// it erased 16kb at beginning of 0x8000000
	status=  FlashNVM_EraseSector(FLASH_SECTOR_11);// it is erased last flash sector TO CHANGE. A lot of bytes 128kb

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Write(ORIGIN_SECTOR+4, data_in, size); // data are written with +4 offset from starting. First 4 bytes are used to write size of data
																// and watching if there are data or not.
	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Write(ORIGIN_SECTOR, &size, 4); // quantity of data are saved at first 4 bytes.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Read(ORIGIN_SECTOR, &sizeReceived, 4); // quantity of data are read again to verify it wrote good.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	if (status==HAL_OK) return sizeReceived; // if all goes fine, it returns size of data.


}

int MIC_Flash_Memory_Read(const uint8_t *data_out, uint32_t size)
{
	HAL_StatusTypeDef status = HAL_ERROR;
	uint32_t sizeReceived=0;

	status=FlashNVM_Read(ORIGIN_SECTOR+4, data_out, size); // start to read all data.
	status=FlashNVM_Read(ORIGIN_SECTOR+4, data_out, size); // start to read all data.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	status=FlashNVM_Read(ORIGIN_SECTOR, &sizeReceived, 4); // received quantity of saved bytes.

	if (status==HAL_ERROR) return -1; // if fails, it returns -1;

	if ((status==HAL_OK)&&(sizeReceived==size)) return sizeReceived; // if all goes fine, it returns size of data.


}

/// Below. Three function for SPI interface



/**
result config_spi(spi_handler,spi_interface,spi_pinspack, spi_portNSS, spi_pinNSS, spi_mode, spi_baudRate)

Inputs:

spi_handler:  This is the specific handler to configure the SPI interface (spi_lines *)
spi_interface: This is the interface to configure (SPIx_interface)
spi_pinspack: This is the chosen PINs configuration for one SPI (TM_SPI_PinsPack_t)
spi_portNSS: This is the port of pin NSS that implements the chip select control in SPI interface.
spu_pinNSS: This is the pin NSS that implements the select control in SPI interface.
spi_mode: This is the mode of working for SPI interface (MASTER_FULLDUPLEX, MASTER_HALLDUPLEX, ...)
spi_baudRate (int): Speed in KHZ to be used in the SPI interface

Output:

result: Number code indicating success (0) or error (number indicating the error code)
**/

int  config_spi(spi_lines *hspi,SPIx_interface spi, TM_SPI_PinsPack_t PinsPack,GPIO_TypeDef * portNSS, uint16_t pinNSS, SPImode mode, SPIbaudrate baudRate)
{
	// config_spi has embedded MX_SPI_init and HAL_SPI_init.
	// this function allows configure till three SPI interface with four different pinspack

	GPIO_InitTypeDef GPIO_InitStruct;

	  /* Check the SPI handle allocation */
	if(hspi == NULL)
	{
		return 1;
	}

	if (spi==SPI1_interface)
	{

		hspi->spi->Instance = SPI1;
	}
	if (spi==SPI2_interface)
	{
		hspi->spi->Instance = SPI2;
	}
	if (spi==SPI3_interface)
	{
		hspi->spi->Instance = SPI3;
	}

	if (mode==MASTER_FULLDUPLEX_SOFTWARE)
	{
		hspi->spi->Init.Mode = SPI_MODE_MASTER;
		hspi->spi->Init.Direction = SPI_DIRECTION_2LINES;
		hspi->spi->Init.NSS = SPI_NSS_SOFT;
	}
	else
	{
	 				   // to implement
	}

		hspi->spi->Init.DataSize = SPI_DATASIZE_8BIT;
		hspi->spi->Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi->spi->Init.CLKPhase = SPI_PHASE_1EDGE;
		hspi->spi->Init.NSS = SPI_NSS_SOFT;
		hspi->spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi->spi->Init.TIMode = SPI_TIMODE_DISABLE;
		hspi->spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		hspi->spi->Init.CRCPolynomial = 10;

	  switch (baudRate)
	  {
	   	  case  baud_13000_26MHzHSE: hspi->spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	   	  break;
	   	  case  baud_6500_26MHzHSE: hspi->spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	   	  break;
	   	  case  baud_3250_26MHzHSE: hspi->spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	 	  break;
	 	  case  baud_1625_26MHzHSE: hspi->spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	 	  break;
	 	  case  baud_812_26MHzHSE: hspi->spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	 	  break;
	 	  case  baud_406_26MHzHSE: hspi->spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	 	  break;
	 	  case  baud_203_26MHzHSE: hspi->spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	 	  break;
	 	  case  baud_101_26MHzHSE: hspi->spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	 	  break;

	 	}


	  	  if (portNSS==GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
	 	  else if (portNSS==GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
	  	  else if (portNSS==GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
	  	  else if (portNSS==GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
	  	  else if (portNSS==GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
	  	  else if (portNSS==GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
	  	  else if (portNSS==GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
	  	  else if (portNSS==GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
	  	  else if (portNSS==GPIOI) __HAL_RCC_GPIOI_CLK_ENABLE();
	 	  else return 1;






	   	   GPIO_InitStruct.Pin = pinNSS;
	   	   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	   	   GPIO_InitStruct.Pull = GPIO_NOPULL;
	   	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	   	   HAL_GPIO_Init(portNSS, &GPIO_InitStruct);

	   	   hspi->portNSS=portNSS;
	   	   hspi->pinNSS=pinNSS;

	  	  /* Check the parameters */
	  	  assert_param(IS_SPI_ALL_INSTANCE(hspi->spi->Instance));
	  	  assert_param(IS_SPI_MODE(hspi->spi->Init.Mode));
	  	  assert_param(IS_SPI_DIRECTION(hspi->spi->Init.Direction));
	  	  assert_param(IS_SPI_DATASIZE(hspi->spi->Init.DataSize));
	  	  assert_param(IS_SPI_NSS(hspi->spi->Init.NSS));
	  	  assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi->spi->Init.BaudRatePrescaler));
	  	  assert_param(IS_SPI_FIRST_BIT(hspi->spi->Init.FirstBit));
	  	  assert_param(IS_SPI_TIMODE(hspi->spi->Init.TIMode));
	  	  if(hspi->spi->Init.TIMode == SPI_TIMODE_DISABLE)
	  	  {
	  		  assert_param(IS_SPI_CPOL(hspi->spi->Init.CLKPolarity));
	  		  assert_param(IS_SPI_CPHA(hspi->spi->Init.CLKPhase));
	  	  }

		#if (USE_SPI_CRC != 0U)
	  	  assert_param(IS_SPI_CRC_CALCULATION(hspi->spi->Init.CRCCalculation));
	  	  if(hspi->spi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
	  	  {
	  		  assert_param(IS_SPI_CRC_POLYNOMIALhspi->spi->Init.CRCPolynomial));
	  	  }
   	   #else
	  	hspi->spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    	#endif /* USE_SPI_CRC */

	  	hspi->spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  	  if(hspi->spi->State == HAL_SPI_STATE_RESET)
	  	  {
	  		  /* Allocate lock resource and initialize it */
	  		  	  hspi->spi->Lock = HAL_UNLOCKED;

				 // First the mapping and configuration of SPI interface must be made.

				 /// Only 6 combinations have been implemented. If this library is needed in other upper
				 // microcontroller, as STM32F4 or STM32F7 is needed to include more SPI interfaces

				if ((spi==SPI1_interface)&(PinsPack==TM_SPI_PinsPack_1))
				{

					  __HAL_RCC_SPI1_CLK_ENABLE();
					 GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
					 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					 GPIO_InitStruct.Pull = GPIO_NOPULL;
					 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
					 GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
					 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
					  /* SPI1 interrupt Init */
					 HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
					 HAL_NVIC_EnableIRQ(SPI1_IRQn);

				}
				if ((spi==SPI1_interface)&(PinsPack==TM_SPI_PinsPack_2))
				{

					  __HAL_RCC_SPI1_CLK_ENABLE();
					 GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
					 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					 GPIO_InitStruct.Pull = GPIO_NOPULL;
					 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
					 GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
					 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
					 /* SPI1 interrupt Init */
					 HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
					 HAL_NVIC_EnableIRQ(SPI1_IRQn);
				}

				if ((spi==SPI2_interface)&(PinsPack==TM_SPI_PinsPack_1))
				{

					  __HAL_RCC_SPI2_CLK_ENABLE();
					 GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_2;
					 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					 GPIO_InitStruct.Pull = GPIO_NOPULL;
					 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
					 GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
					 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

					 GPIO_InitStruct.Pin = GPIO_PIN_10;
					 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					 GPIO_InitStruct.Pull = GPIO_NOPULL;
					 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
					 GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
					 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

					 /* SPI2 interrupt Init */
					 HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
					 HAL_NVIC_EnableIRQ(SPI2_IRQn);
				}

				if ((spi==SPI2_interface)&(PinsPack==TM_SPI_PinsPack_2))
				{

					  __HAL_RCC_SPI2_CLK_ENABLE();
					 GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
					 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					 GPIO_InitStruct.Pull = GPIO_NOPULL;
					 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
					 GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
					 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

					 /* SPI2 interrupt Init */
					 HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
					 HAL_NVIC_EnableIRQ(SPI2_IRQn);

				}

				if ((spi==SPI3_interface)&(PinsPack==TM_SPI_PinsPack_1))
				{
					  __HAL_RCC_SPI3_CLK_ENABLE();
					 GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
					 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					 GPIO_InitStruct.Pull = GPIO_NOPULL;
					 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
					 GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
					 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

					 /* SPI3 interrupt Init */
					 HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
					 HAL_NVIC_EnableIRQ(SPI3_IRQn);


				}
				if ((spi==SPI3_interface)&(PinsPack==TM_SPI_PinsPack_2))
				{

					  __HAL_RCC_SPI3_CLK_ENABLE();
					 GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
					 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					 GPIO_InitStruct.Pull = GPIO_NOPULL;
					 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
					 GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
					 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

					 /* SPI3 interrupt Init */
					HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
					HAL_NVIC_EnableIRQ(SPI3_IRQn);


				}


 	 } ///  if(hspi->State == HAL_SPI_STATE_RESET


	  	hspi->spi->State = HAL_SPI_STATE_BUSY;

 	  /* Disable the selected SPI peripheral */
 	  __HAL_SPI_DISABLE(hspi->spi);

 	  /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
 	  /* Configure : SPI Mode, Communication Mode, Data size, Clock polarity and phase, NSS management,
 	  Communication speed, First bit and CRC calculation state */
 	  WRITE_REG(hspi->spi->Instance->CR1, (hspi->spi->Init.Mode | hspi->spi->Init.Direction | hspi->spi->Init.DataSize |
 			 hspi->spi->Init.CLKPolarity | hspi->spi->Init.CLKPhase | (hspi->spi->Init.NSS & SPI_CR1_SSM) |
			 hspi->spi->Init.BaudRatePrescaler | hspi->spi->Init.FirstBit  | hspi->spi->Init.CRCCalculation) );

 	  /* Configure : NSS management */
 	  WRITE_REG(hspi->spi->Instance->CR2, (((hspi->spi->Init.NSS >> 16U) & SPI_CR2_SSOE) | hspi->spi->Init.TIMode));

 	 #if (USE_SPI_CRC != 0U)
 	  /*---------------------------- SPIx CRCPOLY Configuration ------------------*/
 	  /* Configure : CRC Polynomial */
 	  if(hspi->spi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
 	  {
 	    WRITE_REG(hspi->spi->Instance->CRCPR, hspi->spi->Init.CRCPolynomial);
 	  }
 	 #endif /* USE_SPI_CRC */

 	 #if defined(SPI_I2SCFGR_I2SMOD)
 	  /* Activate the SPI mode (Make sure that I2SMOD bit in I2SCFGR register is reset) */
 	  CLEAR_BIT(hspi->spi->Instance->I2SCFGR, SPI_I2SCFGR_I2SMOD);
 	 #endif /* USE_SPI_CRC */

 	 hspi->spi->ErrorCode = HAL_SPI_ERROR_NONE;
 	hspi->spi->State     = HAL_SPI_STATE_READY;

	return 0; //success

}

/**
int  write_spi_message(spi_handler, register_address, payload, size_payload)

// It is supposed that address is sent in Big Endian mode. Upper bytes are sent first.

Inputs
spi_handler: This is the specific handler for SPI lines (spi_lines*).
register_address: pointer to address of register in slave device to write. Address codified in HEX. (uint8_t *)
payload: Payload to be sent codified in HEX. (uint8_t *)
size_payload: number of bytes that contains payload. (int)
Output
result: Number code indicating success (0) or error (number indicating the error code).
**/



int  write_spi_message(spi_lines *hspi, uint8_t *reg, uint8_t *payload, int size_payload)
{

	uint8_t bufferTemp[BUFFER_SPI];

	if (hspi==NULL)
	{
		return 1;
	}

	HAL_GPIO_WritePin(hspi->portNSS,hspi->pinNSS,GPIO_PIN_RESET);

	continue_INT_SPI=0;
	if(HAL_SPI_TransmitReceive_IT(hspi->spi, reg, bufferTemp, 1) != HAL_OK)
	{
		return 1;					    /* Transfer error in transmission process */
	    Error_Handler();
	}

	while (HAL_SPI_GetState(hspi->spi) != HAL_SPI_STATE_READY)
	{
	}
	while(continue_INT_SPI==0);

	continue_INT_SPI=0;
	if(HAL_SPI_TransmitReceive_IT(hspi->spi, payload, bufferTemp, size_payload) != HAL_OK)
	{
		    /* Transfer error in transmission process */
			return 1;
		    Error_Handler();
	}

	while (HAL_SPI_GetState(hspi->spi) != HAL_SPI_STATE_READY)
	{
	}
	while(continue_INT_SPI==0);

	HAL_GPIO_WritePin(hspi->portNSS,hspi->pinNSS,GPIO_PIN_SET);
	return 0;


}


/**
int read_spi(spi_handler,register_address, spi_message, spi_bytes_read)
(, , payload, size_payload)
Inputs
spi_handler: This is the specific handler for SPI lines (spi_lines*).
register_address: pointer to address of register in slave device to write. Address codified in HEX. (uint8_t *)
spi_message: The HEX stream with the value or values read from SPI interface.(uint8_t *)
spi_bytes_read: The number of read values.(int)
Output

result: Number code indicating success (0) or error (number indicating the error code).
**/

int read_spi(spi_lines *hspi, uint8_t * reg, uint8_t * spi_message, int spi_bytes_read)
{

	uint8_t bufferTemp[5];
	uint8_t dummyArray[BUFFER_SPI];

	if (hspi==NULL)
	{
			return 1;
	}

	HAL_GPIO_WritePin(hspi->portNSS,hspi->pinNSS,GPIO_PIN_RESET);
	continue_INT_SPI=0;
	if(HAL_SPI_TransmitReceive_IT(hspi->spi, reg, bufferTemp, 1) != HAL_OK)
	{
		    /* Transfer error in transmission process */
			return 1;
		    //Error_Handler();
	}

	while (HAL_SPI_GetState(hspi->spi) != HAL_SPI_STATE_READY)
	{
	}

	while(continue_INT_SPI==0);


	continue_INT_SPI=0;
	if(HAL_SPI_TransmitReceive_IT(hspi->spi, dummyArray, spi_message, spi_bytes_read) != HAL_OK)
	{
		    /* Transfer error in transmission process */
			return 1;
			//Error_Handler();
	}

	while (HAL_SPI_GetState(hspi->spi) != HAL_SPI_STATE_READY)
	{
	}
	while(continue_INT_SPI==0);

	HAL_GPIO_WritePin(hspi->portNSS,hspi->pinNSS,GPIO_PIN_SET);



	return 0;



}







void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{

	continue_INT_SPI=1;


}


