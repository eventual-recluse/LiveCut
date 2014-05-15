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

#include "ParamSlider.h"

ParamSlider::ParamSlider(AudioProcessor &effect, int paramId)
: Slider("ParamSlider")
, Control(effect, paramId)
{  
  setRange(0, 1);
  
  setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);	
  setSliderStyle(juce::Slider::LinearHorizontal);
}

ParamSlider::~ParamSlider()
{
}

void ParamSlider::valueChanged()
{
  setParameter((float)getValue());
}

void ParamSlider::refresh()
{
  const double x = mEffect.getParameter(mParamId);
  
  if(x != getValue())
  {
    setValue(x, dontSendNotification);
  }
}
