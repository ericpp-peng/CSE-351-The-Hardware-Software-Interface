#!/usr/bin/env python3
"""
All in one test script runner for Lab 1b.

By Samantha Dreussi <sdreussi@cs.washington.edu>
"""

import subprocess
import re

compilation_run = subprocess.run(["make"], capture_output=True, text=True)
compilation_err = compilation_run.stderr
if compilation_err:
    print("----- COMPILATION RUN -----")
    print(compilation_err)
else:
    aisle = subprocess.run(["./aisle_test"], capture_output=True, text=True)
    aisle_corr = re.search("Total Score: 22/22", aisle.stdout)
    if not aisle_corr: 
        print("----- AISLE TEST -----")
        print(aisle.stdout)


    store = subprocess.run(["./store_test"], capture_output=True, text=True)
    store_corr = re.search("Total Score: 8/8", store.stdout)
    if not store_corr: 
        print("----- STORE TEST -----")
        print(store.stdout)



print("----- RESULTS -----")
if compilation_err: print("Compilation failed! See results above for compilation error details.")
else:
    if not aisle_corr: print("aisle_test failed! See results above for test details.")
    if not store_corr: print("store_test failed! See results above for test details.")

if (not (compilation_err or not aisle_corr or not store_corr)):
    print("All checks passed! Ready to submit to Gradescope")
