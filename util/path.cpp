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
#include "path.h"
#include "fmt.h"
#include <stdexcept>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <vector>

#ifndef _WIN32
 #include <unistd.h>
 #include <sys/stat.h>
 #include <sys/types.h>
 #include <pwd.h>
#else
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif
 #include <windows.h>
 #include <shlwapi.h>
 #include <userenv.h>
#endif

std::string pathToNativeSeparators(const std::string & path)
{
  #ifndef _WIN32
	return path;
  #else
	std::string result = path;
	for (char & ch : result)
	{
		if (ch == '/')
			ch = '\\';
	}
	return result;
  #endif
}

std::string pathToUnixSeparators(const std::string & path)
{
  #ifndef _WIN32
	return path;
  #else
	std::string result = path;
	for (char & ch : result)
	{
		if (ch == '\\')
			ch = '/';
	}
	return result;
  #endif
}

const char * pathSeparator()
{
  #ifdef _WIN32
	return "\\";
  #else
	return "/";
  #endif
}

bool pathIsSeparator(char ch)
{
	if (ch == '/')
		return true;

  #ifndef _WIN32
	return false;
  #else
	return ch == '\\';
  #endif
}

bool pathIsWin32DriveLetter(char ch)
{
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}

bool pathIsWin32PathWithDriveLetter(const std::string & path)
{
	return (path.length() >= 2 && path[1] == ':' && pathIsWin32DriveLetter(path[0]));
}

std::string pathGetCurrentDirectory()
{
  #ifndef _WIN32
	std::vector<char> buf(std::max(static_cast<size_t>(PATH_MAX), static_cast<size_t>(2048)));
	if (!getcwd(buf.data(), buf.size()))
	{
		int err = errno;
		throw std::runtime_error(fmt() << "unable to determine current directory: " << strerror(err));
	}
	return buf.data();
  #else
	DWORD size = GetCurrentDirectoryA(0, nullptr);
	if (size == 0)
	{
		DWORD err = GetLastError();
		throw std::runtime_error(fmt() << "unable to determine current directory (code " << err << ").");
	}
	std::vector<char> buf(size);
	DWORD len = GetCurrentDirectoryA(size, buf.data());
	if (len == 0 || len >= size)
	{
		DWORD err = GetLastError();
		throw std::runtime_error(fmt() << "unable to determine current directory (code " << err << ").");
	}
	return std::string(buf.data(), len);
  #endif
}

std::string pathGetUserHomeDirectory()
{
  #ifndef _WIN32
	const char * env = getenv("HOME");
	if (env)
		return env;

	struct passwd * pw = getpwuid(getuid());
	if (pw && pw->pw_dir && pw->pw_dir[0])
		return pw->pw_dir;

	throw std::runtime_error("unable to determine path to the user home directory.");
  #else
	std::string result;
	HANDLE hToken = nullptr;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		DWORD err = GetLastError();
		throw std::runtime_error(fmt() << "unable to determine path to the user home directory (code " << err << ").");
	}
	try
	{
		std::vector<char> buf(MAX_PATH);
		DWORD size = MAX_PATH;
		if (!GetUserProfileDirectoryA(hToken, buf.data(), &size))
		{
			DWORD err = GetLastError();
			throw std::runtime_error(fmt() << "unable to determine path to the user home directory (code " << err << ").");
		}
		result = buf.data();
	}
	catch (...)
	{
		CloseHandle(hToken);
		throw;
	}
	CloseHandle(hToken);
	return result;
  #endif
}

bool pathIsAbsolute(const std::string & path)
{
  #ifndef _WIN32
	if (path.length() >= 1 && path[0] == '~')
		return (path.length() == 1 || pathIsSeparator(path[1]));
	return pathIsSeparator(path[0]);
  #else
	if (PathIsRelativeA(path.c_str()))
		return false;
	return (path.length() > 2 && pathIsWin32PathWithDriveLetter(path) && pathIsSeparator(path[2]));
  #endif
}

