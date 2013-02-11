//
//  SoundOutputController.h
//  PPMacho
//
//  Created by C.W. Betts on 7/26/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "PPSoundSettingsViewController.h"

@interface SoundOutputController : NSViewController <PPSoundSettingsViewControllerDelegate> {
	PPSoundSettingsViewController *soundSettingsView;
	IBOutlet NSBox *subView;
}

@end
