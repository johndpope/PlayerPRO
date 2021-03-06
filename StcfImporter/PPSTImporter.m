//
//  PPSTImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 4/20/14.
//
//

#import "PPSTImporter.h"
#include <Carbon/Carbon.h>

static StringPtr GetStringFromHandle(Handle aResource, ResourceIndex aId)
{
	Size handSize = GetHandleSize(aResource);
	long curSize = 2;
	
	if (!aResource)
		return NULL;
	UInt8 *data = *(UInt8**)aResource;
	UInt16 count = *(UInt16*)data;
	count = CFSwapInt16BigToHost(count);
	
	// First 2 bytes are the count of strings that this resource has.
	if (count < aId)
		return NULL;
	// skip count
	data += 2;
	
	// looking for data.  data is in order
	while (--aId >= 0)
	{
		short toAdd = (*(UInt8*)data) + 1;
		curSize += toAdd;
		if (curSize >= handSize) {
			return NULL;
		}
		data = data + toAdd;
	}
	
	return data;
}

@implementation PPSTImporter

+ (instancetype)sharedImporter
{
	static dispatch_once_t onceToken;
	static PPSTImporter *shared;
	dispatch_once(&onceToken, ^{
		shared = [PPSTImporter new];
	});
	return shared;
}

- (OSErr)loadOldMusicListAtURL:(NSURL *)toOpen toDictionary:(NSDictionary **)outDict
{
	NSUInteger		lostMusicCount = 0;
	NSInteger		selectedMusic = -1;
	ResFileRefNum	refNum;
	Handle	aHandle, locHand;
	FSRef	theRef;
	UInt16	theNo;
	if (CFURLGetFSRef((__bridge CFURLRef)toOpen, &theRef) == NO) {
		return fnfErr;
	}
	if (FSIsFSRefValid(&theRef) == false) {
		return fnfErr;
	}
	refNum = FSOpenResFile(&theRef, fsRdPerm);
	OSErr resErr = ResError();
	if (resErr) {
		return resErr;
	}
	UseResFile(refNum);
	aHandle = Get1Resource('STR#', 128);
	if (aHandle == NULL)
	{
		CloseResFile(refNum);
		return ResError();
	}
	locHand = Get1Resource('selc', 128);
	if (locHand == NULL) {
		ReleaseResource(aHandle);
		CloseResFile(refNum);
		return ResError();
	}
	DetachResource(aHandle);
	DetachResource(locHand);
	CloseResFile(refNum);
	
	HLock(aHandle);
	theNo = *((UInt16*)(*aHandle));
	theNo = CFSwapInt16BigToHost(theNo);
	
	theNo /= 2;
	
	HLock(locHand);
	short location = **((short**)locHand);
	location = CFSwapInt16BigToHost(location);
	HUnlock(locHand);
	DisposeHandle(locHand);
	locHand = NULL;
	
	NSMutableArray *newArray = [[NSMutableArray alloc] initWithCapacity:theNo];
	
	for (int i = 0; i < theNo * 2; i += 2) {
		StringPtr aStr, aStr2;
		aStr = GetStringFromHandle(aHandle, i);
		aStr2 = GetStringFromHandle(aHandle, i + 1);
		if (!aStr || !aStr2) {
			break;
		}
		
		NSString *CFaStr = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, kCFStringEncodingMacRoman));
		NSString *CFaStr2 = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr2, kCFStringEncodingMacRoman));
		if (CFaStr == nil && CFaStr2 == nil) {
			// Perhaps the string is in another encoding. Try using the system's encoding to test this theory.
			CFStringEncoding MacCompatible = CFStringGetMostCompatibleMacStringEncoding(CFStringGetSystemEncoding());
			CFaStr = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, MacCompatible));
			CFaStr2 = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr2, MacCompatible));
			if (CFaStr == nil && CFaStr2 == nil) {
				// Maybe GetApplicationTextEncoding can get the right format?
				MacCompatible = GetApplicationTextEncoding();
				CFaStr = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, MacCompatible));
				CFaStr2 = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, aStr2, MacCompatible));
			}
		}
		
		// Final check to make sure we do have string values.
		// If we don't have a valid object pointer, the XPC service will crash.
		if (!CFaStr) {
			CFaStr = @"";
		}
		if (!CFaStr2) {
			CFaStr2 = @"";
		}
		
		NSString *together = [@[CFaStr, CFaStr2] componentsJoinedByString:@":"];
		
		NSURL *fullPath = CFBridgingRelease(CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (__bridge CFStringRef)together, kCFURLHFSPathStyle, false));
		if ([fullPath checkResourceIsReachableAndReturnError:NULL]) {
			[newArray addObject:[fullPath path]];
		} else {
			if (location != -1 && location == (i / 2)) {
				location = -1;
			} else if (location != -1 && location > (i / 2)) {
				location--;
			}
			lostMusicCount++;
		}
	}
	HUnlock(aHandle);
	DisposeHandle(aHandle);
	
	selectedMusic = (location >= [newArray count]) ? location : -1;
	
	*outDict = @{@"MusicPaths":		newArray,
				 @"SelectedMusic":	@(selectedMusic),
				 @"lostMusicCount":	@(lostMusicCount)};
	
	return noErr;
}

- (void)loadStcfAtURL:(NSURL*)theURL withReply:(void (^)(NSDictionary* bookmarkData, NSError *error))reply
{
	NSDictionary *myDict;
	OSErr myErr = [self loadOldMusicListAtURL:theURL toDictionary:&myDict];
	if (myErr != noErr) {
		NSError *nsErr = [NSError errorWithDomain:NSOSStatusErrorDomain code:myErr userInfo:nil];
		reply(nil, nsErr);
	} else {
		reply(myDict, nil);
	}
}

- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection
{
	newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPSTImporterHelper)];
	newConnection.exportedObject = self;
	[newConnection resume];
	
	return YES;
}

@end
