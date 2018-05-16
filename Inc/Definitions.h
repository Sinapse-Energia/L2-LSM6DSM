#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#define DEBUG


#ifdef __cplusplus
 extern "C" {
#endif

#define TIMING_TIMEOUT_GPRS 2

// second to wait after las message, to send a Ping
#define	TIMEPING  120

 // Maximum size for the datablock to save & restore from NVM
#define STORESIZE 512



extern	char	*const_APN;
extern	char	*const_SERVER_NTP;
extern	char	*const_MAIN_SERVER;


extern	char	*user;
extern	char	*password;
extern	char	*host;
extern	unsigned int port;
extern	int 	security;

extern	char	*id0;

// Global, so far
// has to be hideen in a  utilities file when it should be
extern 	char	*topicroot;
extern 	char	*topictr0;
extern	char	*topicpub0;
extern	char	*broadcast;
extern	char	*topicsub0;




typedef	 int  (* Action) (void);


#ifdef __cplusplus
}
#endif


#endif
