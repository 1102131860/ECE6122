/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Oct 22 2024
Description:
There are three structs here: Material, ObjectData, Object
It also includes the function: loadObjdata, initialize Chess and Chess Board
*/

#ifndef LOADOBJ_H
#define LOADOBJ_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <common/texture.hpp>

// Structure to store material-specific data loaded from a .mtl file
struct Material {
    std::string name;
    std::string diffuseTexture;     // Path to the diffuse texture (map_Kd)
    GLuint textureID;               // OpenGL texture ID
    glm::vec3 Ka;                   // Ambient reflectivity
    glm::vec3 Kd;                   // Diffuse reflectivity
    glm::vec3 Ks;                   // Specular reflectivity
    float Ns;                       // Specular exponent (shininess)
};

// Structure to store object-specific data loaded from an .obj file
struct ObjectData {
    std::vector<unsigned short> indices;        // polygons (faces)
    std::vector<glm::vec3> indexed_vertices;    // vertices
    std::vector<glm::vec2> indexed_uvs;         // vertices texture coordinates
    std::vector<glm::vec3> indexed_normals;     // vectices normals
    GLuint vertexbuffer, uvbuffer, normalbuffer, elementbuffer; // OpenGL buffer ID
    Material material;                          // Associated material for this object
};

// Structure to store chess, chessboard object
struct Object {
    std::string name;                           // name
    int chessID;                                // chessID
    bool isBlack;                               // 1 is white, 0 is black
    int row;                                    // the row chess locates
    int col;                                    // the column chess locates
    ObjectData objData;                         // Associated objectData for this object
    glm::mat4 transformMatrix;                  // transformMatrix (from row and col)
};

// Function to load .obj file using Assimp and store data in ObjectData
bool loadObjectData(const char* path, std::string prefix, std::vector<ObjectData>& objects) {
    std::string appendedObjectpath = prefix + path;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(appendedObjectpath.c_str(), 
        aiProcess_GenSmoothNormals |
        aiProcess_Triangulate /* |
        aiProcess_CalcTangentSpace |
        aiProcess_FlipUVs |
       aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType*/);

    if (!scene) {
        std::cerr << "Error loading OBJ file with AssImp: " << importer.GetErrorString() << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        const aiMesh* mesh = scene->mMeshes[i];
        ObjectData objectData;

        objectData.indexed_vertices.reserve(mesh->mNumVertices);
        objectData.indexed_uvs.reserve(mesh->mNumVertices);
        objectData.indexed_normals.reserve(mesh->mNumVertices);
        objectData.indices.reserve(3 * mesh->mNumFaces);

        // Load vertices, uvs, normals, and indices
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D pos = mesh->mVertices[v];
            objectData.indexed_vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));

            aiVector3D UVW = mesh->mTextureCoords[0][v];
            objectData.indexed_uvs.push_back(glm::vec2(UVW.x, UVW.y));

            aiVector3D n = mesh->mNormals[v];
            objectData.indexed_normals.push_back(glm::vec3(n.x, n.y, n.z));
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            objectData.indices.push_back(mesh->mFaces[f].mIndices[0]);
            objectData.indices.push_back(mesh->mFaces[f].mIndices[1]);
            objectData.indices.push_back(mesh->mFaces[f].mIndices[2]);
        }

        // Load associated material data
        if (scene->HasMaterials()) {
            const aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
            Material material;
            aiString matName;
            aiMat->Get(AI_MATKEY_NAME, matName);
            material.name = matName.C_Str();

            // Load the diffuse texture (map_Kd)
            aiString texturePath;
            if (AI_SUCCESS == aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath)) {
                material.diffuseTexture = texturePath.C_Str();
                std::string appendedTexturePath = prefix + material.diffuseTexture;
                // Load the texture using stb_image
                material.textureID = loadBMP_custom(appendedTexturePath.c_str());
                if (!material.textureID) {
                    std::cerr << "Failed to load texture for material: " << material.name << std::endl;
                }
            }

            // Load ambient, diffuse, and specular reflectivity
            aiColor3D color(0.f, 0.f, 0.f);
            aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color);
            material.Ka = glm::vec3(color.r, color.g, color.b);

            aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            material.Kd = glm::vec3(color.r, color.g, color.b);

            aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color);
            material.Ks = glm::vec3(color.r, color.g, color.b);

            // Load the specular exponent (shininess)
            aiMat->Get(AI_MATKEY_SHININESS, material.Ns);

            // Store the material in the object data
            objectData.material = material;
        }

        // Initialize the vertex buffer
        glGenBuffers(1, &objectData.vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, objectData.vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, objectData.indexed_vertices.size() * sizeof(glm::vec3), &objectData.indexed_vertices[0], GL_STATIC_DRAW);

        // Initialize the UV buffer
        glGenBuffers(1, &objectData.uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, objectData.uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, objectData.indexed_uvs.size() * sizeof(glm::vec2), &objectData.indexed_uvs[0], GL_STATIC_DRAW);

        // Initialize the normal buffer
        glGenBuffers(1, &objectData.normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, objectData.normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, objectData.indexed_normals.size() * sizeof(glm::vec3), &objectData.indexed_normals[0], GL_STATIC_DRAW);

        // Initialize the element buffer (indices)
        glGenBuffers(1, &objectData.elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objectData.elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, objectData.indices.size() * sizeof(unsigned short), &objectData.indices[0], GL_STATIC_DRAW);

        objects.push_back(objectData);
    }

    return true;
}

