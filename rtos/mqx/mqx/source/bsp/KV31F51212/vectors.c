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
*    This file contains the exception vector table and flash configuration.
*
*
*END************************************************************************/

#include "mqx_inc.h"
#include "bsp.h"

extern unsigned long __BOOT_STACK_ADDRESS[];

/* Configuration values for flash controller */
#define CONFIG_1        (uint32_t)0xffffffff
#define CONFIG_2        (uint32_t)0xffffffff
#define CONFIG_3        (uint32_t)0xffffffff
#define CONFIG_4        (uint32_t)0xfffffffe

#define BOOT_START __boot

extern void __boot(void);

#ifndef __CC_ARM
extern vector_entry __VECTOR_TABLE_ROM_START[]; /* defined in linker command file */
extern vector_entry __VECTOR_TABLE_RAM_START[]; /* defined in linker command file */
#endif

void _svc_handler(void);
void _pend_svc(void);

#ifdef __ICCARM__
    #pragma language = extended
    #pragma segment  = "CSTACK"

    #pragma location = ".flashx"
    #pragma segment  = ".flashx"
    /* Workaround to define __FLASHX_START_ADDR at the beginning of .flashx */
    const unsigned char __FLASHX_START_ADDR[1];
    /* Here you can put anything to your flashx area */

    #if BSPCFG_ENABLE_CFMPROTECT
        #pragma location = ".cfmconfig"
        #pragma segment  = ".cfmconfig"
        const uint32_t _cfm[4] = {
            CONFIG_1,
            CONFIG_2,
            CONFIG_3,
            CONFIG_4
        };
    #endif /* BSPCFG_ENABLE_CFMPROTECT */
#elif defined(__CC_ARM) || defined(__GNUC__)
    /* flashx location defined in io/flashx/freescale/flash_mk60.h for KEIL */
    __attribute__((section(".cfmconfig"))) const uint32_t _cfm[4] __attribute__((used)) = {
        CONFIG_1,
        CONFIG_2,
        CONFIG_3,
        CONFIG_4
    };
#else /* CodeWarrior compiler assumed */
    #pragma define_section flashx    ".flashx" ".flashx" ".flashx"  far_abs R
    /* Here you can put anything to your flashx area */

    #if BSPCFG_ENABLE_CFMPROTECT
        /* Pragma to place the flash configuration field on correct location defined in linker file. */
        #pragma define_section cfmconfig ".cfmconfig" ".cfmconfig" ".cfmconfig" far_abs R
        #pragma explicit_zero_data on
        __declspec(cfmconfig)  uint32_t _cfm[4] = {
            CONFIG_1,
            CONFIG_2,
            CONFIG_3,
            CONFIG_4
        };
    #endif /* BSPCFG_ENABLE_CFMPROTECT */
#endif /* CodeWarrior compiler assumed */

typedef union { vector_entry __fun; void * __ptr; } intvec_elem;

#if MQX_ROM_VECTORS
    #define DEFAULT_VECTOR  _int_kernel_isr
#else
    static void __boot_exception(void) {
        while(1);
    }

    #define DEFAULT_VECTOR  __boot_exception

    #if   defined(__ICCARM__)
        #pragma language = extended
        #pragma location = ".vectors_ram"
        #pragma segment  = ".vectors_ram"
        intvec_elem ram_vector[256] @ ".vectors_ram" =
    #elif defined(__CC_ARM) || defined(__GNUC__)
        __attribute__((section(".vectors_ram"))) vector_entry ram_vector[256] __attribute__((used)) =
    #else /* CodeWarrior compiler assumed */
        #pragma  define_section vectors_ram ".vectors_ram" far_abs RW
        /*
         * Array for exception vectors in ram + space (6 words)
         * for CW (when CW debugger handles exceptions, using(rewrite)
         * VBR+0x408 address
         */
        __declspec(vectors_ram) vector_entry ram_vector[256 + 6] =
    #endif /* CodeWarrior compiler assumed */
    {
        (vector_entry)__BOOT_STACK_ADDRESS,
        BOOT_START,         /* 0x01  0x00000004   -   ivINT_Initial_Program_Counter     */
        _int_kernel_isr,    /* 0x02  0x00000008   -   ivINT_NMI                         */
        _int_kernel_isr,    /* 0x03  0x0000000C   -   ivINT_Hard_Fault                  */
        _int_kernel_isr,    /* 0x04  0x00000010   -   ivINT_Mem_Manage_Fault            */
        _int_kernel_isr,    /* 0x05  0x00000014   -   ivINT_Bus_Fault                   */
        _int_kernel_isr,    /* 0x06  0x00000018   -   ivINT_Usage_Fault                 */
        0,                  /* 0x07  0x0000001C   -   ivINT_Reserved7                   */
        0,                  /* 0x08  0x00000020   -   ivINT_Reserved8                   */
        0,                  /* 0x09  0x00000024   -   ivINT_Reserved9                   */
        0,                  /* 0x0A  0x00000028   -   ivINT_Reserved10                  */
        _svc_handler,       /* 0x0B  0x0000002C   -   ivINT_SVCall                      */
        _int_kernel_isr,    /* 0x0C  0x00000030   -   ivINT_DebugMonitor                */
        0,                  /* 0x0D  0x00000034   -   ivINT_Reserved13                  */
        _pend_svc,          /* 0x0E  0x00000038   -   ivINT_PendableSrvReq              */
        _int_kernel_isr     /* 0x0F  0x0000003C   -   ivINT_SysTick                     */
    };
