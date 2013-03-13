/*	AIFF/AIFC		*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include "PPPlug.h"
#include "AIFF.h"
#include "sound.h"
#include "soundinput.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( OSType)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( InstrData*)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( sData**)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( short*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( FSSpec*)))
		| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof( PPInfoPlug*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif


/*Ptr		 AIFFtoSnd(	Ptr				sound,
					long			*loopStart,
					long			*loopEnd,
					short			*sampleSize,
					unsigned long	*rate,
					Boolean			*stereo)
{
long				i, SizeH, numSampleFrames, StartId, numChannels;
ContainerChunk		*CC;
ChunkHeader			*CH;
static	ExtCommonChunk		*CommC;
SoundDataChunk		*SData;
MarkerChunk			*marker;
Marker				*mm;
unsigned long		theFixed, soundSize;
Boolean				Compression = false;
ID					compType;
OSErr				result;

	CC = (ContainerChunk*) sound;

	*loopStart = *loopEnd = 0;
	*sampleSize = 8;
	*rate = rate22khz;
	*stereo = false;
	numChannels = 1;
	soundSize = 0;
	
	if( CC->formType == AIFCID) Compression = true;
	else if( CC->formType == AIFFID) Compression = false;

	for( i = sizeof( ContainerChunk); i < CC->ckSize;)
	{
		CH = (ChunkHeader*) (sound + i);
		if( CH->ckID == CommonID)
		{
			CommC = (ExtCommonChunk*) CH;			
			numChannels = CommC->numChannels;
			numSampleFrames = CommC->numSampleFrames;
			compType = CommC->compressionType;
			
			{
				struct myFloat80
				{
					SInt16 	exp;
					UInt16 	man[4];
				};
			
				struct myFloat80		*too;
				
				too = (struct myFloat80* ) &CommC->sampleRate;
				
				*rate = *((unsigned long*) too->man);
				*rate >>= (15L - (too->exp - 0x3FFFL));
			}
			
			*sampleSize = CommC->sampleSize;
			
			soundSize = numSampleFrames;
			
			if( *sampleSize == 16) soundSize *= 2;
			if( numChannels == 2) soundSize *= 2;

		}
		else if( CH->ckID == SoundDataID)
		{
			SData = (SoundDataChunk*) CH;
			StartId = i;
		}
		else if( CH->ckID == MarkerID)
		{
			marker = (MarkerChunk*) CH;
			if( marker->numMarkers == 2)
			{
				*loopStart = marker->Markers[ 0].position;
				mm = (Marker*) marker->Markers;
				mm = (Marker*) ((Ptr) mm + 8L);
				mm = (Marker*) ((Ptr) mm + marker->Markers[ 0].markerName[0]);
				*loopEnd = mm->position;
				
				if( *sampleSize == 16)
				{
					*loopStart *= 2;
					*loopEnd *= 2;
				}
			}
		}
		
		i += CH->ckSize;
		i += 8;
	}

	StartId += 16;
	
	SizeH = GetPtrSize( sound);
	if( numChannels == 1)
	{
		BlockMoveData( sound + StartId, sound, SizeH - StartId);
	}
	else if( numChannels == 2)
	{
		BlockMoveData( sound + StartId, sound, SizeH - StartId);
		*stereo = true;
	}
	else
	{
		if( *sampleSize == 8)
		{
			for( i = 0; i < SizeH - StartId; i++)
			{
				(sound)[ i] = (sound + StartId)[ i * numChannels];
			}
		}
		else
		{
			for( i = 0; i < (SizeH - StartId)/2; i++)
			{
				((short*) sound)[ i] = ((short*) (sound + StartId))[ i * numChannels];
			}
		}
	}
	
	if( Compression)
	{
			CompressionInfo 		cp;
			
			result = GetCompressionInfo( -1, compType, numChannels, *sampleSize, &cp);
			if (result != noErr) DebugStr("\pGetCompressionInfo");
			
			{
			SoundConverter			sc;
			SoundComponentData		inputFormat, outputFormat;
			unsigned long			inputFrames, outputFrames;
			unsigned long			inputBytes, outputBytes;
			Ptr						inputPtr, outputPtr;
			OSErr					err;
			Ptr						dstPtr;
			Str63					str;
			
			inputFormat.flags = 0;
			inputFormat.format = compType;
			inputFormat.numChannels = numChannels;
			inputFormat.sampleSize = *sampleSize;
			inputFormat.sampleRate = *rate;
			inputFormat.sampleCount = 0;
			inputFormat.buffer = nil;
			inputFormat.reserved = 0;
			
			outputFormat = inputFormat;
			if( *sampleSize == 8) outputFormat.format = kOffsetBinary;
			else outputFormat.format = k16BitBigEndianFormat;
			
			err = SoundConverterOpen(&inputFormat, &outputFormat, &sc);
			if (err != noErr)
			DebugStr("\pOpen failed");
			
			err = SoundConverterBeginConversion(sc);
			if (err != noErr)
			DebugStr("\pBegin Conversion failed");
			
			inputFrames = (SizeH - StartId) / cp.bytesPerFrame;
			
			dstPtr = NewPtr( inputFrames * numChannels * (*sampleSize/8) * cp.samplesPerPacket);
			if( dstPtr == 0L)
			{
				DisposePtr( sound);
				return 0L;
			}
			
			err = SoundConverterConvertBuffer(sc, sound, numSampleFrames, dstPtr, &outputFrames, &outputBytes);
			if (err != noErr)	DebugStr("\pConversion failed");
			
			soundSize = outputBytes;
			
			err = SoundConverterEndConversion(sc, dstPtr, &outputFrames, &outputBytes);
			if (err != noErr)
			DebugStr("\pEnd Conversion failed");
			
			if( outputFrames != 0) Debugger();
			if( outputBytes != 0) Debugger();
			
			err = SoundConverterClose(sc);
			if (err != noErr)
			DebugStr("\pClose failed");
			
			DisposePtr( sound);
			sound = dstPtr;
			}
	}
	
	SetPtrSize( sound, soundSize);
	
	return sound;
}*/

