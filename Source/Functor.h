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

#ifndef FUNCTOR_H
#define FUNCTOR_H

#include <functional>

template<class T,class R>
struct nop : public std::unary_function<T,R>
{	
    R operator()(const T val) const
	{	
	    return;
	}
};

template<class T>
struct Fun
{
    virtual void call(T v) = 0;
};

template<class Fn>
struct MemFun : public Fun<typename Fn::argument_type>
{
    Fn fun;
    MemFun(Fn fun):Fun<typename Fn::argument_type>(),fun(fun){}
    virtual void call(typename Fn::argument_type v)
    {
        fun(v);
    }
};

template<class Fn> 
inline MemFun< Fn > *newfun(Fn f)
{	
    return new MemFun< Fn >(f);
}

// functor adapter (*p->*pfunc)(val,val2), non-const *pfunc
template<class R,class T,class A1,class A2>
class mem_fun2_t : public std::binary_function<A1, A2, R>
{
public:
	explicit mem_fun2_t(T *o,R (T::*pm)(A1,A2))
	: method(pm),obj(o)
	{
	}
    R operator()(A1 left, A2 right) const
	{
	    return ((obj->*method)(left,right));
	}
private:
	R (T::*method)(A1,A2); // the member function pointer
    T *obj; //this pointer
};

template<class R,class T,class A1,class A2> 
inline mem_fun2_t<R,T,A1,A2> memfun2(T *o,R (T::*pm)(A1,A2))
{	
    return (mem_fun2_t<R,T,A1,A2>(o,pm));
}

#endif
