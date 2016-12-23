#pragma once

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

//#include "unit_sphere.hpp"
#include "implicit_function/primitives.hpp"
//#include "implicit_function/crisp_subtract.hpp"
//#include "implicit_function/linearly_transformed.hpp"

//#include""  will be moved into sweep.hpp, extrude.hpp, screw.hpp, etc
#include "implicit_function/2d/primitives_2d.hpp"

#include "object_collector.hpp"
//using namespace mp5_implicit;
using mp5_implicit::implicit_function;

implicit_function*  object_factory_simple(REAL f_argument, std::string name){
    implicit_function* object;
    /*
    if (name == "double_mushroom"){
        object = new mp5_implicit::double_mushroom(0.8, 1/(f_argument+3), 1/(f_argument+3), f_argument+3);
        register_new_object(object);
    }
    else if (name == "egg"){
        object = new mp5_implicit::egg(f_argument,f_argument,f_argument);
        register_new_object(object);
    }
    else if (name == "sphere"){
        object = new mp5_implicit::unit_sphere((sin(0.033*10 * f_argument * 3.1415*2.)*0.33+0.3)*10);
        register_new_object(object);
    }
    else if (name == "cube"){
        object = new mp5_implicit::cube(f_argument+0.2, f_argument+0.2, f_argument+0.2);
        register_new_object(object);
    }
    else if (name == "super_bowl"){// not working
        object = new mp5_implicit::super_bowl(1.5/(f_argument+3.0));
        register_new_object(object);
    }
    else if (name == "scone"){
        object = new mp5_implicit::scone(f_argument +2.5,f_argument +2.5,f_argument +2.5,-0.1);
        register_new_object(object);
    }
    //else if (name == "scylinder"){
    //    object = new mp5_implicit::scylinder(f_argument, 1.6); //0.7
    //    register_new_object(object);
    //}
    //else
    */
    if(name == "meta_balls"){
        REAL r = (sin(0.033*10 * f_argument * 3.1415*2.)*0.33+0.3)*1;
        std::clog << " META BALLS r : " << r << std::endl;
        object = new mp5_implicit::unit_sphere(r);
        register_new_object(object);
    }
    /*else if(name == "sub_spheres"){
        mp5_implicit::unit_sphere * s1 = new mp5_implicit::unit_sphere(2, 1, 1, 1);
        mp5_implicit::unit_sphere * s2 = new mp5_implicit::unit_sphere(1.3);
        object = new mp5_implicit::CrispSubtract(*s1, *s2);
        register_new_object(s1);
        register_new_object(s2);
        register_new_object(object);
    }
    */
    else {
        std::clog << "Error! You must enter a valid name " <<  name << "! So I made a sphere!" << std::endl;
        //object = new mp5_implicit::unit_sphere(sin(0.033*10 * f_argument * 3.1415*2.)*0.33+0.3);
        object = new mp5_implicit::unit_sphere(1000.);
        register_new_object(object);
    }
    return object;
}


namespace pt = boost::property_tree ;

void getMatrix12(REAL * matrix12, const pt::ptree& shapeparams_dict){
        int i = 0;
        for (const pt::ptree::value_type &element : shapeparams_dict.get_child("matrix")){

            REAL x = element.second.get_value<REAL>();
            //std::clog << "matrix value : " << x << std::endl;
            matrix12[i] = x;
            i++;
        }
}

void getCorners(std::vector<boost::array<REAL,3>>& corners, const pt::ptree& shapeparams_dict){

    int i = 0;
    for (const pt::ptree::value_type &element : shapeparams_dict.get_child("corners")) {
        int j = 0;
        for (const pt::ptree::value_type &cell : element.second)
        {
            corners[i][j] = cell.second.get_value<REAL>();
            j++;
        }
        i++;

    }
}

void copy_eye(REAL matrix12[12]){
    REAL eye[12] = {1,0,0,0,  0,1,0,0,  0,0,1,0 };
    for(int j=0;j<12;j++)
        matrix12[j] = eye[j];
}


