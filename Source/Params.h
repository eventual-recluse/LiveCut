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

#if ! defined (Params_HEADER_INCLUDED)
#define	Params_HEADER_INCLUDED

#include "Functor.h"
#include <string>
#include <cmath>
#include <map>
#include <vector>
#include <sstream>
#include <cassert>
using std::string;
using std::unary_function;

enum ControllerType
{
  eHSlider,
  eVSlider,
  eKnob,
  eSwitch,
  eKick,
	eEnum,
  
  eNumControls
};

enum Type
{
  TYPELONG,
  TYPEFLOAT,
  TYPEBOOL,
  TYPEUNKNOWN
};

struct Atom
{
	union
  {
		float	f;
		long	i;
		bool	b;
	} u;
  
	Type type;
  
  
	Atom(double v)	{u.f=float(v); type = TYPEFLOAT;}
	Atom(float v)	{u.f=v; type = TYPEFLOAT;}
	Atom(int	v)	{u.i=v; type = TYPELONG;}
	Atom(bool v)	{u.b=v; type = TYPEBOOL;}
	Atom()			{u.i = 0; type = TYPEUNKNOWN;}
	inline float	AsFloat()
	{
		switch(type)
		{
      case TYPEFLOAT:		return float(u.f);
      case TYPELONG:		return float(u.i);
      case TYPEBOOL:		return float(u.b);
      case TYPEUNKNOWN:	
      default:
        return float(0.f);
		}
	}
	inline void		FromFloat(float v)
	{
		switch(type)
		{
      case TYPEFLOAT: u.f = float(v);	break;
      case TYPELONG:	u.i = long(v);	break;
      case TYPEBOOL:	u.b = (v>0.5f)?true:false;	break;
      case TYPEUNKNOWN: break;
      default:break;
		}
	}
	inline float	GetFloat(){return u.f;}
	inline long		GetLong() {return u.i;}
	inline bool		GetBool() {return u.b;}
	inline Type		GetType(){return type;}
  inline operator float() {return u.f;}
  inline operator bool()  {return u.b;}
  inline operator long()  {return u.i;}
  inline operator int()   {return u.i;}
  inline operator Type()  {return type;}
  inline Atom& operator=(double v){u.f = float(v);  type = TYPEFLOAT; return *this;}
  inline Atom& operator=(float v) {u.f = v;         type = TYPEFLOAT; return *this;}
  inline Atom& operator=(long v)  {u.i = v;         type = TYPELONG;  return *this;}
  inline Atom& operator=(int v)   {u.i = long(v);   type = TYPELONG; return *this;}
  inline Atom& operator=(bool v)  {u.b = v;         type = TYPEBOOL; return *this;}
  /*
   friend inline double operator=(double &v, Atom& a);
   friend inline float operator=(float &v,  Atom& a);
   friend inline long  operator=(long  &v,  Atom& a);
   friend inline int   operator=(int   &v,  Atom& a);
   friend inline bool  operator=(bool  &v,  Atom& a);
   */
};

/*
 inline double operator=(double &v, Atom& a){v = double(a);}
 inline float operator=(float &v,  Atom& a){v = float(a);}
 inline long operator=(long  &v,  Atom& a){v = long(a);}
 inline int  operator=(int   &v,  Atom& a){v = int(a);}
 inline bool operator=(bool  &v,  Atom& a){v = bool(a);}
 */
template<typename T>
inline void SetAtom(Atom& a, T v)	    {a = v;}

class IParam
{
public:
	virtual string	GetDisplay()=0;
	virtual string	GetName()=0;
	virtual string  GetUnit()=0;
	virtual Atom	GetValue()=0;
	virtual void	SetValue(Atom &a)=0;
	virtual float	GetFloatValue()=0;
	virtual long	GetIntValue()=0;
	virtual bool	GetBoolValue()=0;
	virtual float   GetNormalized()=0;
	virtual long	GetTag()=0;
	virtual char	GetCC()=0;
	virtual void    SetFromNormalized(float norm)=0;
	virtual void	SetDirty(bool dirty_)=0;
	virtual bool	IsDirty()=0;
	virtual string	toString()=0;
	virtual void	fromString(string s)=0;
};


