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

//Needed to quiet a few warnings on Windows.
#define _CRT_SECURE_NO_WARNINGS 1

#ifdef __APPLE__
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#include "FileUtils.h"
#endif
#include "S3M.h"

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
#include "embeddedPlugs.h"
#endif

#ifdef WIN32
#define strlcpy(dst, src, size) strncpy_s(dst, size, src, _TRUNCATE)
#endif

#define LOW(para) ((para) & 15)
#define HI(para) ((para) >> 4)

static Byte LastAEffect[ MAXTRACK], LastJEffect[ MAXTRACK];

static void ConvertS3MEffect( Byte B0, Byte B1, Byte *Cmd, Byte *Arg, short channel)
{
	Byte		LoB1 = LOW( B1);
	Byte		HiB1 = HI( B1);
	
	switch( B0 + 0x40)
	{
			// Speed
		case 'A':	*Cmd = speedE;		*Arg = B1;	break;
			// Tempo
		case 'T':	*Cmd = speedE;		*Arg = B1;	break;
			
		case 'B':	*Cmd = fastskipE;		*Arg = B1;	break;
			
		case 'C':	*Cmd = skipE;			*Arg = B1;	break;
			
		case 'D':
			if (LoB1 == 0 || HiB1 == 0)		// Slide volume
			{
				*Cmd = slidevolE;		*Arg = B1;
				
				/*	if (*Arg == 0)				// Use last command
				 {
				 *Arg = LastAEffect[ channel];
				 }
				 else LastAEffect[ channel] = *Arg;*/
				
			}
			else if (HiB1 == 0x0F)		// Fine Slide volume DOWN
			{
				*Cmd = extendedE;
				*Arg = 11 << 4;
				*Arg += LoB1;
			}
			else if (LoB1 == 0x0F)		// Fine Slide volume UP
			{
				*Cmd = extendedE;
				*Arg = 10 << 4;
				*Arg += HiB1;
			}
			break;
			
		case 'E':
			if (HiB1 == 0x0F)		// FineSlide DOWN
			{
				*Cmd = extendedE;
				*Arg = 2 << 4;		//not supported
				*Arg += LoB1;
			}
			else if (HiB1 == 0x0E)	// ExtraFineSlide DOWN
			{
				*Cmd = 0;			*Arg = 0;		//not supported
			}
			else					// Slide DOWN
			{
				*Cmd = upslideE;		*Arg = B1;
			}
			break;
			
		case 'F':
			if (HiB1 == 0x0F)		// FineSlide UP
			{
				*Cmd = extendedE;
				*Arg = 1 << 4;		//not supported
				*Arg += LoB1;
			}
			else if (HiB1 == 0x0E)	// ExtraFineSlide UP
			{
				*Cmd = 0;			*Arg = 0;		//not supported
			}
			else					// Slide UP
			{
				*Cmd = downslideE;		*Arg = B1;
			}
			break;
			
		case 'G':	*Cmd = portamentoE;	*Arg = B1;	break;
		case 'H':	*Cmd = vibratoE;		*Arg = B1;	break;
			
		case 'J':
			*Cmd = arpeggioE;
			*Arg = B1;
			
			if (*Arg == 0)				// Use last command
			{
				*Arg = LastJEffect[ channel];
			}
			else LastJEffect[ channel] = *Arg;
			break;
		case 'K':	*Cmd = vibratoslideE;	*Arg = B1;	break;
		case 'L':	*Cmd = portaslideE;		*Arg = B1;	break;
		case 'O':	*Cmd = offsetE;		*Arg = B1;	break;
			
		case 'S':		// Special Effects
			switch( HiB1)
		{
			case 2:	*Cmd = extendedE;	*Arg = 5 << 4;		*Arg += LoB1;		break;	// FineTune
			case 3:	*Cmd = extendedE;	*Arg = 4 << 4;		*Arg += LoB1;		break;	// Set Vibrato WaveForm
			case 4:	*Cmd = extendedE;	*Arg = 7 << 4;		*Arg += LoB1;		break;	// Set Tremolo WaveForm
			case 8:	*Cmd = extendedE;	*Arg = 8 << 4;		*Arg += LoB1;		break;	// Set Panning
			case 0xB:	*Cmd = extendedE;	*Arg = 6 << 4;		*Arg += LoB1;		break;	// Loop pattern
			case 0xC:	*Cmd = extendedE;	*Arg = 12 << 4;	*Arg += LoB1;		break;	// Cut sample
			case 0xD:	*Cmd = extendedE;	*Arg = 13 << 4;	*Arg += LoB1;		break;	// Delay sample
			case 0xE:	*Cmd = extendedE;	*Arg = 14 << 4;	*Arg += LoB1;		break;	// Delay pattern
			default:	*Cmd = 0;		*Arg = 0;							break;
				
		}
			break;
			
		case 'X':
			if (B1 <= 128)
			{
				*Cmd = panningE;
				
				if (B1 == 128) *Arg = 255;
				else *Arg = B1<<1;
			}
			break;
		default:	*Cmd = 0;			*Arg = 0;		break;
			
	}
}

