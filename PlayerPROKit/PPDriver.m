//
//  PPDriver.m
//  PPMacho
//
//  Created by C.W. Betts on 11/28/12.
//
//

#import "PPDriver.h"
#import "PPLibrary.h"
#import "PPLibrary_PPKPrivate.h"
#import "PPDriver_Private.h"
#import "PPMusicObject.h"
#import "PPMusicObject_PPKPrivate.h"

@implementation PPDriver

@synthesize rec = theRec;
@synthesize currentMusic;

- (void)setCurrentMusic:(PPMusicObject *)curMusic
{
	if (curMusic != currentMusic) {
		if (currentMusic) {
			[currentMusic release];
		}
		currentMusic = [curMusic retain];
		MADAttachDriverToMusic(theRec, currentMusic._currentMusic, NULL);
	}
}

@synthesize theLibrary = thePPLib;

- (id)init
{
	NSAssert(NO, @"PPDriver cannot be inited without a library");
	return nil;
}

- (id)initWithLibrary:(PPLibrary *)theLib
{
	MADDriverSettings theSet;
	MADGetBestDriver(&theSet);
	return [self initWithLibrary:theLib settings:&theSet];
}

- (id)initWithLibrary:(PPLibrary *)theLib settings:(MADDriverSettings *)theSettings
{
	if (self = [super init]) {
		thePPLib = [theLib retain];
		if(MADCreateDriver(theSettings, theLib._madLib, &theRec) !=noErr)
		{
			[self release];
			return nil;
		}
	}
	return self;
}

- (void)dealloc
{
	[currentMusic release];
	if(theRec)
		MADDisposeDriver(theRec);
	[thePPLib release];
	
	[super dealloc];
}

- (void)loadMusicFile:(NSString *)path
{
	PPMusicObject *theMus = [[PPMusicObject alloc] initWithPath:path driver:self setAsCurrentMusic:YES];
	[theMus release];
}

- (void)loadMusicURL:(NSURL*)url
{
	PPMusicObject *theMus = [[PPMusicObject alloc] initWithURL:url driver:self setAsCurrentMusic:YES];
	[theMus release];
}

@end