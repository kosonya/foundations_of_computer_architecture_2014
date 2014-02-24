#!/usr/bin/env python

import subprocess
import os

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
	process = subprocess.Popen(["release/predictor"], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
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

def main():
	traces_path = "traces"
	total_branches = 0
	total_rate = 0.0
	for filename in sorted(os.listdir(traces_path)):
	#for filename in ["COMP-1.gz", "COMP-2.gz"]:
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
		print "Current misprediction rate:", cur_rate
		total_branches += branches
		print "Total branches:", total_branches
		total_rate += branches*cur_rate
		print "Average misprediction rate:", total_rate/total_branches
	print "Processing done!"
	print "Total branches:", total_branches
	print "Average misprediction rate:", total_rate/total_branches

if __name__ == "__main__":
	main()
