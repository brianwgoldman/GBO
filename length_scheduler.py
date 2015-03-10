import sys
import json
from os import path, remove, makedirs
from subprocess import call
import signal
import time

begin_time = time.time()

runs = 50
job_minutes = 230
run_minutes = 180
folder = "length"
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
  print base_string % fill
  call((base_string % fill).split())
  if path.exists(fill['filename'] + ".dat"):
    remove(fill['filename'] + ".start")
  return True


base_string = "Release/SFX config/default.cfg -minutes %(minutes)i"
base_string += " -optimizer %(solver)s -problem %(problem)s -length %(length)i -k %(k)i -radius %(radius)i"
base_string += " -dat_file %(filename)s.dat -problem_seed %(pseed)i -seed %(seed)i"

def signal_handler(signal, frame):
        raise Exception('Signal received, exiting')
signal.signal(signal.SIGINT, signal_handler)

tuning = {"NearestNeighborNKQ": {"Pyramid": 1, "TUX": 5, "HammingBallHC": 5, "BlackBoxP3": 1},
          "UnrestrictedNKQ": {"Pyramid": 2, "TUX": 2, "HammingBallHC": 3, "BlackBoxP3": 1},
          "IsingSpinGlass": {"Pyramid": 1, "TUX": 6, "HammingBallHC": 6, "BlackBoxP3": 1}}

fill = {"minutes": run_minutes}
for fill['pseed'] in range(runs):
  fill['seed'] = fill['pseed'] + 1
  for fill['problem'], fill['k'] in [("IsingSpinGlass", 1)]:
    #for fill['length'] in [200, 400, 600, 800, 1000, 2000, 4000, 6000, 8000, 10000]:
    for fill['length'] in [196, 400, 625, 784, 1024, 1296, 1600, 2025, 2916, 4096, 6084]:
      problem_folder = path.join(folder, "%(problem)s_%(length)0.5i_%(k)0.2i"%fill)
      try:
        makedirs(problem_folder)
      except OSError:
        pass
      for fill['solver'] in ["Pyramid", "TUX", "HammingBallHC", "BlackBoxP3"]:
        if fill['problem'] == "NearestNeighborNKQ":
          if fill['solver'] == "BlackBoxP3" and fill['length'] > 1000:
            continue
          if fill['solver'] in ['TUX', 'HammingBallHC'] and fill['length'] > 8000:
            continue
        if fill['problem'] == "UnrestrictedNKQ":
          if fill['solver'] == "BlackBoxP3" and fill['length'] > 1000:
            continue
        if fill['problem'] == "IsingSpinGlass":
          if fill['solver'] == "BlackBoxP3" and fill['length'] >= 2916:
            continue
        fill['radius'] = tuning[fill['problem']][fill['solver']]
        fill['filename'] = path.join(problem_folder, "%(solver)s_%(radius)0.2i_%(pseed)0.5i" % (fill))
        elapsed_minutes = (time.time() - begin_time) / 60
        if elapsed_minutes + run_minutes >= job_minutes:
          print "No time for new job, already ran for", elapsed_minutes, "minutes"
          sys.exit()
        try_run_fill(base_string, fill)

open(path.join(folder, "finished.txt"), 'w').close()
