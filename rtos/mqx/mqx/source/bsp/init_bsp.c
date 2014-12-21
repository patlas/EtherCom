/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*   This file contains the source functions for functions required to
*   specifically initialize the card.
*
*END************************************************************************/

#include "user_config.h"
#include "mqx_inc.h"
#include "bsp.h"
#include "bsp_prv.h"
#include <assert.h>

#include "nio.h"
#include "nio_serial.h"
#include "nio_tty.h"
#include "nio_dummy.h"
//#if (__MPU_PRESENT == 1)
#ifdef HW_MPU_INSTANCE_COUNT
  #include <fsl_mpu_hal.h>
  #include <fsl_mpu_common.h>
#endif
//#endif

hwtimer_t systimer;     /* System timer handle */
_mem_pool_id _BSP_sram_pool;
static uint32_t _bsp_get_hwticks(void *param);

/** Workaround for link error message with Keil. It complains that DbgConsole_Init
  * symbol is undefined (referenced by hardware_init.o, even thought it is not used at all 
  * and after link processs the DbgConsole_Init is listed in "removed unused symbols".
 */
#ifdef __CC_ARM
#pragma weak DbgConsole_Init
#include "fsl_debug_console.h"
debug_console_status_t DbgConsole_Init(
  uint32_t uartInstance, uint32_t baudRate, debug_console_device_type_t device)
  {return kStatus_DEBUGCONSOLE_InvalidDevice;}
#endif

/** Pre initialization - initializing requested modules for basic run of MQX.
 */
int _bsp_pre_init(void)
{
    uint32_t result;
    KERNEL_DATA_STRUCT_PTR kernel_data;

    /* Set the CPU type */
    _mqx_set_cpu_type(MQX_CPU);

#if MQX_EXIT_ENABLED
    /* Set the bsp exit handler, called by _mqx_exit */
    _mqx_set_exit_handler(_bsp_exit_handler);
#endif

    /* Memory splitter - prevent accessing both ram banks in one instruction */
    _mem_alloc_at(0, (void*)0x20000000);

    result = _psp_int_init(BSP_FIRST_INTERRUPT_VECTOR_USED, BSP_LAST_INTERRUPT_VECTOR_USED);
    if (result != MQX_OK) {
        return result;
    }

    /* set possible new interrupt vector table - if MQX_ROM_VECTORS = 0 switch to
    ram interrupt table which was initialized in _psp_int_init) */
    _int_set_vector_table(BSP_INTERRUPT_VECTOR_TABLE);

/******************************************************************************
                        Init MQX tick timer
******************************************************************************/
    /* Initialize , set and run system hwtimer */
    result = HWTIMER_SYS_Init(&systimer, &BSP_SYSTIMER_DEV, BSP_SYSTIMER_ID, BSP_SYSTIMER_ISR_PRIOR, NULL);
    if (kStatus_OSA_Success != result) {
        return MQX_INVALID_POINTER;
    }
    result = HWTIMER_SYS_SetFreq(&systimer, BSP_SYSTIMER_SRC_CLK, BSP_ALARM_FREQUENCY);
    if (kStatus_OSA_Success != result) {
        HWTIMER_SYS_Deinit(&systimer);
        return MQX_INVALID_POINTER;
    }
    result = HWTIMER_SYS_RegisterCallback(&systimer,(hwtimer_callback_t)_time_notify_kernel, NULL);
    if (kStatus_OSA_Success != result) {
        HWTIMER_SYS_Deinit(&systimer);
        return MQX_INVALID_POINTER;
    }
    result = HWTIMER_SYS_Start(&systimer);
    if (kStatus_OSA_Success != result) {
        HWTIMER_SYS_Deinit(&systimer);
        return MQX_INVALID_POINTER;
    }
    /* Initialize the system ticks */
    _GET_KERNEL_DATA(kernel_data);
   // kernel_data->TIMER_HW_REFERENCE = (BSP_SYSTEM_CLOCK / BSP_ALARM_FREQUENCY);
    _time_set_ticks_per_sec(BSP_ALARM_FREQUENCY);
    _time_set_hwticks_per_tick(HWTIMER_SYS_GetModulo(&systimer));
    _time_set_hwtick_function(_bsp_get_hwticks, (void *)NULL);

    return MQX_OK;
}

/** Initialization - called from init task, usually for io initialization.
 */
