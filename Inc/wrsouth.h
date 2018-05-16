/*
 * wrsouth.h
 *
 *  Created on: 2 jun. 2017
 *      Author: juanra
 */

#ifndef WRSOUTH_H_
#define WRSOUTH_H_


#ifdef __cplusplus
extern "C" {
#endif

		char			*BuildListEPD	(const char *);


#ifdef __cplusplus
}
#endif



// Common facade for all READING wrappers
extern const char	*GenericREAD(class CVariable *v);


// Common facade for all WRITING  wrappers
extern const char	*GenericWRITE(class CVariable *v, const char *val);


extern	char		*Read_Temp(const char *);
extern	char		*Read_I(const char *);
extern	char		*Read_V(const char *);
extern	char		*Read_P(const char *);
extern	char		*Read_Q(const char *);
extern	char		*Read_S(const char *);
extern	char		*Read_EP(const char *);
extern	char		*Read_EQ(const char *);
extern	char		*Read_ES(const char *);

extern	char		*Write_DSTAT(const char *);


#endif /* WRSOUTH_H_ */
