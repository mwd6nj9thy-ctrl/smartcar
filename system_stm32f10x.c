/**
  ******************************************************************************
  * @file    system_stm32f10x.c
  * @author  MicroControler Team
  * @version V3.5.0
  * @date    11-March-2011
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Source File.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32f10x_system
  * @{
  */

/** @addtogroup STM32F10x_System_Private_Includes
  * @{
  */

#include "stm32f10x.h"

/**
  * @}
  */

/** @addtogroup STM32F10x_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32F10x_System_Private_Defines
  * @{
  */

/*!< Uncomment the line corresponding to the desired System clock (SYSCLK) frequency (after reset the HSI is used as SYSCLK source) */

#if defined (STM32F10X_LD_VL) || (defined STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
/* #define SYSCLK_FREQ_HSE    HSE_VALUE */
 #define SYSCLK_FREQ_24MHz  24000000
#else
/* #define SYSCLK_FREQ_HSE    HSE_VALUE */
/* #define SYSCLK_FREQ_24MHz  24000000 */
 #define SYSCLK_FREQ_36MHz  36000000
/* #define SYSCLK_FREQ_48MHz  48000000 */
/* #define SYSCLK_FREQ_56MHz  56000000 */
 #define SYSCLK_FREQ_72MHz  72000000
#endif

/*!< Typical Value of the HSI in Hz */
#define HSI_VALUE    ((uint32_t)8000000)

/*!< Typical Value of the HSE in Hz */
#define HSE_VALUE    ((uint32_t)8000000)

/*!< Time out for HSE start up, in ms */
#define HSE_STARTUP_TIMEOUT   ((uint16_t)500)

/*!< Internal Voltage Reference */
#define VREFINT_CAL_ADDR                 ((uint32_t)0x1FFFF7BA)

/**
  * @}
  */

/** @addtogroup STM32F10x_System_Private_Macros
  * @{
  */

/*!< \cond */
#if defined SYSCLK_FREQ_72MHz
  #define SYSCLK_FREQ       72000000
#elif defined SYSCLK_FREQ_56MHz
  #define SYSCLK_FREQ       56000000
#elif defined SYSCLK_FREQ_48MHz
  #define SYSCLK_FREQ       48000000
#elif defined SYSCLK_FREQ_36MHz
  #define SYSCLK_FREQ       36000000
#elif defined SYSCLK_FREQ_24MHz
  #define SYSCLK_FREQ       24000000
#elif defined SYSCLK_FREQ_HSE
  #define SYSCLK_FREQ       HSE_VALUE
#else /* HSI Selected as System Clock source */
  #define SYSCLK_FREQ       HSI_VALUE
#endif

/*!< \endcond */

/**
  * @}
  */

/** @addtogroup STM32F10x_System_Private_Variables
  * @{
  */

/***********************   Clock Definitions   *****************************/
#ifdef SYSCLK_FREQ_HSE
  uint32_t SystemCoreClock         = SYSCLK_FREQ_HSE;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_24MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_24MHz;      /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_36MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_36MHz;      /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_48MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_48MHz;      /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_56MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_56MHz;      /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_72MHz
  uint32_t SystemCoreClock         = SYSCLK_FREQ_72MHz;      /*!< System Clock Frequency (Core Clock) */
#else
  uint32_t SystemCoreClock         = HSI_VALUE;              /*!< System Clock Frequency (Core Clock) */
#endif

__I uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

/**
  * @}
  */

/** @addtogroup STM32F10x_System_Private_FunctionPrototypes
  * @{
  */

static void SetSysClock(void);

#ifdef SYSCLK_FREQ_HSE
  static void SetSysClockToHSE(void);
#elif defined SYSCLK_FREQ_24MHz
  static void SetSysClockTo24(void);
#elif defined SYSCLK_FREQ_36MHz
  static void SetSysClockTo36(void);
#elif defined SYSCLK_FREQ_48MHz
  static void SetSysClockTo48(void);
#elif defined SYSCLK_FREQ_56MHz
  static void SetSysClockTo56(void);
#elif defined SYSCLK_FREQ_72MHz
  static void SetSysClockTo72(void);
#endif

/**
  * @}
  */

/** @addtogroup STM32F10x_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system
  *         Initialize the Embedded Flash Interface, the PLL and update the 
  *         SystemCoreClock variable.
  * @note   This function should be called first thing after reset is issued.
  * @param  None
  * @retval None
  */
