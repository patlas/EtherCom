/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 2004-2008 Embedded Access Inc.
* Copyright 1989-2008 ARC International
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
*   This file contains the RTCS shell.
*
*
*END************************************************************************/

#include <ctype.h>
#include <string.h>
#include <mqx.h>
#include "shell.h"
#include "sh_prv.h"

#include "nio.h"
#include "fcntl.h"
#include "fs_supp.h"

#if SHELLCFG_USES_RTCS

#include <rtcs.h>
#include "sh_rtcs.h"
#include <tftp.h>
        
         
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name :  Shell_TFTP_client
*  Returned Value:  none
*  Comments  :  SHELL utility to TFTP to or from a host
*  Usage:  tftp host get source [destination] [mode] 
*
*END*-----------------------------------------------------------------*/

int32_t  Shell_TFTP_client(int32_t argc, char *argv[] )
{
   _ip_address          hostaddr;
   char                 hostname[MAX_HOSTNAMESIZE]="";
   char             *file_ptr;
   uint32_t              tftp_handle, buffer_size,byte_number;
   unsigned char            *buffer_ptr;
   TFTP_DATA_STRUCT     tftp_data;
   int fd;
   int32_t               error;
   bool              print_usage, shorthelp = FALSE;
   int32_t               return_code = SHELL_EXIT_SUCCESS;
   char                 path[SHELL_MAX_FILELEN];  
   bool              trans = FALSE;
   SHELL_CONTEXT_PTR shell_ptr = Shell_get_context(argv);
   
   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
   
      if ((argc >= 3) && (argc <= 5))  {
         RTCS_resolve_ip_address( argv[1], &hostaddr, hostname, MAX_HOSTNAMESIZE ); 

         if (!hostaddr)  {
            fprintf(shell_ptr->STDOUT, "Unable to resolve host.\n");
            return_code = SHELL_EXIT_ERROR;
         } else  {
            tftp_data.SERVER   = hostaddr;
            tftp_data.FILENAME = argv[2];
            tftp_data.FILEMODE = "netascii";
            if (argc > 3)  {
               file_ptr = argv[3];
               if (argc > 4) {
                  tftp_data.FILEMODE = argv[4];
               } else {
                  tftp_data.FILEMODE = "netascii";
               }
            } else {
               file_ptr = argv[2];
            }
#if SHELLCFG_USES_MFS  
            
            Shell_create_prefixed_filename(path, file_ptr, argv);
            fd = open(path, O_CREAT | O_RDWR | O_TRUNC);
            if (fd != -1)  {
               fprintf(shell_ptr->STDOUT, "\nDownloading file %s from TFTP server: %s [%ld.%ld.%ld.%ld]\n",
                  tftp_data.FILENAME,hostname, IPBYTES(hostaddr));
               tftp_handle = TFTP_open( (void *) &tftp_data );
               if ( tftp_handle != RTCS_OK )  {
                  fprintf(shell_ptr->STDOUT, "\nError opening file %s\n",tftp_data.FILENAME);
                  return_code = SHELL_EXIT_ERROR;
               } else  {
                if (!TFTP_eof())  {
                   do {
                     buffer_ptr = TFTP_read( &buffer_size );
                     if ((buffer_ptr != NULL) && (buffer_size))  {
                        lseek(fd, 0 , SEEK_CUR);
                        write(fd, buffer_ptr, buffer_size); 
                        trans = TRUE;
                     } else {
                   
                         switch (buffer_size) {
                         case 0:
                            // end of file
                           break;
                         case (RTCSERR_TFTP_ERROR + 1):
                            fprintf(shell_ptr->STDOUT, "\nFile %s not found\n", tftp_data.FILENAME);
                            break;
                         case (RTCSERR_TFTP_ERROR + 2):
                            fprintf(shell_ptr->STDOUT, "\nAccess violation\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 3):
                            fprintf(shell_ptr->STDOUT, "\nDisk full or allocation exceeded\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 4):
                            fprintf(shell_ptr->STDOUT, "\nIllegal TFTP operation\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 5):
                            fprintf(shell_ptr->STDOUT, "\nUnknown transfer ID\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 6):
                            fprintf(shell_ptr->STDOUT, "\nFile already exists\n");
                            break;
                         case (RTCSERR_TFTP_ERROR + 7):
                            fprintf(shell_ptr->STDOUT, "\nNo such user\n");
                            break;
                         default:
                            if(trans) 
                            {
                              trans =FALSE;
                              break;
                            }
                          else
                            fprintf(shell_ptr->STDOUT, "\nError reading file %s\n", tftp_data.FILENAME);
                         } /* Endswitch */
                        }
                     } while((buffer_ptr !=NULL) && buffer_size &&  (!TFTP_eof()) );
                     close(fd);
                  }
                  
                  TFTP_close();
                  
               }
               
            } else  {
               fprintf(shell_ptr->STDOUT, "\nError opening local file %s\n",file_ptr);
               return_code = SHELL_EXIT_ERROR;
            }
#else
            tftp_handle = TFTP_open( (void *) &tftp_data );
            if ( tftp_handle != RTCS_OK )  {
               fprintf(shell_ptr->STDOUT, "\nError opening file %s\n",tftp_data.FILENAME);
               return_code = SHELL_EXIT_ERROR;
            } else  {
               fprintf(shell_ptr->STDOUT, "SHELLCFG_USES_MFS is not set to 1 in user_config.h - file wont be written to disk\n");
            }
            TFTP_close();
#endif            
         }
      } else  {
         fprintf(shell_ptr->STDOUT, "Error, %s invoked with incorrect number of arguments\n", argv[0]);
         print_usage = TRUE;
      }
   }
   
   if (print_usage)  {
      if (shorthelp)  {
         fprintf(shell_ptr->STDOUT, "%s <host> <source> [<dest>] [<mode>]\n", argv[0]);
      } else  {
         fprintf(shell_ptr->STDOUT, "Usage: %s <host> <source> [<dest>] [<mode>]\n", argv[0]);
         fprintf(shell_ptr->STDOUT, "   <host>   = host ip address or name\n");
         fprintf(shell_ptr->STDOUT, "   <source> = remote file name\n");
         fprintf(shell_ptr->STDOUT, "   <dest>   = local file name\n");
         fprintf(shell_ptr->STDOUT, "   <mode>   = file transfer mode (netascii, etc.)\n");
      }
   }
   return return_code;
} /* Endbody */

#endif /* SHELLCFG_USES_RTCS */

/* EOF */
