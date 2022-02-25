from ast import Mod
from cProfile import run
from distutils.log import debug
import os
import copy
import subprocess
from enum import Enum
import sys

LANE_FILE = "src/include/duckdb/function/scalar/sql_pgq_functions.hpp"
THREAD_FILE = "create_csr_vertex.benchmark"
SQL_FILE = "create_csr_vertex.sql"

TEST_FILE = "test_write.hpp"

DEFINE_STRING = "#define LANE_LIMIT "
DIVISOR_STRING = "#define VISIT_SIZE_DIVISOR "
THREAD_STRING = "PRAGMA threads="
SF_STRING = "data/ldbc-snb/social-network-sf"

CMD = "BUILD_BENCHMARK=1 BUILD_TPCH=1 make"

RUNNER = "build/release/benchmark/benchmark_runner"

BENCHMARK_DIR = "benchmark/micro/sql-pgq/"

OUTPUT_DIR = os.path.join(BENCHMARK_DIR, "results")
DEBUG_DIR =  os.path.join(BENCHMARK_DIR, "debug")

LOG_TIME_FILE = "timings-test.txt"

msbfs_file = "msbfs.benchmark"

FULLQUERY_FILE = os.path.join(BENCHMARK_DIR, "fullquery.benchmark")

Mode = Enum('Mode', 'THREAD LANE SF MSBFS TIMING')

debug = False

def replace_string(mode, in_file, lane, thread_count, sf_count, divisor, timing_file_name):
	write_lines = list()
	print(mode)
	# new_line = create_line(mode, lane_count, thread_count, vertex_count)
	for lines in in_file.readlines():
		if(mode == Mode.LANE):
			if DEFINE_STRING in lines:
				new_lane = DEFINE_STRING + str(lane) + "\n"
				write_lines.append(new_lane)
			else:
				write_lines.append(copy.deepcopy(lines))
		elif(mode == Mode.MSBFS):
			if DIVISOR_STRING in lines:
				new_divisor = DIVISOR_STRING + str(divisor) + "\n"
				write_lines.append(new_divisor)
			else:
				write_lines.append(copy.deepcopy(lines))
		elif(mode == Mode.THREAD):
			if THREAD_STRING in lines:
				# print("Found" + str(thread_count))
				new_thread = THREAD_STRING + str(thread_count) + ";\n"
				write_lines.append(new_thread)
			else:
				write_lines.append(copy.deepcopy(lines))
		elif(mode == Mode.SF):
			if SF_STRING in lines:
				# new_sql = SQL_STRING + str(vertex_count) + ") ORDER BY RANDOM();\n"
				prefix = lines.find("sf")
				suffix = lines.find("-projected")
				new_sql = lines[:prefix] + "sf" + str(sf_count) + lines[suffix:] 
				write_lines.append(new_sql)
			else:
				write_lines.append(copy.deepcopy(lines))
		elif(mode == Mode.TIMING):
			# if "sf" in lines:
			# 	sf_pos = lines.find("sf")
			# 	closing_paren_pos = lines.find(")")
			# 	new_line = lines[:sf_pos] + timing_file_name + "\'" + lines[closing_paren_pos:]
			# 	write_lines.append(new_line)
			if "debug" in lines:
				left_pos = lines.find("benchmark")
				closing_paren_pos = lines.find("out\'") + len("out\'") + 1
				print(timing_file_name)
				new_line = lines[:left_pos] + timing_file_name + "\')" + lines[closing_paren_pos:]
				write_lines.append(new_line)
			
			else:
				write_lines.append(copy.deepcopy(lines))


		else:
			Exception("Unknown Mode")
	return write_lines



	# return write_lines