static void ConvertMADEffect( Byte Cmd, Byte Arg, Byte *B0, Byte *B1)
{
	*B0 = 0;
	*B1 = 0;
	
	switch( Cmd)
	{
		case speedE:
			if (Arg < 32)
			{
				*B0 = 'A' - 0x40;	*B1 = Arg;
			}
			else
			{
				*B0 = 'T' - 0x40;	*B1 = Arg;
			}
			break;
		case fastskipE:		*B0 = 'B' - 0x40;	*B1 = Arg;	break;
		case skipE:			*B0 = 'C' - 0x40;	*B1 = Arg;	break;
		case slidevolE:		*B0 = 'D' - 0x40;	*B1 = Arg;	break;
		case upslideE:		*B0 = 'E' - 0x40;	*B1 = Arg;	break;
		case downslideE:	*B0 = 'F' - 0x40;	*B1 = Arg;	break;
		case portamentoE:	*B0 = 'G' - 0x40;	*B1 = Arg;	break;
		case vibratoE:		*B0 = 'H' - 0x40;	*B1 = Arg;	break;
			
		case arpeggioE:		*B0 = 'J' - 0x40;	*B1 = Arg;	break;
		case vibratoslideE:	*B0 = 'K' - 0x40;	*B1 = Arg;	break;
		case portaslideE:	*B0 = 'L' - 0x40;	*B1 = Arg;	break;
		case offsetE:		*B0 = 'O' - 0x40;	*B1 = Arg;	break;
			
		case extendedE:
			switch( (Arg & 0xF0) >> 4)
		{
			case 1:	*B0 = 'F' - 0x40;	*B1 = 0xf0 + (Arg & 0x0F);		break;
			case 2:	*B0 = 'E' - 0x40;	*B1 = 0xf0 + (Arg & 0x0F);		break;
				
			case 5:	*B0 = 'S' - 0x40;	*B1 = 0x20 + (Arg & 0x0F);		break;
				
			case 4:	*B0 = 'S' - 0x40;	*B1 = 0x30 + (Arg & 0x0F);		break;
			case 7:	*B0 = 'S' - 0x40;	*B1 = 0x40 + (Arg & 0x0F);		break;
			case 6:	*B0 = 'S' - 0x40;	*B1 = 0xb0 + (Arg & 0x0F);		break;
			case 12:	*B0 = 'S' - 0x40;	*B1 = 0xc0 + (Arg & 0x0F);		break;
			case 13:	*B0 = 'S' - 0x40;	*B1 = 0xd0 + (Arg & 0x0F);		break;
			case 14:	*B0 = 'S' - 0x40;	*B1 = 0xe0 + (Arg & 0x0F);		break;
				
			case 11:	*B0 = 'D' - 0x40;	*B1 = 0xf0 + (Arg & 0x0F);		break;
			case 10:	*B0 = 'D' - 0x40;	*B1 = 0x0f + ((Arg & 0x0F)<<4);	break;
		}
			break;
	}
}

