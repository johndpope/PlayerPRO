//
//  PPCurrentlyPlayingIndex.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/25/14.
//
//

import Foundation

class CurrentlyPlayingIndex: NSObject, DebugPrintable, Printable {
	internal var index = -1
	internal var playbackURL: NSURL? = nil
	internal func movePlayingIndexToOtherIndex(othIdx: CurrentlyPlayingIndex) {
		othIdx.index = index
		othIdx.playbackURL = playbackURL
	}
	
	override var description: String {
		return "Index: \(index) URL: \(playbackURL!.relativePath!)"
	}
	
	override var debugDescription: String {
		return "Index: \(index) URL: \(playbackURL) URL Path: \(playbackURL!.path)"
	}
}
