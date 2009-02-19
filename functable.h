#ifndef _FUNCTABLE_H_
#define _FUNCTABLE_H_


uint16_t func_table[] __attribute__((section(".func_table"))) = {
    (uint16_t)(enc28j60ReadOp),
	(uint16_t)(enc28j60WriteOp),
	(uint16_t)(enc28j60ReadBuffer),
	(uint16_t)(enc28j60WriteBuffer),
	(uint16_t)(enc28j60SetBank),
	(uint16_t)(enc28j60Read),
	(uint16_t)(enc28j60Write),
	(uint16_t)(enc28j60PhyRead),
	(uint16_t)(enc28j60PhyWrite),
	(uint16_t)(enc28j60Init),
	(uint16_t)(enc28j60PacketSend),
	(uint16_t)(enc28j60PacketReceive),
	(uint16_t)(enc28j60PacketReceiveLenght),
}; 

#define ENC28J60_READ_OP(op, address) ((unsigned char(*)(unsigned char, unsigned char)) pgm_read_word(&func_table[0])) (op, address)
#define ENC28J60_WRITE_OP(op, address, data) ((void(*)(unsigned char, unsigned char, unsigned char)) pgm_read_word(&func_table[1])) (op, address, data)
#define ENC28J60_READ_BUFFER(len, data) ((void(*)(unsigned int, unsigned char*)) pgm_read_word(&func_table[2])) (len, data)
#define ENC28J60_WRITE_BUFFER(len, data) ((void(*)(unsigned int, unsigned char*)) pgm_read_word(&func_table[3])) (len, data)
#define ENC28J60_SET_BANK(address) ((void(*)(unsigned char)) pgm_read_word(&func_table[4])) (address)
#define ENC28J60_READ(address) ((unsigned char(*)(unsigned char)) pgm_read_word(&func_table[5])) (address)
#define ENC28J60_WRITE(address, data) ((void(*)(unsigned char, unsigned char)) pgm_read_word(&func_table[6])) (address, data)
#define ENC28J60_PHY_READ(address) ((unsigned int(*)(unsigned char)) pgm_read_word(&func_table[7])) (address)
#define ENC28J60_PHY_WRITE(address, data) ((void(*)(unsigned char, unsigned int)) pgm_read_word(&func_table[8])) (address, data)
#define ENC28J60_INIT() ((void(*)(void)) pgm_read_word(&func_table[9])) ()
#define ENC28J60_PACKET_SEND(len, packet) ((void(*)(unsigned int, unsigned char*)) pgm_read_word(&func_table[10])) (len, packet)
#define ENC28J60_PACKET_RECEIVE(maxlen, packet) ((unsigned int(*)(unsigned int, unsigned char*)) pgm_read_word(&func_table[11])) (maxlen, packet)
#define ENC28J60_PACKET_RECEIVE_LENGTH() ((unsigned int(*)(void)) pgm_read_word(&func_table[12])) ()

#endif
