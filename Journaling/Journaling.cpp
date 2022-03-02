// Journaling.cpp : Defines the exported functions for the DLL.
//

#include "framework.h"
#include "Journaling.h"
#include "JournalingInternal.h"
#include "JournalHelpers.h"
#include "JournalCallParamDataClass.h"
#include "JournalCallParamDataString.h"
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include "JournalCallData.h"
#include "JournalFile.h"

using namespace Journal;

static JournalFile* activeJournalFile = nullptr;


static JournalingLanguage m_JournalingLanguage;
static bool m_isJournaling = false;

static Journal::JournalCallData * currentCall = nullptr;

static std::map<int, std::string> m_guidToParamMap;

static std::map<std::string, int> m_paramNameCounts;

JournalFile* GetActiveJournalFile()
{
    return activeJournalFile;
}

bool InGuidToParamMap(int guid)
{
    if (m_guidToParamMap.find(guid) == m_guidToParamMap.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void AddGuidToParamMap(int guid, std::string& param)
{
    m_guidToParamMap[guid] = std::string(param);
}

std::string GetGuidToParam(int guid, bool & found)
{
    
    if (m_guidToParamMap.find(guid) != m_guidToParamMap.end())
    {
        found = true;
        return m_guidToParamMap[guid];
    }
    else
    {
        found = false;
        return "";
    }

    
}

void SetJournalingLangauge(JournalingLanguage jnlLang)
{
    if (m_isJournaling)
    {
        throw std::exception("Cannot change journal language while currently journaling");
    }
    else
    {
        m_JournalingLanguage = jnlLang;
    }
    
}

JournalingLanguage GetJournalingLangauge()
{
    return m_JournalingLanguage;
}

void StartJournaling(std::string jnlFile)
{
    if (m_isJournaling)
    {
        throw std::exception("Already Journaling , StartJournaling");
    }
    else
    {
        m_isJournaling = true;
        activeJournalFile = new JournalFile(jnlFile, GetJournalingLangauge());

    }
}



void EndJournaling()
{
    if (!m_isJournaling)
    {
        throw std::exception("Not Journaling, EndJournaling");
    }
    else
    {
        //write out file and delete resources
        activeJournalFile->WriteJournalFile();
        delete activeJournalFile;

        m_isJournaling = false;
        m_guidToParamMap.clear();
        m_paramNameCounts.clear();
    }

}


bool IsJournaling()
{
    return m_isJournaling;
}


void JournalIntInParam(int value, std::string paramName)
{
    // TODO 
    // You will need ot create a JournalCallParamDataInteger class as a reminder
}

void JournalBoolInParam(bool value, std::string paramName)
{
    // TODO
    // You will need ot create a JournalCallParamDataBoolean class as a reminder
}

void JournalStringInParam(std::string value, std::string paramName)
{
    JournalCallParamDataString* journalCallParamData = new JournalCallParamDataString(paramName, JournalCallParamData::ParameterMetaType::INPUT, value);
    currentCall->AddParameter(journalCallParamData);
}

void JournalReturnClass(GuidObject* classObject, std::string className, std::string paramName)
{
    JournalCallParamDataClass* journalCallParamData = 
        new JournalCallParamDataClass(paramName, JournalCallParamData::ParameterMetaType::RETURN, classObject, className);

    currentCall->AddReturnValue(journalCallParamData);
}

void JournalStartCall(std::string methodName, CannedGlobals cannedGlobal)
{
    currentCall = new JournalCallData(methodName, cannedGlobal);
}

void JournalStartCall(std::string methodName, GuidObject* classObject)
{
    currentCall = new JournalCallData(methodName, classObject);
}

void JournalEndCall()
{
    WriteCall();
}

void WriteCall()
{
    if (GetJournalingLangauge() == JournalingLanguage::CPP)
    {
        WriteCallCPP();
    }
    else
    {
        WriteCallJava();
    }
}

void WriteCallCPP()
{
    currentCall->Journal();
}

void WriteCallJava()
{
    std::string NIY = std::string("NIY WriteCallJava");
    activeJournalFile->WriteToFile(NIY);
    activeJournalFile->NewLine(); 
}

std::string GenerateParamaterName(std::string paramNameBase)
{
    std::string retVal;
    int numAppend = 0;
    //Is nameBase in map already?
    if (m_paramNameCounts.find(paramNameBase) == m_paramNameCounts.end())
    {
        //Not in map so append 1 and store it
        m_paramNameCounts[paramNameBase] = 1;
        numAppend = 1;
    }
    else
    {
        numAppend = m_paramNameCounts[paramNameBase] ;
        numAppend = numAppend + 1;
        m_paramNameCounts[paramNameBase] = numAppend;
    }

    std::stringstream paramNameBuilder;
    paramNameBuilder << paramNameBase << numAppend;
    retVal = paramNameBuilder.str();
    
    return retVal;
}