def read_and_replace(mode, thread_file = '', sf_file = '', timing_file = '', timing_output_file = '', lane = 64, thread_count = 1, sf_count = 0.1, divisor = 2):
	if(mode == Mode.THREAD):
		input_file = thread_file
		output_file = thread_file
	elif(mode == Mode.LANE):
		input_file = LANE_FILE 
		output_file = LANE_FILE
	elif(mode == Mode.SF):
		input_file = sf_file
		output_file = sf_file
	elif(mode == Mode.MSBFS):
		input_file = LANE_FILE
		output_file = LANE_FILE
	elif(mode == Mode.TIMING):
		input_file = timing_file
		output_file = timing_file
	else:
		print("Unknown mode")
		exit(0)
	with open(input_file, 'r') as in_file:
		write_lines = replace_string(mode, in_file, lane, thread_count, sf_count, divisor, timing_output_file)
			
	# print(write_lines)
	with open(output_file, 'w') as out_file:
		out_file.writelines(write_lines)


num_errors = 0

def run_make():
	proc = subprocess.Popen(CMD, stderr=subprocess.PIPE, shell=True)
	global num_errors
	res = proc.wait()
	if res != 0:
		num_errors = num_errors + 1
		if ( num_errors > 2):
			sys.exit(-1)
		with open("error.log", "a") as error_file:
			text = proc.stderr.read().decode('utf8')
			error_file.write(text)
		print(text)


def run_benchmark( bc_file, output_dir, output_file):
	if not os.path.isdir(output_dir):
		os.makedirs(output_dir)
	
	out_arg = "--out=" + os.path.join(output_dir, output_file) 

	cmd = [RUNNER, bc_file, out_arg ]
	proc = subprocess.run(cmd)
	# res = proc.wait()
	# if res != 0:
	# 	text = proc.stderr.read().decode('utf8')
	# 	print(text)


# thread_values = [1, 2, 4, 8, 12, 16]
# thread_values = [2]
thread_values = [1, 2, 4]
vertex_count = 1000
variant = 10
nruns = 1
def thread_benchmark(sf):
	
	# thread_files = [ "create_csr_edge.benchmark"]
	# thread_files = ["create_csr_vertex.benchmark", "create_csr_edge.benchmark"]
	thread_files = [ msbfs_file]
	# thread_files = ["fullquery.benchmark"]
	global debug
	# f = open('timings-test.txt', 'a')
	# f.write("sf " + str(sf) + "########\n")
	# f.close()
	
	lane_count = 64
	edge_mapping = {"vertex":1000000, "edge":1000000, "msbfs":1000000, "fullquery":1000000}
	# msbfs.benchmark
	if debug:
		output_dir = os.path.join(OUTPUT_DIR, "thread_debug")
		output_dir = os.path.join(output_dir,  str(sf))
	else:
		output_dir = os.path.join(OUTPUT_DIR, "thread")
		output_dir = os.path.join(output_dir,  str(sf))
	debug_dir = os.path.join(DEBUG_DIR, "thread")
	debug_dir = os.path.join(debug_dir, str(sf))
	

	for file in thread_files:
		output_base = file.split(".")[0].split("_")[-1]
		debug_dir = os.path.join(debug_dir, output_base)
		if not os.path.isdir(debug_dir):
			os.makedirs(debug_dir)
		for thread_count in thread_values:
			# f.write("Thread values " + str(thread_count) + "\n")
			for run in range(nruns): 
				thread_file = os.path.join(BENCHMARK_DIR, file)
				read_and_replace(Mode.THREAD, thread_file=thread_file, thread_count=thread_count)
				if debug:
					debug_file = "sf" + str(sf) + "_" + "lane" + str(lane_count) + "_" + "thread" + str(thread_count) + "_variant" + str(variant) + "_run" + str(run) + ".out"
					debug_file = os.path.join(debug_dir, debug_file)
					print(debug_file)
					read_and_replace(Mode.TIMING, timing_file=thread_file, timing_output_file=debug_file)
				
				edge_count = edge_mapping[output_base]
				run_benchmark(thread_file, output_dir, output_base +  '_' + str(lane_count) + '_' +  str(vertex_count) + '_' + str(edge_count) + '_' + str(thread_count) + '_' + str(run) + ".out")
				# if output_file == "TIMEOUT":
				# 	continue
				# run_benchmark(thread_file, output_dir, output_base +  '_' + str(lane_count) + '_' +  str(vertex_count) + '_' + str(edge_count) + '_' + str(thread_count) + ".out")
	# f.close()



