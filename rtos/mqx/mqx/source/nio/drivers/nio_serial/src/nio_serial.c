/*HEADER**********************************************************************
 *
 * Copyright 2013 Freescale Semiconductor, Inc.
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
 *
 *END************************************************************************/

#include <stdarg.h>
#include <stdint.h>
#include <assert.h>
#include <fsl_uart_driver.h>
#include <fsl_os_abstraction.h>

#include "nio_serial.h"
#include "nio.h"
#include "ioctl.h"
#include "errno.h"

extern void UART0_RX_TX_IRQHandler(void);
extern void UART1_RX_TX_IRQHandler(void);
extern void UART2_RX_TX_IRQHandler(void);
extern void UART3_RX_TX_IRQHandler(void);
extern void UART4_RX_TX_IRQHandler(void);
extern void UART5_RX_TX_IRQHandler(void);
extern void UART6_RX_TX_IRQHandler(void);
extern void UART7_RX_TX_IRQHandler(void);

#if defined ( __IAR_SYSTEMS_ICC__ )
/* MISRA C 2004 rule 20.5 suppress: Error[Pm101]: The error indicator errno shall not be used */
_Pragma ("diag_suppress= Pm101")
#endif

static int nio_serial_open(void *dev_context, const char *dev_name, int flags, void **fp_context);
static int nio_serial_read(void *dev_context, void *fp_context, void *buf, size_t nbytes);
static int nio_serial_write(void *dev_context, void *fp_context, const void *buf, size_t nbytes);
static off_t nio_serial_lseek(void *dev_context, void *fp_context, off_t offset, int whence);
static int nio_serial_ioctl(void *dev_context, void *fp_context, unsigned long int request, va_list ap);
static int nio_serial_close(void *dev_context, void *fp_context);
static int nio_serial_init(void *init_data, void **dev_context);
static int nio_serial_deinit(void *dev_context);

const NIO_DEV_FN_STRUCT nio_serial_dev_fn =
{
    .OPEN = nio_serial_open,
    .READ = nio_serial_read,
    .WRITE = nio_serial_write,
    .LSEEK = nio_serial_lseek,
    .IOCTL = nio_serial_ioctl,
    .CLOSE = nio_serial_close,
    .INIT = nio_serial_init,
    .DEINIT = nio_serial_deinit,
};

typedef struct
{
    uart_state_t uart_state;
    uint32_t instance;
} NIO_SERIAL_DEV_CONTEXT_STRUCT;

typedef struct
{
    size_t rcnt;
    size_t wcnt;
} NIO_SERIAL_FP_CONTEXT_STRUCT;

static int nio_serial_open(void *dev_context, const char *dev_name, int flags, void **fp_context)
{
    return 0;
}

static int nio_serial_read(void *dev_context, void *fp_context, void *buf, size_t nbytes)
{

    NIO_SERIAL_DEV_CONTEXT_STRUCT *serial_dev_context = (NIO_SERIAL_DEV_CONTEXT_STRUCT *)dev_context;
    if (kStatus_UART_Success != UART_DRV_ReceiveDataBlocking(serial_dev_context->instance, buf, nbytes, OSA_WAIT_FOREVER))
    {
        errno = EIO;
    }

    return (nbytes - serial_dev_context->uart_state.rxSize);
}

static int nio_serial_write(void *dev_context, void *fp_context, const void *buf, size_t nbytes)
{

    NIO_SERIAL_DEV_CONTEXT_STRUCT *serial_dev_context = (NIO_SERIAL_DEV_CONTEXT_STRUCT *)dev_context;

    if (kStatus_UART_Success != UART_DRV_SendDataBlocking(serial_dev_context->instance, buf, nbytes, OSA_WAIT_FOREVER))
    {
        errno = EIO;
    }

    return (nbytes - serial_dev_context->uart_state.txSize);
}

static off_t nio_serial_lseek(void *dev_context, void *fp_context, off_t offset, int whence)
{
    return 0;
}

static int nio_serial_ioctl(void *dev_context, void *fp_context, unsigned long int request, va_list ap) {

    NIO_SERIAL_DEV_CONTEXT_STRUCT *serial_dev_context = (NIO_SERIAL_DEV_CONTEXT_STRUCT *)dev_context;

    switch (request) {
    case IOCTL_ABORT:
        UART_DRV_AbortSendingData(serial_dev_context->instance);
        UART_DRV_AbortReceivingData(serial_dev_context->instance);
        break;
    default:
        break;
    }

    return 0;
}

static int nio_serial_close(void *dev_context, void *fp_context)
{
    return 0;
}

