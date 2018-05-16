/*
 * Frames.cpp
 *
 *  Created on: May 26, 2017
 *      Author: External
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Frames.h"
#include "context.h"




#include "NBinterface.h"






//	class CFrame

CFrame		**CFrame::InAPI = NULL;
CFrame		**CFrame::OutAPI = NULL;
char		CFrame::separator1 = '%';
char		CFrame::separator2 = ';';




////////////////////////////////////////////////////////////////
//	Provisional plain function to resolve pre and post actions
//	 (those have to be converted to static methods asap)
////////////////////////////////////////////////////////////////
int		PreAction(CFrame *x) {
	int i = 0;

	x->Pre();


	while (actions[i].id ) {
		if (!strcmp(actions[i].id, x->Id()))
			if (actions[i].pre)
				return actions[i].pre();
			else
				return 0;
		else
			i++;
	}
		return 0;
}

int		PostAction(CFrame *x) {
	int i = 0;

	x->Post();

	while (actions[i].id ) {
		const char *id = actions[i].id;
		if (!strcmp(id, x->Id()))
			if (actions[i].post){
				int rc = actions[i].post();
				return rc;
			}
			else
				return 0;
		else
			i++;
	}
		return 0;
}

// Read one CSV line of up to FOUR FIELDS (ID, TEMPLATE, NAME, REPLAY)
//	without caring about delimited fields
/*
CFrame	**ReadFrames1(char **lineas) {
	int numero = 0;
	// first get the number of lines
	for (char **t = lineas; t[numero]; numero++);

	CFrame **result = new CFrame * [numero+1];  //??

	int it = 0;  // index for objects
	char sep1 = '%';
//	char sep2 = ';';


	int i = 0;
	char *p ;
	while (i < numero) {
		p = strdup(lineas[i]);
		int cpn = 0;  //	index for fields
		char *cps[6] = {0}; //	six seems to be more than enough !!!
		char *x = p;
		while (x) {
			char *y = strchr(x, sep1);
			if (y) {
				*y++ = 0;
			}
			cps[cpn++] = x;
			x = y;
		}

		if (cpn >= 3) {
			result[it++] = new CFrame (cps[0], cps[1], cps[2], cps[3]);
		}
		result[it] = NULL;

		i++;
	}


	return result;
}
*/

// Read one CSV line of up to FIVE FIELDS
//	(ID, TEMPLATE, NAME, TOPIC, REPLAY)
//	without caring about delimited fields
CFrame	**ReadFrames2(const char **lineas) {
	int numero = 0;
	// first get the number of lines
	for (const char **t = lineas; t[numero]; numero++);

	CFrame **result = new CFrame * [numero+1];  //??

	int it = 0;  // index for objects
	char sep1 = '%';
//	char sep2 = ';';


	int i = 0;
	char *p ;
	while (i < numero) {
		p = strdup(lineas[i]);
		int cpn = 0;  //	index for fields
		char *cps[6] = {0}; //	//	six seems to be more than enough !!!
		char *x = p;
		while (x) {
			char *y = strchr(x, sep1);
			if (y) {
				*y++ = 0;
			}
			cps[cpn++] = x;
			x = y;
		}

		if (cpn >= 3) {
			result[it++] = new CFrame (cps[0], cps[1], cps[2], cps[3], cps[4]);
		}
		result[it] = NULL;

		i++;
	}
	return result;
}

/**
int	 ReadMetadata0(char *dominioIn, char *dominioOut){

	static CFrame  *OUTAPIS[] = {
		new CFrame ("MODMD", "999;$MSSG;Unknown message", "Error"),
		new CFrame ("MEASS", "$ID;$TEMP;$STAT;$DSTAT;$A;$V;$P;$Q;$S;$EP;$EQ;$ES;$F;$TIMESTAMP",  "periodic measurement"),
		NULL
	};
	CFrame::OutAPI = OUTAPIS;

	static CFrame  *INAPIS[] = {
		new CFrame ("MODMD", "1;", "pull measurement", "MODMD"),
		new CFrame ("MINTV","4;$INTERVAL", "measurement interval"),
		new CFrame ("MCOMM", "5;$IP;$PORT", "configure communications"),
		NULL
	};
	CFrame::InAPI  =  INAPIS;
	return 1;
}
**/

