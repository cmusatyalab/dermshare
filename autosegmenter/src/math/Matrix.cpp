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

/** =================================================================\n
 *
 * MathLib\n
 * @file Matrix.cpp
 *
 * Primary Author: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * general matrix class
 *
 * @version 1.2\n
 *     1. added template for Vecotr class
 *     2. replaced all division by scaler with multiplication by it's inverse
 *     3. removed .cpp file, all function definition are in .h file now
 * @date 2005-12-16
 * @author howardz
 *
 * @version 1.1\n
 *     Major organization change, moved most of function defintion to\n
 *     the .cpp file to make class declaration more clear.
 * @date 2003-05-14
 * @author howardz
 *
 * @version 1.0\n
 *     Initial Revision
 * @date 2003-02-23
 * @author howardz
 *
 *
 * @todo Implement det() and invert() 
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#include "Matrix.h"

