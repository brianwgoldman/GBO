import sys
import json
from os import path, remove, makedirs
from subprocess import call
import signal
import time

begin_time = time.time()

runs = 30
job_minutes = 230
run_minutes = 180
folder = "radius"
jobname = sys.argv[1]


def try_run_fill(base_string, fill):
  if path.exists(fill['filename'] + ".dat") or path.exists(fill['filename'] + ".start"):
    return False
  with open(fill['filename'] + ".start", 'w') as f:
    f.write(jobname + '\n')
    f.write(base_string % fill + '\n')
  time.sleep(10)
  with open(fill['filename'] + ".start", "r") as f:
    saved = f.read().strip().split()[0].strip()
    if saved != jobname:
      print "Double starts:", jobname, saved
      return False
  try:
    print base_string % fill
    call((base_string % fill).split())
  except Exception as e:
    print e
    sys.exit()
  finally:
    try:
      if path.exists(fill['filename'] + ".dat"):
        remove(fill['filename'] + ".start")
    except OSError as e:
      print e
      sys.exit()
  return True


base_string = "Release/SFX config/default.cfg -minutes %(minutes)i"
base_string += " -optimizer %(solver)s -problem %(problem)s -length %(length)i -k %(k)i -radius %(radius)i"
base_string += " -dat_file %(filename)s.dat -problem_seed %(pseed)i -seed %(seed)i"

def signal_handler(signal, frame):
        raise Exception('Signal received, exiting')
signal.signal(signal.SIGINT, signal_handler)

fill = {"minutes": run_minutes}
for fill['pseed'] in range(runs):
  fill['seed'] = fill['pseed'] + 1
  for fill['problem'], fill['length'] in [("NearestNeighborNKQ", 6000), ("UnrestrictedNKQ", 6000)]:
    for fill['k'] in [2, 3, 4, 5, 1]:
      problem_folder = path.join(folder, "%(problem)s_%(length)0.5i_%(k)0.2i"%fill)
      try:
        makedirs(problem_folder)
      except OSError:
        pass
      for fill['radius'] in [1, 2, 3, 4, 5, 6]:
        if fill['problem'] == "NearestNeighborNKQ":
          if fill['k'] > 3 and fill['radius'] > 5:
            continue
          if fill['k'] > 4 and fill['radius'] > 4:
            continue
        if fill['problem'] == "UnrestrictedNKQ":
          if fill['k'] > 1 and fill['radius'] > 4:
            continue
          if fill['k'] > 2 and fill['radius'] > 3:
            continue
          if fill['k'] > 4 and fill['radius'] > 2:
            continue
                                                      
        for fill['solver'] in ["Pyramid", "TUX", "HammingBallHC"]:
          fill['filename'] = path.join(problem_folder, "%(solver)s_%(radius)0.2i_%(pseed)0.5i" % (fill))
          elapsed_minutes = (time.time() - begin_time) / 60
          if elapsed_minutes + run_minutes >= job_minutes:
            print "No time for new job, already ran for", elapsed_minutes, "minutes"
            sys.exit()
          try_run_fill(base_string, fill)

open(path.join(folder, "finished.txt"), 'w').close()
