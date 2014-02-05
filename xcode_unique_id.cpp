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
#include "xcode_unique_id.h"
#include <ctime>
#include <sstream>

static volatile size_t g_Counter = 1;

XCodeUniqueID::XCodeUniqueID()
{
	m_ID[0] = time(NULL);
	m_ID[1] = reinterpret_cast<size_t>(this) & 0xFFFFFFFFU;
	m_ID[2] = (m_ID[0] + m_ID[1]) ^ 0xCAFEBABE + g_Counter++;
}

XCodeUniqueID::~XCodeUniqueID()
{
}

std::string XCodeUniqueID::toString() const
{
	std::stringstream ss;
	const char * hex = "0123456789ABCDEF";
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 32; j += 4)
			ss << hex[(m_ID[i] >> j) & 0xF];
	}
	return ss.str();
}
