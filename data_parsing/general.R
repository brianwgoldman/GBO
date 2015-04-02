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
six_split <- c("#8c510a", "#bf812d", "#dfc27d", "#80cdc1", "#35978f", "#01665e")

no_axis <- theme(axis.title.x = element_blank(), axis.title.y = element_blank()) 

solver_labels <- c("HBHC", "TUX", "Gray-Box P3", "Black-Box P3")
opt_color <- scale_color_manual(name="Optimizer", values=four_color, labels=solver_labels)
opt_shape <- scale_shape_manual(name="Optimizer", values=c(16, 17, 15, 7, 3, 8), labels=solver_labels)

rad_color <- scale_color_manual(name="Radius", values=six_split)
rad_shape <- scale_shape_manual(name="Radius", values=c(16, 17, 15, 7, 3, 8))

pad_call <- function(fun, padding, value) {
  inner <- function(data) {
    new_data <- c(data, rep(value, padding - length(data)))
    result <- fun(new_data)
    result[result==value] <- NA
    return(result)
  }
  return(inner)
}

median.quartile <- function(x){
  out <- quantile(x, probs = c(0.25,0.5,0.75))
  names(out) <- c("ymin","y","ymax")
  return(out)
}
