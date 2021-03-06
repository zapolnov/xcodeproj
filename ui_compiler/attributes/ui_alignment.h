/* vim: set ai noet ts=4 sw=4 tw=115: */
//
// Copyright (c) 2014 Nikolay Zapolnov (zapolnov@gmail.com).
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#ifndef __93225bb9bf181ba8e83876676b936577__
#define __93225bb9bf181ba8e83876676b936577__

#include "../../3rdparty/tinyxml/tinyxml.h"
#include <string>

enum UIAlignment
{
	// If you change this values, also change the 'ru.zapolnov.yip.Util' class in 'android-util'.

	UIAlignUnspecified = 0,
	UIAlignLeft = 0x01,
	UIAlignRight = 0x02,
	UIAlignHCenter = 0x03,
	UIAlignTop = 0x10,
	UIAlignBottom = 0x20,
	UIAlignVCenter = 0x30,

	UIAlignTopLeft = UIAlignTop | UIAlignLeft,
	UIAlignTopRight = UIAlignTop | UIAlignRight,
	UIAlignBottomLeft = UIAlignBottom | UIAlignLeft,
	UIAlignBottomRight = UIAlignBottom | UIAlignRight,
	UIAlignCenter = UIAlignHCenter | UIAlignVCenter,

	UIAlignHorizontalMask = 0x03,
	UIAlignVerticalMask = 0x30,
};

UIAlignment uiAlignmentFromString(const std::string & str);

#endif
