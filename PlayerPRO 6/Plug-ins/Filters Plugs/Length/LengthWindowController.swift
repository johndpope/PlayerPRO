//
//  LengthWindowController.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/4/14.
//
//

import Cocoa
import PlayerPROKit

class LengthWindowController: NSWindowController {
	@IBOutlet weak var currentSize: LengthViewController!
	@IBOutlet weak var newSize: LengthViewController!
	@IBOutlet weak var theCurrentBox: NSBox!
	@IBOutlet weak var theNewBox: NSBox!
	@IBOutlet weak var lengthCompensationMatrix: NSMatrix!
	
	dynamic var changeRate = false
	
	var selectionRange = NSRange(location: 0, length: 0)
	var stereoMode = false
	var currentBlock: PPPlugErrorBlock!
	var theData: PPSampleObject!

	enum LengthCompensation: Int {
	case MoveLeft
		case MoveRight
		case Stretch
	}
	
	override func windowDidLoad() {
		super.windowDidLoad()

		// Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
		self.theCurrentBox.contentView = self.currentSize.view
		self.theNewBox.contentView = self.newSize.view
		currentSize.isNewSize = false
		newSize.isNewSize = true
		
		var temp = theData.data.length
		currentSize.dataSize = temp
		newSize.dataSize = temp
		
		if (theData.stereo) {
			temp /= 2;
		}
		if (theData.amplitude == 16) {
			temp /= 2;
		}
		currentSize.samplesNumCount = temp
		newSize.samplesNumCount = temp
	}
	
	@IBAction func okay(sender: AnyObject?) {
		var newSize: Int32 = 0
		if theData.stereo {
			newSize *= 2;
		}
		if theData.amplitude == 16 {
			newSize *= 2;
		}

		if theData.loopBegin < 0 {
			theData.loopSize += theData.loopBegin
			theData.loopBegin = 0;
		}
		if theData.loopBegin > newSize {
			theData.loopBegin = 0;
			theData.loopSize = 0;
		}
		if theData.loopSize < 0 {
			theData.loopSize = 0;
		}
		if (theData.loopBegin + theData.loopSize) > newSize {
			theData.loopSize = newSize - theData.loopBegin
		}

		NSApplication.sharedApplication().endSheet(self.window!)
		currentBlock(.NoErr)
	}
	
	@IBAction func cancel(sender: AnyObject?) {
		NSApplication.sharedApplication().endSheet(self.window!)
		currentBlock(.UserCanceledErr)
	}
	
	private func convertSample(size destinationSize: Int) -> NSMutableData? {
		let LRVAL = 3
		var srcSize = theData.data.length
		var dstSize = destinationSize
		let src16 = UnsafePointer<Int16>(theData.data.bytes)
		let src8 = UnsafePointer<Int8>(src16)
		let realsrcSize = srcSize
		
		if let dst = NSMutableData(length: destinationSize) {
			var tempL = 0
			var tempR = 0
			let dst16 = UnsafeMutablePointer<Int16>(dst.mutableBytes)
			let dst8 = UnsafeMutablePointer<Int8>(dst16)
			
			srcSize /= 100
			dstSize /= 100
			
			switch (theData.amplitude) {
			case 8:
				for (var x = 0; x < destinationSize; x++) {
					var pos		= (x * srcSize << LRVAL) / dstSize
					let right	= pos & ((1 << LRVAL) - 1)
					let left	= (1 << LRVAL) - right
					
					if (theData.stereo) {
						pos >>= LRVAL
						pos /= 2
						pos *= 2
						
						if (2 + pos >= realsrcSize) {
						} else {
							tempL = (left * Int(src8[pos]) + right * Int(src8[2 + pos])) >> LRVAL
						}
						
						dst8[x] = Int8(tempL)
						
						x++
						
						if (3 + pos >= realsrcSize) {
						} else {
							tempR = (left * Int(src8[1 + pos]) + right * Int(src8[3 + pos])) >> LRVAL
						}
						
						dst8[x] = Int8(tempR)
					} else {
						pos >>= LRVAL
						
						if (1 + pos >= realsrcSize)	{
						} else {
							tempL = (left * Int(src8[pos]) + right * Int(src8[1 + pos])) >> LRVAL
						}
						
						dst8[x] = Int8(tempL)
					}
				}
				
			case 16:
				for (var x = 0; x < destinationSize / 2; x++) {
					var pos		= (x * srcSize << LRVAL) / dstSize
					let right	= pos & ((1 << LRVAL) - 1)
					let left	= (1 << LRVAL) - right
					
					if (theData.stereo) {
						pos >>= LRVAL
						pos /= 2
						pos *= 2
						
						if (2 + pos >= realsrcSize / 2) {
						} else {
							tempL = (left * Int(src16[pos]) + right * Int(src16[2 + pos])) >> LRVAL
						}
						
						dst16[x] = Int16(tempL)
						
						x++;
						
						if (3 + pos >= realsrcSize / 2) {
						} else {
							tempR = (left * Int(src16[1 + pos]) + right * Int(src16[3 + pos])) >> LRVAL
						}
						
						dst16[x] = Int16(tempR)
					} else {
						pos >>= LRVAL;
						
						if (1 + pos >= realsrcSize / 2) {
						} else {
							tempL = (left * Int(src16[pos]) + right * Int(src16[1 + pos])) >> LRVAL
						}
						
						dst16[x] = Int16(tempR)
					}
				}
				
			default:
				return nil
			}
			
			return dst
		} else {
			return nil
		}
	}
}
