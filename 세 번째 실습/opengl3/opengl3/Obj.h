#ifndef OBJ_H
#define OBJ_H
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>


struct ObjData {
	std::vector<float> vertices; //정점 좌표들
	std::vector<unsigned int> indices;// 정점 번호들
};

inline bool Loadobj(const std::string& filename,ObjData&obj) {
	std::ifstream file(filename);

	if (!file.is_open()) {
		std::cout << "파일을 열 수 없습니다." << std::endl;
		return false;
	}
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream stream(line);

		std::string type;
		stream >> type;
		if (type == "v") {
			float x, y, z;
			if (stream >> x >> y >> z) {
				obj.vertices.push_back(x);
				obj.vertices.push_back(y);
				obj.vertices.push_back(z);
			}
			else {
				return false;
			}
			std::cout << line << std::endl;
			
		}
		else if (type == "f") {
			std::string a, b, c;

			if (stream >> a >> b >> c) {
				int v1 = std::stoi(a);
				int v2 = std::stoi(b);
				int v3 = std::stoi(c);
				if (v1 <= 0 || v2 <= 0 || v3 <= 0) {
					return false;
				}
				else {
					obj.indices.push_back(v1-1);
					obj.indices.push_back(v2-1);
					obj.indices.push_back(v3-1);
				}
			}
			else {
				return false;
			}
		}

	}
	return true;
}

#endif 