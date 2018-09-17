#include "bsp.h"
#include "conf.h"


#define STAMPA_FRAME 		(STAMPA_WIFI + STAMPA_ETH + STAMPA_BR)


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


typedef struct {
	uint8_t IHL: 4,
			ver: 4 ;

	uint8_t ECN: 2,
			DSCP: 6 ;

	// girare
	uint16_t len ;

	// girare
	uint16_t ident ;

	// girare
	uint16_t ofs: 13,
			 flags: 3 ;

	uint8_t TTL ;
	uint8_t proto ;

	// girare
	uint16_t hcsum ;

	uint32_t mit ;
	uint32_t dst ;

} IPv4_HEADER ;


#if STAMPA_FRAME > 0
static void stampa_frame(const char * t, bool in, const void * p, int dim)
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

	if (in) {
		ESP_LOGI(TAG, "%s <- [%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %s",
				t, dim, MAC2STR(pF->srg), MAC2STR(pF->dst), tipo) ;

	}
	else {
		ESP_LOGI(TAG, "%s -> [%d] %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X %s",
				t, dim, MAC2STR(pF->srg), MAC2STR(pF->dst), tipo) ;

	}

	if (0x0800 == et) {
		uint8_t * d = (uint8_t *) p ;
		int j = 1 ;
		printf("    ") ;
		for (int i=0 ; i<dim ; i++, d++, j++) {
			printf("%02X ", *d) ;
			if (j == 16) {
				printf("\n    ") ;
				j = 0 ;
			}
		}
		printf("\n") ;
//		const uint8_t * ip = (const uint8_t *) p ;
//		ip += sizeof(ETH_FRAME) ;
//
//		ESP_LOGI(TAG, "    %02X %02X %02X %02X", ip[0], ip[1], ip[2], ip[3]) ;
//		ESP_LOGI(TAG, "    %02X %02X %02X %02X", ip[4], ip[5], ip[6], ip[7]) ;
//		ESP_LOGI(TAG, "    %02X %02X %02X %02X", ip[8], ip[9], ip[10], ip[11]) ;
//		ESP_LOGI(TAG, "    %02X %02X %02X %02X", ip[12], ip[13], ip[14], ip[15]) ;
//		ESP_LOGI(TAG, "    %02X %02X %02X %02X", ip[16], ip[17], ip[18], ip[19]) ;
//
//		// giro le parti storte
//		IPv4_HEADER h ;
//
//		memcpy(&h, ip, sizeof(h)) ;
//
//		h.hcsum = gira(h.hcsum) ;
//
//		uint16_t * pg = &h.len ;
//		for (int i=0 ; i<3 ; i++, pg++)
//			*pg = gira(*pg) ;
//
//
////		ESP_LOGI(TAG, "IPv4_FRAME = %d", sizeof(IPv4_FRAME)) ;
//
//		ESP_LOGI(TAG, "    ver %d IHL = %d DSCP = %d ECN = %d len = %d", h.ver, h.IHL, h.DSCP, h.ECN, h.len) ;
//		ESP_LOGI(TAG, "    ident %d flags %d frag %d", h.ident, h.flags, h.ofs) ;
//		ESP_LOGI(TAG, "    ttl %d proto %d csum %d", h.TTL, h.proto, h.hcsum) ;
//		ESP_LOGI(TAG, "    mit %08X", h.mit) ;
//		ESP_LOGI(TAG, "    dst %08X", h.dst) ;
//
//		ip += sizeof(uint32_t) * h.IHL ;
//
//		if (6 == h.proto) {
//			ip += sizeof(uint32_t) * h.IHL ;
//			const int DIM = h.len - sizeof(uint32_t) * h.IHL ;
//			printf("    ") ;
//			int i, j ;
//			for (i=0, j=0 ; i<DIM ; i++, j++, ip++) {
//				printf("%02X ", *ip) ;
//				if (j == 16) {
//					printf("\n    ") ;
//					j = 0 ;
//				}
//			}
//			printf("\n") ;
//		}
	}
}
#endif

#if STAMPA_WIFI > 0

void stampa_wifi(bool in, const void * p, int dim)
{
	stampa_frame("WF", in, p, dim) ;
}

#endif

#if STAMPA_ETH > 0

void stampa_eth(bool in, const void * p, int dim)
{
	stampa_frame("ET", in, p, dim) ;
}

#endif

#if STAMPA_BR > 0

void stampa_br(bool in, const void * p, int dim)
{
	stampa_frame("BR", in, p, dim) ;
}

#endif

