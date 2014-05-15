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

#include "LivecutLookAndFeel.h"

static Colour createBaseColour (const Colour& buttonColour,
                                const bool hasKeyboardFocus,
                                const bool isMouseOverButton,
                                const bool isButtonDown)
{
  const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
  const Colour baseColour (buttonColour.withMultipliedSaturation (sat));
  
  if (isButtonDown)
    return baseColour.contrasting (0.2f);
  else if (isMouseOverButton)
    return baseColour.contrasting (0.1f);
  
  return baseColour;
}

static float getCornerSize(const int width, const int height)
{
  return jlimit(1.f, 3.f, 3.f*jmin(width, height)/20.f);
}

static void createRoundedPath (Path& p,
                               const float x, const float y,
                               const float w, const float h,
                               const float cs,
                               const bool curveTopLeft, const bool curveTopRight,
                               const bool curveBottomLeft, const bool curveBottomRight)
{
  const float cs2 = 2.0f * cs;
  
  if (curveTopLeft)
  {
    p.startNewSubPath (x, y + cs);
    p.addArc (x, y, cs2, cs2, float_Pi * 1.5f, float_Pi * 2.0f);
  }
  else
  {
    p.startNewSubPath (x, y);
  }
  
  if (curveTopRight)
  {
    p.lineTo (x + w - cs, y);
    p.addArc (x + w - cs2, y, cs2, cs2, 0.0f, float_Pi * 0.5f);
  }
  else
  {
    p.lineTo (x + w, y);
  }
  
  if (curveBottomRight)
  {
    p.lineTo (x + w, y + h - cs);
    p.addArc (x + w - cs2, y + h - cs2, cs2, cs2, float_Pi * 0.5f, float_Pi);
  }
  else
  {
    p.lineTo (x + w, y + h);
  }
  
  if (curveBottomLeft)
  {
    p.lineTo (x + cs, y + h);
    p.addArc (x, y + h - cs2, cs2, cs2, float_Pi, float_Pi * 1.5f);
  }
  else
  {
    p.lineTo (x, y + h);
  }
  
  p.closeSubPath();
}


//------------------------------------------------------------------------------
LivecutLookAndFeel::LivecutLookAndFeel()
{
  static const int standardColours[] =
  {
    PopupMenu::highlightedBackgroundColourId,   Colours::royalblue.getARGB(),
    PopupMenu::highlightedTextColourId,         0xffffffff,
    PopupMenu::backgroundColourId,              0xff1E1E1E,
    PopupMenu::textColourId,                    0xffeeeeee,
    PopupMenu::headerTextColourId,              0xff57CDFF,
    
    ComboBox::backgroundColourId,					Colours::darkgrey.getARGB(),
    ComboBox::outlineColourId,						Colours::black.withAlpha(0.5f).getARGB(),
    ComboBox::textColourId,								0xffFFFFFF,
    ComboBox::arrowColourId,							Colours::lightgrey.getARGB(),
    
    Label::textColourId,                  0xffFFFFFF,
    
    Slider::rotarySliderFillColourId,           Colours::darkgrey.getARGB(),
    Slider::rotarySliderOutlineColourId,        0xffB0D1F0,
    Slider::trackColourId, 0xff1E1E1E,
    Slider::thumbColourId, Colours::lightgrey.getARGB(),
    
    TextButton::buttonColourId,           0xff464646,
    TextButton::buttonOnColourId,         Colours::royalblue.getARGB(),
    TextButton::textColourOnId,						0xffFFFFFF,
    TextButton::textColourOffId,					0xffFFFFFF,			
  };
  
  for (int i = 0; i < numElementsInArray (standardColours); i += 2)
    setColour (standardColours [i], Colour(standardColours [i + 1]));    
}

LivecutLookAndFeel& LivecutLookAndFeel::getInstance()
{
  static ScopedPointer<LivecutLookAndFeel> lnf;

  if(!lnf.get())
  {
    lnf = new LivecutLookAndFeel();
  }
  
  return *lnf.get();
}

