//
//  PPErrors.h
//  PPMacho
//
//  Created by C.W. Betts on 12/10/12.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/MADDefs.h>

__BEGIN_DECLS
extern NSString * const PPMADErrorDomain;

extern NSError *PPCreateErrorFromMADErrorType(MADErr theErr) NS_RETURNS_RETAINED;
extern NSError *PPCreateErrorFromMADErrorTypeConvertingToCocoa(MADErr theErr, BOOL convertToCocoa) NS_RETURNS_RETAINED;
extern BOOL PPErrorIsUserCancelled(NSError *theErr);

__END_DECLS
