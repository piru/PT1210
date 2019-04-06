/**
 *  ____ _____     _ ____  _  ___
 * |  _ |_   _|   / |___ \/ |/ _ \
 * | |_) || |_____| | __) | | | | |
 * |  __/ | |_____| |/ __/| | |_| |
 * |_|    |_|     |_|_____|_|\___/
 *
 * Protracker DJ Player
 *
 * filesystem.c
 * File I/O functions.
 */

#include <ctype.h>
#include <string.h>

#include <clib/debug_protos.h>
#include <proto/exec.h>

#include "filesystem.h"
#include "utility.h"

file_list_t pt1210_file_list[MAX_FILE_COUNT];
uint16_t pt1210_file_count;

/* Directory locks */
static BPTR old_dir_lock = 0;
static BPTR current_dir_lock = 0;

/* Imported from ASM code */
extern char FS_LoadErrBuff[80];
void FS_DrawLoadError(REG(d0, int32_t error_code));

/* A generic comparator function pointer type */
typedef int (*comparator_t)(const void*, const void*);

/* Flag to swap comparator operands (for reversing sort) */
static bool cmp_swap = false;

/* Comparator functions for sorting each of the file structure fields */
/* FIXME: Turn this into a function and only check vols/assigns when FS in volume mode */
#define CMP_NON_FILE_ENTRIES()														\
	/* Parent entries first... */ 													\
	if (lhs->type == ENTRY_PARENT && rhs->type != ENTRY_PARENT)						\
		return -1;																	\
																					\
	if (rhs->type == ENTRY_PARENT && lhs->type != ENTRY_PARENT) 					\
		return 1;																	\
																					\
	/* ...followed by directories in ascending order */								\
	if (lhs->type == ENTRY_DIRECTORY && rhs->type != ENTRY_DIRECTORY)				\
		return -1;																	\
																					\
	if (rhs->type == ENTRY_DIRECTORY && lhs->type != ENTRY_DIRECTORY)				\
		return 1;																	\
																					\
	/* ...followed by volumes in ascending order */									\
	if (lhs->type == ENTRY_VOLUME && rhs->type != ENTRY_VOLUME)						\
		return -1;																	\
																					\
	if (rhs->type == ENTRY_VOLUME && lhs->type != ENTRY_VOLUME)						\
		return 1;																	\
																					\
	/* ...followed by assigns in ascending order */									\
	if (lhs->type == ENTRY_ASSIGN && rhs->type != ENTRY_ASSIGN)						\
		return -1;																	\
																					\
	if (rhs->type == ENTRY_ASSIGN && lhs->type != ENTRY_ASSIGN)						\
		return 1;																	\
																					\
	if (lhs->type != ENTRY_FILE && lhs->type == rhs->type)							\
		return strncasecmp(lhs->file_name, rhs->file_name, MAX_FILE_NAME_LENGTH);

static int cmp_bpm(const void* a, const void* b)
{
	const file_list_t* lhs = a;
	const file_list_t* rhs = b;

	CMP_NON_FILE_ENTRIES();

	return cmp_swap ? rhs->bpm - lhs->bpm : lhs->bpm - rhs->bpm;
}

static int cmp_name(const void* a, const void* b)
{
	const file_list_t* lhs = a;
	const file_list_t* rhs = b;

	CMP_NON_FILE_ENTRIES();

	/* Ignore any MOD. prefixes */
	const char* lhs_name = lhs->file_name;
	const char* rhs_name = rhs->file_name;

	if (has_mod_prefix(lhs_name) && lhs_name[4] != '\0')
		lhs_name += 4;
	if (has_mod_prefix(rhs_name) && rhs_name[4] != '\0')
		rhs_name += 4;

	return cmp_swap ?
		strncasecmp(rhs_name, lhs_name, MAX_FILE_NAME_LENGTH) :
		strncasecmp(lhs_name, rhs_name, MAX_FILE_NAME_LENGTH);
}

void pt1210_file_initialize()
{
	if (old_dir_lock)
		return;

	/* Find our own process and retrieve lock */
	struct Process* process = (struct Process*) FindTask(NULL);
	old_dir_lock = process->pr_CurrentDir;

	/* Create a copy of the old lock and change to it */
	current_dir_lock = DupLock(old_dir_lock);
	CurrentDir(current_dir_lock);
}

void pt1210_file_shutdown()
{
	/* Restore current directory to old lock and free our own */
	CurrentDir(old_dir_lock);
	UnLock(current_dir_lock);

	current_dir_lock = 0;
	old_dir_lock = 0;
}

bool pt1210_file_change_dir(const char* path)
{
	/* Attempt to get a lock on the selected directory */
	BPTR dir_lock = Lock(path, ACCESS_READ);
	if (!dir_lock)
		return false;

	/* Free current lock and change directory */
	UnLock(current_dir_lock);
	current_dir_lock = dir_lock;
	CurrentDir(dir_lock);
	return true;
}