int _bsp_init(void)
{
    uint32_t result = MQX_OK;
    int i;

    /** Cache settings **/
    _DCACHE_ENABLE(0);
    _ICACHE_ENABLE(0);

    /* Disable MPU if present on device. This dirty hack is done to workaround missing MPU_DRV_Disable() function in KSDK */
#ifdef HW_MPU_INSTANCE_COUNT
    for(i = 0; i < HW_MPU_INSTANCE_COUNT; i++)
    {
        MPU_HAL_Disable(g_mpuBaseAddr[i]);
    }
#endif

/******************************************************************************
                    Init gpio platform pins for LEDs
******************************************************************************/
#ifndef PEX_MQX_KSDK
    hardware_init();
#endif

/******************************************************************************
    Install interrupts for UART driver and setup debug console
******************************************************************************/


#if BSPCFG_ENABLE_NSER0 || (BOARD_DEBUG_UART_INSTANCE == 0)
    _nio_dev_install("nser0:", &nio_serial_dev_fn, (void*) &_bsp_nserial0_init);
#endif

#if BSPCFG_ENABLE_NSER1 || (BOARD_DEBUG_UART_INSTANCE == 1)
    _nio_dev_install("nser1:", &nio_serial_dev_fn, (void*) &_bsp_nserial1_init);
#endif

#if BSPCFG_ENABLE_NSER2 || (BOARD_DEBUG_UART_INSTANCE == 2)
    _nio_dev_install("nser2:", &nio_serial_dev_fn, (void*) &_bsp_nserial2_init);
#endif

#if BSPCFG_ENABLE_NSER3 || (BOARD_DEBUG_UART_INSTANCE == 3)
    _nio_dev_install("nser3:", &nio_serial_dev_fn, (void*) &_bsp_nserial3_init);
#endif

#if BSPCFG_ENABLE_NSER4 || (BOARD_DEBUG_UART_INSTANCE == 4)
    _nio_dev_install("nser4:", &nio_serial_dev_fn, (void*) &_bsp_nserial4_init);
#endif

#if BSPCFG_ENABLE_NSER5 || (BOARD_DEBUG_UART_INSTANCE == 5)
    _nio_dev_install("nser5:", &nio_serial_dev_fn, (void*) &_bsp_nserial5_init);
#endif

    /* Install and open dummy drivers for stdin, stdou and stderr. */
    _nio_dev_install("dummy:",&nio_dummy_dev_fn, (NULL));
    open("dummy:", 0);  // 0 - stdin
    open("dummy:", 0);  // 1 - stdout
    open("dummy:", 0);  // 2 - stderr



    /* Instal and set tty driver */
    _nio_dev_install("tty:", &nio_tty_dev_fn, (void*)&(NIO_TTY_INIT_DATA_STRUCT){BSP_DEFAULT_IO_CHANNEL, 0});
    close(0);
    open("tty:", NIO_TTY_FLAGS_EOL_RN | NIO_TTY_FLAGS_ECHO);  // 0 - stdin
    close(1);
    open("tty:", NIO_TTY_FLAGS_EOL_RN | NIO_TTY_FLAGS_ECHO);  // 1 - stdout
    close(2);
    open("tty:", NIO_TTY_FLAGS_EOL_RN | NIO_TTY_FLAGS_ECHO);  // 2 - stderr

/******************************************************************************
    Initialize ENET and install interrupts
******************************************************************************/
/* Check if enet is available */
#if (defined(HW_ENET_INSTANCE_COUNT) && !defined(PEX_MQX_KSDK))
{
    /* ENET module */
    configure_enet_pins(0);

#if 0 //DM Done by ENET driver
    /* Open ENET clock gate */
    CLOCK_SYS_EnableEnetClock(0);
#endif
    /* Select the ptp timer  outclk */
    CLOCK_HAL_SetSource(SIM_BASE, kClockTimeSrc, 2);

    extern void ENET_DRV_TxIRQHandler(void);
    extern void ENET_DRV_RxIRQHandler(void);
    extern void ENET_DRV_TsIRQHandler(void);
    INT_ISR_FPTR ret_temp;

    ret_temp = _int_install_isr(ENET_Transmit_IRQn,(INT_ISR_FPTR)ENET_DRV_TxIRQHandler, NULL);
    assert(NULL != ret_temp);
    NVIC_SetPriority (ENET_Transmit_IRQn, 4);
    NVIC_EnableIRQ(ENET_Transmit_IRQn);

    ret_temp = _int_install_isr(ENET_Receive_IRQn,(INT_ISR_FPTR)ENET_DRV_RxIRQHandler, NULL);
    assert(NULL != ret_temp);
    NVIC_SetPriority (ENET_Receive_IRQn, 4);
    NVIC_EnableIRQ(ENET_Receive_IRQn);

#if FSL_FEATURE_ENET_SUPPORT_PTP
    ret_temp = _int_install_isr(ENET_1588_Timer_IRQn,(INT_ISR_FPTR)ENET_DRV_TsIRQHandler, NULL);
    assert(NULL != ret_temp);
    NVIC_SetPriority (ENET_Receive_IRQn, 4);
    NVIC_EnableIRQ(ENET_Receive_IRQn);
#endif
}
#endif

#if BSPCFG_ENABLE_IO_SUBSYSTEM
    /* Initialize the I/O Sub-system */
    result = _io_init();
    if (result != MQX_OK) {
        return result;
    }

#endif /* BSPCFG_ENABLE_IO_SUBSYSTEM */

    return result;
}

/**FUNCTION********************************************************************
*
* Function Name    : _bsp_exit_handler
* Returned Value   : void
* Comments         :
*   This function is called when MQX exits
*
*END**************************************************************************/
void _bsp_exit_handler(void)
{
    _DCACHE_DISABLE();
    _ICACHE_DISABLE();
}

/**FUNCTION********************************************************************
 *
 * Function Name    : _bsp_get_hwticks
 * Returned Value   : none
 * Comments         :
 *    This function returns the number of hw ticks that have elapsed
 * since the last interrupt
 *
 *END**************************************************************************/

static uint32_t _bsp_get_hwticks(void *param) {
    hwtimer_time_t time;      //struct for storing time
    HWTIMER_SYS_GetTime(&systimer, &time);
    return time.subTicks;
}
