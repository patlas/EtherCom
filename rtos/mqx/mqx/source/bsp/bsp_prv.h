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
*
*   This file contains the definitions of constants and structures
*   required for initialization of the board.
*
*END************************************************************************/
#ifndef __bsp_prv_h__
#define __bsp_prv_h__ 1

#include "nio_serial.h"
#include "fsl_port_hal.h"
#include "fsl_sim_hal.h"
#include "fsl_clock_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial0_init;
extern const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial1_init;
extern const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial2_init;
extern const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial3_init;
extern const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial4_init;
extern const NIO_SERIAL_INIT_DATA_STRUCT _bsp_nserial5_init;

/* FUNCTION PROTOTYPES */
extern void  _bsp_exit_handler(void);

/* STRUCTURE DEFINITIONS */

#ifdef __cplusplus
}
#endif

#endif /* __bsp_prv_h__ */