static OSErr TestAIFF( ContainerChunk* CC)
{
	if( CC->formType == AIFCID) return noErr;
	else if( CC->formType == AIFFID) return noErr;
	else return MADFileNotSupportedByThisPlug;
}

EXP OSErr main(		OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFileFSSpec,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	short	iFileRefI;
	long	inOutBytes;
	
	#ifndef powerc
		long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
	#endif
	
	switch( order)
	{
		case 'IMPL':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			FSSpec			newFile;
			
			myErr = ConvertDataToWAVE( *AlienFileFSSpec, &newFile, thePPInfoPlug);
			if( myErr == noErr)
			{
				theSound = ConvertWAV( &newFile, &lS, &lE, &sS, &rate, &stereo);
				
				if( theSound) inAddSoundToMAD( theSound, lS, lE, sS, 60, rate, stereo, AlienFileFSSpec->name, InsHeader, sample, sampleID);
				else
				{
					myErr = MADNeedMemory;
				}
				
				FSpDelete( &newFile);
			}
		}
		break;
		
		case 'TEST':
		{
			Ptr	theSound;
			
			myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
			if( myErr == noErr)
			{
				inOutBytes = 50L;
				theSound = NewPtr( inOutBytes);
				if( theSound == 0L) myErr = MADNeedMemory;
				else
				{
					FSRead( iFileRefI, &inOutBytes, theSound);
					
					myErr = TestAIFF( (ContainerChunk*) theSound);
				}
				
				DisposePtr( theSound);
				
				FSClose( iFileRefI);
			}
		}
		break;
		
		case 'EXPL':
			if( *sampleID >= 0)
			{
				OSType				compType = 'NONE';
				unsigned long		rate;
				sData 				*curData = sample[ *sampleID];
				short				numChan;
				
				myErr = FSpCreate( AlienFileFSSpec, 'TVOD', 'AIFF', smCurrentScript);
				myErr = FSpOpenDF( AlienFileFSSpec, fsCurPerm, &iFileRefI);
				
				if( myErr == noErr)
				{
					inOutBytes 	= curData->size;
					rate		= curData->c2spd;
					
					if( curData->stereo) numChan = 2;
					else numChan = 1;
					
					myErr = SetupAIFFHeader(	iFileRefI,
												numChan,
												rate << 16L,
												curData->amp,
												compType,
												inOutBytes,
												0);
					
					myErr = FSWrite( iFileRefI, &inOutBytes, curData->data);
					
					/*
					marker = (MarkerChunk*) CH;
					if( marker->numMarkers == 2)
					{
						*loopStart = marker->Markers[ 0].position;
						mm = (Marker*) marker->Markers;
						mm = (Marker*) ((Ptr) mm + 8L);
						mm = (Marker*) ((Ptr) mm + marker->Markers[ 0].markerName[0]);
						*loopEnd = mm->position;
						
						if( *sampleSize == 16)
						{
							*loopStart *= 2;
							*loopEnd *= 2;
						}
					}*/
					
					FSClose( iFileRefI);
				}
			}
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
	
	#ifndef powerc
		SetA4( oldA4);
	#endif
	
	return myErr;
}
