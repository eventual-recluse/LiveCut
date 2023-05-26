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

#if !defined(_mDelay_line__)
#define _mDelay_line__

#include <math.h>

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
  DelayLine(float delay=22050, long size=44100);
  ~DelayLine();
  
	void resize(int size);
  
  void set_delay(float delay);
  
  inline float tick(const float x)
	{
		mpBuffer[mWriteIndex] = x; 
		const float pos = float(mWriteIndex+mSize)-mDelay;
		(++mWriteIndex) &= mMask;
		return (mLastOut = linear_interpolation(mpBuffer,mMask,pos));
	}
	
  void replace(const float x[],int pos,int size);
	void write(const float x[],int pos,int size);

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
  
  void read(float dest[],int pos,int size);
  
  void clear();
  
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
