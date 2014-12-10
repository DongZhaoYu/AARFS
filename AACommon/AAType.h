/*
* @file: AAType.h
* @brief: flexible data type defined in AARFS, but it is not used now
* @author: dongzhaoyu
* @date: 2008-4-10
*
* Copyright (c) 2008 CTEC Xi'an JiaoTong University
*/

#ifndef _AATYPE_H
#define _AATYPE_H

#include <vector>
#include <string>
#include <map>

#include "AADate.h"

/*
*		AAType wants to apply a flexible data manipulation mechanism. Its behavior will be compatible with
*		all the basic data type used in AARFS. These basic data type includes bool integer double float char
*		and other set data structure.
*		the scalar data types are:
*			-Boolean	- true or false
*			-Integer	- a 32 bit signed integer
*			-Real		- a 64 bit floating point value
*			-String		- a zero-terminated unicode string
*			-Date		- an absolute point in time,UTC
*						  with resolution in second
*			-map		- a dictionary mapping string keys to AAType values
*			-array		- a sequence of AAType values
*/

class AAType
{
public:
	AAType();		//initialized to undefine
	~AAType();		//this class is NOT subclass

	AAType(const AAType&);
	void assign(const AAType&);
	AAType& operator=(const AAType &right)	{ assign(right), return *this;}

	void clear();	//reset to undefine

	// definition of scalar data type
	typedef Bool			Boolean
	typedef S32				Integer
	typedef F64				Real
	typedef std::string		String
	typedef AADate			Date

	//scalar data type constructor
	AAType(Boolean);
	AAType(Integer);
	AAType(Real);
	AAType(const String&);
	AAType(const Date&);

	AAType(float);	//convert float to Real

	//scalar data value assigne
	void assign(Boolean);
	void assign(Interger);
	void assign(Real);
	void assign(const String&);
	void assign(const Date&);

	AAType& operator=(Boolean right)		{ assign(right); return *this;}
	AAType& operator=(Integer right)		{ assign(right); return *this;}
	AAType& operator=(Real right)			{ assign(right); return *this;}
	AAType& operator=(const String& right)	{ assign(right); return *this;}
	AAType& operator=(const Date& right)	{ assign(right); return *this;}

	//conversion among basic data types
	Boolean		asBoolean() const;
	Integer		asInteger() const;
	Real		asReal() const;
	String		asString() const;
	Date		asDate() const;

	operator Boolean() const		{ return asBoolean();}
	operator Integer() const		{ return asInteger();}
	operator Real() const			{ return asReal();}
	operator String() const			{ return asString();}
	operator Date() const			{ return asDate();}

	operator !() const				{ return !asBoolean();}

	//helper method for working with char*
	AAType(char *);
	void assign(const char*);
	AAType& operator=(const char *right)	{ assign(right); return *this;}

	//Map values
	static AAType getMap();

	bool has(const String&) const;
	AAType get(const String&) const;
	void insert(const String&, const AAType&);
	void erase(const String&);

	AAType& operator[](const String&);
	AAType& operator[](const char *key)	{ return (*this)[String(key)];}
	const AAType& operator[](const String&) const;
	const AAType& operator[](const char *key) const		{return (*this)[String(key)];}

	//Array values
	static AAType getArray();

	AAType get(Integer) const;
	void set(Integer, const AAType&);
	void insert(Integer, const AAType&);
	void append(const AAType&);
	void erase(Integer);

	//iterator for map and array
	int size() const;

	typedef std::map<String, AAType>::iterator map_iterator;
	typedef std::map<String, AAType>::const_iterator const_map_iterator;

	map_iterator beginMap();
	map_iterator endMap();
	const_map_iterator beginMap() const;
	const_map_iterator endMap() const;

	typedef std::vector<AAType>::iterator array_iterator;
	typedef std::vector<AAType>::const_iterator const_array_iterator;

	array_iterator beginArray();
	array_iterator endArray();
	const_array_iterator beginArray() const;
	const_array_iterator endArray() const;

	//data type testing
	typedef enum _Type{
		TUndefined,
		TBoolean,
		TInteger,
		TReal,
		TString,
		TDate,
		TMap,
		TArray
	}Type;

	Type type() const;

	bool isUndefined() const		{ return type() == TUndefined;}
	bool isDefined() const			{ return type() != TUndefined;}
	bool isBoolean() const			{ return type() == TBoolean;}
	bool isInteger() const			{ return type() == TInteger;}
	bool isReal() const				{ return type() == TReal;}
	bool isString() const			{ return type() == TString;}
	bool isDate() const				{ return type() == TDate;}
	bool isMap() const				{ return type() == TMap;}
	bool isArray() const			{ return type() == TArray;}

	//implementation class
	class impl;

private:
	impl *_impl;
};
#endif