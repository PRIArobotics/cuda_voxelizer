#include "util.h"
#include "util_io.h"

using namespace std;

size_t get_file_length(const std::string base_filename){
	// open file at the end
	std::ifstream input(base_filename.c_str(), ios_base::ate | ios_base::binary);
	assert(input);
	size_t length = input.tellg();
	input.close();
	return length; // get file length
}
void read_binary(void* data, const size_t length, const std::string base_filename){
	// open file
	std::ifstream input(base_filename.c_str(), ios_base::in | ios_base::binary);
	assert(input);
#ifndef SILENT
	fprintf(stdout, "[I/O] Reading %llu kb of binary data from file %s \n", size_t(length / 1024.0f), base_filename.c_str()); fflush(stdout);
#endif
	input.seekg(0, input.beg);
	input.read((char*) data, 8);
	input.close();
	return;
}

void write_obj(const unsigned int* vtable, const size_t gridsize, const std::string base_filename) {
	string filename_output = base_filename + string("_") + to_string(gridsize) + string(".obj");
#ifndef SILENT
	fprintf(stdout, "[I/O] Writing data in obj format to %s \n", filename_output.c_str());
#endif
	ofstream output(filename_output.c_str(), ios::out);
	assert(output);
	for (size_t x = 0; x < gridsize; x++) {
		for (size_t y = 0; y < gridsize; y++) {
			for (size_t z = 0; z < gridsize; z++) {
				if (checkVoxel(x, y, z, gridsize, vtable)) {
					output << "v " << (x+0.5) << " " << (y + 0.5) << " " << (z + 0.5) << endl; // +0.5 to put vertex in the middle of the voxel
				}
				//else {
				//	output << "NOVOXEL " << x << " " << y << " " << z << endl;
				//}
			}
		}
	}
	output.close();
}

void write_binary(void* data, size_t bytes, const std::string base_filename){
	string filename_output = base_filename + string(".bin");
#ifndef SILENT
	fprintf(stdout, "[I/O] Writing data in binary format to %s (%s) \n", filename_output.c_str(), readableSize(bytes).c_str());
#endif
	ofstream output(filename_output.c_str(), ios_base::out | ios_base::binary);
	output.write((char*)data, bytes);
	output.close();
}

void write_binvox(const unsigned int* vtable, const size_t gridsize, const std::string base_filename){
	// Open file
	string filename_output = base_filename + string("_") + to_string(gridsize) + string(".binvox");
#ifndef SILENT
	fprintf(stdout, "[I/O] Writing data in binvox format to %s \n", filename_output.c_str());
#endif
	ofstream output(filename_output.c_str(), ios::out | ios::binary);
	assert(output);
	
	// Write ASCII header
	output << "#binvox 1" << endl;
	output << "dim " << gridsize << " " << gridsize << " " << gridsize << "" << endl;
	output << "data" << endl;

	// Write BINARY Data (and compress it a bit using run-length encoding)
	char currentvalue, current_seen;
	for (size_t x = 0; x < gridsize; x++){
		for (size_t z = 0; z < gridsize; z++){
			for (size_t y = 0; y < gridsize; y++){
				if (x == 0 && y == 0 && z == 0){ // special case: first voxel
					currentvalue = checkVoxel(0, 0, 0, gridsize, vtable);
					output.write((char*)&currentvalue, 1);
					current_seen = 1;
					continue;
				}
				char nextvalue = checkVoxel(x, y, z, gridsize, vtable);
				if (nextvalue != currentvalue || current_seen == (char) 255){
					output.write((char*)&current_seen, 1);
					current_seen = 1;
					currentvalue = nextvalue;
					output.write((char*)&currentvalue, 1);
				}
				else {
					current_seen++;
				}
			}
		}
	}

	// Write rest
	output.write((char*)&current_seen, 1);
	output.close();
}