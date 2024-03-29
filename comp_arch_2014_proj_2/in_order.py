#!/usr/bin/env python

import numpy

def load_operands(instruction, register_file):
	src1, src2 = instruction[2:4]
	if src1[0] == "r":
		res1 = register_file[src1]
	else:
		res1 = numpy.int32(int(src1))
	if src2[0] == "r":
		res2 = register_file[src2]
	else:
		res2 = numpy.int32(int(src2))
	return instruction[0:2] + [res1, res2]

def parse_instruction(line):
	instruction = line.split(" ")
	if instruction[0] == "add":
		instruction[0] = lambda x, y: (x + y)
	elif instruction[0] == "mul":
		instruction[0] = lambda x, y: (x * y)
	return instruction

def execute(instruction, register_file):
	op = instruction[0]
	dst = instruction[1]
	src1, src2 = instruction[2:4]
	register_file[dst] = op(src1, src2)

def print_register_file(register_file):
	for i in xrange(0, 28+1, 4):
		output = ""
		for j in xrange(i, i+4):
			output += "reg " + str(j) + ": "
			output += str(register_file["r" + str(j)]) + "\t\t"
		print output
	print "\n"

def main():
	verbose = False
	register_file = {}
	for i in xrange(32):
		register_file["r" + str(i)] = numpy.int32(0)
	print "Initial condition"
	print_register_file(register_file)
	cycle = 0
	while True:
		try:
			line = raw_input()
			if not line:
				break
		except Exception:
			break
		if verbose:
			print "Instruction:", line
		instruction = parse_instruction(line)
		if verbose:
			print instruction
		instruction = load_operands(instruction, register_file)
		if verbose:
			print instruction
		execute(instruction, register_file)
		if verbose:
			print "After cycle {}:".format(cycle)
			print_register_file(register_file)
			print "-"*80
		cycle += 1

	print "After cycle {}:".format(cycle)
	print_register_file(register_file)
	print "-"*80

if __name__ == "__main__":
	main()
