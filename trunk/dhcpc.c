/*----------------------------------------------------------------------------
 Copyright:      Michael Kleiber
 Author:         Michael Kleiber
 Remarks:        
 known Problems: none
 Version:        29.11.2008
 Description:    DHCP Client

 Dieses Programm ist freie Software. Sie können es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation veröffentlicht, 
 weitergeben und/oder modifizieren, entweder gemäß Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder späteren Version. 

 Die Veröffentlichung dieses Programms erfolgt in der Hoffnung, 
 daß es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 FÜR EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
------------------------------------------------------------------------------*/
#include "config.h"

#ifdef USE_DHCP
#include "ethernet.h"
#include "dhcpc.h"
#include "udp.h"
#include "eemem.h"

struct dhcp_msg
{
  unsigned char op;				//1		Message op code / message type
								//		1 = BOOTREQUEST, 2 = BOOTREPLY
  unsigned char htype;			//2		Hardware address type, see ARP section in "Assigned
								//		Numbers" RFC; e.g., '1' = 10mb ethernet.
  unsigned char hlen;			//3		Hardware address length (e.g.  '6' for 10mb ethernet).
  unsigned char hops;			//4		Client sets to zero, optionally used by relay agents
								//		when booting via a relay agent.
  unsigned char xid[4];			//8		Transaction ID, a random number chosen by the
								//		client, used by the client and server to associate
								//		messages and responses between a client and a server.
  unsigned int  secs;			//12	Filled in by client, seconds elapsed since client
								//		began address acquisition or renewal process.
  unsigned int  flags;			//14	LSB = BROADCAST flag
  unsigned char ciaddr[4];		//16	Client IP address; only filled in if client is in
								//		BOUND, RENEW or REBINDING state and can respond
								//		to ARP requests.
  unsigned long yiaddr;			//20	'your' (client) IP address.
  unsigned long siaddr;			//24	IP address of next server to use in bootstrap;
								//		returned in DHCPOFFER, DHCPACK by server.
  unsigned char giaddr[4];		//28	Relay agent IP address, used in booting via a
								//		relay agent.
  unsigned char chaddr[16];		//44	Client hardware address (MAC)
  unsigned char sname[64];		//108	Optional server host name, null terminated string.
  unsigned char file[128];		//236 Up to here. Boot file name, null terminated string; "generic"
								//		name or null in DHCPDISCOVER, fully qualified
								//		directory-path name in DHCPOFFER.
  unsigned char options[312];	//		Optional parameters field. 312 is minimum length requested
								//		by RFC2131
};

struct dhcp_cache
{
  unsigned char type;
  unsigned char ovld;
  unsigned long router_ip;
  unsigned long dns1_ip;
  unsigned long dns2_ip;
  unsigned long netmask;
  unsigned long lease;
  unsigned long serv_id;
};


//DHCP Message types for Option 53
#define DHCPDISCOVER   1     // client -> server
#define DHCPOFFER      2     // server -> client
#define DHCPREQUEST    3     // client -> server
#define DHCPDECLINE    4     // client -> server
#define DHCPACK        5     // server -> client
#define DHCPNAK        6     // server -> client
#define DHCPRELEASE    7     // client -> server
#define DHCPINFORM     8     // client -> server

#define DHCP_STATE_IDLE             0
#define DHCP_STATE_DISCOVER_SENT    1
#define DHCP_STATE_OFFER_RCVD       2
#define DHCP_STATE_SEND_REQUEST     3
#define DHCP_STATE_REQUEST_SENT     4
#define DHCP_STATE_ACK_RCVD         5
#define DHCP_STATE_NAK_RCVD         6
#define DHCP_STATE_ERR              7
#define DHCP_STATE_FINISHED         8

unsigned char dhcp_state;
struct dhcp_cache cache; 
volatile unsigned long dhcp_lease;
volatile unsigned char dhcp_timer;

