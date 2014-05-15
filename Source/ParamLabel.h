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

#ifndef PARAM_LABEL_H
#define PARAM_LABEL_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "Control.h"

class ParamLabel : public Label, public Control
{
public:
  enum Mode
  {
    showParameterValue=0,
    showParameterName,
  };
  
  ParamLabel(AudioProcessor &effect, 
             int paramId, 
             Mode mode=showParameterValue);
  ~ParamLabel();
  
  virtual void refresh();
  
protected:
  Mode mMode;
};

#endif