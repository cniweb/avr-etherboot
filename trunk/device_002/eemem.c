#include "eemem.h"

unsigned char EEMEM maMacEEP[6] = {0x02, 0x01, 0x02, 0x03, 0x04, 0x22};
unsigned long EEMEM mlIpEEP = IP(192,168,2,132);
unsigned long EEMEM mlNetmaskEEP = IP(255,255,255,0);
unsigned long EEMEM mlGatewayEEP = IP(192,168,2,2);
unsigned long EEMEM mlDNSserverEEP = IP(192,168,2,2); //0x0302a8c0;
//uint8_t EEMEM miDeviceID = 001;
unsigned char EEMEM maTFTPReqStr[12] = {0x00, 0x01, '0', '0', '2', 0x00, 'o', 'c', 't', 'e', 't', 0x00};
