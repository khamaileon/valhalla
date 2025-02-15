#ifndef VALHALLA_MJOLNIR_FERRY_CONNECTIONS_H_
#define VALHALLA_MJOLNIR_FERRY_CONNECTIONS_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <valhalla/mjolnir/dataquality.h>
#include <valhalla/mjolnir/node_expander.h>
#include <valhalla/mjolnir/osmdata.h>

namespace valhalla {
namespace mjolnir {

// ---------------------------------------------------------------------------
// This file contains methods to reclassify edges that connect to ferries such
// that a path from the highway hierarchy (configurable) to the ferry. This is
// needed on long routes so the hierarchy pruning doesn't cut out ferry
// connections which in OSM are often tagged as "service" (lowest
// classification).
// ---------------------------------------------------------------------------

// Unreached - not yet encountered in search
constexpr uint32_t kUnreached = 0;

// Permanent - shortest path to this edge has been found
constexpr uint32_t kPermanent = 1;

// Temporary - edge has been encountered but there could
// still be a shorter path to this node. This node will
// be "adjacent" to an node that is permanently labeled.
constexpr uint32_t kTemporary = 2;

constexpr uint32_t kFerryUpClass = static_cast<uint32_t>(baldr::RoadClass::kPrimary);

// NodeLabel - for simple shortest path
struct NodeLabel {
  float cost;
  uint32_t node_index;
  uint32_t pred_node_index;
  bool dest_only;

  NodeLabel(const float c, const uint32_t n, const uint32_t p, const bool d)
      : cost(c), node_index(n), pred_node_index(p), dest_only(d) {
  }
};

// Store the node label status and its index in the NodeLabels list
struct NodeStatusInfo {
  uint32_t set;
  uint32_t index;
  NodeStatusInfo() : set(kUnreached), index(0) {
  }
  NodeStatusInfo(const uint32_t s, const uint32_t idx) : set(s), index(idx) {
  }
};

/**
 * Get the best classification for any drivable non-ferry and non-link
 * edges from a node. Skip any reclassified ferry edges
 * @param  edges The file backed list of edges in the graph.
 * @return  Returns the best (most important) classification
 */
uint32_t GetBestNonFerryClass(const std::map<Edge, size_t>& edges);

/**
 * Form the shortest path from the start node until a node that
 * touches the specified road classification.
 */
uint32_t ShortestPath(const uint32_t start_node_idx,
                      const uint32_t node_idx,
                      sequence<OSMWay>& ways,
                      sequence<OSMWayNode>& way_nodes,
                      sequence<Edge>& edges,
                      sequence<Node>& nodes,
                      const bool inbound,
                      const bool remove_dest_only);

/**
 * Check if the ferry included in this node bundle is short. Must be
 * just one edge and length < 2 km. This prevents forming connections
 * to what are most likely river crossing ferries.
 */
bool ShortFerry(const uint32_t node_index,
                node_bundle& bundle,
                sequence<Edge>& edges,
                sequence<Node>& nodes,
                sequence<OSMWay>& ways,
                sequence<OSMWayNode>& way_nodes);

/**
 * Reclassify edges from a ferry along the shortest path to the
 * specified road classification.
 */
void ReclassifyFerryConnections(const std::string& ways_file,
                                const std::string& way_nodes_file,
                                const std::string& nodes_file,
                                const std::string& edges_file);

} // namespace mjolnir
} // namespace valhalla
#endif // VALHALLA_MJOLNIR_FERRY_CONNECTIONS_H_
