/*
 * variant.cpp
 *
 *  Created on: May 27, 2017
 *      Author: External
 */


#include "variant.h"


CVarint::CVarint(int value) {
	this->content = value;
}


CVariant	*CVarint::write (const char *txt){
	this->content = atoi(txt);
	return this;
}

char		*CVarint::tostring (){
	return itoa(this->content, resultado, 10);
}


CVardec::CVardec(double value) {
	this->content = value;
}


CVariant	*CVardec::write (const char *txt){
	this->content = atof(txt);
	return this;
}

char		*CVardec::tostring (){
	// 1 decimal place ad-doc.
	//	Easy to generalize to n, changin 10 and de format specifier

//	char	*resultado = (char *) malloc(8);

	int intp = (int) content;
	int fracp = (int) ((content - intp)* 10 );
	sprintf(resultado, "%d.%1d", intp, fracp);
	return resultado;
}



CVarstring::CVarstring(const char *value) {

#ifdef TFIJO
	if (value)
		strncpy (this->content,value, TFIJO-1);
	else 
		this->content[0] = 0;
#else
	if (value)
		this->content = strdup(value);
	else 
		this->content = NULL;
#endif

}

CVarstring::~CVarstring() {
#ifdef TFIJO
#else
	if (this->content ) {
		free (this->content);
	}
#endif
}


CVariant	*CVarstring::write (const char *txt){
#ifdef TFIJO
	strncpy (this->content,txt, TFIJO-1);
//	strcpy (this->content,txt);
#else
	if (this->content != txt) {  // prevent try to overwrite same string
		if (this->content)
			free (this->content);
		this->content = strdup(txt);
	}
#endif
	return this;
}


char		*CVarstring::tostring (){

	if (this->content) {
//		char *resultado = (char *) malloc (strlen(this->content) +1);
#ifdef	TFIJO
		strcpy (resultado, this->content);
		return resultado;
#else
		return this->content;
#endif

	}
	else {
//		char *resultado = (char *) malloc (1);
#ifdef	TFIJO
		strcpy (resultado, "");
		return resultado;
#else
		return NULL;
#endif


	}
}


CVartime::CVartime(int h, int m, int s) {
	hour = h;
	min = m;
	sec = s;
}


CVariant	*CVartime::write (const char *txt){
	if (txt) {
		int h=0, m=0, s=0;
		int x = sscanf(txt, "%d:%d:%d", &h, &m,  &s);  // easy to generalize a instance separator...
		hour = h;
		min = m;
		sec = s;
	}
	return this;
}

char		*CVartime::tostring (){
	sprintf (resultado, "%dH:%dM", (int) hour, (int) min);
	return resultado;
}


long		CVartime::Timestamp(){
	long result = (hour * 60 + min) * 60  + sec;
	return result;
}

/**
CVartuple::CVartuple (CVariant *v[], char sep){
	this->separator  = sep;
	this->parts[0] = NULL;
	this->parts[1] = NULL;
	if (v) {
		if (v[0]) {
			this->parts[0] = v[0];
			if (v[1]){
				this->parts[1] = v[1];
			}

		}
	}
}
**/


CVartuple::CVartuple (CVariant *v1, CVariant *v2, char sep ){
	this->separator  = sep;
	this->parts[0] = v1;
	this->parts[1] = v2;
	resultado = NULL;
}

CVartuple::~CVartuple() {
	delete parts[0];
	delete parts[1];
}


CVariant	*CVartuple::write (const char *txt){
	char *p = strchr(txt, this->separator);
	if (p) {
		*p = 0;
		p++;
	}
	if (this->parts[0]!= NULL) 
		this->parts[0]->write(txt);
	else
		this->parts[0] = new CVarstring(txt);

	if (p) {
		if (this->parts[1]) 
			this->parts[1]->write(p);
		else
			this->parts[1] = new CVarstring(p);
	}
	return this;
}


char		*CVartuple::tostring (){
	int longitud = 0;
	char *t1;
	if (this->parts[0]) {
		t1 = this->parts[0]->tostring();
		longitud += strlen(t1);
	}
	else {
		t1 = NULL;
	}
	char *t2;
	if (this->parts[1]) {
		t2 = this->parts[1]->tostring();
		longitud += strlen(t2);
	}
	else {
		t2 = NULL;
	}

	if (resultado) 
		free (resultado);
	resultado = (char *) malloc (longitud + 4);

	sprintf (resultado, "(%s%c%s)", t1?t1:"", this->separator, t2?t2:"" ); 
//	if (t1) free (t1);
//	if (t2) free (t2);
	return resultado;
}


CVarlist::CVarlist(unsigned int size, CVariant *spec, char sep){
	this->maxsize = size;
	this->separator  = sep;
	this->specimen = spec;
	this->parts = new CVariant *[size];
	this->number = 0;
	for (unsigned int i = 0; i < size; i++) {
		this->parts[i] = this->specimen->Clone();
	}
	resultado = NULL;


}

/**
CVarlist::CVarlist(unsigned int size, CVariant **elems, char sep, bool tupflag){
	this->maxsize = size;
	this->separator  = sep;
	this->flag = tupflag;
	unsigned int j = 0 ;
	if (elems) 
		do  j++; while (elems[j]);
	this->number = j;
	if (this->number) {
		this->parts = new CVariant *[j+1];
		for (unsigned int i = 0; i < j; i++) {
			this->parts[i] = elems ? elems[i] : NULL;
		}
	}
	else {
		this->parts = NULL;
	}



}
**/

CVariant	*CVarlist::write (const char *txt){
	char *txt0= strdup(txt);
	char *p = txt0;
	const char *s = p;
	unsigned int i = 0;
		
	int u = this->specimen->units();

	for (i=0; s[i]; s[i]==this->separator ? i++ : *s++);
	// if the count in not multiplo u, add 1 
	if (i%u)
		i++;

	this->number = i / u;

		i = 0;
	char *q = p;
	while (i < this->maxsize && p && *p ) {
		int x = u;
		while (x-- && q) {
			q = strchr(q, ';');
			if (q) 
				q = q + 1;
			else
				break;
		}
			if (q) {
			*(q-1) = 0;
			this->parts[i]->write (p);
			p = q;
			}
		else {
			// printf ("premature out of separators when %d out of %d\n", i , this->maxsize);
			this->parts[i]->write (p);
			p = q;
		}
			i++;
		}
		this->number = i;
		
	free (txt0);
	return this;
}

unsigned int	CVarlist::units() { 
//	return number;
	return maxsize * this->specimen->units();
}

char		*CVarlist::tostring (){
	unsigned int i = 0;
	if (resultado)
		free(resultado);
	resultado = (char *) malloc (1024) ; // OJO: ÑAPA!!!
	strcpy (resultado, "(");
	while (i < this->number) {
		if (i > 0) 
			strcat (resultado, ",");
		if (this->parts[i]) {
			char *x = this->parts[i]->tostring();
			strcat (resultado, x);
//			free (x);
		}
		i++;
	}
	strcat (resultado, ")");
	return resultado;
}


CVariant		*CVarlist::Item (unsigned int n){
	if (n < this->number) {
		return this->parts[n];
	}
	else
		return NULL;
}





