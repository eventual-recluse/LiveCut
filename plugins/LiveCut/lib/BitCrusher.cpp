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

#include "BitCrusher.h"

BitCrusher::BitCrusher()
: minbits(32)
, maxbits(32)
, startbits(32)
, endbits(32)
, multiplier(pow(2.f,32.f))
, divider(pow(2.f,-32.f))
, minfreq(44100)
, maxfreq(44100)
, startfreq(44100)
, endfreq(44100)
, sr(44100)
, lag(1.f)
, count(0.f)
, memory1(0.f)
, memory2(0.f)
, on(true)
{
}

void BitCrusher::OnBlock(long bar, long sd)
{
  startbits = Math::randomfloat(minbits,maxbits);
  endbits = Math::randomfloat(minbits,maxbits);
  
  startfreq = Math::randomfloat(minfreq,maxfreq);
  endfreq = Math::randomfloat(minfreq,maxfreq);
}

void BitCrusher::OnCut(long cut, long numcuts)
{
  float bits = startbits + (float(cut)/float(numcuts))*endbits;
  multiplier = pow(2.f,bits);
  divider = 1.f/multiplier;
  
  float freq = startfreq + (float(cut)/float(numcuts))*endfreq;
  lag = sr/freq;
}

// 0 - 32
void BitCrusher::SetMinBits(float v){minbits = v;}
void BitCrusher::SetMaxBits(float v){maxbits = v;}

void BitCrusher::SetMinFreqFromNormalized(float v){minfreq = v*sr;}
void BitCrusher::SetMaxFreqFromNormalized(float v){maxfreq = v*sr;}
void BitCrusher::SetMinFreq(float v){minfreq = v;}
void BitCrusher::SetMaxFreq(float v){maxfreq = v;}
void BitCrusher::SetSampleRate(float v){sr = v;}
void BitCrusher::SetOn(bool v){on = v;}
