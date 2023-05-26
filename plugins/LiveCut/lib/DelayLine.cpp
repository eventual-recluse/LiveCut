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

#include "DelayLine.h"
#include <cstring>

DelayLine::DelayLine(float delay, long size)
: mpBuffer(0)
, mLastOut(0)
, mDelay(delay)
, mWriteIndex(0)
, mMask(0)
, mSize(0)
{
  int n = int(floor (log(double(size))/log(2.0) + 0.5))+1;
  mMask = 1 ; mMask <<= n; mMask -= 1;
  mSize = mMask+1;
  mpBuffer = new float[mSize];
  clear();
}

DelayLine::~DelayLine()
{
  if(0 != mpBuffer)
    delete[] mpBuffer;
  mpBuffer = 0;
}

void DelayLine::resize(int size)
{
  int n = int(floor (log(double(size))/log(2.0) + 0.5))+1;
  // when N is a multiple of 2 we choose the next power of 2...
  // not good... cf OLA ou FFTFactory
  mMask = 1 ; mMask <<= n; mMask -= 1;
  mSize = mMask+1;
  if(0 != mpBuffer)
    delete[] mpBuffer;
  mpBuffer = 0;
  mpBuffer = new float[mSize];
  mWriteIndex = 0;
  clear();
}


void DelayLine::set_delay(float delay)
{
  mDelay = delay;
}

void DelayLine::replace(const float x[],int pos,int size)
{
  int p = (mWriteIndex+pos); p &= mMask;
  int n = mSize-p;
  if(n>size)
    n = size ;
  else
    n = n;
  
  float *dest = mpBuffer+p;
  int i=0;
  for(;i<n;i++)
  {
    dest[i]   = x[i];
  }
  dest = mpBuffer;
  for(;i<size;i++)
  {
    dest[i-n] = x[i];
  }
}

void DelayLine::write(const float x[],int pos,int size)
{
  int p = (mWriteIndex+pos); p &= mMask;
  int n = mSize-p;
  if(n>size)
    n = size ;
  else
    n = n;
  
  float *dest = mpBuffer+p;
  int i=0;
  for(;i<n;i++)
  {
    dest[i]   += x[i];
  }
  dest = mpBuffer;
  for(;i<size;i++)
  {
    dest[i-n] += x[i];
  }
}


void DelayLine::read(float dest[],int pos,int size)
{
  int p = (mWriteIndex+(pos+mSize));
  p    &= mMask;
  int n = mSize-p;
  
  if(size<n)
  {
    std::memcpy(dest,mpBuffer+p,size*sizeof(float));
  }
  else
  {
    std::memcpy(dest,mpBuffer+p,n*sizeof(float));
    std::memcpy(dest,mpBuffer,(size-n)*sizeof(float));
  }
}

void DelayLine::clear()
{
  for(int i=0;i<mSize;i++)
  {
    mpBuffer[i] = 0.0f;
  }
  mLastOut=0.0f;
}