class Param;

typedef std::map<long,  Param *>    ParamList;
typedef std::map<string,Param *>    ParamNameMap;
typedef std::map<char,  long>       CcMap;

class Param : public IParam
{
	virtual float	GetNormalized(Atom v);
public:
	Param()
  :m_name(""), m_unit(""), m_description("")
  ,m_tag(0), m_cc(-1), m_style(0)
  ,dirty(false)	
	{
    fun.f = NULL;
  }

	Param (ParamList &list,float min,float def,float max,string name,string unit,long tag_, char cc_,Fun<float> *f=0)
	{
		fun.f=f;
		init(list,min,def,max,name,unit,tag_,cc_);
	}
	
  Param (ParamList &list,long min,long def,long max,string name,string unit,long tag_, char cc_,Fun<long> *f=0)
	{
		fun.i=f;
		init(list,min,def,max,name,unit,tag_,cc_);
	}
	
  Param (ParamList &list,bool def, string name,string unit,long tag_, char cc_,Fun<bool> *f=0)
	{
		fun.b=f;
		init(list,def,name,unit,tag_,cc_);
	}
	
  virtual	~Param (){}
	
  virtual void	init (ParamList &list,float min,float def,float max,string name,string unit,long tag_, char cc_,Fun<float> *f=0);
	
  virtual void	init (ParamList &list,long min,long def,long max,string name,string unit,long tag_, char cc_,Fun<long> *f=0);
	
  virtual void	init (ParamList &list,bool def, string name,string unit,long tag_, char cc_,Fun<bool> *f=0);
  
  virtual Param&  name(string text);
  virtual Param&  unit(string text);
  virtual Param&  range(float min, float max);
  virtual Param&  range(long min, long max);
  virtual Param&  description(string text) {m_description=text; return *this;}
  virtual Param&  style(long v){m_style = v; return *this;}
  virtual Param&  def(float val);
  virtual Param&  def(long val);
  virtual Param&  def(bool val);
  virtual Param&  tag(long v);
  virtual Param&  cc(long v);
  virtual Param&  functor(Fun<float> *f);
  virtual Param&  functor(Fun<long> *f);
  virtual Param&  functor(Fun<bool> *f);
  
  virtual string  description() {return m_description;}
  virtual long    style(){return m_style;}
  
  virtual string	GetDisplay(); 
	virtual void	FromDisplay(string s); 
	virtual string	GetName()	{return m_name;}
	virtual string	GetUnit()	{return m_unit;}
	virtual void	SetValue(Atom &a) {value = a; Clip(); TellListeners();}
	virtual void	SetValueFromFloat(float v) {value.FromFloat(v); Clip(); TellListeners();}
	virtual Atom	GetValue()	{return value;} 
	virtual Atom	GetMin() {return min;} 
	virtual Atom	GetDefault() {return m_def;} 
	virtual Atom	GetMax() {return max;} 
	virtual float	GetNormalized();
	virtual float	GetMinNormalized();
	virtual float	GetDefaultNormalized();
	virtual float	GetMaxNormalized();
	virtual long	GetTag() {return m_tag;}
	virtual char	GetCC() {return m_cc;}
	virtual Type	GetType(){return value.GetType();}
	virtual void	SetFromNormalized(float norm); 
	virtual void	SetDirty(bool dirty_) {dirty = dirty_;}
	virtual bool	IsDirty(){return dirty;}
	virtual string	toString();
	virtual void	fromString(string s);
	virtual float	GetFloatValue() {return float(value);} 
	virtual long	GetIntValue()  {return long(value);} 
	virtual bool	GetBoolValue() {return bool(value);}
	virtual void	SetFunctor(Fun<float> *f){fun.f = f;}
	virtual void	SetFunctor(Fun<long> *f){fun.i = f;}
	virtual void	SetFunctor(Fun<bool> *f){fun.b = f;}
	virtual void	Clip();
	virtual void	TellListeners();
  
protected:
	Atom    value;
	Atom	min;
	Atom    m_def;
	Atom	max;
	string	m_name;
	string	m_unit;
  string  m_description;
  long    m_style;
	long	m_tag;    
	char	m_cc;
	bool  dirty;
	union {
		Fun<float>  *f;
		Fun<long>  *i;
		Fun<bool>  *b;
	} fun;
};	// class Param

