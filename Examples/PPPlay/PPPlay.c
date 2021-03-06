#include <stdio.h>

#ifdef __APPLE__
#include <PlayerPROCore/PlayerPROCore.h>
#else
#include "RDriver.h"
#endif

#define PPbanner \
"=======================================================================\n" \
"PlayerPRO Driver v 6.0  - Portable version - Programmed by A.ROSSET '02\n" \
"Maintained by Charles \"Madd the Sane\" Betts\n"\
"=======================================================================\n" \
"       This program is FREEWARE - Read README.TXT for more info\n" \
"\n" \
"     E-Mail : computers57@hotmail.com    \n"\
"=======================================================================\n"

static void debugger(const char *a)
{
	printf("%s", a);
	getchar();
}

static void printMADDriverSettings(const MADDriverSettings *toPrint)
{
	printf(" Current settings: ");
	printf("%ikHz-%ibits-", toPrint->outPutRate / 1000, toPrint->outPutBits);
	switch (toPrint->outPutMode) {
		case DeluxeStereoOutPut:
		case StereoOutPut:
			printf("stereo");
			break;
			
		case MonoOutPut:
			printf("mono");
			break;
			
		case PolyPhonic:
			printf("multi");
			break;
			
		default:
			printf("unknown");
			break;
	}
	
	printf("-MicroDelay %ims-Interpolation\n", toPrint->MicroDelaySize);
	
	printf("=======================================================================\n");
}

int main(int argc, char *argv[])
{
	MADDriverSettings	init;
	MADDriverRec		*aDriver;
	MADLibrary			*lib;
	MADMusic 			*aMusic;
	long				fT, cT;
	char 				type[5];
	int					returntype = EXIT_SUCCESS;
	
	printf("%s", PPbanner);
	
	if (argc != 2) {
		fprintf(stderr ,"Usage: %s MyMusicFile\n", argv[0]);
		return EXIT_FAILURE;
	} else {
#if 1
		MADGetBestDriver(&init);
#else
		init.numChn				= 4;
		init.outPutBits 		= 16;
		init.outPutRate			= 44100;
		init.outPutMode			= DeluxeStereoOutPut;
		//init.driverMode			= Wave95NT;
		
		// You can also try: init.driverMode = Wave95NT;
		
		init.repeatMusic		= false;
		init.MicroDelaySize 	= 0;
		init.surround		 	= false;
		init.Reverb				= false;
		init.ReverbSize			= 45;
		init.ReverbStrength		= 50;
		init.TickRemover		= false;
		init.oversampling		= 1;
#endif
		
		printMADDriverSettings(&init);
		
		if (MADInitLibrary("Plugins", &lib)) {
			debugger("ERR MADInitLibrary\n");
			returntype = EXIT_FAILURE;
		} else {
			if (MADCreateDriver(&init, lib, &aDriver)) {
				debugger("ERR MADCreateDriver\n");
				returntype = EXIT_FAILURE;
			} else {
				if (MADMusicIdentifyCString(lib, type, argv[1])) {
					debugger("ERR MADMusicIdentifyCString\n");
					returntype = EXIT_FAILURE;
				} else {
					if (MADLoadMusicFileCString(lib, &aMusic, type, argv[ 1])) {
						debugger("ERR MADLoadMusicFile\n");
						returntype = EXIT_FAILURE;
					} else {
						double fTd = 0;
						MADAttachDriverToMusic(aDriver, aMusic, NULL);
						
						MADStartDriver(aDriver);
						MADPlayMusic(aDriver);
						MADGetMusicStatus(aDriver, &fT, &cT);
						
						fTd = fT / 60.0;
						
						printf("\n\nMusic length: %.2f s", fTd);
						printf("\nPress Return to exit\n");
						
						getchar();
						
						MADDisposeMusic(&aMusic, aDriver);
					}
				}
				MADStopMusic(aDriver);
				MADStopDriver(aDriver);
				
				MADDisposeDriver(aDriver);
			}
			MADDisposeLibrary(lib);
		}
	}
	return returntype;
}
