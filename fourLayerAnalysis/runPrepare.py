import os

def submitOneJob(lower_i, upper_i):
    
    f = open('run.sub', 'w')
    submitLines = """

    Universe = vanilla
    +IsLocalJob = true
    Rank = TARGET.IsLocalSlot
    request_disk = 2MB
    request_memory = 2048MB
    request_cpus = 1
    executable              = abcd4layer_wrapper.sh
    arguments               = {1} {2}
    log                     = /data/users/abcd4layer_logs/log_{1}-{2}_$(PROCESS).log
    output                  = /data/users/abcd4layer_logs/out_{1}-{2}_$(PROCESS).txt
    error                   = /data/users/abcd4layer_logs/error_{1}-{2}_$(PROCESS).txt
    should_transfer_files   = Yes
    when_to_transfer_output = ON_EXIT
    transfer_input_files = {0}abcd4layer_wrapper.sh, {0}abcd4layer.cpp, {0}treesV19Template.h, {0}utilities.h, {0}corrections.h
    getenv = true
    queue 1

    """.format(#FILE, lower_i, upper_i)

    f.write(submitLines)
    f.close()
    os.system('condor_submit run.sub')

if __name__=="__main__":

    batches = 40
    entries = 4000000
    events_per_batch = int(1.0*entries/batches)
    
    for i in range(batches):
        print("Submitting events ",i*events_per_batch,"-",(i+1)*events_per_batch)
        submitOneJob(i*events_per_batch,(i+1)*events_per_batch)