class ParamMap : public std::map<long,Param *>
{
public:
  ParamMap& add(Param &param)
  {
    if( count(param.GetTag()) ) 
    {
      long newtag = rbegin()->second->GetTag() + 1;
      param.tag(newtag);
    }
    if(param.GetCC()<0)
    {
      long newcc = 0;
      if(!empty())
        newcc = rbegin()->second->GetCC() + 1;
      param.cc(newcc);
    }
    (*this)[param.GetTag()] = &param;
    
    return *this;
  }
};


class Freq : public Param
{
public:
  Freq (ParamList &list,float min,float def,float max,string name,long tag_, char cc_,Fun<float> *f=0): Param(list,min,def,max,name,"Hz",tag_,cc_,f) {}
	virtual float	GetNormalized() {return (log10(float(value))-log10(float(min)))/(log10(float(max))-log10(float(min)));}
	virtual void    SetFromNormalized(float norm) {value = pow(10.0f,log10(float(min))+norm*(log10(float(max))-log10(float(min))));} 
};	// class Freq

class OnOffParam : public Param
{
public:
  OnOffParam(ParamList &list,bool def,string name,long tag_,char cc_,Fun<bool> *f=0): Param(list,def,name,"",tag_,cc_,f) {}
  
	virtual string GetDisplay() 
	{
		if(bool(value) == true)
			return string("On");
		else
			return string("Off");
	} 
};

class EnumParam : public Param
{
	std::vector<string> names;
public:
	EnumParam()
  {
    min = 0L;
    max = 0L;
    value = 0L;
    m_def = 0L;
		m_style = eEnum;
  }
	EnumParam(ParamList &list,string name,long tag_,char cc_,Fun<long> *f=0){init(list,name,tag_,cc_,f);}
	void init(ParamList &list,string name,long tag_,char cc_,Fun<long> *f=0){Param::init(list,0L,0L,0L,name,"",tag_,cc_,f);}
	virtual string GetDisplay() {return names.at(long(value));}
	virtual void FromDisplay(string s)
	{
		fromString(s);
	}
	virtual void fromString(string s)
  {	
    //find the index of the value in the array
    long index=0;
    while(index<names.size())
    {
      if(names[index] == s)
        break;
      ++index;
    }
    
    // TODO: find the nearest string if no match
    if(index >= names.size())
      index = 0;
    
    value.u.i = index;	
    Clip();
    this->TellListeners();
  }
	virtual string toString ()
	{
		return GetDisplay();
	}	
	
	void AddNames(std::vector<string> &v) 
	{
		for(unsigned int i=0;i<v.size();i++) 
      names.push_back(v[i]); 
		max = long(names.size()-1);
	}
	EnumParam& AddEntry(string v) 
	{
		names.push_back(v); 
		max = long(names.size()-1);
		return *this;
	}
};

