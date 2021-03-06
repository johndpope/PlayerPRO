//
//  PPFilterPlugHandler.m
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import "PPFilterPlugHandler.h"
#import "PPFilterPlugObject.h"
#import "PPPlugInCommon.h"
#import "UserDefaultKeys.h"

@interface PPFilterPlugHandler ()
@property (strong) NSMutableArray *filterPlugs;
@end

@implementation PPFilterPlugHandler
@synthesize filterPlugs;

- (instancetype)init
{
	if (self = [super init]) {
		self.filterPlugs = [[NSMutableArray alloc] initWithCapacity:20];
		
		NSArray *plugLocs = DefaultPlugInLocations();
		for (NSURL *aPlugLoc in plugLocs) {
			CFArrayRef	somePlugs = CFBundleCreateBundlesFromDirectory(kCFAllocatorDefault, (__bridge CFURLRef)aPlugLoc, CFSTR("plugin"));
			CFIndex		PlugNums = CFArrayGetCount(somePlugs);
			for (NSInteger x = 0; x < PlugNums; x++) {
				@autoreleasepool {
					CFBundleRef tempBundleRef = (CFBundleRef)CFArrayGetValueAtIndex(somePlugs, x);
					NSBundle *tempBundle = [NSBundle bundleWithURL:CFBridgingRelease(CFBundleCopyBundleURL(tempBundleRef))];
					PPFilterPlugObject *tempObj = [[PPFilterPlugObject alloc] initWithBundle:tempBundle];
					//You can ignore the Clang static warning of incorrect decrement here.
					CFRelease(tempBundleRef);
					if (tempObj) {
						[filterPlugs addObject:tempObj];
					}
				}
			}
			CFRelease(somePlugs);
		}
	}
	return self;
}

- (NSArray*)plugInArray
{
	return [filterPlugs copy];
}

- (PPFilterPlugObject *)objectAtIndexedSubscript:(NSInteger)index;
{
	return [self plugInAtIndex:index];
}

- (PPFilterPlugObject*)plugInAtIndex:(NSUInteger)idx
{
	return filterPlugs[idx];
}

- (NSUInteger)plugInCount
{
	return [filterPlugs count];
}

- (void)addPlugInFromBundle:(NSBundle *)theBund
{
	PPFilterPlugObject *obj = [[PPFilterPlugObject alloc] initWithBundle:theBund];
	if (obj)
		[filterPlugs addObject:obj];
}

- (void)addPlugInFromURL:(NSURL *)urlpath
{
	NSBundle *theBund = [NSBundle bundleWithURL:urlpath];
	[self addPlugInFromBundle:theBund];
}

- (void)addPlugInFromPath:(NSString*)thePath
{
	NSBundle *theBund = [NSBundle bundleWithPath:thePath];
	[self addPlugInFromBundle:theBund];
}

- (void)beginWithPlugAtIndex:(NSInteger)idx data:(PPSampleObject*)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver*)driver parentDocument:(NSDocument*)document handler:(PPPlugErrorBlock)handler
{
	PPFilterPlugObject *obj = filterPlugs[idx];
	[obj beginRunWithData:theData selectionRange:selRange onlyCurrentChannel:StereoMode driver:driver parentDocument:document handler:handler];
}

- (NSUInteger)countByEnumeratingWithState:(NSFastEnumerationState *)state objects:(id __unsafe_unretained [])buffer count:(NSUInteger)len
{
	return [filterPlugs countByEnumeratingWithState:state objects:buffer count:len];
}

@end
