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
#ifndef __75611f4e5912eed939ce360c67631442__
#define __75611f4e5912eed939ce360c67631442__

#include "project_file.h"
#include "project_config.h"
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>

class ProjectFileParser
{
public:
	ProjectFileParser(const std::string & filename, const ProjectConfigPtr & prjConfig = ProjectConfigPtr());
	~ProjectFileParser();

	inline const ProjectConfigPtr & config() const { return m_ProjectConfig; }

	void parse(const ProjectFilePtr & projectFile);

protected:
	inline const std::string & fileName() const { return m_FileName; }
	inline int tokenLine() const { return m_TokenLine; }

	virtual void reportError(const std::string & message);

private:
	enum class Token : int;

	std::ifstream m_Stream;
	ProjectFile * m_ProjectFile;
	ProjectConfigPtr m_ProjectConfig;
	std::unordered_map<std::string, void (ProjectFileParser::*)()> m_CommandHandlers;
	std::string m_FileName;
	std::string m_ProjectPath;
	std::stringstream m_Buffer;
	std::string m_TokenText;
	Token m_Token;
	int m_CurLine;
	int m_TokenLine;
	int m_LastChar;

	void parseSources();

	Token getToken();

	int getChar();
	void ungetChar();

	ProjectFileParser(const ProjectFileParser &) = delete;
	ProjectFileParser & operator=(const ProjectFileParser &) = delete;
};

#endif
