//
//  PPSampleWaveformView.h
//  PPMacho
//
//  Created by C.W. Betts on 10/4/14.
//
//

#import <Cocoa/Cocoa.h>

@class PPSampleObject;

@interface PPSampleWaveformView : NSView
@property (nonatomic, strong) PPSampleObject *sample;
@property (nonatomic) BOOL drawBackground;

/*!
 *	Default color is black
 */
@property (nonatomic, strong) NSColor *backgroundColor;

@end
