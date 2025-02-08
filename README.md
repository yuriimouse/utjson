# UTJSON Library Reference

## Overview
UTJSON is a lightweight JSON handling library in C, designed for parsing, manipulating, and serializing JSON data efficiently.

## Data Types
- `utjson_NULL` – Represents a JSON null value.
- `utjson_BOOL` – Boolean values (`true` or `false`).
- `utjson_NUMBER` – Numeric values (integers or floating-point).
- `utjson_STRING` – JSON strings.
- `utjson_ARRAY` – JSON arrays.
- `utjson_OBJECT` – JSON objects (key-value pairs).

## Functions

### Creation Functions
- **`utjson_createNull(void)`** – Creates a JSON `null` value.
- **`utjson_createBool(bool value)`** – Creates a boolean JSON value.
- **`utjson_createNumber(double value)`** – Creates a numeric JSON value.
- **`utjson_createString(char *value)`** – Creates a JSON string.
- **`utjson_createArray(void)`** – Creates an empty JSON array.
- **`utjson_createObject(void)`** – Creates an empty JSON object.

### Data Accessors
- **`bool utjson_asBool(utjson *object)`** – Converts a JSON value to a boolean.
- **`double utjson_asNumber(utjson *object)`** – Converts a JSON value to a number.
- **`char *utjson_asString(utjson *object)`** – Converts a JSON value to a string.

### Object and Array Manipulation
- **`utjson *utjson_get(utjson *object, char *name)`** – Retrieves a value from a JSON object.
- **`utjson *utjson_select(utjson *array, uint16_t index)`** – Retrieves an element from a JSON array.
- **`utjson *utjson_set(utjson *target, char *name, utjson *object)`** – Sets a key-value pair in a JSON object.
- **`utjson *utjson_add(utjson *target, utjson *object)`** – Appends a JSON object to an array.

### Parsing and Serialization
- **`utjson *utjson_parse(char *source)`** – Parses a JSON-formatted string into a `utjson` object.
- **`char *utjson_print(utjson *object, bool readable)`** – Serializes a `utjson` object into a JSON string. If `readable` is `true`, the output is formatted with indentation.

### Memory Management
- **`utjson *utjson_detach(utjson *object)`** – Detaches an object from its parent.
- **`utjson *utjson_clone(const utjson *object)`** – Creates a deep copy of a JSON object.

### Utility
- **`char *utjson_version(void)`** – Returns the UTJSON library version.

## Notes
- JSON arrays automatically expand when new elements are added.
- Objects are stored using hash tables for fast key-value lookups.

This document provides a concise reference to the UTJSON API. A detailed guide with examples will follow in the full documentation.

