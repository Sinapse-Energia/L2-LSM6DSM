/*
 * context.h
 *
 *  Created on: May 26, 2017
 *      Author: External
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#include	<time.h>
#include	"variant.h"

typedef time_t	TIMESTAMP;



// Clase Variable
class CVariable {
	public:
		//  Constructor based on a Variant as mark of type
							CVariable	(const char *name, const char *nick, CVariant *contents);
		// Alternate way of cosntructing Variables, by enum and Factory (no initial value, thus)
		enum				eTypes { Tint, Tshort, Tlong, Tbool, Tdec, Tstring, Tnpla, Tseq };
		static	CVariable	*VarFactory	(char *nombre, char *nick, CVariable::eTypes);

		CVariable			*write		(const char *id, unsigned char options = 0);
		char 				*read		(bool);
		const char			*Name()		{return name; };
		const char			*Id()		{return nick; };
		static CVariable	*get		(const char *id);
//		static	char		*getAll		(void);
		class CSubspace		*NS			() { return ns;};
		CVariant			*Content	() { return value; };
		unsigned int		units()		{ return value->units(); }

		static void			Clean();
	private:
		const char			*name;
		const char			*nick;
		const char			*pref;		// pref del subspace
		class CSubspace		*ns;
		CVariant			*value;
//		bool				updated;
		TIMESTAMP			tstamp;
		unsigned short		lifetime;
//		reader				getter;
		bool				pk;

	public: // PROVISIONAL (just for global initializers)
		static CVariable	**Context;
		static CVariable	*Dummy;

};


//// ABSOLUTE MAX VALUES

// Maximum size of the Subspace "primary key"	( EPDIdent)
#define		KEY_SIZE			10

// Maximum number of Subspace Elements			( EPDs)
#define		MAX_TABLE_SIZE		5

//	Maximum nuber of "Subspace context entries) ( EPD variables )
#define		MAX_ITEMS			20

// Maximum number of Subspaces					(EPDs in the only one so far)
#define		MAX_SUBSPACES		5

// Maximum size of each swaping cell
#define		CELLSIZE			512


	typedef struct {
		char			id[KEY_SIZE];
		unsigned int	position;
		} table_entry;


// Class Subspace
class CSubspace {
	public:
	 static	CSubspace		*Factory	(const char *pr, CVariable	*item);
	 static	CSubspace		*get		(const char *pr);
	 static	int				FlushAll	();
		const char			*Name		()	{ return prefix;};
		CVariable			*Add		(CVariable *);
		int					SwapIn		(const char *, unsigned char flags = 0);
		int					SwapOut		(void);
		table_entry			*getEntry	(const char *id);
		unsigned int		number;
		table_entry			*index;
	static	int				RestoreAll	();
		int					Restore		();
	private:
		CSubspace (const char	*prefix);
	// Tuple to associate ID and position in Flash
		const char			*prefix;
		unsigned int		nitems;
		CVariable			*items[MAX_ITEMS];
	 static unsigned int	nsubspaces;
	 static CSubspace	*List[MAX_SUBSPACES];
};



///////////////////////////////////////////////////////////
//	This is just a (provisional) simple way of
//	linking variable names and their methods of evaluation
//	If the variable is in the array, return its function,
//	otherwise return NULL
///////////////////////////////////////////////////////////

struct	st_wrapper {
	const char	*nombre;
	char		*(* reader)(const char *);
	char		*(* writer)(const char *);
};

extern	struct	st_wrapper	dispatch[];



// compare key routine for Csubspace index
int	compare_entries(const void *_a, const void *_b);


extern  int		GenericReadElement(unsigned int n, unsigned char *to);
extern	int		GenericWriteElement(unsigned int n, unsigned char *to);


extern	"C" void	WriteData(char *p);



#endif /* CONTEXT_H_ */
