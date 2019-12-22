/**
 *  ____ _____     _ ____  _  ___
 * |  _ |_   _|   / |___ \/ |/ _ \
 * | |_) || |_____| | __) | | | | |
 * |  __/ | |_____| |/ __/| | |_| |
 * |_|    |_|     |_|_____|_|\___/
 *
 * Protracker DJ Player
 *
 * fileselector.c
 * File selector UI.
 */

#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <string.h>

#include <proto/dos.h>
#include <proto/exec.h>

#include "fileselector.h"
#include "filesystem.h"
#include "font.h"
#include "graphics.h"
#include "utility.h"

static bool show_volumes = false;
static file_sort_key_t sort_key = SORT_NAME;
static bool sort_descending = false;

static size_t current = 0;
static size_t list_pos = 0;

static char fs_text[FS_HEIGHT_CHARS][FS_WIDTH_CHARS];
/*static*/ uint8_t __chip pt1210_fs_bitplane[FS_BITPLANE_SIZE_BYTES];

static const char* error_template = "--------------------------------------- "
									"%-" STR(FS_WIDTH_CHARS) "c"
									"%-*c%s%-*c"
									"%-" STR(FS_WIDTH_CHARS) "c"
									"--------------------------------------- ";

static const char* error_no_modules = "NO MODULES FOUND";
/*static const char* error_unspecified = "UNSPECIFIED ERROR!";*/

static const char* avail_template = "CHIP: %lukb FAST: %lukb";

/* TODO: Move these into here? */
extern file_list_t pt1210_file_list[MAX_FILE_COUNT];
extern uint16_t pt1210_file_count;

/* ASM functions */
void ST_Type(REG(a0, const char* text), REG(a1, void* dest_surface), REG(d7, uint8_t num_lines));
void UI_TypeTitle(REG(a0, const char*), REG(d4, size_t));

/* Reference to start of ASM copper list instructions that draw selector line */
extern volatile uint16_t selectaline[];

static void clear_list()
{
	memset(fs_text, ' ', sizeof(fs_text));
	memset(pt1210_fs_bitplane, 0, sizeof(pt1210_fs_bitplane));
}

static void draw_list()
{
	size_t offset = current - list_pos;
	size_t draw_len = min(FS_HEIGHT_CHARS, pt1210_file_count);

	for (size_t i = 0; i < draw_len; ++i)
	{
		file_list_t* list_entry = &pt1210_file_list[i + offset];
		switch (list_entry->type)
		{
			case ENTRY_PARENT:
				snprintf(fs_text[i], FS_WIDTH_CHARS, FONT_ICON_PARENT " PARENT%-*c", FS_WIDTH_CHARS - 8, ' ');
				break;

			case ENTRY_DIRECTORY:
				snprintf(fs_text[i], FS_WIDTH_CHARS, FONT_ICON_DRAWER " %-*s", FS_WIDTH_CHARS - 2, list_entry->file_name);
				break;

			case ENTRY_VOLUME:
				{
					const char* dev_name = pt1210_file_dev_name_from_vol_name(list_entry->file_name);
					if (dev_name)
					{
						/* Format the line so we get the device name in brackets at the end */
						char dev[MAX_FILE_NAME_LENGTH];
						snprintf(dev, sizeof(dev), "(%s:)", dev_name);
						snprintf(fs_text[i], FS_WIDTH_CHARS, FONT_ICON_VOLUME " %-*s %s",
							FS_WIDTH_CHARS - 12,
							list_entry->file_name, dev
						);
					}
					else
						snprintf(fs_text[i], FS_WIDTH_CHARS, FONT_ICON_VOLUME " %-*s", FS_WIDTH_CHARS - 2, list_entry->file_name);
					break;
				}

			case ENTRY_ASSIGN:
				snprintf(fs_text[i], FS_WIDTH_CHARS, FONT_ICON_ASSIGN " %-*s", FS_WIDTH_CHARS - 2, list_entry->file_name);
				break;

			case ENTRY_FILE:
				{
					char* file_name_ptr = list_entry->file_name;
					size_t len = strlen(file_name_ptr);

					/* Adjust start of string pointer and length value to clip off prefix/suffix */
					if (len > 4)
					{
						if (has_mod_prefix(file_name_ptr))
						{
							file_name_ptr += 4;
							len -= 4;
						}

						/* Handle 'MOD..MOD' case gracefully */
						if (len > 4 && has_mod_suffix(file_name_ptr, len))
							len -= 4;
					}

					/* Format file selector row text without file name prefix/suffix */
					snprintf(fs_text[i], FS_WIDTH_CHARS, "%-*.*s %-u", FS_WIDTH_CHARS - 5, len, file_name_ptr, list_entry->bpm);
				}
				break;

			default:
				break;
		}

		/* Uppercase the list entry */
		strupr(fs_text[i]);
	}

	/* If the parent dir entry is all we have, show the no mods message */
	if (!show_volumes && pt1210_file_count == 1)
	{
		ST_Type(fs_text[0], &pt1210_fs_bitplane[FS_TEXT_OFFSET], 0);
		pt1210_fs_draw_error(error_no_modules);
		return;
	}

	ST_Type(fs_text[0], &pt1210_fs_bitplane[FS_TEXT_OFFSET], FS_HEIGHT_CHARS - 1);
}

