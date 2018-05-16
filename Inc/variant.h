/*
 * variant.h
 *
 *  Created on: May 27, 2017
 *      Author: External
 */

#ifndef VARIANT_H_
#define VARIANT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>




#undef		TFIJO

enum	Varkind { INT, DEC, STRING, TIME, TUPLE, LIST };

/***************************************************
	Base class (abstract)
	just define the class interface
****************************************************/
class CVariant	{
	public :
		virtual CVariant	*write (const char *txt) = 0;
		virtual				~CVariant	()	{};
		virtual char		*tostring () = 0;
		virtual CVariant	*Clone		()	= 0;
		virtual unsigned int units		() { return 1; }
		virtual	Varkind		Kind		()  = 0;
	protected :
#ifdef	TFIJO
		char				resultado[TFIJO];
#else

#endif


							CVariant() {};

};


/***************************************************
	class CVarint (integer content)
****************************************************/
class CVarint : public CVariant {
	public :
					CVarint		(int value = 0);
		CVariant	*Clone		()	{ return new CVarint(content);};
		CVariant	*write (const char *txt);
		char		*tostring ();
		Varkind		Kind	()	{return INT;}
		int			Value		() { return content;}
	private :
		char		resultado[16];
		int			content;
};


/***************************************************
	class CVardec (decimal content)
****************************************************/
class CVardec : public CVariant {
	public :
					CVardec		(double value = 0.0);
		CVariant	*Clone		()	{ return new CVardec( content);};
		CVariant	*write (const char *txt);
		char		*tostring ();
		Varkind		Kind	()	{return DEC;}
		double		Value		() { return content;}
	private :
		double		content;
		char		resultado[16];
};


/***************************************************
	class CVarstring (text content)
****************************************************/
class CVarstring : public CVariant {
	public :
						CVarstring	(const char *value = NULL);
						~CVarstring	();
		CVariant	*Clone		()	{ return new CVarstring( content);};
		CVariant	*write (const char *txt);
		char		*tostring ();
		Varkind			Kind	()	{return STRING;}
		const  char		*Value		()  { return content;}
	private :
#ifdef TFIJO
		char		content[TFIJO];
#else
		char		*content;
#endif
};



/***************************************************
	class CVartime (time content)
****************************************************/
class CVartime : public CVariant {
	public :
						CVartime	(int h = 0, int m = 0, int s = 0);
//						~CVartime	();
		CVariant	*Clone		()	{ return new CVartime( hour, min, sec);};
		CVariant	*write (const char *txt);
		char		*tostring ();
		Varkind			Kind		() {return TIME;}
		long			Value		() { return Timestamp();};
		long			Timestamp	();

	private :
	private :
		char		resultado[16];
		unsigned char hour;
		unsigned char min;
		unsigned char sec;

};


/***************************************************
	class CVartuple (tuple of Variants)
****************************************************/
class CVartuple : public CVariant {
	public :
						CVartuple	(CVariant *v1 = NULL, CVariant *v2 = NULL, char sep = ';' );
						~CVartuple	();
		CVariant		*Clone		()	{ return new CVartuple( parts[0]->Clone(), parts[1]->Clone(), separator);};
		CVariant		*write (const char *txt);
		char			*tostring ();
		Varkind			Kind		() {return TUPLE;}
		CVariant		*Item		(unsigned int n) { return ((n<2)? parts[n]:NULL); };
		unsigned int	units		() { return 2; } ;
	private :
		char			separator;
		char			*resultado;
		CVariant		*parts[2];
};


/***************************************************
	class CVarlist (variable size list os Variants)
****************************************************/
class CVarlist : public CVariant {
	public :
//						CVarlist(unsigned int size, CVariant **velems = NULL, char sep = ';', bool tupflag = false );
						CVarlist	(unsigned int size, CVariant *specimen, char sep = ';' );
		CVariant		*Clone		()	{ return new CVarlist( maxsize, specimen, separator);};  // FUDGE
		CVariant		*write		( const char *txt );
		char			*tostring	();
		Varkind			Kind		() {return LIST;}
		unsigned int	size		() { return number; };
		CVariant		*Item		(unsigned int n);
		unsigned int	units		(); // { return number; }
	private :
		char			separator;
		char			*resultado;
		CVariant		*specimen;
		unsigned int	maxsize;
		unsigned int	number;
		CVariant		**parts;
};


	


#endif /* VARIANT_H_ */
