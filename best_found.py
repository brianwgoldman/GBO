import sys
from glob import glob
from os import path
from collections import defaultdict

def last_after_limit(filename, max_seconds):
  with open(filename, "r") as f:
    data = f.read().strip().split('\n')
  for line in reversed(data):
    fitness, seconds = line.split()
    if float(seconds) <= max_seconds:
      return fitness, seconds

max_minutes = 180
max_seconds = max_minutes * 60

headers = ["problem", "length", "k", "solver", "radius", "seed"]
headers += ["fitness", "seconds", "best"]
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
      fitness, _ = last_after_limit(filename, max_seconds)
      fitness = int(fitness)
      best[seed] = max(best[seed], fitness)
    except (IOError, ValueError):
      print >> sys.stderr, "ERROR", filename
      continue
  
   
  for filename in glob(path.join(folder, "*.dat")):
    fill['solver'], fill['radius'], fill['seed'] = path.basename(filename[:-4]).split('_')
    try:
      fill['fitness'], fill['seconds'] = last_after_limit(filename, max_seconds)
    except IOError:
      print >> sys.stderr, "ERROR", filename
      continue
    try:
      fill['best'] = 0
      if int(fill['fitness']) == best[fill['seed']]:
        fill['best'] = 1
      print formatter%fill
    except ValueError:
      print >> sys.stderr, "ERROR", filename
      continue
    