static Ptr	ConvertMad2S3M( MADMusic *theMAD, MADDriverSettings *init, size_t *sndSize)
{
	SInt32 				i, x, z;
	Ptr					finalS3M, finalS3MCopy, maxfinalS3M;
	SInt32				InstruSize;
	SInt32				NoIns;
	s3minsform		*ins[ 64];
	Cmd					*aCmd;
	SInt32				PatternSize;
	
	s3mform			*s3minfo;
	
	unsigned short		*parapins;
	unsigned short		*parappat;
	
	/********************************/
	/* 			MAD INFORMATIONS 			  */
	/********************************/
	InstruSize = 0;
	PatternSize = 0;
	NoIns = 0;
	for (i = 0; i < 64 ; i++)
	{
		if (theMAD->fid[ i].numSamples > 0)
		{
			InstruSize			+= 16 + theMAD->sample[ i*MAXSAMPLE + 0]->size;
			//NoIns++;
			
			NoIns = i+1;
		}
	}
	
	for (i = 0; i < theMAD->header->numPat;i++)
	{
		PatternSize += 6L * theMAD->header->numChn * theMAD->partition[ i]->header.size;
		PatternSize += theMAD->partition[ i]->header.size;		// byte 0 de fin de ligne
		PatternSize += 16;
	}
	/********************************/
	
	*sndSize = 				sizeof( s3mform) +
	theMAD->header->numPointers +
	NoIns*2 +
	theMAD->header->numPat*2 +
	sizeof(s3minsform) * NoIns +
	InstruSize +
	PatternSize;
	
	finalS3MCopy = finalS3M = (Ptr)malloc( *sndSize);
	maxfinalS3M = finalS3M + *sndSize;
	if (finalS3M == NULL) return NULL;
	
	
	
	
	/********************************/
	/* s3minfo -- s3minfo -- s3minfo -- s3minfo -- */
	/********************************/
	{
		s3minfo = (s3mform*) finalS3MCopy;
		
		strlcpy( s3minfo->name, theMAD->header->name, 28);
		s3minfo->sig1 			= 26;
		s3minfo->type			= 16;
		s3minfo->sig2[ 0] = s3minfo->sig2[ 1] = 0;
		
		s3minfo->ordernum		=	theMAD->header->numPointers;		PPLE16(  &s3minfo->ordernum);
		s3minfo->insnum		= 	NoIns;													PPLE16(  &s3minfo->insnum);
		s3minfo->patnum		=	theMAD->header->numPat;						PPLE16(  &s3minfo->patnum);
		
		s3minfo->flags			=	0;
		s3minfo->cwtv			=	0x2013;
		s3minfo->ffv			=	2;																PPLE16(  &s3minfo->ffv);
		
		
		strncpy( s3minfo->s3msig, "SCRM", 4);
		
		s3minfo->mastervol		= 	64;
		s3minfo->initialspeed	=	theMAD->header->speed;
		s3minfo->initialtempo	=	theMAD->header->tempo;
		s3minfo->mastermul		=	0x30;
		
		for (i = 0; i < 12; i++) s3minfo->sig3[ i] = 0;
		
		for (i = 0; i < 32; i++) s3minfo->chanset[ i] = 255;
		
		if (theMAD->header->numChn < 32) for (i = 0; i < theMAD->header->numChn; i++) s3minfo->chanset[ i] = i;
		else for (i = 0; i < 32; i++) s3minfo->chanset[ i] = i;
		
		finalS3MCopy += 96;
	}
	
	/********************************/
	/* 	orders -- orders -- orders -- orders	  */
	/********************************/
	{
		unsigned char	*orders;
		
		orders = (unsigned char *) finalS3MCopy;
		for (i = 0; i < theMAD->header->numPointers; i++)
		{
			orders[ i] = theMAD->header->oPointers[ i];
			
			//PPLE16(  &orders[ i]);
		}
		finalS3MCopy += theMAD->header->numPointers;
	}
	
	/********************************/
	/* 	Ins Num -- Ins Num -- Ins Num -- Ins Num	  */
	/********************************/
	{
		parapins = (unsigned short *) finalS3MCopy;
		
		for (i = 0; i < NoIns; i++) parapins[ i] = 0;
		finalS3MCopy += NoIns*2L;
	}
	
	/********************************/
	/* 	Pat Num -- Pat Num -- Pat Num -- Pat Num */
	/********************************/
	{
		parappat = (unsigned short *) finalS3MCopy;
		
		for (i = 0; i < theMAD->header->numPat; i++) parappat[ i] = 0;
		finalS3MCopy += theMAD->header->numPat*2L;
	}
	
	/*********************************/
	/* 	Ins Form -- Ins Form -- Ins Form -- Ins Form */
	/*********************************/
	for (i = 0; i < NoIns; i++)
	{
		if (theMAD->fid[ i].numSamples > 0)
		{
			sData		*curData = theMAD->sample[ i*MAXSAMPLE + 0];
			
			parapins[ i] =  (16L + finalS3MCopy - finalS3M) / 16;
			finalS3MCopy = finalS3M + parapins[ i]*16L;
			PPLE16(  &parapins[ i]);
			
			/**/
			
			ins[ i] = (s3minsform*) finalS3MCopy;
			
			ins[ i]->instype				=	1;
			strlcpy( (Ptr) ins[ i]->insdosname, theMAD->fid[i].name, 12);
			ins[ i]->memsegh			=	0;
			ins[ i]->memsegl			=	0;
			ins[ i]->inslength			=	curData->size;
			ins[ i]->insloopbeg			=	curData->loopBeg;
			ins[ i]->insloopend			=	curData->loopBeg + curData->loopSize;
			
			if (curData->amp == 16)
			{
				ins[ i]->inslength /= 2;
				ins[ i]->insloopbeg /= 2;
				ins[ i]->insloopend /= 2;
			}
			
			if (curData->stereo)
			{
				ins[ i]->inslength /= 2;
				ins[ i]->insloopbeg /= 2;
				ins[ i]->insloopend /= 2;
			}
			
			PPLE32( &ins[ i]->inslength);
			PPLE32( &ins[ i]->insloopbeg);
			PPLE32( &ins[ i]->insloopend);
			
			ins[ i]->insvol				=	curData->vol;
			ins[ i]->insdsk				=	0;
			ins[ i]->inspack			=	0;
			
			ins[ i]->insflags			= 	0;
			if (curData->loopSize != 0) 	ins[ i]->insflags += 1;
			if (curData->amp == 16) 		ins[ i]->insflags += 4;
			
			ins[ i]->c2spd				=	curData->c2spd;						PPLE32(  &ins[ i]->c2spd);
			for (x = 0; x < 4; x++)		ins[ i]->inssig2[ x] = 0;
			ins[ i]->insgvspos			=	256;								PPLE16(  &ins[ i]->insgvspos);
			ins[ i]->insint512			=	0;									PPLE16(  &ins[ i]->insint512);
			ins[ i]->insintlastused		=	0;
			strlcpy( (Ptr) ins[ i]->insname, theMAD->fid[i].name, 28);
			ins[ i]->inssig[ 0]				=	'S';
			ins[ i]->inssig[ 1]				=	'C';
			ins[ i]->inssig[ 2]				=	'R';
			ins[ i]->inssig[ 3]				=	'S';
		}
		else
		{
			parapins[ i] =  (16L + finalS3MCopy - finalS3M) / 16;
			finalS3MCopy = finalS3M + parapins[ i]*16L;
			PPLE16(  &parapins[ i]);
			
			/**/
			
			ins[ i] = (s3minsform*) finalS3MCopy;
			
			ins[ i]->instype				=	0;
			strlcpy( (Ptr) ins[ i]->insdosname, theMAD->fid[i].name, sizeof(ins[ i]->insdosname));
			ins[ i]->memsegh			=	0;
			ins[ i]->memsegl			=	0;
			ins[ i]->inslength			=	0;
			ins[ i]->insloopbeg			=	0;
			ins[ i]->insloopend			=	0;
			ins[ i]->insvol				=	0;
			ins[ i]->insdsk				=	0;
			ins[ i]->inspack				=	0;
			
			ins[ i]->insflags			= 	0;
			
			ins[ i]->c2spd				=	NOFINETUNE;						PPLE32(  &ins[ i]->c2spd);
			for (x = 0; x < 4; x++)		ins[ i]->inssig2[ x] = 0;
			ins[ i]->insgvspos			=	256;
			ins[ i]->insint512			=	0;
			ins[ i]->insintlastused		=	0;
			strlcpy( (Ptr) ins[ i]->insname, theMAD->fid[i].name, 28);
		}
		/**/
		
		finalS3MCopy += sizeof(s3minsform);
	}
	
	/*********************************/
	/* 	Sample Data -- Sample Data -- Sample Data   */
	/*********************************/
	
	for (i = 0; i < NoIns; i++)
	{
		if (theMAD->fid[ i].numSamples > 0)
		{
			sData			*curData = theMAD->sample[ i*MAXSAMPLE + 0];
			SInt32			tempL, dstSize;
			
			tempL = (SInt32)((16L + finalS3MCopy - finalS3M) / 16L);
			
			ins[ i]->memsegl = tempL & 0x0000FFFF;
			PPLE16(  &ins[ i]->memsegl);
			
			ins[ i]->memsegh = (SInt32) (tempL &0x00FF0000)>>16L;
			
			finalS3MCopy = finalS3M + tempL*16L;
			
			//if (finalS3MCopy + curData->size >= maxfinalS3M) Debugger();
			
			memcpy( finalS3MCopy, curData->data, curData->size);
			
			dstSize = curData->size;
			
			if (curData->stereo == true)
			{
				if (curData->amp == 8)
				{
					for (x = 0 ; x < dstSize; x+=2)
					{
						finalS3MCopy[ x / 2] = ((SInt32) finalS3MCopy[ x] + (SInt32) finalS3MCopy[ x + 1]) / 2L;
					}
				}
				else
				{
					short *short16out = (short*) finalS3MCopy, *short16in = (short*) finalS3MCopy;
					
					for (x = 0 ; x < dstSize/2; x+=2)
					{
						short16out[ x / 2] = ((SInt32) short16in[ x] + (SInt32) short16in[ x + 1]) / 2L;
					}
				}
				dstSize /= 2;
			}
			
			if (curData->amp == 8)
			{
				for (tempL = 0; tempL < dstSize; tempL++) *(finalS3MCopy + tempL) += 0x80;
			}
			else
			{
				short *b16 = (short*) finalS3MCopy;
				SInt32 temp;
				
				for (temp = 0; temp < dstSize/2; temp++)
				{
					*(b16 + temp) += 0x8000;
					PPLE16( (b16 + temp));
				}
			}
			finalS3MCopy += dstSize;
		}
	}
	
	/*********************************/
	/* 	Pattern Data -- Pattern Data -- Pattern Data */
	/*********************************/
	
	for (i = 0; i < theMAD->header->numPat; i++)
	{
		SInt32		Row = 0, maxtrackp;
		short		*sizePtr;
		Cmd			nullCmd;
		
		nullCmd.ins		= 0;
		nullCmd.note	= 0xFF;
		nullCmd.cmd		= 0xD;
		nullCmd.arg		= 0;
		nullCmd.vol		= 0xFF;
		nullCmd.unused	= 0;
		
		parappat[ i] = (16 + (size_t)finalS3MCopy - (size_t)finalS3M) / 16;
		finalS3MCopy = finalS3M + ((SInt32) parappat[ i])*16;
		PPLE16(  &parappat[ i]);
		
		sizePtr = (short*) finalS3MCopy;
		
		finalS3MCopy++;
		finalS3MCopy++;		// A short is 2 bytes...
		
		while( Row < 64)
		{
			maxtrackp = theMAD->header->numChn;
			if (maxtrackp > 16) maxtrackp = 16;
			
			for (z = 0; z < maxtrackp; z++)
			{
				Byte		*tempChar = (Byte*) finalS3MCopy;
				
				if (Row < theMAD->partition[ i]->header.size)
				{
					aCmd = GetMADCommand( Row, z, theMAD->partition[ i]);
				}
				else aCmd = &nullCmd;
				
				if(aCmd->note			!= 0xFF ||
				   aCmd->ins			!= 0 ||
				   aCmd->cmd			!= 0 ||
				   aCmd->arg			!= 0 ||
				   aCmd->vol			!= 0xFF)
				{
					//*tempChar = 0;
					finalS3MCopy++;
					
					// Channel
					
					*tempChar	= z;
					
					// PERIOD
					
					if (aCmd->note != 0xFF || aCmd->ins != 0)
					{
						short Octave, Note;
						*tempChar += 32;
						
						if (aCmd->note == 0xFF)
						{
							finalS3MCopy[ 0] = 0xFF;
						}
						else if (aCmd->note == 0xFE)
						{
							finalS3MCopy[ 0] = 254;
						}
						else
						{
							short curNote;
							
							if (aCmd->ins != 0)
							{
								if (theMAD->fid[ aCmd->ins-1].numSamples > 0)
									curNote = aCmd->note + theMAD->sample[ (aCmd->ins-1)*MAXSAMPLE + 0]->relNote;
								else 	curNote = aCmd->note;
							}
							else curNote = aCmd->note;
							
							Octave = curNote / 12;
							Note = curNote - Octave*12;
							
							finalS3MCopy[ 0] = Octave << 4;
							finalS3MCopy[ 0] += Note;
						}
						
						finalS3MCopy[ 1] = aCmd->ins;
						finalS3MCopy += 2L;
					}
					
					// VOLUME
					
					if (aCmd->vol != 0xFF || aCmd->cmd == volumeE)
					{
						*tempChar += 64;
						
						if (aCmd->cmd == volumeE) finalS3MCopy[ 0] = aCmd->arg;
						else finalS3MCopy[ 0] = aCmd->vol - 0x10;
						
						finalS3MCopy += 1L;
					}
					
					// PARAMETER
					
					if (aCmd->cmd != 0 || aCmd->arg != 0)
					{
						*tempChar += 128;
						
						ConvertMADEffect( aCmd->cmd, aCmd->arg, (Byte*) &finalS3MCopy[ 0], (Byte*) &finalS3MCopy[ 1]);
						
						finalS3MCopy += 2L;
					}
				}
			}
			
			*finalS3MCopy = 0;	// end of row
			finalS3MCopy++;
			Row++;
		}
		
		*sizePtr = finalS3MCopy - (Ptr) sizePtr;
		*sizePtr -= 2;
		PPLE16( sizePtr);
	}
	
	finalS3M = realloc(finalS3M, finalS3MCopy - finalS3M);
	*sndSize = finalS3MCopy - finalS3M;
	
	return( (Ptr) finalS3M);
}

