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

#include "ParamCombo.h"

ParamCombo::ParamCombo(AudioProcessor &effect, int paramId)
: ComboBox(effect.getParameterName(paramId))
, Control(effect, paramId)
{
}

ParamCombo::~ParamCombo()
{
}

void ParamCombo::refresh()
{
  /*
  const bool v = (mEffect.getParameter(mParamId)>0.5f)? true : false;
  
  if(v != getToggleState())
  {
    setToggleState(v, false);
  }
   */
}

void ParamCombo::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
  int selected = getSelectedItemIndex();
  
  if(selected>=0)
  {
    /*
    const int minimum = parameterDescription.mRange.GetMin();
    const int maximum = parameterDescription.mRange.GetMax();
    
    const int realValue = selected+minimum;
    
    setParameter(realValue);
    */
  }
}

/*
void ParamCombo::clicked()
{
  setParameter(getToggleState()? 1.f : 0.f);
}
 */
