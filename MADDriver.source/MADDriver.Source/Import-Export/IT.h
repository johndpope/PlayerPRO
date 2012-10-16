/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
#pragma pack(2)
#endif

typedef struct ITPatForm
{
	unsigned short		length;
	unsigned short		row;
	SInt32				no;
	char				data[];
} ITPatForm;

typedef struct ITSampForm
{
	SInt32		ID;
	char		DOSName[ 12];
	Byte		no;
	Byte		GvL;
	Byte		Flag;
	Byte		Vol;
	char		SampName[ 26];
	short		Convert;
	SInt32		length;
	SInt32		loopBegin;
	SInt32		loopEnd;
	SInt32		C5Speed;
	SInt32		SusLoopBegin;
	SInt32		SusLoopEnd;
	SInt32		samplePtr;
	Byte		ViS;
	Byte		ViD;
	Byte		ViR;
	Byte		ViT;
} ITSampForm;

typedef struct ITNode
{
	char		y;
	short		x;
} ITNode;

typedef struct ITEnv
{
	Byte		Flag;
	Byte		Num;
	Byte		LpB;
	Byte		LpE;
	Byte		SLB;
	Byte		SLE;
	ITNode	nodes[25];
} ITEnv;

typedef struct ITKeyMap
{
	Byte		note;
	Byte		samp;
} ITKeyMap;

typedef struct ITInsForm		// size = 547
{
	OSType		ID;
	char		DOSName[ 12];
	Byte		no;
	Byte		NNA;
	Byte		DCT;
	Byte		DCA;
	short	FadeOut;
	Byte		PPS;
	Byte		PPC;
	Byte		GbV;
	Byte		DfP;
	char		no2[ 2];
	short	TrkVers;
	Byte		NoS;
	Byte		no3;
	char		INSName[ 26];
	char		no4[ 6];
	ITKeyMap	keyMap[ 120];
	
	// new structure
	
	ITEnv	volEnv;
	ITEnv	panEnv;
	ITEnv	pitchEnv;
	
} ITInsForm;

typedef struct ITOldInsForm		// size = 554
{
	OSType		ID;
	char		DOSName[ 12];
	Byte		no;
	Byte		NNA;
	Byte		DCT;
	Byte		DCA;
	short		FadeOut;
	Byte		PPS;
	Byte		PPC;
	Byte		GbV;
	Byte		DfP;
	char		no2[ 2];
	short		TrkVers;
	Byte		NoS;
	Byte		no3;
	char		INSName[ 26];
	char		no4[ 6];
	ITKeyMap	keyMap[ 120];
	
	// old structure
	
	ITKeyMap	volEnv[ 100];
	
} ITOldInsForm;

typedef struct ITForm
{
	OSType				ID;
	char				name[26];
	char				no[ 2];
	
	short				orderNum;
	short				insNum;
	short				smpNum;
	short				patNum;
	short				cwtv;
	short				cmwt;
	short				flags;
	short				special;
	
	Byte				globalVol;
	Byte				mixVol;
	Byte				iSpeed;
	Byte				iTempo;
	char				panSeparation;
	char				null;
	short				MsgLgth;
	SInt32				MsgOffset;
	char				no2[ 4];
	
	char				chanPan[ 64];
	char				chanVol[ 64];
	
	unsigned char		*orders;
	SInt32				*parapins;
	SInt32				*parapsamp;
	SInt32				*parappat;
	
	ITInsForm			*insdata;
	ITSampForm			*sampdata;
} ITForm;
#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
#pragma pack()
#endif

