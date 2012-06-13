                         ______
         ________ ______/      \ ___________ ____________
 ........\_      |      \__ ___/     __     \     ______/.........................................
 ::       |      |      |      |      |    _/    ____/___                                       ::
 ::       |      |      |      |      _     \     |      \                                      ::
 ::    __ |      |      |      |      |      \    |       \ __ _                                ::
 ::    \ \\      |      |      |      |      /    |       // ///                                ::
 ::     \_\\___________/|______|____________/____________//_///                                 ::
 ::     _______   _____    __________ _________________________ __________________________      ::
 ::    /   ___/__/    |___ \    _    \    _____/    _    \_    |      /   _____/\    _    \     ::
 ::    \____     \    ___/_/    |   _/   ___/___    |     /   \_/    /   ___/___/    |   _/     ::
 ::__ /    |      \   |     \   _     \   |     \   _     \    |     \    |     \    _     \    ::
 :/ //     |      /   |      \  |      \  |      \  |      \   |      \   |      \   |      \_ _::
 :\ \\     |     /    |      /  |      /  |      /  |      /   |      /   |      /   |  sox  ///::
 ::\_\\_________/___________/___|_____/_________/___|_____/____|_____/__________/____|______/// ::
 ::                                                                                             ::
 ::.............................................................................................::
 
 Copyright (C) 2005-2010 Erik Nilsson, software on versionstudio point com
 Flash player design by Marcus Rydling

 Website: http://www.vibestreamer.com
 Github: http://www.github.com/versionstudio/vibestreamer

================================================
 File structure
================================================

./client        Contains client projects.
                Includes Ext web client and
                Flash player project for Adobe CS3.
                See "build-tools" for build script.
			 
./installer     Contains NSIS installer scripts.

./lib           Contains compiled libraries for
                use with MSVC 2003.

./src           Solution files for MSVC 2003
                can be found here.
				
./src/jsengine  Script engine project.

./src/server    Server core project.

./src/sqlite3x  Sqlite3x: API wrapper for SQLite.

./src/tinyxml   TinyXML: Simple XML parser.

./src/utf8      UTF8-CPP: portable UTF-8 library.

./src/win32     Win32 GUI project.
                Don't mind the mess
                and the hungarian notation :P

================================================
 Dynamically linked libraries
================================================

Note: These libraries has to be downloaded and
included for compilation. Library files provided
with the "lib/win32/release" folder, as well as
all DLL files in the "win32" project, have
been compiled with MSVC 2003.

When compiling for Win32 it's important that all
libraries are compiled with the /MT runtime option.

* ACE (5.7.0 or higher)
  http://www.cse.wustl.edu/~schmidt/ACE.html
  Compiled with the following defines:
  #define ACE_HAS_STANDARD_CPP_LIBRARY 1
  #define ACE_NO_INLINE
  
* ACE SSL (Part of ACE)
  Requires OpenSSL when building.
  
* SQLite (3.6.20 or higher)
  http://www.sqlite.org/

* SpiderMonkey (1.7.0 or higher)
  http://www.mozilla.org/js/spidermonkey/
  Needs to be compiled with multi-thread
  support (JS_THREADSAFE=1). This requires
  the Netscape Portable Runtime (NSPR) when building.
  An easier solution is to use the "JavaScript Mini Netscape Portable Runtime"
  when building (http://www.reteksolutions.com/sections/oss/jsmininspr.asp)
  
* OpenSSL (0.9.8k or higher)

================================================
 Statically linked libraries
================================================

Note: TinyXml, SQLite3x and UTF8-CPP are shipped as sub-projects.

* Boost (1.40 or higher)
  Must be compiled without compiler option: "/zc:wchar_t"

* SQLite3x (2005-06-16 or newer)
  http://www.int64.org/sqlite.html
  This Sqlite3x source has custom modifications.
  For details see the modified.txt file.
  
* TagLib (1.6.1 or higher)
  http://developer.kde.org/~wheeler/taglib.html
  
* TinyXml (2.5.3 or higher)
  http://www.grinninglizard.com/tinyxml
  
* UTF8-CPP (2.2.4 or higher)
  http://utfcpp.sourceforge.net
  This UTF8-CPP source has custom modifications.
  For details see the modified.txt file.