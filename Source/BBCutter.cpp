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

#include "BBCutter.h"

//-------------------------------------------------------------------------------
CutInfo::CutInfo()
: size(0)
, length(0)
, pan(0.f)
, amp(1.f)
, cents(0.f)
{
}

CutProc::CutProc()
: minamp(1.f)
, maxamp(1.f)
, minpan(0.5f)
, maxpan(0.5f)
, mindetune(0.f)
, maxdetune(0.f)
, dutycycle(1.f)
, filldutycycle(1.f)
, maxphraselength(4)
, minphraselength(1)
{
}

CutProc::~CutProc() {}

void CutProc::SetMinAmp(float v) { minamp = v;}
void CutProc::SetMaxAmp(float v) { maxamp = v;}
void CutProc::SetMinPan(float v) { minpan = v;}
void CutProc::SetMaxPan(float v) { maxpan = v;}
void CutProc::SetMinDetune(float v) { mindetune = v;}
void CutProc::SetMaxDetune(float v) { maxdetune = v;}
void CutProc::SetDutyCycle(float v) { dutycycle = v;}
void CutProc::SetFillDutyCycle(float v) { filldutycycle = v;}
void CutProc::SetMinPhraseLength(long v) { minphraselength = v;}
void CutProc::SetMaxPhraseLength(long v) { maxphraselength = v;}

long CutProc::ChoosePhraseLength()
{
  return Math::randominteger(minphraselength,maxphraselength);
}

void CutProc::ChooseCuts(std::vector<CutInfo> &cuts,
                         long &unitsinblock,
                         long unitsdone,
                         long totalunits,
                         long subdiv,
                         double spu)
{
}

//-------------------------------------------------------------------------------
long CutProc11::ChooseRepeats()
{
  return Math::randominteger(minrepeats,maxrepeats);
}

long CutProc11::ChooseUnitsInCut(long subdiv)
{	// units = 2*x+1 , x is natural // 1 <= 2*x + 1 <= subdiv/2+1
  return 2*long(Math::randomfloat(0.f, float(subdiv)/4.f)+0.5f) + 1;
}

CutProc11::CutProc11()
: stutterchance(1.f)
, stutterarea(0.5f)
, minrepeats(1)
, maxrepeats(2)
{
}

void CutProc11::SetStutterChance(float v) { stutterchance = v;}
void CutProc11::SetStutterArea(float v) { stutterarea = v;}
void CutProc11::SetMinRepeats(long v) { minrepeats = v+1;} //not repeats actually but occurences
void CutProc11::SetMaxRepeats(long v) { maxrepeats = v+1;}

void CutProc11::ChooseCuts(std::vector<CutInfo> &cuts,
                           long &unitsinblock,
                           long unitsdone,
                           long totalunits,
                           long subdiv,
                           double spu)
{
  const long unitsleft = totalunits-unitsdone;
  long unitsincut = 1;
  long repeats = 1;
  
  if( float(unitsleft)/float(subdiv) < stutterarea &&
     Math::randomfloat(0.0, 1.0) < stutterchance) //end of phrase stutter
  {
    long numbers[] = { 1, 2, 3, 4, 6, 8 };
    double probs[]  = { 0.4, 0.3, 0.1, 0.1, 0.05, 0.05 };
    long multiplier = Math::wchoose(numbers,probs,6);
    repeats=unitsleft*multiplier;
    double unitsinthiscut= 1.0/multiplier;
    unitsinblock = repeats*unitsinthiscut;
    cuts.resize(repeats);
    const float startpan    = Math::randomfloat(minpan,maxpan);
    const float endpan      = Math::randomfloat(minpan,maxpan);
    const float startamp    = Math::randomfloat(minamp,maxamp);
    const float endamp      = Math::randomfloat(minamp,maxamp);
    const float startdetune = 0.f;
    const float enddetune   = Math::randomfloat(mindetune,maxdetune);
    
    for(int i=0;i<repeats;i++)
    {
      const float phase = float(i)/float(repeats);
      const long cutlength = long(unitsinthiscut*spu);
      cuts[i].size = cutlength;
      cuts[i].length = long(float(cutlength)*filldutycycle);
      cuts[i].pan = startpan + (endpan-startpan)*phase;
      cuts[i].amp = startamp + (endamp-startamp)*phase;
      cuts[i].cents = startdetune + (enddetune-startdetune)*phase;
    }
  }
  else
  {
    unitsincut	= ChooseUnitsInCut(subdiv);
    repeats = ChooseRepeats();
    
    while(unitsincut>unitsleft) {unitsincut -= 2;}
    if(unitsincut<0)			 {unitsincut = 0;}
    
    while((repeats*unitsincut)>unitsleft)
    {
      if(repeats>1)
      {
        repeats--;
      }
      else if(unitsleft<=(subdiv/2+1))
      {
        unitsincut=unitsleft;
        repeats=1;
      }
      else
      {
        unitsincut=1; //shouldn't happen
        repeats=1;
      }
    }
    unitsinblock = repeats*unitsincut;
    cuts.resize(repeats);
    for(int i=0;i<repeats;i++)
    {
      cuts[i].size = long(unitsincut*spu);
      //quantize cut dutycycle to match cuts to units
      //cuts[i].length = long(double(std::max(long(dutycycle*unitsincut),1L))*spu);
      cuts[i].length = long(dutycycle*unitsincut*spu);
      cuts[i].amp = Math::randomfloat(minamp,maxamp);
    }
  }
}

