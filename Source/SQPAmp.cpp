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

#include "SQPAmp.h"

SQPAmp::SQPAmp()
: on(true)
, amp(1.f)
{
}

void SQPAmp::OnSemiQuaver(long semi)
{
  // proba of amp=1 on semiquaver
  static double amptemplate[]=
  {
    1.0,  0,    0.09, 0.06,
    0.24, 0.03, 0.15, 0.06,
    0.21, 0.03, 0.12, 0.09,
    0.24, 0.21, 0.18, 0.21
  };
  const float random = Math::randomfloat(0.0,1.0);
  const float proba = amptemplate[semi];
  amp = (random<proba)? 0.f : 1.f;
}

void SQPAmp::SetOn(bool v)
{
  on = v;
}
