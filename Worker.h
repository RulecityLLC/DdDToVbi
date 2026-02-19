#ifndef DDDTOVBI_WORKER_H
#define DDDTOVBI_WORKER_H

#include <mpolib/mpo_stream.h>
#include <mpolib/mpo_json.h>

using std::string;

class Worker
{
public:

	string loadJson(const string &strPath);

	MpoJsonGenericSPtr parseJson(const string &buf);

	void writeVbiFile(MpoJsonGenericSPtr root, const string &strPath);

private:
	void HandleFieldArray(mpo_json_array* pArray, IBlockingStream* pStream);

	void WriteLine(MPO_UINT64 u64LineVal, IBlockingStream* pStream);

	bool m_bNtscMissingWarned = false;
};


#endif //DDDTOVBI_WORKER_H
