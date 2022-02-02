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
THREAD_STRING = "PRAGMA threads="
SF_STRING = "data/ldbc-snb/social-network-sf"

CMD = "BUILD_BENCHMARK=1 BUILD_TPCH=1 make"

RUNNER = "build/release/benchmark/benchmark_runner"

BENCHMARK_DIR = "benchmark/micro/sql-pgq/"

OUTPUT_DIR = os.path.join(BENCHMARK_DIR, "results")

msbfs_file = "msbfs.benchmark"

Mode = Enum('Mode', 'THREAD LANE SF')

def replace_string(mode, in_file, lane, thread_count, sf_count):
	write_lines = list()
	print(mode)
	# new_line = create_line(mode, lane_count, thread_count, vertex_count)
	for lines in in_file.readlines():
		if(mode == Mode.LANE):
		# 	test_string = DEFINE_STRING
		# 	new_string = DEFINE_STRING + str(lane) + "\n"
		# elif(mode.THREAD):
		# 	test_string = THREAD_STRING
		# 	new_string =
			if DEFINE_STRING in lines:
				new_lane = DEFINE_STRING + str(lane) + "\n"
				write_lines.append(new_lane)
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
		else:
			Exception("Unknown Mode")
	return write_lines



	# return write_lines

def read_and_replace(mode, thread_file = '', sf_file = '', lane = 64, thread_count = 1, sf_count = 0.1):
	if(mode == Mode.THREAD):
		input_file = thread_file
		output_file = thread_file
	elif(mode == Mode.LANE):
		input_file = LANE_FILE 
		output_file = LANE_FILE
	elif(mode == Mode.SF):
		input_file = sf_file
		output_file = sf_file
	else:
		print("Unknown mode")
		exit(0)
	with open(input_file, 'r') as in_file:
		write_lines = replace_string(mode, in_file, lane, thread_count, sf_count)
			
	print(write_lines)
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


thread_values = [1, 2, 4]
# thread_values = [2]
vertex_count = 1000
def thread_benchmark(sf):
	
	# thread_files = [ "create_csr_edge.benchmark"]
	thread_files = ["create_csr_vertex.benchmark", "create_csr_edge.benchmark"]
	# thread_files = [ msbfs_file]
	
	lane_count = 64
	edge_mapping = {"vertex":1000000, "edge":1000000, "msbfs":1000000}
	# msbfs.benchmark
	output_dir = os.path.join(OUTPUT_DIR, "thread")
	output_dir = os.path.join(output_dir,  str(sf))
	for file in thread_files:
		for thread_count in thread_values:
			thread_file = os.path.join(BENCHMARK_DIR, file)
			read_and_replace(Mode.THREAD, thread_file=thread_file, thread_count=thread_count)
			output_base = file.split(".")[0].split("_")[-1]
			edge_count = edge_mapping[output_base]
			run_benchmark(thread_file, output_dir, output_base +  '_' + str(lane_count) + '_' +  str(vertex_count) + '_' + str(edge_count) + '_' + str(thread_count) + ".out")



def lane_benchmark(sf):
	# lane_values = [8, 16, 32, 64, 128, 256, 512]
	# lane_values = [16, 64, 256]
	lane_values = [64]
	edge_count = 1000000
	output_dir = os.path.join(OUTPUT_DIR, "lane")
	output_dir = os.path.join(output_dir,  str(sf))
	for lane in lane_values:
		read_and_replace(Mode.LANE, lane=lane)
		run_make()
		for thread_count in thread_values:
			thread_file = os.path.join(BENCHMARK_DIR, msbfs_file)
			read_and_replace(Mode.THREAD, thread_file=thread_file, thread_count=thread_count)
			run_benchmark(thread_file, output_dir, msbfs_file.split(".")[0] + '_' + str(lane) + '_' + str(vertex_count)  + '_' + str(edge_count) + '_' + str(thread_count) + ".out")
		

# sf_values = [0.1, 0.3, 1, 3, 10, 30, 100]
sf_values = [0.1, 0.3]
sf_files = ["snb.sql", "create_csr.sql"]
for sf in sf_values:
	for f in sf_files:
		sf_file = os.path.join(BENCHMARK_DIR, f)
		read_and_replace(Mode.SF, sf_count=sf, sf_file=sf_file)
	thread_benchmark(sf)
	# lane_benchmark(sf)
