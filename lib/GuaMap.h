#ifndef GuaMap_h
#define GuaMap_h

#include "GuaType.h"
#include "GuaArray.h"
#include "GuaString.h"
#include <stdbool.h>


#pragma mark - type declarations
struct _GuaMap;
typedef struct _GuaMap GuaMap;



#pragma mark - methods of GuaMap

GuaMap *
GuaMap_new(void);

void *
GuaMap_get(GuaMap *map, GuaString *key);

void
GuaMap_put(GuaMap *map, GuaString *key, void *value);

GuaArray *
GuaMap_keys(GuaMap *map);

GuaArray *
GuaMap_values(GuaMap *map);

bool
GuaMap_has(GuaMap *map, GuaString *key);

void
GuaMap_remove(GuaMap *map, GuaString *key);

void
GuaMap_release(GuaMap *map);

#endif /* GuaMap_h */
