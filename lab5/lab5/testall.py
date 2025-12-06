#!/usr/bin/env python3
"""
All in one test script runner for Lab 5.

By Samantha Dreussi <sdreussi@cs.washington.edu>
"""

import subprocess
import re

MAX_CORRECTNESS_POINTS = 36  # max possible correctness points

TRACES = [
    'amptjp-bal.rep',
    'cccp-bal.rep',
    'cp-decl-bal.rep',
    'expr-bal.rep',
    'coalescing-bal.rep',
    'random-bal.rep',
    'random2-bal.rep',
    'binary-bal.rep',
    'binary2-bal.rep'
]

MAX_PERFORMANCE_POINTS = 5  # max possible performance points
PERFORMANCE_THRESHOLD = 0.75  # threshold for full performance credit
TRACE_POINTS = int(MAX_CORRECTNESS_POINTS / len(TRACES))

def correctness_score(correct_traces, total_traces):
    """Calculate correctness score"""
    return (correct_traces / total_traces) * MAX_CORRECTNESS_POINTS;

def performance_score(performance_index):
    """Calculate performance score"""
    if performance_index == 0:
        return 0
    score = ((performance_index / 100.0) + (1 - PERFORMANCE_THRESHOLD)) * MAX_PERFORMANCE_POINTS
    score = min(score, MAX_PERFORMANCE_POINTS)  # cannot exceed max points
    return score // 1  # floor of score

def grade_allocator():
    total_performance, correct_traces = 0, 0
    error = 0
    
    for trace in TRACES:
        make_run = subprocess.run(['make', 'mdriver'], capture_output=True, text=True)
        trace_run = subprocess.run(['./mdriver', '-vg', '-f', f'traces/{trace}'], capture_output=True, text=True)
        
        if make_run.stderr or trace_run.stderr or trace_run.returncode:
            if make_run.stderr:
                print(make_run.stderr)
            elif trace_run.stderr: 
                print(trace_run.stderr)
            else:
                print(f"No output captured for test {trace}. Run the test seperately to see error output.")

            error = 1

            break
        tokens = trace_run.stdout.split('\n')

    
        trace_correct = int(re.match(r'correct:(\d+)', tokens[-3]).group(1))
        if not trace_correct:
            print('\n'.join(tokens[:-3]) + '\n')
            print('-------------------------------\n\n')
        correct_traces += int(re.match(r'correct:(\d+)', tokens[-3]).group(1))
        total_performance += int(re.match(r'perfidx:(\d+)', tokens[-2]).group(1))


    correctness = correctness_score(correct_traces, len(TRACES))
    performance = performance_score(total_performance / len(TRACES))
    if error:
        print('\n-------------------------------\n')
        print("Compilation/Running Failed! See errors above.")
    elif correctness != 36:
        print('\n-------------------------------\n')
        print("Incorrect trace results! See errors above.")
    elif performance != 5:
        print(f"Performance score is {performance} / 5.")
    else:
        print("Full points for correctness and performance. Remember to check for proper style and then submit to Gradescope!")


grade_allocator()
