#ifndef UTJSON_H
#define UTJSON_H

#define _GNU_SOURCE

#include "uthash.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef VERSION
#define VERSION "0.0.0"
#endif

#define FREE_AND_NULL(x) \
    if (x)               \
        free(x);         \
    x = NULL

typedef enum
{
    utjson_NULL,
    utjson_BOOL,
    utjson_NUMBER,
    utjson_STRING,
    utjson_ARRAY,
    utjson_OBJECT
} utjson_type;
#define utjson_IS(TYPE, object) (object && utjson_##TYPE == (object)->type)

#define utjson_ARRAY_INCREMENT 16

typedef struct utjson
{
    char *name;
    UT_hash_handle hh; /* makes this structure hashable */
    struct utjson *parent;
    //
    utjson_type type;
    double number;
    char *string;
    //
    uint16_t allocated;
    uint16_t used;
    struct utjson **children;
} utjson;

char *utjson_version(void);

#define utjson_construct() calloc(1, sizeof(struct utjson))
utjson *utjson_destruct(utjson *object);

utjson *utjson_createNull(void);
utjson *utjson_createBool(bool value);
utjson *utjson_createNumber(double value);
utjson *utjson_createString(char *value);
utjson *utjson_createArray(void);
utjson *utjson_createObject(void);

bool utjson_asBool(utjson *object);
double utjson_asNumber(utjson *object);
char *utjson_asString(utjson *object);

utjson *utjson_get(utjson *object, char *name);
utjson *utjson_select(utjson *array, uint16_t index);

utjson *utjson_set(utjson *target, char *name, utjson *object);
utjson *utjson_setNull(utjson *target, char *name);
utjson *utjson_setBool(utjson *target, char *name, bool value);
utjson *utjson_setNumber(utjson *target, char *name, double value);
utjson *utjson_setString(utjson *target, char *name, char *value);
utjson *utjson_setArray(utjson *target, char *name);
utjson *utjson_setObject(utjson *target, char *name);

utjson *utjson_add(utjson *target, utjson *object);
utjson *utjson_addNull(utjson *target);
utjson *utjson_addBool(utjson *target, bool value);
utjson *utjson_addNumber(utjson *target, double value);
utjson *utjson_addString(utjson *target, char *value);
utjson *utjson_addArray(utjson *target);
utjson *utjson_addObject(utjson *target);

utjson *utjson_parse(char *source);
char *utjson_print(utjson *object, bool readable);

utjson *utjson_detach(utjson *object);
utjson *utjson_clone(const utjson *object);

#define utjson_arrayFor(array, item, index) \
    if (utjson_IS(ARRAY, array))            \
        for (uint16_t index = 0; index < array->used && (item = array[index], 1); index++)

#define utjson_objectForEach(object, item, tmp) \
    if (utjson_IS(OBJECT, object))              \
    HASH_ITER(hh, *(object->children), item, tmp)

#endif // UTJSON_H
