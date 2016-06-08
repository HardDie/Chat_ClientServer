#ifndef _LIST_H_
#define _LIST_H_
#include <stdlib.h>

struct usList_t;

typedef struct usList_t {
	struct usList_t *		leftNode_;
	struct usList_t *		rightNode_;
	void *					data_;
} usList_t;

int			ListFree( usList_t ** list );
int			ListPushBack( usList_t ** list, void *data );
int			ListPushFront( usList_t ** list, void *data );
int			ListSize( usList_t ** list );
void *		ListGetElement( usList_t ** list, int numElement );
int			ListDeleteElementAtNumber( usList_t ** list, int numElement );
int			ListDeleteElementAtData( usList_t ** list, void * data );
void *		ListPopBack( usList_t ** list );
void *		ListPopFront( usList_t ** list );

#endif
