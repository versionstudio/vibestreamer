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

#include "common.h"
#include "bandwidthtracker.h"

void BandwidthTracker::measureBandwidth()
{
	unsigned long currentTime = ACE_High_Res_Timer::gettimeofday().msec();
	unsigned long diffTime = currentTime-m_lastMeasuredTime;

	if ( m_bytesToMeasure>0 ) {
		m_averageBandwidth = (double)m_bytesToMeasure/(double)diffTime*1.024;
	}
	else {
		m_averageBandwidth = 0;
	}

	if ( diffTime>1000 ) {
		m_bytesToMeasure = 0;
		m_lastMeasuredTime = currentTime;
	}
}

void BandwidthTracker::measureBandwidth(int bytes)
{
	measureBandwidth();

	m_bytesToMeasure += bytes;
	m_totalBytes += bytes;
}
