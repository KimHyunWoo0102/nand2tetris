#pragma once
#include<filesystem>
#include<iostream>
#include<string>
#include<vector>

class JackCompiler {
	std::filesystem::path p;
	std::vector<std::filesystem::path> filesToCompile;
public:
	JackCompiler(const std::string& outputPath);
	void compile();
};