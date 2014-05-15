/*
 This file is part of Livecut
 Copyright 2004 by Remy Muller.
 
 AnalogDelay can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.
 
 AnalogDelay is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with AnalogDelay; if not, visit www.gnu.org/licenses or write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
 Boston, MA 02111-1307 USA
 */

#ifndef COMB_H
#define COMB_H

#include "BBCutter.h"
#include "DelayLine.h"
#include "FirstOrderLowpass.h"

inline float clip(float x)
{
  return std::max(std::min(x, 1.f), -1.f);
}

class Comb : public BBCutListener
{
	float mindelay,maxdelay,startdelay,enddelay;//ms
	DelayLine dl1,dl2;
	float sr;
	float feedback;
	float delay;
	FirstOrderLowpass lp;
	bool on;
	long type;
public:
	Comb()
	:mindelay(50),maxdelay(50),startdelay(50),enddelay(50)
	,feedback(0.5)
	,type(0) //feedforward
	,sr(44100)
	,delay(2205)
	,dl1(2205,44100),dl2(2205,44100) // 50 ms
	,on(true)
	{
		lp.SetSampleRate(44100);
		lp.SetTimeConstant(40.f); //40 ms
		lp.SetState(50.f);
	}
	virtual void OnBlock(long bar, long sd)
	{
		startdelay = Math::randomfloat(mindelay,maxdelay);
		enddelay = Math::randomfloat(mindelay,maxdelay);
	}
	virtual void OnCut(long cut, long numcuts)
	{
		delay = (startdelay + (float(cut)/float(numcuts))*enddelay)*sr/1000.f;
		if(type==0) // feedforward
		{
			dl1.set_delay(delay);
			dl2.set_delay(delay);
		}
	}
	void SetMinDelay(float v){mindelay = v;}
	void SetMaxDelay(float v){maxdelay = v;}
	void SetType(long v){type = v;}
	void SetFeedBack(float v){feedback = v;}

	void SetSampleRate(float v){sr = v; lp.SetSampleRate(v);}
	void SetOn(bool v){on = v;}
	inline void tick(float &out1,float &out2, const float in1, const float in2)
	{
		if(on)
		{
			if(type==0) // feedforward
			{
				out1 = 0.5f*(in1+dl1.tick(in1)); 
				out2 = 0.5f*(in2+dl2.tick(in2));
			} 
			else // feedback
			{
				// need delay interpolation
				dl1.set_delay(lp.LastOut());
				dl2.set_delay(lp.LastOut());
				lp.tick(delay);
				out1 = clip((0.99f-feedback)*in1 + feedback*dl1.lastOut()); 
				dl1.tick(out1);
				out2 = clip((0.99f-feedback)*in2 + feedback*dl2.lastOut()); 
				dl2.tick(out2);
			}
		} 
		else
		{
			out1 = in1;
			out2 = in2;
		}
	}
};

#endif
