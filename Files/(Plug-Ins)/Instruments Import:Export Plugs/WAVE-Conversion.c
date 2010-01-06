/*	WAV		*/
/*  IMPORT	*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <Sound.h>
#include "WAV.h"
#include <Movies.h>

#define WAVE_FORMAT_PCM		1
#define kmaxVolume			7
 
#define ksysFailed 			"\pSorry, This software requires system 7.0.0 or greater."

#define kStereoStr 			"\pStereo"
#define kMonoStr 			"\pMonaural"

OSErr TestWAV( PCMWavePtr CC)
{
	if( CC->ckid =='RIFF') return noErr;
	else return MADFileNotSupportedByThisPlug;
}

/*___________________ long byte swap for Intel <-> Motorola Conversions*/
//The following will only byteswap on a Big-endian machine

/*static inline UInt32 longswap(UInt32 ul)
{
	return EndianS32_LtoN(ul);
}*/
#define longswap(ul) EndianU32_LtoN(ul)

/*___________________ word byte swap for Intel <-> Motorola Conversions*/
//The following will only byteswap on a Big-endian machine

/*static inline UInt16 shrtswap(UInt16 us)
{
	return EndianS16_LtoN(us);
}*/
#define shrtswap(us) EndianU16_LtoN(us)

/*_______________________________________________________________________*/

Ptr ConvertWAV(FSSpec *fileSpec, long *loopStart, long *loopEnd, short	*sampleSize, unsigned long *rate, Boolean *stereo)
{
	PCMWavePtr	WAVERsrc = NULL;
	short		fRef;//, tempResRef, x;
	long		fSize;
	//int			theHit;
	//char		test;
 	//short		gRefNum;
 	//short		gVolSet;
	
	*stereo = false;
	
	if(!FSpOpenDF(fileSpec, fsRdWrPerm, &fRef))
	{
		GetEOF(fRef, &fSize);
		if(!(WAVERsrc = (PCMWavePtr) NewPtr(fSize))) 
		{
			FSCloseFork(fRef); return NULL;
		}
		
		if(FSRead(fRef, &fSize, &(*WAVERsrc)))
		{
			FSCloseFork(fRef); return NULL;
		}
		
		if((*WAVERsrc).ckid =='RIFF')
		{
			(*WAVERsrc).cksize = longswap((*WAVERsrc).cksize);
			
			if((*WAVERsrc).fccType =='WAVE')
			{
				(*WAVERsrc).dwDataOffset = longswap((*WAVERsrc).dwDataOffset);
				
				if((*WAVERsrc).fmtType,'fmt ')
				{
					(*WAVERsrc).wFormatTag      = shrtswap((*WAVERsrc).wFormatTag);
					(*WAVERsrc).nCannels        = shrtswap((*WAVERsrc).nCannels);
					(*WAVERsrc).nSamplesPerSec  = longswap((*WAVERsrc).nSamplesPerSec);
					(*WAVERsrc).nSamplesPerSec  = (*WAVERsrc).nSamplesPerSec << 16;
					(*WAVERsrc).nAvgBytesPerSec = longswap((*WAVERsrc).nAvgBytesPerSec);
					(*WAVERsrc).nBlockAlign     = shrtswap((*WAVERsrc).nBlockAlign);
					(*WAVERsrc).wBitsPerSample  = shrtswap((*WAVERsrc).wBitsPerSample);
					(*WAVERsrc).dataSize        = longswap((*WAVERsrc).dataSize);
					
					*loopStart	= 0;
					*loopEnd 	= 0;
					*sampleSize = (*WAVERsrc).wBitsPerSample;
					*rate		= (*WAVERsrc).nSamplesPerSec;
					
					if( (*WAVERsrc).nCannels == 2) *stereo = true;
					else *stereo = false;
					
					if((*WAVERsrc).wFormatTag != 1)
					{
						return NULL;
					}
				}
				else
				{
					DisposePtr( (Ptr) WAVERsrc);
					return NULL;
				}	
			}
			else
			{
				DisposePtr( (Ptr) WAVERsrc);
				return NULL;
			}
		}
		else
		{
			DisposePtr( (Ptr) WAVERsrc);
			return NULL;
		}
		FSCloseFork(fRef);
	}
	
	{
		long sndSize = WAVERsrc->dataSize;
		
		BlockMoveData( (Ptr)(long) (WAVERsrc->theData), (Ptr) WAVERsrc , sndSize);
		
		SetPtrSize( (Ptr) WAVERsrc, sndSize);
		
		switch( *sampleSize)
		{
			case 8:
				ConvertInstrumentIn( (Byte*) WAVERsrc, sndSize);
			break;
			
			case 16:
				{
					long			i;
					unsigned short	*tt = (unsigned short*) WAVERsrc;
					
					i = sndSize/2;
					while( i-- > 0) tt[ i] = shrtswap( tt[ i]);
				}
			break;
		}
	}
	return (Ptr) WAVERsrc;
}

OSErr ConvertDataToWAVE( FSSpec file, FSSpec *newfile, PPInfoPlug *thePPInfoPlug)
{
	//OSType					fileType;
	OSErr					iErr;
	Boolean					canceled;
	Movie 					theMovie;
	//Track					usedTrack;
	//TimeValue				addedDuration;
	//long					outFlags;
	short					resRefNum, /*ins, samp,*/ resId;
	//FInfo					fndrInfo;
	Cursor					watchCrsr;
	CursHandle				myCursH;
	Str255					resName;
	Boolean					dataRefWasChanged;
	
	iErr = EnterMovies();
	if( iErr)
	{
		return -1;
	}
	
	myCursH = GetCursor( 357);
	
	if( myCursH == NULL) Debugger();
	DetachResource( (Handle) myCursH);		HLock( (Handle) myCursH);
	watchCrsr = **myCursH;					HUnlock( (Handle) myCursH);		DisposeHandle((Handle) myCursH);
	
	resRefNum = 0;
	iErr = OpenMovieFile ( &file, &resRefNum, fsCurPerm);
	
	resId = 0;
	iErr = NewMovieFromFile( &theMovie, resRefNum, &resId, resName, 0, &dataRefWasChanged);
	
	CallUpdateALLWindowUPP();
	
	canceled = FALSE;
	
	if( !canceled && iErr == noErr)
	{
		pStrcpy( newfile->name, file.name);
		
		iErr = FindFolder( kOnSystemDisk, kTemporaryFolderType, kCreateFolder, &newfile->vRefNum, &newfile->parID);
		if( iErr == noErr)
		{
			/////////////////////////////////////////////////
			//		WAVE CONVERSION
			/////////////////////////////////////////////////
			
			
			SetCursor( &watchCrsr);
			
			iErr = FSpDelete( newfile);
			
			iErr = ConvertMovieToFile(	theMovie,
										0,
							 			newfile,
							 			'WAVE',
							 			'SNPL',
							 			smCurrentScript,
							 			NULL,
							 			0,
							 			0);
			
			DisposeMovie( theMovie);
			
			CloseMovieFile( resRefNum);
		}
	}
	
	ExitMovies();
	
	return iErr;
}