static void clear_copper()
{
	/* Clear colors in selector copper list */
	for (size_t i = 0; i < FS_HEIGHT_CHARS; ++i)
	{
		size_t index = i * 8 + 3;

		/* Skip over copper instructions that deal with line >255 */
		if (i > 15)
			index += 2;

		selectaline[index] = 0;
	}
}

static void update_copper()
{
	if (!pt1210_file_count)
		return;

	/* Poke highlight color into correct row of copper list */
	size_t index = list_pos * 8 + 3;

	/* Skip over copper instructions that deal with line >255 */
	if (list_pos > 15)
		index += 2;

	selectaline[index] = 0x00F;
}

void pt1210_fs_set_sort(file_sort_key_t new_key)
{
	if (sort_key != new_key)
	{
		sort_key = new_key;
		sort_descending = false;
	}
	else
		sort_descending = !sort_descending;

	pt1210_file_sort_list(sort_key, sort_descending);
	draw_list();
}

void pt1210_fs_select()
{
	file_list_t* selection = &pt1210_file_list[current];

	switch (selection->type)
	{
		case ENTRY_FILE:
			pt1210_file_load_module(current);
			break;

		case ENTRY_DIRECTORY:
		case ENTRY_ASSIGN:
		case ENTRY_VOLUME:
		{
			/* Attempt to change directory */
			if (!pt1210_file_change_dir(selection->file_name))
				return;

			/* Rescan directory */
			show_volumes = false;
			pt1210_fs_rescan(false);
			break;
		}

		case ENTRY_PARENT:
			pt1210_fs_parent();
			break;

		default:
			break;
	}
}

void pt1210_fs_rescan(bool refresh)
{
	/* Disable VBlank */
	pt1210_gfx_enable_vblank_server(false);

	/* Reset file selector indices */
	current = 0;
	list_pos = 0;

	/* Clear text/graphics buffers */
	clear_list();
	clear_copper();
	pt1210_fs_draw_error("READING FOLDER");

	/* Regenerate file list */
	if (show_volumes)
		pt1210_file_gen_volume_list();
	else
		pt1210_file_gen_file_list(refresh);
	pt1210_file_sort_list(sort_key, sort_descending);

	/* Redraw list display */
	draw_list();
	update_copper();

	/* Re-enable VBlank */
	pt1210_gfx_enable_vblank_server(true);
}

void pt1210_fs_move(int32_t offset)
{
	int32_t new_current = clamp(current + offset, 0, pt1210_file_count - 1);
	if (new_current == current)
		return;

	current = new_current;
	list_pos = min(clamp(list_pos + offset, 0, FS_HEIGHT_CHARS - 1), pt1210_file_count - 1);

	draw_list();
	clear_copper();
	update_copper();
}

void pt1210_fs_parent()
{
	if (show_volumes)
		return;

	/* If no parent, we're at the root, so show volumes */
	if (!pt1210_file_parent_dir())
		show_volumes = true;

	pt1210_fs_rescan(false);
}

void pt1210_fs_draw_avail_ram()
{
	uint32_t avail_chip = 0;
	uint32_t avail_fast = 0;
	avail_chip = AvailMem(MEMF_CHIP) / 1024;
	avail_fast = AvailMem(MEMF_FAST) / 1024;

	char avail_buffer[FS_TITLE_CHARS];

	snprintf(avail_buffer, sizeof(avail_buffer), avail_template, avail_chip, avail_fast);

	UI_TypeTitle(avail_buffer, strlen(avail_buffer));
}

void pt1210_fs_draw_title()
{
	char title_buffer[FS_TITLE_CHARS + 1];

	const char* mod_title = pt1210_file_get_module_title();
	if (!mod_title)
		return;

	snprintf(title_buffer, sizeof(title_buffer), "%-*s", FS_TITLE_CHARS, mod_title);

	UI_TypeTitle(title_buffer, FS_TITLE_CHARS);
}

void pt1210_fs_draw_error(const char* error_message)
{
	/* For centering the error message */
	size_t pad_len = (FS_WIDTH_CHARS - strlen(error_message)) / 2;

	char error_buffer[FS_WIDTH_CHARS * 5];
	snprintf(error_buffer, sizeof(error_buffer), error_template,
		' ',
		pad_len, ' ', error_message, pad_len, ' ',
		' '
	);

	ST_Type(error_buffer, &pt1210_fs_bitplane[FS_ERROR_MSG_OFFSET], 4);
}

bool pt1210_fs_find_next(char key, size_t* index)
{
	/* Start at 1 to skip current entry */
	for (size_t i = 1; i < pt1210_file_count; ++i)
	{
		/* Find forward, wrapping around to the start */
		size_t j = (current + i) % pt1210_file_count;
		file_list_t* entry = &pt1210_file_list[j];

		size_t offset = 0;
		if (has_mod_prefix(entry->file_name))
			offset = 4;

		/* Compare uppercase to uppercase */
		char first = toupper(entry->file_name[offset]);

		if (first == key)
		{
			*index = j;
			return true;
		}
	}

	return false;
}

void pt1210_fs_on_char_input(char character)
{
	/* Ignore non-printable characters */
	if (!isprint(character))
		return;

	/* Uppercase our character */
	character = toupper(character);

	/* Find the first matching item in the file list and move to it */
	size_t index;
	if (pt1210_fs_find_next(character, &index))
		pt1210_fs_move(index - current);
}

void pt1210_fs_on_disk_change()
{
	/* If a disk was inserted or removed, and we're showing the volume list, redraw */
	if (!show_volumes)
		return;

	pt1210_fs_rescan(false);
}
