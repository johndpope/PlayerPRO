//
//  PPApp_AppDelegate.m
//  PPMacho
//
//  Created by C.W. Betts on 7/12/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "PPApp_AppDelegate.h"
#import "PPPreferences.h"
#import "PPMusicList.h"
#import "UserDefaultKeys.h"
#import "NSColor+PPPreferences.h"

void CocoaDebugStr( short line, Ptr file, Ptr text)
{
	NSLog(@"%s:%u error text:%s!", file, line, text);
	NSInteger alert = NSRunAlertPanel(NSLocalizedString(@"MyDebugStr_Error", @"Error"),
									  [NSString stringWithFormat:NSLocalizedString(@"MyDebugStr_MainText", @"The Main text to display"), text],
									  NSLocalizedString(@"MyDebugStr_Quit", @"Quit"), NSLocalizedString(@"MyDebugStr_Continue", @"Continue"),
									  NSLocalizedString(@"MyDebugStr_Debug", @"Debug"));
	switch (alert) {
		case NSAlertAlternateReturn:
			break;
		case NSAlertOtherReturn:
			//Debugger();
			NSCAssert(NO, @"Chose to go to debugger.");
			break;
		case NSAlertDefaultReturn:
			NSLog(@"Choosing to fail!");
		default:
			abort();
			break;
	}
}

@implementation PPApp_AppDelegate

- (void)MADDriverWithPreferences
{
	Boolean madWasReading = false;
	long fullTime = 0, curTime = 0;
	if (MADDriver) {
		madWasReading = MADIsPlayingMusic(MADDriver);
		MADStopMusic(MADDriver);
		MADStopDriver(MADDriver);
		MADDisposeDriver(MADDriver);
		if (madWasReading) {
			MADGetMusicStatus(MADDriver, &fullTime, &curTime);
		}
	}
	MADDriverSettings init;
	MADGetBestDriver(&init);
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	//TODO: Sanity Checking
	init.surround = [defaults boolForKey:PPSurroundToggle];
	init.outPutRate = [defaults integerForKey:PPSoundOutRate];
	init.outPutBits = [defaults integerForKey:PPSoundOutBits];
	init.oversampling = [defaults integerForKey:PPOversamplingAmount];
	init.Reverb = [defaults boolForKey:PPReverbToggle];
	init.ReverbSize = [defaults integerForKey:PPReverbSize];
	init.ReverbStrength = [defaults integerForKey:PPReverbStrength];
	init.MicroDelaySize = [defaults integerForKey:PPStereoDelayAmount];
	
	MADCreateDriver(&init, MADLib, &MADDriver);
	MADStartDriver(MADDriver);
	if (Music) {
		MADAttachDriverToMusic(MADDriver, Music, NULL);
	}
	if (madWasReading) {
		MADSetMusicStatus(MADDriver, 0, fullTime, curTime);
		MADPlayMusic(MADDriver);
	}
}

