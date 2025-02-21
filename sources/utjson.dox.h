/**
 * @file utjson.h
 * @brief UTJSON - A Lightweight JSON Library for C
 * @author Yurii (Mouse) Prudius
 * @version 1.0
 * @date 2025
 */
#include "utjson.h"
/**
 * @mainpage UTJSON Documentation
 *
 * @section intro Introduction
 * UTJSON is a minimalistic and efficient JSON parsing and manipulation library written in C.
 * It provides utilities to create, modify, and serialize JSON data.
 *
 * @section features Features
 * - Parse JSON from strings
 * - Create and modify JSON objects and arrays
 * - Serialize JSON structures into formatted or compact strings
 * - Efficient storage with uthash
 * - Memory management functions
 *
 * @section usage Usage Example
 * @code
 * utjson *obj = utjson_createObject();
 * utjson_set(obj, "name", utjson_createString("Alice"));
 * utjson_set(obj, "age", utjson_createNumber(30));
 * char *json_str = utjson_print(obj, true);
 * printf("%s\n", json_str);
 * free(json_str);
 * utjson_free(obj);
 * @endcode
 *
 * @section toc Table of Contents
 * - @ref types
 * - @ref functions
 * - @ref examples
 */

/**
 * @defgroup types JSON Data Types
 * @brief Enumeration of JSON data types.
 */
/**@{*/
/// JSON data type enumeration.
typedef enum
{
    utjson_NULL,   ///< Null value
    utjson_BOOL,   ///< Boolean value (true/false)
    utjson_NUMBER, ///< Numeric value
    utjson_STRING, ///< String value
    utjson_ARRAY,  ///< JSON array
    utjson_OBJECT  ///< JSON object
} utjson_type;
/**@}*/

/**
 * @defgroup functions JSON Functions
 * @brief Functions for creating, manipulating, and serializing JSON objects.
 */
/**@{*/

/**
 * @brief Parses a JSON string into a utjson object.
 * @param source JSON string to parse.
 * @return Pointer to a newly allocated JSON object, or NULL on failure.
 *
 * @code
 * utjson *obj = utjson_parse("{\"name\": \"Alice\"}");
 * if (obj) {
 *     printf("Parsed successfully!\n");
 *     utjson_free(obj);
 * }
 * @endcode
 */
utjson *utjson_parse(char *source);

/**
 * @brief Serializes a utjson object into a JSON string.
 * @param object Pointer to the JSON object.
 * @param readable If true, formats output with indentation.
 * @return Pointer to a dynamically allocated JSON string (must be freed).
 *
 * @code
 * utjson *obj = utjson_createObject();
 * utjson_set(obj, "greeting", utjson_createString("Hello, World!"));
 * char *json_str = utjson_print(obj, true);
 * printf("%s\n", json_str);
 * free(json_str);
 * utjson_free(obj);
 * @endcode
 */
char *utjson_print(utjson *object, bool readable);

/**
 * @brief Creates an empty JSON object.
 * @return Pointer to a newly allocated JSON object.
 */
utjson *utjson_createObject(void);

/**
 * @brief Creates a new JSON array.
 * @return Pointer to a newly allocated JSON array.
 */
utjson *utjson_createArray(void);

/**
 * @brief Sets a key-value pair in a JSON object.
 * @param target Pointer to the JSON object.
 * @param name Key for the JSON value.
 * @param object Pointer to the JSON value.
 * @return Pointer to the updated JSON object.
 *
 * @code
 * utjson *obj = utjson_createObject();
 * utjson_set(obj, "temperature", utjson_createNumber(25.5));
 * @endcode
 */
utjson *utjson_set(utjson *target, char *name, utjson *object);

/**
 * @brief Retrieves a value from a JSON object by key.
 * @param object Pointer to the JSON object.
 * @param name Key name.
 * @return Pointer to the corresponding JSON value, or NULL if not found.
 *
 * @code
 * utjson *value = utjson_get(obj, "temperature");
 * if (value) {
 *     printf("Temperature: %f\n", utjson_asNumber(value));
 * }
 * @endcode
 */
utjson *utjson_get(utjson *object, char *name);

/**
 * @brief Appends a JSON value to an array.
 * @param target Pointer to the JSON array.
 * @param object Pointer to the JSON value.
 * @return Pointer to the updated JSON array.
 *
 * @code
 * utjson *arr = utjson_createArray();
 * utjson_add(arr, utjson_createString("Apple"));
 * utjson_add(arr, utjson_createString("Banana"));
 * @endcode
 */
utjson *utjson_add(utjson *target, utjson *object);

/**
 * @brief Detaches an object from its parent.
 * @param object Pointer to the JSON object.
 * @return The detached object.
 */
utjson *utjson_detach(utjson *object);

/**
 * @brief Clones a JSON object recursively.
 * @param object Pointer to the JSON object to clone.
 * @return A new independent copy of the object.
 */
utjson *utjson_clone(const utjson *object);

/**@}*/
