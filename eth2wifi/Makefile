#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := eth2wifi

EXTRA_COMPONENT_DIRS := $(abspath ../bsp)

# obbligatorio
EXCLUDE_COMPONENTS := mdns

# inutili e lenti
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) coap
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) esp_adc_cal
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) esp_http_client
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) expat
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) nghttp
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) openssl
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) spiffs
#EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) esp32 -> wpa_supplicant
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) esp-tls
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) esp_https_ota
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) fatfs 
EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) libsodium
#EXCLUDE_COMPONENTS := $(EXCLUDE_COMPONENTS) bootloader_support -> mbedtls

include $(IDF_PATH)/make/project.mk