+ (void)initialize
{
	PPMusicList *tempList = [[[PPMusicList alloc] init] autorelease];
	
	[[NSUserDefaults standardUserDefaults] registerDefaults:[NSDictionary dictionaryWithObjectsAndKeys:
															 [NSNumber numberWithBool:YES], PPRememberMusicList,
															 [NSNumber numberWithBool:NO], PPLoadMusicAtListLoad,
															 [NSNumber numberWithInt:PPStopPlaying], PPAfterPlayingMusic,
															 [NSNumber numberWithBool:YES], PPGotoStartupAfterPlaying,
															 [NSNumber numberWithBool:YES], PPSaveModList,
															 [NSNumber numberWithBool:NO], PPLoadMusicAtMusicLoad,
															 
															 [NSNumber numberWithInt:16], PPSoundOutBits,
															 [NSNumber numberWithInt:44100], PPSoundOutRate,
															 [NSNumber numberWithInt:CoreAudioDriver], PPSoundDriver,
															 [NSNumber numberWithBool:NO], PPStereoDelayToggle,
															 [NSNumber numberWithBool:NO], PPReverbToggle,
															 [NSNumber numberWithBool:NO], PPSurroundToggle,
															 [NSNumber numberWithBool:NO], PPOversamplingToggle,
															 [NSNumber numberWithInt:30], PPStereoDelayAmount,
															 [NSNumber numberWithInt:25], PPReverbSize,
															 [NSNumber numberWithInt:30], PPReverbStrength,
															 [NSNumber numberWithInt:1], PPOversamplingAmount,
															 
															 [NSNumber numberWithBool:YES], PPDEShowInstrument,
															 [NSNumber numberWithBool:YES], PPDEShowNote,
															 [NSNumber numberWithBool:YES], PPDEShowEffect,
															 [NSNumber numberWithBool:YES], PPDEShowArgument,
															 [NSNumber numberWithBool:YES], PPDEShowVolume,
															 [NSNumber numberWithBool:YES], PPDEShowMarkers,
															 [NSNumber numberWithInt:0], PPDEMarkerOffsetPref,
															 [NSNumber numberWithInt:3], PPDEMarkerLoopPref,
															 [[NSColor yellowColor] PPencodeColor], PPDEMarkerColorPref,
															 [NSNumber numberWithBool:YES], PPDEMouseClickControlPref,
															 [NSNumber numberWithBool:NO], PPDEMouseClickShiftPref,
															 [NSNumber numberWithBool:NO], PPDEMouseClickCommandPref,
															 [NSNumber numberWithBool:NO], PPDEMouseClickOptionPref,
															 [NSNumber numberWithBool:YES], PPDELineHeightNormal,
															 [NSNumber numberWithBool:YES], PPDEMusicTraceOn,
															 [NSNumber numberWithBool:YES], PPDEPatternWrappingPartition,
															 [NSNumber numberWithBool:YES], PPDEDragAsPcmd,
															 
															 [NSNumber numberWithBool:YES], PPBEMarkersEnabled,
															 [NSNumber numberWithInt:0], PPBEMarkersOffset,
															 [NSNumber numberWithInt:3], PPBEMarkersLoop,
															 [NSNumber numberWithBool:YES], PPBEOctaveMarkers,
															 [NSNumber numberWithBool:NO], PPBENotesProjection,
															 
															 [NSNumber numberWithBool:YES], PPMAddExtension,
															 [NSNumber numberWithBool:YES], PPMMadCompression,
															 [NSNumber numberWithBool:NO], PPMNoLoadMixerFromFiles,
															 [NSNumber numberWithBool:YES], PPMOscilloscopeDrawLines,
															 
															 [NSKeyedArchiver archivedDataWithRootObject:tempList], PPMMusicList,
															 nil]];
}

- (IBAction)showMusicList:(id)sender
{
    [window makeKeyAndOrderFront:sender];
}

- (BOOL)loadMusicURL:(NSURL*)musicToLoad
{
	if (Music != NULL) {
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&Music, MADDriver);
	}

	char fileType[5];
	if(MADMusicIdentifyCFURL(MADLib, fileType, (CFURLRef)musicToLoad) != noErr)
		return NO;
	if(MADLoadMusicCFURLFile(MADLib, &Music, fileType, (CFURLRef)musicToLoad)!= noErr)
		return NO;
	
	MADAttachDriverToMusic(MADDriver, Music, NULL);
	MADPlayMusic(MADDriver);
	return YES;
}

- (IBAction)showPreferences:(id)sender
{
    if (!preferences) {
		preferences = [[PPPreferences alloc] init];
	}
	[preferences showWindow:self];
}


- (IBAction)exportInstrumentAs:(id)sender {
    
}

- (IBAction)showInstrumentsList:(id)sender {
    
}

- (IBAction)showTools:(id)sender {
    [toolsPanel makeKeyAndOrderFront:sender];
}

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	PPRegisterDebugFunc(CocoaDebugStr);

	[self willChangeValueForKey:@"musicList"];
	musicList = [[PPMusicList alloc] init];
	[musicList loadMusicListFromPreferences];
	[self didChangeValueForKey:@"musicList"];
	NSNotificationCenter *defaultCenter = [NSNotificationCenter defaultCenter];
	[defaultCenter addObserver:self selector:@selector(preferencesDidChange:) name:PPListPreferencesDidChange object:nil];
	[defaultCenter addObserver:self selector:@selector(soundPreferencesDidChange:) name:PPSoundPreferencesDidChange object:nil];
	[defaultCenter addObserver:self selector:@selector(digitalEditorPreferencesDidChange:) name:PPDigitalEditorPrefrencesDidChange object:nil];
	
	MADInitLibrary(NULL, &MADLib);
	//[tableView setDataSource:musicList];
	[self MADDriverWithPreferences];

}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	if (Music != NULL) {
		MADStopMusic(MADDriver);
		MADCleanDriver(MADDriver);
		MADDisposeMusic(&Music, MADDriver);
	}
	MADStopDriver(MADDriver);
	MADDisposeDriver(MADDriver);
	MADDisposeLibrary(MADLib);
	
	[musicList saveMusicListToPreferences];
}

