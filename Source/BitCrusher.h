/*
 This file is part of Livecut
 Copyright 2004 by Remy Muller.
 
 Livecut can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.
 
 Livecut is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Livecut; if not, visit www.gnu.org/licenses or write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
 Boston, MA 02111-1307 USA
 */

#ifndef BIT_CRUSHER_H
#define BIT_CRUSHER_H

/*
	BBCut MultiFX:

	* Bitcrusher
	* Samplerate reduction (sr)
	merge the 2 in one

	* Comb filter (feeback, freq Hz)
*/

#include "BBCutter.h"

class BitCrusher : public BBCutListener
{
	float minbits,maxbits,startbits,endbits;
	float multiplier,divider;

	float minfreq,maxfreq,startfreq,endfreq;
	float sr;
	float lag,count;
	float memory1,memory2;

	bool on;
public:
	BitCrusher()
	:minbits(32),maxbits(32),startbits(32),endbits(32)
	,multiplier(pow(2.f,32.f)),divider(pow(2.f,-32.f))
	,minfreq(44100),maxfreq(44100),startfreq(44100),endfreq(44100)
	,sr(44100)
	,lag(1.f)
	,count(0.f)
	,memory1(0.f),memory2(0.f)
	,on(true)
	{}
	virtual void OnBlock(long bar, long sd)
	{
		startbits = Math::randomfloat(minbits,maxbits);
		endbits = Math::randomfloat(minbits,maxbits);

		startfreq = Math::randomfloat(minfreq,maxfreq);
		endfreq = Math::randomfloat(minfreq,maxfreq);
	}
	virtual void OnCut(long cut, long numcuts)
	{
		float bits = startbits + (float(cut)/float(numcuts))*endbits;
		multiplier = pow(2.f,bits);
		divider = 1.f/multiplier;

		float freq = startfreq + (float(cut)/float(numcuts))*endfreq;
		lag = sr/freq;
	}
	// 0 - 32
	void SetMinBits(float v){minbits = v;}
	void SetMaxBits(float v){maxbits = v;}

	void SetMinFreqFromNormalized(float v){minfreq = v*sr;}
	void SetMaxFreqFromNormalized(float v){maxfreq = v*sr;}
	void SetMinFreq(float v){minfreq = v;}
	void SetMaxFreq(float v){maxfreq = v;}
	void SetSampleRate(float v){sr = v;}
	void SetOn(bool v){on = v;}
	inline void tick(float &out1,float &out2, const float in1, const float in2)
	{
		if(on)
		{
			if(count>lag) 
			{
				// it also add jitter we should interpolate instead, 
				// but eh it's a bitcrusher!
				memory1 = floor(in1*multiplier)*divider;
				memory2 = floor(in2*multiplier)*divider;
				while(count>lag) 
					count -= lag;
			}
			count += 1.f;
			out1 = memory1;
			out2 = memory2;
		} // if(on)
		else
		{
			out1 = in1;
			out2 = in2;
		}
	}
};

#endif
