#pragma once


#include "../js_iteration_2/basic_data_structures.hpp"
#include "../js_iteration_2/basic_functions.hpp"
#include "../js_iteration_2/implicit_function/implicit_function.hpp"

namespace marching_cubes {
    


#define MARCHINGCUBES_ENABLE_NORMALS true

/** Pipeline:
    (MC_table_loopup) --> *list_buffer --> *Queue --> result_*
*/
template <bool enableUvs=false, bool enableColors=false>
class MarchingCubes {

    // types
    typedef index_t index3_t;  // Range of the element type has to be large enough, larger than (size^3)*3.
    typedef boost::multi_array<index3_t, 1>   array1d_e3;
    //struct callback_t { void call(void*) const { } callback_t(){} };

    //  member variables

    //bool enableUvs, enableColors;
    //dim_t resolution_;
    index_t resolution_x, resolution_y, resolution_z;
    //index_t size1x, size2xy, size3xyz; //todo: non-equal `grid` sizes
    struct gridbox_t {
        constexpr static index_t xstride = 1;
        index_t  ystride;  // size1x
        index_t  zstride;  // size2xy
        index_t  full_stride = -1;  // size3xyz
    };

    gridbox_t gridbox;

    // size1x=ystride, size2xy=zstride, size3xyz=?
    //  ((x)*y)*z  <-->  x, xy, xyz  <-->  ystride,zstride,fullstride  <-->
    // global cube: for indices (cube coords) in a larger virtual grid,
    // local cube: for the current cube in the boundingbox,
    //index_t  ystride, zstride; // local: for the 'field' and normal_cache arrays
    gridbox_t  localgrid;
    //global: for indexing vertices and their edges, when not all the field is available
    gridbox_t globalbox; // globalgrid
    /*
        globalbox: used only for indexing the edges of triangles
        localgrid: used for field and normal_cache (former zd,yd, ystride,zstride)
        gridbox: ???used for seal only
    */

 
    //buffer_stride, global_stride, grid_stride, subgrids, master_grid, buffer_grid, this_grid,    , globalgrid, midgrid
    // ystride(=size1x), zstride, xyzstride
    /*
    struct midgrid_t {
        index_t resolution_x, resolution_y, resolution_z;
        constexpr static index_t xstride = 1;
        index_t  ystride, zstride, full_stride;  // size3xyz
    } midgrid */
    gridbox_t midgrid;

    //REAL halfsize;
    REAL widthx, widthy, widthz;  // in millimeters
    mp5_implicit::bounding_box box;

    array1d field;      // local_ ystride and zstride
    array1d normal_cache;  // size: 

    // parameters
    static const dim_t queueSize = 4096;  // Name history: maxCount
    // TODO(@sohale): find the fastest size for queueSize (AQ)

    //Queue, Buffer, Cache: sizes are: 4096, 16, 28**3, respectively.

    static constexpr bool ENABLE_NORMALS = false;
    static const int skip_count_l = 2; // -2
    static const int skip_count_h = 3;

public:
    typedef std::map<index3_t, int>  e3map_t;

 protected:
    //Buffers:
    index_t  temp_buffer_size = 12;  // why so small?
    // temp buffers used in polygonize
    array1d vlist_buffer;
    array1d nlist_buffer;  // size: 12 x 3
    array1d_e3 e3list_buffer;


    //Queues:
    int queue_counter = 0;

    bool hasPositions = false;
    bool hasNormals = false;
    bool hasColors = false;
    bool hasUvs = false;

    array1d positionQueue;  // size: MaxCount x 3
    array1d normalQueue;
    array1d_e3 e3Queue;

    // array1d &&colorQueue; // = 0;
    // array1d &&uvQueue; // = 0;
    array1d *colorQueue = 0;
    array1d *uvQueue = 0;

    void kill();

    // MC's lookup tables
    static const int mc_edge_lookup_table[256];
    static const int mc_triangles_table[256*16];

 protected:
    inline REAL lerp(REAL a, REAL b, REAL t ) {
        return a + ( b - a ) * t;
    }

 protected:
    void init( mp5_implicit::bounding_box box);

inline void VIntX(index_t q, array1d &pout, array1d &nout,    int offset, REAL isol, REAL x, REAL y, REAL z, REAL valp1, REAL valp2,  index_t ijk, array1d_e3& e3out);
inline void VIntY(index_t q, array1d& pout, array1d& nout,    int offset, REAL isol, REAL x, REAL y, REAL z, REAL valp1, REAL valp2,  index_t ijk, array1d_e3& e3out);
inline void VIntZ(index_t q, array1d& pout, array1d& nout,    int offset, REAL isol, REAL x, REAL y, REAL z, REAL valp1, REAL valp2,  index_t ijk, array1d_e3& e3out);

void compNorm( index_t q );
void posnormtriv( array1d& pos__vlist, array1d& norm__nlist, array1d_e3& e3__e3list, int o1, int o2, int o3 /*, const callback_t& renderCallback*/ );

void begin_queue();
void finish_queue( /*const callback_t& renderCallback*/ );


public:
    MarchingCubes( dim_t apparent_resolution, mp5_implicit::bounding_box box/*, bool enableUvs*/);
    ~MarchingCubes(); //why does this have to be public: ?

    //void flush_geometry_queue(std::ostream&);
    void flush_geometry_queue(std::ostream& cout, int& normals_start, std::vector<REAL> &normals,  std::vector<REAL> &verts3, std::vector<vertexindex_type> &faces3, e3map_t &e3map, int& next_unique_vect_counter);
    void reset_result();

    inline int polygonize_single_cube( REAL fx, REAL fy, REAL fz, index_t q /*, REAL isol*/ /*, const callback_t& callback*/ );

//field:
    void reset_field(); //???? Nobody calls this.
    void seal_exterior(const REAL exterior_value = -100.);
    vectorized_vect  prepare_grid();
    void eval_shape(const mp5_implicit::implicit_function& object, const boost::multi_array<REAL, 2>& mcgrid_vectorized );


//geometry/threejs interface side.
    void render_geometry(/*const callback_t& renderCallback */);
    void sow();

// output. filled using sow()
    int resultqueue_faces_start = 0;
    std::vector<REAL> result_normals;

    std::vector<REAL> result_verts;
    std::vector<vertexindex_type> result_faces;
    e3map_t result_e3map;
    int next_unique_vect_counter = 0;

 public:
    // high level:
    void produce_mesh(const mp5_implicit::implicit_function& object);
};

//static dim_t MarchingCubes::queueSize = ...;

int EXCESS = 0;
template <bool enableUvs, bool enableColors>
MarchingCubes<enableUvs,enableColors>::MarchingCubes( dim_t apparent_resolution, mp5_implicit::bounding_box box/*, bool enableUvs*/)
    :   //constructor's initialisation list: pre-constructor code
        //All memory allocation code is here. Because the size of arrays is determined in run-time.
        resolution_x(apparent_resolution + MarchingCubes::skip_count_l + MarchingCubes::skip_count_h ),
        resolution_y(apparent_resolution + MarchingCubes::skip_count_l + MarchingCubes::skip_count_h ),
        resolution_z(apparent_resolution + MarchingCubes::skip_count_l + MarchingCubes::skip_count_h ),
        field(array1d( array_shape_t ({{ resolution_x * resolution_y * resolution_z }}) )),
        normal_cache(array1d( array_shape_t({ resolution_x*resolution_y*resolution_z*3 *(MarchingCubes::ENABLE_NORMALS?1:0) }) )),

        vlist_buffer(array1d( array_shape_t( {temp_buffer_size * 3} ) )),
        nlist_buffer(array1d( array_shape_t( {temp_buffer_size * 3 * (MarchingCubes::ENABLE_NORMALS?1:0) } ) )),
        e3list_buffer(array1d_e3(  make_shape_1d(temp_buffer_size)   )),

