# LOAD IN LIBRARIES  -----------------------------------------------------------
library(tidygraph)
library(ggraph)
library(igraph)
library(RColorBrewer)
library(scales)
library(dplyr)
library(pals)
library(svglite)

# LOAD IN FUNCTIONS ------------------------------------------------------------
function_dir <- "C:/Users/mcn24/Desktop/R/PartsDecomp/ColouredNodeGraphs/Functions/"      #CHANGE
function_files <- list.files(function_dir, pattern = "\\.R$", full.names = TRUE)
invisible(lapply(function_files, source))

# DECLARE GLOBAL VARIABLES -----------------------------------------------------
input_dir <- "C:/Users/mcn24/Desktop/R/PartsDecomp/Data/Workshop/"                        #CHANGE
output_dir <- "C:/Users/mcn24/Desktop/R/PartsDecomp/ColouredNodeGraphs/Figures/Workshop/" #CHANGE
sample_names <- c("Astro1_groundTruth", "Astro2_groundTruth")                             #CHANGE

layout_names <- c("lgl", "tree")
node_properties <- c("Distance from Nearest Core", "Out-Degree")
colour_bar_name <- c("Distance", "Out-Degree")

# LOAD IN DATA AND CREATE GRAPH OBJECTS ----------------------------------------
# Initialize empty list to populate with graph objects
all_graphs <- list()

# For each sample --> create graph and load in core node data
for (i in 1:length(sample_names)) {

  # Declare current sample and its paths
  sample_name <- sample_names[i]
  graph_file_path <- paste0(input_dir, sample_name, "_graph.csv")
  core_file_path <- paste0(input_dir, sample_name, "_coreIDs.csv")

  # Create tidy graph (future plotting functions to come require the tidy graph format)
  # Note the nodes in this graph object contain fields "out_degree" and "min_dist" (to core)
  tidy_graph <- createTidyGraph(graph_file_path, core_file_path)

  # Store graph in graph list
  all_graphs[[i]] <- tidy_graph
}

# PLOT CORE DISTANCE GRAPHS WITH COLOURED NODES --------------------------------
node_feature_name <- "min_dist"
for (i in 1:length(sample_names)) {
  for (j in 1:length(layout_names)) {
    # Declare current graph object
    tidy_graph <- all_graphs[[i]]
    sample_name <- sample_names[i]
    
    # Create out degree graph
    # Obtain feature values
    node_values <- tidy_graph %>% activate(nodes) %>% pull(!!sym(node_feature_name))
    
    # Plotting params
    palette <- "magma"
    plot_title <- "Distance from Core"
    colourbar_title <- "Distance"
    layout_name <- layout_names[j]
    
    distance_plot <- plotColouredDistanceNodes(tidy_graph, node_values, palette, plot_title, colourbar_title, layout_name, vertex_size = 2)
    
    ggsave(
      filename = paste0(output_dir, sample_name, "_", node_feature_name, "_", layout_names[j],".svg"),
      plot = distance_plot,
      width = 8,      # inches
      height = 8,     # inches
      dpi = 300
    )
  }
}


# PLOT OUT DEGREE GRAPHS WITH COLOURED NODES ----------------------------------
node_feature_name <- "out_degree"
for (i in 1:length(sample_names)) {
  for (j in 1:length(layout_names)) {
    # Declare current graph object
    tidy_graph <- all_graphs[[i]]
    sample_name <- sample_names[i]
    
    print(paste("Plotting", sample_name))
    
    # Calculate threshold (e.g., 99.5th percentile)
    quantile_percent <- 0.995
    tidy_graph <- thresholdOutDegreeGraph(tidy_graph, quantile_percent)
    
    # Plotting params
    plot_title <- "Out-Degree"
    colourbar_title <- "Out-Degree"
    layout_name <- layout_names[j]
    small_node_size <- ifelse(vcount(tidy_graph) > 1000, 1, 2)
    
    # Plot
    out_degree_plot <- plotColouredDegreeNodes(tidy_graph, plot_title, colourbar_title, layout_name, small_node_size)
    
    print(out_degree_plot)
    
    ggsave(
      filename = paste0(output_dir, sample_name, "_", node_feature_name, "_", layout_names[j], ".svg"),
      plot = out_degree_plot,
      width = 8,      # inches
      height = 8,     # inches
      dpi = 300
    )
  }
}
