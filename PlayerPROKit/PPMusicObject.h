//
//  PPMusicObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/1/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>

#ifndef NS_DESIGNATED_INITIALIZER
#define NS_DESIGNATED_INITIALIZER
#endif

@class PPDriver;
@class PPLibrary;
@class PPInstrumentObject;

@interface PPMusicObject : NSObject <NSCopying>
@property (readonly) int totalPatterns;
@property (readonly) int totalPartitions;
@property (readonly) int partitionLength;
@property (readonly) short totalTracks;
@property (readonly) short totalInstruments;
@property (readonly, strong, nonatomic) NSArray *sDatas;
@property (readonly, strong, nonatomic) NSArray *instruments;
@property (readonly, strong, nonatomic) NSMutableArray *patterns;
@property (readonly, strong, nonatomic) NSMutableArray *buses;
@property (readwrite, copy) NSString *title;
@property (readwrite, copy) NSString *information;
@property (readonly, weak) PPDriver *attachedDriver;
@property (readonly, strong) NSURL *filePath;

@property BOOL usesLinearPitchTable;
@property BOOL limitPitchToMODTable;
@property BOOL showsCopyright;
@property int newPitch;
@property int newSpeed;
@property MADByte generalPitch;
@property MADByte generalSpeed;
@property MADByte generalVolume;

- (instancetype)init;
// Creates a music object from the supplied MADK file ONLY
- (instancetype)initWithURL:(NSURL *)url error:(out NSError* __autoreleasing*)error NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithPath:(NSString *)url error:(out NSError* __autoreleasing*)error;

// Creates a music object from any supported tracker type.
- (instancetype)initWithURL:(NSURL *)url library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error;
- (instancetype)initWithPath:(NSString *)url library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error;

// Creates a music object from the specified music type.
// If the type isn't available, it returns nil.
- (instancetype)initWithURL:(NSURL *)url type:(in const char*)type library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithPath:(NSString *)path type:(in const char*)type library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error;
- (instancetype)initWithURL:(NSURL *)url stringType:(NSString*)type library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error;
- (instancetype)initWithPath:(NSString *)path stringType:(NSString*)type library:(PPLibrary *)theLib error:(out NSError* __autoreleasing*)error;

// Creates a music object from any supported tracker type, also attaching a driver to the music.
- (instancetype)initWithURL:(NSURL *)url driver:(PPDriver *)theLib error:(out NSError* __autoreleasing*)error;
- (instancetype)initWithPath:(NSString *)path driver:(PPDriver *)theLib error:(out NSError* __autoreleasing*)error;
- (instancetype)initWithURL:(NSURL *)url type:(in const char*)type driver:(PPDriver *)theLib error:(out NSError* __autoreleasing*)error;
- (instancetype)initWithPath:(NSString *)path type:(in const char*)type driver:(PPDriver *)theLib error:(out NSError* __autoreleasing*)error;
- (instancetype)initWithURL:(NSURL *)url stringType:(NSString*)type driver:(PPDriver *)theDriv error:(out NSError* __autoreleasing*)error;
- (instancetype)initWithPath:(NSString *)path stringType:(NSString*)type driver:(PPDriver *)theDriv error:(out NSError* __autoreleasing*)error;

/*!
 *	Initializes a music object based on a music struct, copying if specified.
 */
- (instancetype)initWithMusicStruct:(MADMusic*)theStruct copy:(BOOL)copyData NS_DESIGNATED_INITIALIZER;

/*!
 *	Initializes a music object based on a music struct, copying it.
 */
- (instancetype)initWithMusicStruct:(MADMusic*)theStruct;

+ (MADErr)info:(MADInfoRec*)theInfo fromTrackerAtURL:(NSURL*)thURL usingLibrary:(PPLibrary*)theLib;

// Save music to a URL in MADK format.
- (MADErr)saveMusicToURL:(NSURL *)tosave;
- (MADErr)saveMusicToURL:(NSURL *)tosave compress:(BOOL)mad1Comp;
- (MADErr)exportMusicToURL:(NSURL *)tosave format:(NSString*)form library:(PPLibrary*)otherLib;

//This method sets the music object as the playback music
- (void)attachToDriver:(PPDriver *)theDriv;

- (void)addInstrumentObject:(PPInstrumentObject *)object;
- (void)replaceObjectInInstrumentsAtIndex:(NSInteger)index withObject:(PPInstrumentObject *)object;
@property (readonly) NSInteger countOfInstruments;
- (PPInstrumentObject*)instrumentObjectAtIndex:(NSInteger)idx;
- (void)clearInstrumentsAtIndexes:(NSIndexSet *)indexes;
- (void)clearInstrumentObjectAtIndex:(NSInteger)index;

@property (readonly) MADMusic *internalMadMusicStruct NS_RETURNS_INNER_POINTER;

- (MADErr)exportInstrumentListToURL:(NSURL*)outURL;
- (BOOL)addInstrument:(PPInstrumentObject*)theIns;
- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr;

@end
