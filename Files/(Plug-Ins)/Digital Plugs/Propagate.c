/*	Complex Fade	*/
/*	v 1.0			*/
/*	1997 by ANR		*/

//	Usage:
//	A small example of to use Digital Editor Plugs with a MODAL DIALOG

#include "MAD.h"
#include "PPPlug.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( Pcmd*)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( PPInfoPlug*)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif

Cmd* GetCmd( short row, short	track, Pcmd*	myPcmd)
{
	if( row < 0) row = 0;
	else if( row >= myPcmd->length) row = myPcmd->length -1;

	if( track < 0) track = 0;
	else if( track >= myPcmd->tracks) track = myPcmd->tracks -1;
	
	return( &(myPcmd->myCmd[ (myPcmd->length * track) + row]));
}

OSErr main( 	Pcmd					*myPcmd,
				PPInfoPlug				*thePPInfoPlug)
{
	DialogPtr			myDia;
	short				itemHit, mode, track, row;
	Str255				tStr;
	
#ifndef powerc
	long	oldA4 = SetCurrentA4(); 			//this call is necessary for strings in 68k code resources
#endif
	
	for( track = 0; track < myPcmd->tracks; track ++)
	{
		for( row = 0; row < myPcmd->length; row ++)
		{
			Cmd		*myCmd, *myCmdsrc;
			
			myCmdsrc = GetCmd( 0, 0, myPcmd);
			
			myCmd	 = GetCmd( row, track, myPcmd);
			
			*myCmd = *myCmdsrc;
		}
	}
	
	#ifndef powerc
		SetA4( oldA4);
	#endif
		
	return noErr;
}