std::string pathMakeAbsolute(const std::string & path, const std::string & basePath)
{
  #ifndef _WIN32
	if (path.length() >= 1 && path[0] == '~')
	{
		if (path.length() == 1)
			return pathGetUserHomeDirectory();
		else if (pathIsSeparator(path[1]))
			return pathSimplify(pathConcat(pathGetUserHomeDirectory(), path.substr(2)));
	}
	if (pathIsSeparator(path[0]))
		return pathSimplify(path);
  #else
	if (pathIsWin32PathWithDriveLetter(path) || (path.length() > 0 && pathIsSeparator(path[0])))
		return pathMakeAbsolute(path);
  #endif

	return pathSimplify(pathConcat(basePath, path));
}

std::string pathMakeAbsolute(const std::string & path)
{
  #ifndef _WIN32
	return pathMakeAbsolute(path, pathGetCurrentDirectory());
  #else
	DWORD size = GetFullPathNameA(path.c_str(), 0, nullptr, nullptr);
	if (size == 0)
	{
		DWORD err = GetLastError();
		throw std::runtime_error(fmt() << "unable to determine absolute path for file '" << path 
			<< "' (code " << err << ").");
	}
	std::vector<char> buf(size);
	DWORD len = GetFullPathNameA(path.c_str(), size, buf.data(), nullptr);
	if (len == 0 || len >= size)
	{
		DWORD err = GetLastError();
		throw std::runtime_error(fmt() << "unable to determine absolute path for file '" << path 
			<< "' (code " << err << ").");
	}
	return std::string(buf.data(), len);
  #endif
}

size_t pathIndexOfFirstSeparator(const std::string & path, size_t start)
{
	size_t pos = path.find('/', start);

  #ifdef _WIN32
	size_t pos2 = path.find('\\', start);
	if (pos2 != std::string::npos && (pos == std::string::npos || pos2 < pos))
		pos = pos2;
  #endif

	return pos;
}

std::string pathSimplify(const std::string & path)
{
	std::vector<std::string> parts;
	std::stringstream ss;
	size_t off = 0;

  #ifndef _WIN32
	if (path.length() > 0 && path[0] == '~')
	{
		if (path.length() == 1)
			return path;
		else if (pathIsSeparator(path[1]))
		{
			ss << '~' << pathSeparator();
			off = 2;
		}
	}
	else if (path.length() > 0 && pathIsSeparator(path[0]))
	{
		off = 1;
		ss << pathSeparator();
	}
  #else
	if (path.length() >= 2 && path[0] == path[1] && pathIsSeparator(path[0]))
	{
		off = pathIndexOfFirstSeparator(path, 2);
		if (off == std::string::npos)
			return pathToNativeSeparators(path);
		ss << path.substr(0, off) << pathSeparator();
		++off;
	}
	else if (pathIsWin32PathWithDriveLetter(path))
	{
		off = 2;
		ss << path.substr(0, off);
		if (path.length() > 2 && pathIsSeparator(path[2]))
		{
			++off;
			ss << pathSeparator();
		}
	}
	else if (path.length() > 0 && pathIsSeparator(path[0]))
	{
		off = 1;
		ss << pathSeparator();
	}
  #endif

	for (;;)
	{
		size_t pos = pathIndexOfFirstSeparator(path, off);
		if (pos == std::string::npos)
		{
			std::string part = path.substr(off);
			if (part.length() > 0)
				parts.push_back(part);
			break;
		}

		std::string part = path.substr(off, pos - off);
		off = pos + 1;

		if (part.length() > 0 && part != ".")
		{
			if (part == ".." && parts.size() > 0 && parts.back() != "..")
				parts.pop_back();
			else
				parts.push_back(part);
		}
	}

	const char * prefix = "";
	for (const std::string & part : parts)
	{
		ss << prefix << part;
		prefix = pathSeparator();
	}

	return ss.str();
}

std::string pathConcat(const std::string & path1, const std::string & path2)
{
	if (path1.length() == 0)
		return path2;
	if (path2.length() == 0)
		return path1;

	if (pathIsSeparator(path1[path1.length() - 1]))
		return path1 + path2;
	else
		return path1 + pathSeparator() + path2;
}

