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

#ifndef guard_bandwidthtracker_h
#define guard_bandwidthtracker_h

#include <ace/high_res_timer.h>

/**
* BandwidthTracker.
* Used for measuring used bandwidth.
*/
class BandwidthTracker
{
public:
	/**
	* Default constructor.
	* @return instance
	*/
	BandwidthTracker() : m_averageBandwidth(0),
		m_bytesToMeasure(0),
		m_totalBytes(0)
	{
		m_lastMeasuredTime = ACE_High_Res_Timer::gettimeofday().msec();
	}

    /**
     * Measure the bandwidth since the last call to measureBandwidth.
     */
	void measureBandwidth();

    /**
     * Measure the bandwidth since the last call to measureBandwidth.
	 * @param bytes the amount of bytes to append to the measurement
     */
	void measureBandwidth(int bytes);

	/**
	* Get the average measured bandwidth in kb per second.
	* @return the average measured bandwidth
	*/
	int getAverageBandwidth() {
		return m_averageBandwidth;
	}

	/**
	* Get the total bytes measured.
	* @return the total bytes measured
	*/
	uint64_t getTotalBytes() {
		return m_totalBytes;
	}

private:
	uint64_t m_totalBytes;

	int m_averageBandwidth;

	unsigned long m_lastMeasuredTime;

	int m_bytesToMeasure;
};

#endif
