// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_FORWARDS_H_INCLUDED
#define JSON_FORWARDS_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
#include "config.h"
#endif // if !defined(JSON_IS_AMALGAMATION)

namespace Json {

// writer.h
class FastWriter;
class StyledWriter;

// reader.h
class Reader;

// features.h
class Features;

// value.h
typedef unsigned int ArrayIndex;
class JSON_API StaticString;
class JSON_API Path;
class JSON_API PathArgument;
class JSON_API Value;
class JSON_API ValueIteratorBase;
class JSON_API ValueIterator;
class JSON_API ValueConstIterator;

} // namespace Json

#endif // JSON_FORWARDS_H_INCLUDED
