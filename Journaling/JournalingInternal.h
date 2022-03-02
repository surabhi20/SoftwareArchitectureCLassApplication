#pragma once

#include <string>
#include <vector>

class GuidObject;
namespace Journal
{
	class JournalFile;
}


Journal::JournalFile* GetActiveJournalFile();

bool InGuidToParamMap(int guid);
void AddGuidToParamMap(int guid, std::string& param);
std::string GetGuidToParam(int guid, bool& found);

void WriteCall();
void WriteCallCPP();
void WriteCallJava();

std::string GenerateParamaterName(std::string paramName);