// This is a PROVISIONAL way of Handling incoming messages, and their potential responses
char	*ProcessMessage (char *messagein) {
	static CVariable *msg = CVariable::get("MSSG");
	msg->write(messagein);


	CFrame *request = CFrame::Match(messagein);

	if (request) {
		// Message well known
//		request->Pre();
		PreAction(request);
		if (request->Execute (messagein)) {
			int rc = PostAction(request);
//			request->Post();
			if (rc >= 0){
				CFrame *replay = request->Replay();
				if (replay) {
					// incoming message has an immediate  reply
					char	*messageout = replay->Build();
					return messageout;
				}
				else {
					// mssg has no reply
					return NULL;
				}
			}
			else {
				// Command "refused"
				// return "000;REFUSED by APP"; // OJO MUY PROVISIONAL
				LogFrame("BAD"); // PROVISIONAL de momento...
				return NULL;
			}
		}
		else {
			// error in Exec
			return NULL;
		}
	}
	else {
		// this is a placeholder for ad-hoc message processing
		// So far, we send back a error message
//		CFrame *error = CFrame::OutAPI[0];
		/**
		CFrame *error = CFrame::Get("ERROR");
		char	*messageout = error->Build();
		Log(messageout);
		**/
		LogFrame("ERROR");
		return NULL;
	}
}


///////////////////////////////////////
//	Constructor
//
///////////////////////////////////////
CFrame::CFrame(const char *_id, const char	*_description, const char *_name, const char *topic, const char  *replaycode){
	int traza = 0;
	char *description = strdup (_description);
	char *p = description;
	// int counter = 0;
	char separator = CFrame::separator2 ;

	this->id = strdup(_id);
	if (_name)
		this->name = strdup(_name);
	else
		this->name = NULL;

	if (replaycode && *replaycode) {
		this->replay = CFrame::Get(replaycode);
	}
	else {
		this->replay = NULL;
	}

	this->subx = NULL;
#ifdef LINEAL
	int i;
	for ( i=0; p[i]; p[i] == separator ? i++ : *p++);
	p = description;

	this->Segments = new  CSegment * [i+2];
	i = 0;
	char *q = p;
	while ((q = strchr (p, CFrame::separator2)) != NULL) {
		*q = 0;
		q++;
		if (traza)  printf ("I get %s and remains  %s\n", p, q);
		CSegment *x = CSegment::Factory(p);
		if (x->Sub())
			this->subx = x->Sub();
		this->Segments[i++] = x;

		p = q;
	}
	if (p &&*p)
		this->Segments[i++] = CSegment::Factory(p);

	this->Segments[i] = NULL;

#else

	this->head = NULL;
	this->tail = NULL;

	char *q = p;
	while (q = strchr (p, CFrame::separator2)) {
		*q = 0;
		q++;
		if (traza)  printf ("I get %s and remains  %s\n", p, q);
		this->append (CSegment::Factory(p));
		p = q;
	}
	if (p &&*p)
		this->append (CSegment::Factory(p));
#endif
	free (description);
}

#ifndef	LINEAL
// Append a segment at the end of the list
CSegment	*CFrame::append (CSegment *x) {
	if (this->tail) {
		this->tail->linkto(x);
		this->tail = x;
	}
	else {
		this->head = this->tail = x;
	}
	return this->tail;
}
#endif




CFrame		*CFrame::Get(const char *texto) {
	int traza = 0;
	if (traza) printf ("GET of ...%s\n", texto );
	int i = 0;
	while(OutAPI[i]) {
		if (traza) OutAPI[i]->show();
		if (!strcmp(OutAPI[i]->id, texto)){
			return OutAPI[i];
		}
		i++;
	}
	//printf ("PANIC: Frame %s ndoesn't match any known command \n", texto);
	return NULL;
}

