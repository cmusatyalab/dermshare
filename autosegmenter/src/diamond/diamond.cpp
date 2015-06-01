/*
 * DermShare Autosegmenter
 *
 * Copyright (c) 2012-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <lib_filter.h>
#include "diamond.h"

using namespace std;

void CDiamondContext::write_attr(const char *name, const char *value)
{
	_attrs[name] = value;
}

void CDiamondContext::write_attr(const char *name, int value)
{
	stringstream s;
	s << value;
	write_attr(name, s.str().data());
}

void CDiamondContext::write_attr(const char *name, double value)
{
	stringstream s;
	s << value;
	write_attr(name, s.str().data());
}

void CDiamondContext::save(const char *filename)
{
	FILE *fp = fopen(filename, "w");
	if (!fp) {
		return;
	}
	for (map<string, string>::iterator it = _attrs.begin();
			it != _attrs.end(); it++) {
		fprintf(fp, "\"%s\"=\"%s\"\n", it->first.c_str(),
				it->second.c_str());
	}
	fclose(fp);
}

void CDiamondContext::save(lf_obj_handle_t ohandle, bool omit)
{
	for (map<string, string>::iterator it = _attrs.begin();
			it != _attrs.end(); it++) {
		string name = it->first;
		lf_write_attr(ohandle, name.c_str(),
				it->second.size() + 1,
				it->second.c_str());
		if (omit) {
			lf_omit_attr(ohandle, name.c_str());
		}
	}
}
