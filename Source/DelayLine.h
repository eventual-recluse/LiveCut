/*
 This file is part of Livecut
 Copyright 2003 by Remy Muller.
 
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

#if !defined(_mDelay_line__)
#define _mDelay_line__

#include "float_cast.h"

//-------------------------------------------------------
inline float linear_interpolation(const float* data, unsigned long mask, float pos)
{
	int ipos = lrintf(pos);
	float frac = float(pos-ipos);
  return (1.0f-frac)*data[ipos&mask] + frac*data[(ipos+1)&mask];
}

//-------------------------------------------------------
class DelayLine
{
public:
  DelayLine(float delay=22050, long size=44100)
  {
		int n = int(floor (log(double(size))/log(2.0) + 0.5))+1;
    mMask = 1 ; mMask <<= n; mMask -= 1;
    mSize = mMask+1;
    mpBuffer = NULL;
    mpBuffer = new float[mSize];
    mDelay = delay;
    mWriteIndex = 0;
    clear();
  }
  
  virtual ~DelayLine() 
	{
		if(NULL != mpBuffer) 
			delete[] mpBuffer; 
		mpBuffer = NULL;
	}
  
	void resize(int size)
	{
		int n = int(floor (log(double(size))/log(2.0) + 0.5))+1;
		// when N is a multiple of 2 we choose the next power of 2...
		// not good... cf OLA ou FFTFactory
    mMask = 1 ; mMask <<= n; mMask -= 1;
    mSize = mMask+1;
		if(NULL != mpBuffer) 
			delete[] mpBuffer; 
		mpBuffer = NULL;
    mpBuffer = new float[mSize];
    mWriteIndex = 0;
    clear();
	}
  
  
  inline void set_delay(float delay) {mDelay = delay;}
  
  inline float tick(const float x)
	{
		mpBuffer[mWriteIndex] = x; 
		const float pos = float(mWriteIndex+mSize)-mDelay;
		(++mWriteIndex) &= mMask;
		return (mLastOut = linear_interpolation(mpBuffer,mMask,pos));
	}
	
  inline void replace(const float x[],int pos,int size) 
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
  
	inline void write(const float x[],int pos,int size) 
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
  
  inline void write(const float x)
	{
		mpBuffer[mWriteIndex] = x;
		mWriteIndex++; mWriteIndex &= mMask;
  }
  
  inline float read()
	{
		float tmp = mpBuffer[mWriteIndex];
		mWriteIndex++;mWriteIndex &= mMask;
		return tmp;
	}
  
  inline float tap(const long samples)
	{
		return mpBuffer[(mWriteIndex+mSize-samples)&mMask];
	}
  
  inline float tapL(const float samples)
	{
		const float pos = float(mWriteIndex+mSize)-samples;
		return linear_interpolation(mpBuffer,mMask,pos);
	}
  
  inline float readErase()
	{
		float tmp = mpBuffer[mWriteIndex];
		mpBuffer[mWriteIndex] = 0.0f; //erase automatically after read
		mWriteIndex++; mWriteIndex &= mMask;
		return tmp;
	}
  
  inline void read(float dest[],int pos,int size)
	{
		int p = (mWriteIndex+(pos+mSize)); 
		p    &= mMask;	
		int n = mSize-p;
    
		if(size<n)
		{
			memcpy(dest,mpBuffer+p,size*sizeof(float));
		}
		else
		{
			memcpy(dest,mpBuffer+p,n*sizeof(float));
			memcpy(dest,mpBuffer,(size-n)*sizeof(float));
		}
	}
  
  void clear()
	{
		for(int i=0;i<mSize;i++) 
		{
			mpBuffer[i] = 0.0f;
		}
		mLastOut=0.0f;
	}
	
  float* get_ptr(int pos=0)	{return &(mpBuffer[(mWriteIndex+pos)&mMask]);}
  
  inline float lastOut()  {return mLastOut;}
  
private:
  float* mpBuffer;
  float mLastOut;
  float mDelay;
  long mWriteIndex;
  long mMask;
  long mSize;
};

#endif //_mDelay_line_
