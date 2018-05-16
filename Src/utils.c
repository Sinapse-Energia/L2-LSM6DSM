/*
 * utils.c
 *
 *  Created on: Jun 19, 2017
 *      Author: External
 */


#include 	<stdio.h>
#include 	<string.h>
#include 	<stdarg.h>
#include	"utils.h"
#include	"NBinterface.h"		// any of them is implemented here
#include 	"southbound_ec.h"	// NVM access functions


struct tm	ECdatetime;

long		ECtimestamp;


// The function now can deal with the reply oof CCLK command (NTP server), and QLTS command (Network time)
int SetDateTime(const char *reply){
	int			utcoffset = 0;
	char		Ttype[16];
	int n = sscanf (reply+2, "+%s \"%d/%d/%d,%d:%d:%d+%d", Ttype, &ECdatetime.tm_year, &ECdatetime.tm_mon, &ECdatetime.tm_mday, &ECdatetime.tm_hour, &ECdatetime.tm_min, &ECdatetime.tm_sec, &utcoffset);
	if ( n ==8 ){
		if (!strcmp (Ttype, "CCLK:"))
			utcoffset = 1;
		if (!strcmp (Ttype, "QLTS:"))
			utcoffset = utcoffset/4;

		ECdatetime.tm_year += 2000;
		ECdatetime.tm_hour += utcoffset;

		if (ECdatetime.tm_sec > 5) {
			ECdatetime.tm_sec -= 5;
		}
		else {
			ECdatetime.tm_sec = 0;
		}
	//	sscanf (NTP+2, "%d/%d/%d", &ECdatetime.tm_year, &ECdatetime.tm_mon, &ECdatetime.tm_mday);
		ECtimestamp = ((ECdatetime.tm_hour * 60 + ECdatetime.tm_min) * 60 ) + ECdatetime.tm_sec;
		return 1;
	}
	else
		return 0;
};



long GetTimeStamp(){
	return ECtimestamp;
}

void AddSeconds(int n){
	static int counter = 0;
	UpdateTimeStamp(n);
	counter += n;
	if (counter > 60 ){
		AddMinute(1);
		counter = counter - 60;
	}
}

void AddMinute(int n){
}




void UpdateTimeStamp(int n){
	ECtimestamp += n;
	ECdatetime.tm_sec += n;
	if (ECdatetime.tm_sec > 59) {
		ECdatetime.tm_min += ECdatetime.tm_sec / 60;
		ECdatetime.tm_sec = ECdatetime.tm_sec % 60;
		if (ECdatetime.tm_min > 59) {
			ECdatetime.tm_hour += ECdatetime.tm_min / 60;
			ECdatetime.tm_min = ECdatetime.tm_min % 60;
			if (ECdatetime.tm_hour > 23) {
				ECdatetime.tm_mday += ECdatetime.tm_hour / 24;
				ECdatetime.tm_hour = ECdatetime.tm_hour % 24;
			}
		}

	}

}

char	*strDateTime() {
	static	char	DT[64];
	sprintf (DT, "%d-%d-%d %02d:%02d:%02d",
					ECdatetime.tm_mday, ECdatetime.tm_mon, ECdatetime.tm_year,
					ECdatetime.tm_hour, ECdatetime.tm_min, ECdatetime.tm_sec);
	return DT;
}
#ifdef DEBUG
char	*Set_TimeOffset(const char *val) {
	int valor = atoi (val);
	UpdateTimeStamp(valor*60*60);
	return val;
}
#endif


// Pending : to validate the IP format is correct
int	 SetLocalIP(const char *txt){
	char	myIP[20];
	strcpy (myIP, txt+2);
	char *p = strchr(myIP, '\r');
	if (p)
		*p = 0;
	SetVariable ("LOCALIP", myIP);
	return 1;
}

