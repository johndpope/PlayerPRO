/*	CROP			*/
/*	v 1.0			*/
/*	1999 by ROSSET	*/

#include "MAD.h"
#include "PPPlug.h"

#if defined(powerc) || defined(__powerc)
enum {
		PlayerPROPlug = kCStackBased
		| RESULT_SIZE(SIZE_CODE( sizeof(OSErr)))
		| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof( sData*)))
		| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof( long)))
		| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof( long)))
		| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof( PPInfoPlug*)))
		| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof( long)))
};

ProcInfoType __procinfo = PlayerPROPlug;
#else
#include <A4Stuff.h>
#endif

OSErr main( 	sData					*theData,
				long					SelectionStart,
				long					SelectionEnd,
				PPInfoPlug				*thePPInfoPlug,
				long					StereoMode)				// StereoMode = 0 apply on all channels, = 1 apply on current channel
{
	long	i;

	switch( theData->amp)
	{
		case 8:
		{
			Ptr	SamplePtr = theData->data + SelectionStart;
			for( i = 0; i < SelectionEnd - SelectionStart; i++)
			{
				theData->data[ i] = SamplePtr[ i];
			}
			
			theData->size = SelectionEnd - SelectionStart;
			SetPtrSize( theData->data, theData->size);
		}
		break;

		case 16:
		{
			short	*SamplePtr = ((short*) theData->data) + (SelectionStart / 2);
			short	*data = (short*) theData->data;
			
			for( i = 0; i < (SelectionEnd - SelectionStart) / 2; i++)
			{
				data[ i] = SamplePtr[ i];
			}
			
			theData->size = SelectionEnd - SelectionStart;
			SetPtrSize( theData->data, theData->size);
		}
		break;
	}
	
	return noErr;
}