static OSErr ConvertS3M2Mad( Ptr	theS3M, size_t size, MADMusic *theMAD, MADDriverSettings *init)
{
	SInt32 					i, x, z, channel, Row;
	SInt32 					starting;
	Ptr						MaxPtr;
	Ptr						theInstrument[ MAXINSTRU];
	Byte					tempChar, *theS3MCopy;
	short					Note, Octave, maxTrack;
	//short					S3Mperiod[ 12] = {1712,1616,1524,1440,1356,1280,1208,1140,1076,1016, 960, 907};
	Byte					S3Mpan[ 32];
	
	/**** Variables pour le MAD ****/
	Cmd				*aCmd;
	
	/**** Variables pour le S3M ****/
	
	s3mform			s3minfo;
	/********************************/
	
	for (i = 0 ; i < MAXINSTRU; i ++)
	{
		theInstrument[ i] = NULL;
	}
	
	/**** Header principal *****/
	theS3MCopy = (Byte*) theS3M;
	
	//if (sizeof( s3minfo) < 96) Debugger();		// Just a test..
	
	memcpy( &s3minfo, theS3MCopy, 96);
	theS3MCopy += 96;
	
	PPLE16(  &s3minfo.ordernum);
	PPLE16(  &s3minfo.insnum);
	PPLE16(  &s3minfo.patnum);
	PPLE16(  &s3minfo.flags);
	PPLE16(  &s3minfo.cwtv);
	PPLE16(  &s3minfo.ffv);
	
	/**** Order Num *****/
	s3minfo.orders = (unsigned char *) malloc( s3minfo.ordernum);
	if (s3minfo.orders == NULL) {
		
		return MADNeedMemory;
	}
	memcpy( s3minfo.orders, theS3MCopy, s3minfo.ordernum);
	theS3MCopy += s3minfo.ordernum;
	
	/**** Ins Num *****/
	s3minfo.parapins = (unsigned short *) malloc( s3minfo.insnum * 2L);
	if (s3minfo.parapins == NULL)
	{
		free(s3minfo.orders);
		return MADNeedMemory;
	}
	memcpy( s3minfo.parapins, theS3MCopy, s3minfo.insnum * 2L);
	theS3MCopy += s3minfo.insnum * 2L;
	for (i = 0; i < s3minfo.insnum; i++)
	{
		PPLE16(  &s3minfo.parapins[ i]);
	}
	
	
	/**** Pat Num *****/
	s3minfo.parappat = (unsigned short *) malloc( s3minfo.patnum * 2L);
	if (s3minfo.parappat == NULL) {
		free(s3minfo.orders);
		free(s3minfo.parapins);
		return MADNeedMemory;
	}
	memcpy( s3minfo.parappat, theS3MCopy, s3minfo.patnum * 2L);
	theS3MCopy += s3minfo.patnum * 2L;
	for (i = 0; i < s3minfo.patnum; i++)
	{
		PPLE16(  &s3minfo.parappat[ i]);
	}
	
	// Panning information
	if (s3minfo.pantable == 252)
	{
		memcpy( &S3Mpan, theS3MCopy, 32 * sizeof( Byte));
	}
	
	/**** Ins Data ****/
	if (s3minfo.insnum > MAXINSTRU) s3minfo.insnum = MAXINSTRU;
	s3minfo.insdata = (s3minsform *) malloc( sizeof(s3minsform) * s3minfo.insnum);
	if (s3minfo.insdata == NULL)
	{
		free(s3minfo.orders);
		free(s3minfo.parapins);
		free(s3minfo.parappat);
		return MADNeedMemory;
	}
	for (i = 0; i < s3minfo.insnum; i++)
	{
		theInstrument[ i] = NULL;
		
		theS3MCopy = (Byte*) theS3M;
		theS3MCopy += s3minfo.parapins[i]*16L;
		
		memcpy( &s3minfo.insdata[i], theS3MCopy, sizeof(s3minsform));
		
		PPLE16( &s3minfo.insdata[i].memsegl);
		PPLE32( &s3minfo.insdata[i].inslength);
		
		if (s3minfo.insdata[i].insflags&1)
		{
			PPLE32( &s3minfo.insdata[i].insloopbeg);
			PPLE32( &s3minfo.insdata[i].insloopend);
		}
		else
		{
			s3minfo.insdata[i].insloopbeg		= 0;
			s3minfo.insdata[i].insloopend		= 0;
		}
		
		PPLE32(  &s3minfo.insdata[i].c2spd);
		PPLE16(  &s3minfo.insdata[i].insgvspos);
		PPLE16(  &s3minfo.insdata[i].insint512);
		PPLE32(  &s3minfo.insdata[i].insintlastused);
		
		if (s3minfo.insdata[i].instype == 1 && s3minfo.insdata[i].inspack == 0 &&
			s3minfo.insdata[i].inssig[ 0] == 'S' &&
			s3minfo.insdata[i].inssig[ 1] == 'C' &&
			s3minfo.insdata[i].inssig[ 2] == 'R' &&
			s3minfo.insdata[i].inssig[ 3] == 'S')
	    {
	    	SInt32 tempL;
			
	    	theS3MCopy = (Byte*) theS3M;
	    	
			tempL = (((SInt32)s3minfo.insdata[i].memsegh)<<16|s3minfo.insdata[i].memsegl)<<4;
	    	
	    	theS3MCopy += tempL;
	    	
	    	theInstrument[ i] = (Ptr) theS3MCopy;
	    }
	    else
	    {
	    	theInstrument[ i] = NULL;
	    }
	}
	
	/******** Le S3M a été lu et analysé ***********/
	/******** Copie des informations dans le MAD ***/
	
	theMAD->header = (MADSpec*) calloc( sizeof( MADSpec), 1);
	if (theMAD->header == NULL) {
		free(s3minfo.orders);
		free(s3minfo.parapins);
		free(s3minfo.parappat);
		free(s3minfo.insdata);
		return MADNeedMemory;
	}
	
	theMAD->header->MAD = 'MADK';
	for(i = 29; i < 32; i++) theMAD->header->name[i] = 0;
	for(i = 0; i < 28; i++) theMAD->header->name[i] = s3minfo.name[i];
	
	strlcpy( theMAD->header->infos, "Converted by PlayerPRO S3M Plug (\xA9\x41ntoine ROSSET <rossetantoine@bluewin.ch>)", sizeof(theMAD->header->infos));
	
	theMAD->header->numPat		= s3minfo.patnum;
	theMAD->header->numPointers	= s3minfo.ordernum;
	theMAD->header->speed		= s3minfo.initialspeed;
	theMAD->header->tempo		= s3minfo.initialtempo;
	
	for(i=0; i<128; i++) theMAD->header->oPointers[ i] = 0;
	for(i=0; i<s3minfo.ordernum; i++)
	{
		theMAD->header->oPointers[ i] = s3minfo.orders[i];
		
		if (theMAD->header->oPointers[ i] < 0 || theMAD->header->oPointers[ i] >= s3minfo.patnum)
		{
			theMAD->header->oPointers[ i] = 0;
			
			if (s3minfo.orders[i] == 255)
			{
				theMAD->header->numPointers = i;
				break;
			}
		}
	}
	
	x = 1;
	for (i = 0; i < MAXTRACK; i++)
	{
		if (x > 0)
		{
			theMAD->header->chanPan[ i] = MAX_PANNING/4;
			x = -1;
		}
		else
		{
			theMAD->header->chanPan[ i] = MAX_PANNING - MAX_PANNING/4;
			x = 1;
		}
		
		theMAD->header->chanVol[ i] = MAX_VOLUME;
	}
	
	
	if (s3minfo.pantable == 252)
	{
		for (i = 0; i < 32; i++)
		{
			if ((S3Mpan[ i] & 0x0F) != 0) theMAD->header->chanPan[ i] = (MAX_PANNING * (S3Mpan[ i] & 0x0F)) / 16;
		}
	}
	
	theMAD->header->generalVol		= 64;
	theMAD->header->generalSpeed	= 80;
	theMAD->header->generalPitch	= 80;
	
	theMAD->sets = (FXSets*) calloc( MAXTRACK * sizeof(FXSets), 1);
	if (!theMAD->sets) {
		free(s3minfo.orders);
		free(s3minfo.parapins);
		free(s3minfo.parappat);
		free(s3minfo.insdata);
		free(theMAD->header);
		return MADNeedMemory;
	}
	for (i = 0; i < MAXTRACK; i++) theMAD->header->chanBus[ i].copyId = i;
	/************************/
	/***** INSTRUMENTS  *****/
	/************************/
	
	theMAD->fid = ( InstrData*) calloc( sizeof( InstrData) * (long) MAXINSTRU, 1);
	if (!theMAD->fid) {
		free(s3minfo.orders);
		free(s3minfo.parapins);
		free(s3minfo.parappat);
		free(s3minfo.insdata);
		free(theMAD->header);
		free(theMAD->sets);
		return MADNeedMemory;
	}
	
	theMAD->sample = ( sData**) calloc( sizeof( sData*) * (long) MAXINSTRU * (long) MAXSAMPLE, 1);
	if (!theMAD->sample) {
		free(s3minfo.orders);
		free(s3minfo.parapins);
		free(s3minfo.parappat);
		free(s3minfo.insdata);
		free(theMAD->header);
		free(theMAD->sets);
		free(theMAD->fid);
		return MADNeedMemory;
	}
	
	for(i  = 0 ; i < MAXINSTRU; i++)
	{
		for (x = 0; x < MAXSAMPLE; x++) theMAD->sample[ i*MAXSAMPLE + x] = NULL;
		
		theMAD->fid[i].numSamples	= 0;
	}
	
	for(i=0; i<s3minfo.insnum; i++)
	{
		InstrData		*curIns = &theMAD->fid[ i];
		
		curIns->type	= 0;
		
		for (x = 0; x < 28; x++) theMAD->fid[i].name[x] = s3minfo.insdata[i].insname[x];
		
		if (theInstrument[ i] != NULL)
		{
			sData	*curData;
			
			curIns->numSamples = 1;
			curIns->volFade = DEFAULT_VOLFADE;
			
			curData = theMAD->sample[ i*MAXSAMPLE + 0] = (sData*) calloc( sizeof( sData), 1);
			if (curData == NULL) {
				free(s3minfo.orders);
				free(s3minfo.parapins);
				free(s3minfo.parappat);
				free(s3minfo.insdata);
				for (x = 0; x < s3minfo.insnum; x++) {
					if (theMAD->sample[x * MAXSAMPLE + 0]) {
						if (theMAD->sample[ x * MAXSAMPLE + 0]->data) {
							free(theMAD->sample[ x * MAXSAMPLE + 0]->data);
						}
						free(theMAD->sample[x * MAXSAMPLE + 0]);
					}
				}
				free(theMAD->header);
				free(theMAD->sets);
				free(theMAD->fid);
				free(theMAD->sample);
				return MADNeedMemory;
			}
			
			curData->size		= s3minfo.insdata[i].inslength;
			curData->loopBeg 	= s3minfo.insdata[i].insloopbeg;
			curData->loopSize 	= s3minfo.insdata[i].insloopend - s3minfo.insdata[i].insloopbeg;
			curData->vol		= s3minfo.insdata[i].insvol;
			curData->c2spd		= s3minfo.insdata[i].c2spd;
			curData->loopType	= 0;
			curData->amp		= 8;		if (s3minfo.insdata[i].insflags&4)	curData->amp		= 16;
			
			curData->relNote	= 0;
			//	for (x = 0; x < 28; x++) theMAD->fid[i].name[x] = s3minfo.insdata[i].insname[x];
			
			if (curData->amp == 16)
			{
				curData->size *= 2;
				curData->loopBeg *= 2;
				curData->loopSize *= 2;
			}
			
			curData->data 		= (Ptr)malloc( curData->size);
			if (curData->data == NULL) {
				free(s3minfo.orders);
				free(s3minfo.parapins);
				free(s3minfo.parappat);
				free(s3minfo.insdata);
				for (x = 0; x < s3minfo.insnum; x++) {
					if (theMAD->sample[x * MAXSAMPLE + 0]) {
						if (theMAD->sample[ x * MAXSAMPLE + 0]->data) {
							free(theMAD->sample[ x * MAXSAMPLE + 0]->data);
						}
						free(theMAD->sample[x * MAXSAMPLE + 0]);
					}
				}
				free(theMAD->header);
				free(theMAD->sets);
				free(theMAD->fid);
				free(theMAD->sample);
				return MADNeedMemory;
			}
			else
			{
				memcpy( curData->data, theInstrument [i], curData->size);
				
				switch( curData->amp)
				{
					case 16:
					{
						short *b16 = (short*) curData->data;
						SInt32 temp;
						
						for (temp = 0; temp < curData->size/2; temp++)
						{
							PPLE16( (b16 + temp));
							
							if (s3minfo.ffv != 1) *(b16 + temp) -= 0x8000;
						}
					}
						break;
						
					case 8:
						if (s3minfo.ffv != 1)
						{
							SInt32 temp;
							
							for (temp = 0; temp < curData->size; temp++) *(curData->data + temp) -= 0x80;
						}
						break;
				}
			}
		}
		else curIns->numSamples = 0;
	}
	
	for (i = 0; i < MAXINSTRU; i++) theMAD->fid[ i].firstSample = i * MAXSAMPLE;
	/********************/
	
	/*********************/
	/*           Check MaxTrack         */
	/*********************/
	
	maxTrack = 0;
	for(i=0;i<32;i++)
	{
		if(s3minfo.chanset[ i] < 32) maxTrack++;
	}
	maxTrack++;
	maxTrack /= 2;
	maxTrack *= 2;
	
#if 0
	maxTrack = 0;
	for (i = 0; i < theMAD->header->numPat ; i++)
	{
		Row = 0;
		
		if (s3minfo.parappat[ i] > 0)
		{
			theS3MCopy = (Byte*) theS3M;
			theS3MCopy += ( (long) s3minfo.parappat[i] )*16L;
			theS3MCopy++;
			
			while( Row < 64)
			{
				tempChar = *theS3MCopy;
				theS3MCopy++;
				
				if (tempChar == 0) Row++;
				else
				{	// Channel
					channel = tempChar;
					channel &= 31;
					
					if (channel > maxTrack) maxTrack = channel;
					if ((tempChar & 32) != 0) theS3MCopy += 2L;
					if ((tempChar & 64) != 0) theS3MCopy += 1L;
					if ((tempChar & 128) != 0) theS3MCopy += 2L;
				}
			}
		}
	}
	maxTrack ++;
	
	
	// ** Pair **
	maxTrack++;
	maxTrack /= 2;
	maxTrack *= 2;
#endif
	/********************/
	/***** TEMPORAIRE ******/
	/********************/
	
	theMAD->header->numChn = maxTrack;
	
	for (i = 0; i < MAXTRACK; i++) LastAEffect[ i] = 0;
	for (i = 0; i < MAXTRACK; i++) LastJEffect[ i] = 0;
	
	starting = 0;
	
	for (i = 0; i < MAXPATTERN; i++) theMAD->partition[ i] = NULL;
	for (i = 0; i < theMAD->header->numPat ; i++)
	{
		theMAD->partition[ i] = (PatData*) calloc( sizeof( PatHeader) + theMAD->header->numChn * 64L * sizeof( Cmd), 1);
		if (theMAD->partition[ i] == NULL) {
			free(s3minfo.orders);
			free(s3minfo.parapins);
			free(s3minfo.parappat);
			free(s3minfo.insdata);
			for (x = 0; x < s3minfo.insnum; x++) {
				if (theMAD->sample[x * MAXSAMPLE + 0]) {
					if (theMAD->sample[ x * MAXSAMPLE + 0]->data) {
						free(theMAD->sample[ x * MAXSAMPLE + 0]->data);
					}
					free(theMAD->sample[x * MAXSAMPLE + 0]);
				}
			}
			for (x = 0; x < MAXPATTERN; x++) {
				if (theMAD->partition[x]) {
					free(theMAD->partition[x]);
				}
			}
			free(theMAD->header);
			free(theMAD->sets);
			free(theMAD->fid);
			free(theMAD->sample);
			return MADNeedMemory;
		}
		
		theMAD->partition[ i]->header.size = 64L;
		theMAD->partition[ i]->header.compMode = 'NONE';
		
		for (x = 0; x < 20; x++) theMAD->partition[ i]->header.name[ x] = 0;
		
		theMAD->partition[ i]->header.patBytes = 0;
		theMAD->partition[ i]->header.unused2 = 0;
		
		MaxPtr = (Ptr) theMAD->partition[ i];
		MaxPtr += sizeof( PatHeader) + theMAD->header->numChn * 64L * sizeof( Cmd);
		
		for (Row = 0; Row < 64; Row++)
		{
			for(z = 0; z < theMAD->header->numChn; z++)
			{
				aCmd = GetMADCommand( Row, z, theMAD->partition[ i]);
				
				aCmd->note		= 0xFF;
				aCmd->ins			= 0;
				aCmd->cmd		= 0;
				aCmd->arg		= 0;
				aCmd->vol		= 0xFF;
			}
		}
		
		if (s3minfo.parappat[ i] > 0)
		{
			theS3MCopy = (Byte*) theS3M;
			theS3MCopy += ( (SInt32) s3minfo.parappat[i] )*16;
			theS3MCopy++;
			theS3MCopy++;
			
			Row = 0;
			while( Row < 64)
			{
				/*
				 BYTE:flag, 	0		=	end of row
				 &31		=	channel
				 &32		=	follows;  BYTE:note, BYTE:instrument
				 &64		=	follows;  BYTE:volume
				 &128	=	follows; BYTE:command, BYTE:info
				 */
				
				tempChar = *theS3MCopy;
				theS3MCopy++;
				
				if (tempChar == 0) Row++;
				else
				{
					// Channel
					
					channel = tempChar;
					channel &= 31;
					if (channel >= 0 && channel < theMAD->header->numChn) aCmd = GetMADCommand( Row, channel, theMAD->partition[ i]);
					else aCmd = NULL;
					
					// PERIOD
					
					if ((tempChar & 32) != 0)
					{
						if (aCmd != NULL)
						{
							aCmd->note = theS3MCopy[ 0];
							
							if (aCmd->note == 255)
							{
								aCmd->note = 0xFF;
							}
							else if (aCmd->note == 254) // Note-OFF
							{
								aCmd->note = 0xFE;
								//	Debugger();
							}
							else
							{
								Octave = (aCmd->note & 0xF0) >> 4;
								Note = (aCmd->note & 0x0F);
								
								aCmd->note = Octave*12 + Note;
								if (aCmd->note < 0 || aCmd->note >= NUMBER_NOTES) aCmd->note = 0xFF;
							}
							aCmd->ins = theS3MCopy[ 1];
						}
						theS3MCopy += 2L;
					}
					
					// VOLUME
					
					if ((tempChar & 64) != 0)
					{
						if (aCmd != NULL)
						{
							aCmd->vol = theS3MCopy[ 0];
							if (aCmd->vol > 64) aCmd->vol = 64;
							aCmd->vol += 0x10;
						}
						theS3MCopy += 1L;
					}
					else if (aCmd != NULL) aCmd->vol = 255;
					
					// PARAMETER
					
					if ((tempChar & 128) != 0)
					{
						if (aCmd != NULL)
						{
							if (theS3MCopy[ 0] != 255)
							{
								ConvertS3MEffect( theS3MCopy[ 0], theS3MCopy[ 1], &aCmd->cmd, &aCmd->arg, channel);
							}
						}
						theS3MCopy += 2L;
					}
				}
			}
		}
	}
	
	free( s3minfo.orders);			free( s3minfo.parapins);
	free( s3minfo.parappat);		free( s3minfo.insdata);
	
	return noErr;
}