void SystemInit (void)
{
  /*!< Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /*!< Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
#ifndef STM32F10X_LD_VL
  RCC->CFGR &= (uint32_t)0xF8FF0000;
#else
  RCC->CFGR &= (uint32_t)0xF0FF0000;
#endif

  /*!< Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /*!< Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /*!< Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;

#ifdef STM32F10X_LD_VL
  /*!< Reset PLL2ON and PLL3ON bits */
  RCC->CR &= (uint32_t)0xEBFFFFFF;

  /*!< Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x00FF0000;

  /*!< Reset CFGR2 register */
  RCC->CFGR2 = 0x00000000;
#elif defined STM32F10X_MD_VL
  /*!< Reset PLL2ON and PLL3ON bits */
  RCC->CR &= (uint32_t)0xEBFFFFFF;

  /*!< Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x00FF0000;

  /*!< Reset CFGR2 register */
  RCC->CFGR2 = 0x00000000;
#else
  /*!< Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;

  /*!< Reset CFGR2 register */
  RCC->CFGR2 = 0x00000000;
#endif

  /* Configure the System clock frequency, AHB/APBx prescalers and Flash settings */
  SetSysClock();
}

/**
  * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock (HCLK), it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock (HCLK) changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real 
  *       frequency in the chip. It is calculated based on the predefined 
  *       constant and the selected clock source:
  *
  *       - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(*)
  *
  *       - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(**)
  *
  *       - If SYSCLK source is PLL, SystemCoreClock will contain the HSE_VALUE(**) 
  *         or HSI_VALUE(*) multiplied by the PLL factors.
  *
  *       (*) HSI_VALUE is a constant defined in stm32f1xx.h file (default value
  *           is 8 MHz) but the real value may vary depending on the variations
  *           in voltage and temperature.
  *
  *       (**) HSE_VALUE is a constant defined in stm32f1xx.h file (its value
  *            depends on the crystal used on your hardware set-up).
  *
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0, pllmull = 0, pllsource = 0;

#ifdef STM32F10X_LD_VL
  uint32_t prediv1source = 0, prediv1 = 0, prediv2 = 0, pll2mull = 0;
#endif

#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || (defined STM32F10X_HD_VL)
  uint32_t prediv1source = 0, prediv1 = 0;
#endif

  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;

  switch (tmp)
  {
    case 0x00:  /* HSI used as system clock */
      SystemCoreClock = HSI_VALUE;
      break;
    case 0x04:  /* HSE used as system clock */
      SystemCoreClock = HSE_VALUE;
      break;
    case 0x08:  /* PLL used as system clock */

      /* Get PLL clock source and multiplication factor ----------------------*/
      pllmull = RCC->CFGR & RCC_CFGR_PLLMULL;
      pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;

#ifndef STM32F10X_LD_VL
      pllmull = ( pllmull >> 18) + 2;

      if (pllsource == 0x00)
      {
        /* HSI oscillator clock divided by 2 selected as PLL clock entry */
        SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
      }
      else
      {
        /* HSE oscillator clock selected as PLL clock entry */
        if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != (uint32_t)RESET)
        {/* HSE oscillator clock divided by 2 */
          SystemCoreClock = (HSE_VALUE >> 1) * pllmull;
        }
        else
        {
          SystemCoreClock = HSE_VALUE * pllmull;
        }
      }
#else
      pllmull = pllmull >> 18;
      pllmull = (pllmull) + 2;

      if (pllsource == 0x00)
      {
        /* HSI oscillator clock divided by 2 selected as PLL clock entry */
        SystemCoreClock = (HSI_VALUE >> 1) * pllmull;
      }
      else
      {
        prediv1source = (RCC->CFGR2 & RCC_CFGR2_PREDIV1SRC);
        if (prediv1source == 0)
        {
          /* HSE oscillator clock selected as PREDIV1 clock entry */
          prediv1 = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;
          SystemCoreClock = (HSE_VALUE / prediv1) * pllmull;
        }
        else
        {
          /* PLL2 clock selected as PREDIV1 clock entry */

          prediv2 = ((RCC->CFGR2 & RCC_CFGR2_PREDIV2) >> 4) + 1;
          pll2mull = ((RCC->CFGR2 & RCC_CFGR2_PLL2MUL) >> 8 ) + 2;
          prediv1 = (RCC->CFGR2 & RCC_CFGR2_PREDIV1) + 1;

          SystemCoreClock = (((HSE_VALUE / prediv2) * pll2mull) / prediv1) * pllmull;
        }
      }
#endif
      break;

    default:
      SystemCoreClock = HSI_VALUE;
      break;
  }

  /* Compute HCLK clock frequency ----------------*/
  /* Get HCLK prescaler */
  tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
  /* HCLK clock frequency */
  SystemCoreClock >>= tmp;
}

