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

/**
 * @brief JSON value types.
 */
typedef enum
{
    utjson_NULL,   /**< Null value */
    utjson_BOOL,   /**< Boolean value */
    utjson_NUMBER, /**< Numeric value */
    utjson_STRING, /**< String value */
    utjson_ARRAY,  /**< Array of JSON values */
    utjson_OBJECT  /**< JSON object with key-value pairs */
} utjson_type;
#define utjson_IS(TYPE, object) (object && utjson_##TYPE == (object)->type)

#define utjson_ARRAY_INCREMENT 16

/**
 * @brief JSON structure for representing objects, arrays, and values.
 */
typedef struct utjson
{
    char *name;               /**< Key name (for objects) */
    UT_hash_handle hh;        /**< uthash handle for fast lookups in objects */
    struct utjson *parent;    /**< Pointer to parent object */
    utjson_type type;         /**< Type of the JSON value */
    double number;            /**< Numeric value (if type == utjson_NUMBER) */
    char *string;             /**< String value (if type == utjson_STRING) */
    uint16_t allocated;       /**< Number of allocated child elements (arrays/objects) */
    uint16_t used;            /**< Number of used child elements (arrays/objects) */
    struct utjson **children; /**< Array of child elements (for arrays and objects) */
} utjson;

/**
 * @brief Get the library version.
 * @return Pointer to a string representing the version.
 */
char *utjson_version(void);

/**
 * @brief Constructs an empty JSON object.
 * @return Pointer to a newly allocated utjson structure.
 */
#define utjson_construct() calloc(1, sizeof(struct utjson))
utjson *utjson_destruct(utjson *object);

/**
 * @brief Creates a new null JSON value.
 * @return Pointer to a new utjson object of type utjson_NULL.
 */
utjson *utjson_createNull(void);
/**
 * @brief Creates a new boolean JSON value.
 * @param value Boolean value (true/false).
 * @return Pointer to a new utjson object of type utjson_BOOL.
 */
utjson *utjson_createBool(bool value);
/**
 * @brief Creates a new numeric JSON value.
 * @param value The number to store.
 * @return Pointer to a new utjson object of type utjson_NUMBER.
 */
utjson *utjson_createNumber(double value);
/**
 * @brief Creates a new string JSON value.
 * @param value Pointer to a string (will be copied).
 * @return Pointer to a new utjson object of type utjson_STRING.
 */
utjson *utjson_createString(char *value);
/**
 * @brief Creates a new JSON array.
 * @return Pointer to a new utjson object of type utjson_ARRAY.
 */
utjson *utjson_createArray(void);
/**
 * @brief Creates a new JSON object.
 * @return Pointer to a new utjson object of type utjson_OBJECT.
 */
utjson *utjson_createObject(void);

/**
 * @brief Retrieves a boolean value from a JSON object.
 * @param object Pointer to a utjson object.
 * @return Boolean representation of the object.
 */
bool utjson_asBool(utjson *object);

/**
 * @brief Retrieves a numeric value from a JSON object.
 * @param object Pointer to a utjson object.
 * @return The numeric value stored in the object.
 */
double utjson_asNumber(utjson *object);

/**
 * @brief Retrieves a string value from a JSON object.
 * @param object Pointer to a utjson object.
 * @return Pointer to the string stored in the object.
 */
char *utjson_asString(utjson *object);

/**
 * @brief Retrieves a value from a JSON object by key.
 * @param object Pointer to the JSON object.
 * @param name The key to look for.
 * @return Pointer to the corresponding JSON value, or NULL if not found.
 */
utjson *utjson_get(utjson *object, char *name);

/**
 * @brief Retrieves a value from a JSON array by index.
 * @param array Pointer to the JSON array.
 * @param index The index of the element.
 * @return Pointer to the JSON element at the given index, or NULL if out of bounds.
 */
utjson *utjson_select(utjson *array, uint16_t index);

/**
 * @brief Sets a key-value pair in a JSON object.
 * @param target Pointer to the target JSON object.
 * @param name Key to assign.
 * @param object Value to assign.
 * @return Pointer to the modified JSON object.
 */
utjson *utjson_set(utjson *target, char *name, utjson *object);
/**
 * Helpers for object
 */
utjson *utjson_setNull(utjson *target, char *name);
utjson *utjson_setBool(utjson *target, char *name, bool value);
utjson *utjson_setNumber(utjson *target, char *name, double value);
utjson *utjson_setString(utjson *target, char *name, char *value);
utjson *utjson_setArray(utjson *target, char *name);
utjson *utjson_setObject(utjson *target, char *name);

/**
 * @brief Adds an element to a JSON array.
 *
 * This function appends a JSON value to the end of an array.
 * If the array is full, it is automatically resized.
 *
 * @param target Pointer to the JSON array.
 * @param object Pointer to the JSON object to add.
 * @return Pointer to the added JSON object, or NULL on failure.
 */
utjson *utjson_add(utjson *target, utjson *object);
/**
 * Helpers for array
 */
utjson *utjson_addNull(utjson *target);
utjson *utjson_addBool(utjson *target, bool value);
utjson *utjson_addNumber(utjson *target, double value);
utjson *utjson_addString(utjson *target, char *value);
utjson *utjson_addArray(utjson *target);
utjson *utjson_addObject(utjson *target);

utjson *utjson_parse(char *source);
char *utjson_print(utjson *object, bool readable);

/**
 * @brief Detaches an object from its parent.
 * @param object Pointer to the JSON object to detach.
 * @return The detached object.
 */
utjson *utjson_detach(utjson *object);

/**
 * @brief Clones a JSON object recursively.
 * @param object Pointer to the JSON object to clone.
 * @return A new independent copy of the object.
 */
utjson *utjson_clone(const utjson *object);

#define utjson_arrayFor(array, item, index) \
    if (utjson_IS(ARRAY, array))            \
        for (uint16_t index = 0; index < array->used && (item = array[index], 1); index++)

#define utjson_objectForEach(object, item, tmp) \
    if (utjson_IS(OBJECT, object))              \
    HASH_ITER(hh, *(object->children), item, tmp)

#endif // UTJSON_H
