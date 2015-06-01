/*
 * DermShare Autosegmenter
 *
 * Copyright (c) 2010 University of Pittsburgh
 * Copyright (c) 2011-2015 Carnegie Mellon University
 * All rights reserved.
 *
 * This software is distributed under the terms of the Eclipse Public
 * License, Version 1.0 which can be found in the file named LICENSE.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
 */

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <limits>
#include <unistd.h>
#include <glib.h>
#include <lib_filter.h>

#include "diamond/diamond.h"
#include "hairRemover/hairRemover.h"
#include "segmenter/segmenter.h"
#include "DissimilarityMeasure/EMDInterface.h"

#define TIMEOUT 300

using namespace std;

static void removeHair(ImageMap &images, CDiamondContext &diamond)
{
	cout << endl << "======================"
		 << endl << "Removing hair ..." << endl;
	CHairRemover hr;

	hr.load(images[""]);
	hr.run();

	hr.save(images, diamond);
}


static void segment(ImageMap &images, CDiamondContext &diamond)
{
	cout << endl << "======================"
		 << endl << "Segmentation ..." << endl;
	CSegmenter s;

	s.load(images["hairRemoved"], COLORSPACE_CIE_LAB, COORDINATESYSTEM_POLAR, true);
	s.segment();

	s.save(images, diamond);
}

static void dumpImages(ImageMap &images, const char *base_path)
{
	gchar *dir = g_path_get_dirname(base_path);
	gchar *file = g_path_get_basename(base_path);
	gchar **components = g_strsplit(file, ".", 0);
	int components_len = g_strv_length(components);
	gchar *ext = components[components_len - 1];
	components[components_len - 1] = NULL;
	gchar *prefix = g_strjoinv(".", components);
	g_strfreev(components);
	g_free(file);
	g_free(ext);

	for (ImageMap::iterator it = images.begin();
			it != images.end(); it++) {
		if (it->first == "") {
			continue;
		}
		file = g_strdup_printf("%s/%s_%s.png", dir, prefix,
				it->first.c_str());
		it->second.save(file, 0);
		g_free(file);
	}

	g_free(dir);
	g_free(prefix);
}

static void processOne(ImageMap &images, CDiamondContext &diamond)
{
	alarm(TIMEOUT);
	srand(0);
	removeHair(images, diamond);
	segment(images, diamond);
	alarm(0);
}

struct FilterConfig {
	char *heatmap_attr;
};

static int filter_init(int argc, const char * const *argv, int bloblen,
			const void *blob, const char *name, void **filter_data)
{
	FilterConfig *config = new FilterConfig;
	if (argc != 0) {
		fprintf(stderr, "Bad argument list\n");
		return 1;
	}
	config->heatmap_attr = g_strdup_printf("_filter.%s.heatmap.png", name);
	*filter_data = config;

	return 0;
}

static double filter_eval(lf_obj_handle_t ohandle, void *filter_data)
{
	CDiamondContext diamond;
	FilterConfig *config = (FilterConfig *) filter_data;
	ImageMap images;
	const void *data;
	size_t len;

	if (lf_ref_attr(ohandle, "_rgb_image.rgbimage", &len, &data)) {
		return -numeric_limits<double>::infinity();
	}
	images[""].load(*(const RGBImage *) data);
	processOne(images, diamond);

	// write attributes
	diamond.save(ohandle);

	// set heat map
	vector<uchar> buf;
	images["segmFilled"].save(".png", buf);
	lf_write_attr(ohandle, config->heatmap_attr, buf.size(), &buf[0]);

	return 1;
}

void interactive_eval(const char *base_path)
{
	// load the image
	ImageMap images;
	images[""].load(base_path, 0, 1);

	// run
	CDiamondContext diamond;
	processOne(images, diamond);

	// save
	char *filename = g_strdup_printf("%s.text_attr", base_path);
	diamond.save(filename);
	g_free(filename);
	dumpImages(images, base_path);
}

int main(int argc, char **argv)
{
	ippStaticInit();

	if (argc >= 2 && !strcmp(argv[1], "--filter")) {
		lf_main_double(filter_init, filter_eval);
	} else if (argc == 2) {
		interactive_eval(argv[1]);
	} else {
		cout << "Usage : autosegmenter --filter|<prefix>" << endl;
		return -1;
	}
	return 0;
}
