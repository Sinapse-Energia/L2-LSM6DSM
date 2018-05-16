/*
 * BLInterface.h
 *
 *  Created on: 8 dic. 2017
 *      Author: juanra
 */

#ifndef BLINTERFACE_H_
#define BLINTERFACE_H_

// Declarations for variables and functions taking part in the interface with the Bootloader
extern	int			update_firmware;
extern	int			update_firmware_counter;
extern	const char 	*fw_version;
extern	const char	*updfw_server;
extern	int			updfw_port;
extern	const char	*updfw_protocol;
extern	const char	*updfw_route;
extern	const char	*updfw_name;
extern	const char	*updfw_user;
extern	const char	*updfw_password;

extern	const char	*gpio_status;
extern	int			PWM_status;

// Functions to validate command parameters
extern int ValidateIP(const char *);
extern int ValidateDNS(const char *);
extern int ValidatePort(const char *);
extern int ValidateProtocol(const char *);
extern int ValidatePath(const char *);
extern int ValidateProg(const char *);
extern int ValidateUser(const char *);
extern int ValidatePassword(const char *);
extern int ValidateCount(const char *);



#ifdef __cplusplus
extern "C" {
#endif

	int		ValidateFwParams(void);		// action for 207; command
	int		ValidateFwAuth(void);		// action for 208; command

#ifdef __cplusplus
}
#endif



#endif /* BLINTERFACE_H_ */
