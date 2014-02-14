/*
 *  PlayerPROApp.h
 *  PPMacho
 *
 *  Created by C.W. Betts on 2/13/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>
#include "RDriver.h"
extern "C" {
#include "Shuddup.h"
}
#include <string>

namespace PlayerPRO {
	class PlayerPROApp {
	public:
		KeyMap				km;
		RGBColor			theColor;
		ModalFilterUPP		MyDlgFilterDesc;
		ListClickLoopUPP	mylClickLoopDesc;
		NavEventUPP			MyDlgFilterNavDesc;
		
		Boolean		gUseControlSize, MusicPlayActive, UseAEErreur;
		short		gScrollBarID, curvRefNum;
		long		curparID, StartTime;
		Boolean		gUseNavigation, Micro, hasASC, End, Record;
		DialogPtr	ToolsDlog;
		Str63		lastLoadMODListName;
		Cursor		watchCrsr, qdarrow;
		Cursor		PlayCrsr, HandCrsr, CHandCrsr, ZoomInCrsr, ZoomOutCrsr;

		
		Boolean		mainSystemDrag, Direct, Stereo, StereoMixing, NewSoundManager, NewSoundManager31, Audio16;
		short		theDepth, RowBytes;
		Prefs		thePrefs;
		MenuHandle 	InternetMenu, AppleMenu, NoteMenu, InstruMenu, EffectMenu, TracksNumberMenu, BaseNote, InstruEditMenu, PatternEditMenu;
		MenuHandle	ExportMenu, EditorMenu, FileMenu, VSTMenu, ViewsMenu, EditMenu, WindowMenu, HelpMenu;
		EventRecord	theEvent;
		WindowPtr	oldWindow;
		
		std::string ENote(int idx);
		std::string EInstru(int idx);
		std::string EArgu(int idx);
		std::string EEffect(int idx);

		
		MADDriverRec	*MADDriver;
		MADMusic		*curMusic;
		MADLibrary		*gMADLib;
		const EventTypeSpec *CarbonWinList();
		
		//void RollCursor();
		Boolean QTTypeConversion(OSType fileType);
		Boolean CheckFileType(FSSpec theSpec, OSType theType);
		
		OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
		OSErr GetApplicationResourceFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
		
		void Run();
		
		void SetPortFromOldWindow();
		CGrafPtr GetPortFromOldWindow();
		DialogPtr GetDialogFromOldWindow();

	private:;
	};
	PlayerPROApp *TheApp;
}