static OSErr ExtractS3MInfo( PPInfoRec *info, Ptr AlienFile)
{
	s3mform		*myS3M = ( s3mform*) AlienFile;
	s3mform		s3minfo;
	/********************************/
	
	/**** Header principal *****/
	memcpy( &s3minfo, AlienFile, 96);
	
	/*** Signature ***/
	
	info->signature = 'S3M ';
	
	/*** Internal name ***/
	
	//myS3M->name[ 27] = '\0';
	strlcpy( info->internalFileName, myS3M->name, sizeof(myS3M->name));
	
	/*** Total Patterns ***/
	
	PPLE16(  &s3minfo.patnum);
	info->totalPatterns = s3minfo.patnum;
	
	/*** Partition Length ***/
	
	PPLE16( &s3minfo.ordernum);
	info->partitionLength = s3minfo.ordernum;
	
	/*** Total Instruments ***/
	
	PPLE16(  &s3minfo.insnum);
	info->totalInstruments = s3minfo.insnum;
	
	//TODO:
	info->totalTracks = 0;
	
	strlcpy( info->formatDescription, "S3M Plug", sizeof(info->formatDescription));
	
	return noErr;
}

static OSErr TestS3MFile( Ptr AlienFile)
{
	s3mform	*myS3M = ( s3mform*) AlienFile;
	
	if(myS3M->s3msig[ 0] == 'S' &&
	   myS3M->s3msig[ 1] == 'C' &&
	   myS3M->s3msig[ 2] == 'R' &&
	   myS3M->s3msig[ 3] == 'M') return noErr;
	else return  MADFileNotSupportedByThisPlug;
}

