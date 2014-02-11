/********************************************************/
/*
	Player PRO -- Music Driver EXAMPLE

	Library Version

	To use with Think C & CodeWarrior

	Antoine ROSSET
	20 Micheli-Du-Crest
	1205 Geneva
	SWITZERLAND
	
	PHONE: (+41 79) 203 74 62
	Email: rossetantoine@bluewin.ch
*/
/********************************************************/
#include <Carbon/Carbon.h>
#include <PlayerPROCore/PlayerPROCore.h>

OSErr MyAEGetDescData(const AEDesc *desc, DescType *typeCode, void *dataBuffer, ByteCount maximumSize, ByteCount *actualSize)
{
	Handle		h;
	ByteCount	dataSize;
	ByteCount	acSize;
	
	if (typeCode) *typeCode = desc->descriptorType;
	
	h = (Handle)desc->dataHandle;
	dataSize = GetHandleSize(h);
	if (dataSize > maximumSize)
		acSize = maximumSize;
	else
		acSize = dataSize;
	BlockMoveData(*h, dataBuffer, acSize);
	
	if (actualSize) *actualSize = acSize;
	
	return noErr;
}

extern OSErr PPIdentifyFile(MADLibrary *lib, Ptr, Ptr);

/*****************************/
/****** MAIN FUNCTION ********/
/*****************************/

int main(int argc, char* argv[])
{
	Boolean					End = false;
	MADDriverRec			*MADDriver;
	MADMusic				*MADMusic;
	MADLibrary				*MADLib;
	
	/***************					****************/
	/****** 	 	Toolbox Initialization	  **********/
	/***************					****************/
	
	InitCursor();
	MoreMasters();
	
	if (NavServicesAvailable() == false) return EXIT_FAILURE;
	
	/*******************************************************************************************/
	/****** MAD Library Initialisation : choose the best driver for the current hardware  ******/
	/******																				  ******/
	/****** Standard initialization with 4 channels...									  ******/
	/*******************************************************************************************/
	
	{
		MADDriverSettings	init;
		
		
		/*
		 MANUAL DRIVER CONFIGURATION, by example:
		 
		 init.numChn				= 4;
		 init.outPutBits 		= 8;
		 init.outPutRate			= rate22khz;
		 init.outPutMode			= StereoOutPut;
		 init.driverMode			= SoundManagerDriver;
		 init.antiAliasing		= false;
		 init.repeatMusic		= false;
		 init.Interpolation		= false;
		 init.MicroDelay			= false;
		 init.MicroDelaySize 	= 35;
		 init.surround		 	= false;
		 init.sysMemory			= false;
		 init.Reverb				= false;
		 init.ReverbSize			= 45;
		 init.ReverbStrength		= 60;
		 init.ReverbStrength		= 60;
		 init.TickRemover		= false;
		 */
		
		/* or AUTOMATIC DRIVER CONFIGURATION FOR CURRENT MAC HARDWARE*/
		MADGetBestDriver(&init);
		
		init.driverMode = CoreAudioDriver;
		
		{
			FSSpec	appSpec;
			
			HGetVol(NULL, &appSpec.vRefNum, &appSpec.parID);
			
			pStrcpy(appSpec.name, "\pPlugs");
			
			if (MADInitLibrary(&appSpec, init.sysMemory, &MADLib) != noErr) DebugStr("\pSmall Problem...");
		}
		
		if (MADCreateDriver(&init, MADLib, &MADDriver) != noErr) DebugStr("\pSmall Problem...");
	}
	/*********************************/
	/*********************************/
	/*********************************/
	
	
	MADStartDriver(MADDriver);				// Turn interrupt driver function ON
	
	/****** Open a music file via Plugs ********/
	while (End == false)
	{
		OSErr					iErr;
		FSSpec				spec;
		
		FlushEvents(everyEvent, 0);
		
		
		// Open Get File Dialog
		{
			NavReplyRecord			theReply;
			NavDialogOptions		dialogOptions;
			NavTypeListHandle		openList;
			
			
			// default behavior for browser and dialog:
			iErr = NavGetDefaultDialogOptions(&dialogOptions);
			
			openList = (NavTypeListHandle) NewHandle(sizeof(NavTypeList) + 2 * sizeof(OSType));
			if (openList ) HLock((Handle)openList);
			
			(*openList)->componentSignature = 'SNPL';
			(*openList)->osTypeCount		= 2;
			(*openList)->osType[0]			= 'MADK';
			(*openList)->osType[1]			= 'STrk';
			
			iErr = NavGetFile(			NULL,	// use system's default location
							  &theReply,
							  &dialogOptions,
							  NULL,
							  NULL,	// no custom previews
							  NULL,
							  openList, //,
							  (NavCallBackUserData) 2L);
			
			if (openList != NULL)
			{
				HUnlock((Handle)openList);
				DisposeHandle((Handle)openList);
			}
			
			if (theReply.validRecord && iErr == noErr)
			{
				AEDesc 	resultDesc;
				long	index, count;
				
				// we are ready to open the document(s), grab information about each file for opening:
				iErr = AECountItems(&(theReply.selection),&count);
				for (index=1;index<=count;index++)
				{
					AEKeyword keyword;
					
					if ((iErr = AEGetNthDesc(&(theReply.selection),index,typeFSS, &keyword, &resultDesc)) == noErr)
					{
						if ((iErr = MyAEGetDescData (&resultDesc, NULL, &spec, sizeof (FSSpec ), NULL )) == noErr)
							
							iErr = AEDisposeDesc(&resultDesc);
					}
				}
				
				iErr = NavDisposeReply(&theReply);	// clean up after ourselves	
			}
			else iErr = -1;
		}
		
		if (iErr != noErr) End = true;
		else
		{
			char	type[5];
#if 0
			FInfo	info;
			
			FSpGetFInfo(&spec, &info);
			
			OSType2Ptr(info.fdType, type);
#else
			Str63 filename;
			memcpy(filename, spec.name, spec.name[0]+1);
			FSSpec restoreSpec;
			HGetVol(NULL, &restoreSpec.vRefNum, &restoreSpec.parID);
			HSetVol(NULL, spec.vRefNum, spec.parID);
			MYP2CStr(filename);
			PPIdentifyFile(MADLib, type, filename);
			HSetVol(NULL, restoreSpec.vRefNum, restoreSpec.parID);
#endif
			if (MADPlugAvailable(MADLib, type))		// Is available a plug to open this file?
			{
				if (MADLoadMusicFSpFile(MADLib, &MADMusic, type, &spec) == noErr)		// Load this music with help of Plugs
					// in application folder, in 'Plugs' folder or internal resources
				{
					MADAttachDriverToMusic(MADDriver, MADMusic, NULL);
					MADPlayMusic(MADDriver);					// Read the current partition in memory
					
					while (!Button())
					{
						/// Do what you want here....
						/// Bla bla...
						/// By example:	Run your realtime 3D game
						///         	with 24bit texture mapping...
						
						long	fT, cT;
						
						MADGetMusicStatus(MADDriver, &fT, &cT);	// Some infos about current music
					}
					MADStopMusic(MADDriver);					// Stop reading current partition
					MADCleanDriver(MADDriver);
					MADDisposeMusic(&MADMusic, MADDriver);			// Dispose the current music
				}
			}
		}
	}
	MADStopDriver(MADDriver);				// Stop driver interrupt function
	MADDisposeDriver(MADDriver);			// Dispose music driver
	MADDisposeLibrary(MADLib);				// Close music library
	FlushEvents(everyEvent, 0);			// Kill your events and byebye...
	
	return EXIT_SUCCESS;
}