        positionQueue(array1d(make_shape_1d(MarchingCubes::queueSize * 3 + EXCESS))),
        normalQueue(array1d(make_shape_1d(MarchingCubes::queueSize * 3 * (MarchingCubes::ENABLE_NORMALS?1:0) + EXCESS))),
        e3Queue(array1d_e3(make_shape_1d(MarchingCubes::queueSize )))

{
    //THREE.ImmediateRenderObject.call( this, material );

    #if MARCHINGCUBES_ENABLE_NORMALS
    //this->enableUvs = enableUvs;
    //this->enableColors = enableColors;
    #endif

    //if(VERBOSE)
    //    std::clog << resolution_x << " init"<< std::endl;

    this->init( box );

/*
    //preallocate
    int expected_vertices = 10;
    int expected_faces = 10;
    this->result_normals.reserve(expected_faces*3 *(MarchingCubes::ENABLE_NORMALS?1:0));
    this->result_verts.reserve(expected_vertices*3);
    this->result_faces.reserve(expected_faces*3);
    //what about normals?
    //Unfortunately, you cannot reserve elements for a C++ STL map.
    //this->result_e3map.reserve(expected_faces*3 ); //should be less than one third of expercted_verts
*/
}



template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::init( mp5_implicit::bounding_box box) {
        // May throw  std::bad_alloc. See #include <new>
        // init() is only called by the constructor

        // parameters

        // size of field, 32 is pushing it in Javascript :)

        //dim_t resolution_ = apparent_resolution + MarchingCubes::skip_count_l + MarchingCubes::skip_count_h;

        REAL widthx = box.xmax - box.xmin;
        REAL widthy = box.ymax - box.ymin;
        REAL widthz = box.zmax - box.zmin;

        // to make sure the skipped cubes are outside the bounding box.
        auto exi = this->resolution_x - MarchingCubes::skip_count_l - MarchingCubes::skip_count_h;
        auto eyi = this->resolution_y - MarchingCubes::skip_count_l - MarchingCubes::skip_count_h;
        auto ezi = this->resolution_z - MarchingCubes::skip_count_l - MarchingCubes::skip_count_h;

        // width of each mini-cube (each one of the marching cubes)
        this->widthx = widthx / (REAL)( exi );  // (2.0 / (REAL)resolution_)*size
        this->widthy = widthy / (REAL)( eyi );
        this->widthz = widthz / (REAL)( ezi );
        //REAL halfsize = width / 2.0 / delta; // ((REAL)this->resolution_) / 2.0;

        this->box = box;

        // deltas
        //this->delta = delta;  // 2.0 / (REAL)this->resolution_;
        this->gridbox.ystride = this->resolution_x;
        this->gridbox.zstride = this->resolution_x * this->resolution_y;
        this->gridbox.full_stride = this->gridbox.zstride * this->resolution_z;

        this->localgrid.ystride = this->gridbox.ystride;
        this->localgrid.zstride = this->gridbox.zstride;

        this->globalbox.ystride = this->gridbox.ystride;
        this->globalbox.zstride = this->gridbox.zstride;
        this->globalbox.full_stride = this->gridbox.full_stride;

        //gridbox
        //localgrid = gridbox
        //globalbox = gridbox

        array_shape_t fsize = {(int)this->gridbox.full_stride};
        this->field = array1d(fsize);
        // need a guarantee:

        // todo: get available heap.
        // todo: handle memory exception.
        my_assert(this->gridbox.full_stride < 10000000, "size of the field is too large.");
        assert(this->gridbox.full_stride > 0);


/**
 *  COMMENTED OUT
 *
        // auto field_shape = make_shape_1d((int)this->gridbox.full_stride);
        // //array_shape_t  field_shape = {{ (int)this->gridbox.full_stride, }};
        //
        //
        // std::clog << "trouble begins" << std::endl;
        // std::clog << (int)this->gridbox.full_stride << std::endl;
        //
        // //this->field = array1d( field_shape );
        // this->field = array1d( field_shape );
        // //this->field = array1d( field_shape );
*/

        #if MARCHINGCUBES_ENABLE_NORMALS
        if(MarchingCubes::ENABLE_NORMALS){
            // this->normal_cache = new Float32Array( this->gridbox.full_stride * 3 );
            array_shape_t normals_shape = make_shape_1d( (int)this->gridbox.full_stride * 3 );
            // array_shape_t  normals_shape = {{ (int)this->gridbox.full_stride * 3, }};
            this->normal_cache = array1d( normals_shape );

            // std::fill_n(this->normal_cache.begin(), this->normal_cache.size(), 0.0 );  // from #include <algorithm>
            // std::fill from #include <algorithm>
            std::fill(this->normal_cache.begin(), this->normal_cache.end(), 0.0 );
        }
        #endif
        //todo: fill up other arrays with zero.

        // temp buffers used in polygonize_single_cube

        // this->vlist_buffer = new Float32Array( 12 * 3 );
        // this->nlist_buffer = new Float32Array( 12 * 3 );
        // auto twelve3 = make_shape_1d( temp_buffer_size * 3 );

        // array_shape_t twelve3 = {{ 12 * 3, }};

        if(false){
            this->vlist_buffer = array1d( make_shape_1d( temp_buffer_size * 3 ) );
            #ifdef MARCHINGCUBES_ENABLE_NORMALS
            if(MarchingCubes::ENABLE_NORMALS)
                this->nlist_buffer = array1d( make_shape_1d( temp_buffer_size * 3 ) );
            #endif
        }

        // this::queueSize = 4096; // TODO: find the fastest size for this buffer

        this->queue_counter = 0;

        this->hasPositions = false;
        this->hasNormals = false;
        this->hasColors = false;
        this->hasUvs = false;

        // this->positionQueue = new Float32Array( this->queueSize * 3 );
        // this->normalQueue   = new Float32Array( this->queueSize * 3 );


        auto shape_maxCount_x_3 = make_shape_1d(MarchingCubes::queueSize * 3);
        // array_shape_t shape_maxCount_x_3 = {{ this->queueSize * 3, }};
        this->positionQueue = array1d(shape_maxCount_x_3);
        if(MarchingCubes::ENABLE_NORMALS){
            this->normalQueue   = array1d(shape_maxCount_x_3);
        }
        auto shape_maxCount_x_1 = make_shape_1d(MarchingCubes::queueSize * 1);
        this->e3Queue = array1d_e3(shape_maxCount_x_1);


        auto shape_maxCount_x_2 = make_shape_1d(MarchingCubes::queueSize * 2);
        // array_shape_t  shape_maxCount_x_2 = {{ this->queueSize * 2, }};


        // can throw  std::bad_alloc

        if ( enableUvs ) {
            // this->uvQueue = new Float32Array( this->queueSize * 2 );
            this->uvQueue = 0; //for deconstructor, to see if this exited by an exception.
            this->uvQueue = new array1d(shape_maxCount_x_2);
            // assert(this->uvQueue != null);
        }
        // else
        //    this->uvQueue = NULL;

        if ( enableColors ) {
            this->colorQueue = 0;
            this->colorQueue = new array1d(shape_maxCount_x_3);
            // new Float32Array( this->queueSize * 3 );
        }
        // else
        //    this->colorQueue = NULL;
}

template <bool enableUvs, bool enableColors>
MarchingCubes<enableUvs,enableColors>::~MarchingCubes() //deconstructor
{
    //if(VERBOSE)
    //    std::clog << "Destructor: ~MarchingCubes" << std::endl;

    if ( enableUvs )
    {
        if(this->uvQueue){
            delete this->uvQueue;
            this->uvQueue = 0;
            //std::clog << "delete this->uvQueue" << std::endl;
        }
    }
    if ( enableColors ) 
    {
        if(this->colorQueue) // if is not necessary for colorQueue,but keep this if.
        {
            delete this->colorQueue;
            this->colorQueue = 0;
            //std::clog << "delete this->colorQueue" << std::endl;
        }
    }
}

template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::kill()
//opposite of init()
{
    ;
}

//index_t ijk, short_t dir
template <bool enableUvs, bool enableColors>
inline void MarchingCubes<enableUvs,enableColors>:: VIntX(
    const index_t q,
    array1d &pout, array1d &nout,
    const int offset,
    const REAL isol,
    const REAL x, REAL y, REAL z,
    const REAL valp1,
    const REAL valp2,
    index_t ijk, array1d_e3& e3out )
{
    //std::clog << "VIntXX" << std::endl;

    // pout is vlist_buffer
    // nout is nlist_buffer

    REAL mu = ( isol - valp1 ) / ( valp2 - valp1 );
    const array1d& normal_cache = this->normal_cache;

    pout[ offset ]     = x + mu * this->widthx;
    pout[ offset + 1 ] = y;
    pout[ offset + 2 ] = z;

    //e1 = ijk;
    //eout[ eoffset ] = e1;
    //eout[ eoffset + 1 ] = e2;

    if(MarchingCubes::ENABLE_NORMALS){
        //todo: check the type of q
        nout[ offset ]     = lerp( normal_cache[ q ],     normal_cache[ q + 3 ], mu );
        nout[ offset + 1 ] = lerp( normal_cache[ q + 1 ], normal_cache[ q + 4 ], mu );
        nout[ offset + 2 ] = lerp( normal_cache[ q + 2 ], normal_cache[ q + 5 ], mu );
    }

    //std::clog << "here2-a" << std::endl;

    //offsetdiv3
    index3_t e3x = ijk*3;
    //std::clog << "here2-b" << std::endl;

    //very short
    //int offset333 = offset/3;
    //e3out[offset333] = e3x;
    e3out[offset/3] = e3x;
    //std::clog << "here2-c" << std::endl;

}

template <bool enableUvs, bool enableColors>
inline void MarchingCubes<enableUvs,enableColors>:: VIntY (index_t q, array1d& pout, array1d& nout, int offset, REAL isol, REAL x, REAL y, REAL z, REAL valp1, REAL valp2,
    index_t ijk, array1d_e3& e3out )
{
    REAL mu = ( isol - valp1 ) / ( valp2 - valp1 );
    const array1d& normal_cache = this->normal_cache;

    pout[ offset ]     = x;
    pout[ offset + 1 ] = y + mu * this->widthy;
    pout[ offset + 2 ] = z;

    if(MarchingCubes::ENABLE_NORMALS){
        index_t q2 = q + this->localgrid.ystride * 3;
        // q2 is ???
        nout[ offset ]     = lerp( normal_cache[ q ],     normal_cache[ q2 ],     mu );
        nout[ offset + 1 ] = lerp( normal_cache[ q + 1 ], normal_cache[ q2 + 1 ], mu );
        nout[ offset + 2 ] = lerp( normal_cache[ q + 2 ], normal_cache[ q2 + 2 ], mu );
    }
    index3_t e3x = ijk*3+1;
    //std::clog << "e3out.size()" << e3out.size() << std::endl;
    e3out[offset/3] = e3x;
}

template <bool enableUvs, bool enableColors>
inline void MarchingCubes<enableUvs,enableColors>:: VIntZ(index_t q, array1d& pout, array1d& nout, int offset, REAL isol, REAL x, REAL y, REAL z, REAL valp1, REAL valp2,
    index_t ijk, array1d_e3& e3out )
{

    //std::clog << "VIntZZ" << std::endl;

    REAL mu = ( isol - valp1 ) / ( valp2 - valp1 );
    const array1d& normal_cache = this->normal_cache;

    pout[ offset ]     = x;
    pout[ offset + 1 ] = y;
    pout[ offset + 2 ] = z + mu * this->widthz;

    if(MarchingCubes::ENABLE_NORMALS){
        index_t q2 = q + this->localgrid.zstride * 3;

        nout[ offset ]     = lerp( normal_cache[ q ],     normal_cache[ q2 ],     mu );
        nout[ offset + 1 ] = lerp( normal_cache[ q + 1 ], normal_cache[ q2 + 1 ], mu );
        nout[ offset + 2 ] = lerp( normal_cache[ q + 2 ], normal_cache[ q2 + 2 ], mu );
    }

    index3_t e3x = ijk*3+2;
    e3out[offset/3] = e3x;
}

template <bool enableUvs, bool enableColors>
inline void MarchingCubes<enableUvs,enableColors>::compNorm( index_t q ) {
        if(!MarchingCubes::ENABLE_NORMALS){
            std::clog << "This should not hapepn.";
            return;
        }
        index_t q3 = q * 3;
        //What if the x happens to be 0.0 ?
        if ( this->normal_cache[ q3 ] == 0.0 ) {
            this->normal_cache[ q3 ] = this->field[ q - 1 ]            - this->field[ q + 1 ];
            this->normal_cache[ q3 + 1 ] = this->field[ q - this->localgrid.ystride ] - this->field[ q + this->localgrid.ystride ];
            this->normal_cache[ q3 + 2 ] = this->field[ q - this->localgrid.zstride ] - this->field[ q + this->localgrid.zstride ];
        }
}


// Returns total number of triangles. Fills triangles.
// (this is where most of time is spent - it's inner work of O(n3) loop )


template <bool enableUvs, bool enableColors>
inline int MarchingCubes<enableUvs,enableColors>::polygonize_single_cube( REAL fx, REAL fy, REAL fz, index_t q /*, const REAL isol*/ /*, const callback_t& renderCallback*/ ) {
    /** Polygonise a single cube in the grid. */

    constexpr REAL isol = 0.0;

    // cache indices
    index_t qx = q + 1,
        qy = q + this->localgrid.ystride,
        qz = q + this->localgrid.zstride,
        qxy = qx + this->localgrid.ystride,
        qxz = qx + this->localgrid.zstride,
        qyz = q + this->localgrid.ystride + this->localgrid.zstride,
        qxyz = qx + this->localgrid.ystride + this->localgrid.zstride;

    index3_t ijk_0 = q;
    index3_t
             ijk_x = ijk_0 + 1 ,  //xstride==1
             ijk_y = ijk_0 + this->globalbox.ystride ,
             ijk_z = ijk_0 + this->globalbox.zstride ,
             ijk_xy = ijk_0 + 1 + this->globalbox.ystride ,
             ijk_yz = ijk_0 + this->globalbox.ystride + this->globalbox.zstride ,
             ijk_xz = ijk_0 + 1 + this->globalbox.zstride ;

    unsigned int cubeindex = 0;

    REAL
        field0 = this->field[ q ],
        field1 = this->field[ qx ],
        field2 = this->field[ qy ],
        field3 = this->field[ qxy ],
        field4 = this->field[ qz ],
        field5 = this->field[ qxz ],
        field6 = this->field[ qyz ],
        field7 = this->field[ qxyz ];

    if ( field0 < isol ) cubeindex |= 1;
    if ( field1 < isol ) cubeindex |= 2;
    if ( field2 < isol ) cubeindex |= 8;
    if ( field3 < isol ) cubeindex |= 4;
    if ( field4 < isol ) cubeindex |= 16;
    if ( field5 < isol ) cubeindex |= 32;
    if ( field6 < isol ) cubeindex |= 128;
    if ( field7 < isol ) cubeindex |= 64;


    // if cube is entirely in/out of the surface - bail, nothing to draw

    int bits = mc_edge_lookup_table[ cubeindex ];
    if ( bits == 0x00 ) return 0;

    //std::clog  << cubeindex << " ";

    REAL dx = this->widthx;
    REAL dy = this->widthy;
    REAL dz = this->widthz;
    REAL
        fx2 = fx + dx,
        fy2 = fy + dy,
        fz2 = fz + dz;

    // top of the cube

    if ( bits & 1 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( q );
            this->compNorm( qx );
        }
        this->VIntX( q * 3, this->vlist_buffer, this->nlist_buffer, 0, isol, fx, fy, fz, field0, field1,  ijk_0, this->e3list_buffer);

    }