#pragma mark Impulse Header Layout
/*


                             Impulse Header Layout

        0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
      ���������������������������������������������������������������Ŀ
0000: �'I'�'M'�'P'�'M'� Song Name, max 26 characters, includes NULL   �
      ���������������������������������������������������������������Ĵ
0010: �.......................................................�PHiligt�
      ���������������������������������������������������������������Ĵ
0020: �OrdNum �InsNum �SmpNum �PatNum � Cwt/v � Cmwt  � Flags �Special�
      ���������������������������������������������������������������Ĵ
0030: �GV �MV �IS �IT �Sep�PWD�MsgLgth�Message Offset �   Reserved    �
      ���������������������������������������������������������������Ĵ
0040: � Chnl Pan (64 bytes)...........................................�
      ���������������������������������������������������������������Ĵ

      ���������������������������������������������������������������Ĵ
0080: � Chnl Vol (64 bytes)...........................................�
      ���������������������������������������������������������������Ĵ

      ���������������������������������������������������������������Ĵ
00C0: � Orders, Length = OrdNum                                       �
      ���������������������������������������������������������������Ĵ
xxxx: � 'Long' Offset of instruments, Length = InsNum*4 (1)           �
      ���������������������������������������������������������������Ĵ
xxxx: � 'Long' Offset of samples headers, Length = SmpNum*4 (2)       �
      ���������������������������������������������������������������Ĵ
xxxx: � 'Long' Offset of patterns, Length = PatNum*4 (3)              �
      �����������������������������������������������������������������

      (1) Offset = 00C0h+OrdNum
      (2) Offset = 00C0h+OrdNum+InsNum*4
      (3) Offset = 00C0h+OrdNum+InsNum*4+SmpNum*4

        Note that if the (long) offset to a pattern = 0, then the
        pattern is assumed to be a 64 row empty pattern.

      PHiliht = Pattern row hilight information. Only relevant for pattern
                editing situations.

      Cwt:      Created with tracker.
                 Impulse Tracker y.xx = 0yxxh
      Cmwt:     Compatible with tracker with version greater than value.
                 (ie. format version)
      OrdNum:   Number of orders in song.
      InsNum:   Number of instruments in song
      SmpNum:   Number of samples in song
      PatNum:   Number of patterns in song
      Flags:    Bit 0: On = Stereo, Off = Mono
                Bit 1: Vol0MixOptimizations - If on, no mixing occurs if
                       the volume at mixing time is 0 (redundant v1.04+)
                Bit 2: On = Use instruments, Off = Use samples.
                Bit 3: On = Linear slides, Off = Amiga slides.
                Bit 4: On = Old Effects, Off = IT Effects
                        Differences:
                       - Vibrato is updated EVERY frame in IT mode, whereas
                          it is updated every non-row frame in other formats.
                          Also, it is two times deeper with Old Effects ON
                       - Command Oxx will set the sample offset to the END
                         of a sample instead of ignoring the command under
                         old effects mode.
                       - (More to come, probably)
                Bit 5: On = Link Effect G's memory with Effect E/F. Also
                            Gxx with an instrument present will cause the
                            envelopes to be retriggered. If you change a
                            sample on a row with Gxx, it'll adjust the
                            frequency of the current note according to:

                              NewFrequency = OldFrequency * NewC5 / OldC5;
                Bit 6: Use MIDI pitch controller, Pitch depth given by PWD
                Bit 7: Request embedded MIDI configuration
                       (Coded this way to permit cross-version saving)

      Special:  Bit 0: On = song message attached.
                       Song message:
                        Stored at offset given by "Message Offset" field.
                        Length = MsgLgth.
                        NewLine = 0Dh (13 dec)
                        EndOfMsg = 0

                       Note: v1.04+ of IT may have song messages of up to
                             8000 bytes included.
                Bit 1: Reserved
                Bit 2: Reserved
                Bit 3: MIDI configuration embedded
                Bit 4-15: Reserved

      GV:       Global volume. (0->128) All volumes are adjusted by this
      MV:       Mix volume (0->128) During mixing, this value controls
                the magnitude of the wave being mixed.
      IS:       Initial Speed of song.
      IT:       Initial Tempo of song
      Sep:      Panning separation between channels (0->128, 128 is max sep.)
      PWD:      Pitch wheel depth for MIDI controllers
      Chnl Vol: Volume for each channel. Ranges from 0->64
      Chnl Pan: Each byte contains a panning value for a channel. Ranges from
                 0 (absolute left) to 64 (absolute right). 32 = central pan,
                 100 = Surround sound.
                 +128 = disabled channel (notes will not be played, but note
                                          that effects in muted channels are
                                          still processed)
      Orders:   This is the order in which the patterns are played.
                 Valid values are from 0->199.
                 255 = "---", End of song marker
                 254 = "+++", Skip to next order


                  Old Impulse Instrument Format (cmwt < 200h)

        0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
      ���������������������������������������������������������������Ŀ
0000: �'I'�'M'�'P'�'I'� DOS FileName (12345678.123)                   �
      ���������������������������������������������������������������Ĵ
0010: �00h�Flg�VLS�VLE�SLS�SLE� x � x �FadeOut�NNA�DNC�TrkVers�NoS� x �
      ���������������������������������������������������������������Ĵ
0020: � Instrument Name, max 26 bytes, includes NUL...................�
      ���������������������������������������������������������������Ĵ
0030: �.......................................� x � x � x � x � x � x �
      ���������������������������������������������������������������Ĵ
0040: � Note-Sample/Keyboard Table, Length = 240 bytes................�
      ���������������������������������������������������������������Ĵ

      ���������������������������������������������������������������Ĵ
0130: � Volume envelope (200 bytes)...................................�
      ���������������������������������������������������������������Ĵ

                                      �������������������������������Ĵ
01F8:                                 � Node points (25x2 bytes)......�
                                      ���������������������������������

        Total length of old instrument header is 554 bytes.

      Flg:      Bit 0. On = Use volume envelope
                Bit 1. On = Use volume loop
                Bit 2. On = Use sustain volume loop
      VLS:      Volume loop start (node number)
      VLE:      Volume loop end (node number)
      SLS:      Sustain loop start (node number)
      SLE:      Sustain loop end (node number)
      FadeOut:  Ranges between 0 and 64, but the fadeout "Count" is 512.
                Fade applied when:
                1) Note fade NNA is selected and triggered (by another note)
                2) Note off NNA is selected with no volume envelope
                   or volume envelope loop
                3) Volume envelope end is reached

      DNC:      Duplicate note check (0 = Off, 1 = On)
      NNA:      New note action:
                        0 = Note cut
                        1 = Note continue
                        2 = Note off
                        3 = Note fade

      TrkVers:  Tracker version used to save the instrument. This is only
                used in the instrument files.
      NoS:      Number of samples associated with instrument. This is only
                used in the instrument files.

      Note-Sample/Keyboard Table.
       Each note of the instrument is first converted to a sample number
       and a note (C-0 -> B-9). These are stored as note/sample pairs
       (note first, range 0->119 for C-0 to B-9, sample ranges from
       1-99, 0=no sample)

       Volume envelope: Values from 0->64, 0FFh indicating end of envelope.
       (after which note fade applies)

      Node data: Tick THEN magnitude



                           Impulse Instrument Format

        0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
      ���������������������������������������������������������������Ŀ
0000: �'I'�'M'�'P'�'I'� DOS FileName (12345678.123)                   �
      ���������������������������������������������������������������Ĵ
0010: �00h�NNA�DCT�DCA�FadeOut�PPS�PPC�GbV�DfP�RV �RP �TrkVers�NoS� x �
      ���������������������������������������������������������������Ĵ
0020: � Instrument Name, max 26 bytes, includes NUL...................�
      ���������������������������������������������������������������Ĵ
0030: �.......................................�IFC�IFR�MCh�MPr�MIDIBnk�
      ���������������������������������������������������������������Ĵ
0040: � Note-Sample/Keyboard Table, Length = 240 bytes................�
      ���������������������������������������������������������������Ĵ

      ���������������������������������������������������������������Ĵ
0130: � Envelopes.....................................................�
      ���������������������������������������������������������������Ĵ

        IFC = Initial Filter cutoff
        IFR = Initial Filter resonance

        NNA = New Note Action
                0 = Cut                 1 = Continue
                2 = Note off            3 = Note fade

        DCT = Duplicate Check Type
                0 = Off                 1 = Note
                2 = Sample              3 = Instrument

        DCA: Duplicate Check Action
                0 = Cut
                1 = Note Off
                2 = Note fade

      FadeOut:  Ranges between 0 and 128, but the fadeout "Count" is 1024
                See the Last section on how this works.
                Fade applied when:
                1) Note fade NNA is selected and triggered (by another note)
                2) Note off NNA is selected with no volume envelope
                   or volume envelope loop
                3) Volume envelope end is reached

        PPS: Pitch-Pan separation, range -32 -> +32
        PPC: Pitch-Pan center: C-0 to B-9 represented as 0->119 inclusive

        GbV: Global Volume, 0->128
        DfP: Default Pan, 0->64, &128 => Don't use
        RV: Random volume variation (percentage)
        RP: Random panning variation (panning change - not implemented yet)

        MCh = MIDI Channel
        MPr = MIDI Program (Instrument)

      TrkVers:  Tracker version used to save the instrument. This is only
                used in the instrument files.
      NoS:      Number of samples associated with instrument. This is only
                used in the instrument files.

      Note-Sample/Keyboard Table.
       Each note of the instrument is first converted to a sample number
       and a note (C-0 -> B-9). These are stored as note/sample byte pairs
       (note first, range 0->119 for C-0 to B-9, sample ranges from
       1-99, 0=no sample)

                                Envelope layout

        Envelopes: 3 structures, first for volume (130h), second for
                   panning (182h), third for pitch (1D4h).

                   Each is structured as such:

        0   1   2   3   4   5   6.......
      ���������������������������������������������������������������Ŀ
xxxx: �Flg�Num�LpB�LpE�SLB�SLE� Node points, 25 sets, 75 bytes....� x �
      ���������������������������������������������������������������Ĵ

        Flg: Bit 0: Envelope on/off, 1 = on, 0 = off
             Bit 1: Loop on/off, 1 = on, 0 = off
             Bit 2: SusLoop on/off, 1 = on, 0 = off

        For Pitch envelope only:
             Bit 7: Use pitch envelope as filter envelope instead.

        Num = Number of node points

        LpB = Loop beginning            SLB = Sustain loop beginning
        LpE = Loop end                  SLE = Sustain loop end

        Node point = 1 byte for y-value
                        (0->64 for vol, -32->+32 for panning or pitch)
                     1 word (2 bytes) for tick number (0->9999)

        Total length of an instrument is 547 bytes, but 554 bytes are
        written, just to simplify the loading of the old format. (Hence
        there are 7 'wasted' bytes per instrument)



                             Impulse Sample Format

        0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
      ���������������������������������������������������������������Ŀ
0000: �'I'�'M'�'P'�'S'� DOS Filename (12345678.123)                   �
      ���������������������������������������������������������������Ĵ
0010: �00h�GvL�Flg�Vol� Sample Name, max 26 bytes, includes NUL.......�
      ���������������������������������������������������������������Ĵ
0020: �.......................................................�Cvt�DfP�
      ���������������������������������������������������������������Ĵ
0030: � Length        � Loop Begin    � Loop End      � C5Speed       �
      ���������������������������������������������������������������Ĵ
0040: � SusLoop Begin � SusLoop End   � SamplePointer �ViS�ViD�ViR�ViT�
      �����������������������������������������������������������������

The cache file has the following pieces of information added on:

        0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
      ���������������������������������������������������������������Ŀ
0050: � File Size     � Date  � Time  �Fmt�...........................�
      �����������������������������������������������������������������

        Fmt. 0 = unchecked. 1 = directory, 2 = it sample, 3 = st sample


      GvL:      Global volume for instrument, ranges from 0->64
      Flg:   1   Bit 0. On = sample associated with header.
               2 Bit 1. On = 16 bit, Off = 8 bit.
               4 Bit 2. On = stereo, Off = mono. Stereo samples not supported yet
               8 Bit 3. On = compressed samples.
              16 Bit 4. On = Use loop
              32  Bit 5. On = Use sustain loop
              64  Bit 6. On = Ping Pong loop, Off = Forwards loop
             128   Bit 7. On = Ping Pong Sustain loop, Off = Forwards Sustain loop
      Vol:      Default volume for instrument

      Length:   Length of sample in no. of samples NOT no. of bytes
      LoopBeg:  Start of loop (no of samples in, not bytes)
      Loop End: Sample no. AFTER end of loop
      C5Speed:  Number of bytes a second for C-5 (ranges from 0->9999999)
      SusLBeg:  Start of sustain loop
      SusLEnd:  Sample no. AFTER end of sustain loop

      SmpPoint: 'Long' Offset of sample in file.

      ViS:      Vibrato Speed, ranges from 0->64
      ViD:      Vibrato Depth, ranges from 0->64
      ViT:      Vibrato waveform type.
                        0=Sine wave
                        1=Ramp down
                        2=Square wave
                        3=Random (speed is irrelevant)
      ViR:      Vibrato Rate, rate at which vibrato is applied (0->64)

        The depth of the vibrato at any point is worked out in the following
        way:
          Every processing cycle, the following occurs:
                1) Mov AX, [SomeVariableNameRelatingToVibrato]
                2) Add AL, Rate
                3) AdC AH, 0
                4) AH contains the depth of the vibrato as a fine-linear slide.
                5) Mov [SomeVariableNameRelatingToVibrato], AX  ; For the next
                                                                ; cycle.

        For those that don't understand assembly, then the depth is
        basically the running-sum of the rate divided by 256.

        Sample vibrato uses a table 256-bytes long

   Convert - bits other than bit 0 are used internally for the loading
             of alternative formats.
        Bit 0:
         Off: Samples are unsigned   } IT 2.01 and below use unsigned samples
          On: Samples are signed     } IT 2.02 and above use signed samples
        Bit 1:
         Off: Intel lo-hi byte order for 16-bit samples    } Safe to ignore
         On: Motorola hi-lo byte order for 16-bit samples  } these values...
        Bit 2:                                             }
         Off: Samples are stored as PCM values             }
          On: Samples are stored as Delta values           }
        Bit 3:                                             }
          On: Samples are stored as byte delta values      }
              (for PTM loader)                             }
        Bit 4:                                             }
          On: Samples are stored as TX-Wave 12-bit values  }
        Bit 5:                                             }
          On: Left/Right/All Stereo prompt                 }
        Bit 6: Reserved
        Bit 7: Reserved

   DfP - Default Pan. Bits 0->6 = Pan value, Bit 7 ON to USE (opposite of inst)



                             Impulse Pattern Format

        0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
      ���������������������������������������������������������������Ŀ
0000: �Length � Rows  � x � x � x � x � Packed data................   �
      ���������������������������������������������������������������Ĵ

      Length:   Length of packed pattern, not including the 8 byte header
                Note that the pattern + the 8 byte header will ALWAYS
                be less than 64k
      Rows:     Number of rows in this pattern (Ranges from 32->200)

      Patterns are unpacked by the following pseudocode... (this may look
      horrible, but in practise, it's just as convenient as the S3M
      pattern format for playback (but not for display))

      GetNextChannelMarker:
        Read byte into channelvariable.
        if(channelvariable = 0) then end of row
        Channel = (channelvariable-1) & 63              ; Channel is 0 based.
        if(channelvariable & 128) then read byte into maskvariable
          else maskvariable = previousmaskvariable for current channel

        if(maskvariable & 1), then read note. (byte value)
                // Note ranges from 0->119 (C-0 -> B-9)
                // 255 = note off, 254 = notecut
                // Others = note fade (already programmed into IT's player
                //                     but not available in the editor)

        if(maskvariable & 2), then read instrument (byte value)
                // Instrument ranges from 1->99

        if(maskvariable & 4), then read volume/panning (byte value)
                // Volume ranges from 0->64
                // Panning ranges from 0->64, mapped onto 128->192
                // Prepare for the following also:
                //  65->74 = Fine volume up
                //  75->84 = Fine volume down
                //  85->94 = Volume slide up
                //  95->104 = Volume slide down
                //  105->114 = Pitch Slide down
                //  115->124 = Pitch Slide up
                //  193->202 = Portamento to
                //  203->212 = Vibrato

        Effects 65 is equivalent to D0F, 66 is equivalent to D1F -> 74 = D9F
        Similarly for 75-84 (DFx), 85-94 (Dx0), 95->104 (D0x).

        (Fine) Volume up/down all share the same memory (NOT shared with Dxx
        in the effect column tho).

        Pitch slide up/down affect E/F/(G)'s memory - a Pitch slide
        up/down of x is equivalent to a normal slide by x*4

        Portamento to (Gx) affects the memory for Gxx and has the equivalent
        slide given by this table:

        SlideTable      DB      1, 4, 8, 16, 32, 64, 96, 128, 255

        Vibrato uses the same 'memory' as Hxx/Uxx.

        if(maskvariable & 8), then read command (byte value) and commandvalue
                // Valid ranges from 0->31 (0=no effect, 1=A, 2=B, 3=C, etc.)

        if(maskvariable & 16), then note = lastnote for channel
        if(maskvariable & 32), then instrument = lastinstrument for channel
        if(maskvariable & 64), then volume/pan = lastvolume/pan for channel
        if(maskvariable & 128), then {
                command = lastcommand for channel and
                commandvalue = lastcommandvalue for channel
        }
        Goto GetNextChannelMarker



                                  Mathematics

Abbreviations:
 FV = Final Volume (Ranges from 0 to 128). In versions 1.04+, mixed output
      devices are reduced further to a range from 0 to 64 due to lack of
      memory.
 Vol = Volume at which note is to be played. (Ranges from 0 to 64)
 SV = Sample Volume (Ranges from 0 to 64)
 IV = Instrument Volume (Ranges from 0 to 128)
 CV = Channel Volume (Ranges from 0 to 64)
 GV = Global Volume (Ranges from 0 to 128)
 VEV = Volume Envelope Value (Ranges from 0 to 64)

In Sample mode, the following calculation is done:
 FV = Vol * SV * CV * GV / 262144               ; Note that 262144 = 2^18
                                                ; So bit shifting can be done.

In Instrument mode the following procedure is used:

 1) Update volume envelope value. Check for loops / end of envelope.
 2) If end of volume envelope (ie. position >= 200 or VEV = 0FFh), then turn
        on note fade.
 3) If notefade is on, then NoteFadeComponent (NFC) = NFC - FadeOut
        ; NFC should be initialised to 1024 when a note is played.
 4) FV = Vol * SV * IV * CV * GV * VEV * NFC / 2^41

Linear slides work like this:
 Final frequency = Original frequency * 2^(SlideValue/768)

(I used a lookup table for the multipliers here)

For command Exx, SlideValue = -4*EffectValue
For command EEx, SlideValue = -EffectValue
For command Fxx, SlideValue = 4*EffectValue
For command FEx, SlideValue = EffectValue

Note that sample vibrato always uses Linear slides.

Notes about effects (as compared to other module formats)

C               This is now in *HEX*. (Used to be in decimal in ST3)
E/F/G/H/U       You need to check whether the song uses Amiga/Linear slides.
H/U             Vibrato in Impulse Tracker is two times finer than in
                any other tracker and is updated EVERY tick.
                If "Old Effects" is *ON*, then the vibrato is played in the
                normal manner (every non-row tick and normal depth)
E/F/G           These commands ALL share the same memory.
Oxx             Offsets to samples are to the 'xx00th' SAMPLE. (ie. for
                16 bit samples, the offset is xx00h*2)
                Oxx past the sample end will be ignored, unless "Old Effects"
                is ON, in which case the Oxx will play from the end of the
                sample.
Yxy             This uses a table 4 times larger (hence 4 times slower) than
                vibrato or tremelo. If the waveform is set to random, then
                the 'speed' part of the command is interpreted as a delay.

If you read through this document and there are ANY points which you have
troubles with (and have to try out), then let me know - because someone
else will have the same questions - and I'd like to make this DOC as easy
to understand as possible.

For Panning....
 Here's the rough procedure used:

  NotePan = ChannelPan
  if InstrumentPan=On then NotePan = InstrumentPan
  NotePan = NotePan+(InstrumentNote-PPCenter)*PPSeparation/8

Pitch Envelopes
 Each value on the envelope equates to half a semitone. This is interpolated
 64 times for smooth pitch sliding. Positive values indicate a pitch variation
 UP of x semitones, negative values indicate a pitch variation down.

                                  General Info

The player in Impulse Tracker 'allocates' channels to notes whenever they
are *PLAYED*. In sample mode, the allocation is simple:
               Virtual Channel (number) = 'Host' channel (number)

In instrument mode, the following procedure is used:

    Check if channel is already playing ---Yes--> set 'background' flag on.
                |                                 'Trigger' NNA. If NNA=cut,
                No                                then use this virtual
                |                                 channel.
                |                                          |
                |<------------------ else -----------------/
                |
                v
    Search and find the first non-active virtual channel.
                |
    Non-active channel found? ----Yes----> Use this for playback.
                |
                No
                |
                v
   Search through and find the channel of lowest volume that is in the     #
   'background' (ie. no longer controlled directly)                        #
                |                                                          #
   Background channel found? ----Yes----> Use this for playback.           #
                |                                                          #
                No                                                         #
                |                                                          #
                v                                                          #
   Return error - the note is *NOT* allocated a channel, and hence is not  #
   played.                                                                 #

   This is actually quite a simple process... just that it's another of
   those 'hassles' to have to write...

   ### Note: This is by far the simplest implementation of congestion
             resolution. IT 2.03 and above have a greatly enhanced
             method which more selectively removes the most insignificant
             channel. Obviously, there is no best way to do this - I
             encourage you to experiment and find new algorithms for
             yourself.



                                Internal Tables

FineSineData       Label   Byte
        DB       0,  2,  3,  5,  6,  8,  9, 11, 12, 14, 16, 17, 19, 20, 22, 23
        DB      24, 26, 27, 29, 30, 32, 33, 34, 36, 37, 38, 39, 41, 42, 43, 44
        DB      45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59
        DB      59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 64
        DB      64, 64, 64, 64, 64, 64, 63, 63, 63, 62, 62, 62, 61, 61, 60, 60
        DB      59, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46
        DB      45, 44, 43, 42, 41, 39, 38, 37, 36, 34, 33, 32, 30, 29, 27, 26
        DB      24, 23, 22, 20, 19, 17, 16, 14, 12, 11,  9,  8,  6,  5,  3,  2
        DB       0, -2, -3, -5, -6, -8, -9,-11,-12,-14,-16,-17,-19,-20,-22,-23
        DB     -24,-26,-27,-29,-30,-32,-33,-34,-36,-37,-38,-39,-41,-42,-43,-44
        DB     -45,-46,-47,-48,-49,-50,-51,-52,-53,-54,-55,-56,-56,-57,-58,-59
        DB     -59,-60,-60,-61,-61,-62,-62,-62,-63,-63,-63,-64,-64,-64,-64,-64
        DB     -64,-64,-64,-64,-64,-64,-63,-63,-63,-62,-62,-62,-61,-61,-60,-60
        DB     -59,-59,-58,-57,-56,-56,-55,-54,-53,-52,-51,-50,-49,-48,-47,-46
        DB     -45,-44,-43,-42,-41,-39,-38,-37,-36,-34,-33,-32,-30,-29,-27,-26
        DB     -24,-23,-22,-20,-19,-17,-16,-14,-12,-11, -9, -8, -6, -5, -3, -2

FineRampDownData   Label   Byte
        DB      64, 63, 63, 62, 62, 61, 61, 60, 60, 59, 59, 58, 58, 57, 57, 56
        DB      56, 55, 55, 54, 54, 53, 53, 52, 52, 51, 51, 50, 50, 49, 49, 48
        DB      48, 47, 47, 46, 46, 45, 45, 44, 44, 43, 43, 42, 42, 41, 41, 40
        DB      40, 39, 39, 38, 38, 37, 37, 36, 36, 35, 35, 34, 34, 33, 33, 32
        DB      32, 31, 31, 30, 30, 29, 29, 28, 28, 27, 27, 26, 26, 25, 25, 24
        DB      24, 23, 23, 22, 22, 21, 21, 20, 20, 19, 19, 18, 18, 17, 17, 16
        DB      16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8
        DB       8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1,  0
        DB       0, -1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8
        DB      -8, -9, -9,-10,-10,-11,-11,-12,-12,-13,-13,-14,-14,-15,-15,-16
        DB     -16,-17,-17,-18,-18,-19,-19,-20,-20,-21,-21,-22,-22,-23,-23,-24
        DB     -24,-25,-25,-26,-26,-27,-27,-28,-28,-29,-29,-30,-30,-31,-31,-32
        DB     -32,-33,-33,-34,-34,-35,-35,-36,-36,-37,-37,-38,-38,-39,-39,-40
        DB     -40,-41,-41,-42,-42,-43,-43,-44,-44,-45,-45,-46,-46,-47,-47,-48
        DB     -48,-49,-49,-50,-50,-51,-51,-52,-52,-53,-53,-54,-54,-55,-55,-56
        DB     -56,-57,-57,-58,-58,-59,-59,-60,-60,-61,-61,-62,-62,-63,-63,-64

FineSquareWave     Label   Byte
        DB      128 Dup (64), 128 Dup (0)

EmptyPattern            Label
        DW      64, 64, 0, 0
        DB      64 Dup (0)

;�������������������������������������������������������������������������������

PitchTable              Label   DWord   ; Values are 16.16 bit
   DW      2048, 0,   2170, 0,   2299, 0,   2435, 0,   2580, 0,   2734, 0 ; C-0
   DW      2896, 0,   3069, 0,   3251, 0,   3444, 0,   3649, 0,   3866, 0 ;>B-0

   DW      4096, 0,   4340, 0,   4598, 0,   4871, 0,   5161, 0,   5468, 0 ; C-1
   DW      5793, 0,   6137, 0,   6502, 0,   6889, 0,   7298, 0,   7732, 0 ;>B-1

   DW      8192, 0,   8679, 0,   9195, 0,   9742, 0,   10321, 0,  10935, 0
   DW      11585, 0,  12274, 0,  13004, 0,  13777, 0,  14596, 0,  15464, 0

   DW      16384, 0,  17358, 0,  18390, 0,  19484, 0,  20643, 0,  21870, 0
   DW      23170, 0,  24548, 0,  26008, 0,  27554, 0,  29193, 0,  30929, 0

   DW      32768, 0,  34716, 0,  36781, 0,  38968, 0,  41285, 0,  43740, 0
   DW      46341, 0,  49097, 0,  52016, 0,  55109, 0,  58386, 0,  61858, 0

   DW      0, 1,      3897, 1,   8026, 1,   12400, 1,  17034, 1,  21944, 1
   DW      27146, 1,  32657, 1,  38496, 1,  44682, 1,  51236, 1,  58179, 1

   DW      0, 2,      7794, 2,   16051, 2,  24800, 2,  34068, 2,  43888, 2
   DW      54292, 2,  65314, 2,  11456, 3,  23828, 3,  36936, 3,  50823, 3

   DW      0, 4,      15588, 4,  32103, 4,  49600, 4,  2601, 5,   22240, 5
   DW      43048, 5,  65092, 5,  22912, 6,  47656, 6,  8336, 7,   36110, 7

   DW      0, 8,      31176, 8,  64205, 8,  33663, 9,  5201, 10,  44481, 10
   DW      20559, 11, 64648, 11, 45823, 12, 29776, 13, 16671, 14, 6684, 15

   DW      0, 16,     62352, 16, 62875, 17, 1790,  19, 10403, 20, 23425, 21
   DW      41118, 22, 63761, 23, 26111, 25, 59552, 26, 33342, 28, 13368, 30

FineLinearSlideUpTable  Label ; Values are 16.16 bit
        DW      0, 1,     59, 1,    118, 1,   178, 1,   237, 1    ; 0->4
        DW      296, 1,   356, 1,   415, 1,   475, 1,   535, 1    ; 5->9
        DW      594, 1,   654, 1,   714, 1,   773, 1,   833, 1    ; 10->14
        DW      893, 1                                            ; 15

LinearSlideUpTable      Label ; Value = 2^(Val/192), Values are 16.16 bit
        DW      0,     1, 237,   1, 475,   1, 714,   1, 953,  1  ; 0->4
        DW      1194,  1, 1435,  1, 1677,  1, 1920,  1, 2164, 1  ; 5->9
        DW      2409,  1, 2655,  1, 2902,  1, 3149,  1, 3397, 1  ; 10->14
        DW      3647,  1, 3897,  1, 4148,  1, 4400,  1, 4653, 1  ; 15->19
        DW      4907,  1, 5157,  1, 5417,  1, 5674,  1, 5932, 1  ; 20->24
        DW      6190,  1, 6449,  1, 6710,  1, 6971,  1, 7233, 1  ; 25->29
        DW      7496,  1, 7761,  1, 8026,  1, 8292,  1, 8559, 1  ; 30->34
        DW      8027,  1, 9096,  1, 9366,  1, 9636,  1, 9908, 1  ; 35->39
        DW      10181, 1, 10455, 1, 10730, 1, 11006, 1, 11283,1  ; 40->44
        DW      11560, 1, 11839, 1, 12119, 1, 12400, 1, 12682,1  ; 45->49
        DW      12965, 1, 13249, 1, 13533, 1, 13819, 1, 14106,1  ; 50->54
        DW      14394, 1, 14684, 1, 14974, 1, 15265, 1, 15557,1  ; 55->59
        DW      15850, 1, 16145, 1, 16440, 1, 16737, 1, 17034,1  ; 60->64
        DW      17333, 1, 17633, 1, 17933, 1, 18235, 1, 18538,1  ; 65->69
        DW      18842, 1, 19147, 1, 19454, 1, 19761, 1, 20070,1  ; 70->74
        DW      20379, 1, 20690, 1, 21002, 1, 21315, 1, 21629,1  ; 75->79
        DW      21944, 1, 22260, 1, 22578, 1, 22897, 1, 23216,1  ; 80->84
        DW      23537, 1, 23860, 1, 24183, 1, 24507, 1, 24833,1  ; 85->89
        DW      25160, 1, 25488, 1, 25817, 1, 26148, 1, 26479,1  ; 90->94
        DW      26812, 1, 27146, 1, 27481, 1, 27818, 1, 28155,1  ; 95->99
        DW      28494, 1, 28834, 1, 29175, 1, 29518, 1, 29862,1  ; 100->104
        DW      30207, 1, 30553, 1, 30900, 1, 31248, 1, 31599,1  ; 105->109
        DW      31951, 1, 32303, 1, 32657, 1, 33012, 1, 33369,1  ; 110->114
        DW      33726, 1, 34085, 1, 34446, 1, 34807, 1, 35170,1  ; 115->119
        DW      35534, 1, 35900, 1, 36267, 1, 36635, 1, 37004,1  ; 120->124
        DW      37375, 1, 37747, 1, 38121, 1, 38496, 1, 38872,1  ; 125->129
        DW      39250, 1, 39629, 1, 40009, 1, 40391, 1, 40774,1  ; 130->134
        DW      41158, 1, 41544, 1, 41932, 1, 42320, 1, 42710,1  ; 135->139
        DW      43102, 1, 43495, 1, 43889, 1, 44285, 1, 44682,1  ; 140->144
        DW      45081, 1, 45481, 1, 45882, 1, 46285, 1, 46690,1  ; 145->149
        DW      47095, 1, 47503, 1, 47917, 1, 48322, 1, 48734,1  ; 150->154
        DW      49147, 1, 49562, 1, 49978, 1, 50396, 1, 50815,1  ; 155->159
        DW      51236, 1, 51658, 1, 52082, 1, 52507, 1, 52934,1  ; 160->164
        DW      53363, 1, 53793, 1, 54224, 1, 54658, 1, 55092,1  ; 165->169
        DW      55529, 1, 55966, 1, 56406, 1, 56847, 1, 57289,1  ; 170->174
        DW      57734, 1, 58179, 1, 58627, 1, 59076, 1, 59527,1  ; 175->179
        DW      59979, 1, 60433, 1, 60889, 1, 61346, 1, 61805,1  ; 180->184
        DW      62265, 1, 62727, 1, 63191, 1, 63657, 1, 64124,1  ; 185->189
        DW      64593, 1, 65064, 1, 0,     2, 474,   2, 950,  2  ; 190->194
        DW      1427,  2, 1906,  2, 2387,  2, 2870,  2, 3355, 2  ; 195->199
        DW      3841,  2, 4327,  2, 4818,  2, 5310,  2, 5803, 2  ; 200->204
        DW      6298,  2, 6795,  2, 7294,  2, 7794,  2, 8296, 2  ; 205->209
        DW      8800,  2, 9306,  2, 9814,  2, 10323, 2, 10835,2  ; 210->214
        DW      11348, 2, 11863, 2, 12380, 2, 12899, 2, 13419,2  ; 215->219
        DW      13942, 2, 14467, 2, 14993, 2, 15521, 2, 16051,2  ; 220->224
        DW      16583, 2, 17117, 2, 17653, 2, 18191, 2, 18731,2  ; 225->229
        DW      19273, 2, 19817, 2, 20362, 2, 20910, 2, 21460,2  ; 230->234
        DW      22011, 2, 22565, 2, 23121, 2, 23678, 2, 24238,2  ; 235->239
        DW      24800, 2, 25363, 2, 25929, 2, 25497, 2, 27067,2  ; 240->244
        DW      27639, 2, 28213, 2, 28789, 2, 29367, 2, 29947,2  ; 245->249
        DW      30530, 2, 31114, 2, 31701, 2, 32289, 2, 32880, 2 ; 250->254
        DW      33473, 2, 34068, 2                               ; 255->256

FineLinearSlideDownTable Label ; Values are 0.16 bit
        DW      65535, 65477, 65418, 65359, 65300, 65241, 65182, 65359 ; 0->7
        DW      65065, 65006, 64947, 64888, 64830, 64772, 64713, 64645 ; 8->15

LinearSlideDownTable    Label ; Values are 0.16 bit
        DW      65535, 65300, 65065, 64830, 64596, 64364, 64132, 63901 ; 0->7
        DW      63670, 63441, 63212, 62984, 62757, 62531, 62306, 62081 ; 8->15
        DW      61858, 61635, 61413, 61191, 60971, 60751, 60532, 60314 ; 16->23
        DW      60097, 59880, 59664, 59449, 59235, 59022, 58809, 58597 ; 24->31
        DW      58386, 58176, 57966, 57757, 57549, 57341, 57135, 56929 ; 32->39
        DW      56724, 56519, 56316, 56113, 55911, 55709, 55508, 55308 ; 40->47
        DW      55109, 54910, 54713, 54515, 54319, 54123, 53928, 53734 ; 48->55
        DW      53540, 53347, 53155, 52963, 52773, 52582, 52393, 52204 ; 56->63
        DW      52016, 51829, 51642, 51456, 51270, 51085, 50901, 50718 ; 64->71
        DW      50535, 50353, 50172, 49991, 49811, 49631, 49452, 49274 ; 72->79
        DW      49097, 48920, 48743, 48568, 48393, 48128, 48044, 47871 ; 80->87
        DW      47699, 47527, 47356, 47185, 47015, 46846, 46677, 46509 ; 88->95
        DW      46341, 46174, 46008, 45842, 45677, 45512, 45348, 45185 ; 96->103
        DW      45022, 44859, 44698, 44537, 44376, 44216, 44057, 43898 ;104->111
        DW      43740, 43582, 43425, 43269, 43113, 42958, 42803, 42649 ;112->119
        DW      42495, 42342, 42189, 42037, 41886, 41735, 41584, 41434 ;120->127
        DW      41285, 41136, 40988, 40840, 40639, 40566, 40400, 40253 ;128->135
        DW      40110, 39965, 39821, 39678, 39535, 39392, 39250, 39109 ;136->143
        DW      38968, 38828, 38688, 38548, 38409, 38271, 38133, 37996 ;144->151
        DW      37859, 37722, 37586, 37451, 37316, 37181, 37047, 36914 ;152->159
        DW      36781, 36648, 36516, 36385, 36254, 36123, 35993, 35863 ;160->167
        DW      35734, 35605, 35477, 35349, 35221, 35095, 34968, 34842 ;168->175
        DW      34716, 34591, 34467, 34343, 34219, 34095, 33973, 33850 ;176->183
        DW      33728, 33607, 33486, 33365, 33245, 33125, 33005, 32887 ;184->191
        DW      32768, 32650, 32532, 32415, 32298, 32182, 32066, 31950 ;192->199
        DW      31835, 31720, 31606, 31492, 31379, 31266, 31153, 31041 ;200->207
        DW      30929, 30817, 30706, 30596, 30485, 30376, 30226, 30157 ;208->215
        DW      30048, 29940, 29832, 29725, 29618, 29511, 29405, 29299 ;216->223
        DW      29193, 29088, 28983, 28879, 28774, 28671, 28567, 28464 ;224->231
        DW      28362, 28260, 28158, 28056, 27955, 27855, 27754, 27654 ;232->239
        DW      27554, 27455, 27356, 27258, 27159, 27062, 26964, 26867 ;240->247
        DW      26770, 26674, 26577, 26482, 26386, 26291, 26196, 26102 ;248->255
        DW      26008                                                  ; 256

;�������������������������������������������������������������������������������

                                  Effect Info

Here's about all the info I can think of for effects. "Process" variables are
variables used internally by effects to control the direction of playback..
This section has not been completed yet.

First, here is the rough flow chart for processing information, it's not fully
detailed, but all of the important steps are outlined.

       ���������������������������������������������������������Ŀ
       � Set note volume to volume set for each channel          �
       � Set note frequency to frequency set for each channel    �
       �����������������������������������������������������������
                    �
       �������������������������Ŀ
       � Decrease tick counter   �        Yes
       �  Is tick counter 0 ?    �������������������������Ŀ
       ���������������������������                         �
                    �                                      �
                No  �                ����������������������������������������Ŀ
       �������������������������Ŀ   � Tick counter = Tick counter set        �
       � Update effects for each �   �                  (the current 'speed') �
       �  channel as required.   �   �      Decrease Row counter.             �
       �                         �   �        Is row counter 0?               �
       ���������������������������   ������������������������������������������
           �                                  No  �          �
           �                �����������������������          � Yes
           �                �                                �
           �  ����������������������������Ŀ ��������������������������������Ŀ
           �  � Call update-effects for    � �  Row counter = 1               �
           �  � each channel.              � �                                �
           �  ������������������������������ � Increase ProcessRow            �
           �                 �               � Is ProcessRow > NumberOfRows?  �
           �������������������               ����������������������������������
           �                                      Yes �                  � No
           �         �����������������������������������������������Ŀ   �
           �         �  ProcessRow = BreakRow                        �   �
           �         �  BreakRow = 0                                 �   �
           �         �  Increase ProcessOrder                        �   �
           �         �  while Order[ProcessOrder] = 0xFEh,           �   �
           �         �                         increase ProcessOrder �   �
           �         �  if Order[ProcessOrder] = 0xFFh,              �   �
           �         �                         ProcessOrder = 0      �   �
           �         �  CurrentPattern = Order[ProcessOrder]         �   �
           �         �������������������������������������������������   �
           �                               �                             �
           �                               �������������������������������
           �                               �
           �         ������������������������������������������������Ŀ
           �         � CurrentRow = ProcessRow                        �
           �         � Update Pattern Variables (includes jumping to  �
           �         �  the appropriate row if requried and getting   �
           �         �  the NumberOfRows for the pattern)             �
           �         ��������������������������������������������������
           �                               �
           ���������������������������������
           �
       �������������������Ŀ       Yes        �������������������������������Ŀ
       � Instrument mode?  ������������������Ĵ Update Envelopes as required  �
       ���������������������                  � Update fadeout as required    �
           �                                  � Calculate final volume if req �
           � No (Sample mode)                 � Calculate final pan if req    �
           �                                  � Process sample vibrato if req �
       �������������������������������������Ŀ���������������������������������
       � Calculate final volume if required  �                �
       � Calculate final pan if requried     �                �
       � Process sample vibrato if required  �                �
       ���������������������������������������                �
           �                                                  �
           �                                                  �
           ����������������������������������������������������
                                     �
                    �����������������������������������Ŀ
                    � Output sound!!!                   �
                    �������������������������������������

Axx     Set Tempo

        if (xx != 0) {
                Maxtick = xx;
                Currenttick = xx;
        }

Bxx     Jump to Order

        ProcessOrder = xx - 1;
        ProcessRow = 0xFFFE; // indicates new pattern internally for IT...

Cxx     Break to Row

        BreakRow = xx;
        ProcessRow = 0xFFFE;

Dxx     Volume slide down

        if (xx == 0) then xx = last xx for (Dxx/Kxx/Lxx) for this channel.

        Order of testing: Dx0, D0x, DxF, DFx

Dx0     Set effect update for channel enabled if channel is ON.
        If x = F, then slide up volume by 15 straight away also (for S3M compat)
        Every update, add x to the volume, check and clip values > 64 to 64
D0x     Set effect update for channel enabled if channel is ON.
        If x = F, then slide down volume by 15 straight away also (for S3M)
        Every update, subtract x from the volume, check and clip values < 0 to 0
DxF     Add x to volume straight away. Check and clip values > 64 to 64
DFx     Subtract x from volume straight away. Check and clip values < 0 to 0

Hxy     Vibrato

        if (x != 0) {
                speed = 4*x;
        }
        if (y != 0) {
                depth = y * 4;
                if(OldEffects) depth <<= 1;
        }
        Set effect update for channel enabled if channel is ON.
        Goto InitVibrato (explained later)

Ixy     Tremor, ontime x, offtime y

        if (x != 0) {
                ontime = x;
                if (oldeffects) ontime++;
        }
        if (y != 0) {
                offtime = y;
                if (oldeffects) offtime++;
        }

Nxx     Channel volume slide down

        if (xx == 0) then xx = last Nxx for this channel.

        Order of testing: Nx0, N0x, NxF, NFx

Nx0     Set effect update for channel enabled.
        Every update, add x to the volume, check and clip values > 64 to 64
N0x     Set effect update for channel enabled.
        Every update, subtract x from the volume, check and clip values < 0 to 0
NxF     Add x to volume straight away. Check and clip values > 64 to 64
NFx     Subtract x from volume straight away. Check and clip values < 0 to 0

Uxy     Fine Vibrato

        if (x != 0) {
                speed = 4*x;
        }
        if (y != 0) {
                depth = y;
                if(OldEffects) depth <<= 1;
        }
        Set effect update for channel enabled if channel is ON.
        Goto InitVibrato (explained later)

Wxx     Global volume slide down

        if (xx == 0) then xx = last Wxx for this channel.

        Order of testing: Wx0, W0x, WxF, WFx

Wx0     Set effect update for channel enabled.
        Every update, add x to the volume, check and clip values > 128 to 128
W0x     Set effect update for channel enabled.
        Every update, subtract x from the volume, check and clip values < 0 to 0
WxF     Add x to volume straight away. Check and clip values > 128 to 128
WFx     Subtract x from volume straight away. Check and clip values < 0 to 0

.. sorry this is incomplete..
*/
