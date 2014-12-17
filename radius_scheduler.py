import sys
import json
from os import path, remove, makedirs
from subprocess import call
import signal
import time

begin_time = time.time()

runs = 100
job_minutes = 239
run_minutes = 120
folder = "radius"
jobname = sys.argv[1]


def try_run_fill(base_string, fill):
  if path.exists(fill['filename'] + ".dat") or path.exists(fill['filename'] + ".start"):
    return False
  with open(fill['filename'] + ".start", 'w') as f:
    f.write(jobname + '\n')
    f.write(base_string % fill + '\n')
  try:
    print base_string % fill
    call((base_string % fill).split())
  except Exception as e:
    print e
    sys.exit()
  finally:
    try:
      remove(fill['filename'] + ".start")
    except OSError as e:
      print e
      sys.exit()
  return True


base_string = "Release/SFX config/default.cfg -runs 1 -minutes %(minutes)i"
base_string += " -optimizer %(solver)s -problem %(problem)s -length %(length)i -k %(k)i -radius %(radius)i"
base_string += " -dat_file %(filename)s.dat -problem_seed %(pseed)i -seed %(seed)i"

def signal_handler(signal, frame):
        raise Exception('Signal received, exiting')
signal.signal(signal.SIGINT, signal_handler)

fill = {"minutes": run_minutes}
for problem, length in [("NearestNeighborNK", 10000)]:
  fill["problem"] = problem
  fill["length"] = length
  problem_folder = path.join(folder, "%(problem)s_%(length)0.5i_%(k)0.2i"%fill)
  try:
    makedirs(problem_folder)
  except OSError:
    pass
  for run in range(runs):
    fill['seed'] = run + 1
    fill['pseed'] = run
    for k in [2, 5, 10]:
      fill['k'] = k
      for radius in [1, 2, 3]:
        fill["radius"] = radius
      
        for solver in ["Pyramid", "SAC", "RandomRestartHC"]:
          fill['solver'] = solver
          fill['filename'] = path.join(problem_folder, "%(solver)s_%(radius)0.2i_%(pseed)0.5i" % (fill))
          elapsed_minutes = (time.time() - begin_time) / 60
          if elapsed_minutes + run_minutes >= job_minutes:
            print "No time for new job, already ran for", elapsed_minutes, "minutes"
            sys.exit()
          try_run_fill(base_string, fill)

open(path.join(folder, "finished.txt"), 'w').close()