class IntSetParam : public Param
{
  std::vector<long> values;
public:
	IntSetParam()
	{
		min   = 0L;
		max   = 0L;
		m_def = 0L;
		value = 0L;
	}
	IntSetParam(ParamList &list,std::vector<long> vals,string name,string unit,long tag_,char cc_,Fun<long> *f=0)
  {
    init(list,vals,0,name,unit,tag_,cc_,f);
  }
	void init(ParamList &list,std::vector<long> vals,long def,string name,string unit,long tag_,char cc_,Fun<long> *f=0)
  {
    Param::init(list,0L,def,long(vals.size()-1),name,unit,tag_,cc_,f);values = vals;
  }
	virtual void FromDisplay(string s)
	{
		fromString(s);
	}
	virtual void fromString(string s)
	{
		std::istringstream in(s);
		long v=0.f;
		in >> v;
		
		if(!in.fail())
		{
			//find the index of the value in the array
			long index=0;
			while(index<values.size())
			{
				if(values[index] == v)
					break;
				++index;
			}
			if(index >= values.size())
				index = 0;
			value.u.i = index;	
			Clip();
			this->TellListeners();
		}
	}
	IntSetParam& AddEntry(long v) 
	{
		values.push_back(v); 
		max = long(values.size()-1);
		return *this;
	}	
	void AddValues(std::vector<long> &v) 
	{
		values = v; 
		max = long(values.size()-1);
	}	
	virtual void SetFromNormalized(float norm)
	{
	  value = long(float(long(min))+norm*float(long(max)-long(min))/*+0.5f*/);
	  Clip();
	  TellListeners();
	}
	virtual void TellListeners()
  {
    long v = values[long(value)]; 
		fun.i->call(v);
	}
  virtual void SetValue(Atom &a) 
  {
    value = a; 
    Clip(); 
    TellListeners();
  }
	virtual string GetDisplay ()
	{
		std::ostringstream out;
    //		out.precision(6);
    //		out << std::showpoint;
    //		out.width(10);
    out << values[long(value)];
		return out.str();	
	}
	virtual string toString ()
	{
		return GetDisplay();
	}	
	virtual long GetIntValue() {return values[long(value)];} 
};

class FloatSetParam : public Param
{
	std::vector<float> values;
public:
	FloatSetParam()
	{		
		min   = 0L;
		max   = 0L;
		m_def = 0L;
		value = 0L;
	}
	FloatSetParam(ParamList &list,std::vector<float> vals,string name,string unit,long tag_,char cc_,Fun<float> *f=0)
  {
    init(list,vals,0,name,unit,tag_,cc_,f);
  }
	void init(ParamList &list,std::vector<float> vals,long def,string name,string unit,long tag_,char cc_,Fun<float> *f=0)
  {
    Param::init(list,0L,def,long(vals.size()-1),name,unit,tag_,cc_,f); values = vals;
  }
	virtual void FromDisplay(string s)
	{
		fromString(s);
	}
	virtual void fromString(string s)
	{
		std::istringstream in(s);
		float v=0.f;
		in >> v;
    
		if(!in.fail())
		{
			//find the index of the value in the array
			long index=0;
			while(index<values.size())
			{
				if(values[index] == v)
					break;
				++index;
			}
			if(index >= values.size())
				index = 0;
			value.u.i = index;	
			Clip();
			this->TellListeners();
		}
	}
	FloatSetParam& AddEntry(float v) 
	{
		values.push_back(v); 
		max = long(values.size()-1);
		return *this;
	}	
	void AddValues(std::vector<float> &v) 
	{
		values = v;
		max = long(values.size()-1);
	}	
	virtual void TellListeners()
	{
	  if(fun.f)
	    fun.f->call(values[long(value)]);
	}
  virtual void SetValue(Atom &a) 
  {
    value = a; 
    Clip(); 
    TellListeners();
  }
	virtual string GetDisplay ()
	{
		std::ostringstream out;
    out << values[long(value)];
		return out.str();	
	}
	virtual string toString ()
	{
		return GetDisplay();
	}
	virtual float GetFloatValue() {return values[long(value)];} 
};

class AutoParam : public Param
{
public:
  /*
   void	init (ParamList &list,float min,float	def,float	max,string name,string unit,long tag_, char cc_,Fun<float> *f=0)
   {Param::init(list,min,def,max,name,unit,tag_,cc_,f)}
   void	init (ParamList &list,long	min,long		def,long		max,string name,string unit,long tag_, char cc_,Fun<long> *f=0)
   {Param::init(list,min,def,max,name,unit,tag_,cc_); fi = f;}
   void	init (ParamList &list,bool	def, string name,string unit,long tag_, char cc_,Fun<bool> *f=0)
   {Param::init(list,def,name,unit,tag_,cc_); fb = f;}
   */
};

