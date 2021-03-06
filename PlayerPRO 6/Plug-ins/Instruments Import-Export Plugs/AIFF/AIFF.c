/*	AIFF/AIFC						*/
/*  IMPORT/EXPORT					*/
/*	v 1.0							*/
/*	1996 by ANR						*/
/*  2013 rewrite by Madd the Sane	*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

#if 0
static OSErr mainAIFF(void					*unused,
					  OSType				order,			// Order to execute
					  InstrData				*InsHeader,		// Ptr on instrument header
					  sData					**sample,		// Ptr on samples data
					  short					*sampleID,		// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
					  // If sampleID == -1 : add sample else replace selected sample.
					  CFURLRef				AlienFileURL,	// IN/OUT file
					  PPInfoPlug			*thePPInfoPlug)
#endif

static MADErr mainAIFF(void *unused, OSType order, InstrData *InsHeader, sData **sample, short *sampleID, CFURLRef AlienFileURL, PPInfoPlug *thePPInfoPlug)
{
	MADErr	myErr = MADNoErr;
	//char	*AlienFile;

	
	switch(order) {
#if 0
		case 'IMPL':
		{
			char			*theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			FSSpec			newFile;
			
			myErr = ConvertDataToWAVE(*AlienFileFSSpec, &newFile, thePPInfoPlug);
			if (myErr == MADNoErr) {
				theSound = ConvertWAV(&newFile, &lS, &lE, &sS, &rate, &stereo);
				
				if (theSound)
					myErr = inAddSoundToMAD(theSound, lS, lE, sS, 60, rate, stereo, AlienFileFSSpec->name, InsHeader, sample, sampleID);
				else
					myErr = MADNeedMemory;
				
				FSpDelete(&newFile);
			}
		}
			break;
#endif
		case MADPlugImport:
		{
			AudioFileID theInID;
			OSStatus myStat = AudioFileOpenURL(AlienFileURL, kAudioFileReadPermission, 0, &theInID);
			if (myStat != noErr) {
				myErr = MADReadingErr;
			} else {
				AudioFileClose(theInID);
				myErr = MADOrderNotImplemented;
			}
		}
			
		case MADPlugTest:
		{
			AudioFileID audioFile;
			OSStatus res;

			res = AudioFileOpenURL(AlienFileURL, kAudioFileReadPermission, kAudioFileAIFFType, &audioFile);
			if (res != noErr) {
				res = AudioFileOpenURL(AlienFileURL, kAudioFileReadPermission, kAudioFileAIFCType, &audioFile);
				if (res != noErr) {
					myErr = MADFileNotSupportedByThisPlug;
				} else {
					AudioFileClose(audioFile);
				}
			} else {
				AudioFileClose(audioFile);
			}
		}
			break;
			
		case MADPlugExport:
			if (*sampleID >= 0)
			{
				char* data = NULL;
				sData *curData = sample[*sampleID];
				AudioStreamBasicDescription asbd = {0};
				asbd.mSampleRate = curData->c2spd;
				asbd.mFormatID = kAudioFormatLinearPCM;
				asbd.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagIsBigEndian;
				asbd.mBitsPerChannel = curData->amp;
				asbd.mChannelsPerFrame = curData->stereo ? 2 : 1;
				asbd.mFramesPerPacket = 1;
				asbd.mBytesPerFrame = asbd.mBitsPerChannel * asbd.mChannelsPerFrame / 8;
				asbd.mBytesPerPacket = asbd.mBytesPerFrame * asbd.mFramesPerPacket;
				
				AudioFileID audioFile;
				OSStatus res;
				if (curData->amp == 16) {
					data = malloc(curData->size);
					if (!data)
						return MADNeedMemory;
					memcpy(data, curData->data, curData->size);
					
					dispatch_apply(curData->size / 2, dispatch_get_global_queue(0, 0), ^(size_t i) {
						MADBE16(&((short*)data)[i]);
					});
				} else
					data = curData->data;
				
				res = AudioFileCreateWithURL(AlienFileURL, kAudioFileAIFFType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
				if (res != noErr) {
					myErr = MADWritingErr;
				} else {
					UInt32 numBytes = curData->size;
					
					res = AudioFileWriteBytes(audioFile, false, 0, &numBytes, data);
					if (res != noErr) {
						myErr = MADWritingErr;
					} else {
#if 0
						if (curData->loopBeg) {
							AIFFLoop theLoop = {0};
							AudioFileSetUserData(audioFile, 'LOOP', 0, sizeof(theLoop), &theLoop);
						}
						AudioFileOptimize(audioFile);
#endif
						if (curData->amp == 16)
							free(data);
						
						res = AudioFileClose(audioFile);
						if (res != noErr)
							myErr = MADWritingErr;
					}
				}
			}
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}

// C4B85FAC-BD58-4661-9004-CBBF84BA4EDD
#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0xC4, 0xB8, 0x5F, 0xAC, 0xBD, 0x58, 0x46, 0x61, 0x90, 0x04, 0xCB, 0xBF, 0x84, 0xBA, 0x4E, 0xDD)
#define PLUGINFACTORY AIFFFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainAIFF //The old main function, renamed please

#include "../CFPlugin-InstrBridge.c"
