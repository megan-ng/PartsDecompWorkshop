# Description: Plots node graph where each node is coloured by its distance to the nearest core
plotColouredDistanceNodes <- function(tidy_graph, node_values, palette, plot_title, colourbar_title, layout_name, vertex_size = 2) {

  # Compute min and max values for colour bar
  min_val <- min(node_values, na.rm = TRUE)
  max_val <- max(node_values, na.rm = TRUE)
  
  # This variable is used to adjust the positioning of the colour bar tick values
  buffer <- (max_val - min_val) * 0.02
  
  # Set random seed for layout reproducibility 
  set.seed(123) 
  
  # Plot
  p <- ggraph(tidy_graph, layout = layout_name) + theme_void() +
    
    geom_edge_link(alpha = 0.3, edge_colour = "grey66") +
    
    geom_node_point(aes(color = node_values), size = vertex_size) +
    
    scale_color_viridis_c(
      name = colourbar_title,
      option = palette, # direction = -1,     # you can pick "magma", "plasma", "cividis" too
      breaks = c(min_val + buffer, max_val - buffer),
      labels = c(min_val, max_val),
      guide = guide_colorbar(
        barheight = unit(8, "cm"),  # adjust this value as needed
        ticks.colour = NA
      )
    ) + 
    
    ggtitle(paste0(sample_name, ": ", plot_title)) +
    theme(
      legend.position = "right",
      plot.title = element_text(hjust = 0.5),
      plot.margin = margin(t = 20, r = 20, b = 20, l = 20),
      plot.background = element_rect(fill = "white", colour = NA)
      
    )
  
  return(p)
  
}

# Description: Plots node graph where each node is coloured by its out-degree value
plotColouredDegreeNodes <- function(tidy_graph, plot_title, colourbar_title, layout_name, small_node_size) {
  
  # Get levels from tidy_graph
  value_levels <- levels(tidy_graph %>% activate(nodes) %>% pull(value_group))
  
  # Build palette of correct length
  my_random_colors <- polychrome(length(value_levels) + 1)
  my_random_colors <- my_random_colors[-2]  # remove the second colour
  names(my_random_colors) <- value_levels  # <-- this is key
  
  
  # Set random seed for layout reproducibility 
  set.seed(123) 
  
  p <- ggraph(tidy_graph, layout = layout_name) + theme_void() +
    
    geom_edge_link(alpha = 0.3, edge_colour = "grey66") +
    
    geom_node_point(aes(color = value_group, size = is_max)) +
    
    scale_color_viridis_d(
      name = colourbar_title,
    ) +
    
    geom_node_text(aes(label = ifelse(is_max, out_degree, "")),
                   color = "black", size = 2) +
    
    scale_size_manual(values = c(`FALSE` = small_node_size, `TRUE` = 5)) +
    
    guides(size = "none") +
    
    ggtitle(paste0(sample_name, ": ", plot_title)) +
    theme(
      legend.position = "right",
      plot.title = element_text(hjust = 0.5),
      plot.margin = margin(t = 20, r = 20, b = 20, l = 20),
      plot.background = element_rect(fill = "white", colour = NA)
    )
  
  return(p)
  
}

thresholdOutDegreeGraph <- function(tidy_graph, quantile_percent) {
  
  # Obtain feature values
  node_feature_name <- "out_degree"
  node_values <- tidy_graph %>% activate(nodes) %>% pull(!!sym(node_feature_name))
  
  # Calculate threshold
  threshold <- quantile(node_values, quantile_percent, na.rm = TRUE)
  threshold_label_value <- ceiling(threshold)
  threshold_label <- paste0("\u2265 ", threshold_label_value)
  
  # Make sure to capture all unique node_values, then split into groups:
  all_vals <- unique(node_values)
  
  # Values strictly less than threshold_label_value
  below_vals <- sort(all_vals[all_vals < threshold_label_value])
  
  # Values greater or equal to threshold_label_value will be collapsed
  # Set factor levels accordingly, making sure to include all below_vals + threshold_label
  levels_vec <- c(as.character(below_vals), threshold_label)
  
  tidy_graph <- tidy_graph %>%
    activate(nodes) %>%
    mutate(
      value_group = ifelse(node_values >= threshold_label_value,
                           threshold_label,
                           as.character(node_values)),
      
      value_group = factor(value_group, levels = levels_vec),
      capped_node_values = pmin(node_values, threshold),
      is_max = capped_node_values == max(capped_node_values, na.rm = TRUE)
    )
  
  return(tidy_graph)
}



