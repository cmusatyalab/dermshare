#
# Gemini -- Diagnosis abbreviation to full name/severity mappings
#
# Copyright (c) 2014-2015 Carnegie Mellon University
# All rights reserved.
#
# This software is distributed under the terms of the Eclipse Public
# License, Version 1.0 which can be found in the file named LICENSE.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS SOFTWARE CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT
#

_diagnoses_ = [
    ("Melanoma", [
        ("acralmelanoma",   "Acral melanoma"),
        ("metastaticmm",    "Metastatic melanoma"),
        ("mminv",           "Invasive melanoma"),
        ("mmis",            "Melanoma in situ"),
    ]),
    ("Non-melanoma skin cancer", [
        ("bcc",             "Basal cell carcinoma"),
        ("scc",             "Squamous-cell carcinoma"),
    ]),
    ("Borderline lesions", [
        ("hgdn",            "High grade dysplastic nevus"),
        ("spitzatypical",   "Atypical Spitz nevus"),
    ]),
    ("Benign melanocytic lesion", [
        ("acralnevus",      "Acral nevus"),
        ("bennev",          "Benign nevus"),
        ("blue",            "Blue nevus"),
        ("dmn",             ""),
        ("lentigo",         "Lentigo"),
        ("lgdn",            "Low-grade dysplastic nevus"),
        ("spitznevus",      "Splits nevus, no atypical features"),
    ]),
    ("Other benign lesion", [
        ("angioma",         "Angioma"),
        ("df",              "Dermatofibroma"),
        ("lplk",            "Lichen planus-like keratosis"),
        ("sk",              "Seborrheic keratosis"),
        ("vascular",        "Benign vascular lesion"),
    ]),
    ("Other not classified", [
        ("ak",              ""),
        ("nf",              ""),
        ("other",           "Other"),
        ("scar",            "Scar"),
        ("tattoo",          "Tattoo"),
        ("wart",            "Wart"),
        ("undiagnosed",     "Not clinically diagnosed"),
    ])
]

CLASS_NAMES = [ cls for cls, _ in _diagnoses_ ]
CLASSIFICATION = dict((cls, zip(*diag)[0]) for cls, diag in _diagnoses_)
DESCRIPTION = dict((abbr, full) for _, diag in _diagnoses_ for abbr, full in diag)

MELANOMA = set(
    CLASSIFICATION['Melanoma']
)
SEVERE = set(
    CLASSIFICATION['Melanoma'] +
    CLASSIFICATION['Non-melanoma skin cancer']
)
UNDEFINED = set(
    CLASSIFICATION['Borderline lesions']
)
BENIGN = set(
    CLASSIFICATION['Benign melanocytic lesion'] +
    CLASSIFICATION['Other benign lesion'] +
    CLASSIFICATION['Other not classified']
)