//----------------------------------------------------------------------------
//Init of DHCP client port
void dhcp_init (void)
{
  //Port in Anwendungstabelle eintragen für eingehende DHCP Daten!
  dhcp_res.nStat = 0;
  UDP_RegisterSocket (DHCP_CLIENT_PORT, (void(*)(unsigned char))dhcp_get);
  dhcp_state = DHCP_STATE_IDLE;
  dhcp();
  return;
}
//----------------------------------------------------------------------------
// Configure this client by DHCP
unsigned char dhcp (void)
{
  unsigned char timeout_cnt;
  
  timeout_cnt = 0;

	if ( (dhcp_state == DHCP_STATE_FINISHED ) &&
       (dhcp_lease < 600)                         )
  {
      dhcp_state = DHCP_STATE_SEND_REQUEST;
  } 
	
  do
  {
    if ( timeout_cnt > 3 )
    {
      dhcp_state = DHCP_STATE_ERR;
#if DEBUG_AV
	putpgmstring("DHCP timeout\r\n");
#endif	
      return (1);
    }
    switch (dhcp_state)
    {
      case DHCP_STATE_IDLE:
        dhcp_message(DHCPDISCOVER);
        dhcp_timer = 5;
		break;
      
	  case DHCP_STATE_DISCOVER_SENT:
        if (dhcp_timer == 0) 
        {
          dhcp_state = DHCP_STATE_IDLE;
          timeout_cnt++;
        }
		break;
      
	  case DHCP_STATE_OFFER_RCVD:
        timeout_cnt = 0;
				dhcp_state = DHCP_STATE_SEND_REQUEST;
		break;
      
	  case DHCP_STATE_SEND_REQUEST:
        dhcp_timer  = 5;
        dhcp_message(DHCPREQUEST);
		break;
      
	  case DHCP_STATE_REQUEST_SENT:
        if (dhcp_timer == 0) 
        {
          dhcp_state = DHCP_STATE_SEND_REQUEST;
          timeout_cnt++;
        }
		break;
      
	  case DHCP_STATE_ACK_RCVD:
 //       DHCP_DEBUG("LEASE %2x%2x%2x%2x\r\n", cache.lease[0],cache.lease[1],cache.lease[2],cache.lease[3]);
#if DEBUG_AV
	putpgmstring("DHCP DHCP_STATE_ACK_RCVD\r\n");
	puthexbyte(mlIP>>0);
	puthexbyte(mlIP>>8);
	puthexbyte(mlIP>>16);
	puthexbyte(mlIP>>24);
	putpgmstring("\r\n");
#endif	
        dhcp_lease = cache.lease;
		// write to eeprom only if data has changed, we don't want to wear it out
		if (dhcp_res.btStat.bStatIP == 1)
            eeprom_write_dword(&mlIpEEP, mlIP);
		if (dhcp_res.btStat.bStatNetmask== 1)
            eeprom_write_dword(&mlNetmaskEEP, mlNetmask);
		if (dhcp_res.btStat.bStatGateway== 1)
            eeprom_write_dword(&mlGatewayEEP, mlGateway);
		if (dhcp_res.btStat.bStatDNSserver== 1)
            eeprom_write_dword(&mlDNSserverEEP, mlDNSserver);
		dhcp_res.nStat = 0;
        dhcp_state = DHCP_STATE_FINISHED;
		break;
      
	  case DHCP_STATE_NAK_RCVD:
        dhcp_state = DHCP_STATE_IDLE;
		break;
		
    }
    
	eth_packet_dispatcher();
	
  }
  while ( dhcp_state != DHCP_STATE_FINISHED );
  
  return(0);
  
}