def lane_benchmark(sf):
	lane_values = [ 16, 32, 128, 256, 512]
	# lane_values = [16, 64, 256]
	# lane_values = [256]
	# f = open('timings-test.txt', 'a')
	f = open('timings-test.txt', 'a')
	f.write("sf " + str(sf) + "########\n")
	edge_count = 1000000
	output_dir = os.path.join(OUTPUT_DIR, "lane")
	output_dir = os.path.join(output_dir,  str(sf))
	file = "fullquery.benchmark"
	# variant = 5
	for lane in lane_values:
		read_and_replace(Mode.LANE, lane=lane)
		run_make()
		for thread_count in thread_values:
			# thread_file = os.path.join(BENCHMARK_DIR, msbfs_file)
			# f.write("Lane " + str(lane) + " Thread " + str(thread_count) + "\n")
			name = "sf" + str(sf) + "_" + "lane" + str(lane) + "_" + "thread" + str(thread_count) + "_variant" + str(variant) +".out"
			read_and_replace(Mode.TIMING, timing_file_name=name)
			thread_file = os.path.join(BENCHMARK_DIR, file)			
			read_and_replace(Mode.THREAD, thread_file=thread_file, thread_count=thread_count)
			run_benchmark(thread_file, output_dir, file.split(".")[0] + '_' + str(lane) + '_' + str(vertex_count)  + '_' + str(edge_count) + '_' + str(thread_count) + ".out")
			# run_benchmark(thread_file, output_dir, file.split(".")[0] + '_' + str(lane) + '_' + str(vertex_count)  + '_' + str(edge_count) + '_' + str(thread_count) + '_' + str(run) + ".out")
	f.close()


def msbfs_variant_benchmark(sf):
	msbfs_divisors = [1, 2, 3, 4]
	# msbfs_divisors = [2]
	f = open('my_file.txt', 'a')
	f.write("########\n")
	edge_count = 1000000
	output_dir = os.path.join(OUTPUT_DIR, "msbfs")
	output_dir = os.path.join(output_dir,  str(sf))
	for divisor in msbfs_divisors:
		read_and_replace(Mode.MSBFS, divisor=divisor)
		run_make()
		for thread_count in thread_values:
			thread_file = os.path.join(BENCHMARK_DIR, msbfs_file)
			read_and_replace(Mode.THREAD, thread_file=thread_file, thread_count=thread_count)
			run_benchmark(thread_file, output_dir, msbfs_file.split(".")[0] + '_' + str(divisor) + '_' + str(vertex_count)  + '_' + str(edge_count) + '_' + str(thread_count) + ".out")


# sf_values = [0.1, 0.3, 1, 3, 10, 30]
# sf_values = [0.1, 0.3, 1, 3]
sf_values = [0.1, 0.3]
sf_files = ["snb.sql", "create_csr.sql", "fullquery.sql"]
debug = False

for sf in sf_values:
	for f in sf_files:
		sf_file = os.path.join(BENCHMARK_DIR, f)
		read_and_replace(Mode.SF, sf_count=sf, sf_file=sf_file)
		
	
	thread_benchmark(sf)
	# lane_benchmark(sf)
	# msbfs_variant_benchmark(sf)


# WITH cte AS 
# (
# 	SELECT count(p.personid) AS vcount FROM Person p 
# )
# SELECT count(src.rowid)
# FROM cte, Person_Subset_l src, Person_Subset_r dst 
# WHERE  
# ( reachability(0, false, cte.vcount, src.rid, dst.rid) = 1);

