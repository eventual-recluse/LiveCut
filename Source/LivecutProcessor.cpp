/*
 This file is part of Livecut
 Copyright 2004 by Remy Muller.
 
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

#include "LivecutProcessor.h"
#include "LivecutEditor.h"
#include "Functor.h"

LivecutAudioProcessor::LivecutAudioProcessor()
: bbcutter(player)
{
	bbcutter.RegisterListener(&crusher);
	bbcutter.RegisterListener(&comb);
  
	AddParameter(strategy
            .AddEntry("CutProc11")
            .AddEntry("WarpCut")
            .AddEntry("SQPusher")
            .name("CutProc").tag(0).cc(64)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetCutProc),&bbcutter)) ) 
            );
	AddParameter(subdiv
            .AddEntry(6)
            .AddEntry(8)
            .AddEntry(12)
            .AddEntry(16)
            .AddEntry(18)
            .AddEntry(24)
            .AddEntry(32)
            .name("subdiv").def(1L)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetSubdiv),&bbcutter)) )
            );
  AddParameter(seed
            .name("seed")
            .range(1L,16L).def(1L)
            .functor( newfun(std::ptr_fun(&Math::randomseed)) )
            );
  AddParameter(fade
            .name("Fade")
            .unit("ms")
            .range(0.f,100.f).def(0.f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetFade),&bbcutter)) )
            );
  fade.middle(5.f);
  AddParameter(minamp
            .name("minamp")
            .range(0.f,1.f).def(1.f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMinAmp),&bbcutter)) )
            );
  AddParameter(maxamp
            .name("maxamp")
            .range(0.f,1.f).def(1.f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMaxAmp),&bbcutter)) )
            );
  AddParameter(minpan
            .name("minpan").range(-1.f,1.f).def(-0.2f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMinPan),&bbcutter)) )
            );
  AddParameter(maxpan
            .name("maxpan")
            .range(-1.f,1.f).def(0.2f)	
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMaxPan),&bbcutter)) )
            );
  AddParameter(mindetune
            .name("minpitch").unit(" cen")
            .range(-2400.f,2400.f).def(0.f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMinDetune),&bbcutter)) )
            );
  AddParameter(maxdetune
            .name("maxpitch").unit(" cen")
            .range(-2400.f,2400.f).def(0.f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMaxDetune),&bbcutter)) )
            );
  AddParameter(dutycycle
            .name("duty")
            .range(0.f,1.f).def(1.f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetDutyCycle),&bbcutter)) )
            );
  AddParameter(filldutycycle
            .name("fillduty")
            .range(0.f,1.f).def(1.f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetFillDutyCycle),&bbcutter)) )
            );
	AddParameter(maxphraselength
            .name("maxphrs").unit(" bar")
            .range(1L,8L).def(4L)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMaxPhraseLength),&bbcutter)) )
            );
	AddParameter(minphraselength
            .name("minphrs").unit(" bar")
            .range(1L,8L).def(1L)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMinPhraseLength),&bbcutter)) )
            );
  AddParameter(maxrepeats
            .name("maxrep").unit(" rep")
            .range(0L,4L).def(1L)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMaxRepeats),&bbcutter)) )
            );
  AddParameter(minrepeats
            .name("minrep").unit(" rep")
            .range(0L,4L).def(0L)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetMinRepeats),&bbcutter)) )
            );
  AddParameter(stutterchance
            .name("stutter")
            .range(0.f,1.f).def(0.8f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetStutterChance),&bbcutter)) )
            );
  AddParameter(stutterarea
            .name("area")
            .range(0.f,1.f).def(0.5f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetStutterArea),&bbcutter)) )
            );
	AddParameter(straightchance
            .name("straight")
            .range(0.f,1.f).def(0.3f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetStraightChance),&bbcutter)) )
            );
  AddParameter(regularchance
            .name("regular")
            .range(0.f,1.f).def(0.5f)	
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetRegularChance),&bbcutter)) )
            );
	AddParameter(ritardchance
            .name("ritard")
            .range(0.f,1.f).def(0.5f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetRitardChance),&bbcutter)) )
            );
	AddParameter(accel
            .name("speed")
            .range(0.5f,0.999f).def(0.9f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetAccel),&bbcutter)) )
            );
  AddParameter(activity
            .name("activity")
            .range(0.f,1.f).def(0.5f)
            .functor( newfun(bind1st(mem_fun(&BBCutter::SetActivity),&bbcutter)) )
            );
	AddParameter(crusherswitch
            .name("crusher").def(false)
            .functor( newfun(bind1st(mem_fun(&BitCrusher::SetOn),&crusher)) )
            );
	AddParameter(minbits
            .name("minbits").unit("bit")
            .range(1.f,32.f).def(32.f)
            .functor( newfun(bind1st(mem_fun(&BitCrusher::SetMinBits),&crusher)) )
            );
	AddParameter(maxbits
            .name("maxbits").unit("bit")
            .range(1.f,32.f).def(32.f)
            .functor( newfun(bind1st(mem_fun(&BitCrusher::SetMaxBits),&crusher)) )
            );
	float sr = 44100;
	AddParameter(minfreq
            .name("minfreq").unit("Hz")
            .range(sr/100.f,sr).def(sr)
            .functor( newfun(bind1st(mem_fun(&BitCrusher::SetMinFreq),&crusher)) )
            );
  minfreq.middle(sr/10.f);
  AddParameter(maxfreq
            .name("maxfreq").unit("Hz")
            .range(sr/100.f,sr).def(sr)
            .functor( newfun(bind1st(mem_fun(&BitCrusher::SetMaxFreq),&crusher)) )
            );
  maxfreq.middle(sr/10.f);
	AddParameter(combswitch
            .name("comb").def(false)
            .functor( newfun(bind1st(mem_fun(&Comb::SetOn),&comb)) )
            );
	AddParameter(combtype
            .AddEntry("FeedFwd")
            .AddEntry("FeedBack")
            .name("type")
            .functor( newfun(bind1st(mem_fun(&Comb::SetType),&comb)) )
            );
	AddParameter(combfeedback
            .name("feedback")
            .range(0.f,0.9).def(0.5f)
            .functor( newfun(bind1st(mem_fun(&Comb::SetFeedBack),&comb)) )
            );
	AddParameter(combmindelay
            .name("mindelay").unit("ms")
            .range(1.f,50.f).def(10.f)
            .functor( newfun(bind1st(mem_fun(&Comb::SetMinDelay),&comb)) )
            );
	AddParameter(combmaxdelay
            .name("maxdelay").unit("ms")
            .range(1.f,50.f).def(10.f)
            .functor( newfun(bind1st(mem_fun(&Comb::SetMaxDelay),&comb)) )
            );
  
  for(ParamMap::iterator it = params.begin(); it != params.end(); ++it)
    it->second->TellListeners();
}

LivecutAudioProcessor::~LivecutAudioProcessor()
{
}

void LivecutAudioProcessor::AddParameter(Param &param)
{
  params.add(param);
}

const String LivecutAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

int LivecutAudioProcessor::getNumParameters()
{
  return params.size();
}

float LivecutAudioProcessor::getParameter (int index)
{
  if(params.count(index))
    return params[index]->GetNormalized();
  else
    return 0.f;
}

void LivecutAudioProcessor::setParameter (int index, float newValue)
{
  if(!params.count(index))
    return;
  
  Param *p = params[index];
  p->SetFromNormalized(newValue);
}

const String LivecutAudioProcessor::getParameterName (int index)
{
  if(params.count(index))
    return String(params[index]->GetName().c_str());	
  
  return String::empty;
}

const String LivecutAudioProcessor::getParameterText (int index)
{
  if(params.count(index))
    return String(params[index]->GetDisplay().c_str());	
  
  return String::empty;
}

const String LivecutAudioProcessor::getInputChannelName (int channelIndex) const
{
  return String (channelIndex + 1);
}

const String LivecutAudioProcessor::getOutputChannelName (int channelIndex) const
{
  return String (channelIndex + 1);
}

bool LivecutAudioProcessor::isInputChannelStereoPair (int index) const
{
  return true;
}

bool LivecutAudioProcessor::isOutputChannelStereoPair (int index) const
{
  return true;
}

bool LivecutAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool LivecutAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool LivecutAudioProcessor::silenceInProducesSilenceOut() const
{
  return false;
}

int LivecutAudioProcessor::getNumPrograms()
{
  return 0;
}

int LivecutAudioProcessor::getCurrentProgram()
{
  return 0;
}

void LivecutAudioProcessor::setCurrentProgram (int index)
{
}

const String LivecutAudioProcessor::getProgramName (int index)
{
  return String::empty;
}

void LivecutAudioProcessor::changeProgramName (int index, const String& newName)
{
}

void LivecutAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  
	crusher.SetSampleRate(sampleRate);
	comb.SetSampleRate(sampleRate);
  minfreq.range(100.f, sampleRate);
	maxfreq.range(100.f, sampleRate);
}

void LivecutAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
}

void LivecutAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
  // This is the place where you'd normally do the guts of your plugin's
  // audio processing...
  Process(buffer.getArrayOfChannels(), 
          buffer.getArrayOfChannels(), 
          buffer.getNumSamples());
  
  // In case we have more outputs than inputs, we'll clear any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
  {
    buffer.clear (i, 0, buffer.getNumSamples());
  }
}

void LivecutAudioProcessor::Process(float **inputs,float **outputs, const long size)
{
	float *outL			= outputs[0];
	float *outR			= outputs[1];
	float *inL			= inputs[0];
	float *inR			= inputs[1];
	long	div			= subdiv.GetIntValue();
  
  AudioPlayHead* playHead = getPlayHead();
  if(!playHead) return;
  
  AudioPlayHead::CurrentPositionInfo info;
  playHead->getCurrentPosition(info);
     
	double samplePos = info.timeInSamples;	// current location
	double sampleRate = getSampleRate();
	double ppqPos	= info.ppqPosition;		// 1 ppq
	double tempo = info.bpm;		// in bpm
	double numerator = double(info.timeSigNumerator);		// time signature
	double denominator = double(info.timeSigDenominator);
	double ppqduration = ((double)size/sampleRate) * (tempo/60.0); //duration of the block
	double divPerSample	= (double(div)/double(numerator))*ppqduration*(denominator/4.0)/double(size);
	double position	= (double(div)/double(numerator)) * ppqPos * (denominator/4.0);
	double ref = 0.0;
	long oldpositionInMeasure	= long(floor(fmod(position-ref, div)));
	long oldmeasure = long(floor((position-ref)/div));
  
	//double semiquaverPerSample	= (double(16)/double(numerator))*ppqduration*(denominator/4.0)/double(sampleframes);
	//double semiquaverposition = (double(16)/double(numerator))*ppqPos	 * (denominator/4.0);
	//long oldsemiquaverpositionInMeasure	= long(floor(fmod(semiquaverposition-ref,16)));
  
	bbcutter.SetTimeInfos(tempo, numerator, denominator, sampleRate);

  bool transportChanged = false; // predict from last bar
  if(transportChanged && info.isPlaying)
  {
    bbcutter.SetPosition(oldmeasure, oldpositionInMeasure);
  }
  
	for(int i=0; i<size; i++)
	{
		long positionInMeasure= long(floor(fmod(position-ref, div)));
		long measure = long(floor((position-ref)/div));
    
		//long semiquaverpositionInMeasure = long(floor(fmod	(semiquaverposition-ref	,div)));
    
		if(positionInMeasure!=oldpositionInMeasure)
		{
			bbcutter.SetPosition(measure,positionInMeasure);
			sqpamp.OnSemiQuaver((16*positionInMeasure)/div);
		}
    
		//if(semiquaverpositionInMeasure!=oldsemiquaverpositionInMeasure)
		//{
		//	sqpamp.OnSemiQuaver(semiquaverpositionInMeasure);
		//}
    
    float l=0.f;
    float r=0.f;
    player.tick(l,r,inL[i],inR[i]);
		crusher.tick(l,r,l,r);
		comb.tick(l,r,l,r);
		//sqpamp.tick(l,r,l,r);
    outL[i] = l;
    outR[i] = r;		
		oldpositionInMeasure = positionInMeasure;
		oldmeasure			 = measure;
		position			+= divPerSample;
    
		//oldsemiquaverpositionInMeasure = semiquaverpositionInMeasure;
		//semiquaverposition	+= semiquaverPerSample;
	}
}

bool LivecutAudioProcessor::hasEditor() const
{
  return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LivecutAudioProcessor::createEditor()
{
  /*
  AutoParam	maxphraselength, minphraselength, stutterchance, stutterarea, straightchance, regularchance, ritardchance, accel, maxrepeats, minrepeats;
	IntSetParam	subdiv;
  AutoParam activity, minamp, maxamp, minpan, maxpan, mindetune, maxdetune, dutycycle, filldutycycle, seed, minbits, maxbits, crusherswitch;
  PowerParam minfreq, maxfreq, fade;
	AutoParam	combmindelay, combmaxdelay, combfeedback, combswitch, sqpampswitch;
  EnumParam strategy, combtype;
   */
  
  LivecutAudioProcessorEditor *pEditor = new LivecutAudioProcessorEditor (this);
  pEditor->addKnob(minphraselength.GetTag(), 2, 0);
  pEditor->addKnob(maxphraselength.GetTag(), 2, 2);
  pEditor->addKnob(subdiv.GetTag(), 2, 4);
  pEditor->addKnob(seed.GetTag(), 2, 6);

  pEditor->addKnob(stutterchance.GetTag(), 7, 0);
  pEditor->addKnob(stutterarea.GetTag(), 7, 2);
  pEditor->addKnob(minrepeats.GetTag(), 7, 4);
  pEditor->addKnob(maxrepeats.GetTag(), 7, 6);
  
  pEditor->addKnob(straightchance.GetTag(), 7, 8);
  pEditor->addKnob(regularchance.GetTag(), 7, 10);
  pEditor->addKnob(ritardchance.GetTag(), 7, 12);
  pEditor->addKnob(accel.GetTag(), 7, 14);

  pEditor->addKnob(activity.GetTag(), 7, 16);

  pEditor->addKnob(minamp.GetTag(), 12, 0);
  pEditor->addKnob(maxamp.GetTag(), 12, 2);
  pEditor->addKnob(minpan.GetTag(), 12, 4);
  pEditor->addKnob(maxpan.GetTag(), 12, 6);

  pEditor->addKnob(mindetune.GetTag(), 2, 8);
  pEditor->addKnob(maxdetune.GetTag(), 2, 10);

  pEditor->addKnob(dutycycle.GetTag(), 2, 16);
  pEditor->addKnob(filldutycycle.GetTag(), 2, 18);
  pEditor->addKnob(fade.GetTag(), 2, 20);

  pEditor->addKnob(minbits.GetTag(), 12, 8);
  pEditor->addKnob(maxbits.GetTag(), 12, 10);
  pEditor->addKnob(minfreq.GetTag(), 12, 12);
  pEditor->addKnob(maxfreq.GetTag(), 12, 14);
  pEditor->addKnob(crusherswitch.GetTag(), 16, 2);

  
  return pEditor;
}


void LivecutAudioProcessor::getStateInformation (MemoryBlock& destData)
{
  // You should use this method to store your parameters in the memory block.
  // You could do that either as raw data, or use the XML or ValueTree classes
  // as intermediaries to make it easy to save and load complex data.
}

void LivecutAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
  // You should use this method to restore your parameters from this memory block,
  // whose contents will have been created by the getStateInformation() call.
}


// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new LivecutAudioProcessor();
}
