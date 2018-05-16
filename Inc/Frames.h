/*
 * Frames.h
 *
 *  Created on: May 26, 2017
 *      Author: External
 */

#ifndef FRAMES_H_
#define FRAMES_H_
#include "Definitions.h"


// HELPER functions
// Build and send an output frame over publish topic
extern int		SendFrame(const char *tag);

// Build and send an output frame over log topic
extern int		LogFrame(const char *tag);


//class CFrame;
class CSegment;


////////////////////////////////////////////////////////////////////////
//	class CFrame : Internal representation of API Metadata
//		Is built from text definition of the API (name, descriptor, ...)
//		It consist of a list of CSegments
////////////////////////////////////////////////////////////////////////


//	LINEAL/NOLINEAL is a preprocessor choice for alternative ways of
//			implementing the CSements in a CFrame
//		LINEAL :	as an array of pointers
//		NOLINEAL:	as a linked list os CSegments
#define		LINEAL



class	CFrame {
 public:
						CFrame		(const char *id, const char	*descriptor, const char *name=NULL, const char *topic=NULL, const char *replaycode = NULL);
						~CFrame		();
	CFrame				*Replay		() { return replay; };
	bool				Execute		(const char	*message);
	static	CFrame		*Get		(const char *);
	static	CFrame		*Match		(const char *);
	char				*Build		(char *dest = NULL);
	char				*Id			() { return this->id;};  // to extinguish when getPre and getPost become methods

	int					Pre			();
	int					Post		();


#ifndef LINEAL
	CSegment			*append		(CSegment *element);
#endif

	void				show		();

	static	void		Clear		();
 private:
	char				*id;
	char				*name;
	CFrame				*replay;
	class CSubspace		*subx;
#ifdef LINEAL
	CSegment			**Segments;
#else
	CSegment			*head;
	CSegment			*tail;  // acelerador
#endif
	static char			separator1;
	static char			separator2;


 public: // PROVISIONAL (just for global initializers)
	static	CFrame		**InAPI;
	static	CFrame		**OutAPI;
};



class CSegment {
 public:
	static CSegment		*Factory(char *);
	virtual				~CSegment();
	virtual	char		*Build()	= 0;
	virtual	bool		Match	(const char *text) {return false;};

	virtual	void		show	() = 0;
	virtual	const char	*Execute(const char *text) = 0;
	virtual	class CSubspace *Sub()  {return NULL; };

#ifndef	LINEAL
	CSegment			*linkto	(CSegment *x)  {	this->next = x; return this; };
	CSegment			*Next	() { return this->next; };
#endif
 protected:
	CSegment			();
 private:
#ifndef	LINEAL
	CSegment			*next;
#endif
};



class CTextSegment : public CSegment {
 public:
	CTextSegment (const char *);
						~CTextSegment ();
	void				show		();
	bool				Match		(const char *text);
	const char			*Execute	(const char *text);
	char				*Build		();
 private:
	char				*text;
};

class CVarSegment : public CSegment {
 public:
						CVarSegment (const char *n, bool p = false);
//						~CVarSegment (void);
	void				show		();
//	bool				Match		(const char *text);
	const char			*Execute	(const char *text);
	char				*Build		();
	class  CSubspace	*Sub		();
 private:
	class CVariable		*id;
	unsigned char		flags;	
};


class CListSegment : public CSegment {
 public:
	CListSegment (const char *);
//						~CVarSegment (void);
	void				show		();
//	bool				Match		(const char *text);
	const char			*Execute	(const char *text);
	char				*Build		();
//	class  CSubspace	*Sub		();
 private:
	class CFrame		*inner;
	class CSubspace		*sub;
};


CFrame	**ReadFrames2(const char **lineas);




///////////////////////////////////////////////////////////
//	This is the simplest way to implement the pre and post
//		actions associated to commands.
//	This structure can optionally assign a pair
//		of function pointers to each CFrame object.
//	As far as the action need real code to link with,
//		there no is effective possibility of declaring it
//		whithin the metadata, and puting the pointers as
//	constructor's parameter is more confusing and more anoying
//	The pointers have to have the same signature
//		(no parameters and integer return code)
///////////////////////////////////////////////////////////

struct	st_fractions {
	const char		*id;

//	int				(* pre)();
//	int				(* post)();
	Action				pre;
	Action				post;
};


extern	struct st_fractions actions[];





#endif /* FRAMES_H_ */
