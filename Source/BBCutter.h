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

#ifndef BBCUTTER_H
#define BBCUTTER_H

#include <valarray>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <numeric>

//-------------------------------------------------------------------------------
struct Math
{
	static inline long  randominteger(long min, long max)
	{
    return long(0.5000001+randomfloat(min,max));
  }
	
	static inline double randomfloat(double min , double max)
	{
    return min + (max-min)*double(rand())/double(RAND_MAX);
  }
  
  static inline void randomseed(long seed)
  {
    srand(seed);
  }
  
  template<class T>
  static inline T wchoose(T *values,double *weights,long size)
  {
    std::vector<double> cumsum(size);
    cumsum[0] = *weights;
    for(int i=1;i<cumsum.size();++i)
      cumsum[i] = cumsum[i-1] + weights[i];
    
    double v = randomfloat(0.0,cumsum.back());
    long index;
    for(index=0;index<size;++index)
      if(v<cumsum[index])
        break;
    return values[index];
  }
	
  static inline float clip(const float x,const float mn,const float mx)
  {
    return std::min(std::max(x,mn),mx);
  }
};

static const float pi_4 = atan(1.f);

struct CutInfo
{
	long size;
	long length; //<= size
	float pan;
	float amp;
	float cents;
	// bit crusher
	float bits;
	float sr;
	CutInfo();
};

enum CutId
{
  kCutProc11=0,
  kWarpCut,
  kSQPusher,
  kAll,
  kNumCutProcs
};

/**
 @brief base class for cut procedures
 */
class CutProc
{
public:
	CutProc();
  virtual ~CutProc();
  
	void SetMinAmp(float v);
	void SetMaxAmp(float v);
	void SetMinPan(float v);
	void SetMaxPan(float v);
	void SetMinDetune(float v);
	void SetMaxDetune(float v);
	void SetDutyCycle(float v);
	void SetFillDutyCycle(float v);
  void SetMinPhraseLength(long v);
  void SetMaxPhraseLength(long v);
  
  virtual long ChoosePhraseLength();
  virtual void ChooseCuts(std::vector<CutInfo> &cuts,
                          long &unitsinblock,
                          long unitsdone,
                          long totalunits,
                          long subdiv,
                          double spu);
  
protected:
  float minamp,maxamp,minpan,maxpan,mindetune,maxdetune,dutycycle,filldutycycle;
  long maxphraselength,minphraselength;
};

//-------------------------------------------------------------------------------
class CutProc11 : public CutProc
{
public:
	CutProc11();
  
	void SetStutterChance(float v);
	void SetStutterArea(float v);
	void SetMinRepeats(long v);
	void SetMaxRepeats(long v);
  
	void ChooseCuts(std::vector<CutInfo> &cuts,
                  long &unitsinblock,
                  long unitsdone,
                  long totalunits,
                  long subdiv,
                  double spu);

private:
  long ChooseRepeats();
	long ChooseUnitsInCut(long subdiv);
  
private:
	float stutterchance,stutterarea;
	long minrepeats, maxrepeats;
};

//-------------------------------------------------------------------------------
class WarpCutProc : public CutProc
{
public:
	WarpCutProc();
  
	void SetStraightChance(float chance);
	void SetRegularChance(float chance);
	void SetRitardChance(float chance);
	void SetAccel(float v);
  
	void ChooseCuts(std::vector<CutInfo> &cuts,
                  long &unitsinblock,
                  long unitsdone,
                  long totalunits,
                  long subdiv,
                  double spu);
  
	long ChooseRepeats(float beatsinblock);
	long ChooseBlockSize();
  
private:
	float straightchance, regularchance, ritardchance, accel;
};

//-------------------------------------------------------------------------------
class SQPusherCutProc : public CutProc
{
public:
  SQPusherCutProc();
  void SetActivity(float v);
  
  
  long ChoosePhraseLength();
	void ChooseCuts(std::vector<CutInfo> &cuts,
                  long &unitsinblock,
                  long unitsdone,
                  long totalunits,
                  long subdiv,
                  double spu);

private:
  void sqpusherinit();

private:
  typedef std::vector< std::vector< std::vector<double> > > CutSequence;
  double activity;
  bool fill;
  long fillnumber,fillpos;
  CutSequence cs;
};

//-------------------------------------------------------------------------------
/*
 the BBCutter has to notify about phrase, blocks and units
 the player has to notify about cuts
 */
class BBCutListener
{
public:
	BBCutListener() { }
	virtual ~BBCutListener() { }
	virtual void OnPhrase(long bar, long sd) { }
	virtual void OnBlock(long bar, long sd) { }
	virtual void OnUnit(long bar, long sd) { }
	virtual void OnCut(long cut, long numcuts) { }
};

class ListenerManager
{
	std::vector<BBCutListener*> listeners;
public:
	ListenerManager();
	
  void OnPhrase(long bar, long sd);
	void OnBlock(long bar, long sd);
  void OnUnit(long bar, long sd);
  void OnCut(long cut, long numcuts);
	
  void RegisterListener(BBCutListener *l);
};

static float trapeze(long i,long fade, long size)
{
  const long fadein  = (i<fade);
  const long fadeout = (i>=(size-fade));
  const float env = fadein                * (i)
  + (1-fadein)*(1-fadeout)* (fade);
  +  fadeout              * (fade-i);
  return float(env)/float(fade);
}

static float expenv(float i,float fade,float size)
{
  return (1-exp(-5*i/fade)) * (1-exp(5*(i-size)/fade));
}

class LivePlayer
{
public:
	LivePlayer();
	void SetListenerManager(ListenerManager *lm);
  void SetFade(float v);
  void OnBlock(std::vector<CutInfo> newcuts);

