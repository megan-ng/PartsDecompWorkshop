# Description: Create tidy graph object with the fields out_degree and min_dist (from nearest core)
createTidyGraph <- function(graph_file_path, core_file_path) {
  
  # Read in edges as data frame and rename vertex pair columns
  edges <- read.csv(graph_file_path, header = FALSE)
  colnames(edges) <- c("source", "sink")
  
  # Create iGraph object (good for computations)
  iGraph_object <- graph_from_data_frame(edges, directed = FALSE)
  V(iGraph_object)$name <- as.character(seq_len(vcount(iGraph_object)))
  
  # Compute distances in igraph from core nodes (as vector of vertex indices)
  # Load core node IDs
  core_ids <- read.csv(core_file_path, header = FALSE)[[1]]
  core_ids <- as.character(core_ids)
  
  # Compute distances
  min_distances <- computeDistanceToNearestCore(iGraph_object, core_ids)
  
  # Convert to tidygraph (good for plotting) and append colour attributes
  tidy_graph_object <- as_tbl_graph(iGraph_object)
  tidy_graph_object <- tidy_graph_object %>% mutate(min_dist = min_distances[as.numeric(name)])
  tidy_graph_object <- tidy_graph_object %>% mutate(out_degree = centrality_degree(mode = "out"))
  
  return(tidy_graph_object)
}


# Description: For each node in a graph, compute hop distance to nearest core
computeDistanceToNearestCore <- function(iGraph_object, core_ids) {

  core_indices <- which(V(iGraph_object)$name %in% core_ids)
  dist_matrix <- distances(iGraph_object, v = core_indices, to = V(iGraph_object), mode = "out")
  min_distances <- apply(dist_matrix, 2, min, na.rm = TRUE)
  
  return(min_distances)
  
}