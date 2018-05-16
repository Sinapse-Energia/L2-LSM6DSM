#include <string.h>
#include <stdio.h>

#include "Cmdflows.h"

#include "utils.h"



int isdns(unsigned char * host){
	int n1, n2, n3, n4;
	int n = sscanf((const char *)host, "%d.%d.%d.%d", &n1, &n2, &n3, &n4);
	if (n == 4)
		return 0;
	else
		return 1;
}



M95Status	M95_Initialize(
		UART_HandleTypeDef *phuart,
		uint8_t WDT_ENABLED,
		IWDG_HandleTypeDef *hiwdg,
		GPIO_TypeDef* ctrlEmerg_PORT, uint16_t ctrlEmerg_PIN,
		GPIO_TypeDef* ctrlPwrkey_PORT, uint16_t ctrlPwrkey_PIN,
		GPIO_TypeDef* m95Status_PORT, uint16_t m95Status_PIN,
		uint8_t nTimesMaximumFail_GPRS
		) {
	uint8_t countGPRSStatus=0;
	GPIO_PinState statusM95_statusPin;

	//HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_SET); // Writing 0 to ARM_CTRL_EMERG reset module.
	HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_RESET); // Writing 0 in new m2m
	HAL_Delay(400);
	//HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_RESET); // Writing 0 to ARM_CTRL_EMERG reset module.
	HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_SET); // Writing NC in new m2m

	//HAL_GPIO_WritePin(ctrlPwrkey_PORT, ctrlPwrkey_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ctrlPwrkey_PORT, ctrlPwrkey_PIN, GPIO_PIN_RESET); // writing 0 in new M2M
	countGPRSStatus = 0;

	do {
		HAL_Delay(2000);
		statusM95_statusPin = HAL_GPIO_ReadPin(m95Status_PORT, m95Status_PIN); //awaiting status pin goes to 1

		if (countGPRSStatus == nTimesMaximumFail_GPRS) { /// Realizo el apagado de emergencia
			//HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_SET); // Writing 0 to ARM_CTRL_EMERG reset module.
			HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_RESET); // Writing 0 in new m2m
			HAL_Delay(400);
			//HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_RESET); // Writing 0 to ARM_CTRL_EMERG reset module.
			HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_SET); // Writing NC in new m2m
			countGPRSStatus = 0;
		}
		countGPRSStatus++;
	} while (statusM95_statusPin == GPIO_PIN_RESET);

	//HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_RESET); //  PWRKEY is released (ARM_CTRL_PWRKEY is the inverted, 0
	HAL_GPIO_WritePin(ctrlEmerg_PORT, ctrlEmerg_PIN, GPIO_PIN_SET); //  PWRKEY is released NC in new M2M
	HAL_Delay(3000);


	if (WDT_ENABLED == 1) HAL_IWDG_Refresh(hiwdg);

	HAL_UART_Transmit(phuart,(uint8_t*)"AT+IPR=115200&W\r",16,100);
//	if (WIFICommunication_Enabled==0) HAL_UART_Transmit(phuartLOG,(uint8_t*)"Micro in 19200bps sends -> AT+IPR=115200&W\r",43,100);
	HAL_UART_DeInit(phuart);

	phuart->Init.BaudRate=115200;
	HAL_UART_Init(phuart);

	HAL_UART_Transmit(phuart,(uint8_t*)"AT+IPR=115200&W\r",16,100);
//	if (WIFICommunication_Enabled==0) HAL_UART_Transmit(phuartLOG,(uint8_t*)"Micro in 115200bps sends -> AT+IPR=115200&W\r",44,100);
	if (WDT_ENABLED == 1) HAL_IWDG_Refresh(hiwdg);

	HAL_UART_Transmit(phuart,(uint8_t*)"ATE0\r",5,100);   			// ECHO OFF
	HAL_UART_Transmit(phuart,(uint8_t*)"AT+QIURC=0\r",11,100);		// UNSEND URCs

	return M95_OK;
}



