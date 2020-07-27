#ifndef OBJLOADER_H
#define OBJLOADER_H

bool loadOBJ(
	const wchar_t * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);


#ifdef USE_ASSIMP
bool loadAssImp(
	const wchar_t * path, 
	std::vector<unsigned short> & indices,
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals
);
#endif

#endif