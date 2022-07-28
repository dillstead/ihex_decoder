#!/usr/bin/python

import subprocess
import filecmp
import glob
import sys
import os

if len(sys.argv) != 2:
    print("Usage: test.py <path to ihex_decoder>")
    sys.exit(1)
    
for test_file in glob.glob('*.hex'):
    with open(test_file, 'r') as input, open('./test.out', "w+") as output:
        res = subprocess.call(['../ihex_decoder', test_file], stdout=output, stderr=subprocess.STDOUT)
        if test_file.startswith('test') and res == 0 or test_file.startswith('error') and res != 0:
            if filecmp.cmp('./test.out', os.path.splitext(test_file)[0] + '.res'):
                print(test_file + " PASSED")
            else:
                print(test_file + " FAILED")
        else:
            print('Unexpected failure for ' + test_file)
        
        