    if ( bits & 2 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qx );
            this->compNorm( qxy );
        }
        this->VIntY( qx * 3, this->vlist_buffer, this->nlist_buffer, 3, isol, fx2, fy, fz, field1, field3,  ijk_x, this->e3list_buffer);

    }

    if ( bits & 4 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qy );
            this->compNorm( qxy );
        }
        this->VIntX( qy * 3, this->vlist_buffer, this->nlist_buffer, 6, isol, fx, fy2, fz, field2, field3,  ijk_y , this->e3list_buffer);

    }

    if ( bits & 8 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( q );
            this->compNorm( qy );
        }
        this->VIntY( q * 3, this->vlist_buffer, this->nlist_buffer, 9, isol, fx, fy, fz, field0, field2,  ijk_0, this->e3list_buffer);

    }

    // bottom of the cube

    if ( bits & 16 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qz );
            this->compNorm( qxz );
        }
        this->VIntX( qz * 3, this->vlist_buffer, this->nlist_buffer, 12, isol, fx, fy, fz2, field4, field5,  ijk_z, this->e3list_buffer);

    }

    if ( bits & 32 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qxz );
            this->compNorm( qxyz );
        }
        this->VIntY( qxz * 3,  this->vlist_buffer, this->nlist_buffer, 15, isol, fx2, fy, fz2, field5, field7,  ijk_xz, this->e3list_buffer );

    }

    if ( bits & 64 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qyz );
            this->compNorm( qxyz );
        }
        this->VIntX( qyz * 3, this->vlist_buffer, this->nlist_buffer, 18, isol, fx, fy2, fz2, field6, field7,  ijk_yz, this->e3list_buffer );

    }

    if ( bits & 128 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qz );
            this->compNorm( qyz );
        }
        this->VIntY( qz * 3,  this->vlist_buffer, this->nlist_buffer, 21, isol, fx, fy, fz2, field4, field6,  ijk_z, this->e3list_buffer );

    }

    // vertical lines of the cube

    if ( bits & 256 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( q );
            this->compNorm( qz );
        }
        this->VIntZ( q * 3, this->vlist_buffer, this->nlist_buffer, 24, isol, fx, fy, fz, field0, field4,  ijk_0, this->e3list_buffer);

    }

    if ( bits & 512 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qx );
            this->compNorm( qxz );
        }
        this->VIntZ( qx * 3,  this->vlist_buffer, this->nlist_buffer, 27, isol, fx2, fy,  fz, field1, field5,  ijk_x, this->e3list_buffer );

    }

    if ( bits & 1024 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qxy );
            this->compNorm( qxyz );
        }
        this->VIntZ( qxy * 3, this->vlist_buffer, this->nlist_buffer, 30, isol, fx2, fy2, fz, field3, field7,  ijk_xy, this->e3list_buffer);

    }

    if ( bits & 2048 ) {
        if(MarchingCubes::ENABLE_NORMALS){
            this->compNorm( qy );
            this->compNorm( qyz );
        }
        this->VIntZ( qy * 3, this->vlist_buffer, this->nlist_buffer, 33, isol, fx,  fy2, fz, field2, field6,  ijk_y, this->e3list_buffer );

    }

    cubeindex <<= 4;  // re-purpose cubeindex into an offset into mc_triangles_table

    //not sure about the type:
    int o1, o2, o3, numtris = 0, i = 0;

    // here is where triangles are created

    while ( mc_triangles_table[ cubeindex + i ] != - 1 ) {
        o1 = cubeindex + i;
        o2 = o1 + 1;
        o3 = o1 + 2;

        //stores the triangles into the buffers
        this->posnormtriv(
            this->vlist_buffer, this->nlist_buffer, this->e3list_buffer,
            3 * MarchingCubes::mc_triangles_table[ o1 ],
            3 * MarchingCubes::mc_triangles_table[ o2 ],
            3 * MarchingCubes::mc_triangles_table[ o3 ]
            /*,renderCallback*/
         );
        //renderCallback consumes them

        i += 3;
        numtris++;
    }
    return numtris;
}

#define DEBUG_PA001(positionQueue , c)   {std::clog << " >" << positionQueue[ (c) ] << positionQueue[ (c) + 1 ] <<    positionQueue[ (c) + 2 ] << "< ";}

/////////////////////////////////////
// Immediate-render mode simulator
/////////////////////////////////////

