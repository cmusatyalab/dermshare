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
 * Terrain Synthesis by Example: Terrain Analysis\n
 * @file PPA.h
 *
 * Primary Authors: Howard Zhou (howardz@cc.gatech.edu)\n
 *
 *
 * ==================================================================\n
 * Profile-recognition and Polygon-breaking Algorithm
 *
 * @version 1.0\n
 *     Initial Revision
 * @date 2005-04-20
 * @author Howard Zhou
 *
 *
 * @todo
 * @bug
 * @warning 
 *
 * ===================================================================\n
 */

#ifndef ___HAIR_REMOVER__H___
#define ___HAIR_REMOVER__H___

// #pragma warning(disable : 4786)

#include "IPPInterface/IPPImage.h"
#include "inpainting/inpainter.h"
#include "diamond/diamond.h"

/// @namespace using standard namespace
using namespace std;

// ==================================================================================
/**
 * @class CHairRemover
 * @brief Profile-recognition and Polygon-breaking Algorithm
 * @ingroup Terrain Analysis
 *
 * @par requirements
 *
 * @par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 *
 * 
 * @version 1.0\n
 *     Initial Revision
 * @date 2005-04-20
 * @author Howard Zhou
 *
 * 
 * @todo
 * @bug
 * @warning
 */
// ================================================================================

class CHairRemover
{
protected:
	CIPPImage<Ipp8u> _image8u;
	CIPPImage<Ipp8u> _mask8u;
	CIPPImage<Ipp8u> _result8u;

	bool _isValid;
	bool _isInitialized;
	bool _isProcessed;

public:
	// ------------------------------------------------------
	// constructors and destructors
	// ------------------------------------------------------
	/// default constructor
	CHairRemover()
		:_isValid(false),_isInitialized(false),_isProcessed(false)
	{};
	
	// use member initialization list.
	//CHairRemover(const CIPPImage<Ipp32f> &image)
	//	:_image32f(image),
	//	_isValid(image.isInitialized())
	//{};

	/// copy constructor
	//CHairRemover(const CHairRemover<T> & p){}
		
	/// destructor
	~CHairRemover(){};

	// ------------------------------------------------------
	// Predicators
	// ------------------------------------------------------
	bool isValid		()const { return _isValid;		};
	bool isInitialized	()const { return _isInitialized;};
	bool isProcessed	()const { return _isProcessed;	};

	// ------------------------------------------------------
	// Input, Output, Test
	// ------------------------------------------------------
	bool load(const char* filename);
	void load(const CIPPImage<Ipp8u> &image);
	void save(ImageMap &images, CDiamondContext &diamond);

	// ------------------------------------------------------
	// Other operation
	// ------------------------------------------------------
	CIPPImage<Ipp32f> extractThinStructureMorph(const CIPPImage<Ipp8u> &image8u) const;		
	CIPPImage<Ipp8u> getMask(const CIPPImage<Ipp32f> &image32f, float threshold = 10.f) const;
	
	void run();
};


inline bool CHairRemover::load(const char* filename)
{
	assert(filename);
	IppStatus status = _image8u.load(filename, 0, 1);
	
	if(status != ippStsNoErr)
	{
		_isValid = false;
		return false;
	}

	//_result8u = _image8u;
	_isValid = true;

	return true;
}


inline void CHairRemover::load(const CIPPImage<Ipp8u> &image)
{
	_image8u = image;
	_isValid = true;
}


#endif // ___HAIR_REMOVER__H___
