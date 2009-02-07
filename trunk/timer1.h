/*! \file timer1.h \brief Timer1 functions */
///	\ingroup hardware
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
//@{

#ifndef _TIMER1_H

	#define _TIMER1_H
	
	//#include "../config.h"
	
	#define SECDIV 1000

	typedef void ( * TIMER1_CALLBACK_FUNC ) (void);
	
	#define MAX_TIMER1_CALLBACKS		4
	#define MAX_TIMER1_COUNTDOWNTIMER	4
		
	void timer1_init (void);
	void timer1_deinit(void);

#ifndef FALSE
	#define FALSE				0
#endif
#ifndef TRUE
	#define TRUE				(!FALSE)
#endif

#ifndef NULL
	#define NULL				0
#endif

	#define COUNDOWNTIMER_UNUSE	65535

    extern TIMER1_CALLBACK_FUNC TIMER1_msec_CallbackFunc;
	
#endif /* _TIMER_H */
//@}
