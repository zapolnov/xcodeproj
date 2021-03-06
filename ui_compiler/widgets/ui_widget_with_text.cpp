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
#include "ui_widget_with_text.h"
#include "../../util/tinyxml-util/tinyxml-util.h"
#include "../parse_util.h"

UIWidgetWithText::UIWidgetWithText(UILayout * layout, UIGroup * parentGroup, Kind kind)
	: UIWidget(layout, parentGroup, kind),
	  m_TextColor(UIColor::white),
	  m_FontScaleMode(UIScaleDefault),
	  m_LandscapeFontScaleMode(UIScaleDefault),
	  m_HasFontScaleMode(false)
{
}

UIWidgetWithText::~UIWidgetWithText()
{
}

bool UIWidgetWithText::parseAttribute(const TiXmlAttribute * attr)
{
	if (attr->NameTStr() == "text")
	{
		m_Text = attr->ValueStr();
		return true;
	}
	else if (attr->NameTStr() == "textColor")
	{
		m_TextColor = UIColor::fromAttr(attr);
		return true;
	}
	else if (attr->NameTStr() == "font")
	{
		m_Font = UIFont::fromAttr(attr);
		return true;
	}
	else if (attr->NameTStr() == "fontScale")
	{
		uiScaleModeFromAttr(attr, &m_FontScaleMode, &m_LandscapeFontScaleMode);
		m_HasFontScaleMode = true;
		return true;
	}

	return UIWidget::parseAttribute(attr);
}

void UIWidgetWithText::afterParseAttributes(const TiXmlElement * element)
{
	if (!m_Font && m_HasFontScaleMode)
		throw std::runtime_error(xmlError(element, "font scale mode is useless without specifying a font."));

	UIWidget::afterParseAttributes(element);
}
