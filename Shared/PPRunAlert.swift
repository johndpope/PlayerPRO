//
//  PPRunAlert.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/15/14.
//
//

import Foundation
import AppKit.NSAlert

private func PPRunAlertPanelBase(title: String, message msgFormat: String, defaultButton: String?, alternateButton: String?, otherButton: String?, alertStyle: NSAlertStyle, args: CVaListPointer) -> Int {
	let theAlert = NSAlert()
	theAlert.alertStyle = alertStyle
	theAlert.messageText = title
	theAlert.informativeText = NSString(format: msgFormat, arguments: args)
	if defaultButton == nil && alternateButton == nil && otherButton == nil {
		let defButt = theAlert.addButtonWithTitle("OK")
		defButt.tag = NSAlertDefaultReturn
	} else {
		if let aDefaultButton = defaultButton {
			let defButt = theAlert.addButtonWithTitle(aDefaultButton)
			defButt.tag = NSAlertDefaultReturn
		}
		if let aAlternateButton = alternateButton {
			let altButt = theAlert.addButtonWithTitle(aAlternateButton)
			altButt.tag = NSAlertAlternateReturn
		}
		if let aOtherButton = otherButton {
			let othButt = theAlert.addButtonWithTitle(aOtherButton)
			othButt.tag = NSAlertOtherReturn
		}
	}
	return theAlert.runModal()
}

internal func PPRunInformationalAlertPanel(title: String, message msgFormat: String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil, #args: CVarArgType...) -> Int {
	return PPRunAlertPanelBase(title, message: msgFormat, defaultButton, alternateButton, otherButton, .InformationalAlertStyle, getVaList(args))
}

internal func PPRunAlertPanel(title: String, message msgFormat: String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil, #args: CVarArgType...) -> Int {
	return PPRunAlertPanelBase(title, message: msgFormat, defaultButton, alternateButton, otherButton, .WarningAlertStyle, getVaList(args))
}

internal func PPRunCriticalAlertPanel(title: String, message msgFormat: String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil, #args: CVarArgType...) -> Int {
	return PPRunAlertPanelBase(title, message: msgFormat, defaultButton, alternateButton, otherButton, .CriticalAlertStyle, getVaList(args))
}