//-------------------------------------------------------------------------------
long WarpCutProc::ChooseRepeats(float beatsinblock)
{
  long repeatsarray[] = {4,8,16,32};
  
  if(beatsinblock<1.f)
    return repeatsarray[Math::randominteger(0,2)];
  else
    return repeatsarray[Math::randominteger(1,3)];
}

long WarpCutProc::ChooseBlockSize()
{
  long blockarray[] = {1,2,4};
  double blockprobs[]  = {0.5,0.4,0.1};
  return Math::wchoose(blockarray,blockprobs,3);
}

WarpCutProc::WarpCutProc()
: straightchance(0.5)
, regularchance(0.7)
, ritardchance(0.6)
, accel(0.9)
{
}

void WarpCutProc::SetStraightChance(float chance) { straightchance = chance;}
void WarpCutProc::SetRegularChance(float chance) { regularchance = chance;}
void WarpCutProc::SetRitardChance(float chance) { ritardchance = chance;}
void WarpCutProc::SetAccel(float v) { accel = v;}

void WarpCutProc::ChooseCuts(std::vector<CutInfo> &cuts,
                             long &unitsinblock,
                             long unitsdone,
                             long totalunits,
                             long subdiv,
                             double spu)
{
  const long unitsleft = totalunits-unitsdone;
  long repeats = 1;
  unitsinblock = ChooseBlockSize();
  if(unitsinblock>unitsleft)
    unitsinblock = unitsleft;
  
  if(Math::randomfloat(0.0, 1.0)< straightchance)
  {
    double temp = double(unitsinblock)/double(repeats);
    cuts.resize(repeats);
    for(int i=0;i<repeats;i++)
    {
      long l = long(spu*temp);
      cuts[i].size = l;
      //quantize cut dutycycle to match cuts to units
      cuts[i].length = long(double(std::max(long(dutycycle*temp),1L))*spu);
      cuts[i].amp = Math::randomfloat(minamp,maxamp);
    }
  }
  else
  {
    repeats = ChooseRepeats(float(unitsinblock)/float(subdiv));
    const float startpan    = Math::randomfloat(minpan,maxpan);
    const float endpan      = Math::randomfloat(minpan,maxpan);
    const float startamp    = Math::randomfloat(minamp,maxamp);
    const float endamp      = Math::randomfloat(minamp,maxamp);
    const float startdetune = 0.f; //Math::randomfloat(mindetune,maxdetune);
    const float enddetune   = Math::randomfloat(mindetune,maxdetune);
    
    if(Math::randomfloat(0.0, 1.0)< regularchance)
    {
      //long repeatsarray[] = {4,6,8,12,16,18,24,32};
      //repeats = repeatsarray[Math::randominteger(0,7)];
      double temp = double(unitsinblock)/double(repeats);
      cuts.resize(repeats);
      for(int i=0;i<repeats;i++)
      {
        const float phase = float(i)/float(repeats);
        long l = long(spu*temp+0.5);
        cuts[i].size = l;
        cuts[i].length = long(float(l)*filldutycycle);
        cuts[i].pan = startpan + (endpan-startpan)*phase;
        cuts[i].amp = startamp + (endamp-startamp)*phase;
        cuts[i].cents = startdetune + (enddetune-startdetune)*phase;
      }
    }
    else //accel
    {
      double temp = unitsinblock*(1.0-double(accel))/(1.0-pow(double(accel),double(repeats)));
      cuts.resize(repeats);
      for(int i=0;i<repeats;i++)
      {
        const float phase = float(i)/float(repeats);
        long l = long(spu*temp*(pow(double(accel),double(i))));
        cuts[i].size = l;
        cuts[i].length = long(float(l)*filldutycycle);
        cuts[i].pan = startpan + (endpan-startpan)*phase;
        cuts[i].amp = startamp + (endamp-startamp)*phase;
        cuts[i].cents = startdetune + (enddetune-startdetune)*phase;
      }
      if(Math::randomfloat(0.0, 1.0)< ritardchance)
        std::reverse(cuts.begin(),cuts.end());
    }
  }
}

