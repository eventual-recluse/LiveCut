/*
 This file is part of AnalogDelay
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

#ifndef LIVECUT_LOOKANDFEEL_H
#define LIVECUT_LOOKANDFEEL_H

#include "../JuceLibraryCode/JuceHeader.h"

//------------------------------------------------------------------------------
class LivecutLookAndFeel : public LookAndFeel
{
public:
  LivecutLookAndFeel();
  
  static LivecutLookAndFeel& getInstance();
    
  void 	drawComboBox (Graphics &g, 
                      int width, 
                      int height, 
                      bool isButtonDown, 
                      int buttonX, 
                      int buttonY, 
                      int buttonW, 
                      int buttonH, 
                      ComboBox &box);
  
  void 	drawButtonBackground (Graphics &g, 
                              Button &button, 
                              const Colour &backgroundColour, 
                              bool isMouseOverButton, bool isButtonDown);
  
  void drawLinearSlider (Graphics& g,
                         int x, int y,
                         int width, int height,
                         float sliderPos,
                         float minSliderPos,
                         float maxSliderPos,
                         const Slider::SliderStyle style,
                         Slider& slider);
  
  void drawLinearSliderBackground (Graphics& g,
                                   int x, int y,
                                   int width, int height,
                                   float sliderPos,
                                   float minSliderPos,
                                   float maxSliderPos,
                                   const Slider::SliderStyle style,
                                   Slider& slider);
  
  void drawLinearSliderThumb(Graphics& g,
                             int x, int y,
                             int width, int height,
                             float sliderPos,
                             float minSliderPos,
                             float maxSliderPos,
                             const Slider::SliderStyle style,
                             Slider& slider);
  
  int getSliderThumbRadius (Slider& slider);
    
  void drawRotarySlider (Graphics& g,
                         int x, int y,
                         int width, int height,
                         float sliderPos,
                         float rotaryStartAngle,
                         float rotaryEndAngle,
                         Slider& slider);
};

#endif