/**
  * @brief  Configures the System clock frequency, AHB/APBx prescalers and Flash
  *         settings.
  * @note   This function should be called only once the RCC clock is configured
  *         and before running any other init functions.
  * @param  None
  * @retval None
  */
static void SetSysClock(void)
{
#ifdef SYSCLK_FREQ_HSE
  SetSysClockToHSE();
#elif defined SYSCLK_FREQ_24MHz
  SetSysClockTo24();
#elif defined SYSCLK_FREQ_36MHz
  SetSysClockTo36();
#elif defined SYSCLK_FREQ_48MHz
  SetSysClockTo48();
#elif defined SYSCLK_FREQ_56MHz
  SetSysClockTo56();
#elif defined SYSCLK_FREQ_72MHz
  SetSysClockTo72();
#endif
 /* If none of the define above is enabled, the HSI is used as System clock
    source (default after reset) */
}

/**
  * @brief  Setup the external memory controller.
  *         Called in SystemInit function before relocation from Flash to RAM
  *         The SystemInit() function sets the Flash latency before enabling
  *         the external memory controller.
  * @param  None
  * @retval None
  */
#ifdef DATA_IN_ExtSRAM
void SystemInit_ExtMemCtl(void)
{
  /*!< FSMC Bank1 NOR/SRAM 3 is used for the STM3210E-EVAL, if another Bank is
      required, please adjust the FSMC initialization code inside the
      SystemInit_ExtMemCtl() function according to your needs. */

  /*!< Enable FSMC clock */
  RCC->AHBENR = 0x00000114;

  /*!< Enable GPIOD, GPIOE, GPIOF and GPIOG clocks */
  RCC->APB2ENR = 0x000001E0;

  /*!< Configure FSMC_A10 ... FSMC_A15 -----------------------------------------------*/
  GPIOF->CRL = 0x44444444;
  GPIOF->CRH = 0x44444444;
  GPIOG->CRL = 0x44444444;
  GPIOG->CRH = 0x444B4444;

  /*!< Configure FSMC_D0 ... FSMC_D3 -----------------------------------------------*/
  GPIOD->CRL = 0x44B44444;

  /*!< Configure FSMC_D4 ... FSMC_D15 -----------------------------------------------*/
  GPIOD->CRH = 0x4444B444;
  GPIOE->CRL = 0xB44444B4;
  GPIOE->CRH = 0x444B4B44;

  /*!< Configure FSMC_NWE */
  GPIOD->CRH |= 0x00100000;
  /*!< Configure FSMC_NOE */
  GPIOD->CRH |= 0x00001000;
  /*!< Configure FSMC_NE3 */
  GPIOG->CRH |= 0x00100000;
  /*!< Configure FSMC_A16 */
  GPIOD->CRL |= 0x00000004;

  /*-- FSMC Configuration ------------------------------------------------------*/
  /*!< Enable the FSMC */
  FSMC_Bank1->BTCR[0] = 0x00001011;
  FSMC_Bank1->BTCR[1] = 0x00000200;
}
#endif /* DATA_IN_ExtSRAM */

#ifdef SYSCLK_FREQ_HSE
/**
  * @brief  Selects HSE as System Clock source and configure HCLK, PCLK2
  *         and PCLK1 prescalers.
  * @note   This function should be called only once the HSE is enabled.
  * @param  None
  * @retval None
  */
static void SetSysClockToHSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /*!< SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /*!< Enable HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

  /*!< Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /*!< Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /*!< Flash 0 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_0;

    /*!< HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /*!< PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

    /*!< PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

    /*!< Select HSE as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE;

    /*!< Wait till HSE is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x04)
    {
    }
  }
  else
  {
    /*!< If HSE fails to start-up, the application will have wrong clock configuration.
        User can add here some code to deal with this error */
  }
}
#elif defined SYSCLK_FREQ_72MHz
/**
  * @brief  Sets System clock frequency to 72 MHz and configure HCLK, PCLK2
  *         and PCLK1 prescalers.
  * @note   This function should be called only once the HSE is enabled.
  * @param  None
  * @retval None
  */
static void SetSysClockTo72(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

  /*!< SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
  /*!< Enable HSE */
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);

  /*!< Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    /*!< Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /*!< Flash 2 wait states */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    /*!< HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /*!< PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

    /*!< PCLK1 = HCLK/2 */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

    /*!< PLL configuration: PLLCLK = HSE/1 * 9 = 72 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

    /*!< Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /*!< Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /*!< Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

    /*!< Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  {
    /*!< If HSE fails to start-up, the application will have wrong clock configuration.
        User can add here some code to deal with this error */
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
