/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2021 Filipe Coelho <falktx@falktx.com>
 * Copyright (C) 2020 Takamitsu Endo
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 This file is part of Livecut
 Copyright 2004 by Remy Muller.
 Adapted for DISTRHO Plugin Framework (DPF) by eventual-recluse.
 
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

#include "DistrhoPlugin.hpp"
#include "LiveCutControls.hpp"
#include "BBCutter.h"
#include "BitCrusher.h"
#include "Comb.h"

START_NAMESPACE_DISTRHO



// -----------------------------------------------------------------------------------------------------------

/**
  Livecut beat-slicer audio plugin adapted for building with DISTRHO DPF. Livecut Copyright 2003 by Remy Muller.
 */
class LiveCut : public Plugin
{
public:
    LiveCut()
        : Plugin(LVC_CONTROL_NR, 0, 0), // (LVC_CONTROL_NR) parameters, 0 programs, 0 states
          sampleRate(getSampleRate()),
          bbcutter(player),
          
          oldpositionInMeasure(0),
          div(8)

    {
         // clear all parameters
        std::memset(control, 0, sizeof(float)*LVC_CONTROL_NR);
        
        // populate control with defaults
        for (int32_t i = 0; i < LVC_CONTROL_NR; i++)
        {
            control[i] = LVC_DEFAULTS[i];
        }
        
        sampleRateChanged(sampleRate);
        
        bbcutter.RegisterListener(&crusher);
        bbcutter.RegisterListener(&comb);
        
        comb.SetOn(false);
        crusher.SetOn(false);
        
    }

protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

   /**
      Get the plugin label.
      A plugin label follows the same rules as Parameter::symbol, with the exception that it can start with numbers.
    */
    const char* getLabel() const override
    {
        return "LiveCut";
    }

   /**
      Get an extensive comment/description about the plugin.
    */
    const char* getDescription() const override
    {
        return "Beat-slicer";
    }

   /**
      Get the plugin author/maker.
    */
    const char* getMaker() const override
    {
        return "eventual-recluse";
    }

   /**
      Get the plugin homepage.
    */
    const char* getHomePage() const override
    {
        return "https://github.com/eventual-recluse/LiveCut";
    }

   /**
      Get the plugin license name (a single line of text).
      For commercial plugins this should return some short copyright information.
    */
    const char* getLicense() const override
    {
        return "GPL";
    }

   /**
      Get the plugin version, in hexadecimal.
    */
    uint32_t getVersion() const override
    {
        return d_version(1, 0, 0);
    }