//----------------------------------------------------------------------------
//Sendet DHCP messages an Broadcast
void dhcp_message (unsigned char type)
{
  struct dhcp_msg *msg;
  unsigned char   *options;
  
  for (unsigned int i=0; i < sizeof (struct dhcp_msg); i++) //clear eth_buffer to 0
  {
    ethernetbuffer[UDP_DATA_START+i] = 0;
  }
  
  msg = (struct dhcp_msg *)&ethernetbuffer[UDP_DATA_START];
  msg->op          = 1; // BOOTREQUEST
  msg->htype       = 1; // Ethernet
  msg->hlen        = 6; // Ethernet MAC
  msg->xid[0]      = mlMAC[5]; //use the MAC as the ID to be unique in the LAN
  msg->xid[1]      = mlMAC[4];
  msg->xid[2]      = mlMAC[3];
  msg->xid[3]      = mlMAC[2];
  msg->flags       = htons(0x8000);
  msg->chaddr[0]   = mlMAC[0];
  msg->chaddr[1]   = mlMAC[1];
  msg->chaddr[2]   = mlMAC[2];
  msg->chaddr[3]   = mlMAC[3];
  msg->chaddr[4]   = mlMAC[4];
  msg->chaddr[5]   = mlMAC[5];
  
  options = &msg->options[0];  //options
  *options++       = 99;       //magic cookie
  *options++       = 130;
  *options++       = 83;
  *options++       = 99;

  *options++       = 53;    // Option 53: DHCP message type DHCP Discover
  *options++       = 1;     // len = 1
  *options++       = type;  // 1 = DHCP Discover
  
  *options++       = 55;    // Option 55: parameter request list
  *options++       = 3;     // len = 3
  *options++       = 1;     // netmask
  *options++       = 3;     // router
  *options++       = 6;     // dns

  *options++       = 50;    // Option 54: requested IP
  *options++       = 4;     // len = 4
  *options++       = ((unsigned char *)&mlIP)[0];
  *options++       = ((unsigned char *)&mlIP)[1];
  *options++       = ((unsigned char *)&mlIP)[2];
  *options++       = ((unsigned char *)&mlIP)[3];

  switch (type)
  {
    case DHCPDISCOVER:
      dhcp_state = DHCP_STATE_DISCOVER_SENT;
#if DEBUG_AV
	putpgmstring("DISCOVER sent\r\n");
#endif	
    break;
    case DHCPREQUEST:
      *options++       = 54;    // Option 54: server ID
      *options++       = 4;     // len = 4
      *options++       = ((unsigned char *)&cache.serv_id)[0];
      *options++       = ((unsigned char *)&cache.serv_id)[1];
      *options++       = ((unsigned char *)&cache.serv_id)[2];
      *options++       = ((unsigned char *)&cache.serv_id)[3];
      dhcp_state = DHCP_STATE_REQUEST_SENT;
#if DEBUG_AV
	putpgmstring("REQUEST sent\r\n");
#endif	
    break;
    default:
#if DEBUG_AV
	putpgmstring("Wrong DHCP msg type\r\n");
#endif	
    break;
  }

  *options++       = 12;    // Option 12: host name
  *options++       = 8;     // len = 8
  *options++       = 'M';
  *options++       = 'i';
  *options++       = 'n';
  *options++       = 'i';
  *options++       = '-';
  *options++       = 'A';
  *options++       = 'V';
  *options++       = 'R';
  
  *options         = 0xff;  //end option

  UDP_SendPacket(sizeof (struct dhcp_msg),DHCP_CLIENT_PORT,DHCP_SERVER_PORT,(unsigned long)0xffffffff);
}
//----------------------------------------------------------------------------
//liest 4 bytes aus einem buffer und speichert in dem anderen
void get4bytes (unsigned char *source, unsigned char *target)
{
  unsigned char i;
  
  for (i=0; i<4; i++)
  {
    *target++ = *source++;
  }
}
//----------------------------------------------------------------------------
//read all the options
//pointer to the variables and size from options to packet end
void dhcp_parse_options (unsigned char *msg, struct dhcp_cache *c, unsigned int size)
{
  unsigned int ix;

  ix = 0;
  do
  {
    switch (msg[ix])
    {
      case 0: //Padding
      ix++;
      break;
      case 1: //Netmask
        ix++;
        if ( msg[ix] == 4 )
        {
          ix++;
		  c->netmask = *((unsigned long *)(msg+ix));
          ix += 4;
        }
        else
        {
          ix += (msg[ix]+1);
        }
      break;
      case 3: //router (gateway IP)
        ix++;
        if ( msg[ix] == 4 )
        {
          ix++;
		  c->router_ip = *((unsigned long *)(msg+ix));
          ix += 4;
        }
        else
        {
          ix += (msg[ix]+1);
        }
      break;
      case 6: //dns len = n * 4
        ix++;
        if ( msg[ix] == 4 )
        {
          ix++;
		  c->dns1_ip = *((unsigned long *)(msg+ix));
          ix += 4;
        }
        else
        if ( msg[ix] == 8 )
        {
          ix++;
		  c->dns1_ip = *((unsigned long *)(msg+ix));
          ix += 4;
 		  c->dns2_ip = *((unsigned long *)(msg+ix));
          ix += 4;
        }
        else
        {
          ix += (msg[ix]+1);
        }
      break;
      case 51: //lease time
        ix++;
        if ( msg[ix] == 4 )
        {
          ix++;
 		  c->lease = *((unsigned long *)(msg+ix));
          ix += 4;
        }
        else
        {
          ix += msg[ix]+1;
        }
      break;
      case 52: //Options overload 
        ix++;
        if ( msg[ix] == 1 )   //size == 1
        {
          ix++;
          c->ovld   = msg[ix];
          ix++;
        }
        else
        {
          ix += (msg[ix]+1);
        }
      break;
      case 53: //DHCP Type 
        ix++;
        if ( msg[ix] == 1 )   //size == 1
        {
          ix++;
          c->type = msg[ix];
          ix++;
        }
        else
        {
          ix += (msg[ix]+1);
        }
      break;
      case 54: //Server identifier
        ix++;
        if ( msg[ix] == 4 )
        {
          ix++;
 		  c->serv_id = *((unsigned long *)(msg+ix));
          ix += 4;
        }
        else
        {
          ix += msg[ix]+1;
        }
      break;
      case 99:   //Magic cookie
        ix += 4;
      break;
      case 0xff: //end option
      break;
      default: 
#if DEBUG_AV
	putpgmstring("Unknown Option: ");
	puthexbyte(msg[ix]);
	putpgmstring("\r\n");
#endif	
        ix++;
        ix += (msg[ix]+1);
      break;
    }
  }
  while ( (msg[ix] != 0xff) && (ix < size) ); 
}

