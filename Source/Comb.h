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

#ifndef LIVECUT_COMB_H
#define LIVECUT_COMB_H

#include "BBCutter.h"
#include "DelayLine.h"
#include "FirstOrderLowpass.h"

inline float clip(float x)
{
  return std::max(std::min(x, 1.f), -1.f);
}

class Comb : public BBCutListener
{
public:
  enum Type
  {
    FeedForward=0,
    FeedBack
  };
  
	Comb();
  
	virtual void OnBlock(long bar, long sd);
	virtual void OnCut(long cut, long numcuts);
  
	void SetMinDelay(float v);
	void SetMaxDelay(float v);
	void SetType(long v);
	void SetFeedBack(float v);
	void SetSampleRate(float v);
	void SetOn(bool v);
  
	inline void tick(float &out1,float &out2, const float in1, const float in2)
	{
		if(on)
		{
			if(type==FeedForward) // feedforward
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
private:
	float mindelay,maxdelay,startdelay,enddelay;//ms
	DelayLine dl1,dl2;
	float sr;
	float feedback;
	float delay;
	FirstOrderLowpass lp;
	bool on;
	long type;
};

#endif
