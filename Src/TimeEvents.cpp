/*
 * TimeEvents.cpp
 *
 *  Created on: 23 sept. 2017
 *      Author: juanra
 */


#include <stdio.h>
#include <string.h>
#include "TimeEvents.h"
#include "NBinterface.h"
#include "utils.h"

#define SECPERDAY	(24*60*60)

// array of pointers to CTimeEvent objects...
CTimeEvent		*CTimeEvent::alerts[100] = {0};


// Object Constructor
CTimeEvent::CTimeEvent(const char *_id, long _value, int _type, unsigned short  _times, unsigned int _period, Action _handler){
	long now = GetTimeStamp();
	id = _id;
	value = _value + (now / SECPERDAY) * SECPERDAY;
	type = _type;
	times = _times;
	period = _period;
	handler = _handler;
};

// Object method to check if the timer is due, and in case, to activate if (call the handler anr reload if so)
int	CTimeEvent::Check(){
	long now = GetTimeStamp();
	if (now > this->value) {
		if (type == 1) {
			if (this->times){
				this->times--;
				this->value += this->period;
				this->handler();
				return 1;
			}
			else 
				return 0; // run out, but is still in the list....
		}
		else 
			return 0; // other types not supported

	}
	else return 0;
}



// Class  method to call Check in every object in the list
int		CTimeEvent::Schedule(){
	int i = 0;
	int result = 0;
	while (i < sizeof(alerts)/sizeof(*alerts)-1){
		if (alerts[i]){
			result  += alerts[i]->Check();
		}
		i++;
	}
	return result;
}


// Class  method to add a new object in the list 
CTimeEvent	*CTimeEvent::Add(CTimeEvent *al){
	unsigned int i = 0;
	while (i < sizeof(alerts)/sizeof(*alerts)-1){
		if (alerts[i] == NULL) {
			alerts[i] = al;
			return al;
		}
		else
			i++;
	}
	return NULL;
}


// Class  method to delete the objects in the list by id tag  (NO repack- bacuase all the loops are until end)
int		CTimeEvent::Clear(const char *tag){
	unsigned int i = 0;
	unsigned int hits = 0;
	while (i < sizeof(alerts)/sizeof(*alerts)-1){
		if (alerts[i] &&  !strcmp(alerts[i]->id, tag)) {
			delete alerts[i];
			alerts[i] = NULL;
			hits++;
			
		}
		i++;
	}
	return hits;
}

// Timer to execute everyday, at a certain moment
int DoAt(Action action, unsigned int h, unsigned int m, unsigned int s  ){
	long timestamp = ((h * 60) + m )* 60 + s;   // timestamp to be executed
	unsigned int maxtimes = (unsigned int) -1;  // a lot of days... maybe could be enough a month?
	CTimeEvent *timer = new CTimeEvent("AT", timestamp, 1 , maxtimes, SECPERDAY, action);
	CTimeEvent::Add(timer);
	return 1;
}





// C linkage function to give a way to run the scheduler form C code 
int		Schedule(){
	return CTimeEvent::Schedule();
}
