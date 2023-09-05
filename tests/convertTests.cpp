#include <gtest/gtest.h>

#include <boop.h>
#include <json.hpp>

struct ConvertTest : public ::testing::Test
{
	std::string ModelImportPath()
	{
		return _model_import_path;
	}

	std::string ModelExportPath()
	{
		return _model_export_path;
	}

	std::string PreConvertedModelPath()
	{
		return _pre_converted_model_path;
	}

	std::string PreConvertedMeshPath()
	{
		return _pre_converted_mesh_path;
	}

	virtual void SetUp() override
	{
		_model_import_path = "Z:/Dev/Boop/tests/TestAssets/Room/Room.fbx";
		_model_export_path = "Z:/Dev/Boop/tests/TestAssets/Room/Room.bp";
		_pre_converted_model_path = "Z:/Dev/Boop/Tests/TestAssets/PreConverted/Room.bp";
		
		_pre_converted_mesh_path = "Z:/Dev/Boop/Tests/TestAssets/PreConverted/container.bp";
	}

	virtual void TearDown() override
	{
	}

private:
	std::string _model_import_path; 
	std::string _model_export_path;

	std::string _pre_converted_model_path;
	std::string _pre_converted_mesh_path;
};

TEST_F(ConvertTest, ReadModel)
{
	boop::AssetFile model_file; 
	boop::load(PreConvertedModelPath().c_str(), model_file);

	boop::ModelInfo model_info = boop::read_model_info(&model_file);

	EXPECT_TRUE(true);
}

TEST_F(ConvertTest, ReadMesh)
{
	 boop::AssetFile mesh_file; 
	 boop::load(PreConvertedMeshPath().c_str(), mesh_file);

     nlohmann::json metadata = nlohmann::json::parse(mesh_file.json);
	 std::string json_metadata = metadata.dump();

	EXPECT_TRUE(true);
};;

TEST_F(ConvertTest, ConvertPNG)
{
	EXPECT_TRUE(true);
}

TEST_F(ConvertTest, ConvertOBJ)
{
	EXPECT_TRUE(true);
}

TEST_F(ConvertTest, ConvertFBX)
{
	EXPECT_TRUE(boop::convert_model(ModelImportPath(), ModelExportPath()));
}