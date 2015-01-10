import sys
from glob import glob
from os import path
from collections import defaultdict

for folder in sys.stdin:
  folder = folder.strip()
    
  for filename in glob(path.join(folder, "*.dat")):
    try:
      with open(filename, "r") as f:
        data = f.read().strip().split('\n')
    except IOError:
      print filename
      continue
    try:
      split = [line.split() for line in data[1:]]
      split = [(int(fit), float(sec)) for fit, sec in split]
      for i in range(1, len(split)):
        if split[i][0] <= split[i-1][0] or split[i][1] < split[i-1][1]:
          raise ValueError
    except ValueError:
      print filename
      continue
    