CmdProps	*M95DisconnectFlow(){
	static CmdProps	M95Disconnect[] = {
//		{	ATMATCH,	"",		{"+++"}, 			{"\r\nOK\r\n"}, 				{1000, 1000, 1000}, 	1} ,
		{	ATGET,		"",		{"+++"}, 			{NULL, NULL, SetGeneric}, 		{1000, 1000, 1000}, 	1} ,
		{	ATMATCH,	"",		{"AT+QICLOSE\r"}, 	{NULL, NULL, SetGeneric},		{1000, 0}, 				1} ,
		{	ATMATCH,	"DEACT",{"AT+QIDEACT\r"}, 	{NULL, NULL, SetGeneric},		{1000, 0}, 				1} ,
		{	ATMATCH, 	"END", 	NULL }
	};
	return M95Disconnect;
}


static char 	GPRS_TX[100]; 	// to build QICSGP AT command
static char 	NTP_TX[100]; 	// to build QINTP AT command
static char		OPEN_TX[100];	// to build OPEN AT command



CmdProps	*M95ConnectFlow(
		uint8_t *APN,
		uint8_t *host,
		int	port,
		uint8_t *SERVER_NTP,
		uint8_t transparentMode
		) {
	sprintf(GPRS_TX, "AT+QICSGP=1,%s\r", APN);
	sprintf(NTP_TX, "AT+QNTP=%s\r", SERVER_NTP);
	sprintf(OPEN_TX, "AT+QIOPEN=\"TCP\",\"%s\",%d\r", host, port);

	int dns = isdns(host);
	static CmdProps	M95Connect[] = {
			{	ATMATCH,	"INIT",	{"ATE0\r"},			{"\r\nOK\r\n"}, 			{1000, 0}, 	3} ,
			{	ATMATCH,	"",		{"AT+QIFGCNT=0\r"}, {"\r\nOK\r\n"},				{ 300, 0},  1} ,
			{	ATMATCH,	"",		{GPRS_TX},			{"\r\nOK\r\n"}, 			{ 300, 0},  1} ,
			{	ATMATCH,	"",		{"ATE0\r"},			{"\r\nOK\r\n"}, 			{1000, 0}, 	1} ,
			{	ATMATCH,	"",		{dns?"AT+QIDNSIP=1\r":"AT+QIDNSIP=0\r"}, {"\r\nOK\r\n"}, { 300, 0}, 1} ,
			{	ATMATCH,	"",		{"AT+QIMUX=0\r"}, 	{"\r\nOK\r\n"},				{ 300, 0},  1} ,
			{	ATMATCH,	"",		{transparentMode?"AT+QIMODE=1\r":"AT+QIMODE=0\r"},{"\r\nOK\r\n"}, {1000, 0}, 	1} ,
			{	ATMATCH,	"",		{"AT+QITCFG=3,2,512,1\r"},	{"\r\nOK\r\n"}, 	{1000, 0}, 	1} ,
			{	ATGET,		"",		{"AT+QCCID\r"}, 	{NULL, NULL ,SetIdSIM}, 	{1000, 0}, 	1} ,
			{	ATGET,		"",		{"AT+GSN\r"}, 		{NULL, NULL ,SetIMEI}, 		{1000, 0}, 	1} ,
//			{	ATGET,		"",		{"AT+QGSN\r"}, 		{NULL, NULL ,SetIMEI}, 		{1000, 0}, 	1} ,

			{	ATGET,		"",		{"AT+CREG?\r"}, 	{NULL, NULL, ValidateReg}, 	{1000, 0}, 	1 } ,
			{	ATGET,		"",		{"AT+CGREG?\r"},	{NULL, NULL, ValidateReg},	{1000, 0}, 	1},
			{	ATMATCH,	"",		{"AT+QIREGAPP\r"}, 	{"\r\nOK\r\n"},				{1000, 0}, 	1},
			{	ATMATCH,	"",		{"AT+QIACT\r"}, 	{"\r\nOK\r\n"},				{1000, 0}, 	1},
			{	ATGET,		"",		{"AT+QILOCIP\r"}, 	{NULL,	NULL, SetLocalIP}, {1000, 0}, 	1},
#if (1)
			{	ATMATCH,	"",		{NTP_TX,}, 			{"\r\nOK\r\n\r\n+QNTP: 0\r\n"},	{1000, 0, 10000}, 1, },
			{	ATGET,		"",		{"AT+CCLK?\r"}, 	{NULL,	NULL, SetDateTime},	{1000, 0}, 	1,  },
#else
			{	ATMATCH,	"",		{"AT+QNITZ=1\r",}, 	{"\r\nOK\r\n"},	{1000, 0, 1000}, 1, },
			{	ATGET,		"",		{"AT+QLTS\r"}, 		{NULL,	NULL, SetDateTime},	{1000, 0}, 	1,  },
#endif
//			{	ATMATCH,	"",		{OPEN_TX,}, 		{"\r\nOK\r\n\r\nCONNECT OK\r\n"},{3000, 0}, 	1 , NULL, NULL, 1},
			{	ATMATCH,	"",		{OPEN_TX,}, 		{"\r\nOK\r\n\r\nCONNECT\r\n"},{5000, 0}, 2 , NULL, NULL, 1},
			{	ATMATCH, 	"END"  },
			{	ATMATCH, 	NULL	}  // placeholder for error in label lookup
	};
	return M95Connect;
}

