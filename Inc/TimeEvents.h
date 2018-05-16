/*
 * TimeEvents.h
 *
 *  Created on: 23 sept. 2017
 *      Author: juanra
 */

#ifndef TIMEALERTS_H_
#define TIMEALERTS_H_

#include "Definitions.h"

// typedef	 int  (* Action) (void);

// so far type is always 1 : PERIODIC
class CTimeEvent {
	public:
		CTimeEvent(const char *_id, long _value, int _tipo, unsigned short _times, unsigned int _period, Action _handler);
		int					Run			() { return this->handler();};
	    int					Check		();

	    static CTimeEvent	*Add		(CTimeEvent *);
	    static int			Clear		(const char *id);
	    static int			Schedule	();
			
	private:
		const char		*id;
		long			value;
		int				type;
		unsigned short	times;
		unsigned int	period;
		Action			handler;
static	CTimeEvent		*alerts[100];
};




#endif /* TIMEALERTS_H_ */
