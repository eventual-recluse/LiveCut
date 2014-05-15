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

#ifndef __PLUGINPROCESSOR_H_8A12CEC8__
#define __PLUGINPROCESSOR_H_8A12CEC8__

#include "../JuceLibraryCode/JuceHeader.h"
#include "BBCutter.h"
#include "BitCrusher.h"
#include "Comb.h"
#include "SQPAmp.h"
#include "Params.h"

class LivecutAudioProcessor  : public AudioProcessor
{
public:    
  LivecutAudioProcessor();
  ~LivecutAudioProcessor();  
  
  void prepareToPlay (double sampleRate, int samplesPerBlock);
  void releaseResources();
  
  void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
  
  AudioProcessorEditor* createEditor();
  bool hasEditor() const;
  
  const String getName() const;
  
  int getNumParameters();
  
  float getParameter (int index);
  void setParameter (int index, float newValue);
  
  const String getParameterName (int index);
  const String getParameterText (int index);
  
  const String getInputChannelName (int channelIndex) const;
  const String getOutputChannelName (int channelIndex) const;
  bool isInputChannelStereoPair (int index) const;
  bool isOutputChannelStereoPair (int index) const;
  
  bool acceptsMidi() const;
  bool producesMidi() const;
  bool silenceInProducesSilenceOut() const;
  
  int getNumPrograms();
  int getCurrentProgram();
  void setCurrentProgram (int index);
  const String getProgramName (int index);
  void changeProgramName (int index, const String& newName);
  
  void getStateInformation (MemoryBlock& destData);
  void setStateInformation (const void* data, int sizeInBytes);
  
  virtual double getTailLengthSeconds() const { return 0; }

private:
  void Process(float **inputs,float **outputs, const long size);
  void AddParameter(Param &param);
  
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LivecutAudioProcessor)
  ParamMap params;
  
  AutoParam	maxphraselength, minphraselength, stutterchance, stutterarea, straightchance, regularchance, ritardchance, accel, maxrepeats, minrepeats;
	IntSetParam	subdiv;
  AutoParam activity, minamp, maxamp, minpan, maxpan, mindetune, maxdetune, dutycycle, filldutycycle, seed, minbits, maxbits, crusherswitch;
  PowerParam minfreq, maxfreq, fade;
	AutoParam	combmindelay, combmaxdelay, combfeedback, combswitch, sqpampswitch;
  EnumParam strategy, combtype;
  
  LivePlayer player;
	BitCrusher crusher;
	Comb comb;
	SQPAmp sqpamp;
	BBCutter bbcutter;
};

#endif  // __PLUGINPROCESSOR_H_8A12CEC8__
