/*
* @file: AAType.h
* @brief: flexible data type used in AARFS
* @author: dongzhaoyu
* @date: 2008-4-10
*
* Copyright (c) 2008 CTEC Xi'an JiaoTong University
*/

#ifndef _AASTDTYPE_H
#define _AASTDTYPE_H

//the definition of basic data types
typedef unsigned char			U8;
typedef signed char				S8;
typedef unsigned short			U16;
typedef signed short			S16;
typedef unsigned int			U32;
typedef signed int				S32;

#if WIN32
typedef unsigned __int64		U64;
typedef signed __int64			S64;
#else
typedef long long int			S64;
typedef long long unsigned int	U64;
#define S64L(a)					(a##LL)
#define U64L(a)					(a##ULL)
#endif

typedef float					F32;
typedef double					F64;
typedef S32						Bool;

#define U8_MIN					(0)
#define U8_MAX					(UCHAR_MAX)
#define S8_MIN					(SCHAR_MIN)
#define S8_MAX					(SCHAR_MAX)
#define U16_MIN					(0)
#define U16_MAX					(USHRT_MAX)
#define S16_MIN					(SHRT_MIN)
#define S16_MAX					(SHRT_MAX)
#define U32_MIN					(0)
#define U32_MAX					(UINT_MAX)
#define S32_MIN					(0)
#define S32_MAX					(INT_MAX)
#define U64_MIN					(0)
#define U64_MAX					(ULLONG_MAX)
#define S64_MIN					(LLONG_MIN)
#define S64_MAX					(LLONG_MAX)
#define F32_MIN					(1.175494351e-38F)
#define F32_MAX					(3.402823466e+38F)
#define F64_MIN					(2.2250738585072014e-308)
#define F64_MAX					(1.7976931348623158e+308)

#if !defined TRUE && !defined FALSE
#define TRUE					(1)
#define FALSE					(0)
#endif
//

#endif