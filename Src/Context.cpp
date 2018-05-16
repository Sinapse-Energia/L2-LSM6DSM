/*
 * Context.cpp
 *
 *  Created on: May 26, 2017
 *      Author: External
 */



#include	<stdio.h>
#include	<string.h>
#include	"context.h"


#include 	"NBinterface.h"
#include 	"wrsouth.h"


//		This is an out-of-Context variable,
//		is used as a last resocurce
//		when looking for a non-existing variable
//
CVariable *CVariable::Dummy = new CVariable ("Dummy",			"",		new CVarstring("???"));



//////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////
//	Convenient way to recover
//	a variable contents in text form
//	Has extern cdcl storage for C calling
/////////////////////////////////////////////
char	*GetVariable	(const char *name) {
	CVariable	*v = CVariable::get(name);
	if (v){
		char *result = v->read(false);
		return result;
	}
	else
		return NULL;
}

/////////////////////////////////////////////
//	Convenient way to store a value
//	in a variable
//	Has extern cdcl storage for C calling
/////////////////////////////////////////////
int		SetVariable	(const char *name, char *value){
	CVariable	*v = CVariable::get(name);
	if (v) {
		v->write(value);
		return 1;
	}
	else
		return 0;
}


CVarint			*GetIntVar	(const char *name){
	CVariable *var = CVariable::get(name);
	if (var) {
		CVariant *cont = var->Content();
		if (cont->Kind() == INT){
			return (CVarint *) cont;
		}
	}
	return NULL;
}

CVardec			*GetDecVar	(const char *name){
	CVariable *var = CVariable::get(name);
	if (var) {
		CVariant *cont = var->Content();
		if (cont->Kind() == DEC){
			return (CVardec *) cont;
		}
	}
	return NULL;
}

CVarstring			*GetStringVar	(const char *name){
	CVariable *var = CVariable::get(name);
	if (var) {
		CVariant *cont = var->Content();
		if (cont->Kind() == STRING){
			return (CVarstring *) cont;
		}
	}
	return NULL;
}
			
CVartime			*GetTimeVar	(const char *name){
	CVariable *var = CVariable::get(name);
	if (var) {
		CVariant *cont = var->Content();
		if (cont->Kind() == TIME){
			return (CVartime *) cont;
		}
	}
	return NULL;
}


CVartuple			*GetTupleVar	(const char *name){
	CVariable *var = CVariable::get(name);
	if (var) {
		CVariant *cont = var->Content();
		if (cont->Kind() == TUPLE){
			return (CVartuple *) cont;
		}
	}
	return NULL;
}



CVarlist			*GetListVar	(const char *name){
	CVariable *var = CVariable::get(name);
	if (var) {
		CVariant *cont = var->Content();
		if (cont->Kind() == LIST){
			return (CVarlist *) cont;
		}
	}
	return NULL;
}
	


/////////////// CLASS CVARIABLE IMPLEMENTATION ////////////

CVariable	**CVariable::Context  = NULL;


// Constructor
CVariable::CVariable (const char *_name, const char *_nick, CVariant *content) {
	this->nick = strdup(_nick);
	char	*nsx = strstr(_nick, "::");

	this->pk = false;
	if (nsx) {
		// This is a Subspace variable...
		char *tmp = strdup(_nick);
		tmp[nsx - _nick] = 0;
		this->pref = strdup(tmp);

		// To see if the variable is the PK of the Subspace
		if (!strcmp(this->pref, tmp+strlen(this->pref)+2)){
			this->pk = true;
		}
		this->ns = CSubspace::Factory(this->pref, this);
		free (tmp);
	}
	else
		this->ns = NULL;


	this->name = strdup(_name);

	this->nick = strdup(_nick);
	this->value  = content;
	this->tstamp = time(NULL);
	this->lifetime = 0;			// pending

}



// static member to recover a variable by its name
CVariable *CVariable::get(const char *id) {
	int i = 0;
	while (Context[i]) {
		if (!strcmp(Context[i]->nick, id))
			return Context[i];
		i++;
	}
	// printf ("Warning: Error buscando variable %s que no existe, retorno Dummy\n", id);
	return Dummy;
}



// method to recover the contents of the var, converted to string
char	*CVariable::read(bool evaluate)  {
	int traza = 0;
	if (1) {  // auto update on read
	time_t now = time(NULL);
	//	if (now - this->tstamp  > this->lifetime) {
	if (evaluate) {
			if (traza) printf ("VOY a evaluar %s porque se ha quedao viejo\n", this->name );
			const char *newval = GenericREAD(this);
			if (newval) {
				this->value->write (newval);
				this->tstamp = now;
			}
		}
	}
	return this->value->tostring();
}


// method to update the content of a variable, giving a stringized value
CVariable *CVariable::write (const char *txt, unsigned char options) {
	if (this->pk) {
		this->ns->SwapOut();
		if (this->ns->SwapIn(txt, options)) {
			this->value->write(txt);
		}
		else {
			return NULL;
		}
	}
	else {
		this->value->write(txt);  // on the own context variable
	}
	GenericWRITE(this, txt);
	this->tstamp = time(NULL);
	return this;
}

//////////////////////////////////////////////////////////////////////////////
//				SUBSPACES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//	GetSubspaceElement Function to know if a id is stored or is not stored 
//		in a Subspace
//	
void	*GetSubspaceElement	(const char *prefix, const char *id){
	table_entry	*result;
	CSubspace *psub = CSubspace::get(prefix);
	if (psub) {
		result =  psub->getEntry (id);
		return result;
	}
	else
		return NULL;
}



