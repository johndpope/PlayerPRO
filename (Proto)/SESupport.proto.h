
/* SESupport.c */
pascal void ModSetupProgress(ModParamsPtr, short *, StringHandle);
pascal OSErr ModShowProgress(unsigned long, unsigned long, short *, Boolean);
pascal void ModCloseProgress(void);
pascal WindowPtr GetProgressWind(void);
pascal OSErr LoadEffectSettings(StringPtr, ModSettingsHandle *);
pascal OSErr SaveEffectSettings(StringPtr, ModSettingsHandle *);
pascal short GetEffectSettingsRefNum(void);
pascal OSErr ModDoSettingsDialog(ModParamsPtr, ItemInfoHandle, Boolean, ModalFilterProcPtr, ProcPtr, ModSetInitProcPtr);
pascal short ModGetSetupsNumber(ModParamsPtr);
pascal OSErr ModGetSetup(ModParamsPtr, short, Handle *);
pascal OSErr ModGetSetupName(ModParamsPtr, short, StringPtr);
pascal OSErr ModSetSetup(ModParamsPtr, short, Handle, StringPtr);
pascal OSErr ModDeleteSetup(ModParamsPtr, DialogPtr, short, short);
pascal OSErr ModSetupCreateMenu(ModParamsPtr, DialogPtr, short);
pascal OSErr ModSetupSwitchCustom(ModParamsPtr, DialogPtr, short, Boolean);
pascal OSErr GetMachineInfo(MachinePtr *);
pascal short AskDoExtend(void);
pascal void ModInitRandSeed(void);
pascal short ApplWindowsUpdate(WindowPtr);
pascal short ApplWindowsActivate(EventRecord *);
pascal void ModConvertToUnits(ModParamsPtr, StringPtr, unsigned long, short);
pascal void ModConvertToBytes(ModParamsPtr, StringPtr, unsigned long *, short);
pascal long GetBytesToProcess(ModParamsPtr);
pascal OSErr ModMaxChSize(ModParamsHandle, unsigned long *);
pascal OSErr ModMaxRelChSize(ModParamsHandle, unsigned long *);
pascal OSErr ModGetPeak(ModParamsHandle, unsigned long *, unsigned long *);
pascal void ModGetSampleValueLimits(short, signed long *, signed long *);
pascal unsigned long ModGetOffsToSndHeader(Handle);
pascal unsigned long ModGetOffsToSndData(Handle);
pascal Boolean ModToggleInputLevel(Boolean);
pascal OSErr ModDrawWave(ModParamsPtr, Rect *, unsigned long, unsigned long, unsigned long, short, short);
pascal OSErr ModShowError(ModParamsPtr, short, Boolean);
pascal OSErr ModAddNewChannel(ModParamsHandle, short);
pascal OSErr ModDelChannel(ModParamsHandle, short);
pascal short ModGetChannelPan(ChanAttrsHandle, short, Boolean);
pascal void ModSetChannelPan(ChanAttrsHandle, short, short, short);
pascal OSErr ModSilenceOut(ModParamsHandle, short, short, unsigned long, unsigned long);
pascal OSErr ModResampleSoundBits(ModParamsHandle, short);
pascal OSErr ModResampleSoundRate(ModParamsHandle, Fixed);
pascal OSErr ModNewDocument(ModParamsHandle *);
pascal OSErr ModChangeNewDocument(ModParamsHandle);
void ConstructGlue(GluePtr);