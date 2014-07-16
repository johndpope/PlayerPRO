//
//  PPMusicList.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Cocoa

let kMusicListLocation3 = "Music Key Location 3";
let kMusicListKey3 = "Music List Key 3"
let kMusicListKVO = "musicList"
let kPlayerList = "Player List"

class PPMusicList: NSObject, NSSecureCoding, NSFastEnumeration {
	var musicList: [PPMusicListObject] = []
	var lostMusicCount:UInt = 0;
	var selectedMusic = 0;
		
	func countByEnumeratingWithState(state: UnsafePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafePointer<AnyObject?>, count len: Int) -> Int {
		return musicList.bridgeToObjectiveC().countByEnumeratingWithState(state, objects: buffer, count: len);
	}
	func encodeWithCoder(aCoder: NSCoder!) {
		var BookmarkArray: [NSURL] = [];
		for obj in musicList {
			var bookData : NSURL! = obj.musicURL;
			if (bookData) {
				BookmarkArray.append(bookData)
			}
		}
		//TODO: check for failed data initialization, and decrement changedIndex to match.
		aCoder.encodeInteger(selectedMusic, forKey: kMusicListLocation3);
		aCoder.encodeObject(BookmarkArray, forKey: kMusicListKey3)

	}
	
	func indexOfObjectSimilarToURL(theURL: NSURL) -> Int {
		return musicList.bridgeToObjectiveC().indexOfObject(theURL)
	}
	
	func clearMusicList() {
		let theIndex = NSIndexSet(indexesInRange: NSMakeRange(0, musicList.count))
		self.willChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.removeAll(keepCapacity: false)
		self.didChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
	}
	
	func sortMusicListByName() {
		musicList.sort({
			(isOrderedBefore: (var1:PPMusicListObject, var2:PPMusicListObject)) -> Bool in
			let rhsString: NSString = isOrderedBefore.var1.fileName
			let lhsString: NSString = isOrderedBefore.var2.fileName
			var result = rhsString.localizedStandardCompare(lhsString)
			return result == NSComparisonResult.OrderedAscending;
			})
	}
	
	func beginLoadingOfMusicListAtURL(toOpen: NSURL, completionHandle theHandle: (theErr: NSError!) ->Void)
	{
		var conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(`protocol`: PPSTImporterHelper.self);
		
		conn.resume()
		
		conn.remoteObjectProxy.loadStcfAtURL(toOpen, withReply: {(bookmarkData:[NSObject : AnyObject]!, error: NSError!) -> Void in
			NSOperationQueue.mainQueue().addOperationWithBlock({
				if (error) {
					theHandle(theErr: error)
				} else {
					var invalidAny: AnyObject? = bookmarkData["lostMusicCount"];
					var selectedAny: AnyObject? = bookmarkData["SelectedMusic"]
					var pathsAny: AnyObject? = bookmarkData["MusicPaths"]
					if (!invalidAny || !selectedAny || !pathsAny) {
						var lolwut = CreateErrorFromMADErrorType(MADUnknownErr)
						theHandle(theErr: lolwut)
					} else {
						
						self.lostMusicCount = invalidAny as UInt;
						self.selectedMusic = selectedAny as Int;
						for aPath in pathsAny as NSArray {
							
						}
						
						theHandle(theErr: nil)
					}
				}
				
				conn.invalidate();
				})
			})
	}
	
	func addMusicURL(theURL: NSURL) -> Bool
	{
		var obj: PPMusicListObject! = PPMusicListObject(URL: theURL);
		
		if (!obj) {
			return false;
		}
		
		if (musicList.bridgeToObjectiveC().containsObject(obj)) {
			return false;
		}
		
		var theIndex = NSIndexSet(index:musicList.count);
		self.willChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.append(obj)
		self.didChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		return true;
	}
	
	func saveMusicListToURL(URL : NSURL) -> Bool {
		var theList = NSKeyedArchiver.archivedDataWithRootObject(self);
		return theList.writeToURL(URL, atomically: true)
	}
	
	func saveApplicationMusicList() -> Bool {
		var manager = NSFileManager.defaultManager();

		var PPPPath = manager.URLForDirectory(NSSearchPathDirectory.ApplicationSupportDirectory, inDomain:NSSearchPathDomainMask.UserDomainMask, appropriateForURL:nil, create:true, error:nil).URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player");
		if (!PPPPath.checkResourceIsReachableAndReturnError(nil)) {
			//Just making sure...
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories:true, attributes:nil, error:nil);
		}
		
