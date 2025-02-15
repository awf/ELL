////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VectorOperations.h"
#include "ComputeContext.h"
#include "EmitterContext.h"
#include "FunctionDeclaration.h"
#include "LLVMContext.h"
#include "Scalar.h"
#include "Vector.h"

#include <emitters/include/IRModuleEmitter.h>

#include <math/include/BlasWrapper.h>

namespace ell
{
using namespace utilities;

namespace value
{
    Scalar Sum(Vector input)
    {
        Scalar result = Allocate(input.GetType(), ScalarLayout);

        For(input, [&](auto index) { result += input(index); });

        return result;
    }

    Scalar Dot(Vector v1, Vector v2)
    {
        if (v1.Size() != v2.Size())
        {
            throw InputException(InputExceptionErrors::sizeMismatch);
        }
        if (v1.GetType() != v2.GetType())
        {
            throw InputException(InputExceptionErrors::typeMismatch);
        }

        auto defaultImpl = [](Vector v1, Vector v2) {
            Scalar result = Allocate(v1.GetType(), ScalarLayout);
            For(v1, [&](auto index) {
                result += v1[index] * v2[index];
            });

            return result;
        };

        if (v1.GetType() == ValueType::Float)
        {
            auto fn = DeclareFunction("cblas_sdot")
                          .Returns(Value({ ValueType::Float, 0 }, ScalarLayout))
                          .Parameters(
                              Value({ ValueType::Int32, 0 }, ScalarLayout), /*n*/
                              v1, /*x*/
                              Value({ ValueType::Int32, 0 }, ScalarLayout), /*incx*/
                              v2, /*y*/
                              Value({ ValueType::Int32, 0 }, ScalarLayout)); /*incy*/

            auto result = InvokeForContext<ComputeContext>([&] {
                auto wrapper = fn.Define([](Scalar n, Vector x, Scalar incx, Vector y, Scalar incy) -> Scalar {
                    return math::Blas::Dot(n.Get<int>(), x.GetValue().Get<float*>(), incx.Get<int>(), y.GetValue().Get<float*>(), incy.Get<int>());
                });

                return wrapper(
                    static_cast<int>(v1.Size()),
                    v1,
                    static_cast<int>(v1.GetValue().GetLayout().GetCumulativeIncrement(0)),
                    v2,
                    static_cast<int>(v2.GetValue().GetLayout().GetCumulativeIncrement(0)));
            });

            if (result)
            {
                return *result;
            }

            result = InvokeForContext<LLVMContext>([&](LLVMContext& context) -> Scalar {
                if (context.GetModuleEmitter().GetCompilerOptions().useBlas)
                {
                    auto returnValue = fn.Decorated(FunctionDecorated::No)
                                           .Call(
                                               Scalar{ static_cast<int>(v1.Size()) },
                                               v1,
                                               Scalar{ static_cast<int>(v1.GetValue().GetLayout().GetCumulativeIncrement(0)) },
                                               v2,
                                               Scalar{ static_cast<int>(v2.GetValue().GetLayout().GetCumulativeIncrement(0)) });

                    return *returnValue;
                }
                else
                {
                    return defaultImpl(v1, v2);
                }
            });

            return *result;
        }
        else if (v1.GetType() == ValueType::Double)
        {
            auto fn = DeclareFunction("cblas_ddot")
                          .Returns(Value({ ValueType::Double, 0 }, ScalarLayout))
                          .Parameters(
                              Value({ ValueType::Int32, 0 }, ScalarLayout), /*n*/
                              v1, /*x*/
                              Value({ ValueType::Int32, 0 }, ScalarLayout), /*incx*/
                              v2, /*y*/
                              Value({ ValueType::Int32, 0 }, ScalarLayout)); /*incy*/

            auto result = InvokeForContext<ComputeContext>([&] {
                auto wrapper = fn.Define([](Scalar n, Vector x, Scalar incx, Vector y, Scalar incy) -> Scalar {
                    return math::Blas::Dot(n.Get<int>(), x.GetValue().Get<double*>(), incx.Get<int>(), y.GetValue().Get<double*>(), incy.Get<int>());
                });

                return wrapper(
                    static_cast<int>(v1.Size()),
                    v1,
                    static_cast<int>(v1.GetValue().GetLayout().GetCumulativeIncrement(0)),
                    v2,
                    static_cast<int>(v2.GetValue().GetLayout().GetCumulativeIncrement(0)));
            });

            if (result)
            {
                return *result;
            }

            result = InvokeForContext<LLVMContext>([&](LLVMContext& context) -> Scalar {
                if (context.GetModuleEmitter().GetCompilerOptions().useBlas)
                {
                    auto returnValue = fn.Decorated(FunctionDecorated::No)
                                           .Call(
                                               Scalar{ static_cast<int>(v1.Size()) },
                                               v1,
                                               Scalar{ static_cast<int>(v1.GetValue().GetLayout().GetCumulativeIncrement(0)) },
                                               v2,
                                               Scalar{ static_cast<int>(v2.GetValue().GetLayout().GetCumulativeIncrement(0)) });

                    return *returnValue;
                }
                else
                {
                    return defaultImpl(v1, v2);
                }
            });

            return *result;
        }
        else
        {
            return defaultImpl(v1, v2);
        }
    }

    void For(Vector v, std::function<void(Scalar)> fn)
    {
        auto layout = v.GetValue().GetLayout();

        if (layout.NumDimensions() != 1)
        {
            throw InputException(InputExceptionErrors::invalidArgument,
                                 "Layout being looped over must be one-dimensional");
        }

        GetContext().For(layout, [fn = std::move(fn)](std::vector<Scalar> coordinates) { fn(coordinates[0]); });
    }

    Vector operator+(Scalar s, Vector v)
    {
        return v + s;
    }

    Vector operator+(Vector v, Scalar s)
    {
        Vector copy = v.Copy();
        return copy += s;
    }

    Vector operator+(Vector v1, Vector v2)
    {
        Vector copy = v1.Copy();
        return copy += v2;
    }

    Vector operator-(Scalar s, Vector v)
    {
        Vector copy = v.Copy();
        For(copy, [&](Scalar index) {
            copy(index) = s - copy(index);
        });
        return copy;
    }

    Vector operator-(Vector v, Scalar s)
    {
        Vector copy = v.Copy();
        return copy -= s;
    }

    Vector operator-(Vector v1, Vector v2)
    {
        Vector copy = v1.Copy();
        return copy -= v2;
    }

    Vector operator*(Scalar s, Vector v)
    {
        return v * s;
    }

    Vector operator*(Vector v, Scalar s)
    {
        Vector copy = v.Copy();
        return copy *= s;
    }

    Vector operator/(Scalar s, Vector v)
    {
        Vector copy = v.Copy();
        For(copy, [&](Scalar index) {
            copy(index) = s / copy(index);
        });
        return copy;
    }

    Vector operator/(Vector v, Scalar s)
    {
        Vector copy = v.Copy();
        return copy /= s;
    }

} // namespace value
} // namespace ell
