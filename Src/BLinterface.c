/*
 * BLinterface.cpp
 *
 *  Created on: 8 dic. 2017
 *      Author: juanra
 */


#include "BLInterface.h"
#include "NBinterface.h"

int	update_firmware			= 1;
int	update_firmware_counter = 9;
const char 	*fw_version		= "EC-M2M-F2_V1.1";
const char	*updfw_server 	= "sinapseenergia.com";
int	updfw_port 				= 80;
const char	*updfw_protocol	= "http";
const char	*updfw_route	="/";
const char	*updfw_name		="FIRMWARE.IMG";
const char	*updfw_user		="test";
const char	*updfw_password	="test";

const char	*gpio_status 	= "00000000";
int	PWM_status				= 0;



int ValidateIP(const char *param){
	int n1, n2, n3, n4;
	int n = sscanf((const char *)param, "%d.%d.%d.%d", &n1, &n2, &n3, &n4);
	if (n == 4
		&& n1 >= 0 && n1 <= 255
		&& n2 >= 0 && n2 <= 255
		&& n3 >= 0 && n3 <= 255
		&& n4 >= 0 && n4 <= 255)
		return 1;
	else {
		// sign on error ..
		return 0;
	}
}

int ValidateDNS(const char *param){
	if (isalpha(*param)) {
		while (*param) {
			if (isalnum(*param) || *param == '-' || *param == '.')
				param++;
			else
				return 0;
		}
		return 1;
	}
	else
		return 0;
}
int ValidateProtocol(const char *param){
	const char *protos[]= {"http", 0};
	int i = 0;
	while (protos[i]) {
		if (!strcmp(param, protos[i])){
			return 1;
		}
		i++;
	}
	return (0);
}
int ValidatePath(const char *param){
	int nslash = 0;
	while (*param) {
		if (*param == '/'){
			nslash++;
			param++;
		}
		else {
			if (isalnum(*param) || *param == '-' || *param == '_')
				param++;
			else
				return 0;
		}
	}
	return nslash;
}
int ValidatePort(const char *param){
	int val = atoi(param);
	if (val >= 1 && val <= 65535 )
		return (1);
	else
		return 0;
}
int ValidateProg(const char *param){
	const char *p = strchr(param, '.');
	if (p) {
		while (param < p) {
			if (isalnum(*param) )
				param++;
			else
				return 0;
		}
		return (!strcmp(p, ".bin"));
	}
	return 0;
}
int ValidateUser(const char *param){
	if (!strcmp(param, "-1"))
		return 1;
	else
		return 0;
}
int ValidatePassword(const char *param){
	if (!strcmp(param, "-1"))
		return 1;
	else
		return 0;
}
int ValidateCount(const char *param){
	return (*param);
}




int	ValidateFwParamsOLD(){
//	char *par;
	if (
		(ValidateDNS(GetVariable("pUPDFW_HOST")) || ValidateIP(GetVariable("pUPDFW_HOST")))
		&& ValidatePort(GetVariable("pUPDFW_PORT"))
		&& ValidateProtocol(GetVariable("pUPDFW_PROTO"))
		&& ValidatePath(GetVariable("pUPDFW_ROUTE"))
		&& ValidateProg(GetVariable("pUPDFW_NAME"))
		&& ValidateUser(GetVariable("pUPDFW_USER"))
		&& ValidatePassword(GetVariable("pUPDFW_PSWD"))
		&& ValidateCount(GetVariable("pUPDFW_COUNT"))
		){
			SetVariable("UPDFW_PROTO", GetVariable("pUPDFW_PROTO"));
			SetVariable("UPDFW_HOST", GetVariable("pUPDFW_HOST"));
			SetVariable("UPDFW_PORT", GetVariable("pUPDFW_PORT"));
			SetVariable("UPDFW_ROUTE", GetVariable("pUPDFW_ROUTE"));
			SetVariable("UPDFW_NAME", GetVariable("pUPDFW_NAME"));
			SetVariable("UPDFW_USER", GetVariable("pUPDFW_USER"));
			SetVariable("UPDFW_PSWD", GetVariable("pUPDFW_PSWD"));
			SetVariable("UPDFW_COUNT", GetVariable("pUPDFW_COUNT"));
			SetVariable("RESULT", "0");

			SaveBLParams();

			return 1;
	}
	else {
		SetVariable("RESULT", "5");
		return 0;
	}
}

int	ValidateFwParams(){
//	char *par;
	if ( ValidateDNS(GetVariable("pUPDFW_HOST")) || ValidateIP(GetVariable("pUPDFW_HOST"))) {
		if ( ValidatePort(GetVariable("pUPDFW_PORT"))) {
			if (ValidateProtocol(GetVariable("pUPDFW_PROTO"))) {
				if (ValidatePath(GetVariable("pUPDFW_ROUTE"))) {
					if (ValidateProg(GetVariable("pUPDFW_NAME"))) {
						if (ValidateCount(GetVariable("pUPDFW_COUNT"))) {
							if (ValidateUser(GetVariable("pUPDFW_USER")) && ValidatePassword(GetVariable("pUPDFW_PSWD"))) {
									SetVariable("UPDFW_PROTO", GetVariable("pUPDFW_PROTO"));
									SetVariable("UPDFW_HOST", GetVariable("pUPDFW_HOST"));
									SetVariable("UPDFW_PORT", GetVariable("pUPDFW_PORT"));
									SetVariable("UPDFW_ROUTE", GetVariable("pUPDFW_ROUTE"));
									SetVariable("UPDFW_NAME", GetVariable("pUPDFW_NAME"));
									SetVariable("UPDFW_USER", GetVariable("pUPDFW_USER"));
									SetVariable("UPDFW_PSWD", GetVariable("pUPDFW_PSWD"));
									SetVariable("UPDFW_COUNT", GetVariable("pUPDFW_COUNT"));
									SetVariable("RESULT", "0");
									SaveBLParams();
									return 1;
							}
							else {
								SetVariable("RESULT", "1");
								return 0;
							}
						}
						else {
							SetVariable("RESULT", "5");
							return 0;
						}
					}
					else {
						SetVariable("RESULT", "5");
						return 0;
					}
				}
				else {
					SetVariable("RESULT", "4");
					return 0;
				}
			}
			else {
				SetVariable("RESULT", "3");
				return 0;
			}
		}
		else {
			SetVariable("RESULT", "2");
			return 0;
		}
	}
	else {
		SetVariable("RESULT", "2");
		return 0;
	}


}


int	ValidateFwAuth(){
	char *par;
	if ( ValidateUser(GetVariable("pUPDFW_USER")) && ValidatePassword(GetVariable("pUPDFW_PSWD")) ){
			SetVariable("UPDFW_USER", GetVariable("pUPDFW_USER"));
			SetVariable("UPDFW_PSWD", GetVariable("pUPDFW_PSWD"));
			SetVariable("UPDFW", "1");
			SetVariable("RESULT", "0");

			SaveBLParams();

			return 1;
	}
	else {
		SetVariable("RESULT", "5");
		return 0;
	}
}

