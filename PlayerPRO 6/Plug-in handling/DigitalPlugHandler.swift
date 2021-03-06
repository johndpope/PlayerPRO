//
//  DigitalPlugHandler.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/19/14.
//
//

import Cocoa
import PlayerPROKit

func swiftDefaultPlugInLocations() -> [NSURL] {
	return DefaultPlugInLocations() as [NSURL]
}

class DigitalPlugHandler: NSObject, NSFastEnumeration, SequenceType, Sliceable {
	private var digitalPlugs = [PPDigitalPlugInObject]()
	
	override init() {
		let defaultPlugLocs = swiftDefaultPlugInLocations()
		var defaultManager = NSFileManager.defaultManager()
		for aPlugLoc in defaultPlugLocs {
			if let components = defaultManager.contentsOfDirectoryAtURL(aPlugLoc, includingPropertiesForKeys: [], options: nil, error: nil) as? [NSURL] {
				for component in components {
					if let ext = component.pathExtension {
						if ext.compare("plugin", options: .CaseInsensitiveSearch) != .OrderedSame {
							continue
						}
						let theBundle = NSBundle(URL: component)
						if let tempObj = PPDigitalPlugInObject(bundle: theBundle) {
							digitalPlugs.append(tempObj)
						}
					}
				}
			}
		}
		super.init()
	}
	
	func beginCallDigitalPlugIn(plugNum: Int, pcmd myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver, parentDocument doc: PPDocument, handler: PPPlugErrorBlock) {
		let tmp = digitalPlugs[plugNum];
		tmp.beginCallWithPcmd(myPcmd, driver: driver, parentDocument: doc, handler: handler)
	}
	
	func addPlugInFromBundle(theBund: NSBundle) {
		if let obj = PPDigitalPlugInObject(bundle: theBund) as PPDigitalPlugInObject? {
			digitalPlugs.append(obj)
		}
	}
	
	func addPlugInFromURL(urlpath: NSURL) {
		if let theBund = NSBundle(URL: urlpath) {
			addPlugInFromBundle(theBund)
		}
	}
	
	func addPlugInFromPath(thePath: String) {
		if let theBund = NSBundle(path: thePath) {
			addPlugInFromBundle(theBund)
		}
	}
	
	func generate() -> IndexingGenerator<[PPDigitalPlugInObject]> {
		return digitalPlugs.generate()
	}
	
	var startIndex: Int { get {
		return digitalPlugs.startIndex
		}}
	
	var endIndex: Int { get {
		return digitalPlugs.endIndex
		}}
	
	subscript (subRange: Range<Int>) -> Slice<PPDigitalPlugInObject> {
		return digitalPlugs[subRange]
	}
	
	@objc subscript (index: Int) -> PPDigitalPlugInObject {
		return digitalPlugs[index]
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (digitalPlugs as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
}
