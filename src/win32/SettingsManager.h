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

#ifndef guard_settingsmanager_h
#define guard_settingsmanager_h

#include "singleton.h"

class SettingsManager : public Singleton<SettingsManager>
{
public:
	static const std::string AUTOSTART;
	static const std::string CHECKFORUPDATES;
	static const std::string CONFIRMEXIT;
	static const std::string MINIMIZETOTRAY;
	static const std::string UPNPENABLED;

	void load();
	void save();

	void restoreDefaults();

	const bool getBool(const std::string name) {
		return Util::ConvertUtil::toBool(getString(name));
	}

	const int getInt(const std::string name) {
		return Util::ConvertUtil::toInt(getString(name));
	}

	const std::string getString(const std::string name) 
	{
		std::map<std::string,std::string>::iterator iter_find = m_settings.find(name);
		if ( iter_find!=m_settings.end() ) {
			return iter_find->second;
		}

		return "";
	}

	void setBool(const std::string name,const bool value) {
		setString(name,Util::ConvertUtil::toString(value));
	}

	void setInt(const std::string name,const int value) {
		setString(name,Util::ConvertUtil::toString(value));
	}

	void setString(const std::string name,const std::string value) {
		m_settings[name] = value;
	}

	void setDefaultBool(const std::string name,const bool value) {
		setDefaultString(name,Util::ConvertUtil::toString(value));
	}

	void setDefaultInt(const std::string name,const int value) {
		setDefaultString(name,Util::ConvertUtil::toString(value));
	}

	void setDefaultString(const std::string name,const std::string value) {
		if ( m_settings.find(name)==m_settings.end() ) {
			m_settings[name] = value;
		}
	}

private:
	void loadDefaults();

	std::map<std::string,std::string> m_settings;
};

#endif