void LivecutLookAndFeel::drawComboBox (Graphics &g, int width, int height, bool isButtonDown, 
                                       int buttonX, int buttonY, int buttonW, int buttonH, ComboBox &box)
{
  const float cornerSize = getCornerSize(width, height);
  
  const Colour colour(createBaseColour( box.findColour(ComboBox::backgroundColourId), box.hasKeyboardFocus (true), false, false)
                      .withMultipliedAlpha (box.isEnabled() ? 1.0f : 0.5f));
  
  float x = 0.5;
  float y = 0.5;
  float w = width-1;
  float h = height-1;
  
  Path outline;
  outline.addRoundedRectangle(x, y, w, h, cornerSize);
  
  // background
  if(isButtonDown)
  {
    g.setFillType(ColourGradient(colour.darker(), 0, y, 
                                 colour, 0, y+h, 
                                 false));
  }
  else
  {
    g.setFillType(ColourGradient(colour, 0, y, 
                                 colour.darker(), 0, y+h, 
                                 false));
  }
  g.fillPath(outline);
  
  // bevel
  if (!colour.isTransparent())
  {
    ColourGradient bevel(Colours::white.withAlpha(0.1f), 0, y+1, 
                         Colours::transparentWhite, 0, y+1+cornerSize+1, false);
    g.setFillType(bevel);
    g.strokePath(outline, PathStrokeType(1), AffineTransform::translation(0, 1));
  }
  
  // arrow
  if (box.isEnabled())
  {
    if ((buttonW > 0.f) && (buttonH > 0.f))
    {
      const float arrowW = 0.4f * float(buttonH)/float(buttonW);
      const float arrowH = 0.4f * float(buttonW)/float(buttonH);
      
      Path p;
      p.addTriangle ( floor(0.5f+buttonX + buttonW * 0.5f),              floor(0.5f+buttonY + buttonH * (0.5f + arrowH/2)),
                     floor(0.5f+buttonX + buttonW * (0.5f - arrowW/2)), floor(0.5f+buttonY + buttonH * (0.5f - arrowH/2)),
                     floor(0.5f+buttonX + buttonW * (0.5f + arrowW/2)), floor(0.5f+buttonY + buttonH * (0.5f - arrowH/2)));
      
      g.setColour (box.findColour(ComboBox::arrowColourId));
      g.fillPath (p);
    }
  }
  
  // outline
  g.setColour(box.findColour(ComboBox::outlineColourId));
  g.strokePath(outline, PathStrokeType(1));
}