bool pt1210_file_parent_dir()
{
	BPTR parent_lock = ParentDir(current_dir_lock);
	if (parent_lock)
	{
		UnLock(current_dir_lock);
		current_dir_lock = parent_lock;
		CurrentDir(current_dir_lock);
		return true;
	}

	return false;
}

void pt1210_file_gen_file_list()
{
	file_list_t* list_entry = &pt1210_file_list[0];

	/* Get a longword-aligned block of memory to store the file information block */
	struct FileInfoBlock* fib = AllocMem(sizeof(*fib), MEMF_CLEAR | MEMF_PUBLIC);
	if (!fib)
		return;

	/* Add "parent directory" entry */
	list_entry->type = ENTRY_PARENT;
	strcpy(list_entry->file_name, "PARENT");
	pt1210_file_count = 1;

	/* Iterate over directory contents and search for modules */
	if (Examine(current_dir_lock, fib))
	{
		while (pt1210_file_count < MAX_FILE_COUNT)
		{
			if (!ExNext(current_dir_lock, fib))
				break;

			/* If DirEntryType is >0, it's a directory) */
			if (fib->fib_DirEntryType > 0)
			{
				list_entry = &pt1210_file_list[pt1210_file_count];
				list_entry->type = ENTRY_DIRECTORY;
				strncpy(list_entry->file_name, fib->fib_FileName, MAX_FILE_NAME_LENGTH);
				++pt1210_file_count;
			}
			else
			{
				/* Check this file is a module and add it to the file browser if so */
				pt1210_file_check_module(fib);
			}
		}
	}

	FreeMem(fib, sizeof(*fib));
}

void pt1210_file_gen_volume_list()
{
	pt1210_file_count = 0;

	/* Start of critical section */
	Forbid();

	struct DosInfo* dos_info = (struct DosInfo*) BADDR(DOSBase->dl_Root->rn_Info);
	struct DevInfo* dvi = (struct DevInfo*) BADDR(dos_info->di_DevInfo);

	do
	{
#ifdef DEBUG
		kprintf("Checking %s %ld\n", ((char*)BADDR(dvi->dvi_Name) + 1), dvi->dvi_Type);
#endif
		file_list_t* list_entry = &pt1210_file_list[pt1210_file_count];

		switch(dvi->dvi_Type)
		{
			case DLT_VOLUME:		list_entry->type = ENTRY_VOLUME; break;
			case DLT_DIRECTORY:		list_entry->type = ENTRY_ASSIGN; break;
			case DLT_LATE:			list_entry->type = ENTRY_ASSIGN; break;
			case DLT_NONBINDING:	list_entry->type = ENTRY_ASSIGN; break;
			default:				continue;
		}

		/* BCPL strings have the length as the first byte */
		void* vol_name_bstr = BADDR(dvi->dvi_Name);
		uint8_t vol_name_len = *(uint8_t*) vol_name_bstr;
		char* vol_name = (char*) vol_name_bstr + 1;
		strncpy(list_entry->file_name, vol_name, MAX_FILE_NAME_LENGTH);

		/* Add colon */
		list_entry->file_name[vol_name_len] = ':';
		list_entry->file_name[vol_name_len + 1] = '\0';

		++pt1210_file_count;
	} while ((dvi = (struct DevInfo*) BADDR(dvi->dvi_Next)) && pt1210_file_count < MAX_FILE_COUNT);

	/* End of critical section */
	Permit();
}

const char* pt1210_file_dev_name_from_vol_name(const char* vol_name)
{
	/* Start of critical section */
	Forbid();

	struct DosInfo* dos_info = (struct DosInfo*) BADDR(DOSBase->dl_Root->rn_Info);
	struct DevInfo* dvi_vol = (struct DevInfo*) BADDR(dos_info->di_DevInfo);
	struct DevInfo* dvi_dev = (struct DevInfo*) BADDR(dos_info->di_DevInfo);
	const char* dev_name = NULL;

	/* Trim the colon from the end */
	char vol_name_trimmed[MAX_FILE_NAME_LENGTH + 1];
	char* cur_char = vol_name_trimmed;
	while (*vol_name != '\0' && *vol_name != ':')
		*cur_char++ = *vol_name++;
	*cur_char = '\0';

	/* Find our volume in the DOS list*/
	do
	{
		if (dvi_vol->dvi_Type != DLT_VOLUME)
			continue;

		const char* dvi_vol_name = (const char*) BADDR(dvi_vol->dvi_Name) + 1;

		/* Found it */
		if (!strcmp(vol_name_trimmed, dvi_vol_name))
		{
			/* Now look for the device that shares the same Task */
			do
			{
				if (dvi_dev->dvi_Type != DLT_DEVICE)
					continue;

				if (dvi_dev->dvi_Task != dvi_vol->dvi_Task)
					continue;

				/* Found it, return the device name */
				dev_name = (const char*) BADDR(dvi_dev->dvi_Name) + 1;
				break;
			} while ((dvi_dev = (struct DevInfo*) BADDR(dvi_dev->dvi_Next)));
			break;
		}
	} while ((dvi_vol = (struct DevInfo*) BADDR(dvi_vol->dvi_Next)));

	/* End of critical section */
	Permit();

	return dev_name;
}