template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::posnormtriv(
    array1d& pos__vlist, array1d& norm__nlist, array1d_e3& e3__e3list,
    int o1, int o2, int o3 /*,
    const callback_t& renderCallback*/ ) {
    /** Moves data: _list[] into _Queue[] */

    int c = this->queue_counter * 3;

    // positions

    this->positionQueue[ c ]     = pos__vlist[ o1 ];
    this->positionQueue[ c + 1 ] = pos__vlist[ o1 + 1 ];
    this->positionQueue[ c + 2 ] = pos__vlist[ o1 + 2 ];

    //DEBUG_PA001(this->positionQueue , c);

    this->positionQueue[ c + 3 ] = pos__vlist[ o2 ];
    this->positionQueue[ c + 4 ] = pos__vlist[ o2 + 1 ];
    this->positionQueue[ c + 5 ] = pos__vlist[ o2 + 2 ];

    this->positionQueue[ c + 6 ] = pos__vlist[ o3 ];
    this->positionQueue[ c + 7 ] = pos__vlist[ o3 + 1 ];
    this->positionQueue[ c + 8 ] = pos__vlist[ o3 + 2 ];


    int c_div_3 = this->queue_counter; //c/3;
    this->e3Queue[ c_div_3 ]     = e3__e3list[ o1/3 ];
    this->e3Queue[ c_div_3 + 1 ] = e3__e3list[ o2/3 ];
    this->e3Queue[ c_div_3 + 2 ] = e3__e3list[ o3/3 ];  //(c + 3)/3


    //DEBUG_PA001(pos__vlist, o3);
    //std::clog << "[" << o3 << "] ";

    if(MarchingCubes::ENABLE_NORMALS){
        // normals

        this->normalQueue[ c ]     = norm__nlist[ o1 ];
        this->normalQueue[ c + 1 ] = norm__nlist[ o1 + 1 ];
        this->normalQueue[ c + 2 ] = norm__nlist[ o1 + 2 ];

        this->normalQueue[ c + 3 ] = norm__nlist[ o2 ];
        this->normalQueue[ c + 4 ] = norm__nlist[ o2 + 1 ];
        this->normalQueue[ c + 5 ] = norm__nlist[ o2 + 2 ];

        this->normalQueue[ c + 6 ] = norm__nlist[ o3 ];
        this->normalQueue[ c + 7 ] = norm__nlist[ o3 + 1 ];
        this->normalQueue[ c + 8 ] = norm__nlist[ o3 + 2 ];
    }
    // uvs

    if ( enableUvs ) {
        int d = this->queue_counter * 2;

        (*this->uvQueue)[ d ]     = pos__vlist[ o1 ];
        (*this->uvQueue)[ d + 1 ] = pos__vlist[ o1 + 2 ];

        (*this->uvQueue)[ d + 2 ] = pos__vlist[ o2 ];
        (*this->uvQueue)[ d + 3 ] = pos__vlist[ o2 + 2 ];

        (*this->uvQueue)[ d + 4 ] = pos__vlist[ o3 ];
        (*this->uvQueue)[ d + 5 ] = pos__vlist[ o3 + 2 ];
    }

    // colors

    if ( enableColors ) {
        (*this->colorQueue)[ c ]     = pos__vlist[ o1 ];
        (*this->colorQueue)[ c + 1 ] = pos__vlist[ o1 + 1 ];
        (*this->colorQueue)[ c + 2 ] = pos__vlist[ o1 + 2 ];

        (*this->colorQueue)[ c + 3 ] = pos__vlist[ o2 ];
        (*this->colorQueue)[ c + 4 ] = pos__vlist[ o2 + 1 ];
        (*this->colorQueue)[ c + 5 ] = pos__vlist[ o2 + 2 ];

        (*this->colorQueue)[ c + 6 ] = pos__vlist[ o3 ];
        (*this->colorQueue)[ c + 7 ] = pos__vlist[ o3 + 1 ];
        (*this->colorQueue)[ c + 8 ] = pos__vlist[ o3 + 2 ];
    }

    this->queue_counter += 3;

    if ( this->queue_counter >= this->queueSize - 3 ) {  //why equal?
        this->hasPositions = true;
        this->hasNormals = MarchingCubes::ENABLE_NORMALS;
        if ( enableUvs ) {
            this->hasUvs = true;
        }
        if ( enableColors ) {
            this->hasColors = true;
        }
        // renderCallback.call( (void*)this );
        this->sow();
    }
}

/*
static int resultqueue_faces_start = 0;  // static
static std::vector<REAL> result_normals(4100*3);  // static

static std::vector<REAL> result_verts;  // static
static std::vector<vertexindex_type> result_faces; // static
*/

// Takes the vales from the queue:
template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::sow() {
    /*
    typedef array1d::iterator  b_it;
    for(b_it b=this->vlist_buffer.begin(); b < this->vlist_buffer.end(); b++)
        std::clog << *b << " ";
    std::clog << std::endl;
    */
    //std::clog << "Sowing the seeds of love. " << this->queue_counter << std::endl;


    //this->flush_geometry_queue(std::clog, resultqueue_faces_start, result_normals,  result_verts, result_faces);

    this->flush_geometry_queue(std::clog, this->resultqueue_faces_start, this->result_normals,  this->result_verts, this->result_faces,  this->result_e3map, this->next_unique_vect_counter);
}

template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::begin_queue() {
    /** resets the queue. */
    this->queue_counter = 0;

    this->hasPositions = false;
    this->hasNormals = false;
    this->hasUvs = false;
    this->hasColors = false;
}

template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::finish_queue( /*const callback_t& renderCallback*/ ) {
    /** Finish with the queue. Prepares to sow by the callback. */

    // queue_counter := number of prepared (?)
    if ( this->queue_counter == 0 ) return;

    //for ( int i = this->queue_counter * 3; i < this->positionQueue.length; i++ ) {
    //    this->positionQueue[ i ] = 0.0;
    //}

    // Is this really necessary??
    std::fill(this->positionQueue.begin() + (this->queue_counter * 3), this->positionQueue.end(), 0.0 );

    this->hasPositions = true;
    this->hasNormals = MarchingCubes::ENABLE_NORMALS;

    if ( enableUvs ) {
        this->hasUvs = true;
    }

    if ( enableColors ) {
        this->hasColors = true;
    }

    //std::fill(this->e3Queue.begin() + (this->queue_counter), this->e3Queue.end(), 0 );

    /*renderCallback.call(this);*/
    sow();
}


// todo: separate the addBall,seal,etc into the `field` [part of the] class.


template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::seal_exterior(const REAL exterior_value) {

    //const REAL exterior_value = -1.;

    int x, y, z;
    //int cxy;

    // cache attribute lookups
    int ystride = this->localgrid.ystride;
    int zstride = this->localgrid.zstride;
    array1d& field = this->field;

    for ( x = 0; x < this->resolution_x; x++ ) {
        for ( y = 0; y < this->resolution_y; y++ ) {
            // int cxy
            int cxy = x + y * ystride;
            /*
            {
            int z = 0;      field[ zstride * z + cxy ] = exterior_value;
            }{
            int z = resolution_z-1; field[ zstride * z + cxy ] = exterior_value;
            }
            bool border = (x == 0) || (x == resolution_x-1) || (y == 0) || (y == resolution_y-1);
            if(border){
                for ( z = 0; z < resolution_z; z++ ) {
                    field[ zstride * z + cxy ] = exterior_value;
                }
            }
            */
            for ( z = 0; z < this->resolution_z; z++ ) {
                const bool border0 =
                    (x == 0) ||
                    (x == this->resolution_x-1) ||
                    (y == 0) ||
                    (y == this->resolution_y-1) ||
                    (z == 0) ||
                    (z == this->resolution_z-1);

                // Important: It is necessary to leave two cubes empty for sealing.
                const bool border = 
                    (x == 1) || 
                    (x == this->resolution_x-1-1) ||
                    (y == 1) ||
                    (y == this->resolution_y-1-1) ||
                    (z == 1) ||
                    (z == this->resolution_z-1-1);

                // This may fix cylinder's problem
                if (border || border0) {
                    field[ zstride * z + cxy ] = exterior_value;
                }
                //if (z == 4 && x == 2)
                //    field[ zstride * z + cxy ] = +2.;
            }
        }
    }
    /*
    std::clog << "seal_exterior "
        << field[0]  << ","
        << field[ystride]  << ","
        << field[ystride+zstride]  << ";"
        << field[zstride+ystride+1]  << ","
        << field[zstride+ystride+2]  << ","
        << field[zstride+ystride+3]  << ","
        << field[zstride+ystride+4]  << ","
        << field[ zstride * 4 + 2 + (y=3) * ystride ]
        << std::endl;
    */
}


/////////////////////////////////////
// Updates
/////////////////////////////////////

template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::reset_field()
{
    // wipe the normal cache
    for (int i = 0; i < this->gridbox.full_stride; i++ ) {
        #if MARCHINGCUBES_ENABLE_NORMALS
        if(MarchingCubes::ENABLE_NORMALS){
            this->normal_cache[ i * 3 ] = 0.0; // Why the other elements are not done?
        }
        #endif
        this->field[ i ] = 0.0;
    }
}

