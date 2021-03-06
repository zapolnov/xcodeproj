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

#include "project.h"
#include "yip_directory.h"
#include "platform.h"
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>

class ProjectFileParser
{
public:
	static void parse(const ProjectPtr & project, const std::string & filename, bool resolveImports);
	static void parseFromCurrentDirectory(const ProjectPtr & project, bool resolveImports);
	static void parseFromGit(const ProjectPtr & project, const std::string & name, const GitRepositoryPtr & repo,
		Platform::Type platform = Platform::All);
	static void parseFromGit(const ProjectPtr & project, const std::string & url,
		Platform::Type platform = Platform::All);

protected:
	inline const std::string & fileName() const { return m_FileName; }
	inline int tokenLine() const { return m_TokenLine; }

	virtual void reportWarning(const std::string & message);
	virtual void reportError(const std::string & message);

private:
	enum class Token : int;

	struct ImageSize;
	struct Error;

	std::ifstream m_Stream;
	Project * m_Project;
	std::unordered_map<std::string, void (ProjectFileParser::*)()> m_CommandHandlers;
	std::string m_FileName;
	std::string m_PathPrefix;
	std::string m_ProjectPath;
	std::stringstream m_Buffer;
	std::string m_TokenText;
	Platform::Type m_DefaultPlatformMask;
	Token m_Token;
	int m_CurLine;
	int m_TokenLine;
	int m_LastChar;
	bool m_ResolveImports;

	ProjectFileParser(const std::string & filename, const std::string & pathPrefix = std::string(),
		Platform::Type platform = Platform::All);
	~ProjectFileParser();

	void doParse(const ProjectPtr & project, bool resolveImports);

	void parseProjectName();
	void parseSources();
	void parseAppSources();
	void parsePublicHeaders();
	void parseDefines();
	void parseAppDefines();
	void parseImport();
	void parseResources();
	void parseResourcesDir();
	void parseAppResources();
	void parseWinRT();
	void parseTizen();
	void parseIOSorOSX();
	void parseAndroid();
	void parseLicense();
	void parseToDo();
	void parseTranslationFile();

	Platform::Type parsePlatformMask();
	Platform::Type parsePlatformName();

	void parsePlatformOrBuildTypeMask(Platform::Type & platforms, BuildType::Value & buildTypes);
	void parseFileFlags(const SourceFilePtr & sourceFile,
		const SourceFilePtr & sourceFile2 = SourceFilePtr(), bool isPublicHeader = false);

	Token getToken();

	int getChar();
	void ungetChar();

	Project::ImageSize validateImageSize(unsigned width, unsigned height, std::initializer_list<ImageSize> sizes);

	ProjectFileParser(const ProjectFileParser &) = delete;
	ProjectFileParser & operator=(const ProjectFileParser &) = delete;
};

#endif