   /**
      Get the plugin unique Id.
      This value is used by LADSPA, DSSI and VST plugin formats.
    */
    int64_t getUniqueId() const override
    {
        return d_cconst('L', 'C', 'u', 't');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

   /**
      Initialize the audio port @a index.@n
      This function will be called once, shortly after the plugin is created.
    */
    void initAudioPort(bool input, uint32_t index, AudioPort& port) override
    {
        // treat meter audio ports as stereo
        port.groupId = kPortGroupStereo;

        // everything else is as default
        Plugin::initAudioPort(input, index, port);
    }

   /**
      Initialize the parameter @a index.
      This function will be called once, shortly after the plugin is created.
    */
    void initParameter(uint32_t index, Parameter& parameter) override
    {
        parameter.hints  = kParameterIsAutomatable;
        
        switch (index)
        {
        case LVC_CUTPROC:
            parameter.hints  |= kParameterIsInteger;
            parameter.name   = "CutProc";
            parameter.symbol = "livecut_cutproc";
            parameter.enumValues.count = 3;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* const values = new ParameterEnumerationValue[3];
                parameter.enumValues.values = values;

                values[0].label = "CutProc11";
                values[0].value = 0;
                values[1].label = "WarpCut";
                values[1].value = 1;
                values[2].label = "SQPusher";
                values[2].value = 2;
            }
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_SUBDIV:
            parameter.hints  |= kParameterIsInteger;
            parameter.name   = "SubDiv";
            parameter.symbol = "livecut_subdiv";
            parameter.enumValues.count = 7;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* const values = new ParameterEnumerationValue[7];
                parameter.enumValues.values = values;

                values[0].label = "6";
                values[0].value = 0;
                values[1].label = "8";
                values[1].value = 1;
                values[2].label = "12";
                values[2].value = 2;
                values[3].label = "16";
                values[3].value = 3;
                values[4].label = "18";
                values[4].value = 4;
                values[5].label = "24";
                values[5].value = 5;
                values[6].label = "32";
                values[6].value = 6;
            }
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_FADE:
            parameter.name = "Fade";
            parameter.symbol = "livecut_fade";
            parameter.hints |= kParameterIsLogarithmic;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            parameter.unit   = "ms";
            break;
        case LVC_MINAMP:
            parameter.name = "Min Amp";
            parameter.symbol = "livecut_minamp";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MAXAMP:
            parameter.name = "Max Amp";
            parameter.symbol = "livecut_maxamp";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MINPAN:
            parameter.name = "Min Pan";
            parameter.symbol = "livecut_minpan";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MAXPAN:
            parameter.name = "Max Pan";
            parameter.symbol = "livecut_maxpan";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MINPITCH:
            parameter.name = "Min Pitch";
            parameter.symbol = "livecut_minpitch";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            parameter.unit   = "cents";
            break;
        case LVC_MAXPITCH:
            parameter.name = "Max Pitch";
            parameter.symbol = "livecut_maxpitch";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            parameter.unit   = "cents";
            break;
        case LVC_DUTY:
            parameter.name = "Duty";
            parameter.symbol = "livecut_duty";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_FILLDUTY:
            parameter.name = "Fill Duty";
            parameter.symbol = "livecut_fillduty";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MAXPHRS:
            parameter.name = "Max Phrase";
            parameter.symbol = "livecut_maxphrs";
            parameter.hints |= kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            //parameter.unit   = "bars";
            break;
        case LVC_MINPHRS:
            parameter.name = "Min Phrase";
            parameter.symbol = "livecut_minphrs";
            parameter.hints |= kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            //parameter.unit   = "bars";
            break;
        case LVC_MAXREP:
            parameter.name = "CutProc11 Max Rep";
            parameter.symbol = "livecut_maxrep";
            parameter.hints |= kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MINREP:
            parameter.name = "CutProc11 Min Rep";
            parameter.symbol = "livecut_minrep";
            parameter.hints |= kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_STUTTER:
            parameter.name = "CutProc 11 Stutter";
            parameter.symbol = "livecut_stutter";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_AREA:
            parameter.name = "CutProc 11 Area";
            parameter.symbol = "livecut_area";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_STRAIGHT:
            parameter.name = "WarpCut Straight";
            parameter.symbol = "livecut_straight";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_REGULAR:
            parameter.name = "WarpCut Regular";
            parameter.symbol = "livecut_regular";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_RITARD:
            parameter.name = "WarpCut Ritard";
            parameter.symbol = "livecut_ritard";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_SPEED:
            parameter.name = "WarpCut Speed";
            parameter.symbol = "livecut_speed";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_ACTIVITY:
            parameter.name = "SQPusher Activity";
            parameter.symbol = "livecut_activity";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_CRUSHER:
            parameter.name = "Crusher";
            parameter.symbol = "livecut_crusher";
            parameter.hints |= kParameterIsBoolean;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MINBITS:
            parameter.name = "Crusher Min Bits";
            parameter.symbol = "livecut_minbits";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MAXBITS:
            parameter.name = "Crusher Max Bits";
            parameter.symbol = "livecut_maxbits";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MINFREQ:
            parameter.name = "Crusher Min Freq";
            parameter.symbol = "livecut_minfreq";
            parameter.hints |= kParameterIsLogarithmic;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            parameter.unit   = "Hz";
            break;
        case LVC_MAXFREQ:
            parameter.name = "Crusher Max Freq";
            parameter.symbol = "livecut_maxfreq";
            parameter.hints |= kParameterIsLogarithmic;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            parameter.unit   = "Hz";
            break;
        case LVC_COMB:
            parameter.name = "Comb";
            parameter.symbol = "livecut_comb";
            parameter.hints |= kParameterIsBoolean;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_TYPE:
            parameter.hints  |= kParameterIsInteger;
            parameter.name   = "Comb Type";
            parameter.symbol = "livecut_type";
            parameter.enumValues.count = 2;
            parameter.enumValues.restrictedMode = true;
            {
                ParameterEnumerationValue* const values = new ParameterEnumerationValue[2];
                parameter.enumValues.values = values;
                values[0].label = "FeedFwd";
                values[0].value = 0;
                values[1].label = "FeedBack";
                values[1].value = 1;
            }
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_FEEDBACK:
            parameter.name = "Comb Feedback";
            parameter.symbol = "livecut_feedback";
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        case LVC_MINDELAY:
            parameter.name = "Comb Min Delay";
            parameter.symbol = "livecut_mindelay";
            parameter.hints |= kParameterIsLogarithmic;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            parameter.unit   = "ms";
            break;
        case LVC_MAXDELAY:
            parameter.name = "Comb Max Delay";
            parameter.symbol = "livecut_maxdelay";
            parameter.hints |= kParameterIsLogarithmic;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            parameter.unit   = "ms";
            break;
        case LVC_SEED:
            parameter.name = "Seed";
            parameter.symbol = "livecut_seed";
            parameter.hints |= kParameterIsInteger;
            parameter.ranges.min = controlLimits[index].first;
            parameter.ranges.max = controlLimits[index].second;
            parameter.ranges.def = LVC_DEFAULTS[index];
            break;
        }
    }

    /**
      Update parameters and call relevant methods for them
    */
    
    void update_parameter(uint32_t i)
    {
        control[i] = limit<float> (control[i], controlLimits[i].first, controlLimits[i].second);
        
        switch (i)
        {
        case LVC_CUTPROC:
            bbcutter.SetCutProc(static_cast<long> (control[i]));
            break;

        case LVC_SUBDIV:
            {
                div = LVC_SUBDIV_OPTIONS[static_cast<long>(control[i])];
                bbcutter.SetSubdiv(div);
                break;
            }

        case LVC_FADE:
            bbcutter.SetFade(control[i]);
            break;

        case LVC_MINAMP:
            bbcutter.SetMinAmp(control[i]);
            break;

        case LVC_MAXAMP:
            bbcutter.SetMaxAmp(control[i]);
            break;

        case LVC_MINPAN:
            bbcutter.SetMinPan(control[i]);
            break;

        case LVC_MAXPAN:
            bbcutter.SetMaxPan(control[i]);
            break;

        case LVC_MINPITCH:
            bbcutter.SetMinDetune(control[i]);
            break;

        case LVC_MAXPITCH:
            bbcutter.SetMaxDetune(control[i]);
            break;

        case LVC_DUTY:
            bbcutter.SetDutyCycle(control[i]);
            break;

        case LVC_FILLDUTY:
            bbcutter.SetFillDutyCycle(control[i]);
            break;

        case LVC_MAXPHRS:
            bbcutter.SetMaxPhraseLength(static_cast<long> (control[i]));
            break;

        case LVC_MINPHRS:
            bbcutter.SetMinPhraseLength(static_cast<long> (control[i]));
            break;

        case LVC_MAXREP:
            bbcutter.SetMaxRepeats(static_cast<long> (control[i]));
            break;

        case LVC_MINREP:
            bbcutter.SetMinRepeats(static_cast<long> (control[i]));
            break;

        case LVC_STUTTER:
            bbcutter.SetStutterChance(control[i]);
            break;

        case LVC_AREA:
            bbcutter.SetStutterArea(control[i]);
            break;

        case LVC_STRAIGHT:
            bbcutter.SetStraightChance(control[i]);
            break;

        case LVC_REGULAR:
            bbcutter.SetRegularChance(control[i]);
            break;

        case LVC_RITARD:
            bbcutter.SetRitardChance(control[i]);
            break;

        case LVC_SPEED:
            bbcutter.SetAccel(control[i]);
            break;

        case LVC_ACTIVITY:
            bbcutter.SetActivity(control[i]);
            break;

        case LVC_CRUSHER:
            crusher.SetOn(static_cast<bool> (control[i]));
            break;

        case LVC_MINBITS:
            crusher.SetMinBits(control[i]);
            break;

        case LVC_MAXBITS:
            crusher.SetMaxBits(control[i]);
            break;

        case LVC_MINFREQ:
            crusher.SetMinFreq(control[i]);
            break;

        case LVC_MAXFREQ:
            crusher.SetMaxFreq(control[i]);
            break;

        case LVC_COMB:
            comb.SetOn(static_cast<bool> (control[i]));
            break;

        case LVC_TYPE:
            comb.SetType(static_cast<long> (control[i]));
            break;

        case LVC_FEEDBACK:
            comb.SetFeedBack(control[i]);
            break;

        case LVC_MINDELAY:
            comb.SetMinDelay(control[i]);
            break;

        case LVC_MAXDELAY:
            comb.SetMaxDelay(control[i]);
            break;
        
        case LVC_SEED:
            Math::randomseed(static_cast<long> (control[i]));
            break;
            
            
        default:
            break;
        }
    }
    
   /* --------------------------------------------------------------------------------------------------------
    * Internal data */

   /**
      Get the current value of a parameter.
    */
    float getParameterValue(uint32_t index) const override
    {
        if (index >= LVC_CONTROL_NR) return 0.0f;
        return control[index];
    }

   /**
      Change a parameter value.
    */
    void setParameterValue(uint32_t index, float value) override
    {
        if (index >= LVC_CONTROL_NR) return;
        control[index] = value;
        update_parameter(index);
    }

   /* --------------------------------------------------------------------------------------------------------
    * Process */

   /**
      Activate this plugin.
    */
   void activate() override
   {
        crusher.SetSampleRate(sampleRate);
        comb.SetSampleRate(sampleRate);
        
        // TODO  Sample rate change?
        //minfreq.range(100.f, sampleRate);
        //maxfreq.range(100.f, sampleRate);
        
        for (uint32_t i = 0; i < LVC_CONTROL_NR; ++i)
        {
            update_parameter(i);
        }
   }

   /**
      Run/process function for plugins without MIDI input.
    */
    void run(const float** inputs, float** outputs, uint32_t frames) override
    {
        const float* inL  = inputs[0];
        const float* inR  = inputs[1];
        float*       outL = outputs[0];
        float*       outR = outputs[1];
                
        const TimePosition& timePos(getTimePosition());
        
        double bpm = timePos.bbt.beatsPerMinute;
        double beats_per_bar = timePos.bbt.beatsPerBar;
        // In DISTRHO DPF, the first bar == 1. But our calculations require first bar == 0
        double bar = timePos.bbt.bar - 1;
        // In DISTRHO DPF, the first beat of the bar == 1. Our calculations require first beat of the bar == 0
        double beat = timePos.bbt.beat - 1;
        double beatFraction   = timePos.bbt.tick / timePos.bbt.ticksPerBeat;
        
        // denominator
        double beat_unit = timePos.bbt.beatType;
        
        double ppqPos = (bar * beats_per_bar) + beat + beatFraction;
        
        double ppqduration = (static_cast<double>(frames)/sampleRate) * (bpm/60.0); //duration of the block
        double divPerSample	= (static_cast<double>(div)/beats_per_bar)*ppqduration*(beat_unit/4.0)/static_cast<double>(frames);
        double position	= (static_cast<double>(div)/beats_per_bar) * ppqPos * (beat_unit/4.0);
        double ref = 0.0;

        bbcutter.SetTimeInfos(bpm, beats_per_bar, beat_unit, sampleRate);

        long oldmeasure = static_cast<long>(std::floor((position-ref)/div));
        long positionInMeasure = static_cast<long>(std::floor(std::fmod(position-ref, div)));
        if (positionInMeasure != oldpositionInMeasure)
        {
            oldpositionInMeasure = positionInMeasure;
            bbcutter.SetPosition(oldmeasure, oldpositionInMeasure);
        }

        for (uint32_t i = 0; i < frames; ++i)
        {
            positionInMeasure= static_cast<long>(std::floor(std::fmod(position-ref, div)));
            long measure = static_cast<long>(std::floor((position-ref)/div));
            
            if(positionInMeasure!=oldpositionInMeasure)
            {
                bbcutter.SetPosition(measure,positionInMeasure);
            }
            
            float l=0.f;
            float r=0.f;
            player.tick(l, r, inL[i], inR[i]);
            crusher.tick(l,r,l,r);
            comb.tick(l,r,l,r);
            
            outL[i] = l;
            outR[i] = r;		
            oldpositionInMeasure = positionInMeasure;
            oldmeasure			 = measure;
            position			+= divPerSample;
        }
    }


   /* --------------------------------------------------------------------------------------------------------
    * Callbacks (optional) */

   /**
      Optional callback to inform the plugin about a sample rate change.
      This function will only be called when the plugin is deactivated.
    */
    void sampleRateChanged(double newSampleRate) override
    {
        sampleRate = newSampleRate;
    }

    // -------------------------------------------------------------------------------------------------------

private:
    float sampleRate;
    
    // Parameters.
    float control[LVC_CONTROL_NR];
    
    // Livecut components
    LivePlayer player;
    BitCrusher crusher;
    Comb comb;
    BBCutter bbcutter;
    
    // Variables used by the run() method which are stored or updated outside of the run() method
    long oldpositionInMeasure;
    long div;

   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiveCut)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new LiveCut();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