template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::reset_result() {
    //std::vector<REAL> &normals, std::vector<REAL> &verts3, std::vector<int> &faces3, e3map_t &e3map, int& next_unique_vect_counter

    /*
    this->next_unique_vect_counter = 0;
    this->result_faces = std::vector<vertexindex_type>();
    this->result_verts = std::vector<REAL>();
    this->result_normals = std::vector<REAL>();
    this->result_e3map = e3map_t();
    */

    this->next_unique_vect_counter = 0;
    //this->result_faces = std::vector<vertexindex_type>();
    //this->result_verts = std::vector<REAL>();
    //this->result_normals = std::vector<REAL>();
    //this->result_e3map = e3map_t();

    //preallocate
    int expected_vertices = 10;
    int expected_faces = 10;
    this->result_normals.reserve(expected_faces*3 *(MarchingCubes::ENABLE_NORMALS?1:0));
    this->result_verts.reserve(expected_vertices*3);
    this->result_faces.reserve(expected_faces*3);
    //what about normals?
    //Unfortunately, you cannot reserve elements for a C++ STL map.
    //this->result_e3map.reserve(expected_faces*3 ); //should be less than one third of expercted_verts


    //This does not belong here: this->resultqueue_faces_start

}

// Renderes a geometry.
template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::render_geometry(/*const callback_t& renderCallback*/ ) {
    
    //const callback_t renderCallback;
    
    this->reset_result();  //receiver of the queue
    this->begin_queue();

    const REAL xi0 = (+this->box.xmin) / widthx - MarchingCubes::skip_count_l;
    const REAL yi0 = (+this->box.ymin) / widthy - MarchingCubes::skip_count_l;
    const REAL zi0 = (+this->box.zmin) / widthz - MarchingCubes::skip_count_l;

    // Triangulate. Yeah, this is slow.

    // Note: skip_count_l =is= 2.
    const int end_x = this->resolution_x - 2;
    const int end_y = this->resolution_y - 2;
    const int end_z = this->resolution_z - 2;

    const int start_x = 1;
    const int start_y = 1;
    const int start_z = 1;

    for ( int zi = start_z; zi < end_z; zi++ ) {

        index_t z_offset = this->gridbox.zstride * zi;
        REAL fz = ( zi + zi0 ) * this->widthz; //+ 1

        for ( int yi = start_y; yi < end_y; yi++ ) {

            index_t y_offset = z_offset + this->gridbox.ystride * yi;
            REAL fy = ( yi + yi0 ) * this->widthy; //+ 1

            for ( int xi = start_x; xi < end_x; xi++ ) {

                REAL fx = ( xi + xi0 ) * this->widthx; //+ 1
                index_t q = y_offset + xi;

                // use sub-grids for buffer here.
                this->polygonize_single_cube( fx, fy, fz, q /*, renderCallback*/ );

                /*
                only prints zeros
                std::clog << "************************" << std::endl;
                typedef array1d::iterator  b_it;
                for(b_it b=this->vlist_buffer.begin(); b < this->vlist_buffer.end(); b++)
                    std::clog << *b << " ";
                std::clog << std::endl;
                */

            }
        }
    }
    this->finish_queue(/*renderCallback*/);
}


/*
void flush_geometry_queue() {

    var i, x, y, z, vertex, normal,
        face, a, b, c, na, nb, nc, nfaces;


    for ( i = 0; i < object.queue_counter; i++ ) {

        a = i * 3;
        b = a + 1;
        c = a + 2;

        x = object.positionQueue[ a ];
        y = object.positionQueue[ b ];
        z = object.positionQueue[ c ];
        vertex = new THREE.Vector3( x, y, z );

        x = object.normalQueue[ a ];
        y = object.normalQueue[ b ];
        z = object.normalQueue[ c ];
        normal = new THREE.Vector3( x, y, z );
        normal.normalize();

        geo.vertices.push( vertex );
        normals.push( normal );

    }

    nfaces = object.queue_counter / 3;

    for ( face_i = 0; face_i < nfaces; face_i++ ) {

        a = ( normals_start + face_i ) * 3;
        b = a + 1;
        c = a + 2;

        na = normals[ a ];
        nb = normals[ b ];
        nc = normals[ c ];

        face = new THREE.Face3( a, b, c, [ na, nb, nc ] );

        geo.faces.push( face );

    }

    normals_start += nfaces;
    object.queue_counter = 0;
}
*/
/*

var geo_callback = function( object ) {

    var i, x, y, z, vertex, normal,
        face, a, b, c, na, nb, nc, nfaces;


    for ( i = 0; i < object.queue_counter; i++ ) {

        a = i * 3;
        b = a + 1;
        c = a + 2;

        x = object.positionQueue[ a ];
        y = object.positionQueue[ b ];
        z = object.positionQueue[ c ];
        vertex = new THREE.Vector3( x, y, z );

        x = object.normalQueue[ a ];
        y = object.normalQueue[ b ];
        z = object.normalQueue[ c ];
        normal = new THREE.Vector3( x, y, z );
        normal.normalize();

        geo.vertices.push( vertex );
        normals.push( normal );

    }

    nfaces = object.queue_counter / 3;

    for ( i = 0; i < nfaces; i++ ) {

        a = ( normals_start + i ) * 3;
        b = a + 1;
        c = a + 2;

        na = normals[ a ];
        nb = normals[ b ];
        nc = normals[ c ];

        face = new THREE.Face3( a, b, c, [ na, nb, nc ] );

        geo.faces.push( face );

    }

    normals_start += nfaces;
    object.queue_counter = 0;

};

//this->generateGeometry = function() {...}
Geometry generateGeometry = function()
{
    var normals_start = 0, geo = new THREE.Geometry();
    var normals = [];
    this->render_geometry( geo_callback );
    // console.log( "generated " + geo.faces.length + " triangles" );
    return geo;
};
*/


/*
THREE.MarchingCubes.prototype = Object.create( THREE.ImmediateRenderObject.prototype );
THREE.MarchingCubes.prototype.constructor = THREE.MarchingCubes;
*/

/////////////////////////////////////
// Marching cubes lookup tables
/////////////////////////////////////

// These tables are straight from Paul Bourke's page:
// http://local.wasp.uwa.edu.au/~pbourke/geometry/polygonise/
// who in turn got them from Cory Gene Bloyd.

// Maps (8bit -> 12 bit) all possible 2**8 configurations (cases of grid-node signs) into a set of edges (12 edges in total).
// former name: edgeTable.
template <bool enableUvs, bool enableColors>
const int MarchingCubes<enableUvs,enableColors>::mc_edge_lookup_table[256] = {
    0x000, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x099, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x033, 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0x0aa, 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x066, 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0x0ff, 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x055, 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0x0cc,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0x0cc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x055, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0x0ff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x066, 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0x0aa, 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x033, 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x099, 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x000
};


