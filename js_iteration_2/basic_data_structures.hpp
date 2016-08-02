/** File: basic_data_structures.hpp
 *  -------------------------------
 * This file contains data structures and functions that are repeatedly used,
 * so they should be all in a common place, with their documentation and usage
 * messages.
 */


#ifndef IMPLICIT_BASIC_DATASTRUCTURES_HPP
#define IMPLICIT_BASIC_DATASTRUCTURES_HPP


#include "boost/multi_array.hpp"
#include "boost/array.hpp"
#include <math.h>
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/lu.hpp"
#include "boost/numeric/ublas/io.hpp"
#include "svd.cpp"



using namespace boost::numeric::ublas;
using namespace std;

//#include "boost/assert.hpp"

//namespace implicit {


/*
================================================================
=                     Useful Data Structures                   =
================================================================
*/




typedef unsigned short int dim_t;

typedef float REAL;     // heavily used (can be changed to double from here at any time)

const REAL NaN = std::numeric_limits<REAL>::quiet_NaN();
//const REAL NaN = std::numeric_limits<REAL>::signaling_NaN();

inline bool isNaN(REAL x){return isnan(x);};

typedef boost::multi_array<REAL, 1>  array1d;

typedef boost::array<array1d::index, 1>  array_shape_t;

typedef array1d::index  index_t;

typedef short int bool_t;

const bool_t b_true = 1;
const bool_t b_false = 0;

//typedef boost::array<vectorized_vect::index, 2>  shape_t;


typedef array1d::index  vertex_t;

/** Implementation Note
 *  Data type: vectorized_scalar
 *  -----------------------------
 *  The type vectorized_scalar is a container for floats that has one dimension
 *  or else, a vector.
 *
 */

typedef boost::multi_array<REAL, 1>  vectorized_scalar;

/** Implementation Note
 * Data type: vectorized_vect
 * --------------------------
 *
 * The type vectorized_vect is a container for floats and has two dimensions.
 * It aims to be a container for 3d coordinates, so a variable of this type
 * will be a N x 3 array.
 */

typedef boost::multi_array<REAL, 2>  vectorized_vect;


#include ".//my_assert.hpp"


inline array_shape_t make_shape_1d(array1d::index size) {
    my_assert(size>=0, "");
    array_shape_t shape = { size, };
    return shape;
}


typedef boost::multi_array<REAL, 2>  array2d;


/*
================================================================
=                       Useful Functions                       =
================================================================
*/

/**
 * Function: make_empty_x
 * Usage: boost::multi_array<REAL, 2> x = make_empty_x(100)
 * --------------------------------------------------------
 * Creates an empty array with dimensions N x 3, whose elements are of floating
 * point numbers(REAL).
 *
 * Notes: make_empty_x allocates memory according to nsize.
 */

boost::multi_array<REAL, 2>  make_empty_x(const int nsize){
    auto sf = make_shape_1d(nsize);
    //vectorized_scalar  f = vectorized_scalar(sf);

    boost::array<int, 2> values_shape = {{ nsize, 3 }};
    boost::multi_array<REAL, 2> values (values_shape);
    return values;
}

/**
 * Function: invert_matrix
 * Usage:
 * ---------------------------------------
 * Desc:
 *
 * Notes:
 */

// Create a invert_matrix function

 /* Matrix inversion routine.
 Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */

bool invert_matrix(const REAL input_A[], REAL inverse_A[])
{
	typedef permutation_matrix<std::size_t> pmatrix;

  matrix<REAL> input(4,4);
  matrix<REAL> inverse(4,4);

  for(int i=0; i<3; i++){
    for(int j=0; j<4; j++){
      input(i,j)= input_A[i*4+j];
      inverse(i,j)= inverse_A[i*4+j];
    }
  }

  input(3,3) = 1.;
  input(3,0) = 0.;
  input(3,1) = 0.;
  input(3,2) = 0.;

  inverse(3,3) = 1.;
  inverse(3,0) = 0.;
  inverse(3,1) = 0.;
  inverse(3,2) = 0.;
	// create a working copy of the input
	matrix<REAL> A(input);

	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());

	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if (res != 0)
		return false;

	// create identity matrix of "inverse"
	inverse.assign(identity_matrix<REAL> (A.size1()));

	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);

  for(int i=0; i<3; i++){
    for(int j=0; j<4; j++){
       inverse_A[i*4+j] = inverse(i,j);
    }
  }

	return true;
}

/**
 * Function: matrix_vector_product
 * Usage:
 * ---------------------------------------
 * Desc:
 *
 * Notes:
 */


void matrix_vector_product(const REAL matou[], vectorized_vect& vectou){
    const REAL m0 = matou[0];
    const REAL m1 = matou[1];
    const REAL m2 = matou[2];
    const REAL m3 = matou[3];

    const REAL m4 = matou[4];
    const REAL m5 = matou[5];
    const REAL m6 = matou[6];
    const REAL m7 = matou[7];

    const REAL m8 = matou[8];
    const REAL m9 = matou[9];
    const REAL m10 = matou[10];
    const REAL m11 = matou[11];

    for (int i=0; i<vectou.shape()[0]; i++){
        REAL vectou_0 = vectou[i][0];
        REAL vectou_1 = vectou[i][1];
        vectou[i][0] = m0*vectou_0 + m1*vectou_1 +  m2*vectou[i][2] + m3*1.;
        vectou[i][1] = m4*vectou_0 + m5*vectou_1 +  m6*vectou[i][2] + m7*1.;
        vectou[i][2] = m8*vectou_0 + m9*vectou_1 + m10*vectou[i][2] + m11*1.;
    }

}


