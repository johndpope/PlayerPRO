/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#ifndef __PPC_FILEUTILS_H__
#define __PPC_FILEUTILS_H__

#ifndef __MADI__
#include "MAD.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////

#ifdef _MAC_H
#include <CoreServices/CoreServices.h>
#if !defined(__LP64__) && (__MAC_OS_X_VERSION_MIN_REQUIRED < 1050)
typedef SInt16 UNFILE;
#else
typedef FSIORefNum UNFILE;
#endif
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef FILE* UNFILE;
#endif
	
UNFILE iFileOpen( Ptr name);
void iFileCreate( Ptr name, OSType);

long iGetEOF( UNFILE iFileRefI);
OSErr iRead( long size, Ptr dest, UNFILE iFileRefI);
OSErr iWrite( long size, Ptr dest, UNFILE iFileRefI);
OSErr iSeekCur( long size, UNFILE iFileRefI);

void iClose( UNFILE iFileRefI);

char* MADstrcpy( char*, const char*) DEPRECATED_ATTRIBUTE;
int MADstrcmp( const char *dst, const char* src);

	
unsigned char* MYC2PStr( Ptr cStr);
void MYP2CStr( unsigned char *cStr);
OSType Ptr2OSType( Ptr str);
void OSType2Ptr( OSType type, Ptr str);
void pStrcpy(register unsigned char *s1, register unsigned char *s2);


////////////////////////////////////////////////////////////

#ifdef NOINLINE
void INT32( void *msg_buf);
void INT16( void *msg_buf);
void MOT32( void *msg_buf);
void MOT16( void *msg_buf);
#else

static inline void MADByteSwap32(void *msg_buf)
{
	UInt32			temp = *((UInt32*) msg_buf);
#ifdef _MAC_H
	*((UInt32*) msg_buf) = Endian32_Swap(temp);
#else
	*((UInt32*) msg_buf) = ((((temp & 0xff000000) >> 24) | (( temp & 0x00ff0000) >> 8) | (( temp & 0x0000ff00) << 8) | (temp & 0x000000ff) << 24));
#endif
}

static inline void MADByteSwap16(void *msg_buf)
{
	UInt16			buf = *((UInt16*) msg_buf);
#ifndef _MAC_H
	*((UInt16*) msg_buf) = (((((UInt16)buf)<<8) & 0xFF00) | ((((UInt16)buf)>>8) & 0x00FF));
#else
	*((UInt16*) msg_buf) = Endian16_Swap(buf);
#endif
}

/////////////////////////////////

static inline void MOT32(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

static inline void MOT16(void *msg_buf)
{
#ifdef __LITTLE_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}

/////////////////////////////////

static inline void INT32(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap32(msg_buf);
#endif
}

static inline void INT16(void *msg_buf)
{
#ifdef __BIG_ENDIAN__
	MADByteSwap16(msg_buf);
#endif
}
#endif

#ifdef __cplusplus
}
#endif

#endif