#endif
/*
** exception vector table - this table is really used
*/
#ifdef __ICCARM__
    #pragma language = extended
    #pragma segment  = "CSTACK"

    #pragma location = ".intvec"
    #pragma segment  = ".intvec"
    const intvec_elem __vector_table[] =
#elif defined(__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".vectors_rom"))) const vector_entry __vector_table[256] __attribute__((used)) =
#else /* CodeWarrior compiler assumed */
    #pragma  define_section vectors_rom ".vectors_rom" ".vectors_rom" ".vectors_rom" far_abs R
    __declspec(vectors_rom) vector_entry rom_vector[] =
#endif /* CodeWarrior compiler assumed */
{
    (vector_entry)__BOOT_STACK_ADDRESS,
    BOOT_START,             /* 0x01  0x00000004   -   ivINT_Initial_Program_Counter     */
    DEFAULT_VECTOR,         /* 0x02  0x00000008   -   ivINT_NMI                         */
    DEFAULT_VECTOR,         /* 0x03  0x0000000C   -   ivINT_Hard_Fault                  */
    DEFAULT_VECTOR,         /* 0x04  0x00000010   -   ivINT_Mem_Manage_Fault            */
    DEFAULT_VECTOR,         /* 0x05  0x00000014   -   ivINT_Bus_Fault                   */
    DEFAULT_VECTOR,         /* 0x06  0x00000018   -   ivINT_Usage_Fault                 */
    DEFAULT_VECTOR,         /* 0x07  0x0000001C   -   ivINT_Reserved7                   */
    DEFAULT_VECTOR,         /* 0x08  0x00000020   -   ivINT_Reserved8                   */
    DEFAULT_VECTOR,         /* 0x09  0x00000024   -   ivINT_Reserved9                   */
    DEFAULT_VECTOR,         /* 0x0A  0x00000028   -   ivINT_Reserved10                  */
    _svc_handler,           /* 0x0B  0x0000002C   -   ivINT_SVCall                      */
    DEFAULT_VECTOR,         /* 0x0C  0x00000030   -   ivINT_DebugMonitor                */
    DEFAULT_VECTOR,         /* 0x0D  0x00000034   -   ivINT_Reserved13                  */
    _pend_svc,              /* 0x0E  0x00000038   -   ivINT_PendableSrvReq              */
    DEFAULT_VECTOR,         /* 0x0F  0x0000003C   -   ivINT_SysTick                     */
    /* Cortex external interrupt vectors                                                */
    DEFAULT_VECTOR,         /**< DMA Channel 0 Transfer Complete */            
    DEFAULT_VECTOR,         /**< DMA Channel 1 Transfer Complete */            
    DEFAULT_VECTOR,         /**< DMA Channel 2 Transfer Complete */            
    DEFAULT_VECTOR,         /**< DMA Channel 3 Transfer Complete */            
    DEFAULT_VECTOR,         /**< Reserved interrupt 20 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 21 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 22 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 23 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 24 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 25 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 26 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 27 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 28 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 29 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 30 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 31 */                      
    DEFAULT_VECTOR,         /**< DMA Error Interrupt */                        
    DEFAULT_VECTOR,         /**< Normal Interrupt */                           
    DEFAULT_VECTOR,         /**< FTFA Command complete interrupt */            
    DEFAULT_VECTOR,         /**< Read Collision Interrupt */                   
    DEFAULT_VECTOR,         /**< Low Voltage Detect, Low Voltage Warning */    
    DEFAULT_VECTOR,         /**< Low Leakage Wakeup */                         
    DEFAULT_VECTOR,         /**< WDOG Interrupt */                             
    DEFAULT_VECTOR,         /**< Reserved Interrupt 39 */                      
    DEFAULT_VECTOR,         /**< I2C0 interrupt */                             
    DEFAULT_VECTOR,         /**< I2C1 interrupt */                             
    DEFAULT_VECTOR,         /**< SPI0 Interrupt */                             
    DEFAULT_VECTOR,         /**< SPI1 Interrupt */                             
    DEFAULT_VECTOR,         /**< Reserved Interrupt 44 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 45 */                      
    DEFAULT_VECTOR,         /**< LPUART0 status/error interrupt */             
    DEFAULT_VECTOR,         /**< UART0 Receive/Transmit interrupt */           
    DEFAULT_VECTOR,         /**< UART0 Error interrupt */                      
    DEFAULT_VECTOR,         /**< UART1 Receive/Transmit interrupt */           
    DEFAULT_VECTOR,         /**< UART1 Error interrupt */                      
    DEFAULT_VECTOR,         /**< UART2 Receive/Transmit interrupt */           
    DEFAULT_VECTOR,         /**< UART2 Error interrupt */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 53 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 54 */                      
    DEFAULT_VECTOR,         /**< ADC0 interrupt */                             
    DEFAULT_VECTOR,         /**< CMP0 interrupt */                             
    DEFAULT_VECTOR,         /**< CMP1 interrupt */                             
    DEFAULT_VECTOR,         /**< FTM0 fault, overflow and channels interrupt */
    DEFAULT_VECTOR,         /**< FTM1 fault, overflow and channels interrupt */
    DEFAULT_VECTOR,         /**< FTM2 fault, overflow and channels interrupt */
    DEFAULT_VECTOR,         /**< Reserved interrupt 61 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 62 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 63 */                      
    DEFAULT_VECTOR,         /**< PIT timer channel 0 interrupt */              
    DEFAULT_VECTOR,         /**< PIT timer channel 1 interrupt */              
    DEFAULT_VECTOR,         /**< PIT timer channel 2 interrupt */              
    DEFAULT_VECTOR,         /**< PIT timer channel 3 interrupt */              
    DEFAULT_VECTOR,         /**< PDB0 Interrupt */                             
    DEFAULT_VECTOR,         /**< Reserved interrupt 69 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 70 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 71 */                      
    DEFAULT_VECTOR,         /**< DAC0 interrupt */                             
    DEFAULT_VECTOR,         /**< MCG Interrupt */                              
    DEFAULT_VECTOR,         /**< LPTimer interrupt */                          
    DEFAULT_VECTOR,         /**< Port A interrupt */                           
    DEFAULT_VECTOR,         /**< Port B interrupt */                           
    DEFAULT_VECTOR,         /**< Port C interrupt */                           
    DEFAULT_VECTOR,         /**< Port D interrupt */                           
    DEFAULT_VECTOR,         /**< Port E interrupt */                           
    DEFAULT_VECTOR,         /**< Software interrupt */                         
    DEFAULT_VECTOR,         /**< Reserved interrupt 81 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 82 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 83 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 84 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 85 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 86 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 87 */                      
    DEFAULT_VECTOR,         /**< Reserved interrupt 88 */                      
    DEFAULT_VECTOR,         /**< ADC1 interrupt */                             
    DEFAULT_VECTOR,         /**< Reserved Interrupt 90 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 91 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 92 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 93 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 94 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 95 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 96 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 97 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 98 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 99 */                      
    DEFAULT_VECTOR,         /**< Reserved Interrupt 100 */                     
    DEFAULT_VECTOR,         /**< Reserved Interrupt 101 */                     
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,         
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR,
    DEFAULT_VECTOR
};
