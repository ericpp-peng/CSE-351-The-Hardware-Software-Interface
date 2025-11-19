#!/usr/bin/env python3
"""
All in one test script runner for Lab 4. 

By Samantha Dreussi <sdreussi@cs.washington.edu>
"""

import subprocess
import re


'''
Part 1
'''
print("----- PART 1 - CACHE GEOMETRIES -----")

# filename => (block size, size, associativity)
CACHES = {
'cache_1c_1e_1k.o': (1, 1, 1),
'cache_16384c_4e_4k.o': (4 ,16384, 4),
'cache_32768c_8e_8k.o': (8, 32768, 8),
'cache_65536c_2e_16k.o': (16, 65536, 2),
'cache_1048576c_256e_256k.o': (256, 1048576, 256)
}

CACHE_PARAM_NAMES = ['Block Size', 'Cache Size', 'Associativity']
EXE_PARAM_NAMES = ['block_size', 'size', 'assoc']

total_cache_error = 0
for cache_name in CACHES.keys():
    cache_error = 0
    params = CACHES.get(cache_name)
    outputs = []
    cache_run = subprocess.run(['make', 'cache-test', f'TEST_CACHE=caches/{cache_name}']
                        ,capture_output=True, text=True)
    if cache_run.stderr:
        print("Compilation failure")
        cache_error = 1
        total_cache_error = 1
        continue
    
    # compilation passed
    for i, (exe_param, cache_param) in enumerate(zip(EXE_PARAM_NAMES, CACHE_PARAM_NAMES)):
        try:
            output = subprocess.check_output(['./cache-test', exe_param], timeout=60)
        except subprocess.TimeoutExpired:
                outputs.append(f'\trun timeout expired for ./cache-test {CACHE_PARAM_NAMES[i]}')
                cache_error = 1
                continue
        except subprocess.CalledProcessError:
                outputs.append(f'\tfailed to run ./cache-test {CACHE_PARAM_NAMES[i]}')
                cache_error = 1
                continue

        # checks for numbers in output
        nums_in_output = [int(num) for num in re.findall('-?\d+', output.decode())]
        if len(nums_in_output) != 1:
            outputs.append(f'\tcorrupted output for ./cache-test {CACHE_PARAM_NAMES[i]}')
            cache_error = 1

        output_num = nums_in_output[0]
        if params[i] != output_num:
            outputs.append(f'\tfailed {cache_param} - {output_num} != {params[i]}')
            cache_error = 1
    
    if cache_error:
        print(f"{cache_name}:")
        for i in outputs: print(i)
        total_cache_error = 1
    
if not cache_error: print("Given cache tests (non-mystery) passed")
else: print("\nErrors in cache-test-skel.c!!!!")

'''
Part 2
'''
print("\n----- PART 2 - MATRIX TRANSPOSE -----")

compilation_run = subprocess.run(["make"], capture_output=True, text=True)
compilation_err = compilation_run.stderr
if compilation_err:
    print(compilation_err)
else:
    grade_trans = subprocess.run(["python3", "grade_trans.py"], capture_output=True, text=True)
    grade_trans_score = re.findall(r"\d+.\d{2}", grade_trans.stdout)[-1]
    if float(grade_trans_score) != 10:
        print (grade_trans.stdout)
    else:
        print("Maxtrix transpose gets 10/10 points\n")

style_error = 0
style_run = subprocess.run(['./check_trans_vars', 'trans.c'], capture_output=True, text=True)
style_pass = re.search(r"transpose_submit passes the programming rules checks.", style_run.stdout)
if not style_pass:
     print(style_run.stderr)

print("----- RESULTS -----")
if not compilation_err and style_pass and float(grade_trans_score) == 10 and not total_cache_error:
    print("All checks passed! Ready to submit to Gradescope")
else:
    print("Not full points earned - see test results above for more details")
