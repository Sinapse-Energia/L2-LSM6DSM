/*
 * parametrizacion.cpp
 *
 *  Created on: 9 ago. 2017
 *      Author: juanra
 */



#include	"context.h"
#include	"Frames.h"
#include 	"Definitions.h"
#include	"NBinterface.h"
#include	"Application.h"
#include	"BLInterface.h"



//		(1) POPULATE the CONTEXT
//		This is the current way to  populate the Context :
//		Declare an static array of pointers to CVariable objects
//			and then, assign it to the static member
int	 CreateContext() {
	static		CVariable		*ALLVARS[] = {
		new CVariable ("Current message",			"MSSG",		new CVarstring () ),
		new CVariable ("Message tag",				"TAG",		new CVarstring () ),
		new CVariable ("Node identifier",			"ID",		new CVarstring () ),
		new CVariable ("Generic result",			"RESULT",	new CVarstring ("OK") ),


// DEFAULT CONNECTION PARAMETERS
		new CVariable ("Server IP address",			"IP",		new CVarstring () ),
		new CVariable ("Server TCP PORT",			"PORT",		new CVarint () ),
		new CVariable ("Username",					"USER",		new CVarstring () ),
		new CVariable ("Password",					"PSWD",		new CVarstring () ),
		new CVariable ("Server security type",		"SEC",		new CVarint () ),
		new CVariable ("Access Point Name",			"APN",		new CVarstring () ),

// CONTINGENCY CONNECTION PARAMETERS
		new CVariable ("Last server IP address",	"LIP",		new CVarstring () ),
		new CVariable ("Last server TCP PORT",		"LPORT",	new CVarint   () ),
		new CVariable ("Last username",				"LUSER",	new CVarstring () ),
		new CVariable ("Last password",				"LPSWD",	new CVarstring () ),
		new CVariable ("Last security type",		"LSEC",		new CVarint () ),
		new CVariable ("Last Access Point Name",	"LAPN",		new CVarstring () ),

// COLLECTED during connection
		new CVariable ("Local IP",					"LOCALIP",	new CVarstring () ),
		new CVariable ("SIM Card Id",				"IDSIM",	new CVarstring () ),
		new CVariable ("Product SN",				"IMEI",		new CVarstring () ),

// APPLICATION VARIABLES
		new CVariable ("Status",					"STAT",		new CVarint () ),
//		new CVariable ("Dimming status",			"DSTAT",	new CVarint () ),   // obsolete, now renamed to PWM

		new CVariable ("Temperature",				"TEMP",		new CVarint () ),
		new CVariable ("Voltage",					"V",		new CVarint () ),
		new CVariable ("Current",					"A",		new CVarint () ),
		new CVariable ("Active power",				"P",		new CVardec () ),
		new CVariable ("Reactive power",			"Q",		new CVardec () ),
		new CVariable ("Apparent power",			"S",		new CVardec () ),
		new CVariable ("Aggregated active energy",	"EP",		new CVardec () ),
		new CVariable ("Aggregated reactive energy","EQ",		new CVardec () ),
		new CVariable ("Aggregated apparent energy","ES",		new CVardec () ),
		new CVariable ("Frequency",					"F",		new CVardec () ),

// Periodic Meassurement Context entries
		new CVariable ("Meassurement Period",		"MPERIOD",	new CVarint	 () ),
		new CVariable ("Periodic measurement begin","MPBEGIN",	new CVartime () ),
		new CVariable ("Periodic measurement end",	"MPEND",	new CVartime () ),
		new CVariable ("Periodic measurement days", "MPTIMES",	new CVarint  () ),

		new CVariable ("Profile programation","PROGR",			new CVarlist (10, new CVartuple(new CVarint(), new CVartime()) )),

// DATA ITEMS to be shared with BOOTLOADER
		new CVariable ("Firmware Name",				"FWNAME",		new CVarstring () ),
		new CVariable ("Firmware Version",			"FWVERSION",	new CVarstring () ),
		new CVariable ("Update Firmware",			"UPDFW",		new CVarint () ),
		new CVariable ("Update Firmware Counter",	"UPDFW_COUNT",	new CVarint () ),
		new CVariable ("Update Firmware Protocol",	"UPDFW_PROTO",	new CVarstring () ),
		new CVariable ("Update Firmware Server",	"UPDFW_HOST",	new CVarstring () ),
		new CVariable ("Update Firmware Port",		"UPDFW_PORT",	new CVarint () ),
		new CVariable ("Update Firmware Name",		"UPDFW_NAME",	new CVarstring () ),
		new CVariable ("Update Firmware Route",		"UPDFW_ROUTE",	new CVarstring () ),
		new CVariable ("Update Firmware User",		"UPDFW_USER",	new CVarstring () ),
		new CVariable ("Update Firmware Password",	"UPDFW_PSWD",	new CVarstring () ),

		new CVariable ("GPIO Status",				"GPIO",			new CVarstring () ),
		new CVariable ("PWM Status",				"PWM",			new CVarint () ),

		// INPUT PARAMETERS to populate DATA ITEMS
		//	(prefix p to the same name
		new CVariable ("Firmware Name",				"pFWNAME",		new CVarstring () ),
		new CVariable ("Firmware Version",			"pFWVERSION",	new CVarstring () ),
		new CVariable ("Update Firmware",			"pUPDFW",		new CVarint () ),
		new CVariable ("Update Firmware Counter",	"pUPDFW_COUNT",	new CVarint () ),
		new CVariable ("Update Firmware Protocol",	"pUPDFW_PROTO",	new CVarstring () ),
		new CVariable ("Update Firmware Server",	"pUPDFW_HOST",	new CVarstring () ),
		new CVariable ("Update Firmware Port",		"pUPDFW_PORT",	new CVarint () ),
		new CVariable ("Update Firmware Name",		"pUPDFW_NAME",	new CVarstring () ),
		new CVariable ("Update Firmware Route",		"pUPDFW_ROUTE",	new CVarstring () ),
		new CVariable ("Update Firmware User",		"pUPDFW_USER",	new CVarstring () ),
		new CVariable ("Update Firmware Password",	"pUPDFW_PSWD",	new CVarstring () ),


#ifdef DEBUG
//		Helpers for debug & test
		new CVariable ("Time Delay in hours", "TDELAY",	new CVarint  () ),
#endif


	//	All the following variables belong to the EPD Subspace
	//	All of them have short names prefixed with EPD::
	//	The one with the same prefix and name is the primary key of the Subspace

#ifdef CMC

		new CVariable ("EDP Ident",					"EPD::EPD",	new CVarstring () ),
		new CVariable ("EDP Dimming",				"EPD::PWM",	new CVarint () ),
		new CVariable ("EDP Temperature",			"EPD::TEMP",	new CVarint () ),
		new CVariable ("EDP Voltage",				"EPD::V",		new CVarint () ),
		new CVariable ("EDP Current",				"EPD::A",	new CVarint () ),
		new CVariable ("EDP Active power",			"EPD::P",	new CVardec () ),
#endif

		NULL
	};
	CVariable::Context = ALLVARS;
	CSubspace::RestoreAll();
	return 1;
}


	// (2) VARIABLE HANDLERS  
	// to declare the variables having any special handling when writen and/or read
	// Is an array of three element structure (variable nick, read handler, write handler, terminated by a NULL entry 
