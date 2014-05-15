/*
 This file is part of Livecut
 Copyright 2003 by Remy Muller.
 
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

#include "Comb.h"

Comb::Comb()
: mindelay(50)
, maxdelay(50)
, startdelay(50)
, enddelay(50)
, feedback(0.5)
, type(FeedForward) //feedforward
, sr(44100)
, delay(2205)
, dl1(2205,44100)
, dl2(2205,44100) // 50 ms
, on(true)
{
  lp.SetSampleRate(44100);
  lp.SetTimeConstant(40.f); //40 ms
  lp.SetState(50.f);
}

void Comb::OnBlock(long bar, long sd)
{
  startdelay = Math::randomfloat(mindelay,maxdelay);
  enddelay = Math::randomfloat(mindelay,maxdelay);
}

void Comb::OnCut(long cut, long numcuts)
{
  delay = (startdelay + (float(cut)/float(numcuts))*enddelay)*sr/1000.f;
  if(type==FeedForward) // feedforward
  {
    dl1.set_delay(delay);
    dl2.set_delay(delay);
  }
}

void Comb::SetMinDelay(float v)
{
  mindelay = v;
}

void Comb::SetMaxDelay(float v)
{
  maxdelay = v;
}

void Comb::SetType(long v)
{
  type = v;
}

void Comb::SetFeedBack(float v)
{
  feedback = v;
}

void Comb::SetSampleRate(float v)
{
  sr = v;
  lp.SetSampleRate(v);
}

void Comb::SetOn(bool v)
{
  on = v;
}
