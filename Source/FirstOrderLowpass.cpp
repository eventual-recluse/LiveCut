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

#include "FirstOrderLowpass.h"
#include <cmath>


FirstOrderLowpass::FirstOrderLowpass()
: lambda(0)
, sr(0)
, lastout(0)
{
}

FirstOrderLowpass::~FirstOrderLowpass()
{
}

void FirstOrderLowpass::SetTimeConstant(float t)
{
  time = t;
  lambda = std::exp(std::log(1.0-0.66)/(0.001*time*sr));
}

void FirstOrderLowpass::SetSampleRate(float samplerate)
{
  sr = samplerate;
  SetTimeConstant(time);
}

void FirstOrderLowpass::clear()
{
  lastout = 0.0f;
}

void FirstOrderLowpass::SetState(float v)
{
  lastout=v;
}