// Contains 5*3+1 elements: 5 triples, plus a trailing -1
// former name: triTable
template <bool enableUvs, bool enableColors>
const int MarchingCubes<enableUvs,enableColors>::mc_triangles_table[256*16] = {
                                          -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 8, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 1, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 8, 3,   9, 8, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 2,10,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 8, 3,   1, 2,10,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 9, 2,10,   0, 2, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 2, 8, 3,   2,10, 8,  10, 9, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,
 3,11, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0,11, 2,   8,11, 0,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 9, 0,   2, 3,11,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1,11, 2,   1, 9,11,   9, 8,11,                                            -1,-1,-1,  -1,-1,-1,  -1,
 3,10, 1,  11,10, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0,10, 1,   0, 8,10,   8,11,10,                                            -1,-1,-1,  -1,-1,-1,  -1,
 3, 9, 0,   3,11, 9,  11,10, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 8,10,  10, 8,11,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4, 7, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4, 3, 0,   7, 3, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 1, 9,   8, 4, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4, 1, 9,   4, 7, 1,   7, 3, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 2,10,   8, 4, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 3, 4, 7,   3, 0, 4,   1, 2,10,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 2,10,   9, 0, 2,   8, 4, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,
 2,10, 9,   2, 9, 7,   2, 7, 3,   7, 9, 4,                                            -1,-1,-1,  -1,
 8, 4, 7,   3,11, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
11, 4, 7,  11, 2, 4,   2, 0, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 0, 1,   8, 4, 7,   2, 3,11,                                            -1,-1,-1,  -1,-1,-1,  -1,
 4, 7,11,   9, 4,11,   9,11, 2,   9, 2, 1,                                            -1,-1,-1,  -1,
 3,10, 1,   3,11,10,   7, 8, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1,11,10,   1, 4,11,   1, 0, 4,   7,11, 4,                                            -1,-1,-1,  -1,
 4, 7, 8,   9, 0,11,   9,11,10,  11, 0, 3,                                            -1,-1,-1,  -1,
 4, 7,11,   4,11, 9,   9,11,10,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 5, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 9, 5, 4,   0, 8, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 5, 4,   1, 5, 0,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 8, 5, 4,   8, 3, 5,   3, 1, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 2,10,   9, 5, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 3, 0, 8,   1, 2,10,   4, 9, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,
 5, 2,10,   5, 4, 2,   4, 0, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,
 2,10, 5,   3, 2, 5,   3, 5, 4,   3, 4, 8,                                            -1,-1,-1,  -1,
 9, 5, 4,   2, 3,11,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0,11, 2,   0, 8,11,   4, 9, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 5, 4,   0, 1, 5,   2, 3,11,                                            -1,-1,-1,  -1,-1,-1,  -1,
 2, 1, 5,   2, 5, 8,   2, 8,11,   4, 8, 5,                                            -1,-1,-1,  -1,
10, 3,11,  10, 1, 3,   9, 5, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,
 4, 9, 5,   0, 8, 1,   8,10, 1,   8,11,10,                                            -1,-1,-1,  -1,
 5, 4, 0,   5, 0,11,   5,11,10,  11, 0, 3,                                            -1,-1,-1,  -1,
 5, 4, 8,   5, 8,10,  10, 8,11,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 7, 8,   5, 7, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 9, 3, 0,   9, 5, 3,   5, 7, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 7, 8,   0, 1, 7,   1, 5, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 5, 3,   3, 5, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 9, 7, 8,   9, 5, 7,  10, 1, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,
10, 1, 2,   9, 5, 0,   5, 3, 0,   5, 7, 3,                                            -1,-1,-1,  -1,
 8, 0, 2,   8, 2, 5,   8, 5, 7,  10, 5, 2,                                            -1,-1,-1,  -1,
 2,10, 5,   2, 5, 3,   3, 5, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,
 7, 9, 5,   7, 8, 9,   3,11, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 5, 7,   9, 7, 2,   9, 2, 0,   2, 7,11,                                            -1,-1,-1,  -1,
 2, 3,11,   0, 1, 8,   1, 7, 8,   1, 5, 7,                                            -1,-1,-1,  -1,
11, 2, 1,  11, 1, 7,   7, 1, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 5, 8,   8, 5, 7,  10, 1, 3,  10, 3,11,                                            -1,-1,-1,  -1,
 5, 7, 0,   5, 0, 9,   7,11, 0,   1, 0,10,  11,10, 0,                                            -1,
11,10, 0,  11, 0, 3,  10, 5, 0,   8, 0, 7,   5, 7, 0,                                            -1,
11,10, 5,   7,11, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
10, 6, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 8, 3,   5,10, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 9, 0, 1,   5,10, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 8, 3,   1, 9, 8,   5,10, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 6, 5,   2, 6, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 6, 5,   1, 2, 6,   3, 0, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 6, 5,   9, 0, 6,   0, 2, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,
 5, 9, 8,   5, 8, 2,   5, 2, 6,   3, 2, 8,                                            -1,-1,-1,  -1,
 2, 3,11,  10, 6, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
11, 0, 8,  11, 2, 0,  10, 6, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 1, 9,   2, 3,11,   5,10, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,
 5,10, 6,   1, 9, 2,   9,11, 2,   9, 8,11,                                            -1,-1,-1,  -1,
 6, 3,11,   6, 5, 3,   5, 1, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 8,11,   0,11, 5,   0, 5, 1,   5,11, 6,                                            -1,-1,-1,  -1,
 3,11, 6,   0, 3, 6,   0, 6, 5,   0, 5, 9,                                            -1,-1,-1,  -1,
 6, 5, 9,   6, 9,11,  11, 9, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,
 5,10, 6,   4, 7, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4, 3, 0,   4, 7, 3,   6, 5,10,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 9, 0,   5,10, 6,   8, 4, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,
10, 6, 5,   1, 9, 7,   1, 7, 3,   7, 9, 4,                                            -1,-1,-1,  -1,
 6, 1, 2,   6, 5, 1,   4, 7, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 2, 5,   5, 2, 6,   3, 0, 4,   3, 4, 7,                                            -1,-1,-1,  -1,
 8, 4, 7,   9, 0, 5,   0, 6, 5,   0, 2, 6,                                            -1,-1,-1,  -1,
 7, 3, 9,   7, 9, 4,   3, 2, 9,   5, 9, 6,   2, 6, 9,                                            -1,
 3,11, 2,   7, 8, 4,  10, 6, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,
 5,10, 6,   4, 7, 2,   4, 2, 0,   2, 7,11,                                            -1,-1,-1,  -1,
 0, 1, 9,   4, 7, 8,   2, 3,11,   5,10, 6,                                            -1,-1,-1,  -1,
 9, 2, 1,   9,11, 2,   9, 4,11,   7,11, 4,   5,10, 6,                                            -1,
 8, 4, 7,   3,11, 5,   3, 5, 1,   5,11, 6,                                            -1,-1,-1,  -1,
 5, 1,11,   5,11, 6,   1, 0,11,   7,11, 4,   0, 4,11,                                            -1,
 0, 5, 9,   0, 6, 5,   0, 3, 6,  11, 6, 3,   8, 4, 7,                                            -1,
 6, 5, 9,   6, 9,11,   4, 7, 9,   7,11, 9,                                            -1,-1,-1,  -1,
10, 4, 9,   6, 4,10,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4,10, 6,   4, 9,10,   0, 8, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,
10, 0, 1,  10, 6, 0,   6, 4, 0,                                            -1,-1,-1,  -1,-1,-1,  -1,
 8, 3, 1,   8, 1, 6,   8, 6, 4,   6, 1,10,                                            -1,-1,-1,  -1,
 1, 4, 9,   1, 2, 4,   2, 6, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,
 3, 0, 8,   1, 2, 9,   2, 4, 9,   2, 6, 4,                                            -1,-1,-1,  -1,
 0, 2, 4,   4, 2, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 8, 3, 2,   8, 2, 4,   4, 2, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,
10, 4, 9,  10, 6, 4,  11, 2, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 8, 2,   2, 8,11,   4, 9,10,   4,10, 6,                                            -1,-1,-1,  -1,
 3,11, 2,   0, 1, 6,   0, 6, 4,   6, 1,10,                                            -1,-1,-1,  -1,
 6, 4, 1,   6, 1,10,   4, 8, 1,   2, 1,11,   8,11, 1,                                            -1,
 9, 6, 4,   9, 3, 6,   9, 1, 3,  11, 6, 3,                                            -1,-1,-1,  -1,
 8,11, 1,   8, 1, 0,  11, 6, 1,   9, 1, 4,   6, 4, 1,                                            -1,
 3,11, 6,   3, 6, 0,   0, 6, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,
 6, 4, 8,  11, 6, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 7,10, 6,   7, 8,10,   8, 9,10,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 7, 3,   0,10, 7,   0, 9,10,   6, 7,10,                                            -1,-1,-1,  -1,
10, 6, 7,   1,10, 7,   1, 7, 8,   1, 8, 0,                                            -1,-1,-1,  -1,
10, 6, 7,  10, 7, 1,   1, 7, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 2, 6,   1, 6, 8,   1, 8, 9,   8, 6, 7,                                            -1,-1,-1,  -1,
 2, 6, 9,   2, 9, 1,   6, 7, 9,   0, 9, 3,   7, 3, 9,                                            -1,
 7, 8, 0,   7, 0, 6,   6, 0, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,
 7, 3, 2,   6, 7, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 2, 3,11,  10, 6, 8,  10, 8, 9,   8, 6, 7,                                            -1,-1,-1,  -1,
 2, 0, 7,   2, 7,11,   0, 9, 7,   6, 7,10,   9,10, 7,                                            -1,
 1, 8, 0,   1, 7, 8,   1,10, 7,   6, 7,10,   2, 3,11,                                            -1,
11, 2, 1,  11, 1, 7,  10, 6, 1,   6, 7, 1,                                            -1,-1,-1,  -1,
 8, 9, 6,   8, 6, 7,   9, 1, 6,  11, 6, 3,   1, 3, 6,                                            -1,
 0, 9, 1,  11, 6, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 7, 8, 0,   7, 0, 6,   3,11, 0,  11, 6, 0,                                            -1,-1,-1,  -1,
 7,11, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 7, 6,11,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 3, 0, 8,  11, 7, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 1, 9,  11, 7, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 8, 1, 9,   8, 3, 1,  11, 7, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,
10, 1, 2,   6,11, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 2,10,   3, 0, 8,   6,11, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,
 2, 9, 0,   2,10, 9,   6,11, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,
 6,11, 7,   2,10, 3,  10, 8, 3,  10, 9, 8,                                            -1,-1,-1,  -1,
 7, 2, 3,   6, 2, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 7, 0, 8,   7, 6, 0,   6, 2, 0,                                            -1,-1,-1,  -1,-1,-1,  -1,
 2, 7, 6,   2, 3, 7,   0, 1, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 6, 2,   1, 8, 6,   1, 9, 8,   8, 7, 6,                                            -1,-1,-1,  -1,
10, 7, 6,  10, 1, 7,   1, 3, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,
10, 7, 6,   1, 7,10,   1, 8, 7,   1, 0, 8,                                            -1,-1,-1,  -1,
 0, 3, 7,   0, 7,10,   0,10, 9,   6,10, 7,                                            -1,-1,-1,  -1,
 7, 6,10,   7,10, 8,   8,10, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,
 6, 8, 4,  11, 8, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 3, 6,11,   3, 0, 6,   0, 4, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,
 8, 6,11,   8, 4, 6,   9, 0, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 4, 6,   9, 6, 3,   9, 3, 1,  11, 3, 6,                                            -1,-1,-1,  -1,
 6, 8, 4,   6,11, 8,   2,10, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 2,10,   3, 0,11,   0, 6,11,   0, 4, 6,                                            -1,-1,-1,  -1,
 4,11, 8,   4, 6,11,   0, 2, 9,   2,10, 9,                                            -1,-1,-1,  -1,
10, 9, 3,  10, 3, 2,   9, 4, 3,  11, 3, 6,   4, 6, 3,                                            -1,
 8, 2, 3,   8, 4, 2,   4, 6, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 4, 2,   4, 6, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 9, 0,   2, 3, 4,   2, 4, 6,   4, 3, 8,                                            -1,-1,-1,  -1,
 1, 9, 4,   1, 4, 2,   2, 4, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,
 8, 1, 3,   8, 6, 1,   8, 4, 6,   6,10, 1,                                            -1,-1,-1,  -1,
10, 1, 0,  10, 0, 6,   6, 0, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,
 4, 6, 3,   4, 3, 8,   6,10, 3,   0, 3, 9,  10, 9, 3,                                            -1,
10, 9, 4,   6,10, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4, 9, 5,   7, 6,11,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 8, 3,   4, 9, 5,  11, 7, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,
 5, 0, 1,   5, 4, 0,   7, 6,11,                                            -1,-1,-1,  -1,-1,-1,  -1,
11, 7, 6,   8, 3, 4,   3, 5, 4,   3, 1, 5,                                            -1,-1,-1,  -1,
 9, 5, 4,  10, 1, 2,   7, 6,11,                                            -1,-1,-1,  -1,-1,-1,  -1,
 6,11, 7,   1, 2,10,   0, 8, 3,   4, 9, 5,                                            -1,-1,-1,  -1,
 7, 6,11,   5, 4,10,   4, 2,10,   4, 0, 2,                                            -1,-1,-1,  -1,
 3, 4, 8,   3, 5, 4,   3, 2, 5,  10, 5, 2,  11, 7, 6,                                            -1,
 7, 2, 3,   7, 6, 2,   5, 4, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 5, 4,   0, 8, 6,   0, 6, 2,   6, 8, 7,                                            -1,-1,-1,  -1,
 3, 6, 2,   3, 7, 6,   1, 5, 0,   5, 4, 0,                                            -1,-1,-1,  -1,
 6, 2, 8,   6, 8, 7,   2, 1, 8,   4, 8, 5,   1, 5, 8,                                            -1,
 9, 5, 4,  10, 1, 6,   1, 7, 6,   1, 3, 7,                                            -1,-1,-1,  -1,
 1, 6,10,   1, 7, 6,   1, 0, 7,   8, 7, 0,   9, 5, 4,                                            -1,
 4, 0,10,   4,10, 5,   0, 3,10,   6,10, 7,   3, 7,10,                                            -1,
 7, 6,10,   7,10, 8,   5, 4,10,   4, 8,10,                                            -1,-1,-1,  -1,
 6, 9, 5,   6,11, 9,  11, 8, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,
 3, 6,11,   0, 6, 3,   0, 5, 6,   0, 9, 5,                                            -1,-1,-1,  -1,
 0,11, 8,   0, 5,11,   0, 1, 5,   5, 6,11,                                            -1,-1,-1,  -1,
 6,11, 3,   6, 3, 5,   5, 3, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 2,10,   9, 5,11,   9,11, 8,  11, 5, 6,                                            -1,-1,-1,  -1,
 0,11, 3,   0, 6,11,   0, 9, 6,   5, 6, 9,   1, 2,10,                                            -1,
11, 8, 5,  11, 5, 6,   8, 0, 5,  10, 5, 2,   0, 2, 5,                                            -1,
 6,11, 3,   6, 3, 5,   2,10, 3,  10, 5, 3,                                            -1,-1,-1,  -1,
 5, 8, 9,   5, 2, 8,   5, 6, 2,   3, 8, 2,                                            -1,-1,-1,  -1,
 9, 5, 6,   9, 6, 0,   0, 6, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,
 1, 5, 8,   1, 8, 0,   5, 6, 8,   3, 8, 2,   6, 2, 8,                                            -1,
 1, 5, 6,   2, 1, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 3, 6,   1, 6,10,   3, 8, 6,   5, 6, 9,   8, 9, 6,                                            -1,
10, 1, 0,  10, 0, 6,   9, 5, 0,   5, 6, 0,                                            -1,-1,-1,  -1,
 0, 3, 8,   5, 6,10,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
10, 5, 6,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
11, 5,10,   7, 5,11,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
11, 5,10,  11, 7, 5,   8, 3, 0,                                            -1,-1,-1,  -1,-1,-1,  -1,
 5,11, 7,   5,10,11,   1, 9, 0,                                            -1,-1,-1,  -1,-1,-1,  -1,
10, 7, 5,  10,11, 7,   9, 8, 1,   8, 3, 1,                                            -1,-1,-1,  -1,
11, 1, 2,  11, 7, 1,   7, 5, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 8, 3,   1, 2, 7,   1, 7, 5,   7, 2,11,                                            -1,-1,-1,  -1,
 9, 7, 5,   9, 2, 7,   9, 0, 2,   2,11, 7,                                            -1,-1,-1,  -1,
 7, 5, 2,   7, 2,11,   5, 9, 2,   3, 2, 8,   9, 8, 2,                                            -1,
 2, 5,10,   2, 3, 5,   3, 7, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,
 8, 2, 0,   8, 5, 2,   8, 7, 5,  10, 2, 5,                                            -1,-1,-1,  -1,
 9, 0, 1,   5,10, 3,   5, 3, 7,   3,10, 2,                                            -1,-1,-1,  -1,
 9, 8, 2,   9, 2, 1,   8, 7, 2,  10, 2, 5,   7, 5, 2,                                            -1,
 1, 3, 5,   3, 7, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 8, 7,   0, 7, 1,   1, 7, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 0, 3,   9, 3, 5,   5, 3, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9, 8, 7,   5, 9, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 5, 8, 4,   5,10, 8,  10,11, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,
 5, 0, 4,   5,11, 0,   5,10,11,  11, 3, 0,                                            -1,-1,-1,  -1,
 0, 1, 9,   8, 4,10,   8,10,11,  10, 4, 5,                                            -1,-1,-1,  -1,
10,11, 4,  10, 4, 5,  11, 3, 4,   9, 4, 1,   3, 1, 4,                                            -1,
 2, 5, 1,   2, 8, 5,   2,11, 8,   4, 5, 8,                                            -1,-1,-1,  -1,
 0, 4,11,   0,11, 3,   4, 5,11,   2,11, 1,   5, 1,11,                                            -1,
 0, 2, 5,   0, 5, 9,   2,11, 5,   4, 5, 8,  11, 8, 5,                                            -1,
 9, 4, 5,   2,11, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 2, 5,10,   3, 5, 2,   3, 4, 5,   3, 8, 4,                                            -1,-1,-1,  -1,
 5,10, 2,   5, 2, 4,   4, 2, 0,                                            -1,-1,-1,  -1,-1,-1,  -1,
 3,10, 2,   3, 5,10,   3, 8, 5,   4, 5, 8,   0, 1, 9,                                            -1,
 5,10, 2,   5, 2, 4,   1, 9, 2,   9, 4, 2,                                            -1,-1,-1,  -1,
 8, 4, 5,   8, 5, 3,   3, 5, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 4, 5,   1, 0, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 8, 4, 5,   8, 5, 3,   9, 0, 5,   0, 3, 5,                                            -1,-1,-1,  -1,
 9, 4, 5,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4,11, 7,   4, 9,11,   9,10,11,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 8, 3,   4, 9, 7,   9,11, 7,   9,10,11,                                            -1,-1,-1,  -1,
 1,10,11,   1,11, 4,   1, 4, 0,   7, 4,11,                                            -1,-1,-1,  -1,
 3, 1, 4,   3, 4, 8,   1,10, 4,   7, 4,11,  10,11, 4,                                            -1,
 4,11, 7,   9,11, 4,   9, 2,11,   9, 1, 2,                                            -1,-1,-1,  -1,
 9, 7, 4,   9,11, 7,   9, 1,11,   2,11, 1,   0, 8, 3,                                            -1,
11, 7, 4,  11, 4, 2,   2, 4, 0,                                            -1,-1,-1,  -1,-1,-1,  -1,
11, 7, 4,  11, 4, 2,   8, 3, 4,   3, 2, 4,                                            -1,-1,-1,  -1,
 2, 9,10,   2, 7, 9,   2, 3, 7,   7, 4, 9,                                            -1,-1,-1,  -1,
 9,10, 7,   9, 7, 4,  10, 2, 7,   8, 7, 0,   2, 0, 7,                                            -1,
 3, 7,10,   3,10, 2,   7, 4,10,   1,10, 0,   4, 0,10,                                            -1,
 1,10, 2,   8, 7, 4,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4, 9, 1,   4, 1, 7,   7, 1, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,
 4, 9, 1,   4, 1, 7,   0, 8, 1,   8, 7, 1,                                            -1,-1,-1,  -1,
 4, 0, 3,   7, 4, 3,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 4, 8, 7,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 9,10, 8,  10,11, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 3, 0, 9,   3, 9,11,  11, 9,10,                                            -1,-1,-1,  -1,-1,-1,  -1,
 0, 1,10,   0,10, 8,   8,10,11,                                            -1,-1,-1,  -1,-1,-1,  -1,
 3, 1,10,  11, 3,10,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 2,11,   1,11, 9,   9,11, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,
 3, 0, 9,   3, 9,11,   1, 2, 9,   2,11, 9,                                            -1,-1,-1,  -1,
 0, 2,11,   8, 0,11,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 3, 2,11,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 2, 3, 8,   2, 8,10,  10, 8, 9,                                            -1,-1,-1,  -1,-1,-1,  -1,
 9,10, 2,   0, 9, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 2, 3, 8,   2, 8,10,   0, 1, 8,   1,10, 8,                                            -1,-1,-1,  -1,
 1,10, 2,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 1, 3, 8,   9, 1, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 9, 1,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
 0, 3, 8,                                            -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,
                                         -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1,   -1
};


