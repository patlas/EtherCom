/*HEADER**********************************************************************
*
* Copyright 20013 Freescale Semiconductor, Inc.
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
*
*   This file contains the definition for the baud rate for the serial
*   channel
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "nio_serial.h"

#if (0 < HW_UART_INSTANCE_COUNT)
const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial0_init =
{
        .UART_INSTANCE       = 0,
        .BAUDRATE            = 115200,
        .PARITY_MODE         = kUartParityDisabled,
        .STOPBIT_COUNT       = kUartOneStopBit,
        .BITCOUNT_PERCHAR    = kUart8BitsPerChar,
        .RXTX_PRIOR          = 4,
};
#endif

#if (1 < HW_UART_INSTANCE_COUNT)
const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial1_init =
{
        .UART_INSTANCE       = 1,
        .BAUDRATE            = 115200,
        .PARITY_MODE         = kUartParityDisabled,
        .STOPBIT_COUNT       = kUartOneStopBit,
        .BITCOUNT_PERCHAR    = kUart8BitsPerChar,
        .RXTX_PRIOR          = 4,
};
#endif

#if (2 < HW_UART_INSTANCE_COUNT)
const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial2_init =
{
        .UART_INSTANCE       = 2,
        .BAUDRATE            = 115200,
        .PARITY_MODE         = kUartParityDisabled,
        .STOPBIT_COUNT       = kUartOneStopBit,
        .BITCOUNT_PERCHAR    = kUart8BitsPerChar,
        .RXTX_PRIOR          = 4,
};
#endif

#if (3 < HW_UART_INSTANCE_COUNT)
const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial3_init =
{
        .UART_INSTANCE       = 3,
        .BAUDRATE            = 115200,
        .PARITY_MODE         = kUartParityDisabled,
        .STOPBIT_COUNT       = kUartOneStopBit,
        .BITCOUNT_PERCHAR    = kUart8BitsPerChar,
        .RXTX_PRIOR          = 4,
};
#endif

#if (4 < HW_UART_INSTANCE_COUNT)
const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial4_init =
{
        .UART_INSTANCE       = 4,
        .BAUDRATE            = 115200,
        .PARITY_MODE         = kUartParityDisabled,
        .STOPBIT_COUNT       = kUartOneStopBit,
        .BITCOUNT_PERCHAR    = kUart8BitsPerChar,
        .RXTX_PRIOR          = 4,
};
#endif

#if (5 < HW_UART_INSTANCE_COUNT)
const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial5_init =
{
        .UART_INSTANCE       = 5,
        .BAUDRATE            = 115200,
        .PARITY_MODE         = kUartParityDisabled,
        .STOPBIT_COUNT       = kUartOneStopBit,
        .BITCOUNT_PERCHAR    = kUart8BitsPerChar,
        .RXTX_PRIOR          = 4,
};
#endif
