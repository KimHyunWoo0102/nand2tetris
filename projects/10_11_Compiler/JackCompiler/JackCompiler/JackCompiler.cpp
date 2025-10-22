#include "JackCompiler.h"
#include<stdexcept>
#include "CompilationEngine.h"

JackCompiler::JackCompiler(const std::string& outputPath)
	:p(outputPath)
{
	filesToCompile.clear();

	if (!std::filesystem::exists(p)) {
		throw std::runtime_error("can't open file");
	}

	if (std::filesystem::is_directory(p)) {
		for (const auto& entry : std::filesystem::directory_iterator(p)) {
			if (entry.is_regular_file() && entry.path().extension() == ".jack") {
				filesToCompile.push_back(entry.path());
			}
		}
	}
	else {
		filesToCompile.push_back(p);
	}

}

void JackCompiler::compile() {
	for (const auto& jackFile : filesToCompile) {
		std::filesystem::path vmFile = jackFile;
		vmFile.replace_extension(".vm");
		std::string outputFilename = vmFile.string();

		std::cout << "Compiling " << jackFile.filename() << " -> " << vmFile.filename() << std::endl;

		// 1. 새 Tokenizer 생성
		Tokenizer tokenizer(jackFile.string());
		// 2. 새 CompilationEngine 생성 (VMWriter도 내부에서 새로 만들어짐)
		CompilationEngine engine(tokenizer, outputFilename);

		// 3. 해당 파일 컴파일 실행
		engine.compile();
	}
}