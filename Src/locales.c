/*
 * locales.c
 *
 *  Created on: Jun 9, 2017
 *      Author: External
 */



char	*id0 		= "151515";

//#define SINAPSE1S

//#define AWS1
//#define ISSUE58
//#define BADCR
//#define FAKE
#define VODAFONE2


#if defined(VODAFONE2)  || defined(VODAFONE1) || defined(ISSUE58)
//char	*const_APN 			= "\"matooma.m2m\",\"\",\"\"\0";
char	*const_APN 	= "\"lte.m2m\",\"\",\"\"\0";

char	*topicroot 	= "SC/ES/RI/LI/SI";		// base for ALL topics


#else

//char	*const_APN 			= "\"m2m.tele2.com\",\"tele2\",\"tele2\"\r\0";
//char	*const_APN 			= "\"im2m.matooma.com\",\"movistar\",\"movistar\"\r\0";


//char	*const_APN 			= "\"matooma.m2m\",\"\",\"\"\0";
char	*const_APN 			= "\"lte.m2m\",\"\",\"\"\0";
//char	*const_APN = 0;

//char	*const_APN = "\"orangeworld\",\"orange\",\"orange\"\r\0";;

char	*topicroot 	= "LU/LUM";		// base for ALL topics

#endif


char	*const_SERVER_NTP 	= "\"0.europe.pool.ntp.org\"\r\0";
char	*const_MAIN_SERVER 	= "\"puertorealsinapse.dyndns.org\",993\r\0";

char	*topictr0 	= "TRACE";		// sub for trace topics
char	*topicpub0 	= "SEN";  		// sub for publish topic
char	*topicsub0	= "ACT";  	  	// sub for subscript topics .

char	*broadcast = "FFFFFF";


#ifdef VODAFONE1

char	*user 		= "scsi";
char	*password		= "ks87b3kef0af4a2";
char	*host		= "185.8.192.36";
unsigned int port	= 1883;
int		security	=  0;

#endif

#ifdef ISSUE58

char	*user		= "scts";
char	*password	= "c9m2w0dn5k4p299";
char	*host		= "185.8.192.61";
unsigned int port = 1883;
int security = 0;
#endif


#ifdef VODAFONE2

char *user ="scts";
char *password ="c9m2w0dn5k4p299";
char *host ="185.8.192.61";
unsigned int port = 1883;
int security = 0;
#endif

#ifdef CLOUD

char	*user 		= "ugjznzkc";
char	*password	= "7UG2yVgH3zzi";
char	*host		= "m21.cloudmqtt.com";
unsigned int port	= 10609;
int		security		= 0;

#endif

#ifdef BADCR
char	*user 		= "bad";
char	*password	= "bad";
char	*host		= "m21.cloudmqtt.com";
unsigned int port	= 10609;
int		security		= 0;

#endif

#ifdef FAKE
char	*user 		= "any";
char	*password	= "any";
char	*host		= "m21.cloudmqtt.com";
unsigned int port	= 7777;
int		security		= 0;

#endif


#ifdef ECLIPSE

char	*user 		= "testuser";
char	*password	= "testpassword";
char	*host		= "m2m.eclipse.org";
unsigned int port	= 1883;
int		security		= 0;
#endif

#ifdef AWS0
char	*user 		= "sinapse_mqtt";
char	*password	= "12345678";
char	*host		= "ec2-54-217-8-108.eu-west-1.compute.amazonaws.com";
unsigned int port	= 1883;
int		security		= 0;
#endif

#ifdef AWS1
char	*user 		= "test";
char	*password	= "test";
//char	*host		= "ec2-54-216-34-233.eu-west-1.compute.amazonaws.com";
char	*host		= "soporte-tecnico.bitnamiapp.com";
unsigned int port	= 1883;
int		security		= 0;
#endif


#ifdef SINAPSE1S
char	*user 		= "test";
char	*password	= "test";
char	*host		= "soportesinapse.bitnamiapp.com";
unsigned int port	= 8883;
int		security	= 1;
#endif