const bool VERTS_FROM_MAP = true;


typedef struct {
    std::vector<REAL> &normals;
    std::vector<REAL> &verts3;
    std::vector<vertexindex_type> &faces3;
    MarchingCubes<false>::e3map_t &e3map;
    int& next_unique_vect_counter;
} result_state;

//void flush_geometry_queue(MarchingCubes& object) {

template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::flush_geometry_queue(std::ostream& cout, int& normals_start,
    //outputs:
    std::vector<REAL> &normals, std::vector<REAL> &verts3, std::vector<vertexindex_type> &faces3, e3map_t &e3map, int& next_unique_vect_counter)
{
    //todo: receive a facces and verts vector.
    /** consumes the queue. (sow)*/
    //changes the queue. => should be inside the queue's "territory".

    //MarchingCubes& this-> = *this;

    //todo: refactor: vert_i -> local_vert_i,  global_vert_i = local_vert_i + normals_start*3;  ; local === within/in Queue
    for ( int vert_i = 0; vert_i < this->queue_counter; vert_i++ ) {

        int a = vert_i * 3;
        int b = a + 1;
        int c = a + 2;

        REAL x,y,z;
        x = this->positionQueue[ a ];
        y = this->positionQueue[ b ];
        z = this->positionQueue[ c ];
        //vertex = new THREE.Vector3( x, y, z );
        //cout << "(" << x << " " << y << " " << z << ")    ";
        if(!VERTS_FROM_MAP){
            verts3.push_back(x);
            verts3.push_back(y);
            verts3.push_back(z);
        }

        if(VERTS_FROM_MAP)
        {

            //index3_t  e3_code = this->e3Queue[vert_i];
            //index3_t  e3_code = this->e3Queue[vert_i + normals_start*3];
            index3_t  e3_code = this->e3Queue[vert_i];
            std::pair<e3map_t::iterator, bool> e = e3map.emplace(e3_code, next_unique_vect_counter);
            const bool& novel = e.second;


            int overall_vert_index;
            if(novel)
                overall_vert_index = next_unique_vect_counter;
            else
                overall_vert_index = e.first->second;  // e.first = map entry, i.e. (key, value). Its .second=value is the unique sorted certex index.

            if(novel)
                next_unique_vect_counter++;

            //struct {} next_unique_vect_counter;
            // Dont use next_unique_vect_counter below this point.

            if(novel){
                verts3.push_back(x);
                verts3.push_back(y);
                verts3.push_back(z);
            }
            else{
                assert(verts3.size()/3 > overall_vert_index);
                assert(overall_vert_index < next_unique_vect_counter);
            }

            //Loop invariant
            assert(verts3.size()/3 == next_unique_vect_counter);

            faces3.push_back(overall_vert_index);
            int old_overall_vert_index = vert_i + normals_start*3;  // // old index was this. not used now.
        }

        //e3map_counter ++;
        //e3map.push_back(e3_code);
        //verts_e3

        if(MarchingCubes::ENABLE_NORMALS){
            x = this->normalQueue[ a ];
            y = this->normalQueue[ b ];
            z = this->normalQueue[ c ];
            //normal = new THREE.Vector3( x, y, z ); normal.normalize();
            //cout << x << " " << y << " " << z << std::endl;

            //geo.vertices.push( vertex );
            //normals.push( normal );
            REAL nd = sqrt((x*x)+(y*y)+(z*z));
            if(fabs(nd)<0.000001)
                nd = 0.0001;
            normals.push_back( (REAL)(x / nd) );
            normals.push_back( (REAL)(y / nd) );
            normals.push_back( (REAL)(z / nd) );
        }


    }


    int nfaces = this->queue_counter / 3;

    for ( int face_i = 0; face_i < nfaces; face_i++ ) {

        int a = ( normals_start + face_i ) * 3;
        int b = a + 1;
        int c = a + 2;

        if(MarchingCubes::ENABLE_NORMALS){
            // Why does it store them in normals and reads them back?
            REAL na = normals[ a ];
            REAL nb = normals[ b ];
            REAL nc = normals[ c ];
        }

        //face = new THREE.Face3( a, b, c, [ na, nb, nc ] );
        //geo.faces.push( face );

        if(!VERTS_FROM_MAP){
            faces3.push_back(a);
            faces3.push_back(b);
            faces3.push_back(c);
        }
        //faces3.push_back(na);
        //faces3.push_back(nb);
        //faces3.push_back(nc);
    }

    normals_start += nfaces;
    this->queue_counter = 0;

    // Why not directly write back an array into the "index" and other geometry arrays? (i.e. doing part of the making of geometry on C++ side)
    if(REPORT_STATS){
    std::clog << "flush_geometry_queue(): " ;
    int mapctr = 0;
    for (auto& kv_pair: e3map){
        if(0)
            std::clog << " [" << kv_pair.first << ':' << kv_pair.second << ']';
        mapctr++;
    }
    std::clog << " e3Map: " << mapctr;
    std::clog << " Faces: " << faces3.size()/3;
    std::clog << " Verts: " << verts3.size()/3;
    std::clog << std::endl;
    }
}

