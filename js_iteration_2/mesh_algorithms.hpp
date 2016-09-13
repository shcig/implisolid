/*
This file contains functions as building blocks ("code blocks") that implement algorithms and operations on meshes.

These functions are not organised in classes. An OOP design can call these, but should not include the bodies.

No function should use any "verts". These are operations that are done on a "faces_t" variable.

These are efficient graph theoretic algorithms. Most of them involve building data structures involved in graph adjancency structure.

@author sohale
@author marcFraysse
@author schauvier
*/

#pragma once
namespace mp5_implicit {

void make_edge_lookup(faces_t faces, faces_t& edges_of_faces, faces_t& faces_of_edges){
  int nfaces = faces.shape()[0];
  cout << "nfaces is : " << nfaces << endl;
  assert(nfaces % 2 == 0);
  int num_edges = nfaces*3./2.;

  long modulo = long(num_edges);
  long lookup_array_size = modulo*num_edges + num_edges;
  map<int, int> eulookup;
  map<int, int>::iterator iter;
  int edge_counter = 0;
  for (int fi=0; fi<nfaces; fi++){
    for (int vj=0; vj<3; vj++){
      assert(fi<nfaces);
      bool new_edge = false;
      int v2j = (vj+1)%3;
      int e1 = faces[fi][vj];
      int e2 = faces[fi][v2j];
      int eu_pair_int;
      if (e2 > e1){
        eu_pair_int = int(e1 + e2*modulo);
      }
      else{
        eu_pair_int = int(e2 + e1*modulo);
      }

      iter = eulookup.find(eu_pair_int);
      if (iter== eulookup.end()){
        new_edge = true;
      }
      if (new_edge){
        int e_id = edge_counter;
        edges_of_faces[fi][vj] = e_id;
        faces_of_edges[e_id][0] = fi;
        assert (vj!= v2j);

        eulookup.insert(pair<int,int>(eu_pair_int,e_id));
        edge_counter ++;
        assert(edge_counter <= num_edges);
      }
      else{
        int e_id = iter->second;
        assert (e_id >= 0);
        edges_of_faces[fi][vj] = e_id;
        faces_of_edges[e_id][1] = fi;
        assert (vj!= v2j);
        int other_fi = faces_of_edges[e_id][0];

      }
    }
  }

}

void build_faces_of_faces(const faces_t& edges_of_faces, const faces_t& faces_of_edges, faces_t& faces_of_faces){
  for(int face = 0; face < edges_of_faces.shape()[0]; face++){
    for(int edge = 0; edge < 3; edge++){
      if(faces_of_edges[edges_of_faces[face][edge]][0]!=face)
        faces_of_faces[face][edge]=faces_of_edges[edges_of_faces[face][edge]][0];
      else{
        assert(faces_of_edges[edges_of_faces[face][edge]][1] != face);
        faces_of_faces[face][edge]=faces_of_edges[edges_of_faces[face][edge]][1];}
    }
  }
  for(int face = 0; face < faces_of_faces.shape()[0]; face ++){
    for(int faces = 0; faces<3; faces++){
      assert(face==faces_of_faces[faces_of_faces[face][faces]][0] ||
          face==faces_of_faces[faces_of_faces[face][faces]][1] ||
          face==faces_of_faces[faces_of_faces[face][faces]][2]);
    }
  }
}


void print_faces(const faces_t& faces) {
    for( int i = 0 ; i < faces.size(); i++) {
        std::clog << "Face " << i << ": ";
        for( int j = 0 ; j < 3; j++) {
            std::clog << faces[i][j] << ", ";
        }
        std::clog << std::endl;
    }
}

void print_vertex_neighbourhood(const std::vector< std::vector<int>> & vertex_neighbours_list) {
    for( int i = 0 ; i < vertex_neighbours_list.size(); i++) {
        const std::vector<int> & na = vertex_neighbours_list[i];
        if (na.size() > 0) {
            std::clog << "Vertex " << i << ": ";
            for( int j = 0 ; j < na.size(); j++) {
                std::clog << na[j] << ", ";
            }
            std::clog << std::endl;
        }
    }
    /*
      // It is correct.

                F  F  F  F
      Vertex 0: 0, 1, 2, 3.
      Vertex 1: 0, 1, 4, 5.
      Vertex 2: 0, 2, 4, 6.
      Vertex 3: 1, 3, 5, 7.
      Vertex 4: 2, 3, 6, 7.
      Vertex 5: 4, 5, 6, 7.

              V  V  V
      Face 0: 0, 1, 2,
      Face 1: 1, 0, 3,
      Face 2: 0, 2, 4,
      Face 3: 3, 0, 4,
      Face 4: 1, 5, 2,
      Face 5: 1, 3, 5,
      Face 6: 2, 5, 4,
      Face 7: 3, 4, 5,
    */
}

std::vector< std::vector<int>> make_neighbour_faces_of_vertex(const faces_t& faces, vertexindex_type  max_vert_index) {
    /*
    note: max_vert_index could be derived from maximum index in faces, but 1-It is safer, may prevent future capacity increase? (not really needed though) 2- It will not be much smaller enyway
    This will be replaced by the more efficient "sparse" matrices (or equivalent data structures) anyway.
    */
    vertexindex_type num_verts = max_vert_index;  // verts.shape()[0];
    std::vector< std::vector<int>> neighbour_faces_of_vertex;
    for (vertexindex_type vi=0; vi < num_verts; vi++) {
        neighbour_faces_of_vertex.push_back(std::vector<int>());
    }

    // todo: initialise using C++ RAII principle:
    // std::vector< std::vector<int>> neighbour_faces_of_vertex(max_vert_index);

    // static_assert(vertexindex_type == neighbour_faces_of_vertex::index_type);
    // static_assert(faces_t::value_type == vertexindex_type);

    int num_faces = faces.shape()[0];
    for (int fi=0; fi < num_faces; fi++) {
        for (int side_i=0; side_i < 3; side_i++) {
            vertexindex_type v1 = faces[fi][side_i];
            neighbour_faces_of_vertex[v1].push_back(fi);
        }
    }

    #if DEBUG_VERBOSE
    print_faces(faces);
    print_vertex_neighbourhood(neighbour_faces_of_vertex);
    #endif

    return neighbour_faces_of_vertex;
}


}   // namespace