// Pending : to validate the SIM format is correct
int	SetIdSIM(const char *txt){
	char	SIM[40];
	strcpy (SIM, txt+2);
	char *p = strchr(SIM, '\r');
	if (p)
		*p = 0;
	SetVariable ("IDSIM", SIM);
	return 1;
}

int	SetIMEI(const char *txt){
	char	IMEI[32];  // supossed to have 16 chars
	strncpy (IMEI, txt+2, 20);  // provisional.... sometimes comes longer to null terminator
	char *p = strchr(IMEI, '\r');
	if (p)
		*p = 0;
	SetVariable ("IMEI", IMEI);
	return 1;
}


int		ValidateReg(const char *reply){
	int	n;
	int stat;
	char	cmd[10];
	int x = sscanf (reply, "\r\n+%s %d,%d\r\n\r\nOK\r\n", cmd, &n, &stat );
	printf ("tengo %s, %d, %d", reply, n, stat);
	if (x == 3) {
		if ( (n == 0) && ((stat == 1) || (stat == 5)))
				return 1;
		if ( (n == 0) && (stat == 2))
			return -1; //it lasts.... retry again
		else
			return 0;
	}
	else
		return 0;

	return 1;
}



int		SetGeneric(const char *txt){
	const char *tmp = txt;

	return 1;
}
int		SetState(const char *txt){
	const char *tmp = txt;

	return 1;
}






// Helper function to accumulate trace until it can be shown
// Small buffer allocation until verify how much memory is available
char	pretrbuf[500] = "";
int		pretrace(char *texto,...) {
	va_list	  ap;

	va_start	  (ap, texto);
	vsprintf (pretrbuf+ strlen(pretrbuf), texto, ap);
	return 1;
}

//
//
// 	This is a PROVISIONAL function intended to verify the message manipulation, without having to depend on message reception
//	Uses a arbitrary sequence of commands in a circular list....
//	Each item de function is called, returns the next message form the list

char	*GetLocalMessage(int h, char *buffer, int maxsize){
	static  	char 	*messages[] = {
	  			"1;",
				"3;20;",
				"1;",
				"3;60;",
				"1;",
				"3;90;",
				"1;",
				"3;0;"
	  	};
	static int i = 0;
	char	*result = messages[i];
	i = (i+1)%(sizeof(messages)/sizeof(*messages));
	if (strlen(result) < maxsize)
		strcpy (buffer, result);
	else
		strncpy(buffer, result, maxsize);
	// HAL_Delay(1000);
	return buffer;
}





#define STORESIZE 512
char	store[STORESIZE];


int	SaveALL(){
	const char	*listofvars[] = {
			"IP","PORT", "USER", "PSWD", "SEC", "LIP", "LPORT","LUSER", "LPSWD","LSEC", "APN", "LAPN",
			"MPERIOD", "MPTIMES", "MPBEGIN", "MPEND",
			"ID", "UPDFW", "UPDFW_COUNT", "FWNAME", "FWVERSION", "UPDFW_PROTO", "UPDFW_HOST", "UPDFW_PORT",	 "UPDFW_NAME", "UPDFW_ROUTE", "UPDFW_USER", "UPDFW_PSWD",
			"UPDFW", "UPDFW_COUNT",
			NULL
	};
	int i = 0;
	store[0] = 0;
	while (listofvars[i]) {
		char item[128];
		const char *name = listofvars[i];
		char *value = GetVariable(name);
		if (value){
			sprintf (item, "%s=%s;",  name, value );
			if (strlen(store) + strlen(item) < STORESIZE){
				strcat(store, item);
				i++;
			}
			else {
				break;
			}
		}
		else
			i++;
	}
	MIC_Flash_Memory_Write((const uint8_t *) store, (uint32_t)strlen(store)+ 1);
	return 1;
}


int	SaveConnParams(){
	return SaveALL();
}

int	SaveAppParams(){
	return SaveALL();
}

int	SaveBLParams(){
	return SaveALL();
}


