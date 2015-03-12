#!/usr/bin/Rscript
source("general.R")

data <- load_data("over.csv")

runs <- length(unique(data$seed))

make_plot <- function(sub_data, group) {
  plt <- ggplot(data = sub_data,
                aes_string("seconds", "fitness_scaled", color=group, shape=group))
  plt <- plt + geom_line(stat="summary", fun.y=median_NA_high(runs), show_guide=FALSE)
  plt <- plt + geom_point(stat="summary", fun.y=median_NA_high(runs), size=3)
  plt <- plt + clean + log_x + log_y
  return(plt)
}

# Scale fitness to ratio of best found
bests <- aggregate(fitness~problem+length+k+seed, data, FUN=max)
names(bests)[names(bests) == 'fitness'] <- "max_fitness"
data <- merge(data, bests)
data$fitness_scaled <- 1 - data$fitness / data$max_fitness

# Use the best radius for each method
nn_rad_choice <- c(5, 5, 1, 1) # Same values work for ising spin glass
names(nn_rad_choice) <- levels(data$solver)

un_rad_choice <- c(3, 2, 2, 1)
names(un_rad_choice) <- levels(data$solver)


nn <- make_plot(subset(data, problem=="NearestNeighborNKQ" & k==4 & solver=="Pyramid"), "factor(radius)") +
      legend_bot_left + ggtitle("Nearest Neighbor NKq") + rad_color + rad_shape
un <- make_plot(subset(data, problem=="UnrestrictedNKQ" & k==4 & solver=="Pyramid"), "factor(radius)") +
      legend_bot_left + ggtitle("Unrestricted NKq") + rad_color + rad_shape
is <- make_plot(subset(data, problem=="IsingSpinGlass" & k==1 & solver=="Pyramid"), "factor(radius)") +
      legend_bot_left + ggtitle("2D Ising Spin Glass") + rad_color + rad_shape

result <- arrangeGrob(arrangeGrob(nn + no_axis), arrangeGrob(un + no_axis), arrangeGrob(is + no_axis),
            sub=paste("Seconds", "\n", sep=""), left=paste("\n", "Error", sep=""), ncol=3)

ggsave("radius-over.eps", result, width=12, height=5)

nn <- make_plot(subset(data, problem=="NearestNeighborNKQ" & k==4 & radius==nn_rad_choice[solver]), "solver") +
      legend_bot_left + ggtitle("Nearest Neighbor NKq") + opt_color + opt_shape
un <- make_plot(subset(data, problem=="UnrestrictedNKQ" & k==4 & radius==un_rad_choice[solver]), "solver") +
      legend_bot_left + ggtitle("Unrestricted NKq") + opt_color + opt_shape
is <- make_plot(subset(data, problem=="IsingSpinGlass" & k==1 & radius==nn_rad_choice[solver]), "solver") +
      legend_bot_left + ggtitle("2D Ising Spin Glass") + opt_color + opt_shape

result <- arrangeGrob(arrangeGrob(nn + no_axis), arrangeGrob(un + no_axis), arrangeGrob(is + no_axis),
            sub=paste("Seconds", "\n", sep=""), left=paste("\n", "Error", sep=""), ncol=3)

ggsave("solver-over.eps", result, width=12, height=5)