#include "wrsouth.h"


struct	st_wrapper	dispatch[] = {
	"TEMP", Read_Temp,		NULL,
	"A",	Read_I,			NULL,
	"V",	Read_V,			NULL,
	"P",	Read_P,			NULL,
	"Q",	Read_Q,			NULL,
	"S",	Read_S,			NULL,
	"EP",	Read_EP,		NULL,
	"EQ",	Read_EQ,		NULL,
	"ES",	Read_ES,		NULL,
	"PWM",NULL,			Write_DSTAT,

#ifdef DEBUG
	"TDELAY",NULL,			Set_TimeOffset,
#endif

	NULL
};



//	(3) MESSAGE METADATA DEFINITIONS
// This is the current way of populating the input and output Framesets
//	IMPORTANT:	the OutAPI array has to be populated BEFORE,
//				because input frames can eventually reference output frames as replay
//
//	The string  have some PARTS, separated by % character
//
//		FRAMEID % TEMPLATE % DESCRIPTION % TOPIC-ID % REPLY
//			FRAMEID:	Optional.Is only necesary for Request/Replay pairs
//			TEMPLATE	The template of the message (bolerplate text and variables)
//			DESCRIPTION	Optional, as aditional information
//			TOPIC-ID		(not used by now)
//			REPLAY		Optional. Only for INPUT messages which have to trigger a response.