- (void)preferencesDidChange:(NSNotification *)notification {
	
}

- (void)dealloc
{
	[preferences release];
	[musicList release];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	[super dealloc];
}

- (IBAction)deleteInstrument:(id)sender
{
    
}

- (IBAction)showBoxEditor:(id)sender
{
    
}

- (IBAction)showClassicEditor:(id)sender
{
    
}

- (IBAction)showDigitalEditor:(id)sender
{
    
}

- (IBAction)sortMusicList:(id)sender {
	[self willChangeValueForKey:@"musicList"];
	[musicList sortMusicList];
	[self didChangeValueForKey:@"musicList"];
}

- (IBAction)playSelectedMusic:(id)sender
{
	
}

- (IBAction)addMusic:(id)sender
{
	
}

- (IBAction)removeSelectedMusic:(id)sender {
	NSIndexSet *selMusic = [tableView selectedRowIndexes];
	NSInteger i = 0;
	NSInteger selIndexes = [selMusic count];
	NSUInteger *indexArray = malloc(sizeof(NSUInteger) * selIndexes);
	NSRange theRange;
	theRange.location = 0;
	theRange.length = NSUIntegerMax;
	[selMusic getIndexes:indexArray maxCount:selIndexes inIndexRange:&theRange];
	[self willChangeValueForKey:@"musicList"];
	for (i = selIndexes - 1; i >= 0 ; i--) {
		[musicList removeObjectInMusicListAtIndex:indexArray[i]];
	}
	[self didChangeValueForKey:@"musicList"];
	free(indexArray);
}

enum PPMusicToolbarTypes {
	PPToolbarSort = 1001,
	PPToolbarAddMusic,
	PPToolbarRemoveMusic,
	PPToolbarPlayMusic,
	PPToolbarFileInfo
};

- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem
{
	switch ([theItem tag]) {
		case PPToolbarSort:
		case PPToolbarAddMusic:
		case PPToolbarFileInfo:
			return YES;
			break;
			
		case PPToolbarPlayMusic:
			if([[tableView selectedRowIndexes] count] == 1) {
				return YES;
			} else {
				return NO;
			}
			break;
			
		case PPToolbarRemoveMusic:
			if([[tableView selectedRowIndexes] count] > 0) {
				return YES;
			} else {
				return NO;
			}
			break;

		default:
		return NO;
		break;
	}	
}

- (void)soundPreferencesDidChange:(NSNotification *)notification
{
	[self MADDriverWithPreferences];
}

- (void)digitalEditorPreferencesDidChange:(NSNotification *)notification
{
	
}

- (IBAction)fastForwardButtonPressed:(id)sender
{
    
}

- (IBAction)loopButtonPressed:(id)sender
{
    
}

- (IBAction)nextButtonPressed:(id)sender
{
    
}

- (IBAction)playButtonPressed:(id)sender
{
    
}

- (IBAction)prevButtonPressed:(id)sender
{
    
}

- (IBAction)recordButtonPressed:(id)sender
{
    
}

- (IBAction)rewindButtonPressed:(id)sender
{
    
}

- (IBAction)sliderChanged:(id)sender
{
    
}

- (IBAction)stopButtonPressed:(id)sender
{
    
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{
	NSError *err = nil;
	NSWorkspace *sharedWorkspace = [NSWorkspace sharedWorkspace];
	NSString *utiFile = [sharedWorkspace typeOfFile:filename error:&err];
	if (err) {
		NSRunAlertPanel(@"Error opening file", [NSString stringWithFormat:@"Unable to open %@: %@", [filename lastPathComponent], [err localizedFailureReason]], nil, nil, nil);
		return NO;
	}
	
	if([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.tracker"])
	{
		[self willChangeValueForKey:@"musicList"];
		[musicList addMusicURL:[NSURL fileURLWithPath:filename]];
		[self didChangeValueForKey:@"musicList"];
		return YES;
	} else if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.musiclist"])
	{
		[self willChangeValueForKey:@"musicList"];
		[musicList loadMusicListAtURL:[NSURL fileURLWithPath:filename]];
		[self didChangeValueForKey:@"musicList"];
		return YES;
	} else if ([sharedWorkspace type:utiFile conformsToType:@"net.sourceforge.playerpro.stcfmusiclist"])
	{
		[self willChangeValueForKey:@"musicList"];
		[musicList loadOldMusicListAtURL:[NSURL fileURLWithPath:filename]];
		[self didChangeValueForKey:@"musicList"];
		return YES;
	}
	return NO;
}



@end
