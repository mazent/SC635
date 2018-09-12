#include "bsp.h"
#include "cfg.h"

#ifndef STAMPA_REGISTRI
#	define STAMPA_REGISTRI		0
#endif

#ifndef STAMPA_WIFI
#	define STAMPA_WIFI		0
#endif

#ifndef STAMPA_ETH
#	define STAMPA_ETH		0
#endif

#define STAMPA_FRAME 		(STAMPA_WIFI + STAMPA_ETH)


static const char * TAG = "bridge";

#if STAMPA_REGISTRI > 0

void stampa_registri(void)
{
    //phy_lan8720_dump_registers() ;
	uint16_t val ;

    ESP_LOGI(TAG, "LAN8720 Registers:");

    val = esp_eth_smi_read(0) ;
    ESP_LOGI(TAG, "    BCR    0x%04X", val);
    ESP_LOGI(TAG, "           %s", val & (1 << 14) ? "loopback" : "normal");
    if (val & (1 << 12))
    	ESP_LOGI(TAG, "           auto-negotiate");
    else {
    	ESP_LOGI(TAG, "           %s", val & (1 << 13) ? "100Mbps" : "10Mbps");
    	ESP_LOGI(TAG, "           %s", val & (1 << 8) ? "full duplex" : "half duplex");
    }
    ESP_LOGI(TAG, "           %s", val & (1 << 10) ? "electrical isolation of PHY from the RMII" : "NO electrical isolation of PHY from the RMII");

    val = esp_eth_smi_read(1) ;
    ESP_LOGI(TAG, "    BSR    0x%04X", val) ;
    ESP_LOGI(TAG, "           T4 able: %s", val & (1 << 15) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           100BASE-TX Full Duplex: %s", val & (1 << 14) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           100BASE-TX Half Duplex: %s", val & (1 << 13) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           10BASE-T Full Duplex : %s", val & (1 << 12) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           10BASE-T Half Duplex : %s", val & (1 << 11) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           100BASE-T2 Full Duplex : %s", val & (1 << 10) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           100BASE-T2 Half Duplex : %s", val & (1 <<  9) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           Auto-Negotiate Complete : %s", val & (1 <<  5) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           Remote Fault : %s", val & (1 <<  4) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           Auto-Negotiate Ability : %s", val & (1 <<  3) ? "SI" : "no") ;
    ESP_LOGI(TAG, "           Link Status : %s", val & (1 <<  2) ? "UP" : "down") ;
    ESP_LOGI(TAG, "           Jabber Detect : %s", val & (1 <<  1) ? "SI" : "no") ;

    ESP_LOGI(TAG, "    PHY1   0x%04X", esp_eth_smi_read(2)) ;
    ESP_LOGI(TAG, "    PHY2   0x%04X", esp_eth_smi_read(3)) ;
    ESP_LOGI(TAG, "    ANAR   0x%04X", esp_eth_smi_read(4));
    ESP_LOGI(TAG, "    ANLPAR 0x%04X", esp_eth_smi_read(5));
    ESP_LOGI(TAG, "    ANER   0x%04X", esp_eth_smi_read(6));
    ESP_LOGI(TAG, "    MCSR   0x%04X", esp_eth_smi_read(17));
    ESP_LOGI(TAG, "    SM     0x%04X", esp_eth_smi_read(18));
    ESP_LOGI(TAG, "    SECR   0x%04X", esp_eth_smi_read(26));
    ESP_LOGI(TAG, "    CSIR   0x%04X", esp_eth_smi_read(27));
    ESP_LOGI(TAG, "    ISR    0x%04X", esp_eth_smi_read(29));
    ESP_LOGI(TAG, "    IMR    0x%04X", esp_eth_smi_read(30));

    val = esp_eth_smi_read(31) ;
    ESP_LOGI(TAG, "    PSCSR  0x%04X", val);
    ESP_LOGI(TAG, "           %s", val & (1 << 12) ? "Auto-negotiation is done" : "Auto-negotiation is not done or disabled (or not active)") ;
    val >>= 2 ;
    val &= 0x7 ;
    switch (val) {
    case 1:
    	ESP_LOGI(TAG, "           10BASE-T half-duplex") ;
    	break ;
    case 5:
    	ESP_LOGI(TAG, "           10BASE-T full-duplex") ;
    	break ;
    case 2:
    	ESP_LOGI(TAG, "           100BASE-TX half-duplex") ;
    	break ;
    case 6:
    	ESP_LOGI(TAG, "           100BASE-TX full-duplex") ;
    	break ;
    default:
    	ESP_LOGE(TAG, "           ? %02X ?", val) ;
    	break ;
    }
}

#endif

typedef struct {
	uint8_t dst[6] ;
	uint8_t srg[6] ;
	uint16_t type ;
} ETH_FRAME ;

static inline uint16_t gira(uint16_t val)
{
	union {
		uint16_t x ;
		uint8_t b[2] ;
	} u ;
	uint8_t tmp ;

	u.x = val ;
	tmp = u.b[0] ;
	u.b[0] = u.b[1] ;
	u.b[1] = tmp ;

	return u.x ;
}

#if STAMPA_FRAME > 0
static void stampa_frame(const char * t, const void * p, int dim)
{
	ETH_FRAME * pF = (ETH_FRAME *) p ;
	const char * tipo = NULL ;
	uint16_t et = gira(pF->type) ;

	switch (et) {
	case 0x0800:
		tipo = "IPv4" ;
		break ;
	case 0x86DD:
		tipo = "IPv6" ;
		break ;
	case 0x0806:
		tipo = "ARP" ;
		break ;
	default: {
			static char xxx[20] ;
			sprintf(xxx, "? %04X ?", et) ;
			tipo = xxx ;
		}
		break ;
	}

	ESP_LOGI(TAG, "%s: [%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %s",
			t, dim, MAC2STR(pF->srg), MAC2STR(pF->dst), tipo) ;
}
#endif

#if STAMPA_WIFI > 0

void stampa_wifi(const void * p, int dim)
{
	stampa_frame("Wifi", p, dim) ;
}

#endif

#if STAMPA_ETH > 0

void stampa_eth(const void * p, int dim)
{
	stampa_frame("ETH", p, dim) ;
}

#endif