//-------------------------------------------------------------------------------
SQPusherCutProc::SQPusherCutProc()
: activity(0.1)
, fill(false)
, fillnumber(0)
, fillpos(0)
{
  sqpusherinit();
}

void SQPusherCutProc::SetActivity(float v)
{
  activity=v;
}

void SQPusherCutProc::sqpusherinit()
{
  //boring manual initialisation
  cs.resize(13);
  
  cs[0].resize(2);
  cs[0][0].push_back(0.75);cs[0][0].push_back(0.75);cs[0][0].push_back(0.75);cs[0][0].push_back(0.75);
  cs[0][1].push_back(1.0);
  
  cs[1].resize(3);
  cs[1][0].push_back(0.5);cs[1][0].push_back(1.0);
  cs[1][1].push_back(1.0);
  cs[1][2].push_back(1.0);cs[1][2].push_back(0.5);
  
  cs[2].resize(3);
  cs[2][0].push_back(0.5);
  cs[2][1].push_back(1.0); cs[2][1].push_back(1.0); cs[2][1].push_back(1.0);
  cs[2][2].push_back(0.5);
  
  cs[3].resize(5);
  cs[3][0].push_back(0.571429 );
  cs[3][1].push_back(0.571429 );  cs[3][1].push_back(0.571429 );
  cs[3][2].push_back(0.571429 );  cs[3][2].push_back(0.571429 );
  cs[3][3].push_back(0.571429 );
  cs[3][4].push_back(0.285714 );  cs[3][4].push_back(0.285716 );
  
  cs[4].resize(3);
  cs[4][0].push_back(1.0); cs[4][0].push_back(0.5);
  cs[4][1].push_back(1.0); cs[4][1].push_back(0.5);
  cs[4][2].push_back(0.5); cs[4][2].push_back(0.5);
  
  cs[5].resize(3);
  cs[5][0].push_back(0.5);cs[5][0].push_back(0.5);
  cs[5][1].push_back(0.66);cs[5][1].push_back(0.67); cs[5][1].push_back(0.67);
  cs[5][2].push_back(1.0);
  
  cs[6].resize(4);
  cs[6][0].push_back(0.34);
  cs[6][1].push_back(0.33);  cs[6][1].push_back(0.33);
  cs[6][2].push_back(0.34);  cs[6][2].push_back(0.33);
  cs[6][1].push_back(2.33);
  
  cs[7].resize(4);
  cs[7][0].push_back(1.4);
  cs[7][1].push_back(0.4);cs[7][1].push_back(0.4);
  cs[7][2].push_back(0.6);cs[7][2].push_back(0.2);
  cs[7][3].push_back(1.0);
  
  cs[8].resize(3);
  cs[8][0].push_back(0.167);cs[8][0].push_back(0.167);cs[8][0].push_back(0.166);
  cs[8][1].push_back(1.0);cs[8][0].push_back(1.0);cs[8][0].push_back(1.0);
  cs[8][0].push_back(0.5);
  
  cs[9].resize(2);
  cs[9][0].push_back(1.5);  cs[9][0].push_back(0.5);  cs[9][0].push_back(1.0);
  cs[9][1].push_back(0.25); cs[9][1].push_back(0.25); cs[9][1].push_back(0.25); cs[9][1].push_back(0.25);
  
  cs[10].resize(4);
  cs[10][0].push_back(0.2);cs[10][0].push_back(0.2);
  cs[10][1].push_back(0.4);cs[10][1].push_back(0.4);
  cs[10][2].push_back(0.4);cs[10][2].push_back(0.4);
  cs[10][3].push_back(2.0);
  
  cs[11].resize(2);
  cs[11][0].push_back(0.75);cs[11][0].push_back(0.75);cs[11][0].push_back(1.0);
  cs[11][1].push_back(0.25);cs[11][1].push_back(0.25);cs[11][1].push_back(0.25);cs[11][1].push_back(0.25);cs[11][1].push_back(0.25);cs[11][1].push_back(0.25);
  
  cs[12].resize(5);
  cs[12][0].push_back(0.5);cs[12][0].push_back(1.0);
  cs[12][1].push_back(0.5);
  cs[12][2].push_back(0.125);cs[12][2].push_back(0.125);cs[12][2].push_back(0.125);cs[12][2].push_back(0.125);
  cs[12][3].push_back(1.0);
  cs[12][4].push_back(0.167); cs[12][4].push_back(0.167); cs[12][4].push_back(0.166);
}

