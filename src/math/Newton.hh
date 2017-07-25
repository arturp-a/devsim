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

#ifndef DS_NEWTON_HH
#define DS_NEWTON_HH
#include "MatrixEntries.hh"
#include "dsMathTypes.hh"
#include "MathEnum.hh"
#include <cstddef>
#include <vector>
#include <complex>
#include <map>

class ObjectHolder;
typedef std::map<std::string, ObjectHolder> ObjectHolderMap_t;

class Device;
/// This is the outer nonlinear solver
namespace dsMath {
class Matrix;
class LinearSolver;
namespace TimeMethods {
    enum TimeMethod_t {DCONLY, INTEGRATEDC, INTEGRATETR, INTEGRATEBDF1, INTEGRATEBDF2};
    struct TimeParams {
      TimeParams(TimeMethod_t tm, double ts, double g) : method(tm), tstep(ts), gamma(g), tdelta(0.0), a0(0.0), a1(0.0), a2(0.0), b0(1.0), b1(0.0), b2(0.0) {
      }

    // strictly dc
    bool IsDCOnly() const {
      return (method == DCONLY);
    }
    // could be dc transient too.
    bool IsDCMethod() const {
      return (method == DCONLY) || (method == INTEGRATEDC);
    }

    // can be dc transient too
    bool IsTransient() const {
      return (method != DCONLY);
    }

    bool IsIntegration() const {
      return (method == INTEGRATEBDF1) || (method == INTEGRATETR) || (method == INTEGRATEBDF2);
    }
      TimeMethod_t method;
      double tstep;
      double gamma;
      double tdelta;
      double a0;
      double a1;
      double a2;
      double b0;
      double b1;
      double b2;
    };

struct DCOnly : public TimeParams
{
  DCOnly() : TimeParams(DCONLY, 0.0, 0.0)
  {
    b0 = 1.0;
  }
};

struct TransientDC : public TimeParams
{
  TransientDC() : TimeParams(INTEGRATEDC, 0.0, 0.0)
  {
    b0 = 1.0;
  }
};

struct BDF1 : public TimeParams
{
  BDF1(double tstep, double gamma) : TimeParams(INTEGRATEBDF1, tstep, gamma)
  {
    tdelta = gamma * tstep;
    const double tf = 1.0 / tdelta;
    a0 = tf;
    a1 = -tf;
    b0 = 1.0;
  }
};

struct BDF2 : public TimeParams
{
  BDF2(double tstep, double gamma) : TimeParams(INTEGRATEBDF2, tstep, gamma)
  {
    //// td for first order projection
    tdelta = (1.0 - gamma) * tstep;
    a0 = (2.0 - gamma) / tdelta;
    a1 = (-1.0) / (gamma * tdelta);
    a2 = (1.0 - gamma) / (gamma * tstep);
    b0 = 1.0;
  }
};

struct TR : public TimeParams
{
  TR(double tstep, double gamma) : TimeParams(INTEGRATETR, tstep, gamma)
  {
    tdelta = gamma * tstep;
    const double tf = 2.0 / tdelta;
    a0 = tf;
    a1 = -tf;
    b0 = 1.0;
    b1 = -1.0;
  }
};

}


class Newton {
    public:
        typedef std::vector<size_t> permvec_t;

        /// Newton takes on linear solver
        /// near solver selects Preconditioner
        Newton() : maxiter(DefaultMaxIter), absLimit(DefaultAbsError), relLimit(DefaultRelError), qrelLimit(DefaultQRelError), dimension(0) {}
        ~Newton() {};

        //// INTEGRATE_DC means that we are just gonna Assemble I, Q when done

        bool Solve(LinearSolver &, const TimeMethods::TimeParams &, ObjectHolderMap_t *ohm);

        bool ACSolve(LinearSolver &, double);

        bool NoiseSolve(const std::string &, LinearSolver &, double);
        //Newton(LinearSolver &iterator);
        void SetAbsError(double x)
        {
            absLimit = x;
        }
        void SetRelError(double x)
        {
            relLimit = x;
        }
        void SetQRelError(double x)
        {
            qrelLimit = x;
        }
        void SetMaxIter(size_t x)
        {
            maxiter = x;
        }
    protected:
        template <typename T>
        void LoadIntoMatrix(const RealRowColValueVec<double> &rcv, Matrix &matrix, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoMatrixPermutated(const RealRowColValueVec<double> &rcv, Matrix &matrix, const permvec_t &, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoRHS(const RHSEntryVec<double> &, std::vector<T> &, T scl = 1.0, size_t offset = 0);
        template <typename T>
        void LoadIntoRHSPermutated(const RHSEntryVec<double> &, std::vector<T> &, const permvec_t &, T scl = 1.0, size_t offset = 0);
    private:

        void InitializeTransientAssemble(const TimeMethods::TimeParams &, size_t, std::vector<double> &);
        bool CheckTransientProjection(const TimeMethods::TimeParams &, const std::vector<double> &);
        void UpdateTransientCurrent(const TimeMethods::TimeParams &, size_t, const std::vector<double> &, std::vector<double> &);

        void PrintDeviceErrors(const Device &device, ObjectHolderMap_t *);
        void PrintCircuitErrors(ObjectHolderMap_t *);
        void PrintNumberEquations(size_t, ObjectHolderMap_t *);
        void PrintIteration(size_t, ObjectHolderMap_t *);

        size_t NumberEquationsAndSetDimension();

        void BackupSolutions();
        void RestoreSolutions();

        template <typename T>
        void LoadMatrixAndRHS(Matrix &, std::vector<T> &, permvec_t &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode, T);

        //// TODO: may be more efficient to reuse matrix and scale imaginary elements
        void LoadMatrixAndRHSAC(Matrix &, ComplexDoubleVec_t &, permvec_t &, double);
        void LoadCircuitRHSAC(ComplexDoubleVec_t &);

        void LoadMatrixAndRHSOnCircuit(RealRowColValueVec<double> &, RHSEntryVec<double> &rhs, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void AssembleContactsAndInterfaces(RealRowColValueVec<double> &, RHSEntryVec<double> &, permvec_t &, Device &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);
        //// This one can't permutate anything
        void AssembleBulk(RealRowColValueVec<double> &, RHSEntryVec<double> &, Device &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        void AssembleTclEquations(RealRowColValueVec<double> &, RHSEntryVec<double> &, dsMathEnum::WhatToLoad, dsMathEnum::TimeMode);

        static const size_t DefaultMaxIter;
        static const double DefaultAbsError;
        static const double DefaultRelError;
        static const double DefaultQRelError;

        Newton(const Newton &);

        size_t maxiter; /// The maximum number of iterations
        double absLimit;  /// The calculated abs error (maybe come on per device or per region basis)
        double relLimit;  /// The calculated rel error
        double qrelLimit;


        size_t dimension;

        static const double rhssign;
};
}
#endif

