#include "utjson.h"
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
static utjson *utjson_destruct(utjson *object)
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
        FREE_AND_NULL(object->children);
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
    }
    FREE_AND_NULL(object->string);
    FREE_AND_NULL(object->name);
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
    }
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
        bool created = false;
        if (!object)
        {
            object = utjson_createNull();
            created = true;
        }
        if (object)
        {
            if (object->allocated <= object->used)
            {
                uint16_t resized = object->allocated + utjson_ARRAY_INCREMENT;
                object->children = realloc(object->children, resized * sizeof(struct utjson *));
                object->allocated = object->children ? resized : 0;
            }
            if (object->used < object->allocated)
            {
                object->children[object->used] = object;
                object->used++;
            }
            else if (created)
            {
                FREE_AND_NULL(object);
            }
            else
            {
                return NULL;
            }
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
