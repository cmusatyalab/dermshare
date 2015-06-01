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

#ifndef __DIAMOND_H__
#define __DIAMOND_H__

#include <string>
#include <map>
#include <lib_filter.h>
#include "IPPInterface/IPPImage.h"

class CDiamondContext
{
private:
	std::map<std::string, std::string> _attrs;

public:
	void write_attr(const char *name, const char *value);
	void write_attr(const char *name, int value);
	void write_attr(const char *name, double value);
	void save(const char *filename);
	void save(lf_obj_handle_t ohandle, bool omit=false);
};


typedef std::map<std::string, CIPPImage<Ipp8u> > ImageMap;

#endif
