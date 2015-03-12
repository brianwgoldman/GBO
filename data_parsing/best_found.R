#!/usr/bin/Rscript
source("general.R")

data <- load_data("best_found.csv")

true <- read.csv("true_best.csv", header=T)
together <- merge(data, true)
aggregate(seed~problem, subset(together, best==1 & fitness!=optimal), FUN=unique)

# Scale fitness based on best found by anything
bests <- aggregate(fitness~problem+length+k+seed, data, FUN=max)
names(bests)[names(bests) == 'fitness'] <- "max_fitness"
data <- merge(data, bests)
data$fitness_scaled <- 1 - data$fitness / data$max_fitness

runs <- length(unique(data$seed))

# Create the fitness by radius plots
make_plot <- function(all_data, problem_name, key) {
  plt <- ggplot(data = subset(all_data, problem==problem_name),
                aes_string(x = "radius", y=key, color="solver", shape="solver"))
  plt <- plt + geom_line(stat="summary", fun.y=median_NA_high(runs), show_guide=FALSE)
  plt <- plt + geom_point(stat="summary", fun.y=median_NA_high(runs), size=3)
  plt <- plt + opt_color + opt_shape
  plt <- plt + clean
  return(plt)
}

nn <- make_plot(subset(data, k==4), "NearestNeighborNKQ", "fitness_scaled") + legend_top_right + ggtitle("Nearest Neighbor NKq")
un <- make_plot(subset(data, k==4), "UnrestrictedNKQ", "fitness_scaled") + legend_top_right + ggtitle("Unrestricted NKq")
is <- make_plot(subset(data, k==1), "IsingSpinGlass", "fitness_scaled") + legend_top_right + ggtitle("2D Ising Spin Glass")

result <- arrangeGrob(arrangeGrob(nn + no_axis), arrangeGrob(un + no_axis), arrangeGrob(is + no_axis),
            sub=paste("Radius", "\n", sep=""), left=paste("\n", "Error", sep=""), ncol=3)

ggsave("fitness.eps", result, width=12, height=5)

# Set up NA for runs that didn't make it to the best found
data$bsec <- data$seconds
data$bsec[data$best==0] <- NA


make_plot <- function(all_data, problem_name, key) {
  plt <- ggplot(data = subset(all_data, problem==problem_name & solver=="Pyramid"),
                aes_string(x = "radius", y=key, color="factor(k)", shape="factor(k)"))
  plt <- plt + geom_line(stat="summary", fun.y=median_NA_high(runs), show_guide=FALSE)
  plt <- plt + geom_point(stat="summary", fun.y=median_NA_high(runs), size=3)
  plt <- plt + scale_color_manual(name="K", values=six_split)
  plt <- plt + scale_shape_manual(name="K", values=c(16, 17, 15, 7, 3, 8))
  plt <- plt + clean# + ggtitle(problem_name)
  return(plt)
}
ggsave("p3-seconds.eps", make_plot(data, "NearestNeighborNKQ", "bsec") + log_y + ylab("Seconds") + xlab("Radius") + theme(legend.justification=c(1,0), legend.position=c(1,.2)), width=5, height=5)