long SQPusherCutProc::ChoosePhraseLength()
{
  fill = false;
  return CutProc::ChoosePhraseLength();
}

void SQPusherCutProc::ChooseCuts(std::vector<CutInfo> &cuts,
                                 long &unitsinblock,
                                 long unitsdone,
                                 long totalunits,
                                 long subdiv,
                                 double spu)
{
  const long unitsleft = subdiv - (unitsdone%subdiv); //one bar at a time
  double done = double(unitsdone)/double(subdiv);
  long phrase  = long(done);                   //{0,1,2,3} bars
  long barpos  = long(done*4.0) % 4;           //{0,1,2,3} beats
  long beatpos = long(done*16.0)%4;           //position inside beat {0,1,2,3} semiquaver
  long quaver = long(done*8.0) % 8;	         //quaver inside bar
  //quaver = (long(done*8.0)*2) % 8;
  double barprop = double(barpos)/4.0;
  double sqweights[]= {0.0, 0.3, 0.0, 0.5, 0.7, 0.8, 0.9, 0.6};
  double sqchance = sqweights[quaver]*activity;
  double spb = spu*double(subdiv)/4.0;        //samplesperbeat
  
  if((totalunits-unitsdone) == subdiv)
  {
    fill = true;
    fillnumber = Math::randominteger(0,12);
    fillpos=0;
  }
  
  if(fill==true)
  {
    if(fillpos<cs[fillnumber].size())
    {
      cuts.resize(cs[fillnumber][fillpos].size());
      double beatsdone = 0.0;
      for(int i=0;i<cuts.size();i++)
      {
        const float phase = float(i)/float(cuts.size());
        beatsdone += cs[fillnumber][fillpos][i];
        long l = long(spb * cs[fillnumber][fillpos][i]);
        cuts[i].size = l;
        cuts[i].length = long(spb * cs[fillnumber][fillpos][i] * filldutycycle);
        cuts[i].pan = Math::randomfloat(minpan,maxpan);
        cuts[i].amp = Math::randomfloat(minamp,maxamp);
        cuts[i].cents = Math::randomfloat(mindetune,maxdetune);
      }
      unitsdone = long(double(subdiv)*beatsdone/4.0);
      if(unitsinblock>unitsleft)
        unitsinblock=unitsleft; //will automatically interrupt cutsequence before its end
      fillpos++;
      return;
    }
    else
    {
      fill = false;
      fillpos=0;
    }
  }
  
  {
    long temp = 1 + 2*Math::randominteger(0,1);
    if(beatpos == 2)
      temp=1;
    
    unitsinblock = long(double(temp*8.0)/double(subdiv));
    if(unitsinblock>unitsleft)
      unitsinblock=unitsleft; //will automatically interrupt cutsequence before its end
    
    if(Math::randomfloat(0.0,1.0) < sqchance) // 2*temp semiquaver
    {
      cuts.resize(temp*2);
      for(int i=0;i<cuts.size();i++)
      {
        long l = long(0.25*spb);
        cuts[i].size = l;
        cuts[i].pan = Math::randomfloat(minpan,maxpan);
        cuts[i].amp = Math::randomfloat(minamp,maxamp);
        cuts[i].cents = Math::randomfloat(mindetune,maxdetune);
        cuts[i].length = long(0.25*spb*dutycycle);
      }
    }
    else            // or temp quaver i.e same duration
    {
      cuts.resize(temp);
      for(int i=0;i<cuts.size();i++)
      {
        long l = long(0.5*spb);
        cuts[i].size = l;
        cuts[i].amp = Math::randomfloat(minamp,maxamp);
        cuts[i].cents = Math::randomfloat(mindetune,maxdetune);
        cuts[i].length = long(0.5*spb*dutycycle);
      }
    }
  }
}

