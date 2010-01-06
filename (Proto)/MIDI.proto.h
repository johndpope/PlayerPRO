
/* MIDI.c */
void ClearMODPattern(short whichPat);
Boolean ShowMIDIInfo(Str255 name, short Tracks, short *start, short *end, short *noteStart);
void MIDScanMIDIEvent(void);
void MIDScanMetaEvent(void);
void MIDScanSysExtEvent(void);
void ImportMIDI(FSSpec theMidiFile);