CmdProps	*M95ReConnectFlow( uint8_t *host,uint16_t	port) {
	sprintf(OPEN_TX, "AT+QIOPEN=\"TCP\",\"%s\",%d\r", host, port);

	static CmdProps	M95ReConnect[] = {
			{	ATGET,		"",		{"+++"}, 			{NULL, NULL, SetGeneric}, 		{1000, 1000, 1000}, 	1} ,
			{	ATMATCH,	"",		{"AT+QIDEACT\r"}, 	{NULL, NULL, SetGeneric},		{1000, 0}, 				1} ,
			{	ATMATCH,	"",		{"AT+QIREGAPP\r"}, 	{"\r\nOK\r\n"},				{1000, 0}, 	1},
			{	ATMATCH,	"",		{"AT+QIACT\r"}, 	{"\r\nOK\r\n"},				{1000, 0}, 	1},
			{	ATGET,		"",		{"AT+QILOCIP\r"}, 	{NULL,	NULL, SetLocalIP}, {1000, 0}, 	1},
			{	ATMATCH,	"",		{OPEN_TX,}, 		{"\r\nOK\r\n\r\nCONNECT\r\n"},{2000, 0}, 	1 },
			{	ATMATCH, 	"END"  },
			{	ATMATCH, 	NULL	}  // placeholder for error in label lookup
	};
	return M95ReConnect;
}

char		ATstate[64];
char		ATerror[64];
CmdProps trstate = {ATGET,	"",		{"AT+QISTAT\r"},			{NULL, ATstate, SetGeneric}, 				{1000, 0}, 	1} ;
CmdProps trerror = {ATGET,	"",		{"AT+GETERROR\r"},			{NULL, ATerror, SetGeneric}, 				{1000, 0}, 	1} ;





static char	SECRD1_TX[100];	// to build AT SECREAD command

static char	CACER_TX[100];	// to build AT SSLCFG CACERT certificate command


