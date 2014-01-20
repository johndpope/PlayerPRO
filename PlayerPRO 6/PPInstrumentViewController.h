//
//  PPInstrumentWindowController.h
//  PPMacho
//
//  Created by C.W. Betts on 12/31/12.
//
//

#import <Cocoa/Cocoa.h>
#include <PlayerPROCore/PlayerPROCore.h>
#import <PlayerPROKit/PlayerPROKit.h>

@class PPInstrumentPlugHandler;
@class InstrumentInfoController;
@class PPFilterPlugHandler;
@class PPDocument;

@interface PPInstrumentViewController : NSViewController <NSOutlineViewDataSource, NSOutlineViewDelegate>
{
	IBOutlet NSDrawer *infoDrawer;
	IBOutlet NSTextField *instrumentSize;
	IBOutlet NSTextField *instrumentLoopStart;
	IBOutlet NSTextField *instrumentLoopSize;
	IBOutlet NSTextField *instrumentVolume;
	IBOutlet NSTextField *instrumentRate;
	IBOutlet NSTextField *instrumentNote;
	IBOutlet NSTextField *instrumentBits;
	IBOutlet NSTextField *instrumentMode;

	IBOutlet NSImageView *waveFormImage;
	IBOutlet NSOutlineView *instrumentView;
	InstrumentInfoController *instrumentInfo;
	
	NSUndoManager *undoManager;
}

@property (weak) PPInstrumentPlugHandler *importer;
@property (strong) NSUndoManager *undoManager;
@property (weak) PPFilterPlugHandler *filterHandler;
@property (weak) IBOutlet PPDocument *currentDocument;

- (BOOL)importSampleFromURL:(NSURL *)sampURL;
- (BOOL)importSampleFromURL:(NSURL *)sampURL error:(out NSError *__autoreleasing*)theErr;
- (BOOL)importSampleFromURL:(NSURL *)sampURL makeUserSelectInstrument:(BOOL)selIns error:(out NSError *__autoreleasing*)theErr;
- (BOOL)importInstrumentListFromURL:(NSURL *)insURL error:(out NSError *__autoreleasing*)theErr;
- (OSErr)exportInstrumentListToURL:(NSURL*)outURL;

//Toolbar functions
- (IBAction)exportInstrument:(id)sender;
- (IBAction)importInstrument:(id)sender;
- (IBAction)deleteInstrument:(id)sender;
- (IBAction)playInstrument:(id)sender;
- (IBAction)showInstrumentInfo:(id)sender;

- (IBAction)deleteSample:(id)sender;

- (IBAction)playSample:(id)sender;

- (IBAction)toggleInfo:(id)sender;

- (void)replaceObjectInInstrumentsAtIndex:(NSUInteger)index withObject:(id)object;

@end