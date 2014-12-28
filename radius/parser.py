import sys
from glob import glob
from os import path
from collections import defaultdict

headers = ["problem", "length", "k", "solver", "radius", "seed"]
headers += ["fitness", "seconds", "final", "best"]
formatter = ','.join(["%(" + header + ")s" for header in headers])
print ",".join(headers)

for folder in sys.stdin:
  folder = folder.strip()
  fill = {}
  try:
    fill['problem'], fill['length'], fill['k'] = path.basename(folder).split('_')
  except ValueError:
    continue
  best = defaultdict(int)
  for filename in glob(path.join(folder, "*.dat")):
    seed = path.basename(filename[:-4]).split('_')[-1]
    try:
      with open(filename, "r") as f:
        data = f.read().strip().split('\n')
      fitness = int(data[-1].split()[0])
      best[seed] = max(best[seed], fitness)
    except (IOError, ValueError):
      print >> sys.stderr, "ERROR", filename
      continue
  
   
  for filename in glob(path.join(folder, "*.dat")):
    fill['solver'], fill['radius'], fill['seed'] = path.basename(filename[:-4]).split('_')
    try:
      with open(filename, "r") as f:
        data = f.read().strip().split('\n')
    except IOError:
      print >> sys.stderr, "ERROR", filename
      continue
    try:
      fill['final'], fill['best'] = 0, 0
      '''
      for line in data[:-1]:
        fill['fitness'], fill['seconds'] = line.split()
        print formatter%fill
      #'''
      fill['final'] = 1
      fill['fitness'], fill['seconds'] = data[-1].split()
      if int(fill['fitness']) == best[fill['seed']]:
        fill['best'] = 1
      print formatter%fill
    except ValueError:
      print >> sys.stderr, "ERROR", filename
      continue
    
