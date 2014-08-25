//
//  PPInstrumentImporterObject.h
//  PPMacho
//
//  Created by C.W. Betts on 12/27/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#import "PPPlugInObject.h"

@interface PPInstrumentImporterObject : PPPlugInObject
@property (readonly, copy) NSArray *UTITypes;
@property (readonly) MADPlugModes mode;
@property (readonly) BOOL isSample;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)theBund NS_DESIGNATED_INITIALIZER;
- (OSErr)importInstrument:(NSURL *)fileToImport instrumentDataReference:(InstrData*)insData sampleDataReference:(sData**)sdataref instrumentSample:(short*)insSamp function:(OSType)imporexp plugInfo:(PPInfoPlug*)plugInfo;

@end