// Function to initialize chess object list
void initializeChesses(std::vector<Object>& objectList, std::vector<ObjectData>& objectDataList) {
    objectList.resize(32);
    for (int i = 0; i < 32; i++) {
        if (i == 0 || i == 1 || i == 16 || i == 17) {
            objectList[i].name = "Rock";
            objectList[i].chessID = i % 16;
            objectList[i].isBlack = i > 15;
            objectList[i].row = 7 * objectList[i].isBlack;
            objectList[i].col = objectList[i].chessID ? 7 : 0;

            for (ObjectData& objData : objectDataList) {
                if (objData.material.name.find("TORRE") != std::string::npos && // Extract the number from "Rock##"
                    objectList[i].isBlack == (std::stoi(objData.material.name.substr(5)) - 2 == 0)) {
                    objectList[i].objData = objData;
                }
            }
        }
        else if (i == 2 || i == 3 || i == 18 || i == 19) {
            objectList[i].name = "Knight";
            objectList[i].chessID = i % 16 - 2;
            objectList[i].isBlack = i > 15;
            objectList[i].row = 7 * objectList[i].isBlack;
            objectList[i].col = objectList[i].chessID ? 6 : 1;

            for (ObjectData& objData : objectDataList) {
                if (objData.material.name.find("Object") != std::string::npos && // Extract the number from "Object##"
                    objectList[i].isBlack == (std::stoi(objData.material.name.substr(6)) - 2 == 0)) {
                    objectList[i].objData = objData;
                }
            }
        }
        else if (i == 4 || i == 5 || i == 20 || i == 21) {
            objectList[i].name = "Bishop";
            objectList[i].chessID = i % 16 - 4;
            objectList[i].isBlack = i > 15;
            objectList[i].row = 7 * objectList[i].isBlack;
            objectList[i].col = objectList[i].chessID ? 5 : 2;

            for (ObjectData& objData : objectDataList) {
                if (objData.material.name.find("ALFIERE") != std::string::npos && // Extract the number from "ALFIERE##"
                    objectList[i].isBlack == (std::stoi(objData.material.name.substr(7)) - 2 == 0)) {
                    objectList[i].objData = objData;
                }
            }
        }
        else if (i == 6 || i == 7 || i == 22 || i == 23) {
            objectList[i].name = (i % 16 - 6) ? "Queen" : "King";
            objectList[i].chessID = 0;
            objectList[i].isBlack = i > 15;
            objectList[i].row = 7 * objectList[i].isBlack;
            objectList[i].col = (i % 16 - 6) ? 4 : 3;

            for (ObjectData& objData : objectDataList) {
                if (objectList[i].name == "King" &&
                    objData.material.name.find("REGINA") != std::string::npos &&    // Extract the number from "REGINA##"
                    objectList[i].isBlack == (std::stoi(objData.material.name.substr(6)) - 1 == 0)) {
                    objectList[i].objData = objData;
                }
                if (objectList[i].name == "Queen" &&
                    objData.material.name.find("REGINA") == std::string::npos &&    // doesn't contain "REGINA"
                    objData.material.name.find("RE") == 0 &&                        // must start with "RE"
                    objData.material.name.find("RE") != std::string::npos &&        // Extract the number from "RE##"
                    objectList[i].isBlack == (std::stoi(objData.material.name.substr(2)) - 1 == 0)) {
                    objectList[i].objData = objData;
                }
            }
        }
        else {
            objectList[i].name = "Pawn";
            objectList[i].chessID = i % 16 - 8;
            objectList[i].isBlack = i > 15;
            objectList[i].row = objectList[i].isBlack ? 6 : 1;
            objectList[i].col = objectList[i].chessID;

            for (ObjectData& objData : objectDataList) {
                if (objData.material.name.find("PEDONE") != std::string::npos && // Extract the number from "PEDONE##"
                    objectList[i].isBlack == (std::stoi(objData.material.name.substr(6)) - 12 == 0)) {
                    objectList[i].objData = objData;
                }
            }
        }

        float scaleFactor = 0.025f;                         // used to get scalingMatrix
        glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor));

        float row = (float)5.5 * objectList[i].row + 28;    // the row interval
        float col = (float)objectList[i].col - 12;          // the column interval
        if (objectList[i].chessID > 0) {
            if (objectList[i].name == "Rock" ||
                objectList[i].name == "Knight" ||
                objectList[i].name == "Bishop") {
                col = (float)15 * (objectList[i].col - 4);
            }
        }
        if (objectList[i].name == "Pawn") {
            col = (float)(8 * objectList[i].col - 45);
        }
        float height = -0.5f;

        glm::vec3 destinationPosition = glm::vec3(col, height, row);
        glm::mat4 transitionMatrix = glm::translate(glm::mat4(1.0f), destinationPosition);

        objectList[i].transformMatrix = transitionMatrix * scalingMatrix;
    }
}

// Function to initialize chessboard
void initializeChessBoard(Object& chessBoard, ObjectData& chessBoardData) {
    chessBoard.name = "ChessBoard";
    chessBoard.chessID = -1;       // indicates invalid
    chessBoard.isBlack = 0;
    chessBoard.row = -1;
    chessBoard.col = -1;
    chessBoard.objData = chessBoardData;    // binds to the given object data

    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f),
        glm::radians(-90.0f),           // Rotate by 90 degrees, 
        glm::vec3(1.0f, 0.0f, 0.0f));   // Rotate around the X-axis

    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f),
        glm::radians(180.0f),            // Rotate by 180 degrees
        glm::vec3(0.0f, 1.0f, 0.0f));   // Rotate around the Z-axis

    float scaleFactor = 1.5f;     // used to get scalingMatrix (may need tuning)
    glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor));

    chessBoard.transformMatrix = rotationZ * rotationX * scalingMatrix;
}

#endif // LOADOBJ_H