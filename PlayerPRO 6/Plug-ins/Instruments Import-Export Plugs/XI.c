/*	XI				*/
/*  IMPORT/EXPORT	*/
/*	v 1.0			*/
/*	1996 by ANR		*/

#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>

#ifndef WIN32
//Windows Defines
typedef SInt16			WORD;
typedef UInt16			UWORD;
typedef SInt32			HRESULT;
typedef UInt32			ULONG;
typedef void*			LPVOID;
typedef SInt32			LONG;

typedef UInt16			UINT;
typedef Boolean			BOOL;
typedef UInt32			DWORD;
typedef UInt16			USHORT;
typedef SInt16			SHORT;
typedef FourCharCode	FOURCC;
typedef SInt8			BYTE;
#endif

typedef UInt8			UBYTE;

#include "XM.h"

static inline OSErr TestXI(void *CC)
{
	OSType Ident = *((OSType*)CC);
	MADBE32(&Ident);
	
	if (Ident == 'Exte')
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

static OSErr MAD2KillInstrument(InstrData *curIns, sData **sample)
{
	short firstSample;
	
	if (!curIns || !sample)
		return MADParametersErr;
	
	dispatch_apply(curIns->numSamples, dispatch_get_global_queue(0, 0), ^(size_t i) {
		if (sample[i] != NULL) {
			if (sample[i]->data != NULL) {
				free(sample[i]->data);
				sample[i]->data = NULL;
			}
			free(sample[i]);
			sample[i] = NULL;
		}
	});
	
	firstSample = curIns->firstSample;
	memset(curIns, 0, sizeof(InstrData));
	curIns->firstSample = firstSample;
	
	return MADNoErr;
}

//hack around the fact that there isn't an equivalent of CFStringGetMaximumSizeOfFileSystemRepresentation for CFURLs
static CFIndex getCFURLFilePathRepresentationLength(CFURLRef theRef, Boolean resolveAgainstBase)
{
	CFStringRef fileString = CFURLCopyFileSystemPath(theRef, kCFURLPOSIXPathStyle);
	CFIndex strLength = CFStringGetMaximumSizeOfFileSystemRepresentation(fileString);
	CFRelease(fileString);
	return strLength;
}

static OSErr mainXI(void		*unused,
					OSType		order,			// Order to execute
					InstrData	*InsHeader,		// Ptr on instrument header
					sData		**sample,		// Ptr on samples data
					short		*sampleID,		// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
					// If sampleID == -1 : add sample else replace selected sample.
					CFURLRef	AlienFileCFURL,	// IN/OUT file
					PPInfoPlug	*thePPInfoPlug)
{
	OSErr	myErr = MADNoErr;
	UNFILE	iFileRefI;
	short	x;
	long	inOutCount;
	
	char *file = NULL;
	char *fileName = NULL;
	
	char *longStr = NULL;
	CFIndex pathLen = getCFURLFilePathRepresentationLength(AlienFileCFURL, TRUE);
	longStr = malloc(pathLen);
	if (!longStr) {
		return MADNeedMemory;
	}
	Boolean pathOK = CFURLGetFileSystemRepresentation(AlienFileCFURL, true, (unsigned char*)longStr, pathLen);
	if (!pathOK) {
		free(longStr);
		return MADReadingErr;
	}
	file = realloc(longStr, strlen(longStr) + 1);
	if (!file)
		file = longStr;

	char *FileNameLong = NULL;
	CFStringRef filenam = CFURLCopyLastPathComponent(AlienFileCFURL);
	CFIndex filenamLen = CFStringGetMaximumSizeForEncoding(CFStringGetLength(filenam), kCFStringEncodingMacRoman);
	filenamLen *= 2;
	FileNameLong = malloc(filenamLen);
	CFStringGetCString(filenam, FileNameLong, filenamLen, kCFStringEncodingMacRoman);
	CFRelease(filenam);
	fileName = realloc(FileNameLong, strlen(FileNameLong) + 1);
	if (!fileName)
		fileName = FileNameLong;
	
	switch (order) {
		case MADPlugImport:
		{
			Ptr				theXI;
			XMPATCHHEADER	*pth;
			XMWAVHEADER		*wh = NULL;
			short			numSamples;
			
			iFileRefI = iFileOpenRead(file);
			if (iFileRefI != NULL) {
				inOutCount = iGetEOF(iFileRefI);
				
				theXI = malloc(inOutCount);
				if (theXI == NULL)
					myErr = MADNeedMemory;
				else {
					MAD2KillInstrument(InsHeader, sample);
					
					memset(InsHeader->name, 0, sizeof(InsHeader->name));
					
					for (x = 0; x < 32; x++) {
						if (fileName[x] == '\0') {
							break;
						}
						InsHeader->name[x] = fileName[x];
						
					}
					
					//inOutCount = iGetEOF(iFileRefI);
					
					iRead(inOutCount, theXI, iFileRefI);
					
					// READ instrument header
					
					pth = (XMPATCHHEADER*)(theXI + 0x42);
					
					numSamples = *((short*)(theXI + 0x42 + sizeof(XMPATCHHEADER)));
					MADLE16(&numSamples);
					InsHeader->numSamples = numSamples;
					
					MADLE16(&pth->volfade);
					
					memcpy(InsHeader->what, pth->what, 96);
					memcpy(InsHeader->volEnv, pth->volenv, 48);
#ifdef __BIG_ENDIAN__
					for (x = 0; x < 12; x++) {
						MADLE16(&InsHeader->volEnv[x].pos);
						MADLE16(&InsHeader->volEnv[x].val);
					}
#endif
					
					InsHeader->volSize	= pth->volpts;
					InsHeader->volType	= pth->volflg;
					InsHeader->volSus	= pth->volsus;
					InsHeader->volBeg	= pth->volbeg;
					InsHeader->volEnd	= pth->volend;
					InsHeader->volFade	= pth->volfade;
					
					memcpy(InsHeader->pannEnv, pth->panenv, 48);
#ifdef __BIG_ENDIAN__
					for (x = 0; x < 12; x++) {
						MADLE16(&InsHeader->pannEnv[x].pos);
						MADLE16(&InsHeader->pannEnv[x].val);
					}
#endif
					
					InsHeader->pannSize	= pth->panpts;
					InsHeader->pannType	= pth->panflg;
					InsHeader->pannSus	= pth->pansus;
					InsHeader->pannBeg	= pth->panbeg;
					InsHeader->pannEnd	= pth->panend;
					
					// Read SAMPLE HEADERS
					
					for (x = 0; x < InsHeader->numSamples; x++) {
						sData	*curData;
						int 	finetune[16] = {
							7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
							8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
						};
						
						wh = (XMWAVHEADER*)(theXI + 0x42 + 0x02 + sizeof(XMPATCHHEADER) + x * sizeof(XMWAVHEADER));
						
						MADLE32(&wh->length);
						MADLE32(&wh->loopstart);
						MADLE32(&wh->looplength);
						
						///////////
						
						curData = sample[x] = inMADCreateSample();
						
						curData->size		= wh->length;
						curData->loopBeg 	= wh->loopstart;
						curData->loopSize 	= wh->looplength;
						
						curData->vol		= wh->volume;
						curData->c2spd		= finetune[(wh->finetune + 128) / 16];
						curData->loopType	= 0;
						curData->amp		= 8;
						if (wh->type & 0x10) {		// 16 Bits
							curData->amp = 16;
						}
						
						if (!(wh->type & 0x3)) {
							curData->loopBeg = 0;
							curData->loopSize = 0;
						}
						
						//curData->panning	= wh->panning;
						curData->relNote	= wh->relnote;
						strlcpy(curData->name, wh->samplename, sizeof(wh->samplename));
					}
					
					// Read SAMPLE DATA
					{
						Ptr reader = (Ptr)wh;
						
						reader += sizeof(XMWAVHEADER);
						
						for (x = 0; x < InsHeader->numSamples; x++) {
							sData *curData = sample[x];
							
							curData->data = malloc(curData->size);
							if (curData->data != NULL) {
								memcpy(curData->data, reader, curData->size);
								
								if (curData->amp == 16) {
									short	*tt = (short*)curData->data;
									
#ifdef __BIG_ENDIAN__
									long	tL;
									
									for (tL = 0; tL < curData->size / 2; tL++) {
										MADLE16((Ptr)(tt + tL));
									}
#endif
									
									{
										/* Delta to Real */
										long	oldV, newV;
										long	xL;
										
										oldV = 0;
										
										for (xL = 0; xL < curData->size / 2; xL++) {
											newV = tt[xL] + oldV;
											tt[xL] = newV;
											oldV = newV;
										}
									}
								} else {
									/* Delta to Real */
									long	oldV, newV;
									long	xL;
									
									oldV = 0;
									
									for (xL = 0; xL < curData->size; xL++) {
										newV = curData->data[xL] + oldV;
										curData->data[xL] = newV;
										oldV = newV;
									}
								}
							}
							
							reader += curData->size;
						}
					}
				}
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
		}
			break;
			
		case MADPlugTest:
		{
			Ptr	theSound;
			
			iFileRefI = iFileOpenRead(file);
			if (iFileRefI != NULL) {
				inOutCount = 50;
				theSound = malloc(inOutCount);
				if (theSound == NULL)
					myErr = MADNeedMemory;
				else {
					iRead(inOutCount, theSound, iFileRefI);
					
					myErr = TestXI(theSound);
				}
				
				free(theSound);
				iClose(iFileRefI);
			} else
				myErr = MADReadingErr;
		}
			break;
			
		case MADPlugExport:
			iFileCreate(file, 'XI  ');
			iFileRefI = iFileOpenWrite(file);
			
			if (iFileRefI != NULL) {
				// Write instrument header
				
				short			u, i, x;
				long			inOutCount = 0;
				XMPATCHHEADER	pth;
				char			start[0x42] = "Extended Instrument:                       \241FastTracker v2.00   \x02\x01";
				
				inOutCount = 0x42;
				iWrite(inOutCount, start, iFileRefI);
				
				memcpy(pth.what, InsHeader->what, 96);
				memcpy(pth.volenv, InsHeader->volEnv, 48);
				for (x = 0; x < 24; x++) {
					MADLE16(&pth.volenv[x]);
				}
				
				pth.volpts	= InsHeader->volSize;
				pth.volflg	= InsHeader->volType;
				pth.volsus	= InsHeader->volSus;
				pth.volbeg	= InsHeader->volBeg;
				pth.volend	= InsHeader->volEnd;
				pth.volfade = InsHeader->volFade;
				MADLE16(&pth.volfade);
				
				memcpy(pth.panenv, InsHeader->pannEnv, 48);
				for (x = 0; x < 24; x++) {
					MADLE16(&pth.panenv[x]);
				}
				
				pth.panpts = InsHeader->pannSize;
				pth.panflg = InsHeader->pannType;
				pth.pansus = InsHeader->pannSus;
				pth.panbeg = InsHeader->pannBeg;
				pth.panend = InsHeader->pannEnd;
				
				inOutCount = sizeof(XMPATCHHEADER);
				iWrite(inOutCount, &pth, iFileRefI);
				
				inOutCount = 2;
				x = InsHeader->numSamples;
				MADLE16(&x);
				iWrite(inOutCount, &x, iFileRefI);
				
				/** WRITE samples */
				
				for (u = 0 ; u < InsHeader->numSamples ; u++) {
					XMWAVHEADER		wh;
					sData			*curData;
					int				finetune[16] = {
						7895,	7941,	7985,	8046,	8107,	8169,	8232,	8280,
						8363,	8413,	8463,	8529,	8581,	8651,	8723,	8757
					};
					
					curData = sample[u];
					
					if (curData->stereo)
						wh.length = curData->size / 2;
					else
						wh.length = curData->size;
					
					inOutCount += wh.length;
					
					wh.loopstart	= curData->loopBeg;
					wh.looplength	= curData->loopSize;
					
					if (curData->stereo) {
						wh.loopstart /=2;
						wh.looplength /=2;
					}
					
					wh.volume = curData->vol;
					
					wh.finetune = -128;
					if (curData->c2spd > 8757)
						wh.finetune = 127;
					else {
						while (finetune[(wh.finetune + 128) / 16] < curData->c2spd) {
							wh.finetune += 16;
						}
					}
					wh.type = 0;
					if (curData->amp == 16)
						wh.type |= 0x10;
					if (curData->loopSize > 0)
						wh.type |= 0x3;
					
					//	wh.panning = curData->panning;
					wh.relnote = curData->relNote;
					strlcpy(wh.samplename, curData->name, sizeof(wh.samplename));
					
					MADLE32(&wh.length);
					MADLE32(&wh.loopstart);
					MADLE32(&wh.looplength);
					
					inOutCount = sizeof(wh);
					iWrite(inOutCount, &wh, iFileRefI);
				}
				
				for (u = 0 ; u < InsHeader->numSamples ; u++) {
					sData 	*curData = sample[u];
					char	*tempPtr;
					long	dstSize;
					
					tempPtr = malloc(curData->size);
					
					/// WriteData
					if (tempPtr != NULL) {
						memcpy(tempPtr, curData->data, curData->size);
						
						dstSize = curData->size;
						
						if (curData->amp == 16) {
							short	*tt = (short*)tempPtr;
							long	tL;
							
							/* Real to Delta */
							long oldV, newV;
							long xL;
							
							if (curData->stereo) {
								for (i = 0; i < dstSize; i++) tt[i] = tt[i*2];
								dstSize /= 2;
							}
							
							oldV = 0;
							
							for (xL = 0; xL < dstSize / 2; xL++) {
								newV = tt[xL];
								tt[xL] -= oldV;
								oldV = newV;
							}
							
							for (tL = 0; tL < dstSize / 2; tL++) {
								MADLE16((void*)(tt + tL));
							}
						} else {
							/* Real to Delta */
							long	oldV, newV;
							long	xL;
							char	*tt = (char*) tempPtr;
							
							if (curData->stereo) {
								for (i = 0; i < dstSize; i++)
									tt[i] = tt[i * 2];
								dstSize /= 2;
							}
							
							oldV = 0;
							
							for (xL = 0; xL < dstSize; xL++) {
								newV = tt[xL];
								tt[xL] -= oldV;
								oldV = newV;
							}
						}
						
						inOutCount = dstSize;
						iWrite(inOutCount, tempPtr, iFileRefI);
						
						free(tempPtr);
					} else
						myErr = MADNeedMemory;
				}
				iClose(iFileRefI);
			} else
				myErr = MADWritingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	free(file);
	free(fileName);
	
	return myErr;
}

#define PLUGUUID CFUUIDGetConstantUUIDWithBytes(kCFAllocatorSystemDefault, 0x62, 0x08, 0x75, 0xF5, 0x1E, 0x38, 0x45, 0xEF, 0x9F, 0xBA, 0xAA, 0xE9, 0x29, 0x50, 0x2D, 0x63)
//620875F5-1E38-45EF-9FBA-AAE929502D63
#define PLUGINFACTORY XIFactory //The factory name as defined in the Info.plist file
#define PLUGMAIN mainXI //The old main function, renamed please

#include "CFPlugin-InstrBridge.c"