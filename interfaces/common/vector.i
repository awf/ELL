////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     vector.i (interfaces)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// Include language specific SWIG definitions that must be declared before the
// C++ code to be wrapped
#ifdef SWIGPYTHON
    %include "vector_python_pre.i"
#elif SWIGJAVASCRIPT
    %include "vector_javascript_pre.i"
#endif

%{
#include <vector>
#include <stdint.h>
%}

#ifndef SWIGXML
%include "std_string.i"
%include "std_vector.i"
%include "macros.i"
%include "stdint.i"

%template(DoubleVector) std::vector<double>;
%template(DoubleVectorVector) std::vector<std::vector<double>>;
%template(FloatVector) std::vector<float>;
%template(FloatVectorVector) std::vector<std::vector<float>>;
%template(StringVector) std::vector<std::string>;
%template(IntVector) std::vector<int>;
// bugbug: doesn't compile with GCC or CLang.
// %template(Int64Vector) std::vector<int64_t>;
%template(Int8Vector) std::vector<int8_t>;

// Include language specific SWIG definitions that must be declared after the
// C++ code has been wrapped by SWIG
#ifdef SWIGPYTHON
    %include "vector_python_post.i"
#elif SWIGJAVASCRIPT
    %include "vector_javascript_post.i"
#endif

#endif // SWIGXML