int	 ReadMetadata(char *dominioIn, char *dominioOut){
	const char	*linesOut[] = {
// Core
		"ERROR%999;$MSSG;Unknown message%Error%%",
		"BAD%$MSSG;Ignored: Incorrect parameters%%",
// Application replies
		"MEASU%$ID;$TEMP;$STAT;$PWM;$A;$V;$P;$Q;$S;$EP;$EQ;$ES;$F;%periodic measurement%%",
		"TALRT%$ID;1;$TEMP%temperature alert%%",
		"VALRT%$ID;2;$V%voltage alert%%",
		"IALRT%$ID;3;$A%current alert%%",

// BootLoader share	replies
		"FWCNF%607;$ID;$RESULT;$FWVERSION;$UPDFW_HOST;$UPDFW_PORT;$UPDFW_PROTO;$UPDFW_ROUTE;$UPDFW_USER;$UPDFW_PSWD;$UPDFW_COUNT;%Bootloader parameters%%",
//	

// Show Firmware Version 
		"APIVER%609;$ID;$FWVERSION;%Show Firmware version%%",



#ifdef DEBUG
		"SHAPN%APN=;$APN;LAPN=;$LAPN% SHOW APNs%%",
		"SHPER%$MPERIOD;$MPBEGIN;$MPEND;$MPTIMES;%Show Peridocs%%",
#endif

#ifdef CMC
		// TEXT* are Test ConEXTension Frames 
		// (test Frames for CMC/EPD Context Extension)
		//  713 command (response to TEXT7, 313 comman
		"TEXT3%713;@EPD($EPD::EPD,$EPD::P)%List of EPDs and power values%%",
		// INVENTED FRAME for get on demand EPD values (response to 315 command)
		"TEXT9%$EPD::EPD;$EPD::PWM;$EPD::TEMP;$EPD::V;$EPD::A;$EPD::P;%EPD meassurement%%",
#endif


		// FAKE FRAMES; only to provide echo to some input commands
//		"ECHO4%Send meass each ;$MPERIOD; minutes from;$MPBEGIN; until ;$MPEND; or ;$MPTIMES;%Periodoc meassurement parameters echo%%",
//		"ECHO2%HE AHI;$PROGR; FIN;% ligthing profile programation echo %%",
		
		
		NULL
	};



	const char	*linesIn[] = {
// Application commands
		"PULLM%1;%pull measurement%xx%MEASU%",
		"LPROF%2;$PROGR% lighting profile programation%%",
		"SDIMM%3;$PWM;% set ligthing status%%",
		"PERMS%4;$MPERIOD;$MPBEGIN;$MPEND;$MPTIMES;%Periodoc meassurement parameters%%",
		"CCOMM%5;$IP;$PORT;$USER;$PSWD%configure communications%%",
//		"CTHRS%6;$TX;$AX;$VX;% configure theresolds%%",

//	Bootloader commands
		"APN%175;$APN;% Change APN%%",
		"FWREQ%207;$pUPDFW_HOST;$pUPDFW_PORT;$pUPDFW_PROTO;$pUPDFW_ROUTE;$pUPDFW_NAME;$pUPDFW_USER;$pUPDFW_PSWD;$pUPDFW_COUNT%firmware updates config params%%FWCNF%",
		"FWTRG%208;$pUPDFW_USER;$pUPDFW_PSWD;%Trigger Bootloader%%FWCNF%",
		"APIREQ%209;%Retrieve API version%%APIVER%",

		"FWTRG%299;;%Bootloader Display%%FWCNF%",
#ifdef DEBUG
		"APN1%176;% Request APNs%%SHAPN",
		"VPER%41;% VER PERIODICS%%SHPER",
		"DST%00;$TDELAY% ADELANTO DE HORA%%",

#endif


#ifdef CMC
		// 316; command : PRIVILEGED to CREATE EPDs
		"TEXT1%316;$$EPD::EPD;$EPD::P;%Set EPD Active power%%",
		// INVENTED: 317; command for setting EPD Dimming value
		"TEXT2%317;$EPD::EPD;$EPD::PWM;%Set EPD Dimming%%",
		// 313; command, asking for TEXT3 (713; ) replay
		"TEXT7%313;%On demand list of EDPS%yy%TEXT3%",
		// INVENTED: 313; command for requesting EPD mesaaurement (TEXT9)
		"TEXT8%315;$EPD::EPD;%On demand get EPD%yy%TEXT9%",
#endif

		NULL
	};


	CFrame::OutAPI = ReadFrames2(linesOut);

	CFrame::InAPI = ReadFrames2(linesIn);
	return 1;
}


	// (4) MESSAGE HANDLERS 
	// to declare the messages having any special handling before and/or after processing
	// Is an array of three element structure (message id, pre handler, post handler, terminated by a NULL entry 