//----------------------------------------------------------------------------------------------------------------
// mapping
//----------------------------------------------------------------------------------------------------------------

class LinearMapping 
{
  float y0,y1;
public:
  LinearMapping(){}
  LinearMapping(float min,float max)
  :y0(min),y1(max)
  {
    assert(y1>0.f);
  }
  float FromNormalized(float x){return y0+(y1-y0)*x;}
  float ToNormalized(float y)  {return (y-y0)/(y1-y0);}
};

class ExponentialMapping 
{
  float y0,a,b;
public:
  ExponentialMapping(){}
  ExponentialMapping(float min,float mid,float max)
  {
    init(min,mid,max);
  }
  void init(float min,float mid,float max)
  {
    assert(min<mid && mid<max);
    y0=min;
    b = log( (max-min)/(mid-min) ) / 2.f;
    a = exp((log(mid-min)+log(max-min)-b*1.5f) / 2.f);
    assert(a > 0.f);
    assert(b > 0.f);
  }
  float FromNormalized(float x)
  {
    return y0 + a * exp( b*x );
  }
  float ToNormalized(float y)
  {
    return log( (y-y0) / a ) / b;
  }
};

class PowerMapping 
{
  float y0,y1,a;
public:
  PowerMapping(){}
  PowerMapping(float min_,float mid_,float max_)
  {
    init(min_,mid_,max_);
  }
  void init(float min_,float mid_,float max_)
  {
    assert(min_<mid_ && mid_<max_);
    y0 = min_;
    y1 = max_;
    a = log((mid_-min_)/(max_-min_)) / log(0.5f);      
  }
  float FromNormalized(float x)
  {
    return y0 + (y1-y0)*pow(x,a);
  }
  float ToNormalized(float y)  
  {
    return pow((y-y0)/(y1-y0),1.f/a);
  }
};


//
class ExponentialParam : public Param
{
  ExponentialMapping mapping;
public:
  ExponentialParam (){}
  ExponentialParam (ParamList &list,float min,float def,float mid,float max,string name,long tag_, char cc_,Fun<float> *f=0)
  : Param(list,min,def,max,name,"Hz",tag_,cc_,f) 
  ,mapping(min,mid,max)
  {}
  void	init (ParamList &list,float min,float def,float mid,float max,string name,string unit,long tag_, char cc_,Fun<float> *f=0)
  {
    Param::init(list,min,def,max,name,unit,tag_,cc_,f);
    mapping.init(min,mid,max);
  }
  virtual float GetNormalized() 
  {
    return mapping.ToNormalized(float(value));
  }
  virtual void  SetFromNormalized(float norm) 
  {
    value = mapping.FromNormalized(norm);
    Clip();
	  TellListeners();
  } 
};

class PowerParam : public Param
{
  PowerMapping mapping;
public:
  PowerParam (){}
  PowerParam (ParamList &list,float min,float def,float mid,float max,string name,long tag_, char cc_,Fun<float> *f=0)
  : Param(list,min,def,max,name,"Hz",tag_,cc_,f) 
  ,mapping(min,mid,max)
  {}
  virtual void	init (ParamList &list,float min,float def,float mid,float max,string name,string unit,long tag_, char cc_,Fun<float> *f=0)
  {
    Param::init(list,min,def,max,name,unit,tag_,cc_,f);
    mapping.init(min,mid,max);
  }
  void middle(float mid)
  {
    mapping.init(float(min),mid,float(max));
  }
  virtual float GetNormalized() 
  {
    return mapping.ToNormalized(float(value));
  }
  virtual void  SetFromNormalized(float norm) 
  {
    value = mapping.FromNormalized(norm);
    Clip();
	  TellListeners();
  } 
};


#endif