#ifndef _MAC_H

EXP OSErr FillPlug( PlugInfo *p);
EXP OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init);

EXP OSErr FillPlug( PlugInfo *p)
{
	strlcpy( p->type, "S3M ", sizeof(p->type));
	strlcpy( p->MenuName, "S3M Files", sizeof(p->MenuName));
	p->mode	=	MADPlugImportExport;
	p->version = 2 << 16 | 0 << 8 | 0;
	
	return noErr;
}
#endif

#if defined(NOEXPORTFUNCS) && NOEXPORTFUNCS
OSErr mainS3M( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#else
extern OSErr PPImpExpMain( OSType order, Ptr AlienFileName, MADMusic *MadFile, PPInfoRec *info, MADDriverSettings *init)
#endif
{
	OSErr		myErr;
	Ptr			AlienFile;
	size_t		sndSize;
	UNFILE		iFileRefI;
	
	myErr = noErr;
	
	switch( order)
	{
		case MADPlugExport:
			AlienFile = ConvertMad2S3M( MadFile, init, &sndSize);
			
			if (AlienFile != NULL)
			{
				iFileCreate( AlienFileName, 'S3M ');
				iFileRefI = iFileOpenWrite( AlienFileName);
				if (iFileRefI)
				{
					iWrite( sndSize, AlienFile, iFileRefI);
					iClose( iFileRefI);
				}
				free( AlienFile);	AlienFile = NULL;
			}
			else myErr = MADNeedMemory;
			break;
			
		case MADPlugImport:
			iFileRefI = iFileOpenRead( AlienFileName);
			if (iFileRefI)
			{
				sndSize = iGetEOF( iFileRefI);
				
				// ** MEMORY Test Start
				AlienFile = (Ptr)malloc( sndSize * 2L);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				// ** MEMORY Test End
				
				else
				{
					free( AlienFile);
					
					AlienFile = (Ptr)malloc( sndSize);
					if (AlienFile == NULL) myErr = MADNeedMemory;
					else
					{
						iRead( sndSize, AlienFile, iFileRefI);
						
						myErr = TestS3MFile( AlienFile);
						if (myErr == noErr)
						{
							myErr = ConvertS3M2Mad( AlienFile,  sndSize, MadFile, init);
						}
					}
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case MADPlugTest:
			iFileRefI = iFileOpenRead( AlienFileName);
			if (iFileRefI)
			{
				sndSize = 1024L;
				
				AlienFile = (Ptr)malloc( sndSize);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					
					if(myErr == noErr) myErr = TestS3MFile( AlienFile);
					
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		case 'INFO':
			iFileRefI = iFileOpenRead( AlienFileName);
			if (iFileRefI)
			{
				info->fileSize = iGetEOF( iFileRefI);
				
				sndSize = 5000L;	// Read only 5000 first bytes for optimisation
				
				AlienFile = (Ptr)malloc( sndSize);
				if (AlienFile == NULL) myErr = MADNeedMemory;
				else
				{
					myErr = iRead( sndSize, AlienFile, iFileRefI);
					if (myErr == noErr)
					{
						myErr = TestS3MFile( AlienFile);
						if (!myErr) myErr = ExtractS3MInfo( info, AlienFile);
					}
					free( AlienFile);	AlienFile = NULL;
				}
				iClose( iFileRefI);
			}
			else myErr = MADReadingErr;
			break;
			
		default:
			myErr = MADOrderNotImplemented;
			break;
	}
	
	return myErr;
}