implicit_function*  object_factory(pt::ptree shapeparams_dict, bool& use_metaball, bool ignore_root_matrix) {
    // std::clog << "ignore_root_matrix: " << ignore_root_matrix << std::endl;
    std::string name = shapeparams_dict.get<std::string>("type");
    //REAL xmax = shapeparams_dict.get<REAL>("matrix",NaN);
    //std::clog << "############Name : " << name << std::endl;
    //REAL zmax = shapeparams_dict.get<REAL>("box.zmax",NaN);
    // int resolution = shapeparams_dict.get<int>("resolution",-1);

    // if(isNaN(xmin) || isNaN(xmax) || isNaN(ymin) || isNaN(ymax) || isNaN(zmin) || isNaN(zmax) || resolution <= 2 ){
    //     std::clog << "Error: missing or incorrect values in mc_parameters_json"<< std::endl;
    //     xmin = -1;
    //     xmax = 1;
    //     ymin = -1;
    //     ymax = 1;
    //     zmin = -1;
    //     zmax = 1;
    //     resolution = 28;
    // }
    std::cout << "-------------------------------------" <<std::endl;
    std::cout << name <<std::endl;



    if(name=="meta_balls"){
        use_metaball = true;
    }
    else{
        use_metaball = false;
    }

    implicit_function* object;

    if (name == "implicit_double_mushroom"){
        // std::clog << "implicit_double_mushroom case " << std::endl;
        // object = new mp5_implicit::double_mushroom(0.9, 0.4 ,0.4 , 1/0.2 );
        implicit_function* dm = new mp5_implicit::double_mushroom(0.9, 0.4/2, 0.4/2, 1/0.2 );

        REAL matrix12[12];
        getMatrix12(matrix12,shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        object = new mp5_implicit::linearly_transformed(dm, matrix12);
        register_new_object(object);
        //object = dm;
    }
    else
    /*if (name == "simple_sphere"){
        //std::clog << "******************* simple_sphere case " << std::endl;
        REAL radius = shapeparams_dict.get<REAL>("radius");
        object = new mp5_implicit::unit_sphere(radius);
        std::clog << "radius " << radius << std::endl;
        register_new_object(object);
    }
    else*/
    if (name == "icube" || name == "cube" ){
        REAL matrix12[12];
        getMatrix12(matrix12,shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }
        object = new mp5_implicit::cube(matrix12);
       // object = new mp5_implicit::cube(f_argument+0.2, f_argument+0.2, f_argument+0.2);
        register_new_object(object);
    }
    else
    if (name == "icylinder" || name == "cylinder" ){
        REAL matrix12[12];
        getMatrix12(matrix12,shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        object = new mp5_implicit::scylinder(matrix12);
        register_new_object(object);
       // object = new mp5_implicit::cube(f_argument+0.2, f_argument+0.2, f_argument+0.2);
        //register_new_object(object);
    }
    else
    if (name == "iellipsoid" || name == "ellipsoid" ){
        REAL matrix12[12];
        getMatrix12(matrix12, shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        object = new mp5_implicit::egg(matrix12);
        register_new_object(object);
    }else
    if (name == "icone" || name == "cone" ){
        REAL matrix12[12];
        getMatrix12(matrix12,shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        object = new mp5_implicit::scone(matrix12);
        register_new_object(object);
    }else if(name == "iheart" ){
        REAL matrix12[12];
        getMatrix12(matrix12,shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        object = new mp5_implicit::heart(matrix12);
        register_new_object(object);

    }else if(name == "itorus" ){
        REAL matrix12[12];
        getMatrix12(matrix12, shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        object = new mp5_implicit::torus(matrix12);
        register_new_object(object);

    }
    else
    if (name == "tetrahedron"){

        REAL matrix12[12];
        getMatrix12(matrix12, shapeparams_dict);

        std::vector<boost::array<REAL,3>> corners(4);
        getCorners(corners, shapeparams_dict);

        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }
        object = new mp5_implicit::tetrahedron(corners, matrix12);
        register_new_object(object);
    }
    else if (name == "screw") {

        Matrix<REAL, 4, 4> transformation_matrix;
        REAL pitch_len = 0.0;
        std::string profile;
        std::string end_type;
        REAL delta_ratio = 0.0;
        Matrix<REAL, 3, 1> v;

        mp5_implicit::screw::getScrewParameters(
                           transformation_matrix, pitch_len, profile, 
                           end_type, delta_ratio, v, 
                           shapeparams_dict);

        object = new mp5_implicit::screw(transformation_matrix,
                                         pitch_len,
                                         profile,
                                         end_type,
                                         delta_ratio,
                                         v);

        register_new_object(object);
    }
    else if (name == "Union") {
        //todo: Use SimpleUnion if (matrix12 == eye(4))
        REAL matrix12[12];
        getMatrix12(matrix12, shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        implicit_function * a = NULL;

        implicit_function * o_matrix;
        implicit_function * o_plain;

        for (pt::ptree::value_type &element : shapeparams_dict.get_child("children")) {
            if (a == NULL){
                a = object_factory(element.second, use_metaball, false);
            } else {
                implicit_function * b = object_factory(element.second, use_metaball, false);

                //The following always prints an empty line:
                //std::clog << "element.second.get_value<string>(\"type\")" << element.second.get_value<string>("type") << std::endl ;

                //a = new mp5_implicit::CrispUnion(*a, *b);
                // register_new_object(a);
                //std::vector<const implicit_function*> versus std::vector<implicit_function*>
                std::vector<implicit_function*> ab = std::vector<implicit_function*>();
                ab.push_back(a);
                ab.push_back(b);
                o_matrix = new mp5_implicit::transformed_union(ab, matrix12);
                register_new_object(o_matrix);
                REAL eye_matrix12[12] = {1,0,0,0,  0,1,0,0,  0,0,1,0};
                o_plain =  new mp5_implicit::transformed_union(ab, eye_matrix12);
                a = o_plain;
                register_new_object(o_plain);
            }

            //std::clog << "##### " << element.second.get_child("type") << std::endl;
            //i++;
        }
        a = o_matrix;
        //std::clog  << "#####" << shapeparams_dict.get<string>("children") << std::endl;
        //implicit_function * a = object_factory();
        //implicit_function * b;

        object = a;
        //object = new mp5_implicit::egg(matrix12);
        // register_new_object
    }else if (name == "Intersection") {
        //todo: Use SimpleUnion if (matrix12 == eye(4))
        REAL matrix12[12];
        getMatrix12(matrix12,shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        implicit_function * a = NULL;
        implicit_function * b = NULL;
        int count = 0;
        for (pt::ptree::value_type &element : shapeparams_dict.get_child("children")) {
            if (a==NULL){
                a = object_factory(element.second, use_metaball, false);
            }else{
                if(count > 1){
                    std::clog << "An CrispIntersection should have only 2 child" << std::endl;
                    break;
                }
                b = object_factory(element.second, use_metaball, false);

            }
            count++;
            if (count > 2) {
                std::clog << "Error: Intersection cannot be applied to more than two objects." << std::endl;
            }
        }

        // object = new mp5_implicit::CrispIntersection(*a, *b);
        //object = new mp5_implicit::transformed_intersection(*a, *b, matrix12);
        std::vector<implicit_function *> children;
        children.push_back(a);
        children.push_back(b);
        object = new mp5_implicit::transformed_intersection(children, matrix12);
        register_new_object(object);

    }else if (name == "Difference") {
        //todo: Use SimpleUnion if (matrix12 == eye(4))
        REAL matrix12[12];
        getMatrix12(matrix12,shapeparams_dict);
        if(ignore_root_matrix) {
            copy_eye(matrix12);
        }

        implicit_function * a = NULL;
        implicit_function * b = NULL;
        int count = 0;
        for (pt::ptree::value_type &element : shapeparams_dict.get_child("children")) {
            if (a==NULL){
                a = object_factory(element.second, use_metaball, false);
            }else{
                if(count > 1){
                    std::clog << "An CrispSubstraction should have only 2 child" << std::endl;
                    break;
                }
                b = object_factory(element.second, use_metaball, false);

            }
            count++;
            if (count > 2) {
                std::clog << "Error: Intersection cannot be applied to more than two objects." << std::endl;
            }

        }

        // object = new mp5_implicit::CrispSubtract(*a, *b);
        // object = new mp5_implicit::transformed_subtract(*a, *b, matrix12);
        std::vector<implicit_function *> children;
        children.push_back(a);
        children.push_back(b);
        object = new mp5_implicit::transformed_subtract(children, matrix12);
        register_new_object(object);
    }
    else{
        // if(name=="meta_balls")
        REAL f_argument = shapeparams_dict.get<REAL>("time", NaN);

        std::clog << "otherwise " << "you asked for " << name << std::endl;
        object = object_factory_simple(f_argument, name);

    }

    return object;

}




implicit_function*  object_factory(string shape_parameters_json, bool& use_metaball, bool ignore_root_matrix) {


    /*
    mp5_implicit :: unit_sphere   object(sin(0.033*10 * time * 3.1415*2.)*0.33+0.3);
    // //_state.mc -> prepare_grid(1.0);
    // //object.eval_implicit(grid, implicit_values);
    _state.mc -> eval_shape(object, 1.0);
    */


/*
    std::string name = std::string(obj_name);
    std::clog << "Name : " << name << std::endl;

    REAL f_argument = time;

    implicit_function* object = object_factory_simple(f_argument, name);
*/
    //std::clog << "############################" << shape_parameters_json << std::endl;
    std::stringstream shape_json_stream;
    shape_json_stream << shape_parameters_json ;


    pt::ptree shapeparams_dict;

    pt::read_json(shape_json_stream, shapeparams_dict);

    return object_factory(shapeparams_dict, use_metaball, ignore_root_matrix);

}
