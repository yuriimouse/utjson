#include "utjson.h"
#include <ctype.h>
#include <errno.h>
#include <sys/time.h>
#include <syslog.h>

/**
 * Current version
 *
 * @return char*
 */
char *utjson_version(void)
{
    return VERSION;
}

/**
 * Destroys the object
 *
 * @param object
 * @return utjson*
 */
utjson *utjson_destruct(utjson *object)
{
    switch (object->type)
    {
    case utjson_ARRAY:
        // destuct all elements of array
        for (size_t idx = 0; idx < object->used; idx++)
        {
            if (object->children[idx])
            {
                object->children[idx] = utjson_destruct(object->children[idx]);
            }
        }
        break;
    case utjson_OBJECT:
        // destuct all children of object
        {
            utjson *current = NULL;
            utjson *tmp = NULL;
            HASH_ITER(hh, *(object->children), current, tmp)
            {
                HASH_DEL(*(object->children), current);
                {
                    if (current)
                    {
                        utjson_destruct(current);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    FREE_AND_NULL(object->string);
    FREE_AND_NULL(object->name);
    FREE_AND_NULL(object->pointer_type);
    FREE_AND_NULL(object->children);
    FREE_AND_NULL(object);

    return NULL;
}

/**
 * Creates Null
 *
 * @return utjson*
 */
utjson *utjson_createNull(void)
{
    return utjson_construct();
}

/**
 * Creates boolean
 *
 * @param value
 * @return utjson*
 */
utjson *utjson_createBool(bool value)
{
    utjson *object = utjson_construct();
    if (object)
    {
        object->type = utjson_BOOL;
        object->number = (double)value;
    }
    return object;
}

/**
 * Creates number
 *
 * @param value
 * @return utjson*
 */
utjson *utjson_createNumber(double value)
{
    utjson *object = utjson_construct();
    if (object)
    {
        object->type = utjson_NUMBER;
        object->number = value;
    }
    return object;
}

/**
 * Creates string
 *
 * @param value
 * @return utjson*
 */
utjson *utjson_createString(char *value)
{
    utjson *object = utjson_construct();
    if (object)
    {
        object->type = utjson_STRING;
        object->string = value ? strdup(value) : NULL;
    }
    return object;
}

/**
 * Creates array
 *
 * @param value
 * @return utjson*
 */
utjson *utjson_createArray(void)
{
    utjson *object = utjson_construct();
    if (object)
    {
        object->type = utjson_ARRAY;
    }
    return object;
}

/**
 * Creates object
 *
 * @param value
 * @return utjson*
 */
utjson *utjson_createObject(void)
{
    utjson *object = utjson_construct();
    if (object)
    {
        object->type = utjson_OBJECT;
        object->children = calloc(1, sizeof(utjson *));
    }
    return object;
}

/**
 * Creates a JSON object representing a pointer.
 *
 * @param ptr The pointer to store.
 * @param type A string describing the pointer type.
 * @return A utjson object of type utjson_POINTER.
 */
utjson *utjson_createPointer(void *ptr, const char *type)
{
    if (!type)
        return NULL;
    utjson *object = utjson_construct();
    object->type = utjson_POINTER;
    object->pointer = ptr;
    object->pointer_type = strdup(type);
    return object;
}

/**
 * Converts value to boolean
 *
 * @param object
 * @return true | false
 */
bool utjson_asBool(utjson *object)
{
    if (object)
    {
        switch (object->type)
        {
        case utjson_POINTER:
            // fall through
        case utjson_NULL:
            return false;
        case utjson_BOOL:
            // fall through
        case utjson_NUMBER:
            return (bool)object->number;
        case utjson_STRING:
            return object->string && object->string[0] ? true : false;
        case utjson_ARRAY:
            return (bool)object->used;
        case utjson_OBJECT:
            return object->children ? (bool)HASH_COUNT(object->children[0]) : false;
        }
    }
    errno = EINVAL;
    return false;
}

/**
 * Converts value to number
 *
 * @param object
 * @return double
 */
double utjson_asNumber(utjson *object)
{
    if (object)
    {
        switch (object->type)
        {
        case utjson_POINTER:
            // fall through
        case utjson_NULL:
            return 0;
        case utjson_BOOL:
            // fall through
        case utjson_NUMBER:
            return object->number;
        case utjson_STRING:
            return object->string && object->string[0] ? atof(object->string) : 0;
        case utjson_ARRAY:
            return (double)object->used;
        case utjson_OBJECT:
            return object->children ? (double)HASH_COUNT(object->children[0]) : 0;
        }
    }
    errno = EINVAL;
    return 0;
}

/**
 * Converts value to string
 *
 * @param object
 * @return char*
 */
char *utjson_asString(utjson *object)
{
    if (object)
    {
        switch (object->type)
        {
        case utjson_POINTER:
            // fall through
        case utjson_NULL:
            return NULL;
        case utjson_BOOL:
            // fall through
        case utjson_NUMBER:
            FREE_AND_NULL(object->string);
            asprintf(&(object->string), "%f", object->number);
            // fall through
        case utjson_STRING:
            return object->string;
        case utjson_ARRAY:
            // join ???
            // fall through
        case utjson_OBJECT:
            // join ???
            return NULL;
        }
    }
    errno = EINVAL;
    return NULL;
}

/**
 * Retrieves a pointer value and its type from a JSON object.
 *
 * @param object The JSON object.
 * @param pointer_type A pointer to a string where the type will be stored.
 * @return The stored pointer, or NULL if invalid.
 */
void *utjson_asPointer(utjson *object, char **pointer_type)
{
    if (pointer_type)
    {
        *pointer_type = pointer_type && object ? object->pointer_type : NULL;
    }
    return utjson_IS(POINTER, object) ? object->pointer : NULL;
}

/**
 * Sets the named child object
 *
 * @param target
 * @param name
 * @param object
 * @return utjson *
 */
utjson *utjson_set(utjson *target, char *name, utjson *object)
{
    if (utjson_IS(OBJECT, target) && name)
    {
        // bool created = false;
        if (!object)
        {
            object = utjson_createNull();
            // created = true;
        }
        if (object)
        {
            object->name = strdup(name);
            utjson *replaced = NULL;
            HASH_REPLACE_STR(*(target->children), name, object, replaced);
            if (replaced)
            {
                utjson_destruct(replaced);
            }
            return object;
        }
        errno = ENOMEM;
        return NULL;
    }
    errno = EINVAL;
    return NULL;
}

/**
 * Sets the named Null child
 *
 * @param target
 * @param name
 * @return utjson *
 */
utjson *utjson_setNull(utjson *target, char *name)
{
    return utjson_set(target, name, NULL);
}

/**
 * Sets the named boolean child
 *
 * @param target
 * @param name
 * @param value
 * @return utjson *
 */
utjson *utjson_setBool(utjson *target, char *name, bool value)
{
    utjson *object = utjson_createBool(value);
    if (!utjson_set(target, name, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Sets the named number child
 *
 * @param target
 * @param name
 * @param value
 * @return utjson *
 */
utjson *utjson_setNumber(utjson *target, char *name, double value)
{
    utjson *object = utjson_createNumber(value);
    if (!utjson_set(target, name, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Sets the named string child
 *
 * @param target
 * @param name
 * @param value
 * @return utjson *
 */
utjson *utjson_setString(utjson *target, char *name, char *value)
{
    utjson *object = utjson_createString(value);
    if (!utjson_set(target, name, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Sets the named array child
 *
 * @param target
 * @param name
 * @return utjson*
 */
utjson *utjson_setArray(utjson *target, char *name)
{
    utjson *object = utjson_createArray();
    if (!utjson_set(target, name, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Sets the named object child
 *
 * @param target
 * @param name
 * @return utjson*
 */
utjson *utjson_setObject(utjson *target, char *name)
{
    utjson *object = utjson_createObject();
    if (!utjson_set(target, name, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Adds the object to array
 *
 * @param target
 * @param object
 * @return utjson*
 */
utjson *utjson_add(utjson *target, utjson *object)
{
    if (utjson_IS(ARRAY, target))
    {
        if (!object)
        {
            object = utjson_createNull();
        }
        if (object)
        {
            if (target->allocated <= target->used)
            {
                target->allocated += utjson_ARRAY_INCREMENT;
                target->children = realloc(target->children, target->allocated * sizeof(struct utjson *));
            }
            target->children[target->used++] = object;
            object->parent = target;
            return object;
        }
        else
        {
            errno = ENOMEM;
            return NULL;
        }
    }
    errno = EINVAL;
    return NULL;
}

/**
 * Adds the Null to array
 *
 * @param target
 * @return utjson*
 */
utjson *utjson_addNull(utjson *target)
{
    return utjson_add(target, NULL);
}

/**
 * Adds the boolean to array
 *
 * @param target
 * @param value
 * @return utjson*
 */
utjson *utjson_addBool(utjson *target, bool value)
{
    utjson *object = utjson_createBool(value);
    if (!utjson_add(target, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Adds the number to array
 *
 * @param target
 * @param value
 * @return utjson*
 */
utjson *utjson_addNumber(utjson *target, double value)
{
    utjson *object = utjson_createNumber(value);
    if (!utjson_add(target, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Adds the string to array
 *
 * @param target
 * @param value
 * @return utjson*
 */
utjson *utjson_addString(utjson *target, char *value)
{
    utjson *object = utjson_createString(value);
    if (!utjson_add(target, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Adds the array to array
 *
 * @param target
 * @return utjson*
 */
utjson *utjson_addArray(utjson *target)
{
    utjson *object = utjson_createArray();
    if (!utjson_add(target, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Adds the object to array
 *
 * @param target
 * @return utjson*
 */
utjson *utjson_addObject(utjson *target)
{
    utjson *object = utjson_createObject();
    if (!utjson_add(target, object))
    {
        object = utjson_destruct(object);
    }
    return object;
}

/**
 * Gets an element from an object by name
 *
 * @param object
 * @param name
 * @return utjson*
 */
utjson *utjson_get(utjson *object, char *name)
{
    if (utjson_IS(OBJECT, object) && name)
    {
        utjson *item = NULL;
        HASH_FIND_STR(*(object->children), name, item);
        return item;
    }
    errno = EINVAL;
    return NULL;
}

/**
 * Gets an element from an array by number
 *
 * @param array
 * @param index
 * @return utjson*
 */
utjson *utjson_select(utjson *array, uint16_t index)
{
    if (utjson_IS(ARRAY, array))
    {
        if (index < array->used)
        {
            return array->children[index];
        }
        errno = ERANGE;
        return NULL;
    }
    errno = EINVAL;
    return NULL;
}

static char *skip_whitespace(char *c)
{
    while (*c && isspace((unsigned char)*c))
        c++;
    return c;
}

static utjson *parse_value(char **source);

static utjson *parse_null(char **source)
{
    if (strncmp(*source, "null", 4) == 0)
    {
        *source += 4;
        return utjson_createNull();
    }
    return NULL;
}

static utjson *parse_bool(char **source)
{
    if (strncmp(*source, "true", 4) == 0)
    {
        *source += 4;
        return utjson_createBool(true);
    }
    else if (strncmp(*source, "false", 5) == 0)
    {
        *source += 5;
        return utjson_createBool(false);
    }
    return NULL;
}

static utjson *parse_number(char **source)
{
    char *end;
    double value = strtod(*source, &end);
    if (end == *source)
        return NULL; // No valid number
    *source = end;
    return utjson_createNumber(value);
}

static utjson *parse_string(char **source)
{
    if (**source != '"')
        return NULL;
    (*source)++;
    char *start = *source;
    while (**source && **source != '"')
        (*source)++;
    if (**source != '"')
        return NULL; // Unterminated string
    size_t len = *source - start;
    char *value = strndup(start, len);
    (*source)++;

    // Check if the string follows the pointer format "<:type:>pointer"
    if (strncmp(value, "<:", 2) == 0)
    {
        char *end = strstr(value, ":>pointer");
        if (end)
        {
            size_t type_len = end - (value + 2);
            char *type = strndup(value + 2, type_len);
            utjson *pointer_obj = utjson_createPointer(NULL, type);
            free(type);
            free(value);
            return pointer_obj;
        }
    }

    utjson *str_obj = utjson_createString(value);
    free(value);
    return str_obj;
}

static utjson *parse_array(char **source)
{
    if (**source != '[')
        return NULL;
    (*source)++;
    utjson *array = utjson_createArray();

    while (**source && **source != ']')
    {
        *source = skip_whitespace(*source);
        utjson *element = parse_value(source);
        if (!element)
        {
            utjson_destruct(array);
            return NULL;
        }

        utjson_add(array, element);

        *source = skip_whitespace(*source);
        if (**source == ',')
        {
            (*source)++;
        }
    }
    if (**source == ']')
        (*source)++;
    return array;
}

static utjson *parse_object(char **source)
{
    if (**source != '{')
        return NULL;
    (*source)++;
    utjson *object = utjson_createObject();

    while (**source && **source != '}')
    {
        *source = skip_whitespace(*source);
        utjson *key = parse_string(source);
        if (!key)
        {
            utjson_destruct(object);
            return NULL;
        }

        *source = skip_whitespace(*source);
        if (**source != ':')
        {
            utjson_destruct(object);
            utjson_destruct(key);
            return NULL;
        }
        (*source)++;

        *source = skip_whitespace(*source);
        utjson *value = parse_value(source);
        if (!value)
        {
            utjson_destruct(object);
            utjson_destruct(key);
            return NULL;
        }

        utjson_set(object, key->string, value);
        utjson_destruct(key);

        *source = skip_whitespace(*source);
        if (**source == ',')
        {
            (*source)++;
        }
    }
    if (**source == '}')
        (*source)++;
    return object;
}

static utjson *parse_value(char **source)
{
    *source = skip_whitespace(*source);
    if (**source == 'n')
        return parse_null(source);
    if (**source == 't' || **source == 'f')
        return parse_bool(source);
    if (**source == '"')
        return parse_string(source);
    if ((**source == '-' || isdigit((unsigned char)**source)))
        return parse_number(source);
    if (**source == '[')
        return parse_array(source);
    if (**source == '{')
        return parse_object(source);
    return NULL;
}

/**
 * Parse a JSON string into utjson*
 *
 * @param source
 * @return utjson*
 */
utjson *utjson_parse(char *source)
{
    if (!source)
        return NULL;
    char *ptr = source;
    return parse_value(&ptr);
}

static void append_str(char **dest, const char *format, ...)
{
    va_list args;
    char *temp;
    va_start(args, format);
    vasprintf(&temp, format, args);
    va_end(args);

    size_t len = (*dest ? strlen(*dest) : 0) + strlen(temp) + 1;
    *dest = realloc(*dest, len);
    strcat(*dest, temp);
    free(temp);
}

/**
 * Prints JSON into new string
 *
 * @param object
 * @param readable
 * @return char*
 */
char *utjson_print(utjson *object, bool readable)
{
    if (!object)
        return strdup("null");
    char *output = strdup("");

    switch (object->type)
    {
    case utjson_NULL:
        append_str(&output, "null");
        break;
    case utjson_BOOL:
        append_str(&output, object->number ? "true" : "false");
        break;
    case utjson_NUMBER:
        append_str(&output, "%g", object->number);
        break;
    case utjson_STRING:
        append_str(&output, "\"%s\"", object->string);
        break;
    case utjson_ARRAY:
        append_str(&output, "[");
        for (uint16_t i = 0; i < object->used; i++)
        {
            if (i > 0)
                append_str(&output, ",%s", readable ? " " : "");
            char *item_str = utjson_print(object->children[i], readable);
            append_str(&output, "%s", item_str);
            free(item_str);
        }
        append_str(&output, "]");
        break;
    case utjson_OBJECT:
        append_str(&output, "{");
        utjson *entry, *tmp;
        HASH_ITER(hh, *(object->children), entry, tmp)
        {
            append_str(&output, "\"%s\":%s", entry->name, readable ? " " : "");
            char *value_str = utjson_print(entry, readable);
            append_str(&output, "%s,", value_str);
            free(value_str);
        }
        if (output[strlen(output) - 1] == ',')
        {
            output[strlen(output) - 1] = '}';
        }
        else
        {
            append_str(&output, "}");
        }
        break;
    case utjson_POINTER:
        asprintf(&output, "\"<:%s:>pointer\"", object->pointer_type);
        break;
    }
    return output;
}

/**
 * Detachs an object from parent
 *
 * @param object
 * @return utjson*
 */
utjson *utjson_detach(utjson *object)
{
    if (!object || !object->parent)
        return object; // Already detached

    utjson *parent = object->parent;
    object->parent = NULL;

    if (utjson_IS(ARRAY, parent))
    {
        // Remove from array
        for (uint16_t i = 0; i < parent->used; i++)
        {
            if (parent->children[i] == object)
            {
                memmove(&parent->children[i], &parent->children[i + 1], (parent->used - i - 1) * sizeof(utjson *));
                parent->used--;
                break;
            }
        }
    }
    else if (utjson_IS(OBJECT, parent))
    {
        // Remove from hash table
        HASH_DEL(*(parent->children), object);
    }
    return object;
}

/**
 * Clones an object
 *
 * @param object
 * @return utjson*
 */
utjson *utjson_clone(const utjson *object)
{
    if (!object)
        return NULL;

    utjson *copy = utjson_construct();
    copy->type = object->type;

    switch (object->type)
    {
    case utjson_BOOL:
        // fall through
    case utjson_NUMBER:
        copy->number = object->number;
        break;
    case utjson_STRING:
        copy->string = strdup(object->string);
        break;
    case utjson_ARRAY:
        copy->allocated = object->allocated;
        copy->used = object->used;
        copy->children = calloc(copy->allocated, sizeof(utjson *));
        for (uint16_t i = 0; i < object->used; i++)
        {
            copy->children[i] = utjson_clone(object->children[i]);
        }
        break;
    case utjson_OBJECT:
    {
        utjson *entry, *tmp, *new_entry;
        HASH_ITER(hh, *(object->children), entry, tmp)
        {
            new_entry = utjson_clone(entry);
            HASH_ADD_KEYPTR(hh, *(copy->children), new_entry->name, strlen(new_entry->name), new_entry);
        }
    }
    break;
    default:
        break;
    }
    return copy;
}