// LIKE QUECTEL
CmdProps	*M95ConnectTLSFlow(
		uint8_t *APN,
		uint8_t *host,
		int		port,
		uint8_t *SERVER_NTP,
		uint8_t existDNS,
		uint8_t setTransparentConnection  ) {

	// Mandatory NON-Transp in TLS?
	setTransparentConnection = 0;



//	static char GPRS_TX[100]; 	// to build QICSGP AT command

	const char	*caname = "RAM:ca_cert.pem";

#ifndef		PREATS
	static char	SECWR1_TX[100];	// to build AT SECWRITE command
	static char	SECDEL_TX[100];	// to build AT SECDEL command

	size_t cert_size = 0;
	char	*certificate = (char *)  getCertificateTxt(&cert_size);
	sprintf(SECWR1_TX, "AT+QSECWRITE=\"%s\",%d,100\r", caname, cert_size);
	sprintf(SECDEL_TX, "AT+QSECDEL=\"%s\"\r", caname);

#endif
	sprintf(SECRD1_TX, "AT+QSECREAD=\"%s\"\r", caname);
	sprintf(CACER_TX, "AT+QSSLCFG=\"cacert\",0,\"%s\"\r", caname);
	sprintf(GPRS_TX, "AT+QICSGP=1,%s\r", APN);
	sprintf(NTP_TX, "AT+QNTP=%s\r", SERVER_NTP);
	sprintf(OPEN_TX, "AT+QSSLOPEN=1,0,\"%s\",%d,%d\r", host, port,setTransparentConnection);

	static CmdProps	M95SecureFlow[] = {
	{	ATGET,		"",		{"+++"}, 			{NULL, NULL, SetGeneric}, 	{200,  1000, 1000}, 	1} ,
	{	ATMATCH,	"",		{"ATE0\r"},							{"\r\nOK\r\n"}, 			{1000, 0}, 	3},   // no ATE0, & 3 retries for flush call ready
	{	ATGET,		"",		{"AT+QICLOSE\r"}, 	{NULL, NULL, SetGeneric},	{1000, 0}, 				1} ,
	{	ATGET,		"",		{"AT+QIDEACT\r"}, 	{NULL, NULL, SetGeneric},	{1000, 0}, 				1} ,
//	{	ATMATCH,	"",		{"AT+QIURC=0\r"},					{"\r\nOK\r\n"},				{300,  0},	1},

// CA
#ifndef		PREATS
	{	ATGET,		"",		{SECDEL_TX},						{NULL, NULL, SetGeneric},	{1000, 0}, 	1},
	{	ATMATCH,	"",		{SECWR1_TX},						{"\r\nCONNECT\r\n"},		{1000, 0}, 	1},
	{	ATMATCH,	"",		{certificate ,cert_size},			{"\r\n+QSECWRITE:"},		{1000, 0}, 	1},
#endif
// 1) Basic Settings FGCNT, MODE, MUX, IDNSIP, APN
	{	ATMATCH,	"",		{"AT+QIFGCNT=0\r"},					{"\r\nOK\r\n"},				{300,  0},	1},
	{	ATMATCH,	"",		{"AT+QIMODE=0\r"},					{"\r\nOK\r\n"}, 			{1000, 0}, 	1} ,
	{	ATMATCH,	"",		{"AT+QIMUX=1\r"}, 					{"\r\nOK\r\n"},				{300,  0},  1} ,
	{	ATMATCH,	"",		{"AT+QIDNSIP=1\r"}, 				{"\r\nOK\r\n"}, 			{300,  0},  1} ,
	{	ATMATCH,	"",		{GPRS_TX},							{"\r\nOK\r\n"},				{300,  0}, 	1},
// 2) SIM, IMEI, CREG, CGREG
	{	ATGET,		"",		{"AT+QCCID\r"}, 					{NULL, NULL ,SetIdSIM}, 	{1000, 0}, 	1} ,
	{	ATGET,		"",		{"AT+GSN\r"}, 						{NULL, NULL ,SetIMEI}, 		{1000, 0}, 	1} ,
	{	ATGET,	"",			{"AT+CREG?\r"}, 					{NULL, NULL, ValidateReg}, 	{1000, 0}, 	1 },
	{	ATGET,	"",			{"AT+CGREG?\r"},					{NULL, NULL, ValidateReg},	{1000, 0}, 	1},
// 3) REGAPP, ACT, LOCIP
	{	ATMATCH,	"",		{"AT+QIREGAPP\r"}, 					{"\r\nOK\r\n"},				{1000, 0}, 	1},
	{	ATMATCH,	"",		{"AT+QIACT\r"}, 					{"\r\nOK\r\n"},				{1000, 2000}, 	3}, // WFIX  2000 and 2
	{	ATGET,		"",		{"AT+QILOCIP\r"}, 					{NULL,	NULL, SetLocalIP}, 	{1000, 0}, 	1},
// 4) NTP, CCLK
#if (0)
	{	ATMATCH,	"",		{NTP_TX,}, 							{"\r\nOK\r\n\r\n+QNTP: 0\r\n"},	{1000, 0, 10000}, 1, },
	{	ATGET,		"",		{"AT+CCLK?\r"}, 					{NULL,	NULL, SetDateTime},	{1000, 0}, 	1,  },
#else
	{	ATMATCH,	"",		{"AT+QNITZ=1\r",}, 					{"\r\nOK\r\n"},	{1000, 0, 1000}, 1, },
	{	ATGET,		"",		{"AT+QLTS\r"}, 						{NULL,	NULL, SetDateTime},	{1000, 0}, 	1,  },
#endif
// 5) CFG
	{	ATMATCH,	"",		{"AT+QSSLCFG=\"seclevel\",0,1\r"},	{"\r\nOK\r\n"},  			{1000, 0}, 	1},
	{	ATMATCH,	"",		{"AT+QSSLCFG=\"ignorertctime\",1\r"},{"\r\nOK\r\n"},  			{1000, 0}, 	1},
	{	ATMATCH,	"",		{"AT+QSSLCFG=\"sslversion\",0,2\r"},{"\r\nOK\r\n"},				{1000, 0}, 	1},
	{	ATMATCH,	"",		{"AT+QSSLCFG=\"ciphersuite\",0,\"0XFFFF\"\r"},	{"\r\nOK\r\n"}, {1000, 0}, 	1},
	{	ATMATCH,	"",		{SECRD1_TX}, 						{"\r\n+QSECREAD: 1"},  		{2000, 0}, 	1},
	{	ATMATCH,	"",		{CACER_TX}, 						{"\r\nOK\r\n"},  			{1000, 0}, 	1},
// once again?
	{	ATMATCH,	"",		{"AT+QIDNSIP=1\r"}, 				{"\r\nOK\r\n"}, 			{300,  0},  1} ,

// Al last, connection
	{	ATMATCH,	"",		{OPEN_TX},							{"\r\nOK\r\n\r\n+QSSLOPEN: 1,0\r\n"},		{4000, 0}, 	1} ,  // ok + URC
	{	ATMATCH, 	"END"  }

	};
	return M95SecureFlow;
}