size_t pathIndexOfFileName(const std::string & path)
{
	size_t pos = path.rfind('/');

  #ifdef _WIN32
	size_t pos2 = path.rfind('\\');
	if (pos2 != std::string::npos && (pos == std::string::npos || pos2 > pos))
		pos = pos2;
	if (pos == std::string::npos && pathIsWin32PathWithDriveLetter(path))
		pos = 1;
  #endif

	return (pos != std::string::npos ? pos + 1 : 0);
}

std::string pathGetDirectory(const std::string & path)
{
	size_t pos = pathIndexOfFileName(path);
	if (pos > 0)
		--pos;
	return path.substr(0, pos);
}

std::string pathGetFileName(const std::string & path)
{
	return path.substr(pathIndexOfFileName(path));
}

std::string pathGetShortFileExtension(const std::string & path)
{
	size_t pos = path.rfind('.');
	if (pos == std::string::npos)
		return std::string();

	size_t offset = pathIndexOfFileName(path);
	if (pos < offset)
		return std::string();

	return path.substr(pos);
}

std::string pathGetFullFileExtension(const std::string & path)
{
	size_t offset = pathIndexOfFileName(path);
	size_t pos = path.find('.', offset);
	return (pos == std::string::npos ? std::string() : path.substr(pos));
}

bool pathCreate(const std::string & path)
{
	std::string dir = pathMakeAbsolute(path);
	bool result = false;
	size_t off = 0;

  #ifndef _WIN32
	if (pathIsSeparator(dir[0]))
		off = 1;
	else
		throw std::runtime_error(fmt() << "invalid path '" << dir << "'.");
  #else
	if (pathIsWin32PathWithDriveLetter(dir))
		off = 3;
	else if (PathIsUNCA(dir.c_str()))
	{
		off = pathIndexOfFirstSeparator(dir, 2);
		if (off == std::string::npos)
			throw std::runtime_error(fmt() << "invalid path '" << dir << "'.");
		++off;
	}
	else
		throw std::runtime_error(fmt() << "invalid path '" << dir << "'.");
  #endif

	size_t end;
	do
	{
		end = pathIndexOfFirstSeparator(dir, off);
		off = end + 1;

		std::string subdir;
		if (end == std::string::npos)
			subdir = dir;
		else
			subdir = dir.substr(0, end);

	  #ifndef _WIN32
		if (mkdir(subdir.c_str(), 0755) == 0)
			result = true;
		else
		{
			int err = errno;
			if (err != EEXIST)
			{
				throw std::runtime_error(fmt()
					<< "unable to create directory '" << subdir << "': " << strerror(err));
			}
		}
	  #else
		if (CreateDirectoryA(subdir.c_str(), nullptr))
			result = true;
		else
		{
			DWORD err = GetLastError();
			if (err != ERROR_ALREADY_EXISTS)
			{
				throw std::runtime_error(fmt()
					<< "unable to create directory '" << subdir << "' (code " << err << ").");
			}
		}
	  #endif
	}
	while (end != std::string::npos);

	return result;
}

bool pathIsFile(const std::string & path)
{
  #ifndef _WIN32
	struct stat st;
	int err = stat(path.c_str(), &st);
	if (err < 0)
	{
		err = errno;
		if (err == ENOENT)
			return false;
		throw std::runtime_error(fmt() << "unable to stat file '" << path << "': " << strerror(err));
	}
	return S_ISREG(st.st_mode);
  #else
	DWORD attr = GetFileAttributesA(path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES)
	{
		DWORD err = GetLastError();
		if (err = ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND ||
				err == ERROR_INVALID_DRIVE || err == ERROR_BAD_NETPATH)
			return false;
		throw std::runtime_error(fmt()
			<< "unable to get attributes for file '" << path << "' (code " << err << ").");
	}
	return (attr & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_DIRECTORY)) == 0;
  #endif
}