//----------------------------------------------------------------------------
//Wertet message vom DHCP Server aus
// DHCP packets: 20 Bytes IP Header, 8 Bytes UDP_Header,
// DHCP fixed fields 236 Bytes, min 312 Bytes options -> 576 Bytes min.
void dhcp_get (void)
{
  struct dhcp_msg  *msg;
  struct IP_header *ip;
  unsigned char *p;

  ip  = (struct IP_header *)&ethernetbuffer[ETH_HDR_LEN];
  if ( htons(ip->IP_Totallenght) > MTU_SIZE )
  {
#if DEBUG_AV
	putpgmstring("DHCP too big, discarded\r\n");
#endif	
    return;
  }

  p = &cache.type; //clear the cache
  for (uint8_t i=0; i<sizeof(cache); i++)
  {
    p[i] = 0;
  }

  // set pointer of DHCP message to beginning of UDP data
  msg = (struct dhcp_msg *)&ethernetbuffer[UDP_DATA_START];

  //check the id
  if ( (msg->xid[0] != mlMAC[5]) ||
       (msg->xid[1] != mlMAC[4]) ||
       (msg->xid[2] != mlMAC[3]) ||
       (msg->xid[3] != mlMAC[2])    )
  {
#if DEBUG_AV
	putpgmstring("Wrong DHCP ID, discarded\r\n");
#endif	
    return;
  }


  dhcp_parse_options(&msg->options[0], &cache, (htons(ip->IP_Totallenght)-264) );
  // check if file field or sname field are overloaded (option 52)
  switch (cache.ovld) 
  {
    case 0:  // no overload, do nothing
    break;
    case 1:  // the file field contains options
      dhcp_parse_options(&msg->file[0], &cache, 128);
    break;
    case 2:  // the sname field contains options
      dhcp_parse_options(&msg->sname[0], &cache, 64);
    break;
    case 3:  // the file and the sname field contain options
      dhcp_parse_options(&msg->file[0], &cache, 128);
      dhcp_parse_options(&msg->sname[0], &cache, 64);
    break;
    default: // must not occur
#if DEBUG_AV
	putpgmstring("Option 52 Error\r\n");
#endif	
    break;
  }

  switch (cache.type)
  {
    case DHCPOFFER:
      // this will be our IP address
		if (dhcp_state != DHCP_STATE_DISCOVER_SENT)
		{
			// we are not waiting for a dhcp offer 
			// this happens for example when there are more than one dhcp server answering)
#if DEBUG_AV
			putpgmstring("Received 2nd DHCPOFFER\r\n");
#endif				
			return;
		}
		if ( mlIP != msg->yiaddr)
		{
			mlIP = msg->yiaddr;
			dhcp_res.btStat.bStatIP = 1;
		}
		if (mlNetmask != cache.netmask)
		{
			mlNetmask = cache.netmask;
			dhcp_res.btStat.bStatNetmask= 1;
		}
		if (mlGateway != cache.router_ip)
		{
			mlGateway = cache.router_ip;
			dhcp_res.btStat.bStatGateway= 1;
		}
		if (mlDNSserver != cache.dns1_ip)
		{
			mlDNSserver = cache.dns1_ip;
			dhcp_res.btStat.bStatDNSserver= 1;
		}
#if DHCP_PARSE_TFTP_PARAMS		
		if (msg->siaddr)
		{
			mlTFTPip = msg->siaddr;
			dhcp_res.btStat.bStatTFTPserver = 1;
		}
		if (msg->file[0])
		{
			unsigned char *f;
			uint8_t i;
			f = msg->file;
			i = 0;
			while (*f && i<(TFTP_FILENAME_SIZE-1))
			{
				msTFTPfileName[i++] = *(f++);
			}
			msTFTPfileName[i] = 0;
			dhcp_res.btStat.bStatTFTPfileName = 1;
#if DEBUG_AV
	putstring(msTFTPfileName);
	putpgmstring(" <- TFTP filename received\r\n");
#endif				
		}
#endif

#if DEBUG_AV
	putpgmstring("** DHCP OFFER RECVD! **\r\n");
#endif	
      dhcp_state = DHCP_STATE_OFFER_RCVD;
    break;
    case DHCPACK:
#if DEBUG_AV
	putpgmstring("** DHCP ACK RECVD! **\r\n");
#endif	
      dhcp_state = DHCP_STATE_ACK_RCVD;
    break;
    case DHCPNAK:
#if DEBUG_AV
	putpgmstring("** DHCP NAK RECVD! **\r\n");
#endif	
      dhcp_state = DHCP_STATE_NAK_RCVD;
    break;
  }
}

#endif
