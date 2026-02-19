#include "Worker.h"

#include <mpolib/mpo_file_stream.h>
#include <mpolib/mpo_misc.h>
#include <iostream>

string Worker::loadJson(const string& strPath)
{
	blocking_sharedptr stream = MpoFileStreamFactory::CreateInstance(strPath.c_str(), MPO_OPEN_READONLY);
	return StreamHelpers::StreamToString(stream.get());
}

MpoJsonGenericSPtr Worker::parseJson(const string &buf)
{
	mpo_json j;

	return j.parse(buf.data(), buf.size());
}

void Worker::writeVbiFile(MpoJsonGenericSPtr root, const string &strPath)
{
	blocking_sharedptr stream = MpoFileStreamFactory::CreateInstance(strPath.c_str(), MPO_OPEN_CREATE);

	mpo_json_generic *pRoot = root.get();
	mpo_json_object *pObject = pRoot->ToObject();

	list<mpo_json_keypair> *pChildren = pObject->GetChildren();

	for (const mpo_json_keypair& pair : *pChildren)
	{
		// we only care about fields
		if (pair.key != L"fields")
		{
			continue;
		}

		MpoJsonGenericSPtr fields = pair.value;
		mpo_json_array *pFieldsArray = fields->ToArray();

		this->HandleFieldArray(pFieldsArray, stream.get());
	}
}

void Worker::HandleFieldArray(mpo_json_array* pArray, IBlockingStream* pStream)
{
	list<MpoJsonGenericSPtr> emptyJsonList;	// for performance
	list<MpoJsonGenericSPtr> *pChildren = pArray->GetChildren();

	// see https://www.daphne-emu.com:9443/mediawiki/index.php/VBIInfo#VBI_File_Format
	string header = "1VBI";
	pStream->Write(header.data(), header.size());

	int fieldIdx = 0;	// for logging purposes

	for (MpoJsonGenericSPtr field : *pChildren)
	{
		mpo_json_object *pObject = field->ToObject();

		map<wstring, MpoJsonGenericSPtr> mChildren = pObject->GetChildrenAsMap();

		// get white flag
		bool bWhiteFlag = false;
		auto findNtscResult = mChildren.find(L"ntsc");

		// sometimes the ntsc area may be missing; especially if the disc is PAL :)
		if (findNtscResult != mChildren.end())
		{
			mpo_json_object* pObjectNTSC = findNtscResult->second->ToObject();
			map<wstring, MpoJsonGenericSPtr> mChildrenNTSC = pObjectNTSC->GetChildrenAsMap();

			// whiteflag may not be guaranteed to exist, we need to be defensive against unexpected json
			auto findWhiteflagResult = mChildrenNTSC.find(L"whiteFlag");
			if (findWhiteflagResult != mChildrenNTSC.end())
			{
				mpo_json_boolean* pWhiteFlag = findWhiteflagResult->second->ToBoolean();
				bWhiteFlag = pWhiteFlag->GetValue();
			}
		}
		else
		{
			// we don't want to spam them
			if (!this->m_bNtscMissingWarned)
			{
				std::cout << "WARNING: ntsc section is missing.  Did you forget to run ld-process-vbi?" << std::endl;
				this->m_bNtscMissingWarned = true;
			}
		}

		// get vbi

		list<MpoJsonGenericSPtr> *pListVbiLines = &emptyJsonList;
		MPO_UINT64 u64Line161718[3] = { 0, 0, 0 };

		auto findVbiResult = mChildren.find(L"vbi");
		if (findVbiResult != mChildren.end())
		{
			mpo_json_object *pObjectVBI = findVbiResult->second->ToObject();
			map<wstring, MpoJsonGenericSPtr> mChildrenVBI = pObjectVBI->GetChildrenAsMap();

			mpo_json_array *pVbiData = mChildrenVBI[L"vbiData"]->ToArray();

			pListVbiLines = pVbiData->GetChildren();
		}

		int idx = 0;

		// sometimes this list will not be fully populated, if entries imply a blank line
		while ((idx < 3) && (!pListVbiLines->empty()))
		{
			MPO_INT64 iValue = pListVbiLines->front()->ToInt()->GetValue();
			u64Line161718[idx] = (iValue >= 0) ? iValue : 0x7FFFFF; // parse errors will be -1 in the .json, and should be 0x7FFFFF in the destination file according to specification
			pListVbiLines->pop_front();
			idx++;
		}

		uint8_t u8 = bWhiteFlag ? 1 : 0;
		pStream->Write(&u8, sizeof(u8));

		this->WriteLine(u64Line161718[0], pStream);
		this->WriteLine(u64Line161718[1], pStream);
		this->WriteLine(u64Line161718[2], pStream);

		fieldIdx++;
	}
}

void Worker::WriteLine(MPO_UINT64 u64LineVal, IBlockingStream* pStream)
{
	string s = mpom::uint2bige64(u64LineVal);
	pStream->Write(&s[5], 1);
	pStream->Write(&s[6], 1);
	pStream->Write(&s[7], 1);
}
