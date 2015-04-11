#include <string>
#include <vector>
#include <glm.hpp>

class BasicMesh {

typedef std::vector<float> fVect;
typedef std::vector<size_t> uVect;
typedef std::string str;

public:

	BasicMesh()
	{
	
	}

	BasicMesh(BasicMesh& paramMesh):meshVerts(paramMesh.meshVerts), 
		meshNorms(paramMesh.meshNorms), meshInds(paramMesh.meshInds), meshUVs(paramMesh.meshUVs)
	{

	}


	//Called by Mesh Factory
	BasicMesh(fVect verts, fVect norms, uVect inds, fVect texCoords): meshVerts(verts), meshNorms(norms), 
			meshInds(inds), meshUVs(texCoords)
	{

	}

	~BasicMesh()
	{
	
	}

	void loadGeometricData(fVect vertices, fVect normals, uVect indices)
	{
		meshVerts = vertices;
		meshNorms = normals;
		meshInds = indices;
	}

	void Render()
	{

	}


public:

	fVect meshVerts;
	
	fVect meshNorms;

	uVect meshInds;

	fVect meshUVs;



};