/**
 * Function: Matrix_Vector_Product_0
 * Usage:
 * ---------------------------------------
 * Desc:
 *
 * Notes:
 */

void Matrix_Vector_Product_0(const REAL matou[], vectorized_vect& vectou){
  for (int i=0; i<vectou.shape()[0]; i++){
    REAL vectou_0 = vectou[i][0];
    REAL vectou_1 = vectou[i][1];
    vectou[i][0] = matou[0]*vectou_0 + matou[1]*vectou_1 + matou[2]*vectou[i][2] + matou[3]*1.;
    vectou[i][1] = matou[4]*vectou_0 + matou[5]*vectou_1 + matou[6]*vectou[i][2] + matou[7]*1.;
    vectou[i][2] = matou[8]*vectou_0 + matou[9]*vectou_1 + matou[10]*vectou[i][2] + matou[11]*1.;
  }

}


/**
 * Function: Cross_Vector_Product
 * Usage:
 * ---------------------------------------
 * Desc:
 *
 * Notes:
 */

void Cross_Vector_Product(const REAL vec1[], const REAL vec2[], REAL vec3[]){

    // my_assert (vec1.shape()[0] == vec2.shape()[0], "Sizes don't match");
    // my_assert (vec1.shape()[0] == vec3.shape()[0], "Sizes don't match");
    const REAL vec1_0 = vec1[0];
    const REAL vec1_1 = vec1[1];
    const REAL vec1_2 = vec1[2];
    const REAL vec2_0 = vec2[0];
    const REAL vec2_1 = vec2[1];
    const REAL vec2_2 = vec2[2];

    vec3[0] = vec1_1 * vec2_2 - vec1_2 * vec2_1;
    vec3[1] = vec1_2 * vec2_0 - vec1_0 * vec2_2;
    vec3[2] = vec1_0 * vec2_1 - vec1_1 * vec2_0;

}


/**
 * Function: matrix_matrix_product
 * Usage:
 * ---------------------------------------
 * Desc:
 *
 * Notes:
 */

bool matrix_matrix_product(REAL m1[],const REAL m2[])
{
	typedef permutation_matrix<std::size_t> pmatrix;

  matrix<REAL> M1(4,4);
  matrix<REAL> M2(4,4);

  for(int i=0; i<3; i++){
    for(int j=0; j<4; j++){
      M1(i,j)= m1[i*4+j];
      M2(i,j)= m2[i*4+j];
    }
  }

  M1(3,3) = 1.;
  M1(3,0) = 0.;
  M1(3,1) = 0.;
  M1(3,2) = 0.;

  M2(3,3) = 1.;
  M2(3,0) = 0.;
  M2(3,1) = 0.;
  M2(3,2) = 0.;

  matrix<REAL> M3(4,4);

  M3 = prod(M1, M2);

  for(int i=0; i<3; i++){
    for(int j=0; j<4; j++){
       m1[i*4+j] = M3(i,j);
    }
  }

	return true;
}


/**
 * Function: SVD
 * Usage:
 * ---------------------------------------
 * Desc:
 *
 * Notes:
 */

void SVD(const verts_t& A, verts_t& u, verts_t& s, verts_t& v){
  
  ublas::matrix < float > QQL(3,3);
  ublas::matrix < float > QQW(3,3);
  ublas::matrix < float > QQR(3,3);
  ublas::matrix < float > in(3,3);

  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++){
      in(i,j)= A[i][j];
    }
  }

  svd(in, QQL, QQW, QQR);

  for(int i=0; i<3; i++){
    for(int j=0; j<3; j++){
      u[i][j]= QQL(i,j);
      s[i][j]= QQW(i,j);
      v[i][j]= QQR(i,j);
    }
  }

}

/**
 * Function: prepare_inner_vectors
 *
 * Usage:  vectorized_vect x = prepare_inner_vectors(this->inv_transf_matrix, x)
 *
 * Description:
 * 				This function is used in eval_gradient and eval_implicit of an object
 * 				and it simply creates a copy of the input x, and then applies the object inverse transform matrix.
 */


vectorized_vect prepare_inner_vectors(REAL* inv_transf_matrix, const vectorized_vect& x) {
    //my_assert(assert_implicit_function_io(x, *f_output), "");
    //my_assert(this->integrity_invariant(), ""); // fixme: has problems


    vectorized_vect::index nc = x.shape()[0];  // converts int -> uint
    vectorized_vect::index ndim = x.shape()[1];
    auto shape_ = boost::array<vectorized_vect::index, 2>{nc, ndim};
    vectorized_vect x_copy = vectorized_vect(shape_);
    std::copy(x.begin(), x.end(), x_copy.begin());
    //vectorized_vect x_copy = x;

    //vectorized_vect x_copy = vectorized_vect(x.begin(), x.end());

    matrix_vector_product(inv_transf_matrix, x_copy);

    return x_copy;
}

namespace mp5_implicit {

    struct bounding_box {
        REAL xmin, xmax, ymin, ymax, zmin, zmax;
    };

}

/*,
================================================================
=           Configuration Parameters                           =
================================================================
*/


/**
 * Configuration parameters
 * --------------------------
 * Contains global configuration options for tolerances, epsilon and other
 * parameters
 *
 * See: implicit_config.py
 */

const REAL ROOT_TOLERANCE = 0.001;  //
const REAL MIN_PRINTABLE_LENGTH = 0.01;  //


#endif // IMPLICIT_BASIC_DATASTRUCTURES_HPP
