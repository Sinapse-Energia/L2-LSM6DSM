/*
 * Application.cpp
 *
 *  Created on: 26 sept. 2017
 *      Author: juanra
 */



#include "context.h"
#include "NBinterface.h"
#include "TimeEvents.h"
#include "utils.h"
#define SECPERDAY	(24*60*60)


#include "Application.h"

int		SetProgramm() {
	CVarlist *all = GetListVar ("PROGR"); 
	unsigned int i = 0;
	CTimeEvent::Clear("PR");
	while (i < all->size()) {
		CVartuple *step = (CVartuple * )all->Item(i);
		CVarint  *t1 = (CVarint *) step->Item(0);
		CVartime *t2 = (CVartime *) step->Item(1);

		if (t1 >0) { // skips -1 legacy padding
			unsigned int secondsaday = 24 * 60 * 60;	// number of seconds of a day
			unsigned int maxtimes = (unsigned int) -1;  // a lot of days... maybe could be enough a month?


			CTimeEvent *timer = new CTimeEvent("PR", t2->Timestamp(), 1 , maxtimes, secondsaday, ApplyDimming);
			CTimeEvent::Add(timer);
		}
		i++;
	}
	return i;
}


int		SetDimming() {
	// reset any active programation, i.e.  "PR" timers
	CTimeEvent::Clear("PR");
	// trigger the funct to save in NVM
	SaveConnParams();
	return 1;

}


int		SetPeriodics() {
	// Recover commmad parameters
	CVarint		*Periodo =	GetIntVar("MPERIOD");
	CVarint		*Veces =	GetIntVar("MPTIMES");
	CVartime	*Fecha1 =	GetTimeVar("MPBEGIN");
	CVartime	*Fecha2 =	GetTimeVar("MPEND");

	// evaluate them
	int periodo =	Periodo->Value();
	int veces =		Veces->Value();
	long TS1 =		Fecha1->Timestamp();
	long TS2 =		Fecha2->Timestamp();  //.....//
	long now = 		GetTimeStamp();

	int number1; 	// number of periods, after HM1 and HM2
	unsigned int number;	// final number of periods
	if (periodo && veces){
		if (TS2 < TS1){
			// First Time bigger than second....
			// IGNORE COMMAND:  return 0;
			return -1;
		}

		SaveAppParams();

		// normalize not at relative TS

		now = now % SECPERDAY;

		if (TS1 < now) { // if the time splice is in progress
			//  TRUNCATE  TS1 = now ;
			TS1 = now;
		}
		// Calculate number of periods
		number1 = (TS2 -TS1)/(periodo * 60);

		if (number1 > 0) {
			if (veces > 0 && veces < number1) {
				number = veces;
			}
			else {
				number = number1;
			}
		//	number = ((veces < -0) || number1 < veces)? number1: veces;


			CTimeEvent *timer = new CTimeEvent("PM", TS1, 1 , number, periodo * 60, SendMeassurement);
			int dx = CTimeEvent::Clear("PM");
			CTimeEvent::Add(timer);
			return 1;
		}
		else
			return 0;
	}
	else
		return -1;
}


int GetDimming (long t) {
	CVarlist *progr = GetListVar("PROGR");
	unsigned short i = 0;
	unsigned int lastvalue = -1;
	while (i < progr->size()) {
		CVartuple *step = (CVartuple * ) progr->Item(i);
		CVarint  *st1 = (CVarint *) step->Item(0);
		CVartime *st2 = (CVartime *) step->Item(1);
		if (st2->Timestamp() > t){
			break;
		}
		else {
			lastvalue = st1->Value();
			i++;
		}
	}			
	return lastvalue;
}

//	action to be used for set the timmer to the programmed value 
int		ApplyDimming	(void) {
	// get the Dimming for rigth now
	int value = GetDimming(GetTimeStamp());
	char	txt[8];
	itoa(value, txt, 10);
	SetVariable("PWM", txt);
	return value;
}



