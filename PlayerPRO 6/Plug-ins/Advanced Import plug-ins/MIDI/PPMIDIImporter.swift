//
//  PPMIDIImporter.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit

@objc(PPMIDIImporter) final public class MIDIImporter: NSObject, PPComplexImportPlugInterface {

	public func beginImportOfURL(theURL: NSURL, withHandler handler: PPComplexImportHandler) {
		let aImp = MIDIImporterController.newWithLocation(theURL, handler: handler)
		aImp.beginImportModalSession()
	}
	
	public func canImportURL(theURL: NSURL, error outErr: NSErrorPointer) -> Bool {
		var myErr = MADErr.NoErr;
		let aFile = NSFileHandle.fileHandleForReadingFromURL(theURL, error: nil)
		let fileData = aFile.readDataOfLength(4)
		let headerData = "MThd".dataUsingEncoding(NSASCIIStringEncoding, allowLossyConversion: true)
		
		if fileData == headerData {
			return true
		} else {
			if outErr != nil {
				outErr.memory = CreateErrorFromMADErrorType(.FileNotSupportedByThisPlug)
			}
			return false
		}
	}
	
	public override init() {
		
		super.init()
	}
	
}
