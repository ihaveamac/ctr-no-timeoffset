#include <string.h>
#include <stdio.h>

#include <3ds.h>

union TimeOffset {
	u8 raw_offset[8];
	s64 offset;
};

int main(int argc, char **argv) {
	Result res;
	union TimeOffset timeOffset;

	gfxInitDefault();
	res = cfguInit();
	consoleInit(GFX_TOP, NULL);

	u8 zeros[8] = {0};

	printf("ctr-no-timeoffset v%s by ihaveamac\n\n", VERSION);

	if (R_FAILED(res)) {
		printf("cfguInit failed: 0x%lx\nMake sure the SM patch is enabled or this program has cfg:u access.", res);
	} else {
		res = CFGU_GetConfigInfoBlk2(8, 0x00030001, &timeOffset.raw_offset[0]);
		if (R_FAILED(res)) {
			printf("CFGU_GetConfigInfoBlk2 failed: 0x%lx\nMake sure the SM patch is enabled or this program has cfg:u access.\n", res);
		} else {
			printf("Current offset: %lli\n", timeOffset.offset);
			putchar('\n');
			puts("Press A to set offset to 0.");
		}
	}
	puts("Press START to quit.");
	puts("Press Y to reboot.");

	while (aptMainLoop()) {

		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_A) {
			res = CFG_SetConfigInfoBlk4(8, 0x00030001, zeros);
			if (R_FAILED(res)) {
				printf("\nCFG_SetConfigInfoBlk4 failed: 0x%lx\nMake sure the SM patch is enabled or this program has cfg:s access.\n", res);
			}
			res = CFG_UpdateConfigSavegame();
			if (R_FAILED(res)) {
				printf("\nCFG_UpdateConfigNANDSavegame failed: 0x%lx\nMake sure the SM patch is enabled or this program has cfg:s access.\n", res);
			}

			if (R_SUCCEEDED(res)) {
				puts("Offset changed successfully. Reboot is highly\nrecommended.");
			}
		}
		if (kDown & KEY_Y) {
			puts("Rebooting...");
			APT_HardwareResetAsync();
			while (1);
		}
		if (kDown & KEY_START)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	cfguExit();
	gfxExit();
	return 0;
}
