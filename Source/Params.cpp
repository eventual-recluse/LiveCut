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

/*****************************************************************************

        Params.cpp
        Copyright (c) 2004 [mdsp @ smartelectronix]

*Tab=3***********************************************************************/

#include	"Params.h"

#include	<cassert>
using namespace std;

void Param::init (ParamList &list, float min, float def, float max, std::string name, std::string unit, long tag_, char cc_, Fun<float> *f)
{
  m_tag = tag_;
  m_cc = cc_;
  m_name = name; 
  m_unit = unit;
  this->min = min;
  this->max = max;
  m_def = def;
  value = def;
  list.insert(ParamList::value_type(m_tag,this));
  fun.f=f;
}
void Param::init (ParamList &list, long min, long def, long max, std::string name, std::string unit, long tag_, char cc_, Fun<long> *f)
{
  m_tag = tag_;
  m_cc = cc_;
  m_name = name;
  m_unit = unit;
  this->min = min;
  this->max = max;
  m_def = def;
  value = def;
  list.insert(ParamList::value_type(m_tag,this));
  fun.i=f;
}
void Param::init (ParamList &list, bool def, std::string name, std::string unit, long tag_, char cc_, Fun<bool> *f)
{
  m_tag = tag_;
  m_cc = cc_;
  m_name = name;
  m_unit = unit;
  min = false;
  max = true;
  m_def = def;
  value = def;
  list.insert(ParamList::value_type(m_tag,this));
  fun.b=f;
}
Param& Param::name(string text) 
{
    m_name = text;
    return *this;
}
Param& Param::unit(string text) 
{
    m_unit = text;
    return *this;
}
Param& Param::range(float min, float max) 
{
    this->min = min;
    this->max = max;
    if(float(value)<float(min))
		value = min;
    if(float(value)>float(max))
		value = max;
    return *this;
}
Param& Param::range(long min, long max) 
{
    this->min = min;
    this->max = max;
    if(long(value)<long(min))
		value = min;
    if(long(value)>long(max))
		value = max;
    return *this;
}
Param& Param::def(float val) 
{
    m_def = val;
    value = val;
    return *this;
}
Param& Param::def(long val) 
{
    m_def = val;
    value = val;
    return *this;
}
Param& Param::def(bool val) 
{
    min = false;
    max = true;
    m_def = val;
    value = val;
    m_style = eSwitch;
    return *this;
}
Param& Param::tag(long v) 
{
    m_tag = v;
    return *this;
}
Param& Param::cc(long v) 
{
    m_cc = v;
    return *this;
}
Param& Param::functor(Fun<float> *f) 
{
    fun.f=f;
    return *this;
}
Param& Param::functor(Fun<long> *f) 
{
    fun.i=f;
    return *this;
}
Param& Param::functor(Fun<bool> *f) 
{
    fun.b=f;
    return *this;
}

string	Param::GetDisplay ()
{
	std::ostringstream out;
	//out.precision(6);
	//out << showpoint;
	//out.width(10);
   switch(value.type)
   {
   case TYPEFLOAT:
        out << float(value);
        break;
   case TYPELONG:   
        out << long(value);
        break;
    case TYPEBOOL:
		if(bool(value) == true)
			out << "true";
		else
			out << "false";
        break;
	 case TYPEUNKNOWN:
		 out << "Unknown";
    }    
	return out.str();
}
void	Param::FromDisplay(string s)
{
	istringstream in(s);
    switch(value.type)
    {
    case TYPEFLOAT: 
        {
            float v=0.f;
            in >> v;	
            if(!in.fail())
                value.u.f = v;
            break;
        }
    case TYPELONG:   
        {
            long v = 0;
            in >> v;	
            if(!in.fail())
                value.u.i = v;
            break;
        }
    case TYPEBOOL:  
        if(s == string("true"))
            value.u.b = true;	
        else
            value.u.b = false;
        break;
    case TYPEUNKNOWN: value.u.i = 0;	break;
    }
    Clip();
    this->TellListeners();
}
float	Param::GetNormalized(Atom v)
{
    float norm = 0.0f;
    switch(v.type)
    {
    case TYPEFLOAT: 
        norm = (float(v)-float(min))/(float(max)-float(min));
        break;
    case TYPELONG:   
        norm = float(long(v)-long(min))/float(long(max)-long(min));
        break;
    case TYPEBOOL:   
		if(bool(v) == true)
			norm = 1.0f;
		else
			norm = 0.0f;
        break;
	 case TYPEUNKNOWN:
		 norm = 0.f;
		 break;
    }
    return norm;
}
void    Param::SetFromNormalized(float norm)
{
    switch(value.type)
    {
    case TYPEFLOAT: 
        value = float(min)+norm*(float(max)-float(min));
        break;
    case TYPELONG:   
		value = long(float(long(min))+norm*float(long(max)-long(min))/*+0.5f*/);
        break;
    case TYPEBOOL:
		if(norm>0.5f)
			value = true;
		else
			value = false;
        break;
	 case TYPEUNKNOWN:
		 break;
    }
	 Clip();
	 this->TellListeners();
}
void Param::Clip()
{
   switch(value.type)
    {
	case TYPEFLOAT: 
		if(float(value)<float(min))	value = float(min);
		if(float(value)>float(max))	value = float(max);
		break;
    case TYPELONG:
		if(long(value)<long(min))	value = long(min);
		if(long(value)>long(max))	value = long(max);
		break;
    case TYPEBOOL:        break;
	 case TYPEUNKNOWN:
		 break;
    }
}
void	Param::TellListeners()
{
   switch(value.type)
    {
	case TYPEFLOAT:
        if(fun.f)
    		fun.f->call(float(value));
		break;
    case TYPELONG:
        if(fun.i)
    		fun.i->call(long(value));
		break;
    case TYPEBOOL:        
        if(fun.b)
    		fun.b->call(bool(value));
		break;
	 case TYPEUNKNOWN:
		 break;
    }
	dirty = false;
}

string	Param::toString()
{
	std::ostringstream out;
	out << Param::GetDisplay();
	return out.str();
}
void	Param::fromString(string s)
{
	istringstream in(s);
	//string localname;
	//in >> localname;
	//assert(localname==name);
	
    switch(value.type)
    {
    case TYPEFLOAT: 
        {
            float v=0.f;
            in >> v;
            if(!in.fail())
                value.u.f = v;	
            break;
        }
    case TYPELONG:   
        {
            long v=0.f;
            in >> v;
            if(!in.fail())
                value.u.i = v;	
            break;
        }
    case TYPEBOOL:  
        {
            if(s == "true")
                value.u.b = true;
            else
                value.u.b = false;
            break;
        }
    case TYPEUNKNOWN: value.u.i = 0;	break;
    }
    Clip();
    this->TellListeners();	
}
float	Param::GetNormalized()
{
    return GetNormalized(value);
}
float	Param::GetMinNormalized()
{
    return GetNormalized(min);
}
float	Param::GetDefaultNormalized()
{
    return GetNormalized(m_def);
}
float	Param::GetMaxNormalized()
{
    return GetNormalized(max);
}

