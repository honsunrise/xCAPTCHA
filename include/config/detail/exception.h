//
// Created by zhsyourai on 8/19/17.
//

#ifndef XCAPTCHA_EXCEPTION_H
#define XCAPTCHA_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace captcha_config {
    namespace detail {

        class exception : public std::exception {
        public:
            const char *what() const noexcept override {
                return m.what();
            }

            const int id;

        protected:
            exception(int id_, const char *what_arg) : id(id_), m(what_arg) {}

            static std::string name(const std::string &ename, int id) {
                return "[xcaptcha.config." + ename + "." + std::to_string(id) + "] ";
            }

        private:
            std::runtime_error m;
        };

/*!
@brief exception indicating a parse error

This excpetion is thrown by the library when a parse error occurs. Parse errors
can occur during the deserialization of JSON text as well as when using JSON
Patch.

Member @a byte holds the byte index of the last read character in the input
file.

@note For an input with n bytes, 1 is the index of the first character and n+1
      is the index of the terminating null byte or the end of file. This also
      holds true when reading a byte vector (CBOR or MessagePack).

Exceptions have ids 1xx.

name / id                      | example message | description
------------------------------ | --------------- | -------------------------
json.exception.parse_error.101 | parse error at 2: unexpected end of input; expected string literal | This error indicates a syntax error while deserializing a JSON text. The error message describes that an unexpected token (character) was encountered, and the member @a byte indicates the error position.
json.exception.parse_error.102 | parse error at 14: missing or wrong low surrogate | JSON uses the `\uxxxx` format to describe Unicode characters. Code points above above 0xFFFF are split into two `\uxxxx` entries ("surrogate pairs"). This error indicates that the surrogate pair is incomplete or contains an invalid code point.
json.exception.parse_error.103 | parse error: code points above 0x10FFFF are invalid | Unicode supports code points up to 0x10FFFF. Code points above 0x10FFFF are invalid.
json.exception.parse_error.104 | parse error: JSON patch must be an array of objects | [RFC 6902](https://tools.ietf.org/html/rfc6902) requires a JSON Patch document to be a JSON document that represents an array of objects.
json.exception.parse_error.105 | parse error: operation must have string member 'op' | An operation of a JSON Patch document must contain exactly one "op" member, whose value indicates the operation to perform. Its value must be one of "add", "remove", "replace", "move", "copy", or "test"; other values are errors.
json.exception.parse_error.106 | parse error: array index '01' must not begin with '0' | An array index in a JSON Pointer ([RFC 6901](https://tools.ietf.org/html/rfc6901)) may be `0` or any number wihtout a leading `0`.
json.exception.parse_error.107 | parse error: JSON pointer must be empty or begin with '/' - was: 'foo' | A JSON Pointer must be a Unicode string containing a sequence of zero or more reference tokens, each prefixed by a `/` character.
json.exception.parse_error.108 | parse error: escape character '~' must be followed with '0' or '1' | In a JSON Pointer, only `~0` and `~1` are valid escape sequences.
json.exception.parse_error.109 | parse error: array index 'one' is not a number | A JSON Pointer array index must be a number.
json.exception.parse_error.110 | parse error at 1: cannot read 2 bytes from vector | When parsing CBOR or MessagePack, the byte vector ends before the complete value has been read.
json.exception.parse_error.112 | parse error at 1: error reading CBOR; last byte: 0xf8 | Not all types of CBOR or MessagePack are supported. This exception occurs if an unsupported byte was read.
json.exception.parse_error.113 | parse error at 2: expected a CBOR string; last byte: 0x98 | While parsing a map key, a value that is not a string has been read.

@since version 3.0.0
*/
        class parse_error : public exception {
        public:
            /*!
            @brief create a parse error exception
            @param[in] id        the id of the exception
            @param[in] byte_     the byte index where the error occurred (or 0 if the
                                 position cannot be determined)
            @param[in] what_arg  the explanatory string
            @return parse_error object
            */
            static parse_error create(int id, std::size_t byte_, const std::string &what_arg) {
                std::string w = exception::name("parse_error", id) + "parse error" +
                                (byte_ != 0 ? (" at " + std::to_string(byte_)) : "") +
                                ": " + what_arg;
                return parse_error(id, byte_, w.c_str());
            }

            /*!
            @brief byte index of the parse error

            The byte index of the last read character in the input file.

            @note For an input with n bytes, 1 is the index of the first character and
                  n+1 is the index of the terminating null byte or the end of file.
                  This also holds true when reading a byte vector (CBOR or MessagePack).
            */
            const std::size_t byte;

        private:
            parse_error(int id_, std::size_t byte_, const char *what_arg)
                    : exception(id_, what_arg), byte(byte_) {}
        };

/*!
@brief exception indicating errors with iterators

Exceptions have ids 2xx.

name / id                           | example message | description
----------------------------------- | --------------- | -------------------------
json.exception.invalid_iterator.201 | iterators are not compatible | The iterators passed to constructor @ref basic_json(InputIT first, InputIT last) are not compatible, meaning they do not belong to the same container. Therefore, the range (@a first, @a last) is invalid.
json.exception.invalid_iterator.202 | iterator does not fit current value | In an erase or insert function, the passed iterator @a pos does not belong to the JSON value for which the function was called. It hence does not define a valid position for the deletion/insertion.
json.exception.invalid_iterator.203 | iterators do not fit current value | Either iterator passed to function @ref erase(IteratorType first, IteratorType last) does not belong to the JSON value from which values shall be erased. It hence does not define a valid range to delete values from.
json.exception.invalid_iterator.204 | iterators out of range | When an iterator range for a primitive type (number, boolean, or string) is passed to a constructor or an erase function, this range has to be exactly (@ref begin(), @ref end()), because this is the only way the single stored value is expressed. All other ranges are invalid.
json.exception.invalid_iterator.205 | iterator out of range | When an iterator for a primitive type (number, boolean, or string) is passed to an erase function, the iterator has to be the @ref begin() iterator, because it is the only way to address the stored value. All other iterators are invalid.
json.exception.invalid_iterator.206 | cannot construct with iterators from null | The iterators passed to constructor @ref basic_json(InputIT first, InputIT last) belong to a JSON null value and hence to not define a valid range.
json.exception.invalid_iterator.207 | cannot use key() for non-object iterators | The key() member function can only be used on iterators belonging to a JSON object, because other types do not have a concept of a key.
json.exception.invalid_iterator.210 | iterators do not fit | The iterator range passed to the insert function are not compatible, meaning they do not belong to the same container. Therefore, the range (@a first, @a last) is invalid.
json.exception.invalid_iterator.211 | passed iterators may not belong to container | The iterator range passed to the insert function must not be a subrange of the container to insert to.
json.exception.invalid_iterator.212 | cannot compare iterators of different containers | When two iterators are compared, they must belong to the same container.
json.exception.invalid_iterator.213 | cannot compare order of object iterators | The order of object iterators cannot be compared, because JSON objects are unordered.
json.exception.invalid_iterator.214 | cannot get value | Cannot get value for iterator: Either the iterator belongs to a null value or it is an iterator to a primitive type (number, boolean, or string), but the iterator is different to @ref begin().

@since version 3.0.0
*/
        class invalid_iterator : public exception {
        public:
            static invalid_iterator create(int id, const std::string &what_arg) {
                std::string w = exception::name("invalid_iterator", id) + what_arg;
                return invalid_iterator(id, w.c_str());
            }

        private:
            invalid_iterator(int id_, const char *what_arg)
                    : exception(id_, what_arg) {}
        };

/*!
@brief exception indicating executing a member function with a wrong type

Exceptions have ids 3xx.

name / id                     | example message | description
----------------------------- | --------------- | -------------------------
json.exception.type_error.301 | cannot create object from initializer list | To create an object from an initializer list, the initializer list must consist only of a list of pairs whose first element is a string. When this constraint is violated, an array is created instead.
json.exception.type_error.302 | type must be object, but is array | During implicit or explicit value conversion, the JSON type must be compatible to the target type. For instance, a JSON string can only be converted into string types, but not into numbers or boolean types.
json.exception.type_error.303 | incompatible ReferenceType for get_ref, actual type is object | To retrieve a reference to a value stored in a @ref basic_json object with @ref get_ref, the type of the reference must match the value type. For instance, for a JSON array, the @a ReferenceType must be @ref array_t&.
json.exception.type_error.304 | cannot use at() with string | The @ref at() member functions can only be executed for certain JSON types.
json.exception.type_error.305 | cannot use operator[] with string | The @ref operator[] member functions can only be executed for certain JSON types.
json.exception.type_error.306 | cannot use value() with string | The @ref value() member functions can only be executed for certain JSON types.
json.exception.type_error.307 | cannot use erase() with string | The @ref erase() member functions can only be executed for certain JSON types.
json.exception.type_error.308 | cannot use push_back() with string | The @ref push_back() and @ref operator+= member functions can only be executed for certain JSON types.
json.exception.type_error.309 | cannot use insert() with | The @ref insert() member functions can only be executed for certain JSON types.
json.exception.type_error.310 | cannot use swap() with number | The @ref swap() member functions can only be executed for certain JSON types.
json.exception.type_error.311 | cannot use emplace_back() with string | The @ref emplace_back() member function can only be executed for certain JSON types.
json.exception.type_error.312 | cannot use update() with string | The @ref update() member functions can only be executed for certain JSON types.
json.exception.type_error.313 | invalid value to unflatten | The @ref unflatten function converts an object whose keys are JSON Pointers back into an arbitrary nested JSON value. The JSON Pointers must not overlap, because then the resulting value would not be well defined.
json.exception.type_error.314 | only objects can be unflattened | The @ref unflatten function only works for an object whose keys are JSON Pointers.
json.exception.type_error.315 | values in object must be primitive | The @ref unflatten function only works for an object whose keys are JSON Pointers and whose values are primitive.

@since version 3.0.0
*/
        class type_error : public exception {
        public:
            static type_error create(int id, const std::string &what_arg) {
                std::string w = exception::name("type_error", id) + what_arg;
                return type_error(id, w.c_str());
            }

        private:
            type_error(int id_, const char *what_arg) : exception(id_, what_arg) {}
        };

/*!
@brief exception indicating access out of the defined range

Exceptions have ids 4xx.

name / id                       | example message | description
------------------------------- | --------------- | -------------------------
json.exception.out_of_range.401 | array index 3 is out of range | The provided array index @a i is larger than @a size-1.
json.exception.out_of_range.402 | array index '-' (3) is out of range | The special array index `-` in a JSON Pointer never describes a valid element of the array, but the index past the end. That is, it can only be used to add elements at this position, but not to read it.
json.exception.out_of_range.403 | key 'foo' not found | The provided key was not found in the JSON object.
json.exception.out_of_range.404 | unresolved reference token 'foo' | A reference token in a JSON Pointer could not be resolved.
json.exception.out_of_range.405 | JSON pointer has no parent | The JSON Patch operations 'remove' and 'add' can not be applied to the root element of the JSON value.
json.exception.out_of_range.406 | number overflow parsing '10E1000' | A parsed number could not be stored as without changing it to NaN or INF.

@since version 3.0.0
*/
        class out_of_range : public exception {
        public:
            static out_of_range create(int id, const std::string &what_arg) {
                std::string w = exception::name("out_of_range", id) + what_arg;
                return out_of_range(id, w.c_str());
            }

        private:
            out_of_range(int id_, const char *what_arg) : exception(id_, what_arg) {}
        };

/*!
@brief exception indicating other errors

Exceptions have ids 5xx.

name / id                      | example message | description
------------------------------ | --------------- | -------------------------
json.exception.other_error.501 | unsuccessful: {"op":"test","path":"/baz", "value":"bar"} | A JSON Patch operation 'test' failed. The unsuccessful operation is also printed.
json.exception.other_error.502 | invalid object size for conversion | Some conversions to user-defined types impose constraints on the object size (e.g. std::pair)

@since version 3.0.0
*/
        class other_error : public exception {
        public:
            static other_error create(int id, const std::string &what_arg) {
                std::string w = exception::name("other_error", id) + what_arg;
                return other_error(id, w.c_str());
            }

        private:
            other_error(int id_, const char *what_arg) : exception(id_, what_arg) {}
        };
    }
}
#endif //XCAPTCHA_EXCEPTION_H
