/******************************************************************************
 *	Copyright 2008-2013 by Aerospike.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy 
 *	of this software and associated documentation files (the "Software"), to 
 *	deal in the Software without restriction, including without limitation the 
 *	rights to use, copy, modify, merge, publish, distribute, sublicense, and/or 
 *	sell copies of the Software, and to permit persons to whom the Software is 
 *	furnished to do so, subject to the following conditions:
 * 
 *	The above copyright notice and this permission notice shall be included in 
 *	all copies or substantial portions of the Software.
 * 
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *	IN THE SOFTWARE.
 *****************************************************************************/

#pragma once

#include <aerospike/as_util.h>
#include <aerospike/as_val.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************
 *	TYPES
 ******************************************************************************/

/**
 *	Container for NULL-terminates string values.
 *
 *	## Initialization
 *	
 *	An as_string should be initialized via one of the provided function.
 *	- as_string_init()
 *	- as_string_new()
 *
 *	To initialize a stack allocated as_string, use as_string_init():
 *
 *	~~~~~~~~~~{.c}
 *	as_string s;
 *	as_string_init(&s, "abc", false);
 *	~~~~~~~~~~
 *
 *	The 3rd argument indicates whether the string value should be `free()`d 
 *	when as_string is destroyed.
 *
 *	To create and initialize a heap allocated as_integer, use as_integer_new():
 *	
 *	~~~~~~~~~~{.c}
 *	as_string * s = as_string_new("abc", false);
 *	~~~~~~~~~~
 *
 *	## Destruction
 *
 *	When the as_string instance is no longer required, then you should
 *	release the resources associated with it via as_string_destroy():
 *
 *	~~~~~~~~~~{.c}
 *	as_string_destroy(s);
 *	~~~~~~~~~~
 *	
 *	## Usage
 *
 *	There are two functions for getting the boxed value contained by 
 *	as_string:
 *
 *	as_string_get() returns the contained value. If an error occurred, then
 *	NULL is returned. Possible errors is the as_integer instance is NULL.
 *
 *	~~~~~~~~~~{.c}
 *	char * sval = as_string_get(i);
 *	~~~~~~~~~~
 *
 *	as_string_getorelse() allows you to return a default value if an error 
 *	occurs:
 *
 *	~~~~~~~~~~{.c}
 *	char * sval = as_string_getorelse(i, "oops!");
 *	~~~~~~~~~~
 *
 *	## Conversions
 *
 *	as_string is derived from as_val, so it is generally safe to down cast:
 *
 *	~~~~~~~~~~{.c}
 *	as_val val = (as_val) s;
 *	~~~~~~~~~~
 *	
 *	However, upcasting is more error prone. When doing so, you should use 
 *	as_string_fromval(). If conversion fails, then the return value is NULL.
 *
 *	~~~~~~~~~~{.c}
 *	as_string * i = as_string_fromval(val);
 *	~~~~~~~~~~
 *
 *
 *	
 *	@extends as_val
 *	@ingroup aerospike_t
 */
typedef struct as_string_s {
	
	/**
	 *	@private
	 *	as_boolean is a subtype of as_val.
	 *	You can cast as_boolean to as_val.
	 */
	as_val _;

	/**
	 *	If true, then `as_string.value` can be freed.
	 */
	bool free;

	/**
	 *	The string value.
	 */
	char * value;

	/**
	 *	The length of the string.
	 */
	size_t len;

} as_string;

/******************************************************************************
 *	INSTANCE FUNCTIONS
 ******************************************************************************/

/**
 *	Initialize a stack allocated `as_string`.
 *
 *	If free is true, then the string value will be freed when the as_string is destroyed.
 *
 *	@param string	The stack allocated as_string to initialize
 *	@param value 	The NULL terminated string of character.
 *	@param free		If true, then the value will be freed when as_string is destroyed.
 *
 *	@return On success, the initialized string. Otherwise NULL.
 *
 *	@relatesalso as_string
 */
as_string * as_string_init(as_string * string, char * value, bool free);

/**
 *	Create and initialize a new heap allocated `as_string`.
 *
 *	If free is true, then the string value will be freed when the as_string is destroyed.
 *
 *	@param value 	The NULL terminated string of character.
 *	@param free		If true, then the value will be freed when as_string is destroyed.
 *
 *	@return On success, the new string. Otherwise NULL.
 *
 *	@relatesalso as_string
 */
as_string * as_string_new(char * value, bool free);

/**
 *	Create and initialize a new heap allocated `as_string`.
 *
 *	Value is cf_strdup()'d and will be freed when the as_string is destroyed.
 *
 *	@param value 	The NULL terminated string of character.
 *
 *	@return On success, the new string. Otherwise NULL.
 */
as_string * as_string_new_strdup(const char * value);

/**
 *	Destroy the as_string and associated resources.
 *
 *	@relatesalso as_string
 */
inline void as_string_destroy(as_string * string) 
{
	as_val_destroy((as_val *) string);
}

/******************************************************************************
 *	VALUE FUNCTIONS
 ******************************************************************************/

/**
 *	The length of the string
 *
 *	@param string The string to get the length of. 
 *
 *	@return the length of the string in bytes.
 *
 *	@relatesalso as_string
 */
size_t as_string_len(as_string * string);

/**
 *	Get the string value. If string is NULL, then return the fallback value.
 *
 *	@relatesalso as_string
 */
inline char * as_string_getorelse(const as_string * string, char * fallback) 
{
	return string ? string->value : fallback;
}

/**
 *	Get the string value.
 *
 *	@relatesalso as_string
 */
inline char * as_string_get(const as_string * string) 
{
	return as_string_getorelse(string, NULL);
}

/**
 *	Get the string value.
 *	@deprecated Use as_string_get() instead
 *
 *	@relatesalso as_string
 */
inline char * as_string_tostring(const as_string * string) 
{
	return as_string_getorelse(string, NULL);
}

/******************************************************************************
 *	CONVERSION FUNCTIONS
 ******************************************************************************/

/**
 *	Convert to an as_val.
 *
 *	@relatesalso as_string
 */
inline as_val * as_string_toval(const as_string * s) 
{
	return (as_val *) s;
}

/**
 *	Convert from an as_val.
 *
 *	@relatesalso as_string
 */
inline as_string * as_string_fromval(const as_val * v) 
{
	return as_util_fromval(v, AS_STRING, as_string);
}

/******************************************************************************
 *	as_val FUNCTIONS
 ******************************************************************************/

/**
 *	@private
 *	Internal helper function for destroying an as_val.
 */
void as_string_val_destroy(as_val * v);

/**
 *	@private
 *	Internal helper function for getting the hashcode of an as_val.
 */
uint32_t as_string_val_hashcode(const as_val * v);

/**
 *	@private
 *	Internal helper function for getting the string representation of an as_val.
 */
char * as_string_val_tostring(const as_val * v);