CSubspace	*CSubspace::List[MAX_SUBSPACES];
unsigned int	CSubspace::nsubspaces = 0;

CSubspace	*CSubspace::get (const char *pr) {
	unsigned int i = 0;
	while ((i < nsubspaces) && CSubspace::List[i]) {
		if (!strcmp(pr, CSubspace::List[i]->Name()))
			return CSubspace::List[i];
		else
			i++;
	}
	return NULL;
}


CSubspace	*CSubspace::Factory (const char *pr, CVariable *it) {
	CSubspace *result = get(pr);
	if (result == NULL) {
		result = new CSubspace(pr);
		CSubspace::List[nsubspaces++] = result;
	}
	result->Add(it);
	return result;
}

CSubspace::CSubspace (const char *pr){
	this->prefix = pr;
	this->number = 0;
	this->index = new table_entry[MAX_TABLE_SIZE];
	this->nitems = 0;
	this->items[0] = NULL;
}

int	CSubspace::Restore() {
	// After creating the table, there is to look for NVM records remaining
	while (number < MAX_TABLE_SIZE) {
		unsigned char	tmp[CELLSIZE];
		int rc = GenericReadElement(number, tmp);
		if (rc) { 
			char	pk[32];
			WriteData((char *) tmp);
			sprintf (pk, "%s::%s", prefix, prefix);
			char * xid = GetVariable(pk);
			this->index[number].position = number;
			strcpy (this->index[number].id, xid);
			this->number++;
		    qsort(index, number, sizeof(table_entry), compare_entries);
		}
		else 
			break;
	}
//	number = 0; 
	return number;
}


int	CSubspace::RestoreAll() {
	for (unsigned int i = 0; i < nsubspaces; i++) {
		printf ("Restoring el %d\n", i);
		List[i]->Restore(); 
	}
	return 1;
}

CVariable	*CSubspace::Add (CVariable *x) {
	if (nitems < MAX_ITEMS) {
		items[nitems++] = x;
		return x;
	}
	else
		return NULL;
}

table_entry			*CSubspace::getEntry	(const char *id){
	table_entry *result = (table_entry *) bsearch(id, this->index, this->number, sizeof(table_entry), compare_entries);
	return result;
}



// compare routine
int	compare_entries(const void *_a, const void *_b){
	table_entry *a = (table_entry *) _a;
	table_entry *b = (table_entry *) _b;
    return (strcmp(a->id , b->id));

}


char total[CELLSIZE];


////////////////////////////////////////////////////////////////////////////
//	Function, that takes the data stream from NVM 
//		splits it in elements, and, one by one, writes them into de Context
//	Formerly was part of Swap-In, but has been factorized-out because 
//		the same functionality is required in Restore
////////////////////////////////////////////////////////////////////////////  
void	WriteData(char *p) {
		while (p) {
			char	name[16];
			char	value[128];

			char *q = strstr(p, "=");
			if (q) {
				strncpy(name, p, q-p);
				name[q-p] = 0;
				p = q+1;
				q = strstr(p, ";");
				if (q) {
					strncpy(value, p, q-p);
					value[q-p] = 0;
					p = q+1;
					CVariable *x = CVariable::get(name);
//					x->Content()->write(value);
					x->write(value);
				}
				else
					p = 0;
			}
			else
				p = 0;
		}

}



int	CSubspace::SwapIn (const char *id, unsigned char options) {
//	table_entry *px = (table_entry *) bsearch(id, this->index, this->number, sizeof(table_entry), compare_entries);
	table_entry *px = this->getEntry(id);
	if (px) {
		// Recover the Subc variables form Flash/disc
		GenericReadElement(px->position, (unsigned char *) total);
		WriteData (total);
		return 1;
	}
	else {
		if ((number < MAX_TABLE_SIZE) && (options & 1)){
			index[number].position = number;
			strcpy (index[number].id, id);
			number++;
		    qsort(index, number, sizeof(table_entry), compare_entries);
			return 1;
		}
		else {
			// printf ("Se ha alcanzado el limite de registros\n");
			return 0;
		}
	}

}



int	CSubspace::SwapOut () {

	unsigned int		i =0;
	char	pk[32];
	sprintf (pk, "%s::%s", prefix, prefix);
	char * xid = GetVariable(pk);
	if (xid && *xid) {
		table_entry *px;
		unsigned int pos;
		total[0] = 0;
		total[CELLSIZE-1] = '\n';

		while (i < this->nitems ) {
			char	element[64];
			sprintf (element, "%s=%s;",  this->items[i]->Id(), this->items[i]->read(false) );
			if (strlen(total) + strlen(element) < CELLSIZE){
				strcat(total, element);
				i++;
			}
			else {
				break;
			}
		}
		strcat (total, "\n");

//		px = (table_entry *) bsearch(xid, this->index, this->number, sizeof(table_entry), compare_entries);
		px = this->getEntry(xid);
		if (px) {
			pos = px->position;
		}
		else {
			if (number < MAX_TABLE_SIZE) {
				index[number].position = number;
				strcpy (index[number].id, xid);
				pos = number;
				number++;
				qsort(index, number, sizeof(table_entry), compare_entries);
			}
			else
				return 0;
		}
		GenericWriteElement(pos, (unsigned char *) total);
		return 1;
	}
	else
		return 0;
}



int		CSubspace::FlushAll() {
	unsigned int i;
	int	flushes = 0;
	for (i = 0; i < nsubspaces;i++) {
		flushes += List[i]->SwapOut();
	}
	return flushes;
}


int		Flush() {
	return CSubspace::FlushAll();
}


