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

/*!
 *	@header		RDriverInt.h
 *	@abstract	Internal functions used by PlayerPROCore.
 *	@discussion	Refrain from using this header, as the functions here might not even be available.
 *				Many functions have been removed or renamed over the course of PlayerPRO's lifetime,
 *				thus you should only use the functions here if you know what you're doing.
 */


#ifndef __RDRIVERINTH__
#define __RDRIVERINTH__

#include <stdio.h>
#ifndef __PPC_FILEUTILS_H__
#include "FileUtils.h"
#endif

//#define NUMBER_FINETUNES 16
#define AMIGA_CLOCKFREQ2 14317456

#ifdef __GNUC__
#define HAS_LONG_LONG 1
#define HAS_LONG_DOUBLE 1
#endif

#ifdef _MSC_VER
#define HAS_LONG_LONG 1
//MSVC's long double datatype is the same size as a regular double
#undef HAS_LONG_DOUBLE
#endif

#if TARGET_OS_IPHONE
//iOS also has long double the same size as double
#undef HAS_LONG_DOUBLE
#endif

#if defined(HAS_LONG_LONG) && defined(HAS_LONG_DOUBLE)
// Work on 64bits for much better precision
#define BYTEDIV 16
#else
#define BYTEDIV 8
#endif

#define	EXTRASMALLCOUNTER 5
//#define POSPITCH		170
//#define SoundActive  	0x27E

/**  Some #define functions for easiest access **/

#define SampleDataN(InsD,NoteD)		curMusic->sample[curMusic->fid[(InsD)].firstSample + curMusic->fid[(InsD)].what[(NoteD)]]
#define SampleDataS(InsD,SampleD)	curMusic->sample[curMusic->fid[(InsD)].firstSample + (SampleD)]
#define SampleDataD(InsD)			curMusic->sample[curMusic->fid[(InsD)].firstSample + curSample[(InsD)]]

#ifdef __cplusplus
extern "C" {
#endif

//TODO: either migrate all these functions to PlayerPROCore or remove them from the header.
void	AllNoteOff(MADDriverRec *intDriver);
bool	MADIsPressed(unsigned char* km2, unsigned short k);
MADErr	MADCreateTiming(MADDriverRec *intDriver);
MADErr	MADCreateDriverBuffer(MADDriverRec *intDriver);
void	MADDisposeDriverBuffer(MADDriverRec *intDriver);
MADErr	MADCreateReverb(MADDriverRec *intDriver);
void	MADDisposeReverb(MADDriverRec *intDriver);
MADErr	MADCreateMicroDelay(MADDriverRec *intDriver);
MADErr	DBSndClose(MADDriverRec *inMADDriver);
void	StopChannel(MADDriverRec *inMADDriver);
void	PlayChannel(MADDriverRec *inMADDriver);
void	MADPurgeTrack(MADDriverRec *intDriver);
PPEXPORT bool DirectSave(char *myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver);
PPEXPORT bool DirectSaveAlways(char *myPtr, MADDriverSettings *driverType, MADDriverRec *intDriver);
MADErr MADCreateVibrato(MADDriverRec *MDriver);
PPEXPORT PatData* DecompressPartitionMAD1(MADMusic *MDriver, PatData* myPat);
PPEXPORT PatData* CompressPartitionMAD1(MADMusic *MDriver, PatData* myPat);
PPEXPORT void 	GenerateSound(MADDriverRec *intDriver);
PPEXPORT int	GetOldPeriod(short note, int c2spd, MADDriverRec *intDriver);
PPEXPORT MADErr	MADResetInstrument(InstrData *curIns);
PPEXPORT void	MADCheckSpeed(MADMusic *MDriver, MADDriverRec *intDriver);
PPEXPORT MADErr	AddSoundToMAD(char			*theSound,
							  int			lS,
							  int			lE,
							  short			sS,
							  short			bFreq,
							  uint32_t		rate,
							  bool			stereo,
							  MADPStr255	name,
							  short			ins,
							  short			*sampleID);

PPEXPORT MADErr	MADCopyCurrentPartition(MADMusic *theNewMAD);
PPEXPORT MADErr	MADLoadMADFileCString(MADMusic **, const char *fName);
int		DoVolPanning(short, Channel *ch, MADDriverRec *intDriver);
int		DoVolPanning256(short, Channel *ch, MADDriverRec *intDriver, bool);
void	MADKeyOFF(MADDriverRec *MDriver, short track);

PPEXPORT size_t	MADMinimize(MADMusic*);
void	MADPurgeTrackIfInstru(MADDriverRec *intDriver, short instru);
void	MADTickLoopFill8(Channel *curVoice, int *ASCBuffer1, int *ASCBuffer2, size_t size, short left, short right);
void	MADTickLoop8(size_t size, Channel *curVoice, int *ASCBuffer1, int *ASCBuffer2, MADDriverRec *intDriver);
void	MADTickRemoverStart8(Channel *curVoice, int	*ASCBuffer1, int *ASCBuffer2, MADDriverRec *intDriver);
void	MADTickRemoverLoop16(int size, Channel *curVoice, int *ASCBuffer1, int *ASCBuffer2, MADDriverRec *intDriver, int diffL, int diffR);
void	MADTickRemoverLoop8(int size, Channel *curVoice, int *ASCBuffer1, int *ASCBuffer2, MADDriverRec *intDriver, int diff);

short	MADGetNextReader(MADMusic *music, MADDriverRec *intDriver, short cur, short *pat);
PPEXPORT MADErr	MADCleanCurrentMusic(MADMusic *MDriver, MADDriverRec *intDriver);
void	CloseEffect(Channel *ch, short call, MADDriverRec *intDriver);
int		Interpolate(int p, int p1, int p2, int v1, int v2);
int		InterpolateEnv(int p, EnvRec *a,EnvRec *b);
void	ProcessFadeOut(Channel *ch, MADDriverRec *intDriver);
void	ProcessEnvelope(Channel *ch, MADDriverRec *intDriver, bool);
void	StartEnvelope(Channel *ch);
void	StartPanning(Channel *ch);
void	SetUpCmdEffect(Channel *ch, MADDriverRec *intDriver);
MADErr	MADInitEqualizer(MADDriverRec *intDriver);
void	MADCloseEqualizer(MADDriverRec *intDriver);
void	MADrealft(double *data,int n,int isign);
void	MADCallFFT(sData *SData, double *filter, MADDriverRec *intDriver, bool);
void	FFT16S(short *SData, size_t size, double *filter, MADDriverRec *intDriver, short nochan, bool);
void	FFT8S(char *SData, size_t size, double *filter, MADDriverRec *intDriver, short nochan, bool);
void	IntNoteOff(Channel *curVoice, MADDriverRec *intDriver);
bool	NewMADCommand(Cmd *theNoteCmd);
short	FindAFreeChannel(MADDriverRec *intDriver);

// Effects.c

void parse_slidevol(Channel *ch, MADByte Arg);
void ConvertTo64Rows(MADMusic *music);

// MIDI
void InitMIDIHarware(void);
void OpenMIDIHardware(MADDriverRec *rec);
void CloseMIDIHarware(void);
void SendMIDIClock(MADDriverRec *intDriver, MADByte MIDIByte);

#ifdef __cplusplus
}
#endif	

#endif
