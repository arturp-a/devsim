/***
DEVSIM
Copyright 2013 Devsim LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***/

#include "Bernoulli.hh"
#include <cmath>
using std::abs;
//#include <climits>
#include <cstdlib>
//#include <values.h>
#include <limits>

const double lnmax = log(std::numeric_limits<double>().max());

#if 1
inline double Bernoulli_Taylor(const double x)
{
    double d = 1.0;
    double xv = x;;

    d += 1./2. * xv;
    xv *= x;
    d += 1./6. * xv;
    xv *= x;
    d += 1./24. * xv;
#if 0
    xv *= x;
    d += 1./120. * xv;
    xv *= x;
    d += 1./720. * xv;
    xv *= x;
    d += 1./5040. * xv;
    xv *= x;
    d += 1./40320. * xv;
    xv *= x;
    d += 1./362880. * xv;
    xv *= x;
    d += 1./3628800. * xv;
    xv *= x;
    d += 1./39916800. * xv;
    xv *= x;
    d += 1./479001600. * xv;
    xv *= x;
    d += 1./6227020800. * xv;
    xv *= x;
    d += 1./87178291200. * xv;
    xv *= x;
    d += 1./1307674368000. * xv;
    xv *= x;
    d += 1./20922789888000. * xv;
    xv *= x;
    d += 1./355687428096000. * xv;
    xv *= x;
    d += 1./6402373705728000. * xv;
    xv *= x;
    d += 1./121645100408832000. * xv;
    xv *= x;
    d += 1./2432902008176640000. * xv;
    xv *= x;
    d += 1./14197454024290336768. * xv;
#endif
    return 1.0 / d;

//    double y = 1 / (x+1/2*x^2+1/6*x^3+1/24*x^4+1/120*x^5+1/720*x^6+1/5040*x^7+1/40320*x^8+1/362880*x^9+1/3628800* x^10+1/39916800*x^11+1/479001600*x^12+1/6227020800*x^13+1/87178291200*x^14+1/1307674368000*x^15+1/20922789888000*x^16+1/355687428096000*x^17+1/6402373705728000*x^18+1/121645100408832000* x^19;
}

inline double Bernoulli_Regular(const double x)
{
    return x / (exp(x) - 1.0);
}

double Bernoulli(const double x)
{
    const double fx = abs(x);
    double ret;
    if (fx < 1.0e-4)
    {
        ret = Bernoulli_Taylor(x);
    }
    else if (fx < lnmax)
    {
        ret = Bernoulli_Regular(x);
    }
    else if (x < 0.0)
    {
        // B(-x) = B(x) + x
        ret = -x;
    }
    else
    {
        ret = 0.0;
        // exp(-lnmax) ~= 0.0
//      ret = x * exp(-x);
    }
    return ret;

}

// Expanded version
inline double derBernoulliExp(const double x)
{
    double num = -1./2.;
    double den = 1.0;
    double xv = x;
    num -= 1./3. * xv;
    den += xv; 
    xv *= x;
    num -= 1./8. * xv;
    den += 7./12. * xv;
    xv *= x;
    num -= 1./30. * xv;
    den += 1./4. * xv;
#if 0
    xv *= x;
    num -= 1./144. * xv;
    den += 31./360. * xv;
    xv *= x;
    num -= 1./840. * xv;
    den += 1./40. * xv;
    xv *= x;
    num -= 1./5760. * xv;
    den += 127./20160. * xv;
    xv *= x;
    num -= 1./45360. * xv;
    den += 17./12096. * xv;
    xv *= x;
    num -= 1./403200. * xv;
    den += 73./259200. * xv;
    xv *= x;
    num -= 1./3991680. * xv;
    den += 31./604800. * xv;
    xv *= x;
    num -= 1./43545600. * xv;
    den += 2047./239500800. * xv;
    xv *= x;
    num -= 1./518918400. * xv;
    den += 1./760320. * xv;
    xv *= x;
    num -= 1./6706022400. * xv;
    den += 8191./43589145600. * xv;
    xv *= x;
    num -= 1./93405312000. * xv;
    den += 5461./217945728000. * xv;
    xv *= x;
    num -= 1./1394852659200. * xv;
    den += 4681./1494484992000. * xv;
    xv *= x;
    num -= 1./22230464256000. * xv;
    den += 257./697426329600. * xv;
    xv *= x;
    num -= 1./376610217984000. * xv;
    den += 131071./3201186852864000. * xv;
    xv *= x;
    num -= 1./6758061133824000. * xv;
    den += 73./16937496576000. * xv;
#endif

    return num/den;
}

inline double derBernoulli3(double x)
{
//    return 1.0/(exp(x) - 1.0) - x * exp(x) / ((exp(x) - 1.0)*(exp(x)-1.0));
    //return (ex*(1.0-x) - 1.0) / pow((ex - 1.0),2.0);
    // The first expression blows up, so this one behaves better
    // since the pow function goes to zero instead of infinity
    //return (ex*(1.0-x) - 1.0) * pow((ex - 1.0), -2.0);
    const double ex = exp(x);
    const double ex1 = ex - 1;
    if (ex != ex1)
    { 
      //return (1.0 - x * ex / ex1) / ex1;
      return (ex*(1.0-x) - 1.0) / pow((ex - 1.0),2.0);
    }
    else
    {
      return (1.0-x) / ex;
    }
}

double derBernoulli(const double x)
{
    const double fx = abs(x);
    double ret;
    if (fx < 1.0e-4)
    {
        ret = derBernoulliExp(x);
    }
    else if (fx < lnmax)
    {
        ret = derBernoulli3(x);
    }
    else if (x < 0.0)
    {
        ret = -1.0;
    }
    else
    {
        // lnmax should mean exp(-x) = 0.
        ret = 0.0;
//      ret = (1.0 - x) * exp(-x);
    }

    return ret;
}

#else
double Bernoulli(double x)
{
    const double fx = std::abs(x);
    double ret;
    if ( fx < 1.0e-4 ) 
    {
        ret = 1.0 / (1.0 + 0.5 * x + x * x / 6.0);
    }
    else if ( fx < lnmax ) 
    {
        ret = x / (exp(x)-1.0);
    }
    else if ( x < 0.0 )
    {
        ret = -x;
    }
    else
    {
        ret = x * exp(-x);
    }
    return ret;
}

double derBernoulli(double x)
{
    const double fx = abs(x);
    double ret;
    if ( fx < 1.0e-4 )
    {
        ret = -(0.5+x/3.0+x*x/8.0)/(1.0+x+7.0*x*x/12.0);
    }
    else if ( fx < lnmax )
    {
        const double ex = exp(x);
        const double ex1 = ex - 1.0;
        ret = (ex1 - x * ex) / (ex1 * ex1);
    }
    else if ( x < 0.0 )
    {
        ret = - 1.0;
    }
    else
    {
        ret = (1-x)*exp(-x);
    }
    return ret;
}
#endif
