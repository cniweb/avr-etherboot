/* test

	*** INT2

*/

#ifndef _INT2_H
	#define _INT2_H


	#ifndef FALSE
		#define FALSE	0
		#define TRUE	(!FALSE)
	#endif
	
	#ifndef NULL
		#define NULL	0
	#endif



	typedef void ( * INT2_CALLBACK_FUNC ) ( void );
	
	
	
	#ifdef __AVR_ATmega2561__

		#define MAX_INT2_CALLBACKS		5
		
		// Beim Mega 2561 alle 8 externen Interrupts anbieten
	
		#define INT_TYPE_FALLING_EDGE 1
		#define INT_TYPE_RISING_EDGE  2
		#define INT_TYPE_LOW_LEVEL    3
	
		void INT_init (uint8_t piINTBitMask);
		unsigned char INT_RemoveCallbackFunction (INT2_CALLBACK_FUNC pFunc);
		unsigned char INT_RegisterCallbackFunction (uint8_t piINTBitMask, uint8_t piINTType, INT2_CALLBACK_FUNC pFunc);

		void INT_enable_mask (uint8_t piINTBitMask);
		void INT_enable_bit (uint8_t piINTBit);
		void INT_disable_bit (uint8_t piINTBit);
		uint8_t INT_enabledBitMask (void);

	#else
	#ifdef __AVR_ATmega644__

		#define MAX_INT2_CALLBACKS		5
		
		// Beim Mega 2561 alle 8 externen Interrupts anbieten
	
		#define INT_TYPE_FALLING_EDGE 1
		#define INT_TYPE_RISING_EDGE  2
		#define INT_TYPE_LOW_LEVEL    3
	
		void INT_init (uint8_t piINTBitMask);
		unsigned char INT_RemoveCallbackFunction (INT2_CALLBACK_FUNC pFunc);
		unsigned char INT_RegisterCallbackFunction (uint8_t piINTBitMask, uint8_t piINTType, INT2_CALLBACK_FUNC pFunc);

		void INT_enable_mask (uint8_t piINTBitMask);
		void INT_enable_bit (uint8_t piINTBit);
		void INT_disable_bit (uint8_t piINTBit);
		uint8_t INT_enabledBitMask (void);
	
	#else
		
		#define MAX_INT2_CALLBACKS		2
		
		void INT2_init(void);
		unsigned char INT2_RemoveCallbackFunction( INT2_CALLBACK_FUNC pFunc );
		unsigned char INT2_RegisterCallbackFunction( INT2_CALLBACK_FUNC pFunc );

		void INT2_disable(void);
		void INT2_enable(void);
		
	#endif
	#endif
	
#endif /* _INT2_H */
