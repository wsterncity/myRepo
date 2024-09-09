#include "iGameOBJReader.h"

IGAME_NAMESPACE_BEGIN
bool OBJReader::Parsing()
{
	int VertexNum = 0;
	int FaceNum = 0;

	const char* left = this->FILESTART;
	const char* right = this->FILEEND;

	size_t idx = -1;
	{
		std::string fileStr(left);
		const std::string targetString = "\nf ";
		idx = fileStr.find(targetString);
	}

	const char* v_begin = left;
	const char* v_end = v_begin + idx;
	const char* f_begin = v_end + 1;
	const char* f_end = right;

	double progress_1 = 0.0;
	double progress_2 = 0.0;
	ThreadPool::Pointer tp = ThreadPool::Instance();
	Points::Pointer points = m_Data.GetPoints();

	while (v_begin < v_end) {
		// 查找行结束位置
		const char* lineEnd = strchr(v_begin, '\n');
		if (!lineEnd) {
			lineEnd = v_end;
		}
		std::string line(v_begin, lineEnd);
		// 更新指针位置		
		if (line.size() < 1) {
			v_begin = lineEnd + 1;
			continue;
		}
		if (line[0] == 'v' && line[1] == ' ') {
			float data[3] = { 0,0,0 };
			const char* data_st;
			data_st = v_begin + 2;
			for (int i = 0; i < 3; i++) {
				data_st = mAtof(data_st, data[i]) + 1;
			}
			points->AddPoint(data);
			VertexNum++;
		}
		v_begin = lineEnd + 1;
		progress_1 = 1.0 * (v_begin - left) / (v_end - left);
	}

	//auto result1 = tp->Commit([&]()->void {

	//	});

	CellArray::Pointer Faces = m_Data.GetFaces();
	AttributeSet::Pointer Data = m_Data.GetData();
	int vhs[64] = { 0 };
	while (f_begin < f_end) {
		// 查找行结束位置
		const char* lineEnd = strchr(f_begin, '\n');
		if (!lineEnd) {
			lineEnd = f_end;
		}
		std::string line(f_begin, lineEnd);
		// 更新指针位置
		if (line.size() < 1) {
			f_begin = lineEnd + 1;
			continue;
		}
		const char* data_st;
		if (line[0] == 'f' && line[1] == ' ') {
			int vh;
			data_st = f_begin + 2;
			int size = 0;
			while (data_st < lineEnd && *data_st != '\r') {
				data_st = mAtoi(data_st, vh);
				if (*data_st != ' ') {
					data_st = strchr(data_st, ' ');
				}
				vhs[size++] = vh - 1;
				if (!data_st) {
					data_st = f_end;
				}
				data_st++;
			}
			Faces->AddCellIds(vhs, size);
			FaceNum++;
		}
		f_begin = lineEnd + 1;
		progress_2 = 1.0 * (f_begin - v_end) / (f_end - v_end);
	}
	

	//auto result2 = tp->Commit([&]()->void {
	//	
	//	});
	//result1.wait();
	//result2.wait();
	return true;
}
IGAME_NAMESPACE_END