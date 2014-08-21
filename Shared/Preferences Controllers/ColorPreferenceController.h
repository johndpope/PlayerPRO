//
//  ColorPreferenceController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PPPreferences.h"

@interface ColorPreferenceController : NSViewController <PPPreferenceObject>
@property (weak) IBOutlet NSCollectionView *colorWells;
@property NSMutableArray *colors;

- (IBAction)changeColorWell:(id)sender;

@end