void LivecutLookAndFeel::drawButtonBackground (Graphics &g, 
                                               Button &button, 
                                               const Colour &backgroundColour, 
                                               bool isMouseOverButton, bool isButtonDown)
{
  const int width = button.getWidth();
  const int height = button.getHeight();
  
  const Colour colour(createBaseColour(backgroundColour, button.hasKeyboardFocus (true), isMouseOverButton, isButtonDown)
                      .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
  
  
  const float cornerSize = getCornerSize(width, height);
  
  float x = 0.5;
  float y = 0.5;
  float w = width-1;
  float h = height-1;
  if(button.isConnectedOnRight())
    w += 1;
  Path outline;
  createRoundedPath (outline, x, y, w, h, cornerSize,
                     ! (button.isConnectedOnLeft() || button.isConnectedOnTop()),
                     ! (button.isConnectedOnRight() || button.isConnectedOnTop()),
                     ! (button.isConnectedOnLeft() || button.isConnectedOnBottom()),
                     ! (button.isConnectedOnRight() || button.isConnectedOnBottom()));
  
  Rectangle<float> r(x, y, width, height);
  
  if(!(isButtonDown || button.getToggleState()))
  {
    g.setFillType(ColourGradient(colour, 0, r.getY(), 
                                 colour.darker(), 0, r.getBottom(), false));
    g.fillPath(outline);
    
    ColourGradient bevel(Colours::white.withAlpha(0.1f), 0, y+1, 
                         Colours::transparentWhite, 0, y+1+cornerSize+1, false);
    g.setFillType(bevel);
    g.strokePath(outline, PathStrokeType(1), AffineTransform::translation(0, 1));
  }
  else
  {
    g.setFillType(ColourGradient(colour.darker(), 0, 0, 
                                 colour, 0, r.getHeight(), false));
    g.fillPath(outline);
  }
  
  g.setColour(Colours::black.withAlpha(0.5f));
  g.strokePath(outline, PathStrokeType(1));
}	

void LivecutLookAndFeel::drawLinearSlider (Graphics& g,
                                           int x, int y,
                                           int width, int height,
                                           float sliderPos,
                                           float minSliderPos,
                                           float maxSliderPos,
                                           const Slider::SliderStyle style,
                                           Slider& slider)
{
  LookAndFeel::drawLinearSlider(g,x,y,width,height,sliderPos,minSliderPos, maxSliderPos, style, slider);
}

void LivecutLookAndFeel::drawLinearSliderBackground (Graphics& g,
                                                     int x, int y,
                                                     int width, int height,
                                                     float sliderPos,
                                                     float /*minSliderPos*/,
                                                     float /*maxSliderPos*/,
                                                     const Slider::SliderStyle /*style*/,
                                                     Slider& slider)
{
  const float sliderRadius = (float) (getSliderThumbRadius (slider));
  
  const Colour trackColour(slider.findColour(Slider::trackColourId));
  const Colour gradCol1 (trackColour.overlaidWith (Colours::black.withAlpha(slider.isEnabled() ? 0.25f : 0.13f)));
  const Colour gradCol2 (trackColour.overlaidWith (Colour(0x14000000)));
  Path indent;
  
  const float zeroPos = slider.getPositionOfValue(0);
  const int from = jmin(zeroPos, sliderPos);
  const int to   = jmax(zeroPos, sliderPos);
  
  if (slider.isHorizontal())
  {
    const float iy = y + height * 0.5f - sliderRadius * 0.5f;
    const float ih = sliderRadius;
    {
      ColourGradient gb (gradCol1, 0.0f, iy, gradCol2, 0.0f, iy + ih, false);
      g.setFillType(gb);
      
      indent.addRoundedRectangle (x - sliderRadius * 0.5f, iy,
                                  width + sliderRadius, ih,
                                  5.0f);
      g.fillPath (indent);
    }
    {
      const Colour c = Colours::royalblue.darker();
      const Colour c1 (c.overlaidWith(Colours::black.withAlpha(slider.isEnabled() ? 0.25f : 0.13f)));
      const Colour c2 (c.overlaidWith(Colour(0x14000000)));
      ColourGradient gb (c1, 0.0f, iy, c2, 0.0f, iy + ih, false);
      g.setFillType(gb);
      
      Path indent2;
      float rx = from - sliderRadius * 0.5f;
      float rw = to-from;
      indent2.addRoundedRectangle (rx, iy, rw, ih, 5.0f);
      g.fillPath (indent2);
    }
  }
  else
  {
    const float ix = x + width * 0.5f - sliderRadius * 0.5f;
    const float iw = sliderRadius;
    {
      ColourGradient gb (gradCol1, ix, 0.0f, gradCol2, ix + iw, 0.0f, false);
      g.setFillType(gb);
      
      indent.addRoundedRectangle (ix, y - sliderRadius * 0.5f,
                                  iw, height + sliderRadius,
                                  5.0f);
      g.fillPath (indent);
    }
    {
      const Colour c = Colours::royalblue.darker();
      const Colour c1(c.overlaidWith(Colours::black.withAlpha(slider.isEnabled() ? 0.25f : 0.13f)));
      const Colour c2(c.overlaidWith(Colour(0x14000000)));
      ColourGradient gb (c1, ix, 0.0f, c2, ix + iw, 0.0f, false);
      g.setFillType(gb);
      
      Path indent2;
      float ry = from+sliderRadius*0.5f;
      float rh = to-from+sliderRadius*0.5f;
      indent2.addRoundedRectangle (ix, ry, iw, rh, 5.0f);
      g.fillPath (indent2);
    }
  }
  
  g.setColour (Colour (0x4c000000));
  g.strokePath (indent, PathStrokeType (1.f));
  
  if (slider.isHorizontal())
  {
    Rectangle<float> r = indent.getBounds();
    g.setFillType(ColourGradient(Colours::black.withAlpha(0.1f), 0, r.getY(),
                                 Colours::transparentBlack, 0, r.getY()+5, false));
    g.strokePath (indent, PathStrokeType (1.f));
    
    g.setFillType(ColourGradient(Colours::transparentWhite, 0, r.getBottom()-5,
                                 Colours::white.withAlpha(0.15f), 0, r.getBottom(), false));
    g.strokePath (indent, PathStrokeType (1.f));
  }
}

void LivecutLookAndFeel::drawLinearSliderThumb(Graphics& g,
                                               int x, int y,
                                               int width, int height,
                                               float sliderPos,
                                               float minSliderPos,
                                               float maxSliderPos,
                                               const Slider::SliderStyle style,
                                               Slider& slider)
{
  const float sliderRadius = (float)getSliderThumbRadius (slider);
  
  Colour knobColour (createBaseColour (slider.findColour (Slider::thumbColourId),
                                       slider.hasKeyboardFocus (false) && slider.isEnabled(),
                                       slider.isMouseOverOrDragging() && slider.isEnabled(),
                                       slider.isMouseButtonDown() && slider.isEnabled()));
  
  const float outlineThickness = slider.isEnabled() ? 1.f : 0.5f;
  
  if (style == Slider::LinearHorizontal || style == Slider::LinearVertical)
  {
    float kx, ky;
    
    if (style == Slider::LinearVertical)
    {
      kx = x + width * 0.5f;
      ky = sliderPos;
    }
    else
    {
      kx = sliderPos;
      ky = y + height * 0.5f;
    }
    
    Path ellipse;
    ellipse.addEllipse(kx-sliderRadius+0.5, ky-sliderRadius+0.5, 2*sliderRadius-1, 2*sliderRadius-1);
    
    //background
    g.setFillType(ColourGradient(knobColour, kx, ky, 
                                 knobColour.darker(), kx, ky+2*sliderRadius, false));
    g.fillPath(ellipse);
    
    // higlight
    g.setFillType(ColourGradient(Colours::white.withAlpha(0.1f), kx, ky+1, 
                                 Colours::transparentWhite,			 kx, ky+sliderRadius, false));
    g.strokePath(ellipse, PathStrokeType(1.f), AffineTransform::translation(0, 1));
    
    // outline
    g.setColour(Colours::black.withAlpha(0.5f));
    g.strokePath(ellipse, PathStrokeType(1.f));
  }
  else
  {
    LookAndFeel::drawLinearSliderThumb(g,x,y,width,height, sliderPos, minSliderPos, maxSliderPos, style, slider);
  }
}

int LivecutLookAndFeel::getSliderThumbRadius (Slider& slider)
{
  return jmin (7, slider.getHeight()/2, slider.getWidth()/2);
}

void LivecutLookAndFeel::drawRotarySlider (Graphics& g,
                                           int x, int y,
                                           int width, int height,
                                           float sliderPos,
                                           float rotaryStartAngle,
                                           float rotaryEndAngle,
                                           Slider& slider)
{
  const float radius = jmin (width / 2, height / 2);
  const float centreX = x + width * 0.5f;
  const float centreY = y + height * 0.5f;
  const float rx = centreX - radius;
  const float ry = centreY - radius;
  const float rw = radius * 2.0f;
  
  const float lineWidth = jmax(1.f, radius * 0.15f);
  
  const float minimum = slider.getMinimum();
  const float maximum = slider.getMaximum();
  
  const float referencePos = slider.valueToProportionOfLength(jlimit(minimum, maximum, 0.f));
  
  const float from = jmin(referencePos, sliderPos);
  const float to   = jmax(referencePos, sliderPos);
  
  const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
  const float fromAngle = rotaryStartAngle + from * (rotaryEndAngle - rotaryStartAngle);
  const float toAngle = rotaryStartAngle + to * (rotaryEndAngle - rotaryStartAngle);
  
  const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
  
  Colour colour = slider.isEnabled()? slider.findColour (Slider::rotarySliderFillColourId) : Colour (0x80808080);
  g.setColour (colour);
  
  Colour outlineColour = slider.isEnabled()? slider.findColour (Slider::rotarySliderOutlineColourId) : Colour (0x80808080);
  
  {
    //const float thickness = (radius>12.f)? 0.8f : 1.f;
    
    {
      const float thickness = (radius - lineWidth) / float(radius);
      Path filledArc;
      filledArc.addPieSegment(rx, ry, rw, rw,
                              rotaryStartAngle,
                              rotaryEndAngle,
                              thickness);
      g.setColour(Colours::black.withAlpha(0.3f));
      g.fillPath (filledArc); 
    }
    {
      const float thickness = (radius - lineWidth) / float(radius);
      Path filledArc;
      filledArc.addPieSegment(rx, ry, rw, rw,
                              fromAngle,
                              toAngle,
                              thickness);
      g.setColour(outlineColour);
      g.fillPath (filledArc);
    }
    
    //if (thickness > 0)
    {
      g.saveState();
      
      const float innerRadius = radius - lineWidth - 2;
      Path shape;
      shape.addEllipse(-innerRadius+0.5f, -innerRadius+0.5f, 2*innerRadius-1, 2*innerRadius-1);
      
      {
        // inside
        ColourGradient gradient(colour,  centreX, centreY-innerRadius, 
                                colour.darker(),centreX, centreY+innerRadius, 
                                false);
        ColourGradient brush(gradient);
        g.setGradientFill(brush);
        g.fillPath (shape, AffineTransform::translation (centreX, centreY));
      }
      
      // higlight
      g.setFillType(ColourGradient(Colours::white.withAlpha(0.2f), centreX, centreY-innerRadius,
                                   Colours::transparentWhite, centreX, centreY, 
                                   false));
      g.strokePath(shape, PathStrokeType(2), AffineTransform::translation(centreX, centreY+1));
      
      // shadow
      g.setFillType(ColourGradient(Colours::transparentBlack, centreX, centreY,
                                   Colours::black.withAlpha(0.2f), centreX, centreY+innerRadius, 
                                   false));
      g.strokePath(shape, PathStrokeType(2), AffineTransform::translation(centreX, centreY+1));
      
      g.restoreState();
      
      //handle
      g.setColour(colour.contrasting(0.5f));
      Path handle;
      float handleWidth = jmax(2.f, innerRadius/4.f);
      handle.addRoundedRectangle(-handleWidth/2.f, -innerRadius, handleWidth , innerRadius, 2);
      g.fillPath (handle, AffineTransform::rotation(angle).scaled(0.8f, 0.8f).translated (centreX, centreY));      
      
      // outline
      g.setColour(Colours::black.withAlpha(0.5f));
      g.strokePath(shape, PathStrokeType(1), AffineTransform::translation(centreX, centreY));
      
    }
  }
}	  

