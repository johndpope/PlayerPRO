//
//  PPPlugInInfo.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Cocoa

func ==(lhs: PlugInInfo, rhs: PlugInInfo) -> Bool {
	if (lhs === rhs) {
		return true;
	}
	
	if (lhs.plugName != rhs.plugName) {
		return false;
	} else if (lhs.authorName != rhs.authorName) {
		return false;
	} else if (lhs.plugType != rhs.plugType) {
		return false;
	} else {
		return true;
	}
}

class PlugInInfo: NSObject, Hashable, DebugPrintable, Printable {
	let plugName: String
	let authorName: String
	let plugType: String
	let plugCopyright: String
	let plugURL: NSURL
	
	init(plugName pn: String, author aut: String = "Unknown Author", plugType pt: String = "unknown", plugURL pu: NSURL) {
		plugName = pn
		authorName = aut
		plugType = pt
		plugURL = pu
		if let tmpBundle = NSBundle(URL: pu) {
			let tmpCopy: AnyObject? = tmpBundle.infoDictionary!["NSHumanReadableCopyright"]
			if let acopy: AnyObject = tmpCopy {
				plugCopyright = acopy as NSString as String
			} else {
				plugCopyright = "No copyright info available"
			}
		} else {
			plugCopyright = "No copyright info available"
		}
		super.init()
	}
	
	override var hash: Int {
		return self.hashValue
	}
	
	override var hashValue: Int {
		return plugName.hashValue ^ plugType.hashValue ^ authorName.hashValue ^ plugCopyright.hashValue
	}
	
	override var debugDescription: String {
		return "\(plugName): \(authorName) (\(plugCopyright)), \(plugType)"
	}
	
	override var description: String {
		return "\(plugName): \(plugType)"
	}
	
	override func isEqual(object: AnyObject?) -> Bool {
		if (object == nil) {
			return false
		}
		
		if (object === self) {
			return true;
		}
		
		if let unwrapped = object as? PlugInInfo {
			return self == unwrapped
		} else {
			return false;
		}
	}
}
