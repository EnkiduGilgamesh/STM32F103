/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI93xx.h"
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RXBUFFERSIZE  256     		//最大接收字节数
char RxBuffer[RXBUFFERSIZE];   		//接收数据
uint8_t aRxBuffer;								//接收中断缓冲
//uint8_t aRxBuffers[20];
uint8_t Uart1_Rx_Cnt = 0;					//接收缓冲计数

extern uint8_t message[200]; 			//接收字符串缓冲区
extern uint8_t offset; 						//接收字符串缓冲区的下标及大小
extern uint8_t mesg; 							//用于中断时，接收单个字符
extern uint8_t RX_Flag; 					//发生中断的标志
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int steAngleLv = 0;
int steAngleNum = 90;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void delay_ms(uint16_t time)
{    
   uint16_t i=0;  
   while(time--)
   {
      i=15000;  //自己定义
      while(i--) ;    
   }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin==KEY0_Pin)
  {
		delay_ms(400);
    if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)==0)
    {
			if(steAngleLv > -2)
				steAngleLv --;
			LCD_ShowxNum(280, 110, steAngleLv2Num(steAngleLv), 3, 24, 0);
    }
		//delay_ms(200);
    __HAL_GPIO_EXTI_CLEAR_IT(KEY0_Pin);
  }
  else if(GPIO_Pin==KEY1_Pin)
  {
		delay_ms(400);
    if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)==0)
    {
			if(steAngleLv < 2)
				steAngleLv ++;
			LCD_ShowxNum(280, 110, steAngleLv2Num(steAngleLv), 3, 24, 0);
    }
		//delay_ms(200);
    __HAL_GPIO_EXTI_CLEAR_IT(KEY1_Pin);
  }
}

int steAngleLv2CCR(int steAngleLv){
	return 185 - steAngleLv * 5;
}

int steAngleLv2Num(int steAngleLv){
	return 90 - steAngleLv * 45;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	// aRxBuffers[5] = '\0';
	// HAL_UART_Transmit(&huart1, message, offset, 100);
	if(huart == &huart1){
		if((char)aRxBuffer == 'a'){
			if(steAngleLv < 2){
				steAngleLv ++;
				HAL_UART_Transmit(&huart1, "逆时针旋转45度\r\n", 16, 0xFFFF);
			}
			else{
				HAL_UART_Transmit(&huart1, "最大角度\r\n", 10, 0xFFFF);
			}
			LCD_ShowxNum(280, 110, steAngleLv2Num(steAngleLv), 3, 24, 0);
			//HAL_UART_Transmit(&huart1, &cmd, 10,0xFFFF); 
		}
		else if((char)aRxBuffer == 'c'){
			if(steAngleLv > -2){
				steAngleLv --;
			HAL_UART_Transmit(&huart1, "顺时针旋转45度\r\n", 16, 0xFFFF);
			}
			else{
				HAL_UART_Transmit(&huart1, "最大角度\r\n", 10, 0xFFFF);
			}
			LCD_ShowxNum(280, 110, steAngleLv2Num(steAngleLv), 3, 24, 0);
		}
	}
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	TFTLCD_Init();
	LCD_Clear(BLUE);
	LCD_ShowString(30,40,210,24,24, "What a nice day!");
	LCD_ShowString(30,70,200,16,16, "TFTLCD TEST");
	LCD_ShowString(30,90,200,16,16, "2020/7/29");
	LCD_ShowString(30, 110, 250, 24, 24, "The angle is: ");
	LCD_ShowxNum(280, 110, steAngleLv2Num(steAngleLv), 3, 24, 0);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
	// printf("Z小旋测试\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, steAngleLv2CCR(steAngleLv));
		HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
