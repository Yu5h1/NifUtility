// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"
#include <iostream>
using namespace std;

char* ConvertToPointerChar(string txt) {
	char *cstr = new char[txt.length() + 1];
	strcpy(cstr, txt.c_str());
	return cstr;
}
string Join(string separator,vector<string> list) {
	string result = "";
	if (list.size() > 0) {
		result = list[0];
		if (list.size() > 1) {
			for (int i = 1; i < list.size(); i++)
				result += separator + list[i];
		}
	}
	return result;
}
template<typename T>
string Join(string separator, vector<T> list, string GetString(T obj)) {
	vector<string> results;
	for (auto item : list)
		results.push_back(GetString(item));
	return Join(separator,results);
}

string GetShapeName(NiShape* shape) { return shape->GetName(); }
string GetPartID(BSDismemberSkinInstance::PartitionInfo partitionInfo) { return to_string(partitionInfo.partID); }
string GetNiString(NiString obj) { return obj.GetString(); }
string GetShapeBodyPartsTxT(NifFile* targetNif, NiShape* shape) {
	string result = shape->GetName() + ":";
	auto bsdSkinInst = targetNif->GetHeader().GetBlock<BSDismemberSkinInstance>(shape->GetSkinInstanceRef());
	if (bsdSkinInst) result += Join(",", bsdSkinInst->GetPartitions(), GetPartID);
	return result;
}

extern "C"
{
	__declspec(dllexport) char* GetShapesName(const char* filePath)
	{
		NifFile target = NifFile(string(filePath));
		return ConvertToPointerChar(Join("\n", target.GetShapes(), GetShapeName));
	}

	__declspec(dllexport) char* GetShapeTextures(const char* filePath, const char* shapeName)
	{
		string result = "";
		NifFile target = NifFile(string(filePath));
		NiShape* targetShape = target.FindShape(string(shapeName));
		if (targetShape) {
			auto shader = target.GetHeader().GetBlock<BSShaderProperty>(targetShape->GetShaderPropertyRef());
			auto textures = target.GetHeader().GetBlock<BSShaderTextureSet>(shader->GetTextureSetRef())->textures;
			result = Join("\n", textures, GetNiString);
		}
		return ConvertToPointerChar(result);
	}

	__declspec(dllexport) char* GetBSDismemberBodyParts(const char* filePath)
	{
		NifFile* target = new NifFile(string(filePath));
		vector<string> shapePartIDs;
		for (auto shape : target->GetShapes())
			shapePartIDs.push_back(GetShapeBodyPartsTxT(target, shape));

		return ConvertToPointerChar(Join("\n", shapePartIDs));
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

