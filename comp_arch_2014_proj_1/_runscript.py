#!/usr/bin/env python

import subprocess
import os
import multiprocessing

def read_zcat_data(filename):
	res = []
	process = subprocess.Popen(["zcat", filename], stdout=subprocess.PIPE)
	while True:
		entry = process.stdout.read()
		if entry == "":
			break
		res.append(entry)
	return res

def run_predictor(data):
	res = []
	process = subprocess.Popen(["_release/predictor"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	for entry in data:
		process.stdin.write(entry)
	process.stdin.flush()
	process.stdin.close()
	while True:
		entry = process.stdout.read()
		if entry == "":
			break
		res.append(entry)
	return res

def process_file(filename):
	traces_path = "traces"
	f = os.path.join(traces_path, filename)
	print "Reading:", f
	data = read_zcat_data(f)
	print "Reading done, running the predictor"
	stats = run_predictor(data)
	print "Predicting done, results:"
	print "\n".join(stats)
	branches = int(stats[0].split("\n")[1].split(" ")[2])
	print "Branches processed:", branches
	cur_rate = float(stats[0].split("\n")[7].split(" ")[3])
	return branches, cur_rate

def main():
	traces_path = "traces"
	total_branches = 0
	total_rate = 0.0
	pool = multiprocessing.Pool(5)
	results = pool.map(process_file, sorted(os.listdir(traces_path)))
	print "Results:", results
	total_branches = sum([x[0] for x in results])
	total_rate = sum([float(x[0])*x[1] for x in results])	
	print "Processing done!"
	print "Total branches:", total_branches
	print "Average misprediction rate:", total_rate/total_branches

if __name__ == "__main__":
	main()
