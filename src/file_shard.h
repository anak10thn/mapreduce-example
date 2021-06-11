#pragma once

#include <math.h>
#include <vector>
#include "mapreduce_spec.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;

/* CS6210_TASK: Create your own data structure here, where you can hold information about file splits,
     that your master would use for its own bookkeeping and to convey the tasks to the workers for mapping */

struct fileOffset {
	string filename;
	int start_offset;
	int end_offset;
};

struct FileShard {
	int shard_id;
	vector<fileOffset*> pieces;
};


/* CS6210_TASK: Create fileshards from the list of input files, map_kilobytes etc. using mr_spec you populated  */ 
inline bool shard_files(const MapReduceSpec& mr_spec, vector<FileShard>& fileShards) {
	cout << "Start file shard procedure!" << endl;

	vector<string> filenames = mr_spec.input_files;

	int totalsize;	
	FileShard* shard = new FileShard;
	int cur_bytes = 0, shard_id = 0;

	for(auto filename : filenames) {
		ifstream file(filename, ifstream::ate | ifstream::binary);
		fileOffset* file_shard = new fileOffset;
		
		file.seekg(0, file.end);
		int size = file.tellg(); 
		file.seekg(0, file.beg);
		int cur_offset = 0;
		char ch;

		while(!file.eof()) {
			file.seekg(cur_offset);

			int num_bytes_left = (mr_spec.map_kilobytes * 1000 - cur_bytes);
			int size_left = (size) - cur_offset;
			
			int num_to_seek = fmin(num_bytes_left, size_left);

			file.seekg(cur_offset + num_to_seek);
			file_shard->filename = filename;
			file_shard->start_offset = cur_offset;		
			if(num_to_seek != size_left) {
				char ch;
				int newline_offset = 0;
				while(ch != '\n') {
					file.get(ch);
					newline_offset++;
				}
				cur_offset += newline_offset;
			}

			file_shard->end_offset = file.tellg();
			shard->pieces.push_back(file_shard);
			cur_offset += num_to_seek;
			file_shard = new fileOffset;
			cur_bytes += num_to_seek;

			if(cur_bytes >= mr_spec.map_kilobytes * 1000) {
				shard -> shard_id = shard_id;
				fileShards.push_back(*shard);
				shard = new FileShard;
				shard_id++;
				cur_bytes = 0;
			}

			file >> ch;
		}

		totalsize += size;
	}

	shard->shard_id = shard_id;
	fileShards.push_back(*shard);

	auto shards_num = ceil(totalsize / (mr_spec.map_kilobytes * 1000));
	cout << "Files size: " << totalsize << ", number of shards: " << shards_num << endl;
	cout << "number of shards: " << fileShards.size() << endl;
	return true;
}
