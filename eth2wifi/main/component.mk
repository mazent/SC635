#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_EXTRA_CLEAN := versione.h

appv.o: versione.h

versione.h:
	$(COMPONENT_PATH)/versione.sh

$(call compile_only_if,$(CONFIG_USA_MS),appv.o)
$(call compile_only_if_not,$(CONFIG_USA_MS),app.o)

# CPPFLAGS += -DLWIP_NETIF_STATUS_CALLBACK=1

# vedi conf.h (che imposta valori predefiniti se mancano le macro)
#CPPFLAGS += -DBRIDGE=0
#CPPFLAGS += -DBRIDGE_DHCP_CLN=0
#CPPFLAGS += -DSTAMPA_REGISTRI=1
#CPPFLAGS += -DSTAMPA_WIFI=1
#CPPFLAGS += -DSTAMPA_ETH=1
