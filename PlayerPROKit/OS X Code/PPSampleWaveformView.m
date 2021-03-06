//
//  PPSampleWaveformView.m
//  PPMacho
//
//  Created by C.W. Betts on 10/4/14.
//
//

#import <PlayerPROKit/PlayerPROKit-Swift.h>
#import "PPSampleWaveformView.h"
#import "PPSampleObject.h"

@interface PPSampleWaveformView ()
@property (strong) NSImage *waveform;
@end

@implementation PPSampleWaveformView
@synthesize sample = _sample;
- (void)setSample:(PPSampleObject *)sample
{
	self.waveform = nil;
	_sample = sample;
}

- (void)setDrawBackground:(BOOL)drawBackground
{
	_drawBackground = drawBackground;
	[self setNeedsDisplay:YES];
}

- (void)setBackgroundColor:(NSColor *)backgroundColor
{
	_backgroundColor = backgroundColor;
	self.needsDisplay = YES;
}

- (BOOL)isOpaque
{
	return !_drawBackground;
}

- (instancetype)initWithFrame:(NSRect)frame
{
	self = [super initWithFrame:frame];
	if (self) {
		_backgroundColor = [NSColor blackColor];
	}
	return self;
}

- (void)generateWaveform
{
	if (_sample == nil) {
		self.waveform = [[NSImage alloc] initWithSize:self.bounds.size];
	}
	self.waveform = [self.sample waveformImageUsingView:self];
}

- (void)regenerateWaveformIfNeeded
{
	if (_waveform == nil) {
		[self generateWaveform];
		return;
	}
	
	if (!CGSizeEqualToSize(self.bounds.size, self.waveform.size)) {
		[self generateWaveform];
	}
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
	[self regenerateWaveformIfNeeded];
    
    // Drawing code here.
}

@end
