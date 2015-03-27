from os import path
from subprocess import call
import time

def try_run(jobid, command, task_finished_file):
    '''
    Determine if a specific command should be performed by this process.
    
    jobid - The unique identifier of this job
    command - The execuable call + all of its arguments to perform
    task_finished_file - This file exists only if a run was successful
    '''
    lock_file = task_finished_file + ".lock"
    # Check if the task has already been done or if someone else is doing it.
    if path.exists(task_finished_file) or path.exists(lock_file):
        return False
    # Attempt to grab the lock on this task
    with open(lock_file, 'w') as f:
        f.write(jobid + '\n')
        f.write(command + '\n')
    time.sleep(10)
    # Check if you successfully grabbed the lock
    with open(lock_file, "r") as f:
        saved = f.read().strip().split()[0].strip()
        if saved != jobid:
            # Some other job wrote to the lock after you did.
            print "Double starts:", jobid, saved
            return False
    print command
    # Execute the command. This is a blocking call.
    call(command.split())
    # If the task was successful, remove the lock.
    # If the task failed, the lock must be removed manually. Prevents cascade failure. 
    if path.exists(task_finished_file):
        remove(lock_file)
    return True

if __name__ == "__main__":
    import sys
    jobname = sys.argv[1]
    # This is the command I want to call, using different arguments each time
    command_template = "Release/GBO config/default.cfg"
    command_template += " -problem %(problem)s -length %(length)i -k %(k)i -radius %(radius)i"
    command_template += " -problem_seed %(pseed)i -enum_file $(filename)s"
    # Where I am writing the output file.
    filename_template = "%(problem)s_%(length)0.5i_%(k)0.2i_%(radius)0.2i_%(pseed)0.5i.txt"
    
    # Iterate over each configuration I want to test
    configure = {}
    for configure['pseed'] in range(50):
        for configure['problem'] in ['NearestNeighborNKQ', "UnrestrictedNKQ"]:
            for configure['length'] in [40, 50, 60, 70, 80]:
                for configure['k'] in [1, 2, 3, 4]:
                    for configure['radius'] in [1, 2, 3]:
                        filename = filename_template % configure
                        configure['filename'] = filename 
                        command = command_template % configure
                        if try_run(jobname, command, filename):
                            print "Job Completed, existing"
                            sys.exit()
    print "No jobs left to run, stopping resubmissions"
    open("finished.txt", 'w').close()