template <bool enableUvs, bool enableColors>
vectorized_vect
MarchingCubes<enableUvs,enableColors>::prepare_grid() {
      int min_xi = 0;
      int max_xi = this->resolution_x;
      int min_yi = 0;
      int max_yi = this->resolution_y;
      int min_zi = 0;
      int max_zi = this->resolution_z;

      REAL wx = this->box.xmax - this->box.xmin ;
      REAL wy = this->box.ymax - this->box.ymin ;
      REAL wz = this->box.zmax - this->box.zmin ;

      // to make sure the skipped cubes are outside the bounding box.
      auto exi = this->resolution_x - MarchingCubes::skip_count_l - MarchingCubes::skip_count_h;
      auto eyi = this->resolution_y - MarchingCubes::skip_count_l - MarchingCubes::skip_count_h;
      auto ezi = this->resolution_z - MarchingCubes::skip_count_l - MarchingCubes::skip_count_h;

      REAL xfactor = wx /(REAL)( exi );
      REAL yfactor = wy /(REAL)( eyi );
      REAL zfactor = wz /(REAL)( ezi );

      boost::array<vectorized_vect::index, 2> grid_shape = {{ this->resolution_x * this->resolution_y * this->resolution_z , 3 }};
      vectorized_vect  grid(grid_shape);

      // Todo: write an iterator
      for (int z = min_zi; z < max_zi; z++ ) {
          for (int y = min_yi; y < max_yi; y++ ) {
              for (int x = min_xi; x < max_xi; x++ ) {
                  vectorized_vect::index  i = x + y * this->gridbox.ystride + z * this->gridbox.zstride;
                  grid[i][0] = (REAL)x * xfactor + this->box.xmin - MarchingCubes::skip_count_l * this->widthx;
                  grid[i][1] = (REAL)y * yfactor + this->box.ymin - MarchingCubes::skip_count_l * this->widthy;
                  grid[i][2] = (REAL)z * zfactor + this->box.zmin - MarchingCubes::skip_count_l * this->widthz;
              }
          }
      }
      return grid;
}

//Based on mcc2_MS.cpp
template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::eval_shape(const mp5_implicit::implicit_function& object, const boost::multi_array<REAL, 2>& mcgrid_vectorized ){

      boost::array<vectorized_vect::index, 1> implicit_values_shape = {{ this->resolution_x * this->resolution_y * this->resolution_z }};
      boost::multi_array<REAL, 1> implicit_values(implicit_values_shape);

      // todo: skip the skip_count_l and skip_count_h
      object.eval_implicit(mcgrid_vectorized, &implicit_values);

      int min_xi = 0;
      int min_yi = 0;
      int min_zi = 0;
      int max_xi = this->resolution_x;
      int max_yi = this->resolution_y;
      int max_zi = this->resolution_z;

      // todo: make this unnecessary, or a simple assignment. Or a simple flat for loop.
      for (int z = min_zi; z < max_zi; z++ ) {
          for (int y = min_yi; y < max_yi; y++ ) {
              for (int x = min_xi; x < max_xi; x++ ) {
                this->field[x + y*this->gridbox.ystride + z*this->gridbox.zstride] += implicit_values[x + y*this->gridbox.ystride + z*this->gridbox.zstride];
              }
          }
      }
}

template <bool enableUvs, bool enableColors>
void MarchingCubes<enableUvs,enableColors>::produce_mesh(const mp5_implicit::implicit_function& object) {
    // moved from mcc2.cpp

    // Does thismake things slower ?
    vectorized_vect   mcgrid_vectorized = this -> prepare_grid();  // 10.0
    this -> eval_shape(object, mcgrid_vectorized);
    this -> seal_exterior(-10000000.0);

    this -> render_geometry();

}

}  // namespace marching_cubes

using marching_cubes::MarchingCubes;