void pt1210_file_sort_list(file_sort_key_t key, bool descending)
{
	if (pt1210_file_count <= 1)
		return;

	/* Function pointer to the comparator we want to use */
	comparator_t comparator;
	cmp_swap = descending;

	switch (key)
	{
		case SORT_NAME:		comparator = cmp_name;		break;
		case SORT_BPM:		comparator = cmp_bpm;		break;
		default:			return;
	}

	/* Perform quicksort */
	qsort(pt1210_file_list, pt1210_file_count, sizeof(*pt1210_file_list), comparator);
}

void pt1210_file_check_module(struct FileInfoBlock* fib)
{
	file_list_t* list_entry = &pt1210_file_list[pt1210_file_count];
	uint32_t magic = 0;
	uint8_t first_pattern = 0;
	uint32_t pattern_row[4];
	uint32_t fpb_tag[2];

	/* Ignore files too small to be valid Protracker modules */
	if (fib->fib_Size < MIN_MODULE_FILE_SIZE)
		return;

	/* Check for valid magic numbers */
	pt1210_file_read(fib->fib_FileName, &magic, PT_MAGIC_OFFSET, sizeof(magic));
	if (magic != PT_MAGIC && magic != PT_MAGIC_64_PAT)
		return;

	/* Get number of first pattern */
	if (!pt1210_file_read(fib->fib_FileName, &first_pattern, PT_POSITION_OFFSET, sizeof(first_pattern)))
		return;

	/* Multiply to get offset into pattern data */
	size_t pattern_offset = PT_PATTERN_OFFSET + first_pattern * PT_PATTERN_DATA_LEN;

	/* Read first row of first pattern */
	if (!pt1210_file_read(fib->fib_FileName, pattern_row, pattern_offset, sizeof(pattern_row)))
		return;

	/* Store a default BPM */
	list_entry->bpm = DEFAULT_BPM;

	/* Iterate over the first row and look for FXX commands to determine BPM */
	for (uint8_t i = 0; i < 4; ++i)
	{
		/* Do we have a tempo command? */
		if ((pattern_row[i] & 0x0F00) != 0x0F00)
			continue;

		/* Parameters >= 0x20 are tempo; else frames per row ("SPD") */
		uint8_t param = pattern_row[i] & 0xFF;
		if (param >= 0x20)
		{
			list_entry->bpm = param;
			break;
		}
	}

	/* Look for a frames-per-beat tag in the name string of sample 31 */
	list_entry->frames = 0;
	if (!pt1210_file_read(fib->fib_FileName, fpb_tag, PT_SMP_31_NAME_OFFSET, sizeof(fpb_tag)))
		return;

	/* Force upper case on text */
	fpb_tag[0] &= FPB_MAGIC_UPPER;

	if (fpb_tag[0] == FPB_MAGIC)
	{
		uint8_t tens = (fpb_tag[1] >> 24) & 0x0F;
		uint8_t units = (fpb_tag[1] >> 16) & 0x0F;
		uint16_t fpb = (tens * 10) + units;

		if (fpb)
		{
			list_entry->frames = fpb;
			list_entry->bpm = list_entry->bpm * DEFAULT_FPB / fpb;
		}
	}

	/* Set list entry type as file */
	list_entry->type = ENTRY_FILE;

	/* Store file name */
	strncpy(list_entry->file_name, fib->fib_FileName, MAX_FILE_NAME_LENGTH);

	/* Store file size */
	list_entry->file_size = fib->fib_Size;

	++pt1210_file_count;
}

bool pt1210_file_read(const char* file_name, void* buffer, size_t seek_point, size_t read_size)
{
	BPTR file;
	LONG result;

	file = Open(file_name, MODE_OLDFILE);
	if (!file)
	{
		pt1210_file_read_error();
		return false;
	}

	/* FIXME: Possible bug in Kickstarts v36/v37 not returning -1 on error */
	result = Seek(file, seek_point, OFFSET_BEGINNING);
	if (result == -1)
	{
		pt1210_file_read_error();
		Close(file);
		return false;
	}

	result = Read(file, buffer, read_size);
	if (result == -1)
		pt1210_file_read_error();

	Close(file);

	if (result != read_size)
		return false;

	/* Success */
	return true;
}

void pt1210_file_read_error()
{
	LONG error = IoErr();

	/* TODO: Use Fault() when it's available (Kickstart v36) */
	/* Fault(error, "", FS_LoadErrBuff, sizeof(FS_LoadErrBuff)); */

	FS_DrawLoadError(error);
}
