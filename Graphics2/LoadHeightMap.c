//
//bool TerrainNode::LoadHeightMap(wstring heightMapFilename)
//{
//	unsigned int mapSize = _numberOfXPoints * _numberOfZPoints;
//	USHORT * rawFileValues = new USHORT[mapSize];
//
//	std::ifstream inputHeightMap;
//	inputHeightMap.open(heightMapFilename.c_str(), std::ios_base::binary);
//	if (!inputHeightMap)
//	{
//		return false;
//	}
//
//	inputHeightMap.read((char*)rawFileValues, mapSize * 2);
//	inputHeightMap.close();
//
//	// Normalise BYTE values to the range 0.0f - 1.0f;
//	for (unsigned int i = 0; i < mapSize; i++)
//	{
//		_heightValues.push_back((float)rawFileValues[i] / 65536);
//	}
//	delete[] rawFileValues;
//	return true;
//}