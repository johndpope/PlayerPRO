/********************						***********************/
//
//	Player PRO 4.5x --	Usefull functions for sound file import/export
//
//	Version 3.0
//
//	To use with PlayerPRO & CodeWarrior ( current vers 8)
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//	
//	FAX:			(+41 22) 346 11 97
//	Compuserve:		100277,164
//	Phone:			(+41 79) 203 74 62
//	Internet: 		rosset@dial.eunet.ch
/********************						***********************/

#include <PlayerPROCore/PlayerPROCore.h>
#include <Sound.h>

void ConvertInstrumentIn( register Byte *tempPtr, register long sSize)
{
	register Byte val = 0x80;

	while( sSize > 0)
	{
		sSize--;
		*(tempPtr + sSize) -= val;
	}
}

sData* inMADCreateSample()
{
	sData	*curData;

	curData = (sData*) NewPtrClear( sizeof( sData));
	
	curData->size		= 0;
	curData->loopBeg	= 0;
	curData->loopSize	= 0;
	curData->vol		= MAX_VOLUME;
	curData->c2spd		= NOFINETUNE;
	curData->loopType	= 0;
	curData->amp		= 8;
	curData->relNote	= 0;
	curData->data		= NULL;
	
	return curData;		
}

OSErr inAddSoundToMAD(Ptr			theSound,
					  long			lS,
					  long			lE,
					  short			sS,
					  short			bFreq,
					  unsigned long	rate,
					  Boolean		stereo,
					  Str255		name,
					  InstrData		*InsHeader,					// Ptr on instrument header
					  sData			**sample,					// Ptr on samples data
					  short			*sampleID)
{
	long 	inOutBytes, i;
	sData	*curData;

	if( theSound == NULL) return MADParametersErr;

	if( *sampleID > MAXSAMPLE) return MADParametersErr;

	inOutBytes = GetPtrSize( theSound);
	
	///////
	
	if( *sampleID >= 0)		// replace this sample
	{
		curData = sample[ *sampleID];
	}
	else					// add a sample : why? because *sampleID == -1
	{
		*sampleID = InsHeader->numSamples;
		InsHeader->numSamples++;
		
		curData = sample[ *sampleID] = inMADCreateSample();
	}
	
	if( curData->data != NULL) DisposePtr( curData->data);
	curData->data = theSound;
	
	curData->size		= inOutBytes;
	curData->loopBeg	= lS;
	curData->loopSize	= lE - lS;
	curData->vol		= 64;
	curData->amp		= sS;
	curData->c2spd		= rate >> 16L;
	curData->relNote	= 60 - bFreq;
	curData->stereo		= stereo;
	
	for(i=0; i<32; i++)
	{
		if( i < name[ 0]) curData->name[i] = name[i+1];
		else curData->name[i] = '\0';
	}
	
	return noErr;
}
