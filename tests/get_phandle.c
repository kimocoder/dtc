/*
 * libfdt - Flat Device Tree manipulation
 *	Testcase for fdt_get_phandle()
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <libfdt.h>

#include "tests.h"
#include "testdata.h"

static void check_phandle(void *fdt, const char *path, uint32_t checkhandle)
{
	int offset;
	uint32_t phandle;

	offset = fdt_path_offset(fdt, path);
	if (offset < 0)
		FAIL("Couldn't find %s", path);

	phandle = fdt_get_phandle(fdt, offset);
	if (phandle != checkhandle)
		FAIL("fdt_get_phandle(%s) returned 0x%x instead of 0x%x\n",
		     path, phandle, checkhandle);
}

static void check_phandle_unique(const void *fdt, uint32_t checkhandle)
{
	uint32_t phandle;
	int offset = -1;

	while (true) {
		offset = fdt_next_node(fdt, offset, NULL);
		if (offset < 0) {
			if (offset == -FDT_ERR_NOTFOUND)
				break;

			FAIL("error looking for phandle %#x", checkhandle);
		}

		phandle = fdt_get_phandle(fdt, offset);

		if (phandle == checkhandle)
			FAIL("generated phandle already exists");
	}
}

int main(int argc, char *argv[])
{
	uint32_t max, phandle;
	void *fdt;
	int err;

	test_init(argc, argv);
	fdt = load_blob_arg(argc, argv);

	check_phandle(fdt, "/", 0);
	check_phandle(fdt, "/subnode@2", PHANDLE_1);
	check_phandle(fdt, "/subnode@2/subsubnode@0", PHANDLE_2);

	err = fdt_find_max_phandle(fdt, &max);
	if (err < 0)
		FAIL("fdt_find_max_phandle returned %d instead of 0\n", err);

	if (max != PHANDLE_2)
		FAIL("fdt_find_max_phandle found 0x%x instead of 0x%x", max,
		     PHANDLE_2);

	max = fdt_get_max_phandle(fdt);
	if (max != PHANDLE_2)
		FAIL("fdt_get_max_phandle returned 0x%x instead of 0x%x\n",
		     max, PHANDLE_2);

	err = fdt_generate_phandle(fdt, &phandle);
	if (err < 0)
		FAIL("failed to generate phandle: %d", err);

	check_phandle_unique(fdt, phandle);

	PASS();
}
