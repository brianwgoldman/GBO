#!/usr/bin/Rscript
source("general.R")

data <- load_data("scaling_length.csv")

data$bsec <- data$seconds
data$bsec[data$best==0] <- NA

bests <- aggregate(fitness~problem+length+k+seed, data, FUN=max)
names(bests)[names(bests) == 'fitness'] <- "max_fitness"
data <- merge(data, bests)
data$fitness_scaled <- 1 - data$fitness / data$max_fitness

true <- read.csv("../radius/true_best.csv", header=T)
together <- merge(data, true)
aggregate(seed~problem+length, subset(together, best==1 & fitness!=optimal), FUN=function(x) length(unique(x)))

runs <- length(unique(data$seed))

# Look at the ratio of qualites at specific points
biggest_nk <- subset(data, problem=="NearestNeighborNKQ" & length==1000)
biggest_is <- subset(data, problem=="IsingSpinGlass" & length==2025)
biggest_un <- subset(data, problem=="UnrestrictedNKQ" & length==10000)
aggregate(seconds~solver+problem+length, biggest_nk, FUN=mean)
aggregate(seconds~solver+problem+length, biggest_is, FUN=mean)
subset(aggregate(best~solver+problem+length, subset(data, problem=="UnrestrictedNKQ"), FUN=mean), best > 0)

# Mann-Whitney-U tests are unpaired wilcox rank sum tests
wilcox.test(subset(biggest_nk, best==1 & solver=="Pyramid")$seconds, subset(biggest_nk, best==1 & solver=="BlackBoxP3")$seconds)
wilcox.test(subset(biggest_is, best==1 & solver=="Pyramid")$seconds, subset(biggest_is, best==1 & solver=="BlackBoxP3")$seconds)

wilcox.test(subset(biggest_un, solver=="Pyramid")$fitness, subset(biggest_un, solver=="TUX")$fitness)

BigO <- function(all_data, problem_key, solver_key) {
  return(lm(log(seconds)~log(length), subset(all_data, problem==problem_key & solver==solver_key & best==1)))
}

# Fit aN^B on Nearest Neighbor
BigO(data, "NearestNeighborNKQ", "Pyramid")
BigO(data, "NearestNeighborNKQ", "BlackBoxP3")

# Fit aN^B on Ising
BigO(subset(data, length <= 4096), "IsingSpinGlass", "Pyramid")
BigO(subset(data, length <= 4096), "IsingSpinGlass", "BlackBoxP3")


make_plot <- function(all_data, problem_name, key) {
  plt <- ggplot(data = subset(all_data, problem==problem_name),
                aes_string(x = "length", y=key, color="solver", shape="solver"))
  plt <- plt + geom_line(stat="summary", fun.y=median_NA_high(runs), show_guide=FALSE)
  plt <- plt + geom_point(stat="summary", fun.y=median_NA_high(runs), size=3)
  plt <- plt + opt_color + opt_shape
  plt <- plt + clean + xlab("Length")
  return(plt)
}
plt <- make_plot(data, "NearestNeighborNKQ", "bsec") + ylab("Seconds") + log_x + log_y + legend_bot_right
plt <- plt + geom_segment(aes(x=1000, y=3, xend=1000, yend=1100), color="black") + annotate("text", x=1000, y=200, label="-375x Speedup", hjust=0)
ggsave("length-nn.eps", plt, width=5, height=5)


plt <- make_plot(data, "IsingSpinGlass", "bsec") + ylab("Seconds") + log_x + log_y + legend_bot_right
plt <- plt + geom_segment(aes(x=2025, y=220, xend=2025, yend=1500), color="black") + annotate("text", x=2025, y=600, label="-4.6x Speedup", hjust=0)
ggsave("length-is.eps", plt, width=5, height=5)

ggsave("length-un.eps", make_plot(data, "UnrestrictedNKQ", "fitness_scaled") + log_x + ylab("Error") + theme(legend.justification=c(1,0), legend.position=c(1,.2)), width=5, height=5)

