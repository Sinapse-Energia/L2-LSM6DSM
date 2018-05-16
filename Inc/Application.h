/*
 * Apllication.h
 *
 *  Created on: 26 sept. 2017
 *      Author: juanra
 */

#ifndef APLLICATION_H_
#define APLLICATION_H_




extern	"C"	int		SendMeassurement(void);		//	helper function to trigger a Meassurement message sending
extern	"C" int		SetProgramm		(void);		//	action to be called when a 2; command is processed
extern	"C" int		SetDimming		(void);		//	action to be called when a 3; command is processed
extern	"C" int		SetPeriodics	(void);		//	action to be called when a 4; command is processed
extern	"C"	int		ApplyDimming	(void);		//	action to be used for set the timmer to the programmed value 
extern	"C"	int		GetDimming		(long t);	//	helper function to get the dimming value corresponding to any time





#endif /* APLLICATION_H_ */