int RecConnParams(){
	MIC_Flash_Memory_Read( (const uint8_t *) store, sizeof(store));
	WriteData((char *) store);
}

int RecAppParams(){
	MIC_Flash_Memory_Read( (const uint8_t *) store, sizeof(store));
	WriteData((char *) store);
}


char *textCert =
"-----BEGIN CERTIFICATE-----\n"
"MIIEATCCAumgAwIBAgIJALFjqVBg1LHlMA0GCSqGSIb3DQEBCwUAMIGWMQswCQYD"
"VQQGEwJFUzEPMA0GA1UECAwGTWFkcmlkMQ8wDQYDVQQHDAZNYWRyaWQxEDAOBgNV"
"BAoMB1NpbmFwc2UxFDASBgNVBAsMC0RldmVsb3BtZW50MQ0wCwYDVQQDDARNUVRU"
"MS4wLAYJKoZIhvcNAQkBFh9yYWZhLmFsY2FpZGVAc2luYXBzZWVuZXJnaWEuY29t"
"MB4XDTE3MDkxNDExMjY0MloXDTIyMDkxNDExMjY0MlowgZYxCzAJBgNVBAYTAkVT"
"MQ8wDQYDVQQIDAZNYWRyaWQxDzANBgNVBAcMBk1hZHJpZDEQMA4GA1UECgwHU2lu"
"YXBzZTEUMBIGA1UECwwLRGV2ZWxvcG1lbnQxDTALBgNVBAMMBE1RVFQxLjAsBgkq"
"hkiG9w0BCQEWH3JhZmEuYWxjYWlkZUBzaW5hcHNlZW5lcmdpYS5jb20wggEiMA0G"
"CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDCfuQkv6LrQoyynZMi4jsmGgQ32geK"
"5v7fYWCH3dB7f6KVDozRraFW4a9vqkhQEpwmMcXZ6+aI/NaFW0ifc16BKdXXMslw"
"aJmQIBbBgrpXCYCmED7v4h8bBYNoA+/yIqo+EAYfLSYwMvM/9D7n2x28LiytuNsf"
"Nsga6tToN2rIIfsMrCRxBT4Ex8NKlpyRF0EO29jbZFrlUXp0wqowZFobgM5mgNjG"
"MSYzPJFvlF/Hlp5XL2MM5nbgWWYgGk6w2Ep79gR4a32Np0Gq8C3r7avv29num6Mu"
"3pKp+wiBUtZzafNHGqbRClcQEUZk7E6K/yXRqEfOdBO1RZE99/orO7vTAgMBAAGj"
"UDBOMB0GA1UdDgQWBBT4A15SVpWhKsWWWD9nWnn91bPVDDAfBgNVHSMEGDAWgBT4"
"A15SVpWhKsWWWD9nWnn91bPVDDAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUA"
"A4IBAQBdkrJkQ5f6wA+FoofKy19GewXWtn6UTnE1U/dVehJmB8WXxnLZAqH7aPq8"
"kFN+MsAXa6qaLe9C52ne75bop8rzLE73fcWjRtfbJr5fovk32jF20VL0QhjRfwko"
"pRqqrYBoDMzpvyzPaJnPEcqyVf+MHVPqaRdnXz/9Qqumu9yz09XVBPL6KkskpQgj"
"oZs2jpCPSDhAavzNTJzQuJmVYj5eYTBBMmg8y9UENqvmQsUPFx8lAcWF/BcKLBH5"
"BMCnZ9MYAwA4kwMpv4/yeKfDShF9RbG2U6rSbW0Rv4bpS5IuKBgIyWdfJf3Qcb1c"
"IsqfjZ5GlW+Gih34ZxlzNYEAjm/j\n"
"-----END CERTIFICATE-----";


unsigned char	*getCertificateTxt(size_t *lcert){
	*lcert = strlen(textCert);
	return textCert;
}




