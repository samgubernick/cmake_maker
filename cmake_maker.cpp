
#include "cmake_maker.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {
	constexpr auto const MATCH		= 0;
	auto const RECOGNIZED_EXTENSION	= std::vector<std::string>({ ".c", ".cpp", ".h", ".hpp", ".ixx" });
	//auto const INPUT_DIRECTORY		= std::string("../../../source/");
	//auto const INPUT_DIRECTORY		= std::string("src/");
	auto const INPUT_DIRECTORY		= std::string("./");
}

class Directory {
public:
	std::filesystem::path				path;
	std::vector<std::filesystem::path>	files;
	std::vector<Directory>				directories;

	Directory(std::filesystem::path path)
		: path(path) {

	};

	bool containsFiles() const {
		if (!files.empty()) {
			return true;
		}
		else {
			for (auto dir : directories) {
				if (dir.containsFiles()) {
					return true;
				}
			}
		}

		return false;
	}
};

void stop() {
	std::cout << std::endl << "Finished creating CMakeLists files" << std::endl;

	for (auto s = std::string(); std::getline(std::cin, s);) {
		if (s.compare("exit") == MATCH) {
			break;
		}
		else if (s.compare("quit") == MATCH) {
			break;
		}
	}
}

bool createDirectory(std::string const & path) {
	if (!std::filesystem::exists(path)) {
		auto created = std::filesystem::create_directory(path);
		if (created) {
			std::cout << "Successfully created directory: " << path << std::endl;
			assert(std::filesystem::exists(path));
			return true;
		}
		else {
			std::cout << "Failed to create directory: " << path << std::endl;
			return false;
		}
	}
	else {
		std::cout << "Directory exists: " << path << std::endl;
		return true;
	}
}

bool isProjectFile(std::string const & extension) {
	return (std::find(std::begin(RECOGNIZED_EXTENSION), std::end(RECOGNIZED_EXTENSION), extension) < std::end(RECOGNIZED_EXTENSION));
}

void scanDirectory(Directory & directory) {
	std::cout << "Current path: " << directory.path.relative_path().string() << std::endl;
	//for (auto & p : std::filesystem::recursive_directory_iterator(directory.path)) {
	for (auto & p : std::filesystem::directory_iterator(directory.path)) {
		if (p.is_directory() && !p.is_symlink()) {
			std::cout << "Sub directory: " << p.path().filename().string() << std::endl;
			auto & subDir = directory.directories.emplace_back(p.path());

			scanDirectory(subDir);
		}
		else if (p.is_regular_file()) {
			if (isProjectFile(p.path().extension().string())) {
				std::cout << "File: " << p.path().filename().string() << std::endl;
				directory.files.push_back(p);
			}
			else {
				std::cout << "Ignoring file (unrecognized extension): " << p.path().filename().string() << std::endl;
			}
		}
	}
}

void writeCMakeFiles(Directory const & directory) {
	if (directory.containsFiles()) {
		auto ofs = std::ofstream(directory.path.string() + "/CMakeLists.txt");
		if (ofs.is_open()) {
			if (!directory.files.empty()) {
				ofs << "target_sources(${PROJECT_NAME}" << std::endl
					<< "		PRIVATE" << std::endl;
				for (auto const& f : directory.files) {
					auto file = f.string().substr(INPUT_DIRECTORY.length());
					std::cout << f.filename().string() << std::endl;
					ofs << "		${CMAKE_CURRENT_LIST_DIR}/" << f.filename().string() << std::endl;
				}
				ofs << ")" << std::endl << std::endl;
			}

			if (!directory.directories.empty()) {
				for (auto const & d : directory.directories) {
					writeCMakeFiles(d);

					if (d.containsFiles()) {
						ofs << "add_subdirectory(\"${CMAKE_CURRENT_LIST_DIR}/" << d.path.filename().string() << "\")" << std::endl;
					}
				}
			}
			ofs.close();
		}
	}
}

void displayFiles(Directory const & directory) {
	for (auto const & d : directory.directories) {
		displayFiles(d);
	}

	for (auto const & f : directory.files) {
		std::cout << "File: " << f.string() << std::endl;
	}
}

int main()
{
	auto const OUTPUT_DIRECTORY = std::string("output");
	auto directory = Directory(INPUT_DIRECTORY); // input directory
	
	if (std::filesystem::is_directory(directory.path)) {
		scanDirectory(directory);


		std::cout << "Output files:" << std::endl;

		if (directory.containsFiles()) {
			std::cout << "Directory has source files!" << std::endl;

			displayFiles(directory);
			writeCMakeFiles(directory);
		}
		else {
			std::cout << "Directory has no source files" << std::endl;
		}
	}
	else {
		std::cerr << "Path (" << directory.path.string() << ") is not a directory" << std::endl;
	}

#ifdef SKIP
	for (auto & p : std::filesystem::recursive_directory_iterator(SOURCE)) {
		if (p.is_directory() && !p.is_symlink()) {
			auto folderToCreate = std::string();
			folderToCreate.reserve(OUTPUT_LENGTH + p.path().string().length());
			folderToCreate.append(OUTPUT).append(p.path().string(), SOURCE.length());
			auto directoryExists = createDirectory(folderToCreate);

			if (directoryExists) {
				createMkFile(p.path(),
							 getMkFilename(folderToCreate),
							 SOURCE);
			}
		}
	}
#endif // SKIP
	stop();
}
