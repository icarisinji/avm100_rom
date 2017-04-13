#include <stdio.h>

#define NOR_BASE_ROM_NAME		"D:\\resource\\avm100_rom\\NOR_ROM.bin"
#define MAIN_COLOR_LUT_NAME		"D:\\resource\\avm100_rom\\AVM100V431_OSDView_mod_panorama .bin"
#define NOR_BLOCK_SIZE			(128*1024)
#define NOR_MAINLUT_OFFSET		(NOR_BLOCK_SIZE*70)
#define NOR_NEW_ROM_NAME		"D:\\resource\\avm100_rom\\NOR_ROM_NEW.bin"
#define NOR_MA_IMAGE			"D:\\resource\\avm100_rom\\MA.bin"

#define SPI_BASE_130_NAME		"D:\\resource\\avm100_rom\\AVM100V431_OSDView.bin_130_old"
#define SPI_MOD_ICON_NAME		"D:\\resource\\avm100_rom\\AVM100V431_OSDView_MOD_ICON.bin"
#define SPI_NEW_130_NAME		"D:\\resource\\avm100_rom\\AVM100V431_OSDView.bin_130_new"

#define OFFSET_APP_MAIN			0x440000
#define SIZE_APP_MAIN			0x400000
#define OFFSET_UP_MAIN			0x240000
#define SIZE_UP_MAIN			0x200000

#define OFFSET_MA_CONFIG_VERSION 0x880020 /* 바이너리 파일에서 직접 수정해서 적용 하였는데, 나중에는 이 옵셋사용해서 할수 있게 해야될듯*/

static int _load_file(char *_pPath, unsigned char **_ppBuf, long *_pReadsize)
{
	FILE *pF = NULL;
	int errno = 0;
	int retval = 0;
	long filesize = 0;


	if (!_pPath)
	{
		printf("file input path is unknown \n");
		retval = -1;
		goto return_1;
	}
	errno = fopen_s(&pF, _pPath, "rb");
	if (pF == NULL)
	{
		printf("failed to open file %s\n", _pPath);
		retval = -1;
		goto return_1;
	}
	fseek(pF, 0L, SEEK_END);
	filesize = ftell(pF);
	fseek(pF, 0L, SEEK_SET);

	*_ppBuf = malloc(sizeof(char)*filesize);
	if (*_ppBuf == NULL)
	{
		printf("failed to alloc buffer\n");
		retval = -1;
		goto return_1;
	}

	errno = fread(*_ppBuf, 1, filesize, pF);
	if (errno != filesize)
	{
		printf("failed to read file \n");
		retval = -1;
		goto return_1;
	}

	printf("\n ================== %s ==========================\n", _pPath);
	//printf("read size : %d\n", filesize);

	*_pReadsize = filesize;
return_1:
	if (pF != NULL)
		fclose(pF);

	return retval;
}

static int _write(char *_pPath, unsigned char *_pBuff, long _size)
{
	FILE *pF = NULL;
	int errno = 0;
	int retval = 0;
	long writtenSize = 0;

	errno = fopen_s(&pF, _pPath, "wb+");
	if (pF == NULL)
	{
		printf("failed to open file %s\n", _pPath);
		retval = -1;
	}
	writtenSize = fwrite(_pBuff, 1, _size, pF);
	printf("written size  %d\n", writtenSize);

	if (pF != NULL)
		fclose(pF);

	return retval;
}

static void replace_main_app(void)
{
	unsigned char *pBufNorBase = NULL, *pBufMainApp = NULL;
	int errno = 0;
	int retval = 0;
	long norBaseSize = 0, MainAPPsize = 0;

	if (_load_file(NOR_BASE_ROM_NAME, &pBufNorBase, &norBaseSize) < 0)
	{
		retval = -1;
		goto return_1;
	}
	printf("NOR Base size : %d\n", norBaseSize);
	if (_load_file(NOR_MA_IMAGE, &pBufMainApp, &MainAPPsize) < 0)
	{
		retval = -1;
		goto return_1;
	}
	printf("Main APP size : %d\n", MainAPPsize);

	memset(&pBufNorBase[OFFSET_APP_MAIN], 0xff, SIZE_APP_MAIN);
	memcpy(&pBufNorBase[OFFSET_APP_MAIN], pBufMainApp, MainAPPsize);
	_write(NOR_NEW_ROM_NAME, pBufNorBase, norBaseSize);


return_1:
	if (pBufNorBase != NULL)
		free(pBufNorBase);
	if (pBufMainApp != NULL)
		free(pBufMainApp);
}

static int update_icon(void)
{
	unsigned char *pBufNorBase = NULL, *pBufMainLut = NULL, *pBufST = NULL;
	int errno = 0;
	int retval = 0;
	long norBaseSize = 0, MainLUTsize = 0, fftSizeWithPadding = 0, stsize = 0, stSizeWithPadding = 0, totalSize = 0;
	int numofblock = 0;
	unsigned char *ptempFFTBuff = NULL, *ptempSTBuff = NULL, *ptempSpiBuff = NULL;

	if (_load_file(SPI_BASE_130_NAME, &pBufNorBase, &norBaseSize) < 0)
	{
		retval = -1;
		goto return_1;
	}
	printf("NOR Base size : %d\n", norBaseSize);
	if (_load_file(SPI_MOD_ICON_NAME, &pBufMainLut, &MainLUTsize) < 0)
	{
		retval = -1;
		goto return_1;
	}
	printf("Main LUT size : %d\n", MainLUTsize);

	memcpy(&pBufNorBase[0x4c0000], &pBufMainLut[7], (MainLUTsize - 7));
	_write(SPI_NEW_130_NAME, pBufNorBase, norBaseSize);


return_1:
	if (pBufNorBase != NULL)
		free(pBufNorBase);
	if (pBufMainLut != NULL)
		free(pBufMainLut);

	system("pause");
	return retval;
}
int main(void)
{
	int retval = 0;
	replace_main_app();

	system("pause");
	return retval;
}