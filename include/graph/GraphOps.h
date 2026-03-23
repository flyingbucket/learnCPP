#ifndef INCLUDE_GRAPH_GRAPAOPS_H
#define INCLUDE_GRAPH_GRAPAOPS_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int VertexId;
typedef double Weight;

typedef struct GraphInfoOps {
  int (*vertex_count)(void* G);
  bool (*valid_vertex)(void* G, VertexId v);
  bool (*directed)(void* G);
} GraphInfoOps;

typedef struct GraphQueryOps {
  bool (*adjacent)(void* G, VertexId v1, VertexId v2);
  int (*first_neighbor)(void* G, VertexId v);
  int (*next_neighbor)(void* G, VertexId v, VertexId w);
} GraphQueryOps;

typedef struct GraphMutateOps {
  VertexId (*add_vert)(void* G);
  bool (*delete_vert)(void* G, VertexId v);
  bool (*add_edge)(void* G, VertexId v1, VertexId v2, Weight w);
  bool (*remove_edge)(void* G, VertexId v1, VertexId v2);
} GraphMutateOps;

typedef struct WeightedGraphOps {
  Weight (*update_edge_weight)(void* G, VertexId v1, VertexId v2, Weight w);
  Weight (*get_edge_weight)(void* G, VertexId v1, VertexId v2);
} WeightedGraphOps;

typedef struct {
  const GraphInfoOps* iops;
  const GraphQueryOps* qops;
  const GraphMutateOps* mops;
  const WeightedGraphOps* wops;
} BaseGraph;

#ifdef __cplusplus
}
#endif

// typedef enum {
//   WEIGHT_INT,
//   WEIGHT_DOUBLE,
//   WEIGHT_FLOAT,
// } WeightType;
//
// typedef struct {
//   WeightType type;
//   union {
//     int i;
//     double d;
//     float f;
//   } value;
// } Weight;
#endif  // !INCLUDE_GRAPH_GRAPAOPS_H
