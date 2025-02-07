#include "utjson.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Test case for utjson_createNull
void test_utjson_createNull(void)
{
    utjson *obj = utjson_createNull();
    assert(obj != NULL);
    assert(obj->type == utjson_NULL); // Assuming your utjson structure has a type field
    utjson_destruct(obj);
}

// Test case for utjson_createBool
void test_utjson_createBool(void)
{
    utjson *obj = utjson_createBool(true);
    assert(obj != NULL);
    assert(obj->type == utjson_BOOL);
    assert(utjson_asBool(obj) == true);
    utjson_destruct(obj);

    obj = utjson_createBool(false);
    assert(obj != NULL);
    assert(utjson_asBool(obj) == false);
    utjson_destruct(obj);
}

// Test case for utjson_createNumber
void test_utjson_createNumber(void)
{
    utjson *obj = utjson_createNumber(42.0);
    assert(obj != NULL);
    assert(obj->type == utjson_NUMBER);
    assert(utjson_asNumber(obj) == 42.0);
    utjson_destruct(obj);
}

// Test case for utjson_createString
void test_utjson_createString(void)
{
    char *str = "Hello, World!";
    utjson *obj = utjson_createString(str);
    assert(obj != NULL);
    assert(obj->type == utjson_STRING);
    assert(strcmp(utjson_asString(obj), str) == 0);
    utjson_destruct(obj);
}

// Test case for utjson_createArray
void test_utjson_createArray(void)
{
    utjson *obj = utjson_createArray();
    assert(obj != NULL);
    assert(obj->type == utjson_ARRAY);
    utjson_destruct(obj);
}

// Test case for utjson_createObject
void test_utjson_createObject(void)
{
    utjson *obj = utjson_createObject();
    assert(obj != NULL);
    assert(obj->type == utjson_OBJECT);
    utjson_destruct(obj);
}

// Test case for utjson_get
void test_utjson_get(void)
{
    utjson *obj = utjson_createObject();
    utjson *value = utjson_createString("Test value");

    // Set value in object
    utjson_set(obj, "key", value);

    // Test getting the value
    utjson *retrieved = utjson_get(obj, "key");
    assert(retrieved != NULL);
    assert(utjson_asString(retrieved) != NULL);
    assert(strcmp(utjson_asString(retrieved), "Test value") == 0);

    utjson_destruct(obj);
}

// Test case for utjson_select
void test_utjson_select(void)
{
    utjson *array = utjson_createArray();
    utjson *obj1 = utjson_createNumber(1.0);
    utjson *obj2 = utjson_createNumber(2.0);

    utjson_add(array, obj1);
    utjson_add(array, obj2);

    utjson *selected = utjson_select(array, 1);
    assert(selected != NULL);
    assert(utjson_asNumber(selected) == 2.0);
    utjson_destruct(array);
}

// Test case for utjson_set
void test_utjson_set(void)
{
    utjson *obj = utjson_createObject();
    utjson *value = utjson_createString("New value");

    // Set value in object
    utjson_set(obj, "key", value);

    // Test that the value was set correctly
    utjson *retrieved = utjson_get(obj, "key");
    assert(retrieved != NULL);
    assert(strcmp(utjson_asString(retrieved), "New value") == 0);
    utjson_destruct(obj);
}

// Test case for utjson_add
void test_utjson_add(void)
{
    utjson *array = utjson_createArray();
    utjson *obj = utjson_createNumber(3.14);

    utjson_add(array, obj);

    // Check if the object was added
    utjson *retrieved = utjson_select(array, 0);
    assert(retrieved != NULL);
    assert(utjson_asNumber(retrieved) == 3.14);
    utjson_destruct(array);
}

// Test case for utjson_parse and utjson_print
void test_utjson_parse_print(void)
{
    char *json_string = "{\"key\": \"value\", \"num\": 42}";

    utjson *parsed = utjson_parse(json_string);
    assert(parsed != NULL);
    assert(utjson_get(parsed, "key") != NULL);
    assert(strcmp(utjson_asString(utjson_get(parsed, "key")), "value") == 0);

    char *printed = utjson_print(parsed, true);
    assert(printed != NULL);
    // You could also check if the printed JSON matches expected format, but this will depend on your printing logic
    printf("Printed JSON: %s\n", printed);
    free(printed); // Assuming you allocate memory for printed output
    utjson_destruct(parsed);
}

int main(void)
{
    // Run the tests
    test_utjson_createNull();
    test_utjson_createBool();
    test_utjson_createNumber();
    test_utjson_createString();
    test_utjson_createArray();
    test_utjson_createObject();
    test_utjson_get();
    test_utjson_select();
    test_utjson_set();
    test_utjson_add();
    test_utjson_parse_print();

    printf("All tests passed!\n");
    return 0;
}
