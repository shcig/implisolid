/*
    example/example-numpy-vectorize.cpp -- auto-vectorize functions over NumPy array
    arguments

    Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include "example.h"
#include <pybind11/numpy.h>

double my_func(int x, float y, double z) {
    lout << "my_func(x:int=" << x << ", y:float=" << y << ", z:float=" << z << ")" << std::endl;
    return (float) x*y*z;
}

std::complex<double> my_func3(std::complex<double> c) {
    return c * std::complex<double>(2.f);
}

void init_ex_numpy_vectorize(py::module &m) {
    // Vectorize all arguments of a function (though non-vector arguments are also allowed)
    m.def("vectorized_func", py::vectorize(my_func));

    // Vectorize a lambda function with a capture object (e.g. to exclude some arguments from the vectorization)
    m.def("vectorized_func2",
        [](py::array_t<int> x, py::array_t<float> y, float z) {
            return py::vectorize([z](int x, float y) { return my_func(x, y, z); })(x, y);
        }
    );

    // Vectorize a complex-valued function
    m.def("vectorized_func3", py::vectorize(my_func3));

    /// Numpy function which only accepts specific data types
    m.def("selective_func", [](py::array_t<int, py::array::c_style>) { lout << "Int branch taken. "<< std::endl; });
    m.def("selective_func", [](py::array_t<float, py::array::c_style>) { lout << "Float branch taken. "<< std::endl; });
    m.def("selective_func", [](py::array_t<std::complex<float>, py::array::c_style>) { lout << "Complex float branch taken. "<< std::endl; });
}