CmdProps	*QSendFlow(char	*mssg, size_t len){
	static	char	cmd[32];  		// small array, where the string for the command QSSLSEND to be built
	static	char	request[512];  	// Big in order to admit long messages (like traces..) to be built
	static CmdProps CmdSend[] = {
		{ ATMATCH,	"", 	{cmd},				{"\r\n> "},							{3000, 0}, 	1},
		{ ATMATCH,	"",		{request, len},		{"\r\nSEND OK\r\n"}, 				{3000, 0}, 	1},
		{ ATMATCH,	"END"}
	};
	sprintf (cmd, "AT+QSSLSEND=1,%d\r", len);
	memcpy (request, mssg, len);
	CmdSend[1].request.length = len;
	return CmdSend;
}


#if 0
typedef struct 	st_transport {
	M95Status		(* Initializer)		(UART_HandleTypeDef *, uint8_t , IWDG_HandleTypeDef *, GPIO_TypeDef*, uint16_t ctrlEmerg_PIN, GPIO_TypeDef* , uint16_t , GPIO_TypeDef* , uint16_t , uint8_t );

	CmdProps	*	(* ConnectFlow)		(uint8_t *, uint8_t*, int, uint8_t *, uint8_t, uint8_t);
	CmdProps	*	(* DisconnectFlow)	();
	int				(* Sender) 			(unsigned char* , int );
	int				(* Receiver)		(unsigned char* buffer, int lengthBuffer);

} Transport;


static Transport	Transports[] = {
		{ M95_Initialize, M95ConnectFlow, 	M95DisconnectFlow, NULL, NULL },
		{ M95_Initialize, M95ConnectTLSFlow, M95DisconnectFlow, NULL, NULL }

};
**/
#endif