static int nio_serial_init(void *init_data, void **dev_context)
{
    osa_status_t status;

    NIO_SERIAL_DEV_CONTEXT_STRUCT *serial_dev_context = (NIO_SERIAL_DEV_CONTEXT_STRUCT *)dev_context;

    NIO_SERIAL_INIT_DATA_STRUCT *init = (NIO_SERIAL_INIT_DATA_STRUCT*)init_data;

    assert(init->UART_INSTANCE < HW_UART_INSTANCE_COUNT);

    uart_user_config_t uartConfig =
    {
        .baudRate = init->BAUDRATE,
        .parityMode = init->PARITY_MODE,
        .stopBitCount = init->STOPBIT_COUNT,
        .bitCountPerChar = init->BITCOUNT_PERCHAR,
    };

    serial_dev_context = (NIO_SERIAL_DEV_CONTEXT_STRUCT*) OSA_MemAlloc(sizeof(NIO_SERIAL_DEV_CONTEXT_STRUCT));

    /* SDK HAL init */
    if ( kStatus_UART_Success != UART_DRV_Init(init->UART_INSTANCE, &serial_dev_context->uart_state, &uartConfig))
    {
        errno = ENXIO;
    }
    serial_dev_context->instance = init->UART_INSTANCE;
    
    /* UART handler interrupt installation */
    switch (init->UART_INSTANCE)
    {
    #if (0 < HW_UART_INSTANCE_COUNT)
    case 0:
        status = OSA_InstallIntHandler(UART0_RX_TX_IRQn, UART0_RX_TX_IRQHandler);
        if (kStatus_OSA_Success != status)
        {
            errno = ENXIO;
        }
        NVIC_SetPriority(UART0_RX_TX_IRQn, init->RXTX_PRIOR);
        NVIC_EnableIRQ(UART0_RX_TX_IRQn);
        break;
    #endif
    #if (1 < HW_UART_INSTANCE_COUNT)
    case 1:
        status = OSA_InstallIntHandler(UART1_RX_TX_IRQn, UART1_RX_TX_IRQHandler);
        if (kStatus_OSA_Success != status)
        {
            errno = ENXIO;
        }
        NVIC_SetPriority(UART1_RX_TX_IRQn, init->RXTX_PRIOR);
        NVIC_EnableIRQ(UART1_RX_TX_IRQn);
        break;
    #endif
    #if (2 < HW_UART_INSTANCE_COUNT)
    case 2:
        status = OSA_InstallIntHandler(UART2_RX_TX_IRQn, UART2_RX_TX_IRQHandler);
        if (kStatus_OSA_Success != status)
        {
            errno = ENXIO;
        }
        NVIC_SetPriority(UART2_RX_TX_IRQn, init->RXTX_PRIOR);
        NVIC_EnableIRQ(UART2_RX_TX_IRQn);
        break;
    #endif
    #if (3 < HW_UART_INSTANCE_COUNT)
    case 3:
        status = OSA_InstallIntHandler(UART3_RX_TX_IRQn, UART3_RX_TX_IRQHandler);
        if (kStatus_OSA_Success != status)
        {
            errno = ENXIO;
        }
        NVIC_SetPriority(UART3_RX_TX_IRQn, init->RXTX_PRIOR);
        NVIC_EnableIRQ(UART3_RX_TX_IRQn);
        break;
    #endif
    #if (4 < HW_UART_INSTANCE_COUNT)
    case 4:
        status = OSA_InstallIntHandler(UART4_RX_TX_IRQn, UART4_RX_TX_IRQHandler);
        if (kStatus_OSA_Success != status)
        {
            errno = ENXIO;
        }
        NVIC_SetPriority(UART4_RX_TX_IRQn, init->RXTX_PRIOR);
        NVIC_EnableIRQ(UART4_RX_TX_IRQn);
        break;
    #endif
    #if (5 < HW_UART_INSTANCE_COUNT)
    case 5:
        status = OSA_InstallIntHandler(UART5_RX_TX_IRQn, UART5_RX_TX_IRQHandler);
        if (kStatus_OSA_Success != status)
        {
            errno = ENXIO;
        }
        NVIC_SetPriority(UART5_RX_TX_IRQn, init->RXTX_PRIOR);
        NVIC_EnableIRQ(UART5_RX_TX_IRQn);
        break;
    #endif
    #if (6 < HW_UART_INSTANCE_COUNT)
    case 6:
        status = OSA_InstallIntHandler(UART6_RX_TX_IRQn, UART6_RX_TX_IRQHandler);
        if (kStatus_OSA_Success != status)
        {
            errno = ENXIO;
        }
        NVIC_SetPriority(UART6_RX_TX_IRQn, init->RXTX_PRIOR);
        NVIC_EnableIRQ(UART6_RX_TX_IRQn);
        break;
    #endif
    #if (7 < HW_UART_INSTANCE_COUNT)
    case 7:
        status = OSA_InstallIntHandler(UART7_RX_TX_IRQn, UART7_RX_TX_IRQHandler);
        if (kStatus_OSA_Success != status)
        {
            errno = ENXIO;
        }
        NVIC_SetPriority(UART7_RX_TX_IRQn, init->RXTX_PRIOR);
        NVIC_EnableIRQ(UART7_RX_TX_IRQn);
        break;
    #endif
    default:
        /* Unsupported UART instance */
        break;
    }

    *dev_context = (void*)serial_dev_context;

    return 0;
}

static int nio_serial_deinit(void *dev_context)
{
    NIO_SERIAL_DEV_CONTEXT_STRUCT *serial_dev_context = (NIO_SERIAL_DEV_CONTEXT_STRUCT *)dev_context;

    UART_DRV_Deinit(serial_dev_context->instance);
    OSA_MemFree(dev_context);
    return 0;
}