CFrame		*CFrame::Match (const char *texto) {
	int traza = 0;
	if (traza) printf ("FETCH  ...%s\n", texto );
	int i = 0;
	while(InAPI[i]) {
		if (traza) InAPI[i]->show();
#ifdef LINEAL
		if (InAPI[i]->Segments[0]->Match(texto)) {
			return InAPI[i];
		}
#else
		if (InAPI[i]->head->Match(texto)) {
			return InAPI[i];
		}
#endif

		i++;
	}
	//printf ("PANIC: Frame %s ndoesn't match any known command \n", texto);
	return NULL;
}


bool	CFrame::Execute(const char *message){
//	int traza = 0;
	
	const char	*p = message;
#ifdef LINEAL
	int i = 0;
	while (Segments[i] && p) {
		p = Segments[i]->Execute(p);  
		if (p && *p)
			p++;		// 1 more to skip the separator
		else
			break;
//			return false;
		i++;
	}
	return true;
#else
	CSegment *e = this->head;
	while (e) {
		p = e->Execute(p) ;			
		if (p && *p)
			p++;		// 1 more to skip the separator
		else
			break;
//			return false;
		e = e->Next();
	}
	return true;
#endif
}



char	*CFrame::Build(char *dest){
//	int traza = 0;
	static char	result[1024];

	char *p = (dest?dest:result);
	*p = 0;
#ifdef LINEAL
	int i = 0;
	while (Segments[i]) {
		char *x = Segments[i]->Build();
		if (x && strlen(x)) {
			strcat (p, x);
		}
		strcat (p, ";");				// put ; always, even in empty segments
		i++;
	}

#else
	CSegment *e = this->head;
	while (e) {
		strcat (p, e->Build());
		strcat (p, ";");				//@ PROVISIONAL
		e = e->Next();
	}
#endif
	return p;
}


int	CFrame::Pre() {
	return 0;
}


int	CFrame::Post() {
	if (this->subx) {
		this->subx->SwapOut();
		return 1;
	}
	else {
		return 0;
	}
}


CFrame::~CFrame() {
	free (this->name);
#ifdef LINEAL
	delete [] this->Segments ;
#else
	CSegment *e = this->head;
	if (e)
		delete e;
#endif
}



/////////////////////////////////////////////////////////////////



//		Abstract Factory
//		If text begin with any sigil ($ or @), construct a Var or a List
//		else a Text
CSegment	*CSegment::Factory(char *text) {
	if (text[0] == '$') {
		if (text[1] == '$') 
			return new CVarSegment(text+2, true);
		else 
		return new CVarSegment(text+1);
	}
	else if (text[0] == '@') {
		return new CListSegment(text+1);
	}
	else {
		return new CTextSegment(text);
	}
}




CSegment::CSegment() {
#ifndef LINEAL
	this->next = NULL;
#endif
}

CSegment::~CSegment() {
#ifndef LINEAL
	if (this->next )
		delete this->next;
#endif
}

///////////////////////////////////////////////

// Constructor trivial
CTextSegment::CTextSegment(const char *txt) : CSegment() {
	this->text  = strdup(txt);
}


const char	*CTextSegment::Execute(const char *txt){
	int traza = 0;
	if (traza) printf ("Text %s against %s\n", text, txt);
	if (!strncmp(text, txt, strlen(text)))
		return txt + strlen(text);
	else
		return 0;
}


char	*CTextSegment::Build(){
	return this->text;
}

CTextSegment::~CTextSegment() {
	free (this->text);
}



// CORRECTIVE
#include <ctype.h>
bool	CTextSegment::Match(const char *x) {
	int m = strncmp(x, this->text , strlen(this->text));
	if (m == 0) {
		int s = strlen(this->text);
		if (isalnum(x[s]))
			return 0;
		else
			return 1;
	}
	else
		return 0;
}



////////////////////////////////////////////////


// Constructor looks into collection and populates the pointer
CVarSegment::CVarSegment(const char *nombre, bool grant) : CSegment() {
	id = CVariable::get(nombre);
	flags = grant;
}


