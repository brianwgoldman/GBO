#!/usr/bin/Rscript
library(ggplot2)
library(scales)
library(gridExtra)

load_data <- function(filename) {
  data <- read.csv(filename, header=TRUE)
  data <- subset(data, k != 6)
  data$solver <- factor(data$solver, levels = c("HammingBallHC", "TUX", "Pyramid", "BlackBoxP3"))
  return(data)
}

log_x <- scale_x_log10(breaks = trans_breaks("log10", function(x) 10^x),
                       labels = trans_format("log10", math_format(10^.x)))
log_y <- scale_y_log10(breaks = trans_breaks("log10", function(x) 10^x),
                       labels = trans_format("log10", math_format(10^.x)))

clean <- theme_bw() + theme(
    plot.background = element_blank()
   ,panel.grid.major = element_blank()
   ,panel.grid.minor = element_blank()
   ,panel.border = element_blank()
  ) + theme(axis.line = element_line(color = 'black'))

legend_bot_left <- theme(legend.justification=c(0,0), legend.position=c(0,0))
legend_bot_right <- theme(legend.justification=c(1,0), legend.position=c(1,0))
legend_top_left <- theme(legend.justification=c(0,1), legend.position=c(0,1))
legend_top_right <- theme(legend.justification=c(1,1), legend.position=c(1,1))


four_color = c("#b2df8a", "#33a02c", "#1f78b4", "#a6cee3")
five_split <- c("#d7191c", "#fdae61", "#000000", "#abd9e9", "#2c7bb6")
#six_split <- c("#8c510a", "#d8b365", "#f6e8c3", "#c7eae5", "#5ab4ac", "#01665e")
six_split <- c("#8c510a", "#bf812d", "#dfc27d", "#80cdc1", "#35978f", "#01665e")

no_axis <- theme(axis.title.x = element_blank(), axis.title.y = element_blank()) 

solver_labels <- c("HBHC", "TUX", "Gray-Box P3", "Black-Box P3")
opt_color <- scale_color_manual(name="Optimizer", values=four_color, labels=solver_labels)
opt_shape <- scale_shape_manual(name="Optimizer", values=c(16, 17, 15, 7, 3, 8), labels=solver_labels)

rad_color <- scale_color_manual(name="Radius", values=six_split)
rad_shape <- scale_shape_manual(name="Radius", values=c(16, 17, 15, 7, 3, 8))

median_NA_high <- function(padding) {  
  median_inner <- function(data) {
    worst <- max(data, na.rm=TRUE)
    data <- c(data, rep(worst+1, padding - length(data)))
    data[is.na(data)] <- worst+1
    result <- median(data)
    if(result > worst) {
      result <- NA
    }
    return(result)
  }
  return(median_inner)
}


################################### Currently Unused ###################

solver_labels <- c('Hill Climber', '1+(Lambda,Lambda)', 'hBOA', 'Parameter-less hBOA', 'LTGA', 'P3')
problem_labels <- c("Deceptive Trap", "Deceptive Step Trap", "HIFF", "Rastrigin", "Nearest Neighbor NK", "Ising Spin Glass", "MAX-SAT")
names(problem_labels) <- c("DeceptiveTrap", "DeceptiveStepTrap", "HIFF", "Rastrigin", "NearestNeighborNK", "IsingSpinGlass", "MAXSAT")

problem_shapes <- c(24, 25, 21, 22, 23, 3, 4)


seven_plot <- function(p1, p2, p3, p4, p5, p6, p7, xlabel, ylabel) {
  tmp <- ggplot_gtable(ggplot_build(p1))
  leg <- which(sapply(tmp$grobs, function(x) x$name) == "guide-box")
  group_legend <- tmp$grobs[[leg]]

  nl <- theme(legend.position="none", axis.title.x = element_blank(), axis.title.y = element_blank()) 
  result <- arrangeGrob(arrangeGrob(p1 + nl),
                       arrangeGrob(p2 + nl),
                       arrangeGrob(p3 + nl),
                       arrangeGrob(p4 + nl),
                       arrangeGrob(p5 + nl),
                       arrangeGrob(p6 + nl),
                       arrangeGrob(p7 + nl),
                       group_legend,
                       sub=paste(xlabel, "\n", sep=""), left=paste("\n", ylabel, sep=""), ncol=3)
  return(result)
}
