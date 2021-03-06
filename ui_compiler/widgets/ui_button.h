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
#ifndef __f7c700c34a2d02ea09d0d3a63e537db2__
#define __f7c700c34a2d02ea09d0d3a63e537db2__

#include "ui_widget_with_text.h"
#include "../attributes/ui_image.h"
#include <vector>
#include <sstream>

class UIButton : public UIWidgetWithText
{
public:
	UIButton(UILayout * layout, UIGroup * parentGroup);
	~UIButton();

	inline const UIImagePtr & image() const { return m_Image; }
	inline const UIImagePtr & backgroundImage() const { return m_BackgroundImage; }

	// In ios_generator.cpp
	void iosGenerateInitCode(const ProjectPtr & project, const std::string & prefix, std::stringstream & ss,
		bool isViewController) override;
	void iosGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

	// In android_generator.cpp
	void androidGenerateInitCode(const ProjectPtr & project, const std::string & prefix,
		std::stringstream & ss, std::map<std::string, std::string> & translations) override;
	void androidGenerateLayoutCode(const std::string & prefix, std::stringstream & ss) override;

protected:
	const char * iosDefaultClassName() const override { return "NZButton"; }
	const char * androidDefaultClassName() const override { return "android.widget.Button"; }

	bool parseAttribute(const TiXmlAttribute * attr) override;

private:
	bool m_ImageOnRightSide;
	UIImagePtr m_Image;
	UIImagePtr m_BackgroundImage;

	UIButton(const UIButton &) = delete;
	UIButton & operator=(const UIButton &) = delete;
};

#endif