const char	*CVarSegment::Execute(const char *txt) {

	int traza = 0;
	char	tmp[256];
	const char *p = txt;
	const char *q;
	CVariable	*result;

	if (traza) printf ("Get the variable value for %s after %s \n", this->id->Name(), p);
	int x = this->id->units();
	q = p;
	while (x-- && q) {
		q = strchr(q, ';');
		if (q) 
			q = q + 1;
		else
			break;
	}
	// q sale sobre el siguiente al ; (o nulo)

#ifdef LINEAL

//	q = strstr(p, ";");
	if (q) {
		q = q-1;
		strncpy(tmp, p, q-p);
		tmp[q-p] = 0 ;
		if (traza)  printf ("...that is %s\n", tmp);
		result = this->id->write(tmp, flags);
		p = q;
	}
	else {
		if (traza) printf ("All the remainder: var. %s gets %s\n", this->id->Name(), p);
		result =  this->id->write(p, flags);
		return p + strlen(p);
	}

#else
	if (this->Next() && q) {
		q = q -1;
//		q = strstr(p, ";");
		strncpy(tmp, p, q-p);
		tmp[q-p] = 0 ;
		if (traza)  printf ("...that is %s\n", tmp);
		result = this->id->write(tmp);
		p = q;
	}
	else {
		if (traza) printf ("All the remainder: var. %s gets %s\n", this->id->Name(), p);
		result =  this->id->write(p, flags);
		return p + strlen(p);
	}
#endif
	if (result)	
	return (q);
	else
		return NULL;
}









char	*CVarSegment::Build(){
	return this->id->read(1);
}

CSubspace	*CVarSegment::Sub () {
	return this->id->NS();
}






void	CFrame::show (void) {
#ifdef	DEBUG
	printf ("FRAME .%s..composed of :\n", this->name);
#ifdef LINEAL
	int i = 0;
	while (this->Segments[i]) {
		this->Segments[i]->show();
		i++;
	}
#else
	CSegment *e = this->head;
	while (e) {
		e->show();
		e = e->Next();
	}

#endif
	printf ("\n");
#endif
}

// Show para debug
void	CTextSegment::show() {
	printf ("Boilerplate text #%s#\n", this->text);
}


// Show para debug
void	CVarSegment::show() {
	printf ("Variable  %s\n", this->id? this->id->Name(): "???");
}


// Show para debug
void	CListSegment::show() {
	printf ("Variable List %s\n", this->inner? this->inner->Id(): "???");
}



char	*replace(char *str, char f, char r) {
	char *x = strchr(str, f);
	while (x) {
		*x = r;
		x = strchr(str, f);
	}
	return str;
}



// List Segment Constructor
CListSegment::CListSegment(const char *_n) : CSegment() {
	const char *nombre = strdup(_n);
	char *p = strchr(nombre, '(');

	char prefix[16];
	strncpy (prefix, nombre, p - nombre);
	prefix [p-nombre] =0;
	char *lista = replace (p+1, ',', ';');
	p =strchr(p, ')');
	*p = 0;

	printf ("Pref es %s y lista %s\n", prefix, lista);

//	this->id = CVariable::get(nombre);
	this->sub = CSubspace::get(prefix);
	this->inner = new CFrame("tmp", lista);
}


const char	*CListSegment::Execute(const char *txt) {
	return NULL;
}


char	*CListSegment::Build(){
	static char	aux[2048];
	char *dest = aux;
	*dest = 0;
	unsigned int i = 0;
	while (i < this->sub->number) {
		this->sub->SwapIn(this->sub->index[i].id);

		char *r = this->inner->Build(dest);
		dest = dest + strlen(dest);
		i++;
	}
	return aux;
}

// Provisional placement for this

int	SendFrame(const char *tag) {
	CFrame *cmd = CFrame::Get(tag);
	if (cmd) {
		// TO DO: retrieve the topic, and use in Publish
		char	*mssg = cmd->Build();
		int rc = Publish(mssg);
		return rc;
	}
	else
		return -1;
}

int	LogFrame(const char *tag) {
	CFrame *cmd = CFrame::Get(tag);
	if (cmd) {
		// TO DO: retrieve the topic, and use in Publish
		char	*mssg = cmd->Build();
		int rc = Log(mssg);
		return rc;
	}
	else
		return -1;
}

