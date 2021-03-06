#pragma once
#include <vector>
//#include "basic_data_structures.hpp"
//#include "basic_functions.hpp"
#include "transformed.hpp"

namespace mp5_implicit {
namespace implicit_functions {


class transformed_intersection : public transformed {

    std::vector<implicit_function*> children;

public:
    transformed_intersection (std::vector<implicit_function*> children, REAL matrix[12])
        : transformed(matrix), children(children)
    {
        my_assert(children.size() == 2, "for now only works on two objects.");

        /*
      this->transf_matrix = new REAL [12];
      this->inv_transf_matrix = new REAL [12];

      for (int i=0; i<12; i++){
          transf_matrix[i] = matrix12[i];
      }

      invert_matrix(this->transf_matrix, this->inv_transf_matrix);
      my_assert(this->integrity_invariant(), "");
      */
    }

    virtual void eval_implicit(const vectorized_vect& x, vectorized_scalar* f_output) const {
        
        my_assert(this->integrity_invariant(), "");
        const vectorized_scalar::index nsize = x.shape()[0];

        const vectorized_vect local_x = prepare_inner_vectors(this->inv_transf_matrix, x);
        auto sf = make_shape_1d(nsize);
        vectorized_scalar f1 = vectorized_scalar(sf);  // first function
        children[0]->eval_implicit(local_x, &f1);

        vectorized_scalar f2 = vectorized_scalar(sf);  // second function
        children[1]->eval_implicit(local_x, &f2);

        vectorized_scalar::index output_ctr = 0;
        auto e = local_x.end();
        for (auto i = local_x.begin(); i < e; i++, output_ctr++){
            (*f_output)[output_ctr] = (f1[output_ctr] > f2[output_ctr]) ? (f2[output_ctr]): f1[output_ctr];
        }
    };
    virtual void eval_gradient(const vectorized_vect& x, vectorized_vect* output) const {
      const vectorized_scalar::index nsize = x.shape()[0];

      auto sf = make_shape_1d(nsize);

      vectorized_scalar f1 = vectorized_scalar(sf);  // first function
      vectorized_scalar f2 = vectorized_scalar(sf);  // second function

      auto shape = boost::array<vectorized_vect::index, 2>{nsize, 3};
      vectorized_vect grad1 = vectorized_vect(shape);
      vectorized_vect grad2 = vectorized_vect(shape);

      const vectorized_vect local_x = prepare_inner_vectors(this->inv_transf_matrix, x);

      children[0]->eval_implicit(local_x, &f1);
      children[1]->eval_implicit(local_x, &f2);

      children[0]->eval_gradient(local_x, &grad1);
      children[1]->eval_gradient(local_x, &grad2);

      vectorized_scalar::index output_ctr = 0;

      auto e = local_x.end();

      for (auto i = local_x.begin(); i < e; i++, output_ctr++){
          (*output)[output_ctr] = (f1[output_ctr] > f2[output_ctr]) ? (grad2[output_ctr]): grad1[output_ctr];

          REAL gx = (*output)[output_ctr][0];
          REAL gy = (*output)[output_ctr][1];
          REAL gz = (*output)[output_ctr][2];
          (*output)[output_ctr][0] = this->inv_transf_matrix[0]*gx + this->inv_transf_matrix[4]*gy + this->inv_transf_matrix[8]*gz;
          (*output)[output_ctr][1] = this->inv_transf_matrix[1]*gx + this->inv_transf_matrix[5]*gy + this->inv_transf_matrix[9]*gz;
          (*output)[output_ctr][2] = this->inv_transf_matrix[2]*gx + this->inv_transf_matrix[6]*gy + this->inv_transf_matrix[10]*gz;
      }
    };
    bool integrity_invariant() const {

        //transformed(this).integrity_invariant();
        //???????????

        //TODO(sohail): Check the minimum size based on the SVD of the matrix.
        return true;
    }

    virtual mp5_implicit::bounding_box  get_boundingbox() const {
        for ( auto it = children.begin(); it < children.end(); it++ ){

        }
        REAL max_size = norm_squared(transf_matrix[0], transf_matrix[4], transf_matrix[8]);
        return mp5_implicit::bounding_box{-max_size, max_size, -max_size, max_size, -max_size, max_size};
    }

};

}  // namespace implicit_functions
}  // namespace mp5_implicit
