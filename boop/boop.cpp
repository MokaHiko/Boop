#include "boop.h"
#include <fstream>

namespace boop
{
	bool save(const char* path, const AssetFile& file)
	{
		std::ofstream outfile;
		outfile.open(path, std::ios::binary | std::ios::out);
		
		// file type
		outfile.write(file.type, 4);

		// version
		uint32_t version = file.version;
		outfile.write((const char*)&version, sizeof(uint32_t));

		// json meta data length
		uint32_t metaDataSize = file.json.size();
		outfile.write((const char*)&metaDataSize, sizeof(uint32_t));

		// binary data length after compression
		uint32_t binarySize = file.raw_data.size();
		outfile.write((const char*)&binarySize, sizeof(uint32_t));

		// json meta data
		outfile.write(file.json.data(), metaDataSize);

		// write compressed binary data
		outfile.write(file.raw_data.data(), binarySize); 	

		outfile.close();

		return true;
	}
	
	bool load(const char* path, AssetFile& file)
	{
		std::ifstream inFile;
		inFile.open(path, std::ios::binary);

		if(!inFile.is_open())
			return false;
		
		// make sure cursor starts at beginning
		inFile.seekg(0);

		// file type
		inFile.read(file.type, 4);

		// version
		inFile.read((char*)&file.version, sizeof(uint32_t));

		// meta data size
		uint32_t metaDataSize = 0;
		inFile.read((char*)&metaDataSize, sizeof(uint32_t));

		// raw data size
		uint32_t binarySize = 0;
		inFile.read((char*)&binarySize, sizeof(uint32_t));

		// meta data
		file.json.resize(static_cast<size_t>(metaDataSize));
		inFile.read(file.json.data(), metaDataSize);

		// raw binary data
		file.raw_data.resize(static_cast<size_t>(binarySize));
		inFile.read(file.raw_data.data(), binarySize);

		inFile.close();

		return true;
	}
}