//-------------------------------------------------------------------------------

ListenerManager::ListenerManager()	{}

void ListenerManager::OnPhrase(long bar, long sd)
{
  for(int i=0;i<listeners.size();++i) {
    listeners[i]->OnPhrase(bar,sd);
  }
}

void ListenerManager::OnBlock(long bar, long sd)
{
  for(int i=0;i<listeners.size();++i)
  {
    listeners[i]->OnBlock(bar,sd);
  }
}

void ListenerManager::OnUnit(long bar, long sd)
{
  for(int i=0;i<listeners.size();++i)
  {
    listeners[i]->OnUnit(bar,sd);
  }
}

void ListenerManager::OnCut(long cut, long numcuts)
{
  for(int i=0;i<listeners.size();++i)
  {
    listeners[i]->OnCut(cut,numcuts);
  }
}

void ListenerManager::RegisterListener(BBCutListener *l)
{
  if(l)
    listeners.push_back(l);
}

//------------------------------------------------------------------------
LivePlayer::LivePlayer()
: currentcut(0)
, inputindex(0)
, readindex(0)
, ll(0.f)
, lr(0.f)
, rl(0.f)
, rr(0.f)
, fade(0)
, L(&inputbufferL)
, R(&inputbufferR)
, listenermanager(NULL)
{
}

void LivePlayer::SetListenerManager(ListenerManager *lm)
{
  listenermanager = lm;
}

void LivePlayer::SetFade(float v)
{
  if(v<1.f) v = 1.f ; //0.1f;
  fade = v;
}

void LivePlayer::OnBlock(std::vector<CutInfo> newcuts)
{
  if(!newcuts.empty())
  {
    cuts = newcuts;
    //rotation matrix
    // [ll lr]
    // [rl rr]
    const float pan = cuts[0].pan;
    const float amp = cuts[0].amp;
    ll = amp * ((pan<0)? 1.f :  cos(pan*2*pi_4));
    lr = amp * ((pan<0)? 0.f :  sin(pan*2*pi_4));
    rl = amp * ((pan>0)? 0.f : -sin(pan*2*pi_4));
    rr = amp * ((pan>0)? 1.f :  cos(pan*2*pi_4));
    inputindex = readindex = 0;
    currentcut=0;
    
    // tell cut-synchrone effects
    if(listenermanager)
      listenermanager->OnCut(currentcut,cuts.size()); // allow interpolation...
    
    
    long maxcutlength=0;
    for(int i=0;i<cuts.size();i++)
      if(cuts[i].size>maxcutlength)
        maxcutlength = cuts[i].size;
    
    inputbufferL.resize(maxcutlength,0.f);
    inputbufferR.resize(maxcutlength,0.f);
    pitchedbufferL.resize(maxcutlength,0.f);
    pitchedbufferR.resize(maxcutlength,0.f);
    L = &inputbufferL;
    R = &inputbufferR;
  }
}

//------------------------------------------------------------------------------------------------
#include <algorithm>
#include <functional>
#include <cassert>
using std::for_each;
using std::mem_fun;
using std::bind2nd;

BBCutter::BBCutter(LivePlayer &player)
: player(player)
, tempo(180)
, sr(44100)
, subdiv(8)
, numerator(4)
, denominator(4)
, beatsPerBar(4.0*numerator/double(denominator))
//states
, unitsdone(0)
, totalunits(0)
, barsnow(0)
, unitsinblock(0)
, unitsinsideblock(0)
, barsinsample(1)
, slicestart(0)
, strategy(&cutproc11)
{
  procs.reserve(kNumCutProcs);
  procs.push_back(&cutproc11);
  procs.push_back(&warpcutproc);
  procs.push_back(&sqpusher);
  
  player.SetListenerManager(&listenermanager);
}

 BBCutter::~BBCutter()
{
}

void BBCutter::RegisterListener(BBCutListener *l)
{
  listenermanager.RegisterListener(l);
}

void	BBCutter::SetCutProc(long i)
{
  assert(i>=0 && i<procs.size());
  if(i>=0 && i<procs.size())
  {
    strategy = procs[i];
  }
}