struct	st_fractions
	actions[] = {
	"SDIMM", 	NULL,			SaveConnParams,		// Command 3; post-action to save
	"CCOMM", 	NULL,  			FULLNEWCONN,			// Command 5; post-action to reconnect
	"PERMS",	NULL,			SetPeriodics,		// Command 4; post-action to update timers
	"LPROF",	NULL,			SetProgramm,		// Command 2; post-action to update timers
	"APN", 		NULL,			SaveConnParams,		// Command 175; post-action to save
	"FWREQ",	NULL,			ValidateFwParams,	// Command 207; post action to validate and etc..
	"FWTRG",	NULL,			ValidateFwAuth,		// Command 208; post action to validate and etc..
	NULL
};





//////////////////////////  MISCELANEA /////////////////////////////////////

// Provisional placement for this

int	SendMeassurement() {
	return SendFrame("MEASU");
}



// SOME I PLEMENTATION SAMPLES of NVM save and restore  Subspace records  

#define RAM



#ifdef	DISCO

const char	*filename = "flashfile.txt";
int		GenericReadElement(unsigned int n, unsigned char *to) {
	unsigned int base = CELLSIZE * n;  // por el CRLF
	int rc; 
	FILE *Flash = fopen(filename, "r");
	fseek (Flash, base, SEEK_SET);
//	printf ("Leyendo %d a %d sale %s\n", pos, pos+s, p);
	fgetc(Flash);
	rc = fread (to, CELLSIZE - 1, 1, Flash);
	fclose (Flash);
	return rc;
}

int		GenericWriteElement(unsigned int n, unsigned char *to) {
	unsigned int base = CELLSIZE * n;
	FILE *Flash = fopen(filename, "r+");
	if (Flash == NULL) 
		FILE *Flash = fopen(filename, "w+");
	fseek (Flash, base, SEEK_SET);
	fputc('\n', Flash); fwrite (to, CELLSIZE - 1 , 1, Flash);
	fclose (Flash);
	return 1;
}

#endif

static	char	swaparea[CELLSIZE*MAX_TABLE_SIZE];

#ifdef	RAM
int		GenericReadElement(unsigned int n, unsigned char *to) {
	unsigned int base = CELLSIZE * n;
	memcpy (to, swaparea+base, CELLSIZE);
	if (strchr((char *) to, '='))
	return 1;
	else
		return 0;

}

int		GenericWriteElement(unsigned int n, unsigned char *to) {
	unsigned int base = CELLSIZE * n;
	memcpy (swaparea+base, to, CELLSIZE);
	return 1;
}
#endif