		return self.saveMusicListToURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory:false));
	}
	
	init() {
		
		
		super.init();
	}
	
	init(coder aDecoder: NSCoder!) {
		lostMusicCount = 0;
		var BookmarkArray : NSArray = aDecoder.decodeObjectForKey(kMusicListKey3) as NSArray;
		selectedMusic = aDecoder.decodeIntegerForKey(kMusicListLocation3);
		musicList.removeAll(keepCapacity: false);
		for bookURL : AnyObject in BookmarkArray {
			var BookURLURL = bookURL as NSURL;
			if (!BookURLURL.checkResourceIsReachableAndReturnError(nil)) {
				if (selectedMusic == -1) {
					//Do nothing
				} else if (selectedMusic == musicList.count + 1) {
					selectedMusic = -1;
				} else if (selectedMusic > musicList.count + 1) {
					selectedMusic--;
				}
				lostMusicCount++;
				continue;
			}
			var obj = PPMusicListObject(URL:BookURLURL);
			musicList.append(obj)
		}
		super.init()
	}

	class func supportsSecureCoding() -> Bool {
		return true;
	}
	
	func URLAtIndex(index :Int) -> NSURL {
		return musicList[index].musicURL;
	}
	
	func loadMusicList(newArray: [PPMusicListObject])
	{
		self.willChangeValueForKey(kMusicListKVO);
		musicList = newArray;
		self.didChangeValueForKey(kMusicListKVO);
	}
	
	func loadMusicListFromData(theData: NSData) -> Bool {
		var preList: PPMusicList! = NSKeyedUnarchiver.unarchiveObjectWithData(theData) as PPMusicList
		if (!preList) {
			return false
		}
		
		lostMusicCount = preList.lostMusicCount
		loadMusicList(preList.musicList)
		self.selectedMusic = preList.selectedMusic
		return true
	}
	
	func loadMusicListAtURL(fromURL: NSURL) -> Bool {
		var listData: NSData! = NSData(contentsOfURL: fromURL)
		if (!listData) {
			return false
		}
		return loadMusicListFromData(listData)
	}
	
	func loadApplicationMusicList() -> Bool {
		var manager = NSFileManager.defaultManager();
		var PPPPath = manager.URLForDirectory(NSSearchPathDirectory.ApplicationSupportDirectory, inDomain:NSSearchPathDomainMask.UserDomainMask, appropriateForURL:nil, create:true, error:nil).URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player");
		if (!PPPPath.checkResourceIsReachableAndReturnError(nil)) {
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories: true, attributes: nil, error: nil)
			return false;
		}
		return loadMusicListAtURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	// Key-valued Coding
	
	func addMusicListObject(object:PPMusicListObject)
	{
		if (!musicList.bridgeToObjectiveC().containsObject(object)) {
			musicList.append(object)
		}
	}
	func countOfMusicList() -> Int
	{
		return musicList.count;
	}
	
	func replaceObjectInMusicListAtIndex(index: Int, withObject object: PPMusicListObject)
	{
		musicList[index] = object;
	}
	
	func objectInMusicListAtIndex(index: Int) -> PPMusicListObject
	{
		return musicList[index];
	}
	
	func removeObjectInMusicListAtIndex(atIndex: Int)
	{
		musicList.removeAtIndex(atIndex);
	}
	
	func insertObject(object :PPMusicListObject, inMusicListAtIndex index:Int)
	{
		musicList.insert(object, atIndex: index)
	}
	
	func arrayOfObjectsInMusicListAtIndexes(theSet : NSIndexSet) -> NSArray {
		return musicList.bridgeToObjectiveC().objectsAtIndexes(theSet)
	}
	
	func removeObjectsInMusicListAtIndexes(idxSet :NSIndexSet )
	{
		if (idxSet.containsIndex(selectedMusic)) {
			self.selectedMusic = -1;
		}
		let musicArray = musicList.bridgeToObjectiveC()
		self.willChange(.Removal, valuesAtIndexes: idxSet, forKey: kMusicListKVO)
		musicList = musicList.filter({
			(individualElement: PPMusicListObject) -> Bool in
			let idx = musicArray.indexOfObject(individualElement)
			if (idxSet.containsIndex(idx)) {
				return false
			} else {
				return true
			}
			})
		self.didChange(.Removal, valuesAtIndexes: idxSet, forKey: kMusicListKVO)
	}
	
	func insertObjects(anObj: NSArray, inMusicListAtIndex idx:Int)
	{
		
	}
	
	/*
	- (NSArray*)arrayOfObjectsInMusicListAtIndexes:(NSIndexSet*)theSet
	{
	return [musicList objectsAtIndexes:theSet];
	}
	
	- (void)removeObjectsInMusicListAtIndexes:(NSIndexSet *)set
	{
	if ([set containsIndex:selectedMusic]) {
	self.selectedMusic = -1;
	}
	[self willChange:NSKeyValueChangeRemoval valuesAtIndexes:set forKey:kMusicListKVO];
	[musicList removeObjectsAtIndexes:set];
	[self didChange:NSKeyValueChangeRemoval valuesAtIndexes:set forKey:kMusicListKVO];
	}
	
	- (id)objectInMusicListAtIndex:(NSUInteger)idx
	{
	return musicList[idx];
	}
	
	- (void)insertObject:(id)anObj inMusicListAtIndex:(NSUInteger)idx
	{
	[musicList insertObject:anObj atIndex:idx];
	}
	
	- (void)removeObjectInMusicListAtIndex:(NSUInteger)object
	{
	[musicList removeObjectAtIndex:object];
	}
	
	- (void)replaceObjectInMusicListAtIndex:(NSUInteger)index withObject:(id)anObject
	{
	musicList[index] = anObject;
	}*/

}