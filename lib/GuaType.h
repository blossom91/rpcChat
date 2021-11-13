#ifndef GuaType_h
#define GuaType_h

#pragma mark - type declarations
typedef enum {
    GuaTypeNumber=0,
    GuaTypeString,
    GuaTypeArray,
    GuaTypeMap,
} GuaType;

typedef enum {
    GuaNumberTypeInt=0,
    GuaNumberTypeUnsignedInt,
    GuaNumberTypeLong,
    GuaNumberTypeUnsignedLong,
    GuaNumberTypeFloat,
    GuaNumberTypeDouble,
} GuaNumberValueType;



#pragma mark - methods of GuaType
GuaType
GuaGetType(void *v);


#endif /* GuaType_h */
