/*
 * Copyright (C) 2005-2010 Erik Nilsson, software on versionstudio point com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
* Common includes.
* Used as precompiled header.
*/
#pragma once

#define XP_WIN 1 // spidermonkey
#define JS_THREADSAFE 1 // spidermonkey
#define TAGLIB_STATIC 1 // taglib

typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;

/* ace library */
#include <ace/ace.h>

/* boost */
#include <boost/algorithm/string.hpp>
#include <boost/cstdint.hpp>
#include <boost/regex.hpp>

/* standard */
#include <fstream>
#include <map>
#include <list>
#include <stack>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>

/* project specific */
#include "util.h"
#include "version.h"
