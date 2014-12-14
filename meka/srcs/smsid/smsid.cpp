#include "shared.h"
#include "db.h"
#include "vdp.h"
#include <zlib.h>
#include <string>

//#define MEKA_NAM_PATH		"meka.nam"
//#define MEKA_NAM_PATH		"C:\\S-Sms\\mekasvn\\trunk\\meka.nam"

static bool		System_SetClipboardText(const char *text);

int main(int argc, char* argv[])
{
	const char* filename = NULL;
	const char* opt_meka_nam = "meka.nam";
	bool opt_copy_to_clipboard = false;

	for (int n = 1; n < argc; n++)
	{
		const char* arg = argv[n];
		if (arg[0] == '-' || arg[0] == '/')
		{
			if (strcmp(arg+1, "c") == 0)
			{
				opt_copy_to_clipboard = true;
			}
			else if (strcmp(arg+1, "db") == 0)
			{
				n++;
				if (n >= argc)
				{
					printf("Error: missing filename as argument to option '%s'\n", arg);
					return 1;
				}
				opt_meka_nam = argv[n];
			}
			else
			{
				printf("Error: unknown argument '%s'\n", arg);
				return 1;
			}
		}
		else
		{
			filename = arg;
		}
	}

	if (filename == NULL)
	{
		printf("smsid 1.0\n");
		printf("Syntax: smsid [/db <file>] [/c] <rom>\n");
		printf(" /db    path to meka.nam [meka.nam]\n");
		printf(" /c     copy text to OS clipboard [false]\n");
		return 0;
	}

	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		printf("Error: unable to input file '%s', aborting.\n", filename);
		return 1;
	}

	fseek(f, 0, SEEK_END);
	const int rom_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	unsigned char* rom_data = new unsigned char[rom_size];
	if (fread(rom_data, 1, rom_size, f) != rom_size)
	{
		printf("Error: unable to read file data, aborting.\n");
		return 1;
	}

	const u32 crc = crc32(0, rom_data, rom_size);
	//const u32 crc = 0xe5ff50d8;
	//const u32 crc = 0xaed9aac4;
	fclose(f);
	delete[] rom_data;

	if (!DB_Init(opt_meka_nam, false))
	{
		printf("Error: failed to open DB file '%s'\n", opt_meka_nam);
		return 1;
	}

	if (opt_copy_to_clipboard)
		printf("(copying following output to OS clipboard)\n");

	std::string s;

	t_db_entry* entry = DB_Entry_Find(crc, NULL);
	if (entry)
	{
		char buf[256];
		sprintf(buf, "%s %08x %08X%08X   %s", 
			DB_FindDriverNameById(entry->system),
			entry->crc_crc32, 
			entry->crc_mekacrc.v[0], entry->crc_mekacrc.v[1],
			entry->names[0]);
		s += buf;

		int countries = 0;
		for (int i = 0; i != DB_COUNTRY_COUNT_; i++)
		{
			if (entry->country & (1 << i))
			{
				if (countries == 0)
					s += " (";
				else
					s += ",";
				s += DB_FindCountryNameByFlag(1 << i);
				countries++;
			}
		}
		if (countries > 0)
			s += ")";
		s += "\n";
	}
	else
	{
		s = "Unknown.\n";
	}
	printf("%s", s.c_str());

	if (opt_copy_to_clipboard)
	{
		System_SetClipboardText(s.c_str());
	}

	return 0;
}

static bool		System_SetClipboardText(const char *text)
{
	// Open clipboard
	if (!OpenClipboard(NULL))
		return false;

	// Global alloc
	const int text_length = strlen(text) + 1;
	HGLOBAL text_handle = GlobalAlloc(GMEM_MOVEABLE, text_length * sizeof(char)); 
	if (text_handle == NULL)
		return false;

	// Lock the handle and copy the text to the buffer. 
	char *text_copy = (char *)GlobalLock(text_handle); 
	strcpy(text_copy, text);
	GlobalUnlock(text_handle); 

	EmptyClipboard();
	SetClipboardData(CF_TEXT, text_handle);
	CloseClipboard();
	return true;
}

