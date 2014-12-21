#####################################################
# This files add all hal and drivers to include path
#####################################################

HAL_INCLUDES := $(dir $(wildcard $(SDK_ROOT)/platform/hal/*/))

SYS_DRIVER_INCLUDES := $(SDK_ROOT)/platform/system/clock/ \
                       $(SDK_ROOT)/platform/system/clock/src \
                       $(SDK_ROOT)/platform/system/hwtimer \
                       $(SDK_ROOT)/platform/system/interrupt \
                       $(SDK_ROOT)/platform/system/power \
                       $(SDK_ROOT)/platform/system/power/src \

DRIVER_INCLUDES := $(dir $(wildcard $(SDK_ROOT)/platform/drivers/*/)) \
                   $(wildcard $(SDK_ROOT)/platform/drivers/*/common/) \
                   $(SDK_ROOT)/platform/drivers/dspi/dspi_master \
                   $(SDK_ROOT)/platform/drivers/dspi/dspi_slave \
                   $(SDK_ROOT)/platform/drivers/i2c/i2c_master \
                   $(SDK_ROOT)/platform/drivers/i2c/i2c_slave \
                   $(SDK_ROOT)/platform/drivers/spi/spi_master \
                   $(SDK_ROOT)/platform/drivers/spi/spi_slave

OSA_INCLUDES := $(SDK_ROOT)/platform/osa

INCLUDES += $(HAL_INCLUDES) $(SYS_DRIVER_INCLUDES) $(DRIVER_INCLUDES) $(OSA_INCLUDES)

