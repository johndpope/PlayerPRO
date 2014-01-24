#include <Carbon/Carbon.h>
#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"

Boolean DragManagerUse;

void DragManagerInstalled(void)
{
	/* 
	 //Since Mac OS X always has the Drag manager installed,
	 // we're just going to return true.
	long  gestaltResponse;
	
	if ((Gestalt(gestaltDragMgrAttr, &gestaltResponse) != noErr) ||
	   (!(gestaltResponse & (1 << gestaltDragMgrPresent))))
	{
		DragManagerUse = false;
	}
	else */DragManagerUse = true;
}

void ReceiveNewFSSpec(AEDesc *dropLocation, FSSpec	*newFSSpec)
{
	OSErr			result;
	AEDesc			dropSpec;
	FSSpec			theSpec;
	CInfoPBRec		thePB;
	
	//
	//	Coerce the dropLocation descriptor to an FSSpec. If there's no dropLocation or
	//	it can't be coerced into an FSSpec, then it couldn't have been the Trash.
	//
	
	if ((dropLocation->descriptorType != typeNull) &&
		(AECoerceDesc(dropLocation, typeFSS, &dropSpec) == noErr)) {
		
		result = MyAEGetDescData(&dropSpec, NULL, &theSpec, sizeof(theSpec), NULL);
		if (result != noErr) return;
		
		//	HLock(dropSpec.dataHandle);
		//	theSpec = (FSSpec *) *dropSpec.dataHandle;
		
		//
		//	Get the directory ID of the given dropLocation object.
		//
		
		thePB.dirInfo.ioCompletion = NULL;
		thePB.dirInfo.ioNamePtr = (StringPtr) &theSpec.name;
		thePB.dirInfo.ioVRefNum = theSpec.vRefNum;
		thePB.dirInfo.ioFDirIndex = 0;
		thePB.dirInfo.ioDrDirID = theSpec.parID;
		
		result = PBGetCatInfoSync(&thePB);
		
		//	HUnlock(dropSpec.dataHandle);
		AEDisposeDesc(&dropSpec);
		
		if (result != noErr) return;
		
		//
		//	If the result is not a directory, it must not be the Trash.
		//
		
		if (!(thePB.dirInfo.ioFlAttrib & (1 << 4))) return;
		
		//
		//	Get information about the Trash folder.
		//
		
		newFSSpec->parID = thePB.dirInfo.ioDrDirID;
		newFSSpec->vRefNum = thePB.dirInfo.ioVRefNum;
	}
}

Boolean DropLocationIsFinderTrash(AEDesc *dropLocation)
{
	OSErr			result;
	AEDesc			dropSpec;
	FSSpec			theSpec;
	CInfoPBRec		thePB;
	short			trashVRefNum;
	long			trashDirID;
	
	//
	//	Coerce the dropLocation descriptor to an FSSpec. If there's no dropLocation or
	//	it can't be coerced into an FSSpec, then it couldn't have been the Trash.
	//
	
	if ((dropLocation->descriptorType != typeNull) &&
		(AECoerceDesc(dropLocation, typeFSS, &dropSpec) == noErr))
	{
		
		result = MyAEGetDescData(&dropSpec, NULL, &theSpec, sizeof(theSpec), NULL);
		if (result != noErr) return false;
		
		//	HLock(dropSpec.dataHandle);
		//	theSpec = (FSSpec *) *dropSpec.dataHandle;
		
		//
		//	Get the directory ID of the given dropLocation object.
		//
		
		thePB.dirInfo.ioCompletion = NULL;
		thePB.dirInfo.ioNamePtr = (StringPtr) &theSpec.name;
		thePB.dirInfo.ioVRefNum = theSpec.vRefNum;
		thePB.dirInfo.ioFDirIndex = 0;
		thePB.dirInfo.ioDrDirID = theSpec.parID;
		
		result = PBGetCatInfoSync(&thePB);
		
		//	HUnlock(dropSpec.dataHandle);
		AEDisposeDesc(&dropSpec);
		
		if (result != noErr)
			return(false);
		
		//
		//	If the result is not a directory, it must not be the Trash.
		//
		
		if (!(thePB.dirInfo.ioFlAttrib & (1 << 4)))
			return(false);
		
		//
		//	Get information about the Trash folder.
		//
		
		FindFolder(theSpec.vRefNum, kTrashFolderType, kCreateFolder, &trashVRefNum, &trashDirID);
		
		//
		//	If the directory ID of the dropLocation object is the same as the directory ID
		//	returned by FindFolder, then the drop must have occurred into the Trash.
		//
		
		if (thePB.dirInfo.ioDrDirID == trashDirID) return(true);
	}
	
	return(false);
}