void	BBCutter::SetBarsInSample(long bars) { barsinsample = bars; UpdateRates(); }
void	BBCutter::SetTempo(double v) { tempo=v; UpdateRates(); }
void	BBCutter::SetSubdiv(long v) { subdiv = v; UpdateRates(); }
void	BBCutter::SetStutterChance(float chance)  { cutproc11.SetStutterChance(chance);}
void	BBCutter::SetStutterArea(float area)      { cutproc11.SetStutterArea(area);}
void	BBCutter::SetMaxRepeats(long repeats)     { cutproc11.SetMaxRepeats(repeats);}
void	BBCutter::SetMinRepeats(long repeats)     { cutproc11.SetMinRepeats(repeats);}
void	BBCutter::SetStraightChance(float chance) { warpcutproc.SetStraightChance(chance);}
void	BBCutter::SetRegularChance(float chance)  { warpcutproc.SetRegularChance(chance);}
void	BBCutter::SetRitardChance(float chance)   { warpcutproc.SetRitardChance(chance);}
void	BBCutter::SetAccel(float v)               { warpcutproc.SetAccel(v);}
void	BBCutter::SetActivity(float v)            { sqpusher.SetActivity(v);}
void	BBCutter::SetFade(float v)                { player.SetFade( ms2samples(v,sr) );}
void	BBCutter::SetMinPhraseLength(long v) { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetMinPhraseLength),v));}
void	BBCutter::SetMaxPhraseLength(long v) { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetMaxPhraseLength),v));}
void	BBCutter::SetMinAmp(float v)      { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetMinAmp),v));}
void	BBCutter::SetMaxAmp(float v)      { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetMaxAmp),v));}
void	BBCutter::SetMinPan(float v)      { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetMinPan),v));}
void	BBCutter::SetMaxPan(float v)      { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetMaxPan),v));}
void	BBCutter::SetDutyCycle(float v)   { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetDutyCycle),v));}
void	BBCutter::SetFillDutyCycle(float v) { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetFillDutyCycle),v));}
void	BBCutter::SetMinDetune(float v)   { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetMinDetune),v));}
void	BBCutter::SetMaxDetune(float v)   { for_each(procs.begin(),procs.end(),bind2nd(mem_fun(&CutProc::SetMaxDetune),v));}
void	BBCutter::SetNumerator(double v)  { numerator   = v; beatsPerBar=4.0*numerator/denominator; UpdateRates();}
void	BBCutter::SetDenominator(double v) { denominator = v; beatsPerBar=4.0*numerator/denominator; UpdateRates();}

void	BBCutter::SetSampleRate(double v)
{
  sr = v;
  UpdateRates();
}

void	BBCutter::SetTimeInfos(double bpm,double num,double den,double srate)
{
  bool changed = false;
  if(tempo != bpm)
  {
    tempo = bpm;
    changed = true;
  }
  if(numerator != num)
  {
    numerator = num;
    changed = true;
  }
  if(denominator != den)
  {
    denominator = den;
    changed = true;
  }
  if(srate != sr)
  {
    sr = srate;
    changed = true;
  }
  if(changed)
    UpdateRates();
}

void	BBCutter::UpdateRates()
{
}

void	BBCutter::Phrase(long bar, long sd)
{
  barsnow = strategy->ChoosePhraseLength();
  totalunits = barsnow*subdiv;
  unitsdone = 0;
  
  listenermanager.OnPhrase(bar,sd);
}

void	BBCutter::Block(long bar,long sd)
{
  unitsinsideblock=0;
  std::vector<CutInfo> cuts;
  strategy->ChooseCuts(cuts,unitsinblock,
                       unitsdone,totalunits,subdiv,SamplesPerUnit());
  player.OnBlock(cuts);
  
  listenermanager.OnBlock(bar,sd);
}

void	BBCutter::Unit(long bar, long sd)
{
  if( totalunits<=0 || unitsdone>=totalunits || unitsdone<0 ) //out of phrase bounds and start of a bar
  {
    if(sd == 0)
      Phrase(bar,sd);
    else
      return;
  }
  
  if( unitsinsideblock>=unitsinblock || unitsinsideblock<0) //out of block bounds
    Block(bar,sd);
  
  unitsinsideblock++;
  unitsdone++;
  
  listenermanager.OnUnit(bar,sd);
}

void	BBCutter::SetPosition(long bar, long sd)
{
  const long delta = sd - (unitsdone % long(UnitsPerBar(subdiv,numerator,denominator)));
  unitsinsideblock += delta;
  unitsdone += delta;
  Unit(bar,sd);
}