  inline bool tick(float &outL, float &outR,const float inL,const float inR)
  {
    if(currentcut<cuts.size())
    {
      if(inputindex<inputbufferL.size()) //store input
      {
        inputbufferL[inputindex] = inL;
        inputbufferR[inputindex] = inR;
        inputindex++;
      }
      
      if(readindex<cuts[currentcut].length) //dutycycle on
      {
        const float l = (*L)[readindex];
        const float r = (*R)[readindex];
        //rotation matrix
        const float env = expenv(readindex,fade,cuts[currentcut].length);
        
        outL = env*(ll*l + rl*r);
        outR = env*(lr*l + rr*r);
        readindex++;
      }
      else                             //dutycycle off
      {
        readindex++;
        outL = 0.f;
        outR = 0.f;
      }
      
      if(readindex>=cuts[currentcut].size) //end of cut
      {
        currentcut++;
        readindex = 0;
        if(currentcut>=cuts.size())
        {
          outL = 0.f;
          outR = 0.f;
          return false;
        }
				const float amp = cuts[currentcut].amp;
				const float pan = cuts[currentcut].pan;
        ll = amp * ((pan<0)? 1.f :  cos(pan*2*pi_4));
        lr = amp * ((pan<0)? 0.f :  sin(pan*2*pi_4));
        rl = amp * ((pan>0)? 0.f : -sin(pan*2*pi_4));
        rr = amp * ((pan>0)? 1.f :  cos(pan*2*pi_4));
        
        if(fabs(cuts[currentcut].cents) > 1e-10)
        {
          //resample
          const double ratio = pow(2.f,cuts[currentcut].cents/1200.f);
          const long sz = std::min(   long(pitchedbufferL.size()),
                                   (long(double(inputbufferL.size())/ratio)-1));
          for(int i=0;i<sz;++i)
          {
            const long pos = long(float(i)*ratio);
            const float frac = float(i)*ratio-float(pos);
            const float a = inputbufferL[pos]*(1-frac)+inputbufferL[pos+1]*frac;
            const float b = inputbufferR[pos]*(1-frac)+inputbufferR[pos+1]*frac;
            pitchedbufferL[i] = a;
            pitchedbufferR[i] = b;
          }
          L = &pitchedbufferL;
          R = &pitchedbufferR;
        }
        // tell cut-synchrone effects
        if(listenermanager)
          listenermanager->OnCut(currentcut,cuts.size()); // allow interpolation...
        
        return true;
      }
    }
    else
    {
      outL = 0.f;
      outR = 0.f;
    }
    
		return false;
  }
  
private:
	std::valarray<float> inputbufferL;
	std::valarray<float> inputbufferR;
	std::valarray<float> pitchedbufferL;
	std::valarray<float> pitchedbufferR;
	std::valarray<float> *L;
	std::valarray<float> *R;
  long currentcut;
  long inputindex,readindex;
  float ll,lr,rl,rr;
  long fade;
  std::vector<CutInfo> cuts;
	ListenerManager *listenermanager;
};

//------------------------------------------------------------------------------------------------
inline float ms2samples(float t,float sr)
{
  return (t*0.001f)*sr;
}

class BBCutter
{
public:
	BBCutter(LivePlayer &player);
  ~BBCutter();
  
	void RegisterListener(BBCutListener *l);
	void	SetCutProc(long i);
	
  void	SetBarsInSample(long bars) ;
	void	SetTempo(double v) ;
	void	SetSubdiv(long v) ;
	void	SetStutterChance(float chance);
	void	SetStutterArea(float area);
  void	SetMaxRepeats(long repeats);
	void	SetMinRepeats(long repeats);
	void	SetStraightChance(float chance);
	void	SetRegularChance(float chance);
	void	SetRitardChance(float chance);
	void	SetAccel(float v);
  void	SetActivity(float v);
  void	SetFade(float v);
  void	SetMinPhraseLength(long v);
	void	SetMaxPhraseLength(long v);
	void	SetMinAmp(float v);
	void	SetMaxAmp(float v);
	void	SetMinPan(float v);
	void	SetMaxPan(float v);
	void	SetDutyCycle(float v);
	void	SetFillDutyCycle(float v);
	void	SetMinDetune(float v);
	void	SetMaxDetune(float v);
	void	SetNumerator(double v);
	void	SetDenominator(double v);
	
  void	SetSampleRate(double v);
	void	SetTimeInfos(double bpm,double num,double den,double srate);
	void	UpdateRates();
  
	inline long	  GetUnitPosition() { return slicestart+unitsinsideblock;}
	inline double UnitsPerBar(double subdiv,double numerator,double denominator) { return subdiv*numerator/denominator;}
	inline double BeatsPerSecond(double tempo) { return tempo/60.0;}
	inline double SamplesPerBeat(double sr,double tempo) { return sr/BeatsPerSecond(tempo);}
	inline double SamplesPerBar() { return SamplesPerBeat(sr,tempo)*beatsPerBar;}
	inline double SamplesPerUnit() { return SamplesPerBar()/double(subdiv);}
  
	void	Phrase(long bar, long sd);
  void	Block(long bar,long sd);
  void	Unit(long bar, long sd);
	void	SetPosition(long bar, long sd);
  
private:
	// params
	double	tempo, sr;
	long	subdiv;
	double	numerator,denominator,beatsPerBar;
  
	// states
	long	unitsdone, totalunits, barsnow,
  unitsinblock, unitsinsideblock,	barsinsample,slicestart;
  
	CutProc11 cutproc11;
	WarpCutProc warpcutproc;
	SQPusherCutProc sqpusher;
	CutProc	*strategy;
  std::vector<CutProc *> procs;
	ListenerManager listenermanager;
	LivePlayer	&player;
};

#endif

