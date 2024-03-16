//  FILE:	CvInfos.cpp
//
//  PURPOSE: The base class for all info classes to inherit from.  This gives us the base description
//				and type strings
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvHeritageInfo.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoUtil.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"

bool shouldHaveType = false;

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CInfoBase()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvInfoBase::CvInfoBase() :
m_bGraphicalOnly(false)
{
}

CvInfoBase::CvInfoBase(const char* szType) :
m_bGraphicalOnly(false),
m_szType(szType)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CInfoBase()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvInfoBase::~CvInfoBase()
{
}

void CvInfoBase::reset()
{
	//clear cache
	m_aCachedDescriptions.clear();
	m_szCachedText.clear();
	m_szCachedHelp.clear();
	m_szCachedStrategy.clear();
	m_szCachedCivilopedia.clear();
}

bool CvInfoBase::isGraphicalOnly() const
{
	return m_bGraphicalOnly;
}

const char* CvInfoBase::getType() const
{
	// AIAndy: Comparing a CvString to NULL makes no real sense
	//if (m_szType == NULL)
	//	return NULL;

	if (m_szType.empty())
	{
		return NULL;
	}

	return m_szType;
}

const char* CvInfoBase::getButton() const
{
	if (m_szButton.empty())
	{
		return NULL;
	}

	return m_szButton;
}

const wchar_t* CvInfoBase::getCivilopediaKey() const
{
	return m_szCivilopediaKey;
}

const wchar_t* CvInfoBase::getHelpKey() const
{
	return m_szHelpKey;
}

const wchar_t* CvInfoBase::getStrategyKey() const
{
	return m_szStrategyKey;
}

const wchar_t* CvInfoBase::getTextKeyWide() const
{
	return m_szTextKey;
}

const wchar_t* CvInfoBase::getDescription(uint uiForm) const
{
	PROFILE_EXTRA_FUNC();
	while(m_aCachedDescriptions.size() <= uiForm)
	{
		m_aCachedDescriptions.push_back(gDLL->getObjectText(m_szTextKey, m_aCachedDescriptions.size()));
	}

	return m_aCachedDescriptions[uiForm];
}

const wchar_t* CvInfoBase::getText() const
{
	// used instead of getDescription for Info entries that are not objects
	// so they do not have gender/plurality/forms defined in the Translator system
	if(m_szCachedText.empty())
	{
		m_szCachedText = gDLL->getText(m_szTextKey);
	}

	return m_szCachedText;
}

const wchar_t* CvInfoBase::getCivilopedia() const
{
	if(m_szCachedCivilopedia.empty())
	{
		m_szCachedCivilopedia = gDLL->getText(m_szCivilopediaKey);
	}

	return m_szCachedCivilopedia;
}

const wchar_t* CvInfoBase::getHelp() const
{
	if (m_szCachedHelp.empty())
	{
		m_szCachedHelp = gDLL->getText(m_szHelpKey);
	}

	return m_szCachedHelp;
}

const wchar_t* CvInfoBase::getStrategy() const
{
	if (m_szCachedStrategy.empty())
	{
		m_szCachedStrategy = gDLL->getText(m_szStrategyKey);
	}

	return m_szCachedStrategy;
}

//
// read from XML
//
bool CvInfoBase::read(CvXMLLoadUtility* pXML)
{
	pXML->GetOptionalChildXmlValByName(&m_bGraphicalOnly, L"bGraphicalOnly");
	pXML->GetOptionalChildXmlValByName(m_szType, L"Type");
	pXML->GetOptionalChildXmlValByName(m_szTextKey, L"Description");
	pXML->GetOptionalChildXmlValByName(m_szCivilopediaKey, L"Civilopedia");
	pXML->GetOptionalChildXmlValByName(m_szHelpKey, L"Help");
	pXML->GetOptionalChildXmlValByName(m_szStrategyKey, L"Strategy");
	pXML->GetOptionalChildXmlValByName(m_szButton, L"Button");

	return true;
}

void CvInfoBase::copyNonDefaults(const CvInfoBase* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	if ( m_bGraphicalOnly == false )
	{
		m_bGraphicalOnly = pClassInfo->isGraphicalOnly();
	}

	if (( getType() == NULL ) || ( getType() == cDefault ))
	{
		m_szType = pClassInfo->getType();
	}

	if ( getCivilopediaKey() == NULL || getCivilopediaKey() == wDefault)
	{
		m_szCivilopediaKey = pClassInfo->getCivilopediaKey();
	}

	if ( getHelpKey() == NULL ||  getHelpKey() == wDefault )
	{
		m_szHelpKey = pClassInfo->getHelpKey();
	}

	if ( getStrategyKey() == NULL || getStrategyKey() == wDefault)
	{
		m_szStrategyKey = pClassInfo->getStrategyKey();
	}

	if ( getButton() == NULL || getButton() == cDefault)
	{
		m_szButton = pClassInfo->getButton();
	}

	if ( getTextKeyWide() == NULL || getTextKeyWide() == wDefault )
	{
		m_szTextKey = pClassInfo->getTextKeyWide();
	}
}

//======================================================================================================
//					CvScalableInfo
//======================================================================================================
bool CvScalableInfo::read(CvXMLLoadUtility* pXML)
{
	pXML->GetOptionalChildXmlValByName(&m_fScale, L"fScale");
	pXML->GetOptionalChildXmlValByName(&m_fInterfaceScale, L"fInterfaceScale", 1.0f);
	return true;
}

void CvScalableInfo::copyNonDefaults(const CvScalableInfo* pClassInfo)
{
	const float fDefault = 0.0f;

	if (getScale() == fDefault) m_fScale = pClassInfo->getScale();
	if (getInterfaceScale() == 1.0f) m_fInterfaceScale = pClassInfo->getInterfaceScale();
}

float CvScalableInfo::getScale() const
{
	return m_fScale;
}

float CvScalableInfo::getInterfaceScale() const
{
	return m_fInterfaceScale;
}

//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHotkeyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHotkeyInfo::CvHotkeyInfo() :
m_iActionInfoIndex(-1),
m_iHotKeyVal(-1),
m_iHotKeyPriority(-1),
m_iHotKeyValAlt(-1),
m_iHotKeyPriorityAlt(-1),
m_iOrderPriority(0),
m_bAltDown(false),
m_bShiftDown(false),
m_bCtrlDown(false),
m_bAltDownAlt(false),
m_bShiftDownAlt(false),
m_bCtrlDownAlt(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHotkeyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHotkeyInfo::~CvHotkeyInfo()
{
}

bool CvHotkeyInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szHotKey, L"HotKey");
	m_iHotKeyVal = pXML->GetHotKeyInt(m_szHotKey);

	pXML->GetOptionalChildXmlValByName(&m_iHotKeyPriority, L"iHotKeyPriority", -1);

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"HotKeyAlt");
	m_iHotKeyValAlt = pXML->GetHotKeyInt(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iHotKeyPriorityAlt, L"iHotKeyPriorityAlt", -1);
	pXML->GetOptionalChildXmlValByName(&m_bAltDown, L"bAltDown");
	pXML->GetOptionalChildXmlValByName(&m_bShiftDown, L"bShiftDown");
	pXML->GetOptionalChildXmlValByName(&m_bCtrlDown, L"bCtrlDown");
	pXML->GetOptionalChildXmlValByName(&m_bAltDownAlt, L"bAltDownAlt");
	pXML->GetOptionalChildXmlValByName(&m_bShiftDownAlt, L"bShiftDownAlt");
	pXML->GetOptionalChildXmlValByName(&m_bCtrlDownAlt, L"bCtrlDownAlt");
	pXML->GetOptionalChildXmlValByName(&m_iOrderPriority, L"iOrderPriority", 0);

	setHotKeyDescription(getTextKeyWide(), NULL, pXML->CreateHotKeyFromDescription(getHotKey(), m_bShiftDown, m_bAltDown, m_bCtrlDown));

	return true;
}

void CvHotkeyInfo::copyNonDefaults(const CvHotkeyInfo* pClassInfo)
{
	CvInfoBase::copyNonDefaults(pClassInfo);

	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	if (getHotKeyVal() == 0)
	{
		m_iHotKeyVal = pClassInfo->getHotKeyVal();
	}
	if (getHotKeyPriority() == -1)
	{
		m_iHotKeyPriority = pClassInfo->getHotKeyPriority();
	}
	if (getHotKeyValAlt() == 0)
	{
		m_iHotKeyValAlt = pClassInfo->getHotKeyValAlt();
	}
	if (getHotKeyPriorityAlt() == -1)
	{
		m_iHotKeyPriorityAlt = pClassInfo->getHotKeyPriorityAlt();
	}
	if (getOrderPriority() == 0)
	{
		m_iOrderPriority = pClassInfo->getOrderPriority();
	}

	if (isAltDown() == false)
	{
		m_bAltDown = pClassInfo->isAltDown();
	}
	if (isShiftDown() == false)
	{
		m_bShiftDown = pClassInfo->isShiftDown();
	}
	if (isCtrlDown() == false)
	{
		m_bCtrlDown = pClassInfo->isCtrlDown();
	}
	if (isAltDownAlt() == false)
	{
		m_bAltDownAlt = pClassInfo->isAltDownAlt();
	}
	if (isShiftDownAlt() == false)
	{
		m_bShiftDownAlt = pClassInfo->isShiftDownAlt();
	}
	if (isCtrlDownAlt() == false)
	{
		m_bCtrlDownAlt = pClassInfo->isCtrlDownAlt();
	}

	if (getHotKey() == cDefault)
	{
		m_szHotKey = pClassInfo->getHotKey();
	}
	if (getHotKeyDescriptionKey() == wDefault)
	{
		m_szHotKeyDescriptionKey = pClassInfo->getHotKeyDescriptionKey();
	}
	if (getHotKeyAltDescriptionKey() == wDefault)
	{
		m_szHotKeyAltDescriptionKey = pClassInfo->getHotKeyAltDescriptionKey();
	}
	if (getHotKeyString() == wDefault)
	{
		m_szHotKeyString = pClassInfo->getHotKeyString();
	}
}

int CvHotkeyInfo::getActionInfoIndex() const
{
	return m_iActionInfoIndex;
}

void CvHotkeyInfo::setActionInfoIndex(int i)
{
	m_iActionInfoIndex = i;
}

int CvHotkeyInfo::getHotKeyVal() const
{
	return m_iHotKeyVal;
}

int CvHotkeyInfo::getHotKeyPriority() const
{
	return m_iHotKeyPriority;
}

int CvHotkeyInfo::getHotKeyValAlt() const
{
	return m_iHotKeyValAlt;
}

int CvHotkeyInfo::getHotKeyPriorityAlt() const
{
	return m_iHotKeyPriorityAlt;
}

int CvHotkeyInfo::getOrderPriority() const
{
	return m_iOrderPriority;
}

bool CvHotkeyInfo::isAltDown() const
{
	return m_bAltDown;
}

bool CvHotkeyInfo::isShiftDown() const
{
	return m_bShiftDown;
}

bool CvHotkeyInfo::isCtrlDown() const
{
	return m_bCtrlDown;
}

bool CvHotkeyInfo::isAltDownAlt() const
{
	return m_bAltDownAlt;
}

bool CvHotkeyInfo::isShiftDownAlt() const
{
	return m_bShiftDownAlt;
}

bool CvHotkeyInfo::isCtrlDownAlt() const
{
	return m_bCtrlDownAlt;
}

const char* CvHotkeyInfo::getHotKey() const
{
	return m_szHotKey;
}

const wchar_t* CvHotkeyInfo::getHotKeyDescriptionKey() const
{
	return m_szHotKeyDescriptionKey;
}

const wchar_t* CvHotkeyInfo::getHotKeyAltDescriptionKey() const
{
	return m_szHotKeyAltDescriptionKey;
}

const wchar_t* CvHotkeyInfo::getHotKeyString() const
{
	return m_szHotKeyString;
}
std::wstring CvHotkeyInfo::getHotKeyDescription() const
{
	CvWString szTemptext;
	if (!m_szHotKeyAltDescriptionKey.empty())
	{
		szTemptext.Format(L"%s (%s)", gDLL->getObjectText(m_szHotKeyAltDescriptionKey, 0).GetCString(), gDLL->getObjectText(m_szHotKeyDescriptionKey, 0).GetCString());
	}
	else
	{
		szTemptext = gDLL->getObjectText(m_szHotKeyDescriptionKey, 0);
	}

	if (!m_szHotKeyString.empty())
	{
		szTemptext += m_szHotKeyString;
	}

	return szTemptext;
}

void CvHotkeyInfo::setHotKeyDescription(const wchar_t* szHotKeyDescKey, const wchar_t* szHotKeyAltDescKey, const wchar_t* szHotKeyString)
{
	m_szHotKeyDescriptionKey = szHotKeyDescKey;
	m_szHotKeyAltDescriptionKey = szHotKeyAltDescKey;
	m_szHotKeyString = szHotKeyString;
}

//======================================================================================================
//					CvSpecialistInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialistInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialistInfo::CvSpecialistInfo() :
 m_iGreatPeopleUnitType(NO_UNIT)
,m_iGreatPeopleRateChange(0)
,m_iMissionType(NO_MISSION)
,m_bVisible(false)
,m_piYieldChange(NULL)
,m_piCommerceChange(NULL)
,m_piFlavorValue(NULL)
,m_iExperience(0)
,m_iHealthPercent(0)
,m_iHappinessPercent(0)
,m_iInsidiousness(0)
,m_iInvestigation(0)
,m_bSlave(false)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialistInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialistInfo::~CvSpecialistInfo()
{
	PROFILE_EXTRA_FUNC();
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piCommerceChange);
	SAFE_DELETE_ARRAY(m_piFlavorValue);

	m_aTechHappinessTypes.removeDelayedResolution();
	m_aTechHealthTypes.removeDelayedResolution();

	for (int i=0; i<(int)m_aUnitCombatExperienceTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitCombatExperienceTypes[i]));
	}

	for (int i=0; i<(int)m_aUnitCombatExperienceTypesNull.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitCombatExperienceTypesNull[i]));
	}

	GC.removeDelayedResolution((int*)&m_iGreatPeopleUnitType);
}

int CvSpecialistInfo::getGreatPeopleUnitType() const
{
	return m_iGreatPeopleUnitType;
}

int CvSpecialistInfo::getGreatPeopleRateChange() const
{
	return m_iGreatPeopleRateChange;
}

int CvSpecialistInfo::getMissionType() const
{
	return m_iMissionType;
}

void CvSpecialistInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}

bool CvSpecialistInfo::isVisible() const
{
	return m_bVisible;
}

int CvSpecialistInfo::getExperience() const
{
	return m_iExperience;
}

// Arrays

int CvSpecialistInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

const int* CvSpecialistInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}

int CvSpecialistInfo::getCommerceChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceChange ? m_piCommerceChange[i] : 0;
}

int CvSpecialistInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

const char* CvSpecialistInfo::getTexture() const
{
	return m_szTexture;
}

int CvSpecialistInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}
int CvSpecialistInfo::getHappinessPercent() const
{
	return m_iHappinessPercent;
}

bool CvSpecialistInfo::isSlave() const
{
	return m_bSlave;
}

//TB Specialist Tags
int CvSpecialistInfo::getInsidiousness() const
{
	return m_iInsidiousness;
}

int CvSpecialistInfo::getInvestigation() const
{
	return m_iInvestigation;
}

//int CvSpecialistInfo::getNumTechHappinessTypes() const
//{
//	return (int)m_aTechHappinessTypes.size();
//}

int CvSpecialistInfo::getTechHappiness(TechTypes eTech) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);
	return m_aTechHappinessTypes.getValue(eTech);
}

//int CvSpecialistInfo::getNumTechHealthTypes() const
//{
//	return (int)m_aTechHealthTypes.size();
//}

int CvSpecialistInfo::getTechHealth(TechTypes eTech) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);
	return m_aTechHealthTypes.getValue(eTech);
}

int CvSpecialistInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvSpecialistInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvSpecialistInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

int CvSpecialistInfo::getNumUnitCombatExperienceTypes() const
{
	return (int)m_aUnitCombatExperienceTypes.size();
}

const UnitCombatModifier& CvSpecialistInfo::getUnitCombatExperienceType(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, (int)m_aUnitCombatExperienceTypes.size(), iUnitCombat);
	FASSERT_BOUNDS(0, (int)m_aUnitCombatExperienceTypesNull.size(), iUnitCombat);

	if (!GC.getGame().isOption(GAMEOPTION_UNIT_XP_FROM_SPECIALISTS) && isVisible())
	{
		return m_aUnitCombatExperienceTypesNull[iUnitCombat];
	}
	return m_aUnitCombatExperienceTypes[iUnitCombat];
}


// read from xml
//
bool CvSpecialistInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szTexture, L"Texture");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GreatPeopleUnitType");
	GC.addDelayedResolution((int*)&m_iGreatPeopleUnitType, szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateChange, L"iGreatPeopleRateChange");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	if (pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"Commerces"))
	{
		pXML->SetCommerce(&m_piCommerceChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceChange);
	}

	pXML->GetOptionalChildXmlValByName(&m_iExperience, L"iExperience");

	m_PropertyManipulators.read(pXML);

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());

	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iHappinessPercent, L"iHappinessPercent");
	pXML->GetOptionalChildXmlValByName(&m_bSlave, L"bSlave");

	//TB Specialist Tags
	pXML->GetOptionalChildXmlValByName(&m_iInsidiousness, L"iInsidiousness");
	pXML->GetOptionalChildXmlValByName(&m_iInvestigation, L"iInvestigation");

	m_aTechHappinessTypes.readWithDelayedResolution(pXML, L"TechHappinessTypes");
	m_aTechHealthTypes.readWithDelayedResolution(pXML, L"TechHealthTypes");

	if(pXML->TryMoveToXmlFirstChild(L"UnitCombatExperienceTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitCombatExperienceType" );
		m_aUnitCombatExperienceTypes.resize(iNum); // Important to keep the delayed resolution pointers correct
		m_aUnitCombatExperienceTypesNull.resize(iNum);

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitCombatExperienceType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aUnitCombatExperienceTypes[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aUnitCombatExperienceTypes[i].eUnitCombat), szTextVal);
					GC.addDelayedResolution((int*)&(m_aUnitCombatExperienceTypesNull[i].eUnitCombat), szTextVal);
					m_aUnitCombatExperienceTypesNull[i].iModifier = 0;
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitCombatExperienceType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
	return true;
}

void CvSpecialistInfo::copyNonDefaults(const CvSpecialistInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getTexture() == cDefault) m_szTexture = pClassInfo->getTexture();
	if (isVisible() == bDefault) m_bVisible = pClassInfo->isVisible();
	GC.copyNonDefaultDelayedResolution((int*)&m_iGreatPeopleUnitType, (int*)&pClassInfo->m_iGreatPeopleUnitType);
	if (getGreatPeopleRateChange() == iDefault) m_iGreatPeopleRateChange = pClassInfo->getGreatPeopleRateChange();

	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
	}

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( getCommerceChange(i) == iDefault ) m_piCommerceChange[i] = pClassInfo->getCommerceChange(i);
	}

	if (getExperience() == iDefault) m_iExperience = pClassInfo->getExperience();

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++ )
	{
		if ( getFlavorValue(i) == iDefault && pClassInfo->getFlavorValue(i) != iDefault )
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[i] = pClassInfo->getFlavorValue(i);
		}
	}

	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
	if (getHappinessPercent() == iDefault) m_iHappinessPercent = pClassInfo->getHappinessPercent();
	if (isSlave() == bDefault) m_bSlave = pClassInfo->isSlave();

	//TB Specialist Tags
	if (getInsidiousness() == iDefault) m_iInsidiousness = pClassInfo->getInsidiousness();
	if (getInvestigation() == iDefault) m_iInvestigation = pClassInfo->getInvestigation();

	m_aTechHappinessTypes.copyNonDefaultDelayedResolution(pClassInfo->getTechHappinessTypes());
	m_aTechHealthTypes.copyNonDefaultDelayedResolution(pClassInfo->getTechHealthTypes());

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	if (getNumUnitCombatExperienceTypes() == 0)
	{
		const int iNum = pClassInfo->getNumUnitCombatExperienceTypes();
		m_aUnitCombatExperienceTypes.resize(iNum);
		m_aUnitCombatExperienceTypesNull.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aUnitCombatExperienceTypes[i].iModifier = pClassInfo->m_aUnitCombatExperienceTypes[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aUnitCombatExperienceTypes[i].eUnitCombat), (int*)&(pClassInfo->m_aUnitCombatExperienceTypes[i].eUnitCombat));
			m_aUnitCombatExperienceTypesNull[i].iModifier = 0;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aUnitCombatExperienceTypesNull[i].eUnitCombat), (int*)&(pClassInfo->m_aUnitCombatExperienceTypesNull[i].eUnitCombat));
		}
	}
}

void CvSpecialistInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iGreatPeopleUnitType);
	CheckSum(iSum, m_iGreatPeopleRateChange);
	CheckSum(iSum, m_iMissionType);
	CheckSum(iSum, m_iHealthPercent);
	CheckSum(iSum, m_iHappinessPercent);
	CheckSum(iSum, m_bSlave);
	CheckSum(iSum, m_iExperience);
	m_PropertyManipulators.getCheckSum(iSum);
	CheckSum(iSum, m_bVisible);
	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piCommerceChange, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());
	//Team Project (1)
	//TB Specialist Tags
	// int
	CheckSum(iSum, m_iInsidiousness);
	CheckSum(iSum, m_iInvestigation);
	CheckSumC(iSum, m_aTechHappinessTypes);
	CheckSumC(iSum, m_aTechHealthTypes);
	CheckSumC(iSum, m_aiCategories);

	int iNumElements = m_aUnitCombatExperienceTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatExperienceTypes[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatExperienceTypes[i].iModifier);
	}

	iNumElements = m_aUnitCombatExperienceTypesNull.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatExperienceTypesNull[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatExperienceTypesNull[i].iModifier);
	}

}

//======================================================================================================
//					CvTechInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTechInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTechInfo::CvTechInfo() :
m_iAdvisorType(NO_ADVISOR),
m_iAIWeight(0),
m_iAITradeModifier(0),
m_iResearchCost(-1),
m_iEra(NO_ERA),
m_iTradeRoutes(0),
m_iFeatureProductionModifier(0),
m_iWorkerSpeedModifier(0),
//DPII < Maintenance Modifier >
m_iMaintenanceModifier(0),
m_iDistanceMaintenanceModifier(0),
m_iNumCitiesMaintenanceModifier(0),
m_iCoastalDistanceMaintenanceModifier(0),
//DPII < Maintenance Modifier >
m_iFirstFreeUnit(NO_UNIT),
m_iFirstFreeProphet(NO_UNIT),
m_iHealth(0),
m_iHappiness(0),
m_iFirstFreeTechs(0),
m_iAssetValue(0),
m_iPowerValue(0),
m_iGridX(0),
m_iGridY(0),
m_bRepeat(false),
m_bTrade(false),
m_bDisable(false),
m_bGoodyTech(false),
m_bExtraWaterSeeFrom(false),
m_bMapCentering(false),
m_bMapVisible(false),
m_bMapTrading(false),
m_bTechTrading(false),
m_bGoldTrading(false),
m_bOpenBordersTrading(false),
m_bDefensivePactTrading(false),
m_bPermanentAllianceTrading(false),
m_bVassalStateTrading(false),
m_bBridgeBuilding(false),
m_bIrrigation(false),
m_bIgnoreIrrigation(false),
m_bWaterWork(false),
m_bRiverTrade(false),
m_bLanguage(false),
// Dale - AB: Bombing START
m_bDCMAirBombTech1(0),
m_bDCMAirBombTech2(0),
// Dale - AB: Bombing END
m_piDomainExtraMoves(NULL),
m_piFlavorValue(NULL),
m_pbCommerceFlexible(NULL),
m_pbTerrainTrade(NULL),
//ls612: Tech Commerce Modifiers
m_piCommerceModifier(NULL)
,m_bEmbassyTrading(false)
,m_bCanPassPeaks(false)
,m_bMoveFastPeaks(false)
,m_bCanFoundOnPeaks(false)
,m_bEnableDarkAges(false)
,m_bRebaseAnywhere(false)
,m_bEnablesDesertFarming(false)
,m_iInflationModifier(0)
,m_iGlobalTradeModifier(0)
,m_iGlobalForeignTradeModifier(0)
,m_iTradeMissionModifier(0)
,m_iCorporationRevenueModifier(0)
,m_iCorporationMaintenanceModifier(0)
,m_iPrereqGameOption(NO_GAMEOPTION)
,m_piFreeSpecialistCount(NULL)
,m_bGlobal(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTechInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTechInfo::~CvTechInfo()
{
	SAFE_DELETE_ARRAY(m_piDomainExtraMoves);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_pbCommerceFlexible);
	SAFE_DELETE_ARRAY(m_pbTerrainTrade);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	GC.removeDelayedResolutionVector(m_piPrereqOrTechs);
	GC.removeDelayedResolutionVector(m_piPrereqAndTechs);
	GC.removeDelayedResolutionVector(m_aPrereqBuilding);
	GC.removeDelayedResolutionVector(m_aPrereqOrBuilding);
	GC.removeDelayedResolution((int*)&m_iFirstFreeUnit);
	GC.removeDelayedResolution((int*)&m_iFirstFreeProphet);
}


int CvTechInfo::getAdvisorType() const
{
	return m_iAdvisorType;
}

int CvTechInfo::getAIWeight() const
{
	return m_iAIWeight;
}

int CvTechInfo::getAITradeModifier() const
{
	return m_iAITradeModifier;
}

int CvTechInfo::getResearchCost() const
{
	return m_iResearchCost;
}

int CvTechInfo::getEra() const
{
	return m_iEra;
}

int CvTechInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}

int CvTechInfo::getFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}

int CvTechInfo::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}
//DPII < Maintenance Modifier >
int CvTechInfo::getMaintenanceModifier() const
{
	return m_iMaintenanceModifier;
}

int CvTechInfo::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}

int CvTechInfo::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;
}

int CvTechInfo::getCoastalDistanceMaintenanceModifier() const
{
	return m_iCoastalDistanceMaintenanceModifier;
}
//DPII < Maintenance Modifier >
int CvTechInfo::getFirstFreeUnit() const
{
	return m_iFirstFreeUnit;
}

int CvTechInfo::getFirstFreeProphet() const
{
	return m_iFirstFreeProphet;
}

int CvTechInfo::getHealth() const
{
	return m_iHealth;
}

int CvTechInfo::getHappiness() const
{
	return m_iHappiness;
}

int CvTechInfo::getFirstFreeTechs() const
{
	return m_iFirstFreeTechs;
}

int CvTechInfo::getAssetValue() const
{
	return m_iAssetValue * 100;
}

int CvTechInfo::getPowerValue() const
{
	return m_iPowerValue * 100;
}

int CvTechInfo::getGridX() const
{
	return m_iGridX;
}

int CvTechInfo::getGridY() const
{
	return m_iGridY;
}

bool CvTechInfo::isRepeat() const
{
	return m_bRepeat;
}

bool CvTechInfo::isTrade() const
{
	return m_bTrade;
}

bool CvTechInfo::isDisable() const
{
	return m_bDisable;
}

bool CvTechInfo::isGoodyTech() const
{
	return m_bGoodyTech;
}

bool CvTechInfo::isExtraWaterSeeFrom() const
{
	return m_bExtraWaterSeeFrom;
}

bool CvTechInfo::isMapCentering() const
{
	return m_bMapCentering;
}

bool CvTechInfo::isMapVisible() const
{
	return m_bMapVisible;
}

bool CvTechInfo::isMapTrading() const
{
	return m_bMapTrading;
}

bool CvTechInfo::isTechTrading() const
{
	return m_bTechTrading;
}

bool CvTechInfo::isGoldTrading() const
{
	return m_bGoldTrading;
}

bool CvTechInfo::isOpenBordersTrading() const
{
	return m_bOpenBordersTrading;
}

bool CvTechInfo::isDefensivePactTrading() const
{
	return m_bDefensivePactTrading;
}

bool CvTechInfo::isPermanentAllianceTrading() const
{
	return m_bPermanentAllianceTrading;
}

bool CvTechInfo::isVassalStateTrading() const
{
	return m_bVassalStateTrading;
}

bool CvTechInfo::isBridgeBuilding() const
{
	return m_bBridgeBuilding;
}

bool CvTechInfo::isIrrigation() const
{
	return m_bIrrigation;
}

bool CvTechInfo::isIgnoreIrrigation() const
{
	return m_bIgnoreIrrigation;
}

bool CvTechInfo::isWaterWork() const
{
	return m_bWaterWork;
}

bool CvTechInfo::isRiverTrade() const
{
	return m_bRiverTrade;
}

bool CvTechInfo::getDCMAirBombTech1() const
{
	return m_bDCMAirBombTech1;
}

bool CvTechInfo::getDCMAirBombTech2() const
{
	return m_bDCMAirBombTech2;
}

std::wstring CvTechInfo::getQuote()	const
{
	CvWString text = gDLL->getText(m_szQuoteKey);
	FAssert(!text.empty());
	return text;
}

const char* CvTechInfo::getQuoteKey() const
{
	return m_szQuoteKey;
}

const char* CvTechInfo::getSound() const
{
	return m_szSound;
}

const char* CvTechInfo::getSoundMP() const
{
	return m_szSoundMP;
}

// Arrays

int CvTechInfo::getDomainExtraMoves(int i) const
{
	return m_piDomainExtraMoves ? m_piDomainExtraMoves[i] : 0;
}

int CvTechInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

const std::vector<TechTypes>& CvTechInfo::getPrereqOrTechs() const
{
	return m_piPrereqOrTechs;
}

const std::vector<TechTypes>& CvTechInfo::getPrereqAndTechs() const
{
	return m_piPrereqAndTechs;
}

bool CvTechInfo::isCommerceFlexible(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_pbCommerceFlexible ? m_pbCommerceFlexible[i] : false;
}

bool CvTechInfo::isTerrainTrade(int i) const
{
	return m_pbTerrainTrade ? m_pbTerrainTrade[i] : false;
}

//ls612: Tech Commerce Modifiers
int CvTechInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int* CvTechInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}

bool CvTechInfo::isEmbassyTrading() const
{
	return m_bEmbassyTrading;
}

bool CvTechInfo::isEnablesDesertFarming() const
{
	return m_bEnablesDesertFarming;
}

bool CvTechInfo::isCanPassPeaks() const
{
	return m_bCanPassPeaks;
}

bool CvTechInfo::isMoveFastPeaks() const
{
	return m_bMoveFastPeaks;
}

bool CvTechInfo::isCanFoundOnPeaks() const
{
	return m_bCanFoundOnPeaks;
}
bool CvTechInfo::isEnableDarkAges() const
{
	return m_bEnableDarkAges;
}
bool CvTechInfo::isRebaseAnywhere() const
{
	return m_bRebaseAnywhere;
}
int CvTechInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}
int CvTechInfo::getGlobalTradeModifier() const
{
	return m_iGlobalTradeModifier;
}
int CvTechInfo::getGlobalForeignTradeModifier() const
{
	return  m_iGlobalForeignTradeModifier;
}
int CvTechInfo::getTradeMissionModifier() const
{
	return m_iTradeMissionModifier;
}
int CvTechInfo::getCorporationRevenueModifier() const
{
	return m_iCorporationRevenueModifier;
}
int CvTechInfo::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}
int CvTechInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}
int CvTechInfo::getFreeSpecialistCount(int i) const
{
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}

int CvTechInfo::getNumPrereqBuildings() const
{
	return m_aPrereqBuilding.size();
}

const PrereqBuilding& CvTechInfo::getPrereqBuilding(int iIndex) const
{
	return m_aPrereqBuilding[iIndex];
}

int CvTechInfo::getPrereqBuildingType(int iIndex) const
{
	return (int)m_aPrereqBuilding[iIndex].eBuilding;
}

int CvTechInfo::getPrereqBuildingMinimumRequired(int iIndex) const
{
	return m_aPrereqBuilding[iIndex].iMinimumRequired;
}

int CvTechInfo::getNumPrereqOrBuildings() const
{
	return m_aPrereqOrBuilding.size();
}

const PrereqBuilding& CvTechInfo::getPrereqOrBuilding(int iIndex) const
{
	return m_aPrereqOrBuilding[iIndex];
}

int CvTechInfo::getPrereqOrBuildingType(int iIndex) const
{
	return (int)m_aPrereqOrBuilding[iIndex].eBuilding;
}

int CvTechInfo::getPrereqOrBuildingMinimumRequired(int iIndex) const
{
	return m_aPrereqOrBuilding[iIndex].iMinimumRequired;
}

int CvTechInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvTechInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvTechInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

bool CvTechInfo::isGlobal() const
{
	return m_bGlobal;
}

bool CvTechInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Advisor");
	m_iAdvisorType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iAIWeight, L"iAIWeight");
	pXML->GetOptionalChildXmlValByName(&m_iAITradeModifier, L"iAITradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iResearchCost, L"iCost");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Era");
	m_iEra = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FirstFreeUnit");
	GC.addDelayedResolution((int*)&m_iFirstFreeUnit, szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FirstFreeProphet");
	GC.addDelayedResolution((int*)&m_iFirstFreeProphet, szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iFeatureProductionModifier, L"iFeatureProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorkerSpeedModifier, L"iWorkerSpeedModifier");
	//DPII < Maintenance Modifiers >
	pXML->GetOptionalChildXmlValByName(&m_iMaintenanceModifier, L"iMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCoastalDistanceMaintenanceModifier, L"iCoastalDistanceMaintenanceModifier");
	//DPII < Maintenance Modifiers >
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iFirstFreeTechs, L"iFirstFreeTechs");
	pXML->GetOptionalChildXmlValByName(&m_iAssetValue, L"iAsset");
	pXML->GetOptionalChildXmlValByName(&m_iPowerValue, L"iPower");
	pXML->GetOptionalChildXmlValByName(&m_bRepeat, L"bRepeat");
	pXML->GetOptionalChildXmlValByName(&m_bTrade, L"bTrade");
	pXML->GetOptionalChildXmlValByName(&m_bDisable, L"bDisable");
	pXML->GetOptionalChildXmlValByName(&m_bGoodyTech, L"bGoodyTech");
	pXML->GetOptionalChildXmlValByName(&m_bExtraWaterSeeFrom, L"bExtraWaterSeeFrom");
	pXML->GetOptionalChildXmlValByName(&m_bMapCentering, L"bMapCentering");
	pXML->GetOptionalChildXmlValByName(&m_bMapVisible, L"bMapVisible");
	pXML->GetOptionalChildXmlValByName(&m_bMapTrading, L"bMapTrading");
	pXML->GetOptionalChildXmlValByName(&m_bTechTrading, L"bTechTrading");
	pXML->GetOptionalChildXmlValByName(&m_bGoldTrading, L"bGoldTrading");
	pXML->GetOptionalChildXmlValByName(&m_bOpenBordersTrading, L"bOpenBordersTrading");
	pXML->GetOptionalChildXmlValByName(&m_bDefensivePactTrading, L"bDefensivePactTrading");
	pXML->GetOptionalChildXmlValByName(&m_bPermanentAllianceTrading, L"bPermanentAllianceTrading");
	pXML->GetOptionalChildXmlValByName(&m_bVassalStateTrading, L"bVassalTrading");
	pXML->GetOptionalChildXmlValByName(&m_bBridgeBuilding, L"bBridgeBuilding");
	pXML->GetOptionalChildXmlValByName(&m_bIrrigation, L"bIrrigation");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreIrrigation, L"bIgnoreIrrigation");
	pXML->GetOptionalChildXmlValByName(&m_bWaterWork, L"bWaterWork");
	pXML->GetOptionalChildXmlValByName(&m_bRiverTrade, L"bRiverTrade");
	pXML->GetOptionalChildXmlValByName(&m_bLanguage, L"bLanguage");
	pXML->GetOptionalChildXmlValByName(&m_iGridX, L"iGridX");
	pXML->GetOptionalChildXmlValByName(&m_iGridY, L"iGridY");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBombTech1, L"bDCMAirBombTech1");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBombTech2, L"bDCMAirBombTech2");

	if (pXML->TryMoveToXmlFirstChild(L"CommerceFlexible"))
	{
		pXML->SetCommerce(&m_pbCommerceFlexible);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_pbCommerceFlexible);
	}

	pXML->SetVariableListTagPair(&m_piDomainExtraMoves, L"DomainExtraMoves", NUM_DOMAIN_TYPES);
	pXML->SetVariableListTagPair(&m_pbTerrainTrade, L"TerrainTrades", GC.getNumTerrainInfos(), false);
	//ls612: Tech Commerce Modifiers
	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());

	pXML->GetOptionalChildXmlValByName(m_szQuoteKey, L"Quote");
	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");
	pXML->GetOptionalChildXmlValByName(m_szSoundMP, L"SoundMP");

	pXML->GetOptionalChildXmlValByName(&m_bEmbassyTrading, L"bEmbassyTrading");
	pXML->GetOptionalChildXmlValByName(&m_bCanPassPeaks, L"bCanPassPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bMoveFastPeaks, L"bMoveFastPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bCanFoundOnPeaks, L"bCanFoundOnPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bEnableDarkAges, L"bEnableDarkAges");
	pXML->GetOptionalChildXmlValByName(&m_bRebaseAnywhere, L"bRebaseAnywhere");
	pXML->GetOptionalChildXmlValByName(&m_bEnablesDesertFarming, L"bAllowsDesertFarming");

	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalTradeModifier, L"iGlobalTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalForeignTradeModifier, L"iGlobalForeignTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTradeMissionModifier, L"iTradeMissionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationRevenueModifier, L"iCorporationRevenueModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenanceModifier, L"iCorporationMaintenanceModifier");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, L"FreeSpecialistCounts", GC.getNumSpecialistInfos());

	if(pXML->TryMoveToXmlFirstChild(L"PrereqBuildings"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"PrereqBuilding" );
		m_aPrereqBuilding.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PrereqBuilding"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					pXML->GetChildXmlValByName(&(m_aPrereqBuilding[i].iMinimumRequired), L"iNumBuildingNeeded");
					GC.addDelayedResolution((int*)&(m_aPrereqBuilding[i].eBuilding), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PrereqBuilding"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PrereqOrBuildings"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"PrereqOrBuilding" );
		m_aPrereqOrBuilding.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PrereqOrBuilding"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					pXML->GetChildXmlValByName(&(m_aPrereqOrBuilding[i].iMinimumRequired), L"iNumBuildingNeeded");
					GC.addDelayedResolution((int*)&(m_aPrereqOrBuilding[i].eBuilding), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PrereqOrBuilding"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//TB Tech Tags
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	//TB Tech Tags end

	pXML->SetOptionalVectorWithDelayedResolution(m_piPrereqOrTechs, L"OrPreReqs");
	pXML->SetOptionalVectorWithDelayedResolution(m_piPrereqAndTechs, L"AndPreReqs");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	return true;
}

void CvTechInfo::copyNonDefaults(const CvTechInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvInfoBase::copyNonDefaults(pClassInfo);

	const bool bDefault = false;
	const int iDefault = 0;
	const int iTextDefault = -1;
	const CvString cDefault = CvString::format("").GetCString();

	if (m_iAdvisorType == NO_ADVISOR) m_iAdvisorType = pClassInfo->getAdvisorType();
	if (getAIWeight() == iDefault) m_iAIWeight = pClassInfo->getAIWeight();
	if (getAITradeModifier() == iDefault) m_iAITradeModifier = pClassInfo->getAITradeModifier();
	if (m_iResearchCost == iDefault) m_iResearchCost = pClassInfo->m_iResearchCost;
	if (m_iEra == NO_ERA) m_iEra = pClassInfo->getEra();
	GC.copyNonDefaultDelayedResolution((int*)&m_iFirstFreeUnit, (int*)&pClassInfo->m_iFirstFreeUnit);
	GC.copyNonDefaultDelayedResolution((int*)&m_iFirstFreeProphet, (int*)&pClassInfo->m_iFirstFreeProphet);
	if (getFeatureProductionModifier() == iDefault) m_iFeatureProductionModifier = pClassInfo->getFeatureProductionModifier();
	if (getWorkerSpeedModifier() == iDefault) m_iWorkerSpeedModifier = pClassInfo->getWorkerSpeedModifier();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getFirstFreeTechs() == iDefault) m_iFirstFreeTechs = pClassInfo->getFirstFreeTechs();
	if (m_iAssetValue == iDefault) m_iAssetValue = pClassInfo->m_iAssetValue;
	if (m_iPowerValue == iDefault) m_iPowerValue = pClassInfo->m_iPowerValue;

	if (isRepeat() == bDefault) m_bRepeat = pClassInfo->isRepeat();
	if (isTrade() == bDefault) m_bTrade = pClassInfo->isTrade();
	if (isDisable() == bDefault) m_bDisable = pClassInfo->isDisable();
	if (isGoodyTech() == bDefault) m_bGoodyTech = pClassInfo->isGoodyTech();
	if (isExtraWaterSeeFrom() == bDefault) m_bExtraWaterSeeFrom = pClassInfo->isExtraWaterSeeFrom();
	if (isMapCentering() == bDefault) m_bMapCentering = pClassInfo->isMapCentering();
	if (isMapVisible() == bDefault) m_bMapVisible = pClassInfo->isMapVisible();
	if (isMapTrading() == bDefault) m_bMapTrading = pClassInfo->isMapTrading();
	if (isTechTrading() == bDefault) m_bTechTrading = pClassInfo->isTechTrading();
	if (isGoldTrading() == bDefault) m_bGoldTrading = pClassInfo->isGoldTrading();
	if (isOpenBordersTrading() == bDefault) m_bOpenBordersTrading = pClassInfo->isOpenBordersTrading();
	if (isDefensivePactTrading() == bDefault) m_bDefensivePactTrading = pClassInfo->isDefensivePactTrading();
	if (isPermanentAllianceTrading() == bDefault) m_bPermanentAllianceTrading = pClassInfo->isPermanentAllianceTrading();
	if (isVassalStateTrading() == bDefault) m_bVassalStateTrading = pClassInfo->isVassalStateTrading();
	if (isBridgeBuilding() == bDefault) m_bBridgeBuilding = pClassInfo->isBridgeBuilding();
	if (isIrrigation() == bDefault) m_bIrrigation = pClassInfo->isIrrigation();
	if (isIgnoreIrrigation() == bDefault) m_bIgnoreIrrigation = pClassInfo->isIgnoreIrrigation();
	if (isWaterWork() == bDefault) m_bWaterWork = pClassInfo->isWaterWork();
	if (isRiverTrade() == bDefault) m_bRiverTrade = pClassInfo->isRiverTrade();
	if (m_bLanguage == bDefault) m_bLanguage = pClassInfo->isLanguage();

	if (getGridX() == iDefault) m_iGridX = pClassInfo->getGridX();
	if (getGridY() == iDefault) m_iGridY = pClassInfo->getGridY();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if (isCommerceFlexible(j) == bDefault && pClassInfo->isCommerceFlexible(j) != bDefault)
		{
			if ( NULL == m_pbCommerceFlexible )
			{
				CvXMLLoadUtility::InitList(&m_pbCommerceFlexible,NUM_COMMERCE_TYPES,bDefault);
			}
			m_pbCommerceFlexible[j] = pClassInfo->isCommerceFlexible(j);
		}
	}
	for ( int j = 0; j < NUM_DOMAIN_TYPES; j++)
	{
		if ((m_piDomainExtraMoves == NULL || m_piDomainExtraMoves[j] == iDefault) &&
			pClassInfo->getDomainExtraMoves(j) != iDefault)
		{
			if ( m_piDomainExtraMoves == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piDomainExtraMoves,NUM_DOMAIN_TYPES,iDefault);
			}
			m_piDomainExtraMoves[j] = pClassInfo->getDomainExtraMoves(j);
		}
	}
	for ( int j = 0; j < GC.getNumTerrainInfos(); j++)
	{
		if ((m_pbTerrainTrade == NULL || m_pbTerrainTrade[j] == bDefault) &&
			pClassInfo->isTerrainTrade(j) != bDefault)
		{
			if ( m_pbTerrainTrade == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrainTrade,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbTerrainTrade[j] = pClassInfo->isTerrainTrade(j);
		}
	}
	//ls612: Tech Commerce Modifiers
	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( getCommerceModifier(j) == iDefault && pClassInfo->getCommerceModifier(j) != iDefault)
		{
			if ( NULL == m_piCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
	}
	for ( int j = 0; j < GC.getNumFlavorTypes(); j++)
	{
		if ((m_piFlavorValue == NULL || m_piFlavorValue[j] == iDefault) &&
			pClassInfo->getFlavorValue(j) != iDefault)
		{
			if ( m_piFlavorValue == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[j] = pClassInfo->getFlavorValue(j);
		}
	}

	if (getQuoteKey() == cDefault) m_szQuoteKey = pClassInfo->getQuoteKey();
	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();
	if (getSoundMP() == cDefault) m_szSoundMP = pClassInfo->getSoundMP();
	//TB Tech Tags
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();
	//TB Tech Tags end
	if (!isEmbassyTrading()) m_bEmbassyTrading = pClassInfo->isEmbassyTrading();
	if (!isCanPassPeaks()) m_bCanPassPeaks = pClassInfo->isCanPassPeaks();
	if (!isMoveFastPeaks()) m_bMoveFastPeaks = pClassInfo->isMoveFastPeaks();
	if (!isCanFoundOnPeaks()) m_bCanFoundOnPeaks = pClassInfo->isCanFoundOnPeaks();
	if (!isEnableDarkAges()) m_bEnableDarkAges = pClassInfo->isEnableDarkAges();
	if (!isRebaseAnywhere()) m_bRebaseAnywhere = pClassInfo->isRebaseAnywhere();
	if (!isEnablesDesertFarming()) m_bEnablesDesertFarming = pClassInfo->isEnablesDesertFarming();

	if (m_iInflationModifier == 0) m_iInflationModifier = pClassInfo->getInflationModifier();
	if (m_iGlobalTradeModifier == 0) m_iGlobalTradeModifier = pClassInfo->getGlobalTradeModifier();
	if (m_iGlobalForeignTradeModifier == 0) m_iGlobalForeignTradeModifier = pClassInfo->getGlobalForeignTradeModifier();
	if (m_iTradeMissionModifier == 0) m_iTradeMissionModifier = pClassInfo->getTradeMissionModifier();
	if (m_iCorporationRevenueModifier == 0) m_iCorporationRevenueModifier = pClassInfo->getCorporationRevenueModifier();
	if (m_iCorporationMaintenanceModifier == 0) m_iCorporationMaintenanceModifier = pClassInfo->getCorporationMaintenanceModifier();
	if (m_iPrereqGameOption == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();

	if (!m_iMaintenanceModifier) m_iMaintenanceModifier = pClassInfo->getMaintenanceModifier();
	if (!m_iDistanceMaintenanceModifier) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (!m_iNumCitiesMaintenanceModifier) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (!m_iCoastalDistanceMaintenanceModifier) m_iCoastalDistanceMaintenanceModifier = pClassInfo->getCoastalDistanceMaintenanceModifier();

	for ( int j = 0; j < GC.getNumSpecialistInfos(); j++)
	{
		if ((m_piFreeSpecialistCount == NULL ||m_piFreeSpecialistCount[j] == 0) &&
			pClassInfo->getFreeSpecialistCount(j) != 0)
		{
			if ( m_piFreeSpecialistCount == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFreeSpecialistCount,GC.getNumSpecialistInfos(),0);
			}
			m_piFreeSpecialistCount[j] = pClassInfo->getFreeSpecialistCount(j);
		}
	}

	if (getNumPrereqBuildings() == 0)
	{
		const int iNum = pClassInfo->getNumPrereqBuildings();
		m_aPrereqBuilding.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPrereqBuilding[i].iMinimumRequired = pClassInfo->m_aPrereqBuilding[i].iMinimumRequired;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPrereqBuilding[i].eBuilding), (int*)&(pClassInfo->m_aPrereqBuilding[i].eBuilding));
		}
	}

	if (getNumPrereqOrBuildings() == 0)
	{
		const int iNum = pClassInfo->getNumPrereqOrBuildings();
		m_aPrereqOrBuilding.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPrereqOrBuilding[i].iMinimumRequired = pClassInfo->m_aPrereqOrBuilding[i].iMinimumRequired;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPrereqOrBuilding[i].eBuilding), (int*)&(pClassInfo->m_aPrereqOrBuilding[i].eBuilding));
		}
	}
	GC.copyNonDefaultDelayedResolutionVector(m_piPrereqOrTechs, pClassInfo->getPrereqOrTechs());
	GC.copyNonDefaultDelayedResolutionVector(m_piPrereqAndTechs, pClassInfo->getPrereqAndTechs());
}

void CvTechInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iAdvisorType);
	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_iAITradeModifier);
	CheckSum(iSum, m_iResearchCost);
	CheckSum(iSum, m_iEra);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iFeatureProductionModifier);
	CheckSum(iSum, m_iWorkerSpeedModifier);
	CheckSum(iSum, m_iMaintenanceModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iCoastalDistanceMaintenanceModifier);
	CheckSum(iSum, m_iFirstFreeUnit);
	CheckSum(iSum, m_iFirstFreeProphet);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iFirstFreeTechs);
	CheckSum(iSum, m_iAssetValue);
	CheckSum(iSum, m_iPowerValue);

	CheckSum(iSum, m_bRepeat);
	CheckSum(iSum, m_bTrade);
	CheckSum(iSum, m_bDisable);
	CheckSum(iSum, m_bGoodyTech);
	CheckSum(iSum, m_bExtraWaterSeeFrom);
	CheckSum(iSum, m_bMapCentering);
	CheckSum(iSum, m_bMapVisible);
	CheckSum(iSum, m_bMapTrading);
	CheckSum(iSum, m_bTechTrading);
	CheckSum(iSum, m_bGoldTrading);
	CheckSum(iSum, m_bOpenBordersTrading);
	CheckSum(iSum, m_bDefensivePactTrading);
	CheckSum(iSum, m_bPermanentAllianceTrading);
	CheckSum(iSum, m_bVassalStateTrading);
	CheckSum(iSum, m_bBridgeBuilding);
	CheckSum(iSum, m_bIrrigation);
	CheckSum(iSum, m_bIgnoreIrrigation);
	CheckSum(iSum, m_bWaterWork);
	CheckSum(iSum, m_bRiverTrade);
	CheckSum(iSum, m_bLanguage);

	CheckSum(iSum, m_piDomainExtraMoves, NUM_DOMAIN_TYPES);
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());
	CheckSumC(iSum, m_piPrereqOrTechs);
	CheckSumC(iSum, m_piPrereqAndTechs);
	CheckSum(iSum, m_pbCommerceFlexible, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_pbTerrainTrade, GC.getNumTerrainInfos());
	//ls612: Tech Commerce Modifiers
	CheckSum(iSum, m_piCommerceModifier, NUM_COMMERCE_TYPES);

	CheckSum(iSum, m_bCanPassPeaks);
	CheckSum(iSum, m_bMoveFastPeaks);
	CheckSum(iSum, m_bCanFoundOnPeaks);
	CheckSum(iSum, m_bEmbassyTrading);
	CheckSum(iSum, m_bEnableDarkAges);
	CheckSum(iSum, m_bRebaseAnywhere);
	CheckSum(iSum, m_bEnablesDesertFarming);

	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_iGlobalTradeModifier);
	CheckSum(iSum, m_iGlobalForeignTradeModifier);
	CheckSum(iSum, m_iTradeMissionModifier);
	CheckSum(iSum, m_iCorporationRevenueModifier);
	CheckSum(iSum, m_iCorporationMaintenanceModifier);
	CheckSum(iSum, m_iPrereqGameOption);

	CheckSum(iSum, m_piFreeSpecialistCount, GC.getNumSpecialistInfos());
	CheckSumC(iSum, m_aiCategories);

	const int iNumElements = m_aPrereqBuilding.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPrereqBuilding[i].eBuilding);
		CheckSum(iSum, m_aPrereqBuilding[i].iMinimumRequired);
	}
	//TB Tech Tags
	CheckSum(iSum, m_bGlobal);
	//TB Tech Tags end
}

// Toffer - Derived tech cache
void CvTechInfo::setLeadsTo(const TechTypes eTech)
{
	m_leadsTo.insert(eTech);
}

void CvTechInfo::doPostLoadCaching(uint32_t iThis)
{
	PROFILE_EXTRA_FUNC();
	foreach_(const TechTypes ePrereq, getPrereqOrTechs())
	{
		GC.getTechInfo(ePrereq).setLeadsTo((TechTypes)iThis);
	}
	foreach_(const TechTypes ePrereq, getPrereqAndTechs())
	{
		GC.getTechInfo(ePrereq).setLeadsTo((TechTypes)iThis);
	}
}

//======================================================================================================
//					CvPromotionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvPromotionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvPromotionInfo::CvPromotionInfo() :
m_iLayerAnimationPath(ANIMATIONPATH_NONE),
m_iTechPrereq(NO_TECH),
m_iMinEraType(NO_ERA),
m_iMaxEraType(NO_ERA),
m_iStateReligionPrereq(NO_RELIGION),
m_iVisibilityChange(0),
m_iMovesChange(0),
m_iMoveDiscountChange(0),
m_iAirRangeChange(0),
m_iInterceptChange(0),
m_iEvasionChange(0),
m_iWithdrawalChange(0),
m_iCargoChange(0),
m_iSMCargoChange(0),
m_iSMCargoVolumeChange(0),
m_iSMCargoVolumeModifierChange(0),
m_iCollateralDamageChange(0),
m_iBombardRateChange(0),
m_iFirstStrikesChange(0),
m_iChanceFirstStrikesChange(0),
m_iEnemyHealChange(0),
m_iNeutralHealChange(0),
m_iFriendlyHealChange(0),
m_iSameTileHealChange(0),
m_iAdjacentTileHealChange(0),
m_iCombatPercent(0),
m_iCityAttackPercent(0),
m_iCityDefensePercent(0),
m_iHillsAttackPercent(0),
m_iHillsDefensePercent(0),
m_iHillsWorkPercent(0),
m_iPeaksWorkPercent(0),
//ls612: Work rate modifiers
m_iWorkRatePercent(0),
m_iCommandType(NO_COMMAND),
m_iRevoltProtection(0),
m_iCollateralDamageProtection(0),
m_iPillageChange(0),
m_iUpgradeDiscount(0),
m_iExperiencePercent(0),
m_iKamikazePercent(0),
m_iAirCombatLimitChange(0),
m_iCelebrityHappy(0),
m_iCollateralDamageLimitChange(0),
m_iCollateralDamageMaxUnitsChange(0),
m_iCombatLimitChange(0),
m_iExtraDropRange(0),
m_iSurvivorChance(0),
m_iVictoryAdjacentHeal(0),
m_iVictoryHeal(0),
m_iVictoryStackHeal(0),
m_bDefensiveVictoryMove(false),
m_bFreeDrop(false),
m_bOffensiveVictoryMove(false),
m_bOneUp(false),
m_bPillageEspionage(false),
m_bPillageMarauder(false),
m_bPillageOnMove(false),
m_bPillageOnVictory(false),
m_bPillageResearch(false),
m_bLeader(false),
m_bBlitz(false),
m_bAmphib(false),
m_bRiver(false),
m_bEnemyRoute(false),
m_bAlwaysHeal(false),
m_bHillsDoubleMove(false),
m_bImmuneToFirstStrikes(false),
m_piTerrainAttackPercent(NULL),
m_piTerrainDefensePercent(NULL),
m_piFeatureAttackPercent(NULL),
m_piFeatureDefensePercent(NULL),
m_piUnitCombatModifierPercent(NULL),
m_piDomainModifierPercent(NULL),
//m_piAIWeightbyUnitCombatTypes(NULL),
m_pbTerrainDoubleMove(NULL),
m_pbFeatureDoubleMove(NULL),
m_pbUnitCombat(NULL),
//ls612: Terrain Work modifiers
m_piTerrainWorkPercent(NULL),
m_piFeatureWorkPercent(NULL),

m_bCanMovePeaks(false)
,m_bCanLeadThroughPeaks(false)
,m_iObsoleteTech(NO_TECH)
,m_iControlPoints(0)
,m_iCommandRange(0)
,m_iIgnoreTerrainDamage(NO_TERRAIN)
,m_bZoneOfControl(false)

,m_PropertyManipulators()
//TB Combat Mods Begin
//Textual References
,m_ePromotionLine(NO_PROMOTIONLINE),
m_eReplacesUnitCombat(NO_UNITCOMBAT),
m_eDomainCargoChange(NO_DOMAIN),
m_eSpecialCargoChange(NO_SPECIALUNIT),
m_eSpecialCargoPrereq(NO_SPECIALUNIT),
m_eSMNotSpecialCargoChange(NO_SPECIALUNIT),
m_eSMNotSpecialCargoPrereq(NO_SPECIALUNIT),
m_eSetSpecialUnit(NO_SPECIALUNIT),
//integers
m_iAttackCombatModifierChange(0),
m_iDefenseCombatModifierChange(0),
m_iPursuitChange(0),
m_iEarlyWithdrawChange(0),
m_iVSBarbsChange(0),
m_iArmorChange(0),
m_iPunctureChange(0),
m_iOverrunChange(0),
m_iRepelChange(0),
m_iFortRepelChange(0),
m_iRepelRetriesChange(0),
m_iUnyieldingChange(0),
m_iKnockbackChange(0),
m_iKnockbackRetriesChange(0),
#ifdef BATTLEWORN
m_iStrAdjperRndChange(0),
m_iStrAdjperAttChange(0),
m_iStrAdjperDefChange(0),
m_iWithdrawAdjperAttChange(0),
#endif // BATTLEWORN
m_iUnnerveChange(0),
m_iEncloseChange(0),
m_iLungeChange(0),
m_iDynamicDefenseChange(0),
m_iStrengthChange(0),
m_iLinePriority(0),
m_iFortitudeChange(0),
m_iDamageperTurn(0),
m_iStrAdjperTurn(0),
m_iWeakenperTurn(0),
#ifdef STRENGTH_IN_NUMBERS
m_iFrontSupportPercentChange(0),
m_iShortRangeSupportPercentChange(0),
m_iMediumRangeSupportPercentChange(0),
m_iLongRangeSupportPercentChange(0),
m_iFlankSupportPercentChange(0),
#endif
m_iDodgeModifierChange(0),
m_iPrecisionModifierChange(0),
m_iPowerShotsChange(0),
m_iPowerShotCombatModifierChange(0),
m_iPowerShotPunctureModifierChange(0),
m_iPowerShotPrecisionModifierChange(0),
m_iPowerShotCriticalModifierChange(0),
m_iCriticalModifierChange(0),
m_iEnduranceChange(0),
m_iRoundStunProbChange(0),
m_iPoisonProbabilityModifierChange(0),

m_iCaptureProbabilityModifierChange(0),
m_iCaptureResistanceModifierChange(0),

m_iBreakdownChanceChange(0),
m_iBreakdownDamageChange(0),
m_iTauntChange(0),
m_iMaxHPChange(0),
m_iStrengthModifier(0),
m_iQualityChange(0),
m_iGroupChange(0),
m_iLevelPrereq(0),
m_iDamageModifierChange(0),
m_iUpkeepModifier(0),
m_iExtraUpkeep100(0),
m_iRBombardDamageChange(0),
m_iRBombardDamageLimitChange(0),
m_iRBombardDamageMaxUnitsChange(0),
m_iDCMBombRangeChange(0),
m_iDCMBombAccuracyChange(0),
m_iCombatModifierPerSizeMoreChange(0),
m_iCombatModifierPerSizeLessChange(0),
m_iCombatModifierPerVolumeMoreChange(0),
m_iCombatModifierPerVolumeLessChange(0),
m_iSelfHealModifier(0),
m_iNumHealSupport(0),
m_iExcileChange(0),
m_iPassageChange(0),
m_iNoNonOwnedCityEntryChange(0),
m_iBarbCoExistChange(0),
m_iBlendIntoCityChange(0),
m_iUpgradeAnywhereChange(0),
m_iInsidiousnessChange(0),
m_iInvestigationChange(0),
m_iAssassinChange(0),
m_iStealthStrikesChange(0),
m_iStealthCombatModifierChange(0),
m_iStealthDefenseChange(0),
m_iDefenseOnlyChange(0),
m_iNoInvisibilityChange(0),
m_iTrapDamageMax(0),
m_iTrapDamageMin(0),
m_iTrapComplexity(0),
m_iNumTriggers(0),
m_iTriggerBeforeAttackChange(0),
m_iHiddenNationalityChange(0),
m_iAnimalIgnoresBordersChange(0),
m_iNoDefensiveBonusChange(0),
m_iGatherHerdChange(0),
m_iReligiousCombatModifierChange(0),
//booleans
m_bStampedeChange(false),
m_bRemoveStampede(false),
m_bOnslaughtChange(false),
m_bEquipment(false),
#ifdef OUTBREAKS_AND_AFFLICTIONS
m_bAffliction(false),
#endif
m_bParalyze(false),
m_bMakesDamageCold(false),
m_bMakesDamageNotCold(false),
m_bAddsColdImmunity(false),
m_bRemovesColdImmunity(false),
m_bCritical(false),
m_bAttackOnlyCitiesAdd(false),
m_bAttackOnlyCitiesSubtract(false),
m_bIgnoreNoEntryLevelAdd(false),
m_bIgnoreNoEntryLevelSubtract(false),
m_bIgnoreZoneofControlAdd(false),
m_bIgnoreZoneofControlSubtract(false),
m_bFliesToMoveAdd(false),
m_bFliesToMoveSubtract(false),
m_bZeroesXP(false),
m_bForOffset(false),
m_bCargoPrereq(false),
m_bRBombardPrereq(false),
m_bNoSelfHeal(false),
m_bSetOnHNCapture(false),
m_bSetOnInvestigated(false),
m_bStatus(false),
m_bPrereqNormInvisible(false),
m_bPlotPrereqsKeepAfter(false),
m_bRemoveAfterSet(false),
m_bQuick(false),
m_bStarsign(false)
//TB Combat Mods End
{
	CvInfoUtil(this).initDataMembers();

	m_zobristValue = GC.getGame().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvPromotionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------

CvPromotionInfo::~CvPromotionInfo()
{
	CvInfoUtil(this).uninitDataMembers();

	SAFE_DELETE_ARRAY(m_piTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_piTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_piFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_piFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_pbTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_pbFeatureDoubleMove);
	SAFE_DELETE_ARRAY(m_pbUnitCombat);
	SAFE_DELETE_ARRAY(m_piTerrainWorkPercent);
	SAFE_DELETE_ARRAY(m_piFeatureWorkPercent);

	//GC.removeDelayedResolutionVector(m_vPromotionOverwrites);
	GC.removeDelayedResolutionVector(m_aiAddsBuildTypes);
	GC.removeDelayedResolutionVector(m_aiPrereqLocalBuildingTypes);
	GC.removeDelayedResolutionVector(m_aiTrapSetWithPromotionTypes);
}

int CvPromotionInfo::getLayerAnimationPath() const
{
	return m_iLayerAnimationPath;
}

TechTypes CvPromotionInfo::getTechPrereq() const
{
	if (m_iTechPrereq == NO_TECH)
	{
		if (getPromotionLine() == NO_PROMOTIONLINE)
		{
			return m_iTechPrereq;
		}
		return GC.getPromotionLineInfo(getPromotionLine()).getPrereqTech();
	}
	// Sets up the Tech Prereq on the Promotion Definition as a potential override to the Line Tech Prereq
	return m_iTechPrereq;
}

int CvPromotionInfo::getStateReligionPrereq() const
{
	return m_iStateReligionPrereq;
}

int CvPromotionInfo::getMinEraType() const
{
	return m_iMinEraType;
}

int CvPromotionInfo::getMaxEraType() const
{
	return m_iMaxEraType;
}

int CvPromotionInfo::getVisibilityChange() const
{
	return m_iVisibilityChange;
}

int CvPromotionInfo::getMovesChange() const
{
	return m_iMovesChange;
}

int CvPromotionInfo::getMoveDiscountChange() const
{
	return m_iMoveDiscountChange;
}

int CvPromotionInfo::getAirRangeChange() const
{
	return m_iAirRangeChange;
}

int CvPromotionInfo::getInterceptChange() const
{
	return m_iInterceptChange;
}

int CvPromotionInfo::getEvasionChange() const
{
	return m_iEvasionChange;
}

int CvPromotionInfo::getWithdrawalChange() const
{
	return m_iWithdrawalChange;
}

int CvPromotionInfo::getCargoChange() const
{
	return m_iCargoChange;
}

int CvPromotionInfo::getSMCargoChange() const
{
	return m_iSMCargoChange;
}

int CvPromotionInfo::getSMCargoVolumeChange() const
{
	return m_iSMCargoVolumeChange;
}

int CvPromotionInfo::getSMCargoVolumeModifierChange() const
{
	return m_iSMCargoVolumeModifierChange;
}

int CvPromotionInfo::getCollateralDamageChange() const
{
	return m_iCollateralDamageChange;
}

int CvPromotionInfo::getBombardRateChange() const
{
	return m_iBombardRateChange;
}

int CvPromotionInfo::getFirstStrikesChange() const
{
	return m_iFirstStrikesChange;
}

int CvPromotionInfo::getChanceFirstStrikesChange() const
{
	return m_iChanceFirstStrikesChange;
}

int CvPromotionInfo::getEnemyHealChange() const
{
	return m_iEnemyHealChange;
}

int CvPromotionInfo::getNeutralHealChange() const
{
	return m_iNeutralHealChange;
}

int CvPromotionInfo::getFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}

int CvPromotionInfo::getSameTileHealChange() const
{
	return m_iSameTileHealChange;
}

int CvPromotionInfo::getAdjacentTileHealChange() const
{
	return m_iAdjacentTileHealChange;
}

int CvPromotionInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}

int CvPromotionInfo::getCityAttackPercent() const
{
	return m_iCityAttackPercent;
}

int CvPromotionInfo::getCityDefensePercent() const
{
	return m_iCityDefensePercent;
}

int CvPromotionInfo::getHillsAttackPercent() const
{
	return m_iHillsAttackPercent;
}

int CvPromotionInfo::getHillsDefensePercent() const
{
	return m_iHillsDefensePercent;
}

int CvPromotionInfo::getHillsWorkPercent() const
{
	return m_iHillsWorkPercent;
}

int CvPromotionInfo::getPeaksWorkPercent() const
{
	return m_iPeaksWorkPercent;
}

int CvPromotionInfo::getWorkRatePercent() const
{
	return m_iWorkRatePercent;
}

int CvPromotionInfo::getCommandType() const
{
	return m_iCommandType;
}

void CvPromotionInfo::setCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

int CvPromotionInfo::getRevoltProtection() const
{
	return m_iRevoltProtection;
}

int CvPromotionInfo::getCollateralDamageProtection() const
{
	return m_iCollateralDamageProtection;
}

int CvPromotionInfo::getPillageChange() const
{
	return m_iPillageChange;
}

int CvPromotionInfo::getUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}

int CvPromotionInfo::getExperiencePercent() const
{
	return m_iExperiencePercent;
}

int CvPromotionInfo::getKamikazePercent() const
{
	return m_iKamikazePercent;
}

int CvPromotionInfo::getAirCombatLimitChange() const
{
	return m_iAirCombatLimitChange;
}

int CvPromotionInfo::getCelebrityHappy() const
{
	return m_iCelebrityHappy;
}

int CvPromotionInfo::getCollateralDamageLimitChange() const
{
	return m_iCollateralDamageLimitChange;
}

int CvPromotionInfo::getCollateralDamageMaxUnitsChange() const
{
	return m_iCollateralDamageMaxUnitsChange;
}

int CvPromotionInfo::getCombatLimitChange() const
{
	return m_iCombatLimitChange;
}

int CvPromotionInfo::getExtraDropRange() const
{
	return m_iExtraDropRange;
}

int CvPromotionInfo::getSurvivorChance() const
{
	return m_iSurvivorChance;
}

int CvPromotionInfo::getVictoryAdjacentHeal() const
{
	return m_iVictoryAdjacentHeal;
}

int CvPromotionInfo::getVictoryHeal() const
{
	return m_iVictoryHeal;
}

int CvPromotionInfo::getVictoryStackHeal() const
{
	return m_iVictoryStackHeal;
}

bool CvPromotionInfo::isDefensiveVictoryMove() const
{
	return m_bDefensiveVictoryMove;
}

bool CvPromotionInfo::isFreeDrop() const
{
	return m_bFreeDrop;
}

bool CvPromotionInfo::isOffensiveVictoryMove() const
{
	return m_bOffensiveVictoryMove;
}

bool CvPromotionInfo::isOneUp() const
{
	return m_bOneUp;
}

bool CvPromotionInfo::isPillageEspionage() const
{
	return m_bPillageEspionage;
}

bool CvPromotionInfo::isPillageMarauder() const
{
	return m_bPillageMarauder;
}

bool CvPromotionInfo::isPillageOnMove() const
{
	return m_bPillageOnMove;
}

bool CvPromotionInfo::isPillageOnVictory() const
{
	return m_bPillageOnVictory;
}

bool CvPromotionInfo::isPillageResearch() const
{
	return m_bPillageResearch;
}

bool CvPromotionInfo::isLeader() const
{
	return m_bLeader;
}

bool CvPromotionInfo::isBlitz() const
{
	return m_bBlitz;
}

bool CvPromotionInfo::isAmphib() const
{
	return m_bAmphib;
}

bool CvPromotionInfo::isRiver() const
{
	return m_bRiver;
}

bool CvPromotionInfo::isEnemyRoute() const
{
	return m_bEnemyRoute;
}

bool CvPromotionInfo::isAlwaysHeal() const
{
	return m_bAlwaysHeal;
}

bool CvPromotionInfo::isHillsDoubleMove() const
{
	return m_bHillsDoubleMove;
}

bool CvPromotionInfo::isImmuneToFirstStrikes() const
{
	return m_bImmuneToFirstStrikes;
}

const char* CvPromotionInfo::getSound() const
{
	return m_szSound;
}

bool CvPromotionInfo::changesMoveThroughPlots() const
{
	return (isAmphib() ||
			isCanMovePeaks() ||
			isCanLeadThroughPeaks() ||
			m_pbTerrainDoubleMove != NULL ||
			m_pbFeatureDoubleMove != NULL ||
			m_bHillsDoubleMove);
}

int CvPromotionInfo::getTerrainAttackPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainAttackPercent ? m_piTerrainAttackPercent[i] : 0;
}

bool CvPromotionInfo::isAnyTerrainAttackPercent() const
{
	return m_piTerrainAttackPercent != NULL;
}

int CvPromotionInfo::getTerrainDefensePercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainDefensePercent ? m_piTerrainDefensePercent[i] : 0;
}

bool CvPromotionInfo::isAnyTerrainDefensePercent() const
{
	return m_piTerrainDefensePercent != NULL;
}

int CvPromotionInfo::getFeatureAttackPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureAttackPercent ? m_piFeatureAttackPercent[i] : 0;
}

bool CvPromotionInfo::isAnyFeatureAttackPercent() const
{
	return m_piFeatureAttackPercent != NULL;
}

int CvPromotionInfo::getFeatureDefensePercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureDefensePercent ? m_piFeatureDefensePercent[i] : 0;
}

bool CvPromotionInfo::isAnyFeatureDefensePercent() const
{
	return m_piFeatureDefensePercent != NULL;
}

int CvPromotionInfo::getUnitCombatModifierPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return m_piUnitCombatModifierPercent ? m_piUnitCombatModifierPercent[i] : 0;
}

bool CvPromotionInfo::isAnyUnitCombatModifierPercent() const
{
	return m_piUnitCombatModifierPercent != NULL;
}

int CvPromotionInfo::getDomainModifierPercent(int i) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, i);
	return m_piDomainModifierPercent ? m_piDomainModifierPercent[i] : 0;
}

bool CvPromotionInfo::isAnyDomainModifierPercent() const
{
	return m_piDomainModifierPercent != NULL;
}

bool CvPromotionInfo::getTerrainDoubleMove(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrainDoubleMove ? m_pbTerrainDoubleMove[i] : false;
}

bool CvPromotionInfo::getFeatureDoubleMove(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_pbFeatureDoubleMove ? m_pbFeatureDoubleMove[i] : false;
}

bool CvPromotionInfo::getUnitCombat(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return m_pbUnitCombat ? m_pbUnitCombat[i] : false;
}
//ls612: Terrain Work Modifiers
int CvPromotionInfo::getTerrainWorkPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainWorkPercent ? m_piTerrainWorkPercent[i] : 0;
}

int CvPromotionInfo::getFeatureWorkPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureWorkPercent ? m_piFeatureWorkPercent[i] : 0;
}

bool CvPromotionInfo::isCanMovePeaks() const
{
	return m_bCanMovePeaks;
}

//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
//	mountains, and ability to lead a stack through mountains
bool CvPromotionInfo::isCanLeadThroughPeaks() const
{
	return m_bCanLeadThroughPeaks;
}

TechTypes CvPromotionInfo::getObsoleteTech() const
{
	if (m_iObsoleteTech == NO_TECH)
	{
		if (getPromotionLine() == NO_PROMOTIONLINE)
		{
			return m_iObsoleteTech;
		}
		return GC.getPromotionLineInfo(getPromotionLine()).getObsoleteTech();
	}
	//Sets up the Tech Prereq on the Promotion Definition as a potential override to the Line Tech Obsoletion
	return m_iObsoleteTech;
}

int CvPromotionInfo::getControlPoints() const
{
	return m_iControlPoints;
}

int CvPromotionInfo::getCommandRange() const
{
	return m_iCommandRange;
}

int CvPromotionInfo::getIgnoreTerrainDamage() const
{
	return m_iIgnoreTerrainDamage;
}

bool CvPromotionInfo::isZoneOfControl() const
{
	return m_bZoneOfControl;
}

//TB Combat Mods Begin  TB SubCombat Mod begin
const wchar_t* CvPromotionInfo::getRenamesUnitTo() const
{
	return m_szRenamesUnitTo;
}

//Textual References
PromotionLineTypes CvPromotionInfo::getPromotionLine() const
{
	return m_ePromotionLine;
}

UnitCombatTypes CvPromotionInfo::getReplacesUnitCombat() const
{
	return m_eReplacesUnitCombat;
}

DomainTypes CvPromotionInfo::getDomainCargoChange() const
{
	return m_eDomainCargoChange;
}

SpecialUnitTypes CvPromotionInfo::getSpecialCargoChange() const
{
	return m_eSpecialCargoChange;
}

SpecialUnitTypes CvPromotionInfo::getSpecialCargoPrereq() const
{
	return m_eSpecialCargoPrereq;
}

SpecialUnitTypes CvPromotionInfo::getSMNotSpecialCargoChange() const
{
	return m_eSMNotSpecialCargoChange;
}

SpecialUnitTypes CvPromotionInfo::getSMNotSpecialCargoPrereq() const
{
	return m_eSMNotSpecialCargoPrereq;
}

SpecialUnitTypes CvPromotionInfo::setSpecialUnit() const
{
	return m_eSetSpecialUnit;
}

// integers
int CvPromotionInfo::getAttackCombatModifierChange() const
{
	return m_iAttackCombatModifierChange;
}

int CvPromotionInfo::getDefenseCombatModifierChange() const
{
	return m_iDefenseCombatModifierChange;
}

int CvPromotionInfo::getPursuitChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuitChange;
}

int CvPromotionInfo::getEarlyWithdrawChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iEarlyWithdrawChange;
}

int CvPromotionInfo::getVSBarbsChange() const
{
	return m_iVSBarbsChange;
}

int CvPromotionInfo::getArmorChange() const
{
	return m_iArmorChange;
}

int CvPromotionInfo::getPunctureChange() const
{
	return m_iPunctureChange;
}

//Heart of War
int CvPromotionInfo::getOverrunChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iOverrunChange;
}

int CvPromotionInfo::getRepelChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelChange;
}

int CvPromotionInfo::getFortRepelChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iFortRepelChange;
}

int CvPromotionInfo::getRepelRetriesChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelRetriesChange;
}

int CvPromotionInfo::getUnyieldingChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iUnyieldingChange;
}

int CvPromotionInfo::getKnockbackChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackChange;
}

int CvPromotionInfo::getKnockbackRetriesChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackRetriesChange;
}

#ifdef BATTLEWORN
//Battleworn
int CvPromotionInfo::getStrAdjperRndChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperRndChange;
}

int CvPromotionInfo::getStrAdjperAttChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperAttChange;
}

int CvPromotionInfo::getStrAdjperDefChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperDefChange;
}

int CvPromotionInfo::getWithdrawAdjperAttChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_BATTLEWORN))
	{
		return 0;
	}
	return m_iWithdrawAdjperAttChange;
}
#endif // BATTLEWORN

//S&D extended
int CvPromotionInfo::getUnnerveChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iUnnerveChange;
}

int CvPromotionInfo::getEncloseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iEncloseChange;
}

int CvPromotionInfo::getLungeChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iLungeChange;
}

int CvPromotionInfo::getDynamicDefenseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iDynamicDefenseChange;
}
//

int CvPromotionInfo::getStrengthChange() const
{
	return m_iStrengthChange;
}

int CvPromotionInfo::getLinePriority() const
{
	return m_iLinePriority;
}

//Realm of the Serpent
int CvPromotionInfo::getFortitudeChange() const
{
	return m_iFortitudeChange;
}

int CvPromotionInfo::getDamageperTurn() const
{
	return m_iDamageperTurn;
}

int CvPromotionInfo::getStrAdjperTurn() const
{
	return m_iStrAdjperTurn;
}

int CvPromotionInfo::getWeakenperTurn() const
{
	return m_iWeakenperTurn;
}

#ifdef STRENGTH_IN_NUMBERS
int CvPromotionInfo::getFrontSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iFrontSupportPercentChange;
}

int CvPromotionInfo::getShortRangeSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iShortRangeSupportPercentChange;
}

int CvPromotionInfo::getMediumRangeSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iMediumRangeSupportPercentChange;
}

int CvPromotionInfo::getLongRangeSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iLongRangeSupportPercentChange;
}

int CvPromotionInfo::getFlankSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iFlankSupportPercentChange;
}
#endif

int CvPromotionInfo::getDodgeModifierChange() const
{
	return m_iDodgeModifierChange;
}

int CvPromotionInfo::getPrecisionModifierChange() const
{
	return m_iPrecisionModifierChange;
}

int CvPromotionInfo::getPowerShotsChange() const
{
	return m_iPowerShotsChange;
}

int CvPromotionInfo::getPowerShotCombatModifierChange() const
{
	return m_iPowerShotCombatModifierChange;
}

int CvPromotionInfo::getPowerShotPunctureModifierChange() const
{
	return m_iPowerShotPunctureModifierChange;
}

int CvPromotionInfo::getPowerShotPrecisionModifierChange() const
{
	return m_iPowerShotPrecisionModifierChange;
}

int CvPromotionInfo::getPowerShotCriticalModifierChange() const
{
	return m_iPowerShotCriticalModifierChange;
}

int CvPromotionInfo::getCriticalModifierChange() const
{
	return m_iCriticalModifierChange;
}

int CvPromotionInfo::getEnduranceChange() const
{
	return m_iEnduranceChange;
}

int CvPromotionInfo::getRoundStunProbChange() const
{
	return m_iRoundStunProbChange;
}

int CvPromotionInfo::getPoisonProbabilityModifierChange() const
{
	return m_iPoisonProbabilityModifierChange;
}


int CvPromotionInfo::getCaptureProbabilityModifierChange() const
{
	return m_iCaptureProbabilityModifierChange;
}

int CvPromotionInfo::getCaptureResistanceModifierChange() const
{
	return m_iCaptureResistanceModifierChange;
}


int CvPromotionInfo::getBreakdownChanceChange() const
{
	return m_iBreakdownChanceChange;
}

int CvPromotionInfo::getBreakdownDamageChange() const
{
	return m_iBreakdownDamageChange;
}

int CvPromotionInfo::getTauntChange() const
{
	return m_iTauntChange;
}

int CvPromotionInfo::getMaxHPChange() const
{
	return m_iMaxHPChange;
}

int CvPromotionInfo::getStrengthModifier() const
{
	return m_iStrengthModifier;
}

int CvPromotionInfo::getQualityChange() const
{
	return m_iQualityChange;
}

int CvPromotionInfo::getGroupChange() const
{
	return m_iGroupChange;
}

int CvPromotionInfo::getLevelPrereq() const
{
	return m_iLevelPrereq;
}

int CvPromotionInfo::getDamageModifierChange() const
{
	return m_iDamageModifierChange;
}

int CvPromotionInfo::getUpkeepModifier() const
{
	return m_iUpkeepModifier;
}

int CvPromotionInfo::getExtraUpkeep100() const
{
	return m_iExtraUpkeep100;
}

int CvPromotionInfo::getRBombardDamageChange() const
{
	return m_iRBombardDamageChange;
}

int CvPromotionInfo::getRBombardDamageLimitChange() const
{
	return m_iRBombardDamageLimitChange;
}

int CvPromotionInfo::getRBombardDamageMaxUnitsChange() const
{
	return m_iRBombardDamageMaxUnitsChange;
}

int CvPromotionInfo::getDCMBombRangeChange() const
{
	return m_iDCMBombRangeChange;
}

int CvPromotionInfo::getDCMBombAccuracyChange() const
{
	return m_iDCMBombAccuracyChange;
}

int CvPromotionInfo::getCombatModifierPerSizeMoreChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeMoreChange;
}

int CvPromotionInfo::getCombatModifierPerSizeLessChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeLessChange;
}

int CvPromotionInfo::getCombatModifierPerVolumeMoreChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeMoreChange;
}

int CvPromotionInfo::getCombatModifierPerVolumeLessChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeLessChange;
}

int CvPromotionInfo::getSelfHealModifier() const
{
	return m_iSelfHealModifier;
}

int CvPromotionInfo::getNumHealSupport() const
{
	return m_iNumHealSupport;
}

int CvPromotionInfo::getExcileChange() const
{
	return m_iExcileChange;
}

int CvPromotionInfo::getPassageChange() const
{
	return m_iPassageChange;
}

int CvPromotionInfo::getNoNonOwnedCityEntryChange() const
{
	return m_iNoNonOwnedCityEntryChange;
}

int CvPromotionInfo::getBarbCoExistChange() const
{
	return m_iBarbCoExistChange;
}

int CvPromotionInfo::getBlendIntoCityChange() const
{
	return m_iBlendIntoCityChange;
}

int CvPromotionInfo::getUpgradeAnywhereChange() const
{
	return m_iUpgradeAnywhereChange;
}

int CvPromotionInfo::getInsidiousnessChange() const
{
	return m_iInsidiousnessChange;
}

int CvPromotionInfo::getInvestigationChange() const
{
	return m_iInvestigationChange;
}

int CvPromotionInfo::getAssassinChange() const
{
	return m_iAssassinChange;
}

int CvPromotionInfo::getStealthStrikesChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthStrikesChange;
}

int CvPromotionInfo::getStealthCombatModifierChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthCombatModifierChange;
}

int CvPromotionInfo::getStealthDefenseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthDefenseChange;
}

int CvPromotionInfo::getDefenseOnlyChange() const
{
	return m_iDefenseOnlyChange;
}

int CvPromotionInfo::getNoInvisibilityChange() const
{
	return m_iNoInvisibilityChange;
}

int CvPromotionInfo::getTrapDamageMax() const
{
	return m_iTrapDamageMax;
}

int CvPromotionInfo::getTrapDamageMin() const
{
	return m_iTrapDamageMin;
}

int CvPromotionInfo::getTrapComplexity() const
{
	return m_iTrapComplexity;
}

int CvPromotionInfo::getNumTriggers() const
{
	return m_iNumTriggers;
}

int CvPromotionInfo::getTriggerBeforeAttackChange() const
{
	return m_iTriggerBeforeAttackChange;
}

int CvPromotionInfo::getHiddenNationalityChange() const
{
	return m_iHiddenNationalityChange;
}

int CvPromotionInfo::getAnimalIgnoresBordersChange() const
{
	return m_iAnimalIgnoresBordersChange;
}

int CvPromotionInfo::getNoDefensiveBonusChange() const
{
	return m_iNoDefensiveBonusChange;
}

int CvPromotionInfo::getGatherHerdChange() const
{
	return m_iGatherHerdChange;
}

int CvPromotionInfo::getReligiousCombatModifierChange() const
{
	return m_iReligiousCombatModifierChange;
}
// booleans
bool CvPromotionInfo::isStampedeChange() const
{
	return m_bStampedeChange;
}

bool CvPromotionInfo::isRemoveStampede() const
{
	return m_bRemoveStampede;
}

bool CvPromotionInfo::isOnslaughtChange() const
{
	return m_bOnslaughtChange;
}

bool CvPromotionInfo::isEquipment() const
{
	if ( getPromotionLine() == NO_PROMOTIONLINE)
	{
		return false; // require afflictions to always be parts of lines
	}

	// return the value from the line
	return GC.getPromotionLineInfo(getPromotionLine()).isEquipment();
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
bool CvPromotionInfo::isAffliction() const
{
	if ( getPromotionLine() == NO_PROMOTIONLINE)
	{
		return false; // require afflictions to always be parts of lines
	}

	// return the value from the line
	return GC.getPromotionLineInfo(getPromotionLine()).isAffliction();
}
#endif

bool CvPromotionInfo::isParalyze() const
{
	return m_bParalyze;
}

bool CvPromotionInfo::isMakesDamageCold() const
{
	return m_bMakesDamageCold;
}

bool CvPromotionInfo::isMakesDamageNotCold() const
{
	return m_bMakesDamageNotCold;
}

bool CvPromotionInfo::isAddsColdImmunity() const
{
	return m_bAddsColdImmunity;
}

bool CvPromotionInfo::isRemovesColdImmunity() const
{
	return m_bRemovesColdImmunity;
}

bool CvPromotionInfo::isCritical() const
{
	if ( getPromotionLine() == NO_PROMOTIONLINE)
	{
		return false; // require afflictions to always be parts of lines
	}

	// return the value from the line
	return GC.getPromotionLineInfo(getPromotionLine()).isCritical();
}

bool CvPromotionInfo::isAttackOnlyCitiesAdd() const
{
	return m_bAttackOnlyCitiesAdd;
}

bool CvPromotionInfo::isAttackOnlyCitiesSubtract() const
{
	return m_bAttackOnlyCitiesSubtract;
}

bool CvPromotionInfo::isIgnoreNoEntryLevelAdd() const
{
	return m_bIgnoreNoEntryLevelAdd;
}

bool CvPromotionInfo::isIgnoreNoEntryLevelSubtract() const
{
	return m_bIgnoreNoEntryLevelSubtract;
}

bool CvPromotionInfo::isIgnoreZoneofControlAdd() const
{
	return m_bIgnoreZoneofControlAdd;
}

bool CvPromotionInfo::isIgnoreZoneofControlSubtract() const
{
	return m_bIgnoreZoneofControlSubtract;
}

bool CvPromotionInfo::isFliesToMoveAdd() const
{
	return m_bFliesToMoveAdd;
}

bool CvPromotionInfo::isFliesToMoveSubtract() const
{
	return m_bFliesToMoveSubtract;
}

bool CvPromotionInfo::isZeroesXP() const
{
	return m_bZeroesXP;
}

bool CvPromotionInfo::isForOffset() const
{
	return m_bForOffset;
}

bool CvPromotionInfo::isCargoPrereq() const
{
	return m_bCargoPrereq;
}

bool CvPromotionInfo::isRBombardPrereq() const
{
	return m_bRBombardPrereq;
}

bool CvPromotionInfo::isNoSelfHeal() const
{
	return m_bNoSelfHeal;
}

bool CvPromotionInfo::isSetOnHNCapture() const
{
	return m_bSetOnHNCapture;
}

bool CvPromotionInfo::isSetOnInvestigated() const
{
	return m_bSetOnInvestigated;
}

bool CvPromotionInfo::isStatus() const
{
	return m_bStatus;
}

bool CvPromotionInfo::isPrereqNormInvisible() const
{
	return m_bPrereqNormInvisible;
}

bool CvPromotionInfo::isPlotPrereqsKeepAfter() const
{
	return m_bPlotPrereqsKeepAfter;
}

bool CvPromotionInfo::isRemoveAfterSet() const
{
	return m_bRemoveAfterSet;
}

bool CvPromotionInfo::isQuick() const
{
	return m_bQuick;
}

// bool vectors without delayed resolution
int CvPromotionInfo::getSubCombatChangeType(int i) const
{
	return m_aiSubCombatChangeTypes[i];
}

int CvPromotionInfo::getNumSubCombatChangeTypes() const
{
	return (int)m_aiSubCombatChangeTypes.size();
}

bool CvPromotionInfo::isSubCombatChangeType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiSubCombatChangeTypes, i);
}

int CvPromotionInfo::getRemovesUnitCombatType(int i) const
{
	return m_aiRemovesUnitCombatTypes[i];
}

int CvPromotionInfo::getNumRemovesUnitCombatTypes() const
{
	return (int)m_aiRemovesUnitCombatTypes.size();
}

bool CvPromotionInfo::isRemovesUnitCombatType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiRemovesUnitCombatTypes, i);
}

int CvPromotionInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvPromotionInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvPromotionInfo::isOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiOnGameOptions, i);
}

int CvPromotionInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvPromotionInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvPromotionInfo::isNotOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiNotOnGameOptions, i);
}

int CvPromotionInfo::getFreetoUnitCombat(int i) const
{
	return m_aiFreetoUnitCombats[i];
}

int CvPromotionInfo::getNumFreetoUnitCombats() const
{
	return (int)m_aiFreetoUnitCombats.size();
}

bool CvPromotionInfo::isFreetoUnitCombat(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiFreetoUnitCombats, i);
}

int CvPromotionInfo::getNotOnDomainType(int i) const
{
	return m_aiNotOnDomainTypes[i];
}

int CvPromotionInfo::getNumNotOnDomainTypes() const
{
	return (int)m_aiNotOnDomainTypes.size();
}

bool CvPromotionInfo::isNotOnDomainType(int i) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, i);
	return algo::any_of_equal(m_aiNotOnDomainTypes, i);
}

int CvPromotionInfo::getNoAutoEquiptoCombatClassType(int i) const
{
	return m_aiNoAutoEquiptoCombatClassTypes[i];
}

int CvPromotionInfo::getNumNoAutoEquiptoCombatClassTypes() const
{
	return (int)m_aiNoAutoEquiptoCombatClassTypes.size();
}

bool CvPromotionInfo::isNoAutoEquiptoCombatClassType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiNoAutoEquiptoCombatClassTypes, i);
}

//int CvPromotionInfo::getAIWeightbyUnitCombatType(int i) const
//{
//	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
//	return m_piAIWeightbyUnitCombatTypes ? m_piAIWeightbyUnitCombatTypes[i] : 0;
//}

//bool CvPromotionInfo::isAnyAIWeightbyUnitCombatType() const
//{
//	return m_bAnyAIWeightbyUnitCombat;
//}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvPromotionInfo::getCureAfflictionChangeType(int i) const
{
	return m_aiCureAfflictionChangeTypes[i];
}

int CvPromotionInfo::getNumCureAfflictionChangeTypes() const
{
	return (int)m_aiCureAfflictionChangeTypes.size();
}

bool CvPromotionInfo::isCureAfflictionChangeType(int i) const
{
	return algo::any_of_equal(m_aiCureAfflictionChangeTypes, i);
}
#endif // OUTBREAKS_AND_AFFLICTIONS

int CvPromotionInfo::getAddsBuildType(int i) const
{
	return m_aiAddsBuildTypes[i];
}

int CvPromotionInfo::getNumAddsBuildTypes() const
{
	return (int)m_aiAddsBuildTypes.size();
}

bool CvPromotionInfo::isAddsBuildType(int i) const
{
	return algo::any_of_equal(m_aiAddsBuildTypes, i);
}

int CvPromotionInfo::getNegatesInvisibilityType(int i) const
{
	return m_aiNegatesInvisibilityTypes[i];
}

int CvPromotionInfo::getNumNegatesInvisibilityTypes() const
{
	return (int)m_aiNegatesInvisibilityTypes.size();
}

bool CvPromotionInfo::isNegatesInvisibilityType(int i) const
{
	return algo::any_of_equal(m_aiNegatesInvisibilityTypes, i);
}

int CvPromotionInfo::getPrereqTerrainType(int i) const
{
	return m_aiPrereqTerrainTypes[i];
}

int CvPromotionInfo::getNumPrereqTerrainTypes() const
{
	return (int)m_aiPrereqTerrainTypes.size();
}

bool CvPromotionInfo::isPrereqTerrainType(int i) const
{
	return algo::any_of_equal(m_aiPrereqTerrainTypes, i);
}

int CvPromotionInfo::getPrereqFeatureType(int i) const
{
	return m_aiPrereqFeatureTypes[i];
}

int CvPromotionInfo::getNumPrereqFeatureTypes() const
{
	return (int)m_aiPrereqFeatureTypes.size();
}

bool CvPromotionInfo::isPrereqFeatureType(int i) const
{
	return algo::any_of_equal(m_aiPrereqFeatureTypes, i);
}

int CvPromotionInfo::getPrereqImprovementType(int i) const
{
	return m_aiPrereqImprovementTypes[i];
}

int CvPromotionInfo::getNumPrereqImprovementTypes() const
{
	return (int)m_aiPrereqImprovementTypes.size();
}

bool CvPromotionInfo::isPrereqImprovementType(int i) const
{
	return algo::any_of_equal(m_aiPrereqImprovementTypes, i);
}

int CvPromotionInfo::getPrereqPlotBonusType(int i) const
{
	return m_aiPrereqPlotBonusTypes[i];
}

int CvPromotionInfo::getNumPrereqPlotBonusTypes() const
{
	return (int)m_aiPrereqPlotBonusTypes.size();
}

bool CvPromotionInfo::isPrereqPlotBonusType(int i) const
{
	return algo::any_of_equal(m_aiPrereqPlotBonusTypes, i);
}

int CvPromotionInfo::getPrereqLocalBuildingType(int i) const
{
	return m_aiPrereqLocalBuildingTypes[i];
}

int CvPromotionInfo::getNumPrereqLocalBuildingTypes() const
{
	return (int)m_aiPrereqLocalBuildingTypes.size();
}

bool CvPromotionInfo::isPrereqLocalBuildingType(int i) const
{
	return algo::any_of_equal(m_aiPrereqLocalBuildingTypes, i);
}

int CvPromotionInfo::getTrapSetWithPromotionType(int i) const
{
	return m_aiTrapSetWithPromotionTypes[i];
}

int CvPromotionInfo::getNumTrapSetWithPromotionTypes() const
{
	return (int)m_aiTrapSetWithPromotionTypes.size();
}

bool CvPromotionInfo::isTrapSetWithPromotionType(int i) const
{
	return algo::any_of_equal(m_aiTrapSetWithPromotionTypes, i);
}

int CvPromotionInfo::getTrapImmunityUnitCombatType(int i) const
{
	return m_aiTrapImmunityUnitCombatTypes[i];
}

int CvPromotionInfo::getNumTrapImmunityUnitCombatTypes() const
{
	return (int)m_aiTrapImmunityUnitCombatTypes.size();
}

bool CvPromotionInfo::isTrapImmunityUnitCombatType(int i) const
{
	return algo::any_of_equal(m_aiTrapImmunityUnitCombatTypes, i);
}

int CvPromotionInfo::getTargetUnitCombatType(int i) const
{
	return m_aiTargetUnitCombatTypes[i];
}

int CvPromotionInfo::getNumTargetUnitCombatTypes() const
{
	return (int)m_aiTargetUnitCombatTypes.size();
}

bool CvPromotionInfo::isTargetUnitCombatType(int i) const
{
	return algo::any_of_equal(m_aiTargetUnitCombatTypes, i);
}

int CvPromotionInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvPromotionInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvPromotionInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

// int vectors utilizing pairing without delayed resolution
int CvPromotionInfo::getNumFlankingStrikesbyUnitCombatTypesChange() const
{
	return m_aFlankingStrengthbyUnitCombatTypeChange.size();
}

int CvPromotionInfo::getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aFlankingStrengthbyUnitCombatTypeChange.begin(); it != m_aFlankingStrengthbyUnitCombatTypeChange.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isFlankingStrikebyUnitCombatTypeChange(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aFlankingStrengthbyUnitCombatTypeChange.begin(); it != m_aFlankingStrengthbyUnitCombatTypeChange.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumWithdrawOnTerrainTypeChanges() const
{
	return m_aWithdrawOnTerrainTypesChange.size();
}

int CvPromotionInfo::getWithdrawOnTerrainTypeChange(int iTerrain) const
{
	PROFILE_EXTRA_FUNC();
	for (TerrainModifierArray::const_iterator it = m_aWithdrawOnTerrainTypesChange.begin(); it != m_aWithdrawOnTerrainTypesChange.end(); ++it)
	{
		if ((*it).first == (TerrainTypes)iTerrain)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvPromotionInfo::getNumWithdrawOnFeatureTypeChanges() const
{
	return m_aWithdrawOnFeatureTypesChange.size();
}

int CvPromotionInfo::getWithdrawOnFeatureTypeChange(int iFeature) const
{
	PROFILE_EXTRA_FUNC();
	for (FeatureModifierArray::const_iterator it = m_aWithdrawOnFeatureTypesChange.begin(); it != m_aWithdrawOnFeatureTypesChange.end(); ++it)
	{
		if ((*it).first == (FeatureTypes)iFeature)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvPromotionInfo::getNumWithdrawVSUnitCombatChangeTypes() const
{
	return m_aWithdrawVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getWithdrawVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aWithdrawVSUnitCombatChangeTypes.begin(); it != m_aWithdrawVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isWithdrawVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aWithdrawVSUnitCombatChangeTypes.begin(); it != m_aWithdrawVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumPursuitVSUnitCombatChangeTypes() const
{
	return m_aPursuitVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	PROFILE_EXTRA_FUNC();
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aPursuitVSUnitCombatChangeTypes.begin(); it != m_aPursuitVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	PROFILE_EXTRA_FUNC();
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return false;
	}

	for (UnitCombatModifierArray::const_iterator it = m_aPursuitVSUnitCombatChangeTypes.begin(); it != m_aPursuitVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumRepelVSUnitCombatChangeTypes() const
{
	return m_aRepelVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getRepelVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	PROFILE_EXTRA_FUNC();
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aRepelVSUnitCombatChangeTypes.begin(); it != m_aRepelVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isRepelVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	PROFILE_EXTRA_FUNC();
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return false;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aRepelVSUnitCombatChangeTypes.begin(); it != m_aRepelVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumKnockbackVSUnitCombatChangeTypes() const
{
	return m_aKnockbackVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getKnockbackVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	PROFILE_EXTRA_FUNC();
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aKnockbackVSUnitCombatChangeTypes.begin(); it != m_aKnockbackVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isKnockbackVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	PROFILE_EXTRA_FUNC();
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return false;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aKnockbackVSUnitCombatChangeTypes.begin(); it != m_aKnockbackVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumPunctureVSUnitCombatChangeTypes() const
{
	return m_aPunctureVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getPunctureVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aPunctureVSUnitCombatChangeTypes.begin(); it != m_aPunctureVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isPunctureVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aPunctureVSUnitCombatChangeTypes.begin(); it != m_aPunctureVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumArmorVSUnitCombatChangeTypes() const
{
	return m_aArmorVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getArmorVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aArmorVSUnitCombatChangeTypes.begin(); it != m_aArmorVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isArmorVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aArmorVSUnitCombatChangeTypes.begin(); it != m_aArmorVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumDodgeVSUnitCombatChangeTypes() const
{
	return m_aDodgeVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getDodgeVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aDodgeVSUnitCombatChangeTypes.begin(); it != m_aDodgeVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isDodgeVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aDodgeVSUnitCombatChangeTypes.begin(); it != m_aDodgeVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumPrecisionVSUnitCombatChangeTypes() const
{
	return m_aPrecisionVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getPrecisionVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aPrecisionVSUnitCombatChangeTypes.begin(); it != m_aPrecisionVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isPrecisionVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aPrecisionVSUnitCombatChangeTypes.begin(); it != m_aPrecisionVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumCriticalVSUnitCombatChangeTypes() const
{
	return m_aCriticalVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getCriticalVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aCriticalVSUnitCombatChangeTypes.begin(); it != m_aCriticalVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isCriticalVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aCriticalVSUnitCombatChangeTypes.begin(); it != m_aCriticalVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumRoundStunVSUnitCombatChangeTypes() const
{
	return m_aRoundStunVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getRoundStunVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aRoundStunVSUnitCombatChangeTypes.begin(); it != m_aRoundStunVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isRoundStunVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aRoundStunVSUnitCombatChangeTypes.begin(); it != m_aRoundStunVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumTrapDisableUnitCombatTypes() const
{
	return m_aTrapDisableUnitCombatTypes.size();
}

int CvPromotionInfo::getTrapDisableUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapDisableUnitCombatTypes.begin(); it != m_aTrapDisableUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isTrapDisableUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapDisableUnitCombatTypes.begin(); it != m_aTrapDisableUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumTrapAvoidanceUnitCombatTypes() const
{
	return m_aTrapAvoidanceUnitCombatTypes.size();
}

int CvPromotionInfo::getTrapAvoidanceUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapAvoidanceUnitCombatTypes.begin(); it != m_aTrapAvoidanceUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isTrapAvoidanceUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapAvoidanceUnitCombatTypes.begin(); it != m_aTrapAvoidanceUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumTrapTriggerUnitCombatTypes() const
{
	return m_aTrapTriggerUnitCombatTypes.size();
}

int CvPromotionInfo::getTrapTriggerUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapTriggerUnitCombatTypes.begin(); it != m_aTrapTriggerUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isTrapTriggerUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapTriggerUnitCombatTypes.begin(); it != m_aTrapTriggerUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvPromotionInfo::getNumAidChanges() const
{
	return m_aAidChanges.size();
}

int CvPromotionInfo::getAidChange(int iProperty) const
{
	PROFILE_EXTRA_FUNC();
	for (AidArray::const_iterator it = m_aAidChanges.begin(); it != m_aAidChanges.end(); ++it)
	{
		if ((*it).first == (PropertyTypes)iProperty)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isAidChange(int iProperty) const
{
	PROFILE_EXTRA_FUNC();
	for (AidArray::const_iterator it = m_aAidChanges.begin(); it != m_aAidChanges.end(); ++it)
	{
		if ((*it).first == (PropertyTypes)iProperty)
		{
			return true;
		}
	}
	return false;
}
#endif

int CvPromotionInfo::getNumBuildWorkRateModifierChangeTypes() const
{
	return m_aBuildWorkRateModifierChangeTypes.size();
}

int CvPromotionInfo::getBuildWorkRateModifierChangeType(int iBuild) const
{
	PROFILE_EXTRA_FUNC();
	for (BuildModifierArray::const_iterator it = m_aBuildWorkRateModifierChangeTypes.begin(); it != m_aBuildWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (BuildTypes)iBuild)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isBuildWorkRateModifierChangeType(int iBuild) const
{
	PROFILE_EXTRA_FUNC();
	for (BuildModifierArray::const_iterator it = m_aBuildWorkRateModifierChangeTypes.begin(); it != m_aBuildWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (BuildTypes)iBuild)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumVisibilityIntensityChangeTypes() const
{
	return m_aVisibilityIntensityChangeTypes.size();
}

int CvPromotionInfo::getVisibilityIntensityChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityChangeTypes.begin(); it != m_aVisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isVisibilityIntensityChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityChangeTypes.begin(); it != m_aVisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumInvisibilityIntensityChangeTypes() const
{
	return m_aInvisibilityIntensityChangeTypes.size();
}

int CvPromotionInfo::getInvisibilityIntensityChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityChangeTypes.begin(); it != m_aInvisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isInvisibilityIntensityChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityChangeTypes.begin(); it != m_aInvisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumVisibilityIntensityRangeChangeTypes() const
{
	return m_aVisibilityIntensityRangeChangeTypes.size();
}

int CvPromotionInfo::getVisibilityIntensityRangeChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeChangeTypes.begin(); it != m_aVisibilityIntensityRangeChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isVisibilityIntensityRangeChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeChangeTypes.begin(); it != m_aVisibilityIntensityRangeChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumAIWeightbyUnitCombatTypes() const
{
	return (int)m_aAIWeightbyUnitCombatTypes.size();
}

const UnitCombatModifier& CvPromotionInfo::getAIWeightbyUnitCombatType(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumAIWeightbyUnitCombatTypes(), iUnitCombat);
	return m_aAIWeightbyUnitCombatTypes[iUnitCombat];
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvPromotionInfo::getNumAfflictionFortitudeChangeModifiers() const
{
	return (int)m_aAfflictionFortitudeChangeModifiers.size();
}

const PromotionLineModifier& CvPromotionInfo::getAfflictionFortitudeChangeModifier(int iAfflictionLine) const
{
	FASSERT_BOUNDS(0, getNumAfflictionFortitudeChangeModifiers(), iAfflictionLine);
	return m_aAfflictionFortitudeChangeModifiers[iAfflictionLine];
}

int CvPromotionInfo::getNumAfflictOnAttackChangeTypes() const
{
	return (int)m_aAfflictOnAttackChangeTypes.size();
}

const AfflictOnAttackChange& CvPromotionInfo::getAfflictOnAttackChangeType(int iAfflictionLine) const
{
	FASSERT_BOUNDS(0, getNumAfflictOnAttackChangeTypes(), iAfflictionLine);
	return m_aAfflictOnAttackChangeTypes[iAfflictionLine];
}
#endif

int CvPromotionInfo::getNumInvisibleTerrainChanges() const
{
	return (int)m_aInvisibleTerrainChanges.size();
}

const InvisibleTerrainChanges& CvPromotionInfo::getInvisibleTerrainChange(int iIndex) const
{
	return m_aInvisibleTerrainChanges[iIndex];
}

int CvPromotionInfo::getNumInvisibleFeatureChanges() const
{
	return (int)m_aInvisibleFeatureChanges.size();
}

const InvisibleFeatureChanges& CvPromotionInfo::getInvisibleFeatureChange(int iIndex) const
{
	return m_aInvisibleFeatureChanges[iIndex];
}

int CvPromotionInfo::getNumInvisibleImprovementChanges() const
{
	return (int)m_aInvisibleImprovementChanges.size();
}

const InvisibleImprovementChanges& CvPromotionInfo::getInvisibleImprovementChange(int iIndex) const
{
	return m_aInvisibleImprovementChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleTerrainChanges() const
{
	return (int)m_aVisibleTerrainChanges.size();
}

const InvisibleTerrainChanges& CvPromotionInfo::getVisibleTerrainChange(int iIndex) const
{
	return m_aVisibleTerrainChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleFeatureChanges() const
{
	return (int)m_aVisibleFeatureChanges.size();
}

const InvisibleFeatureChanges& CvPromotionInfo::getVisibleFeatureChange(int iIndex) const
{
	return m_aVisibleFeatureChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleImprovementChanges() const
{
	return (int)m_aVisibleImprovementChanges.size();
}

const InvisibleImprovementChanges& CvPromotionInfo::getVisibleImprovementChange(int iIndex) const
{
	return m_aVisibleImprovementChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleTerrainRangeChanges() const
{
	return (int)m_aVisibleTerrainRangeChanges.size();
}

const InvisibleTerrainChanges& CvPromotionInfo::getVisibleTerrainRangeChange(int iIndex) const
{
	return m_aVisibleTerrainRangeChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleFeatureRangeChanges() const
{
	return (int)m_aVisibleFeatureRangeChanges.size();
}

const InvisibleFeatureChanges& CvPromotionInfo::getVisibleFeatureRangeChange(int iIndex) const
{
	return m_aVisibleFeatureRangeChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleImprovementRangeChanges() const
{
	return (int)m_aVisibleImprovementRangeChanges.size();
}

const InvisibleImprovementChanges& CvPromotionInfo::getVisibleImprovementRangeChange(int iIndex) const
{
	return m_aVisibleImprovementRangeChanges[iIndex];
}

int CvPromotionInfo::getNumDistanceAttackCommunicabilityTypeChanges() const
{
	return (int)m_aDistanceAttackCommunicabilityTypeChanges.size();
}

const AfflictionLineChanges& CvPromotionInfo::getDistanceAttackCommunicabilityTypeChange(int iIndex) const
{
	return m_aDistanceAttackCommunicabilityTypeChanges[iIndex];
}

int CvPromotionInfo::getNumHealUnitCombatChangeTypes() const
{
	return (int)m_aHealUnitCombatChangeTypes.size();
}

const HealUnitCombat& CvPromotionInfo::getHealUnitCombatChangeType(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumHealUnitCombatChangeTypes(), iUnitCombat);
	return m_aHealUnitCombatChangeTypes[iUnitCombat];
}

int CvPromotionInfo::getQualifiedUnitCombatType(int i) const
{
	return m_aiQualifiedUnitCombatTypes[i];
}

int CvPromotionInfo::getNumQualifiedUnitCombatTypes() const
{
	return (int)m_aiQualifiedUnitCombatTypes.size();
}

bool CvPromotionInfo::isQualifiedUnitCombatType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiQualifiedUnitCombatTypes, i);
}

void CvPromotionInfo::setQualifiedUnitCombatTypes()
{
	PROFILE_EXTRA_FUNC();
	m_aiQualifiedUnitCombatTypes.clear();
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (getUnitCombat(iI))
		{
			m_aiQualifiedUnitCombatTypes.push_back(iI);
		}
	}
	const PromotionLineTypes ePromotionLine = getPromotionLine();

	if (ePromotionLine > -1)
	{
		for (int iI = 0; iI < GC.getPromotionLineInfo(ePromotionLine).getNumUnitCombatPrereqTypes(); iI++)
		{
			const int iUnitCombat = GC.getPromotionLineInfo(ePromotionLine).getUnitCombatPrereqType(iI);

			if (!isQualifiedUnitCombatType(iUnitCombat))
			{
				m_aiQualifiedUnitCombatTypes.push_back(iUnitCombat);
			}
		}
	}
}

int CvPromotionInfo::getNotOnUnitCombatType(int i) const
{
	return m_aiNotOnUnitCombatTypes[i];
}

int CvPromotionInfo::getNumNotOnUnitCombatTypes() const
{
	return (int)m_aiNotOnUnitCombatTypes.size();
}

bool CvPromotionInfo::isNotOnUnitCombatType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiNotOnUnitCombatTypes, i);
}

int CvPromotionInfo::getDisqualifiedUnitCombatType(int i) const
{
	return m_disqualifiedUnitCombatTypes[i];
}

int CvPromotionInfo::getNumDisqualifiedUnitCombatTypes() const
{
	return (int)m_disqualifiedUnitCombatTypes.size();
}

void CvPromotionInfo::setDisqualifiedUnitCombatTypes()
{
	PROFILE_EXTRA_FUNC();
	m_disqualifiedUnitCombatTypes.clear();
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (isNotOnUnitCombatType(iI))
		{
			if (isQualifiedUnitCombatType(iI))
			{
				std::vector<int>::iterator itr = find(m_aiQualifiedUnitCombatTypes.begin(), m_aiQualifiedUnitCombatTypes.end(), iI);
				if (itr != m_aiQualifiedUnitCombatTypes.end())
				{
					m_aiQualifiedUnitCombatTypes.erase(itr);
				}
			}
			m_disqualifiedUnitCombatTypes.push_back(iI);
		}
	}
	const PromotionLineTypes ePromotionLine = getPromotionLine();

	if (ePromotionLine > -1)
	{
		for (int iI = 0; iI < GC.getPromotionLineInfo(ePromotionLine).getNumNotOnUnitCombatTypes(); iI++)
		{
			const int iUnitCombat = GC.getPromotionLineInfo(ePromotionLine).getNotOnUnitCombatType(iI);

			if (!isNotOnUnitCombatType(iUnitCombat))
			{
				if (isQualifiedUnitCombatType(iUnitCombat))
				{
					std::vector<int>::iterator itr = find(m_aiQualifiedUnitCombatTypes.begin(), m_aiQualifiedUnitCombatTypes.end(), iI);
					if (itr != m_aiQualifiedUnitCombatTypes.end())
					{
						m_aiQualifiedUnitCombatTypes.erase(itr);
					}
				}
				m_disqualifiedUnitCombatTypes.push_back(iUnitCombat);
			}
		}
	}
}

//TB Combat Mods End  TB SubCombat Mod end

bool CvPromotionInfo::hasNegativeEffects() const
{
	return getPursuitChange() < 0 ||
		getOverrunChange() < 0 ||
		getUnyieldingChange() < 0 ||
		getKnockbackChange() < 0 ||
		getLungeChange() < 0 ||
		getDodgeModifierChange() < 0 ||
		getPrecisionModifierChange() < 0 ||
		getCriticalModifierChange() < 0 ||
		getEnduranceChange() < 0 ||
		getFirstStrikesChange() < 0 ||
		getChanceFirstStrikesChange() < 0 ||
		getVSBarbsChange() < 0 ||
		getStrengthChange() < 0 ||
		getAttackCombatModifierChange() < 0 ||
		getCombatPercent() < 0 ||
		getDefenseOnlyChange() > 0 ||
		getNoInvisibilityChange() > 0 ||
		getHiddenNationalityChange() != 0;
}

void CvPromotionInfo::getDataMembers(CvInfoUtil& util)
{
	util
		.addEnum(m_iPrereqPromotion, L"PromotionPrereq")
		.addEnum(m_iPrereqOrPromotion1, L"PromotionPrereqOr1")
		.addEnum(m_iPrereqOrPromotion2, L"PromotionPrereqOr2")
	;
}

bool CvPromotionInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvInfoUtil(this).readXml(pXML);

	CvString szTextVal;
	CvString szTextVal2;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"LayerAnimationPath");
	m_iLayerAnimationPath = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StateReligionPrereq");
	m_iStateReligionPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MinEraType");
	m_iMinEraType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MaxEraType");
	m_iMaxEraType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bLeader, L"bLeader");
	if (m_bLeader)
	{
		m_bGraphicalOnly = true;  // don't show in Civilopedia list of promotions
	}
	pXML->GetOptionalChildXmlValByName(&m_bBlitz, L"bBlitz");
	pXML->GetOptionalChildXmlValByName(&m_bAmphib, L"bAmphib");
	pXML->GetOptionalChildXmlValByName(&m_bRiver, L"bRiver");
	pXML->GetOptionalChildXmlValByName(&m_bEnemyRoute, L"bEnemyRoute");
	pXML->GetOptionalChildXmlValByName(&m_bAlwaysHeal, L"bAlwaysHeal");
	pXML->GetOptionalChildXmlValByName(&m_bHillsDoubleMove, L"bHillsDoubleMove");
	pXML->GetOptionalChildXmlValByName(&m_bImmuneToFirstStrikes, L"bImmuneToFirstStrikes");
	pXML->GetOptionalChildXmlValByName(&m_iVisibilityChange, L"iVisibilityChange");
	pXML->GetOptionalChildXmlValByName(&m_iMovesChange, L"iMovesChange");
	pXML->GetOptionalChildXmlValByName(&m_iMoveDiscountChange, L"iMoveDiscountChange");
	pXML->GetOptionalChildXmlValByName(&m_iAirRangeChange, L"iAirRangeChange");
	pXML->GetOptionalChildXmlValByName(&m_iInterceptChange, L"iInterceptChange");
	pXML->GetOptionalChildXmlValByName(&m_iEvasionChange, L"iEvasionChange");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawalChange, L"iWithdrawalChange");
	pXML->GetOptionalChildXmlValByName(&m_iCargoChange, L"iCargoChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoChange, L"iSMCargoChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoVolumeChange, L"iSMCargoVolumeChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoVolumeModifierChange, L"iSMCargoVolumeModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageChange, L"iCollateralDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iBombardRateChange, L"iBombardRateChange");
	pXML->GetOptionalChildXmlValByName(&m_iFirstStrikesChange, L"iFirstStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iChanceFirstStrikesChange, L"iChanceFirstStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iEnemyHealChange, L"iEnemyHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iNeutralHealChange, L"iNeutralHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iFriendlyHealChange, L"iFriendlyHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iSameTileHealChange, L"iSameTileHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iAdjacentTileHealChange, L"iAdjacentTileHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatPercent, L"iCombatPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCityAttackPercent, L"iCityAttack");
	pXML->GetOptionalChildXmlValByName(&m_iCityDefensePercent, L"iCityDefense");
	pXML->GetOptionalChildXmlValByName(&m_iHillsAttackPercent, L"iHillsAttack");
	pXML->GetOptionalChildXmlValByName(&m_iHillsDefensePercent, L"iHillsDefense");
	pXML->GetOptionalChildXmlValByName(&m_iHillsWorkPercent, L"iHillsWorkModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPeaksWorkPercent, L"iPeaksWorkModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorkRatePercent, L"iWorkRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevoltProtection, L"iRevoltProtection");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageProtection, L"iCollateralDamageProtection");
	pXML->GetOptionalChildXmlValByName(&m_iPillageChange, L"iPillageChange");
	pXML->GetOptionalChildXmlValByName(&m_iUpgradeDiscount, L"iUpgradeDiscount");
	pXML->GetOptionalChildXmlValByName(&m_iExperiencePercent, L"iExperiencePercent");
	pXML->GetOptionalChildXmlValByName(&m_iKamikazePercent, L"iKamikazePercent");
	pXML->GetOptionalChildXmlValByName(&m_bDefensiveVictoryMove, L"bDefensiveVictoryMove");
	pXML->GetOptionalChildXmlValByName(&m_bFreeDrop, L"bFreeDrop");
	pXML->GetOptionalChildXmlValByName(&m_bOffensiveVictoryMove, L"bOffensiveVictoryMove");
	pXML->GetOptionalChildXmlValByName(&m_bOneUp, L"bOneUp");
	pXML->GetOptionalChildXmlValByName(&m_bPillageEspionage, L"bPillageEspionage");
	pXML->GetOptionalChildXmlValByName(&m_bPillageMarauder, L"bPillageMarauder");
	pXML->GetOptionalChildXmlValByName(&m_bPillageOnMove, L"bPillageOnMove");
	pXML->GetOptionalChildXmlValByName(&m_bPillageOnVictory, L"bPillageOnVictory");
	pXML->GetOptionalChildXmlValByName(&m_bPillageResearch, L"bPillageResearch");
	pXML->GetOptionalChildXmlValByName(&m_iAirCombatLimitChange, L"iAirCombatLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iCelebrityHappy, L"iCelebrityHappy");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageLimitChange, L"iCollateralDamageLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageMaxUnitsChange, L"iCollateralDamageMaxUnitsChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatLimitChange, L"iCombatLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iExtraDropRange, L"iExtraDropRange");
	pXML->GetOptionalChildXmlValByName(&m_iSurvivorChance, L"iSurvivorChance");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryAdjacentHeal, L"iVictoryAdjacentHeal");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryHeal, L"iVictoryHeal");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryStackHeal, L"iVictoryStackHeal");

	pXML->SetVariableListTagPair(&m_piTerrainAttackPercent, L"TerrainAttacks", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piTerrainDefensePercent, L"TerrainDefenses", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piFeatureAttackPercent, L"FeatureAttacks", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piFeatureDefensePercent, L"FeatureDefenses", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piUnitCombatModifierPercent, L"UnitCombatMods", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piDomainModifierPercent, L"DomainMods", NUM_DOMAIN_TYPES);
	pXML->SetVariableListTagPair(&m_pbTerrainDoubleMove, L"TerrainDoubleMoves", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureDoubleMove, L"FeatureDoubleMoves", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_pbUnitCombat, L"UnitCombats", GC.getNumUnitCombatInfos());
	//ls612: Terrain Work Modifiers
	pXML->SetVariableListTagPair(&m_piTerrainWorkPercent, L"TerrainWorks", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piFeatureWorkPercent, L"FeatureWorks", GC.getNumFeatureInfos());

	pXML->GetOptionalChildXmlValByName(&m_bCanMovePeaks, L"bCanMovePeaks");
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	pXML->GetOptionalChildXmlValByName(&m_bCanLeadThroughPeaks, L"bCanLeadThroughPeaks");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = static_cast<TechTypes>(pXML->GetInfoClass(szTextVal));
	pXML->GetOptionalChildXmlValByName(&m_iControlPoints, L"iControlPoints");
	pXML->GetOptionalChildXmlValByName(&m_iCommandRange, L"iCommandRange");
	pXML->GetOptionalChildXmlValByName(&m_bZoneOfControl, L"bZoneOfControl");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"IgnoreTerrainDamage");
	m_iIgnoreTerrainDamage = pXML->GetInfoClass(szTextVal);

	m_PropertyManipulators.read(pXML);

	//TB Combat Mods Begin  TB SubCombat Mod begin
	//Text Strings
	pXML->GetOptionalChildXmlValByName(m_szRenamesUnitTo, L"RenamesUnitTo");

	//Textual References
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionLine");
	m_ePromotionLine = (PromotionLineTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ReplacesUnitCombat");
	m_eReplacesUnitCombat = (UnitCombatTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DomainCargoChange");
	m_eDomainCargoChange = (DomainTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SpecialCargoChange");
	m_eSpecialCargoChange = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SpecialCargoPrereq");
	m_eSpecialCargoPrereq = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SMNotSpecialCargoChange");
	m_eSMNotSpecialCargoChange = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SMNotSpecialCargoPrereq");
	m_eSMNotSpecialCargoPrereq = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SetSpecialUnit");
	m_eSetSpecialUnit = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);
	//integers
	pXML->GetOptionalChildXmlValByName(&m_iAttackCombatModifierChange, L"iAttackCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseCombatModifierChange, L"iDefenseCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPursuitChange, L"iPursuitChange");
	pXML->GetOptionalChildXmlValByName(&m_iEarlyWithdrawChange, L"iEarlyWithdrawChange");
	pXML->GetOptionalChildXmlValByName(&m_iVSBarbsChange, L"iVSBarbsChange");
	pXML->GetOptionalChildXmlValByName(&m_iArmorChange, L"iArmorChange");
	pXML->GetOptionalChildXmlValByName(&m_iPunctureChange, L"iPunctureChange");
	pXML->GetOptionalChildXmlValByName(&m_iOverrunChange, L"iOverrunChange");
	pXML->GetOptionalChildXmlValByName(&m_iRepelChange, L"iRepelChange");
	pXML->GetOptionalChildXmlValByName(&m_iFortRepelChange, L"iFortRepelChange");
	pXML->GetOptionalChildXmlValByName(&m_iRepelRetriesChange, L"iRepelRetriesChange");
	pXML->GetOptionalChildXmlValByName(&m_iUnyieldingChange, L"iUnyieldingChange");
	pXML->GetOptionalChildXmlValByName(&m_iKnockbackChange, L"iKnockbackChange");
	pXML->GetOptionalChildXmlValByName(&m_iKnockbackRetriesChange, L"iKnockbackRetriesChange");
#ifdef BATTLEWORN
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperRndChange, L"iStrAdjperRndChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperAttChange, L"iStrAdjperAttChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperDefChange, L"iStrAdjperDefChange");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawAdjperAttChange, L"iWithdrawAdjperAttChange");
#endif // BATTLEWORN
	pXML->GetOptionalChildXmlValByName(&m_iUnnerveChange, L"iUnnerveChange");
	pXML->GetOptionalChildXmlValByName(&m_iEncloseChange, L"iEncloseChange");
	pXML->GetOptionalChildXmlValByName(&m_iLungeChange, L"iLungeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDynamicDefenseChange, L"iDynamicDefenseChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthChange, L"iStrengthChange");
	pXML->GetOptionalChildXmlValByName(&m_iLinePriority, L"iLinePriority");
	pXML->GetOptionalChildXmlValByName(&m_iFortitudeChange, L"iFortitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDamageperTurn, L"iDamageperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperTurn, L"iStrAdjperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iWeakenperTurn, L"iWeakenperTurn");
#ifdef STRENGTH_IN_NUMBERS
	pXML->GetOptionalChildXmlValByName(&m_iFrontSupportPercentChange, L"iFrontSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iShortRangeSupportPercentChange, L"iShortRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iMediumRangeSupportPercentChange, L"iMediumRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iLongRangeSupportPercentChange, L"iLongRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iFlankSupportPercentChange, L"iFlankSupportPercentChange");
#endif
	pXML->GetOptionalChildXmlValByName(&m_iDodgeModifierChange, L"iDodgeModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPrecisionModifierChange, L"iPrecisionModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotsChange, L"iPowerShotsChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotCombatModifierChange, L"iPowerShotCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotPunctureModifierChange, L"iPowerShotPunctureModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotPrecisionModifierChange, L"iPowerShotPrecisionModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotCriticalModifierChange, L"iPowerShotCriticalModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCriticalModifierChange, L"iCriticalModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iEnduranceChange, L"iEnduranceChange");
	pXML->GetOptionalChildXmlValByName(&m_iRoundStunProbChange, L"iRoundStunProbChange");
	pXML->GetOptionalChildXmlValByName(&m_iPoisonProbabilityModifierChange, L"iPoisonProbabilityModifierChange");

	pXML->GetOptionalChildXmlValByName(&m_iCaptureProbabilityModifierChange, L"iCaptureProbabilityModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCaptureResistanceModifierChange, L"iCaptureResistanceModifierChange");

	pXML->GetOptionalChildXmlValByName(&m_iBreakdownChanceChange, L"iBreakdownChanceChange");
	pXML->GetOptionalChildXmlValByName(&m_iBreakdownDamageChange, L"iBreakdownDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iTauntChange, L"iTauntChange");
	pXML->GetOptionalChildXmlValByName(&m_iMaxHPChange, L"iMaxHPChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthModifier, L"iStrengthModifier");
	pXML->GetOptionalChildXmlValByName(&m_iQualityChange, L"iQualityChange");
	pXML->GetOptionalChildXmlValByName(&m_iGroupChange, L"iGroupChange");
	pXML->GetOptionalChildXmlValByName(&m_iLevelPrereq, L"iLevelPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iDamageModifierChange, L"iDamageModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iUpkeepModifier, L"iUpkeepModifier");
	pXML->GetOptionalChildXmlValByName(&m_iExtraUpkeep100, L"iExtraUpkeep100");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageChange, L"iRBombardDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageLimitChange, L"iRBombardDamageLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageMaxUnitsChange, L"iRBombardDamageMaxUnitsChange");
	pXML->GetOptionalChildXmlValByName(&m_iDCMBombRangeChange, L"iDCMBombRangeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDCMBombAccuracyChange, L"iDCMBombAccuracyChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerSizeMoreChange, L"iCombatModifierPerSizeMoreChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerSizeLessChange, L"iCombatModifierPerSizeLessChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerVolumeMoreChange, L"iCombatModifierPerVolumeMoreChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerVolumeLessChange, L"iCombatModifierPerVolumeLessChange");
	pXML->GetOptionalChildXmlValByName(&m_iSelfHealModifier, L"iSelfHealModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumHealSupport, L"iNumHealSupport");
	pXML->GetOptionalChildXmlValByName(&m_iExcileChange, L"iExcileChange");
	pXML->GetOptionalChildXmlValByName(&m_iPassageChange, L"iPassageChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoNonOwnedCityEntryChange, L"iNoNonOwnedCityEntryChange");
	pXML->GetOptionalChildXmlValByName(&m_iBarbCoExistChange, L"iBarbCoExistChange");
	pXML->GetOptionalChildXmlValByName(&m_iBlendIntoCityChange, L"iBlendIntoCityChange");
	pXML->GetOptionalChildXmlValByName(&m_iUpgradeAnywhereChange, L"iUpgradeAnywhereChange");
	pXML->GetOptionalChildXmlValByName(&m_iInsidiousnessChange, L"iInsidiousnessChange");
	pXML->GetOptionalChildXmlValByName(&m_iInvestigationChange, L"iInvestigationChange");
	pXML->GetOptionalChildXmlValByName(&m_iAssassinChange, L"iAssassinChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthStrikesChange, L"iStealthStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthCombatModifierChange, L"iStealthCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthDefenseChange, L"iStealthDefenseChange");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseOnlyChange, L"iDefenseOnlyChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoInvisibilityChange, L"iNoInvisibilityChange");
	pXML->GetOptionalChildXmlValByName(&m_iTrapDamageMax, L"iTrapDamageMax");
	pXML->GetOptionalChildXmlValByName(&m_iTrapDamageMin, L"iTrapDamageMin");
	pXML->GetOptionalChildXmlValByName(&m_iTrapComplexity, L"iTrapComplexity");
	pXML->GetOptionalChildXmlValByName(&m_iNumTriggers, L"iNumTriggers");
	pXML->GetOptionalChildXmlValByName(&m_iTriggerBeforeAttackChange, L"iTriggerBeforeAttackChange");
	pXML->GetOptionalChildXmlValByName(&m_iHiddenNationalityChange, L"iHiddenNationalityChange");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalIgnoresBordersChange, L"iAnimalIgnoresBordersChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoDefensiveBonusChange, L"iNoDefensiveBonusChange");
	pXML->GetOptionalChildXmlValByName(&m_iGatherHerdChange, L"iGatherHerdChange");
	pXML->GetOptionalChildXmlValByName(&m_iReligiousCombatModifierChange, L"iReligiousCombatModifierChange");
	//booleans
	pXML->GetOptionalChildXmlValByName(&m_bStampedeChange, L"bStampedeChange");
	pXML->GetOptionalChildXmlValByName(&m_bRemoveStampede, L"bRemoveStampede");
	pXML->GetOptionalChildXmlValByName(&m_bOnslaughtChange, L"bOnslaughtChange");
	pXML->GetOptionalChildXmlValByName(&m_bEquipment, L"bEquipment");
#ifdef OUTBREAKS_AND_AFFLICTIONS
	pXML->GetOptionalChildXmlValByName(&m_bAffliction, L"bAffliction");
#endif
	pXML->GetOptionalChildXmlValByName(&m_bParalyze, L"bParalyze");
	pXML->GetOptionalChildXmlValByName(&m_bMakesDamageCold, L"bMakesDamageCold");
	pXML->GetOptionalChildXmlValByName(&m_bMakesDamageNotCold, L"bMakesDamageNotCold");
	pXML->GetOptionalChildXmlValByName(&m_bAddsColdImmunity, L"bAddsColdImmunity");
	pXML->GetOptionalChildXmlValByName(&m_bRemovesColdImmunity, L"bRemovesColdImmunity");
	pXML->GetOptionalChildXmlValByName(&m_bCritical, L"bCritical");
	pXML->GetOptionalChildXmlValByName(&m_bAttackOnlyCitiesAdd, L"bAttackOnlyCitiesAdd");
	pXML->GetOptionalChildXmlValByName(&m_bAttackOnlyCitiesSubtract, L"bAttackOnlyCitiesSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreNoEntryLevelAdd, L"bIgnoreNoEntryLevelAdd");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreNoEntryLevelSubtract, L"bIgnoreNoEntryLevelSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreZoneofControlAdd, L"bIgnoreZoneofControlAdd");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreZoneofControlSubtract, L"bIgnoreZoneofControlSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bFliesToMoveAdd, L"bFliesToMoveAdd");
	pXML->GetOptionalChildXmlValByName(&m_bFliesToMoveSubtract, L"bFliesToMoveSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bZeroesXP, L"bZeroesXP");
	pXML->GetOptionalChildXmlValByName(&m_bForOffset, L"bForOffset");
	pXML->GetOptionalChildXmlValByName(&m_bCargoPrereq, L"bCargoPrereq");
	pXML->GetOptionalChildXmlValByName(&m_bRBombardPrereq, L"bRBombardPrereq");
	pXML->GetOptionalChildXmlValByName(&m_bNoSelfHeal, L"bNoSelfHeal");
	pXML->GetOptionalChildXmlValByName(&m_bSetOnHNCapture, L"bSetOnHNCapture");
	pXML->GetOptionalChildXmlValByName(&m_bSetOnInvestigated, L"bSetOnInvestigated");
	pXML->GetOptionalChildXmlValByName(&m_bStatus, L"bStatus");
	pXML->GetOptionalChildXmlValByName(&m_bPrereqNormInvisible, L"bPrereqNormInvisible");
	pXML->GetOptionalChildXmlValByName(&m_bPlotPrereqsKeepAfter, L"bPlotPrereqsKeepAfter");
	pXML->GetOptionalChildXmlValByName(&m_bRemoveAfterSet, L"bRemoveAfterSet");
	pXML->GetOptionalChildXmlValByName(&m_bQuick, L"bQuick");
	pXML->GetOptionalChildXmlValByName(&m_bStarsign, L"bStarsign");
	//pXML->SetVariableListTagPair(&m_piAIWeightbyUnitCombatTypes, L"AIWeightbyUnitCombatTypes", GC.getNumUnitCombatInfos());
	pXML->SetOptionalVector(&m_aiSubCombatChangeTypes, L"SubCombatChangeTypes");
	pXML->SetOptionalVector(&m_aiRemovesUnitCombatTypes, L"RemovesUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	pXML->SetOptionalVector(&m_aiFreetoUnitCombats, L"FreetoUnitCombats");
	pXML->SetOptionalVector(&m_aiNotOnUnitCombatTypes, L"NotOnUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiNotOnDomainTypes, L"NotOnDomainTypes");
	pXML->SetOptionalVector(&m_aiNoAutoEquiptoCombatClassTypes, L"NoAutoEquiptoCombatClassTypes");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");
#ifdef OUTBREAKS_AND_AFFLICTIONS
	pXML->SetOptionalVector(&m_aiCureAfflictionChangeTypes, L"CureAfflictionChangeTypes");
#endif // OUTBREAKS_AND_AFFLICTIONS
	pXML->SetOptionalVector(&m_aiPrereqBonusTypes, L"PrereqBonusTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiAddsBuildTypes, L"AddsBuildTypes");
	pXML->SetOptionalVector(&m_aiNegatesInvisibilityTypes, L"NegatesInvisibilityTypes");
	pXML->SetOptionalVector(&m_aiPrereqTerrainTypes, L"PrereqTerrainTypes");
	pXML->SetOptionalVector(&m_aiPrereqFeatureTypes, L"PrereqFeatureTypes");
	pXML->SetOptionalVector(&m_aiPrereqImprovementTypes, L"PrereqImprovementTypes");
	pXML->SetOptionalVector(&m_aiPrereqPlotBonusTypes, L"PrereqPlotBonusTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiPrereqLocalBuildingTypes, L"PrereqLocalBuildingTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiTrapSetWithPromotionTypes, L"TrapSetWithPromotionTypes");
	pXML->SetOptionalVector(&m_aiTrapImmunityUnitCombatTypes, L"TrapImmunityUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiTargetUnitCombatTypes, L"TargetUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	// int vector utilizing pairing without delayed resolution
	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aFlankingStrengthbyUnitCombatTypeChange, L"FlankingStrikesbyUnitCombatChange");

	pXML->SetOptionalPairVector<TerrainModifierArray, TerrainTypes, int>(&m_aWithdrawOnTerrainTypesChange, L"WithdrawModifierOnTerrainTypeChanges");

	pXML->SetOptionalPairVector<FeatureModifierArray, FeatureTypes, int>(&m_aWithdrawOnFeatureTypesChange, L"WithdrawModifierOnFeatureTypeChanges");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aWithdrawVSUnitCombatChangeTypes, L"WithdrawVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPursuitVSUnitCombatChangeTypes, L"PursuitVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aRepelVSUnitCombatChangeTypes, L"RepelVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aKnockbackVSUnitCombatChangeTypes, L"KnockbackVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPunctureVSUnitCombatChangeTypes, L"PunctureVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aArmorVSUnitCombatChangeTypes, L"ArmorVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aDodgeVSUnitCombatChangeTypes, L"DodgeVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPrecisionVSUnitCombatChangeTypes, L"PrecisionVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aCriticalVSUnitCombatChangeTypes, L"CriticalVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aRoundStunVSUnitCombatChangeTypes, L"RoundStunVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapDisableUnitCombatTypes, L"TrapDisableUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapAvoidanceUnitCombatTypes, L"TrapAvoidanceUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapTriggerUnitCombatTypes, L"TrapTriggerUnitCombatTypes");

#ifdef OUTBREAKS_AND_AFFLICTIONS
	pXML->SetOptionalPairVector<AidArray, PropertyTypes, int>(&m_aAidChanges, L"AidChanges");
#endif
	//pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(m_aAIWeightbyUnitCombatTypes, L"AIWeightbyUnitCombatTypes");

	pXML->SetOptionalPairVector<BuildModifierArray, BuildTypes, int>(&m_aBuildWorkRateModifierChangeTypes, L"BuildWorkRateModifierChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aVisibilityIntensityChangeTypes, L"VisibilityIntensityChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aInvisibilityIntensityChangeTypes, L"InvisibilityIntensityChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aVisibilityIntensityRangeChangeTypes, L"VisibilityIntensityRangeChangeTypes");

	// int vector utilizing struct with delayed resolution
	if(pXML->TryMoveToXmlFirstChild(L"AIWeightbyUnitCombatTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AIWeightbyUnitCombatType" );
		m_aAIWeightbyUnitCombatTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"AIWeightbyUnitCombatType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aAIWeightbyUnitCombatTypes[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAIWeightbyUnitCombatTypes[i].iModifier), L"iAIWeight");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AIWeightbyUnitCombatType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

#ifdef OUTBREAKS_AND_AFFLICTIONS
	if(pXML->TryMoveToXmlFirstChild(L"AfflictionFortitudeChangeModifiers"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AfflictionFortitudeChangeModifier" );
		m_aAfflictionFortitudeChangeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionFortitudeChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionFortitudeChangeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AfflictionFortitudeChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"AfflictOnAttackChangeTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AfflictOnAttackChangeType" );
		m_aAfflictOnAttackChangeTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictOnAttackChangeType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictOnAttackChangeTypes[i].eAfflictionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iProbabilityChange), L"iProbability");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iMelee), L"iMelee");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iDistance), L"iDistance");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iImmediate), L"iImmediate");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AfflictOnAttackChangeType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
#endif // OUTBREAKS_AND_AFFLICTIONS

	if(pXML->TryMoveToXmlFirstChild(L"HealUnitCombatChangeTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"HealUnitCombatChangeType" );
		m_aHealUnitCombatChangeTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"HealUnitCombatChangeType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aHealUnitCombatChangeTypes[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aHealUnitCombatChangeTypes[i].iHeal), L"iHeal");
					pXML->GetChildXmlValByName(&(m_aHealUnitCombatChangeTypes[i].iAdjacentHeal), L"iAdjacentHeal");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"HealUnitCombatChangeType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"InvisibleTerrainChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"InvisibleTerrainChange" );
		m_aInvisibleTerrainChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"InvisibleTerrainChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aInvisibleTerrainChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aInvisibleTerrainChanges[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aInvisibleTerrainChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"InvisibleTerrainChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"InvisibleFeatureChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"InvisibleFeatureChange" );
		m_aInvisibleFeatureChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"InvisibleFeatureChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aInvisibleFeatureChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aInvisibleFeatureChanges[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aInvisibleFeatureChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"InvisibleFeatureChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"InvisibleImprovementChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"InvisibleImprovementChange" );
		m_aInvisibleImprovementChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"InvisibleImprovementChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aInvisibleImprovementChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					m_aInvisibleImprovementChanges[i].eImprovement = (ImprovementTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aInvisibleImprovementChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"InvisibleImprovementChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleTerrainChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleTerrainChange" );
		m_aVisibleTerrainChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleTerrainChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleTerrainChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aVisibleTerrainChanges[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleTerrainChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleTerrainChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleFeatureChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleFeatureChange" );
		m_aVisibleFeatureChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleFeatureChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleFeatureChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aVisibleFeatureChanges[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleFeatureChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleFeatureChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleImprovementChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleImprovementChange" );
		m_aVisibleImprovementChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleImprovementChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleImprovementChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					m_aVisibleImprovementChanges[i].eImprovement = (ImprovementTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleImprovementChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleImprovementChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleTerrainRangeChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleTerrainRangeChange" );
		m_aVisibleTerrainRangeChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleTerrainRangeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					pXML->GetChildXmlValByName(szTextVal2, L"TerrainType");
					m_aVisibleTerrainRangeChanges[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal2);
					pXML->GetChildXmlValByName(&(m_aVisibleTerrainRangeChanges[i].iIntensity), L"iIntensity");
					GC.addDelayedResolution((int*)&(m_aVisibleTerrainRangeChanges[i].eInvisible), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleTerrainRangeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleFeatureRangeChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleFeatureRangeChange" );
		m_aVisibleFeatureRangeChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleFeatureRangeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleFeatureRangeChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aVisibleFeatureRangeChanges[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleFeatureRangeChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleFeatureRangeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleImprovementRangeChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleImprovementRangeChange" );
		m_aVisibleImprovementRangeChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleImprovementRangeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleImprovementRangeChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					m_aVisibleImprovementRangeChanges[i].eImprovement = (ImprovementTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleImprovementRangeChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleImprovementRangeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"DistanceAttackCommunicabilityTypeChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"DistanceAttackCommunicabilityTypeChange" );
		m_aDistanceAttackCommunicabilityTypeChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"DistanceAttackCommunicabilityTypeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aDistanceAttackCommunicabilityTypeChanges[i].iChange), L"iChange");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"DistanceAttackCommunicabilityTypeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
		//TB Combat Mod End
	//pXML->SetOptionalVectorWithDelayedResolution(PromotionOverwrites, L"PromotionOverwrites");

	return true;
}

void CvPromotionInfo::copyNonDefaults(const CvPromotionInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	CvInfoUtil(this).copyNonDefaults(pClassInfo);

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();

	if (getLayerAnimationPath() == ANIMATIONPATH_NONE) m_iLayerAnimationPath = pClassInfo->getLayerAnimationPath();
	if (getTechPrereq() == NO_TECH) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getStateReligionPrereq() == NO_RELIGION) m_iStateReligionPrereq = pClassInfo->getStateReligionPrereq();
	if (getMinEraType() == NO_ERA) m_iMinEraType = pClassInfo->getMinEraType();
	if (getMaxEraType() == NO_ERA) m_iMaxEraType = pClassInfo->getMaxEraType();

	if (isLeader() == bDefault) m_bLeader = pClassInfo->isLeader();
	if (m_bLeader)
	{
		m_bGraphicalOnly = true;  // don't show in Civilopedia list of promotions
	}
	if (isBlitz() == bDefault) m_bBlitz = pClassInfo->isBlitz();
	if (isAmphib() == bDefault) m_bAmphib = pClassInfo->isAmphib();
	if (isRiver() == bDefault) m_bRiver = pClassInfo->isRiver();
	if (isEnemyRoute() == bDefault) m_bEnemyRoute = pClassInfo->isEnemyRoute();
	if (isAlwaysHeal() == bDefault) m_bAlwaysHeal = pClassInfo->isAlwaysHeal();
	if (isHillsDoubleMove() == bDefault) m_bHillsDoubleMove = pClassInfo->isHillsDoubleMove();
	if (isImmuneToFirstStrikes() == bDefault) m_bImmuneToFirstStrikes = pClassInfo->isImmuneToFirstStrikes();
	if (getVisibilityChange() == iDefault) m_iVisibilityChange = pClassInfo->getVisibilityChange();
	if (getMovesChange() == iDefault) m_iMovesChange = pClassInfo->getMovesChange();
	if (getMoveDiscountChange() == iDefault) m_iMoveDiscountChange = pClassInfo->getMoveDiscountChange();
	if (getAirRangeChange() == iDefault) m_iAirRangeChange = pClassInfo->getAirRangeChange();
	if (getInterceptChange() == iDefault) m_iInterceptChange = pClassInfo->getInterceptChange();
	if (getEvasionChange() == iDefault) m_iEvasionChange = pClassInfo->getEvasionChange();
	if (getWithdrawalChange() == iDefault) m_iWithdrawalChange = pClassInfo->getWithdrawalChange();
	if (getCargoChange() == iDefault) m_iCargoChange = pClassInfo->getCargoChange();
	if (getSMCargoChange() == iDefault) m_iSMCargoChange = pClassInfo->getSMCargoChange();
	if (getSMCargoVolumeChange() == iDefault) m_iSMCargoVolumeChange = pClassInfo->getSMCargoVolumeChange();
	if (getSMCargoVolumeModifierChange() == iDefault) m_iSMCargoVolumeModifierChange = pClassInfo->getSMCargoVolumeModifierChange();
	if (getCollateralDamageChange() == iDefault) m_iCollateralDamageChange = pClassInfo->getCollateralDamageChange();
	if (getBombardRateChange() == iDefault) m_iBombardRateChange = pClassInfo->getBombardRateChange();
	if (getFirstStrikesChange() == iDefault) m_iFirstStrikesChange = pClassInfo->getFirstStrikesChange();
	if (getChanceFirstStrikesChange() == iDefault) m_iChanceFirstStrikesChange = pClassInfo->getChanceFirstStrikesChange();
	if (getEnemyHealChange() == iDefault) m_iEnemyHealChange = pClassInfo->getEnemyHealChange();
	if (getNeutralHealChange() == iDefault) m_iNeutralHealChange = pClassInfo->getNeutralHealChange();
	if (getFriendlyHealChange() == iDefault) m_iFriendlyHealChange = pClassInfo->getFriendlyHealChange();
	if (getSameTileHealChange() == iDefault) m_iSameTileHealChange = pClassInfo->getSameTileHealChange();
	if (getAdjacentTileHealChange() == iDefault) m_iAdjacentTileHealChange = pClassInfo->getAdjacentTileHealChange();
	if (getCombatPercent() == iDefault) m_iCombatPercent = pClassInfo->getCombatPercent();
	if (getCityAttackPercent() == iDefault) m_iCityAttackPercent = pClassInfo->getCityAttackPercent();
	if (getCityDefensePercent() == iDefault) m_iCityDefensePercent = pClassInfo->getCityDefensePercent();
	if (getHillsAttackPercent() == iDefault) m_iHillsAttackPercent = pClassInfo->getHillsAttackPercent();
	if (getHillsDefensePercent() == iDefault) m_iHillsDefensePercent = pClassInfo->getHillsDefensePercent();
	if (getHillsWorkPercent() == iDefault) m_iHillsWorkPercent = pClassInfo->getHillsWorkPercent();
	if (getPeaksWorkPercent() == iDefault) m_iPeaksWorkPercent = pClassInfo->getPeaksWorkPercent();
	if (getWorkRatePercent() == iDefault) m_iWorkRatePercent = pClassInfo->getWorkRatePercent();
	if (getRevoltProtection() == iDefault) m_iRevoltProtection = pClassInfo->getRevoltProtection();
	if (getCollateralDamageProtection() == iDefault) m_iCollateralDamageProtection = pClassInfo->getCollateralDamageProtection();
	if (getPillageChange() == iDefault) m_iPillageChange = pClassInfo->getPillageChange();
	if (getUpgradeDiscount() == iDefault) m_iUpgradeDiscount = pClassInfo->getUpgradeDiscount();
	if (getExperiencePercent() == iDefault) m_iExperiencePercent = pClassInfo->getExperiencePercent();
	if (getKamikazePercent() == iDefault) m_iKamikazePercent = pClassInfo->getKamikazePercent();

	for (int j = 0; j < GC.getNumTerrainInfos(); j++)
	{
		if ((m_piTerrainAttackPercent == NULL || m_piTerrainAttackPercent[j] == iDefault) &&
			pClassInfo->getTerrainAttackPercent(j) != iDefault)
		{
			if ( m_piTerrainAttackPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainAttackPercent,GC.getNumTerrainInfos(),iDefault);
			}
			m_piTerrainAttackPercent[j] = pClassInfo->getTerrainAttackPercent(j);
		}

		if ((m_piTerrainDefensePercent == NULL ||m_piTerrainDefensePercent[j] == iDefault) &&
			pClassInfo->getTerrainDefensePercent(j) != iDefault)
		{
			if ( m_piTerrainDefensePercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainDefensePercent,GC.getNumTerrainInfos(),iDefault);
			}
			m_piTerrainDefensePercent[j] = pClassInfo->getTerrainDefensePercent(j);
		}

		if ((m_pbTerrainDoubleMove == NULL ||m_pbTerrainDoubleMove[j] == bDefault) &&
			pClassInfo->getTerrainDoubleMove(j) != bDefault)
		{
			if ( m_pbTerrainDoubleMove == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrainDoubleMove,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbTerrainDoubleMove[j] = pClassInfo->getTerrainDoubleMove(j);
		}

		//ls612: Terrain Work Modifiers
		if ((m_piTerrainWorkPercent == NULL || m_piTerrainWorkPercent[j] == iDefault) &&
			pClassInfo->getTerrainWorkPercent(j) != iDefault)
		{
			if ( m_piTerrainWorkPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainWorkPercent,GC.getNumTerrainInfos(),iDefault);
			}
			m_piTerrainWorkPercent[j] = pClassInfo->getTerrainWorkPercent(j);
		}
	}
	for (int j = 0; j < GC.getNumFeatureInfos(); j++)
	{
		if ((m_piFeatureAttackPercent == NULL || m_piFeatureAttackPercent[j] == iDefault) &&
			pClassInfo->getFeatureAttackPercent(j) != iDefault)
		{
			if ( m_piFeatureAttackPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFeatureAttackPercent,GC.getNumFeatureInfos(),iDefault);
			}
			m_piFeatureAttackPercent[j] = pClassInfo->getFeatureAttackPercent(j);
		}

		if ((m_piFeatureDefensePercent == NULL ||m_piFeatureDefensePercent[j] == iDefault) &&
			pClassInfo->getFeatureDefensePercent(j) != iDefault)
		{
			if ( m_piFeatureDefensePercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFeatureDefensePercent,GC.getNumFeatureInfos(),iDefault);
			}
			m_piFeatureDefensePercent[j] = pClassInfo->getFeatureDefensePercent(j);
		}

		if ((m_pbFeatureDoubleMove == NULL ||m_pbFeatureDoubleMove[j] == bDefault) &&
			pClassInfo->getFeatureDoubleMove(j) != bDefault)
		{
			if ( m_pbFeatureDoubleMove == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbFeatureDoubleMove,GC.getNumFeatureInfos(),bDefault);
			}
			m_pbFeatureDoubleMove[j] = pClassInfo->getFeatureDoubleMove(j);
		}
		//ls612: Terrain Work Modifiers
		if ((m_piFeatureWorkPercent == NULL || m_piFeatureWorkPercent[j] == iDefault) &&
			pClassInfo->getFeatureWorkPercent(j) != iDefault)
		{
			if ( m_piFeatureWorkPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFeatureWorkPercent,GC.getNumFeatureInfos(),iDefault);
			}
			m_piFeatureWorkPercent[j] = pClassInfo->getFeatureWorkPercent(j);
		}
	}
	for (int j = 0; j < GC.getNumUnitCombatInfos(); j++)
	{
		if ((m_piUnitCombatModifierPercent == NULL || m_piUnitCombatModifierPercent[j] == iDefault) &&
			pClassInfo->getUnitCombatModifierPercent(j) != iDefault)
		{
			if ( m_piUnitCombatModifierPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piUnitCombatModifierPercent,GC.getNumUnitCombatInfos(),iDefault);
			}
			m_piUnitCombatModifierPercent[j] = pClassInfo->getUnitCombatModifierPercent(j);
		}

		if ((m_pbUnitCombat == NULL || m_pbUnitCombat[j] == bDefault) &&
			pClassInfo->getUnitCombat(j) != bDefault)
		{
			if ( m_pbUnitCombat == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbUnitCombat,GC.getNumUnitCombatInfos(),bDefault);
			}
			m_pbUnitCombat[j] = pClassInfo->getUnitCombat(j);
		}

		//if ((m_piAIWeightbyUnitCombatTypes == NULL || m_piAIWeightbyUnitCombatTypes[j] == iDefault) &&
		//	pClassInfo->getAIWeightbyUnitCombatType(j) != iDefault)
		//{
		//	if ( m_piAIWeightbyUnitCombatTypes == NULL )
		//	{
		//		CvXMLLoadUtility::InitList(&m_piAIWeightbyUnitCombatTypes,GC.getNumUnitCombatInfos(),iDefault);
		//	}
		//	m_piAIWeightbyUnitCombatTypes[j] = pClassInfo->getAIWeightbyUnitCombatType(j);
		//}
	}
	for (int j = 0; j < NUM_DOMAIN_TYPES; j++)
	{
		if ((m_piDomainModifierPercent == NULL || m_piDomainModifierPercent[j] == iDefault) &&
			pClassInfo->getDomainModifierPercent(j) != iDefault)
		{
			if ( m_piDomainModifierPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piDomainModifierPercent,NUM_DOMAIN_TYPES,iDefault);
			}
			m_piDomainModifierPercent[j] = pClassInfo->getDomainModifierPercent(j);
		}
	}

	if (isDefensiveVictoryMove() == bDefault) m_bDefensiveVictoryMove = pClassInfo->isDefensiveVictoryMove();
	if (isFreeDrop() == bDefault) m_bFreeDrop = pClassInfo->isFreeDrop();
	if (isOffensiveVictoryMove() == bDefault) m_bOffensiveVictoryMove = pClassInfo->isOffensiveVictoryMove();
	if (isOneUp() == bDefault) m_bOneUp = pClassInfo->isOneUp();
	if (isPillageEspionage() == bDefault) m_bPillageEspionage = pClassInfo->isPillageEspionage();
	if (isPillageMarauder() == bDefault) m_bPillageMarauder = pClassInfo->isPillageMarauder();
	if (isPillageOnMove() == bDefault) m_bPillageOnMove = pClassInfo->isPillageOnMove();
	if (isPillageOnVictory() == bDefault) m_bPillageOnVictory = pClassInfo->isPillageOnVictory();
	if (isPillageResearch() == bDefault) m_bPillageResearch = pClassInfo->isPillageResearch();
	if (getAirCombatLimitChange() == iDefault) m_iAirCombatLimitChange = pClassInfo->getAirCombatLimitChange();
	if (getCelebrityHappy() == iDefault) m_iCelebrityHappy = pClassInfo->getCelebrityHappy();
	if (getCollateralDamageLimitChange() == iDefault) m_iCollateralDamageLimitChange = pClassInfo->getCollateralDamageLimitChange();
	if (getCollateralDamageMaxUnitsChange() == iDefault) m_iCollateralDamageMaxUnitsChange = pClassInfo->getCollateralDamageMaxUnitsChange();
	if (getCombatLimitChange() == iDefault) m_iCombatLimitChange = pClassInfo->getCombatLimitChange();
	if (getExtraDropRange() == iDefault) m_iExtraDropRange = pClassInfo->getExtraDropRange();
	if (getSurvivorChance() == iDefault) m_iSurvivorChance = pClassInfo->getSurvivorChance();
	if (getVictoryAdjacentHeal() == iDefault) m_iVictoryAdjacentHeal = pClassInfo->getVictoryAdjacentHeal();
	if (getVictoryHeal() == iDefault) m_iVictoryHeal = pClassInfo->getVictoryHeal();
	if (getVictoryStackHeal() == iDefault) m_iVictoryStackHeal = pClassInfo->getVictoryStackHeal();

	//TB Combat Mods Begin  TB SubCombat Mods Begin
	// Text Strings
	if ( getRenamesUnitTo() == NULL || getRenamesUnitTo() == wDefault )
	{
		m_szRenamesUnitTo = pClassInfo->getRenamesUnitTo();
	}
	// Textual References
	if (getPromotionLine() == NO_PROMOTIONLINE) m_ePromotionLine = pClassInfo->getPromotionLine();
	if (getReplacesUnitCombat() == NO_UNITCOMBAT) m_eReplacesUnitCombat = pClassInfo->getReplacesUnitCombat();
	if (getDomainCargoChange() == NO_DOMAIN) m_eDomainCargoChange = pClassInfo->getDomainCargoChange();
	if (getSpecialCargoChange() == NO_SPECIALUNIT) m_eSpecialCargoChange = pClassInfo->getSpecialCargoChange();
	if (getSpecialCargoPrereq() == NO_SPECIALUNIT) m_eSpecialCargoPrereq = pClassInfo->getSpecialCargoPrereq();
	if (getSMNotSpecialCargoChange() == NO_SPECIALUNIT) m_eSMNotSpecialCargoChange = pClassInfo->getSMNotSpecialCargoChange();
	if (getSMNotSpecialCargoPrereq() == NO_SPECIALUNIT) m_eSMNotSpecialCargoPrereq = pClassInfo->getSMNotSpecialCargoPrereq();
	if (setSpecialUnit() == NO_SPECIALUNIT) m_eSetSpecialUnit = pClassInfo->setSpecialUnit();
	//integers
	if (getAttackCombatModifierChange() == iDefault) m_iAttackCombatModifierChange = pClassInfo->getAttackCombatModifierChange();
	if (getDefenseCombatModifierChange() == iDefault) m_iDefenseCombatModifierChange = pClassInfo->getDefenseCombatModifierChange();
	if (m_iPursuitChange == iDefault) m_iPursuitChange = pClassInfo->m_iPursuitChange;
	if (m_iEarlyWithdrawChange == iDefault) m_iEarlyWithdrawChange = pClassInfo->m_iEarlyWithdrawChange;
	if (getVSBarbsChange() == iDefault) m_iVSBarbsChange = pClassInfo->getVSBarbsChange();
	if (getArmorChange() == iDefault) m_iArmorChange = pClassInfo->getArmorChange();
	if (getPunctureChange() == iDefault) m_iPunctureChange = pClassInfo->getPunctureChange();
	if (m_iOverrunChange == iDefault) m_iOverrunChange = pClassInfo->m_iOverrunChange;
	if (m_iRepelChange == iDefault) m_iRepelChange = pClassInfo->m_iRepelChange;
	if (m_iFortRepelChange == iDefault) m_iFortRepelChange = pClassInfo->m_iFortRepelChange;
	if (m_iRepelRetriesChange == iDefault) m_iRepelRetriesChange = pClassInfo->m_iRepelRetriesChange;
	if (m_iUnyieldingChange == iDefault) m_iUnyieldingChange = pClassInfo->m_iUnyieldingChange;
	if (m_iKnockbackChange == iDefault) m_iKnockbackChange = pClassInfo->m_iKnockbackChange;
	if (m_iKnockbackRetriesChange == iDefault) m_iKnockbackRetriesChange = pClassInfo->m_iKnockbackRetriesChange;
#ifdef BATTLEWORN
	if (m_iStrAdjperRndChange == iDefault) m_iStrAdjperRndChange = pClassInfo->m_iStrAdjperRndChange;
	if (m_iStrAdjperAttChange == iDefault) m_iStrAdjperAttChange = pClassInfo->m_iStrAdjperAttChange;
	if (m_iStrAdjperDefChange == iDefault) m_iStrAdjperDefChange = pClassInfo->m_iStrAdjperDefChange;
	if (m_iWithdrawAdjperAttChange == iDefault) m_iWithdrawAdjperAttChange = pClassInfo->m_iWithdrawAdjperAttChange;
#endif // BATTLEWORN
	if (m_iUnnerveChange == iDefault) m_iUnnerveChange = pClassInfo->m_iUnnerveChange;
	if (m_iEncloseChange == iDefault) m_iEncloseChange = pClassInfo->m_iEncloseChange;
	if (m_iLungeChange == iDefault) m_iLungeChange = pClassInfo->m_iLungeChange;
	if (m_iDynamicDefenseChange == iDefault) m_iDynamicDefenseChange = pClassInfo->m_iDynamicDefenseChange;
	if (getStrengthChange() == iDefault) m_iStrengthChange = pClassInfo->getStrengthChange();
	if (getLinePriority() == iDefault) m_iLinePriority = pClassInfo->getLinePriority();
	if (getFortitudeChange() == iDefault) m_iFortitudeChange = pClassInfo->getFortitudeChange();
	if (getDamageperTurn() == iDefault) m_iDamageperTurn = pClassInfo->getDamageperTurn();
	if (getStrAdjperTurn() == iDefault) m_iStrAdjperTurn = pClassInfo->getStrAdjperTurn();
	if (getWeakenperTurn() == iDefault) m_iWeakenperTurn = pClassInfo->getWeakenperTurn();
#ifdef STRENGTH_IN_NUMBERS
	if (m_iFrontSupportPercentChange == iDefault) m_iFrontSupportPercentChange = pClassInfo->m_iFrontSupportPercentChange;
	if (m_iShortRangeSupportPercentChange == iDefault) m_iShortRangeSupportPercentChange = pClassInfo->m_iShortRangeSupportPercentChange;
	if (m_iMediumRangeSupportPercentChange == iDefault) m_iMediumRangeSupportPercentChange = pClassInfo->m_iMediumRangeSupportPercentChange;
	if (m_iLongRangeSupportPercentChange == iDefault) m_iLongRangeSupportPercentChange = pClassInfo->m_iLongRangeSupportPercentChange;
	if (m_iFlankSupportPercentChange == iDefault) m_iFlankSupportPercentChange = pClassInfo->m_iFlankSupportPercentChange;
#endif
	if (getDodgeModifierChange() == iDefault) m_iDodgeModifierChange = pClassInfo->getDodgeModifierChange();
	if (getPrecisionModifierChange() == iDefault) m_iPrecisionModifierChange = pClassInfo->getPrecisionModifierChange();
	if (getPowerShotsChange() == iDefault) m_iPowerShotsChange = pClassInfo->getPowerShotsChange();
	if (getPowerShotCombatModifierChange() == iDefault) m_iPowerShotCombatModifierChange = pClassInfo->getPowerShotCombatModifierChange();
	if (getPowerShotPunctureModifierChange() == iDefault) m_iPowerShotPunctureModifierChange = pClassInfo->getPowerShotPunctureModifierChange();
	if (getPowerShotPrecisionModifierChange() == iDefault) m_iPowerShotPrecisionModifierChange = pClassInfo->getPowerShotPrecisionModifierChange();
	if (getPowerShotCriticalModifierChange() == iDefault) m_iPowerShotCriticalModifierChange = pClassInfo->getPowerShotCriticalModifierChange();
	if (getCriticalModifierChange() == iDefault) m_iCriticalModifierChange = pClassInfo->getCriticalModifierChange();
	if (getEnduranceChange() == iDefault) m_iEnduranceChange = pClassInfo->getEnduranceChange();
	if (getRoundStunProbChange() == iDefault) m_iRoundStunProbChange = pClassInfo->getRoundStunProbChange();
	if (getPoisonProbabilityModifierChange() == iDefault) m_iPoisonProbabilityModifierChange = pClassInfo->getPoisonProbabilityModifierChange();

	if (getCaptureProbabilityModifierChange() == iDefault) m_iCaptureProbabilityModifierChange = pClassInfo->getCaptureProbabilityModifierChange();
	if (getCaptureResistanceModifierChange() == iDefault) m_iCaptureResistanceModifierChange = pClassInfo->getCaptureResistanceModifierChange();

	if (getBreakdownChanceChange() == iDefault) m_iBreakdownChanceChange = pClassInfo->getBreakdownChanceChange();
	if (getBreakdownDamageChange() == iDefault) m_iBreakdownDamageChange = pClassInfo->getBreakdownDamageChange();
	if (getTauntChange() == iDefault) m_iTauntChange = pClassInfo->getTauntChange();
	if (getMaxHPChange() == iDefault) m_iMaxHPChange = pClassInfo->getMaxHPChange();
	if (getStrengthModifier() == iDefault) m_iStrengthModifier = pClassInfo->getStrengthModifier();
	if (getQualityChange() == iDefault) m_iQualityChange = pClassInfo->getQualityChange();
	if (getGroupChange() == iDefault) m_iGroupChange = pClassInfo->getGroupChange();
	if (getLevelPrereq() == iDefault) m_iLevelPrereq = pClassInfo->getLevelPrereq();
	if (getDamageModifierChange() == iDefault) m_iDamageModifierChange = pClassInfo->getDamageModifierChange();

	if (m_iUpkeepModifier == iDefault) m_iUpkeepModifier = pClassInfo->getUpkeepModifier();
	if (m_iExtraUpkeep100 == iDefault) m_iExtraUpkeep100 = pClassInfo->getExtraUpkeep100();

	if (getRBombardDamageChange() == iDefault) m_iRBombardDamageChange = pClassInfo->getRBombardDamageChange();
	if (getRBombardDamageLimitChange() == iDefault) m_iRBombardDamageLimitChange = pClassInfo->getRBombardDamageLimitChange();
	if (getRBombardDamageMaxUnitsChange() == iDefault) m_iRBombardDamageMaxUnitsChange = pClassInfo->getRBombardDamageMaxUnitsChange();
	if (getDCMBombRangeChange() == iDefault) m_iDCMBombRangeChange = pClassInfo->getDCMBombRangeChange();
	if (getDCMBombAccuracyChange() == iDefault) m_iDCMBombAccuracyChange = pClassInfo->getDCMBombAccuracyChange();
	if (m_iCombatModifierPerSizeMoreChange == iDefault) m_iCombatModifierPerSizeMoreChange = pClassInfo->m_iCombatModifierPerSizeMoreChange;
	if (m_iCombatModifierPerSizeLessChange == iDefault) m_iCombatModifierPerSizeLessChange = pClassInfo->m_iCombatModifierPerSizeLessChange;
	if (m_iCombatModifierPerVolumeMoreChange == iDefault) m_iCombatModifierPerVolumeMoreChange = pClassInfo->m_iCombatModifierPerVolumeMoreChange;
	if (m_iCombatModifierPerVolumeLessChange == iDefault) m_iCombatModifierPerVolumeLessChange = pClassInfo->m_iCombatModifierPerVolumeLessChange;
	if (getSelfHealModifier() == iDefault) m_iSelfHealModifier = pClassInfo->getSelfHealModifier();
	if (getNumHealSupport() == iDefault) m_iNumHealSupport = pClassInfo->getNumHealSupport();
	if (getExcileChange() == iDefault) m_iExcileChange = pClassInfo->getExcileChange();
	if (getPassageChange() == iDefault) m_iPassageChange = pClassInfo->getPassageChange();
	if (getNoNonOwnedCityEntryChange() == iDefault) m_iNoNonOwnedCityEntryChange = pClassInfo->getNoNonOwnedCityEntryChange();
	if (getBarbCoExistChange() == iDefault) m_iBarbCoExistChange = pClassInfo->getBarbCoExistChange();
	if (getBlendIntoCityChange() == iDefault) m_iBlendIntoCityChange = pClassInfo->getBlendIntoCityChange();
	if (getUpgradeAnywhereChange() == iDefault) m_iUpgradeAnywhereChange = pClassInfo->getUpgradeAnywhereChange();
	if (getInsidiousnessChange() == iDefault) m_iInsidiousnessChange = pClassInfo->getInsidiousnessChange();
	if (getInvestigationChange() == iDefault) m_iInvestigationChange = pClassInfo->getInvestigationChange();
	if (getAssassinChange() == iDefault) m_iAssassinChange = pClassInfo->getAssassinChange();
	if (m_iStealthStrikesChange == iDefault) m_iStealthStrikesChange = pClassInfo->m_iStealthStrikesChange;
	if (m_iStealthCombatModifierChange == iDefault) m_iStealthCombatModifierChange = pClassInfo->m_iStealthCombatModifierChange;
	if (m_iStealthDefenseChange == iDefault) m_iStealthDefenseChange = pClassInfo->m_iStealthDefenseChange;
	if (getDefenseOnlyChange() == iDefault) m_iDefenseOnlyChange = pClassInfo->getDefenseOnlyChange();
	if (getNoInvisibilityChange() == iDefault) m_iNoInvisibilityChange = pClassInfo->getNoInvisibilityChange();
	if (getTrapDamageMax() == iDefault) m_iTrapDamageMax = pClassInfo->getTrapDamageMax();
	if (getTrapDamageMin() == iDefault) m_iTrapDamageMin = pClassInfo->getTrapDamageMin();
	if (getTrapComplexity() == iDefault) m_iTrapComplexity = pClassInfo->getTrapComplexity();
	if (getNumTriggers() == iDefault) m_iNumTriggers = pClassInfo->getNumTriggers();
	if (getTriggerBeforeAttackChange() == iDefault) m_iTriggerBeforeAttackChange = pClassInfo->getTriggerBeforeAttackChange();
	if (getHiddenNationalityChange() == iDefault) m_iHiddenNationalityChange = pClassInfo->getHiddenNationalityChange();
	if (getAnimalIgnoresBordersChange() == iDefault) m_iAnimalIgnoresBordersChange = pClassInfo->getAnimalIgnoresBordersChange();
	if (getNoDefensiveBonusChange() == iDefault) m_iNoDefensiveBonusChange = pClassInfo->getNoDefensiveBonusChange();
	if (getGatherHerdChange() == iDefault) m_iGatherHerdChange = pClassInfo->getGatherHerdChange();
	if (getReligiousCombatModifierChange() == iDefault) m_iReligiousCombatModifierChange = pClassInfo->getReligiousCombatModifierChange();
	// booleans
	if (isStampedeChange() == bDefault) m_bStampedeChange = pClassInfo->isStampedeChange();
	if (isRemoveStampede() == bDefault) m_bRemoveStampede = pClassInfo->isRemoveStampede();
	if (isOnslaughtChange() == bDefault) m_bOnslaughtChange = pClassInfo->isOnslaughtChange();
	if (isEquipment() == bDefault) m_bEquipment = pClassInfo->isEquipment();
#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (isAffliction() == bDefault) m_bAffliction = pClassInfo->isAffliction();
#endif
	if (isParalyze() == bDefault) m_bParalyze = pClassInfo->isParalyze();
	if (isMakesDamageCold() == bDefault) m_bMakesDamageCold = pClassInfo->isMakesDamageCold();
	if (isMakesDamageNotCold() == bDefault) m_bMakesDamageNotCold = pClassInfo->isMakesDamageNotCold();
	if (isAddsColdImmunity() == bDefault) m_bAddsColdImmunity = pClassInfo->isAddsColdImmunity();
	if (isRemovesColdImmunity() == bDefault) m_bRemovesColdImmunity = pClassInfo->isRemovesColdImmunity();
	if (isCritical() == bDefault) m_bCritical = pClassInfo->isCritical();
	if (isAttackOnlyCitiesAdd() == bDefault) m_bAttackOnlyCitiesAdd = pClassInfo->isAttackOnlyCitiesAdd();
	if (isAttackOnlyCitiesSubtract() == bDefault) m_bAttackOnlyCitiesSubtract = pClassInfo->isAttackOnlyCitiesSubtract();
	if (isIgnoreNoEntryLevelAdd() == bDefault) m_bIgnoreNoEntryLevelAdd = pClassInfo->isIgnoreNoEntryLevelAdd();
	if (isIgnoreNoEntryLevelSubtract() == bDefault) m_bIgnoreNoEntryLevelSubtract = pClassInfo->isIgnoreNoEntryLevelSubtract();
	if (isIgnoreZoneofControlAdd() == bDefault) m_bIgnoreZoneofControlAdd = pClassInfo->isIgnoreZoneofControlAdd();
	if (isIgnoreZoneofControlSubtract() == bDefault) m_bIgnoreZoneofControlSubtract = pClassInfo->isIgnoreZoneofControlSubtract();
	if (isFliesToMoveAdd() == bDefault) m_bFliesToMoveAdd = pClassInfo->isFliesToMoveAdd();
	if (isFliesToMoveSubtract() == bDefault) m_bFliesToMoveSubtract = pClassInfo->isFliesToMoveSubtract();
	if (isZeroesXP() == bDefault) m_bZeroesXP = pClassInfo->isZeroesXP();
	if (isForOffset() == bDefault) m_bForOffset = pClassInfo->isForOffset();
	if (isCargoPrereq() == bDefault) m_bCargoPrereq = pClassInfo->isCargoPrereq();
	if (isRBombardPrereq() == bDefault) m_bRBombardPrereq = pClassInfo->isRBombardPrereq();
	if (isNoSelfHeal() == bDefault) m_bNoSelfHeal = pClassInfo->isNoSelfHeal();
	if (isSetOnHNCapture() == bDefault) m_bSetOnHNCapture = pClassInfo->isSetOnHNCapture();
	if (isSetOnInvestigated() == bDefault) m_bSetOnInvestigated = pClassInfo->isSetOnInvestigated();
	if (isStatus() == bDefault) m_bStatus = pClassInfo->isStatus();
	if (isPrereqNormInvisible() == bDefault) m_bPrereqNormInvisible = pClassInfo->isPrereqNormInvisible();
	if (isPlotPrereqsKeepAfter() == bDefault) m_bPlotPrereqsKeepAfter = pClassInfo->isPlotPrereqsKeepAfter();
	if (isRemoveAfterSet() == bDefault) m_bRemoveAfterSet = pClassInfo->isRemoveAfterSet();
	if (isQuick() == bDefault) m_bQuick = pClassInfo->isQuick();
	if (m_bStarsign == false) m_bStarsign = pClassInfo->isStarsign();
	// bool vectors without delayed resolution
	if (getNumSubCombatChangeTypes() == 0)
	{
		m_aiSubCombatChangeTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumSubCombatChangeTypes(); i++)
		{
			m_aiSubCombatChangeTypes.push_back(pClassInfo->getSubCombatChangeType(i));
		}
	}

	if (getNumRemovesUnitCombatTypes() == 0)
	{
		m_aiRemovesUnitCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumRemovesUnitCombatTypes(); i++)
		{
			m_aiRemovesUnitCombatTypes.push_back(pClassInfo->getRemovesUnitCombatType(i));
		}
	}

	if (getNumOnGameOptions() == 0)
	{
		m_aiOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumOnGameOptions(); i++)
		{
			m_aiOnGameOptions.push_back(pClassInfo->getOnGameOption(i));
		}
	}

	if (getNumNotOnGameOptions() == 0)
	{
		m_aiNotOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnGameOptions(); i++)
		{
			m_aiNotOnGameOptions.push_back(pClassInfo->getNotOnGameOption(i));
		}
	}

	if (getNumFreetoUnitCombats() == 0)
	{
		m_aiFreetoUnitCombats.clear();
		for ( int i = 0; i < pClassInfo->getNumFreetoUnitCombats(); i++)
		{
			m_aiFreetoUnitCombats.push_back(pClassInfo->getFreetoUnitCombat(i));
		}
	}

	if (getNumNotOnUnitCombatTypes() == 0)
	{
		m_aiNotOnUnitCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnUnitCombatTypes(); i++)
		{
			m_aiNotOnUnitCombatTypes.push_back(pClassInfo->getNotOnUnitCombatType(i));
		}
	}

	if (getNumNotOnDomainTypes() == 0)
	{
		m_aiNotOnDomainTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnDomainTypes(); i++)
		{
			m_aiNotOnDomainTypes.push_back(pClassInfo->getNotOnDomainType(i));
		}
	}

	if (getNumNoAutoEquiptoCombatClassTypes() == 0)
	{
		m_aiNoAutoEquiptoCombatClassTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNoAutoEquiptoCombatClassTypes(); i++)
		{
			m_aiNoAutoEquiptoCombatClassTypes.push_back(pClassInfo->getNoAutoEquiptoCombatClassType(i));
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
#ifdef OUTBREAKS_AND_AFFLICTIONS
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCureAfflictionChangeTypes, pClassInfo->m_aiCureAfflictionChangeTypes);
#endif // OUTBREAKS_AND_AFFLICTIONS
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqBonusTypes, pClassInfo->m_aiPrereqBonusTypes);

	if (getNumAddsBuildTypes() == 0)
	{
		const int iNum = pClassInfo->getNumAddsBuildTypes();
		m_aiAddsBuildTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiAddsBuildTypes[i]), (int*)&(pClassInfo->m_aiAddsBuildTypes[i]));
		}
	}

	if (getNumNegatesInvisibilityTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiNegatesInvisibilityTypes, pClassInfo->m_aiNegatesInvisibilityTypes);
	}

	if (getNumPrereqTerrainTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqTerrainTypes, pClassInfo->m_aiPrereqTerrainTypes);
	}

	if (getNumPrereqFeatureTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqFeatureTypes, pClassInfo->m_aiPrereqFeatureTypes);
	}

	if (getNumPrereqImprovementTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqImprovementTypes, pClassInfo->m_aiPrereqImprovementTypes);
	}

	if (getNumPrereqPlotBonusTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqPlotBonusTypes, pClassInfo->m_aiPrereqPlotBonusTypes);
	}

	if (getNumPrereqLocalBuildingTypes() == 0)
	{
		const int iNum = pClassInfo->getNumPrereqLocalBuildingTypes();
		m_aiPrereqLocalBuildingTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiPrereqLocalBuildingTypes[i]), (int*)&(pClassInfo->m_aiPrereqLocalBuildingTypes[i]));
		}
	}

	if (getNumTrapSetWithPromotionTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTrapSetWithPromotionTypes, pClassInfo->m_aiTrapSetWithPromotionTypes);
	}

	if (getNumTrapImmunityUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTrapImmunityUnitCombatTypes, pClassInfo->m_aiTrapImmunityUnitCombatTypes);
	}


	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (getNumAidChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumAidChanges(); i++)
		{
			PropertyTypes eProperty = ((PropertyTypes)i);
			int iChange = pClassInfo->getAidChange(i);
			m_aAidChanges.push_back(std::make_pair(eProperty, iChange));
		}
	}
#endif
	if (getNumTargetUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTargetUnitCombatTypes, pClassInfo->m_aiTargetUnitCombatTypes);
	}

	// int vectors utilizing pairing without delayed resolution
	if (getNumFlankingStrikesbyUnitCombatTypesChange()==0)
	{
		for (int i=0; i < pClassInfo->getNumFlankingStrikesbyUnitCombatTypesChange(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getFlankingStrengthbyUnitCombatTypeChange(i);
			m_aFlankingStrengthbyUnitCombatTypeChange.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumWithdrawOnTerrainTypeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnTerrainTypeChanges(); i++)
		{
			TerrainTypes eTerrain = ((TerrainTypes)i);
			int iChange = pClassInfo->getWithdrawOnTerrainTypeChange(i);
			m_aWithdrawOnTerrainTypesChange.push_back(std::make_pair(eTerrain, iChange));
		}
	}

	if (getNumWithdrawOnFeatureTypeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnFeatureTypeChanges(); i++)
		{
			FeatureTypes eFeature = ((FeatureTypes)i);
			int iChange = pClassInfo->getWithdrawOnFeatureTypeChange(i);
			m_aWithdrawOnFeatureTypesChange.push_back(std::make_pair(eFeature, iChange));
		}
	}

	if (getNumWithdrawVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getWithdrawVSUnitCombatChangeType(i);
			m_aWithdrawVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPursuitVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPursuitVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPursuitVSUnitCombatChangeType(i, true);
			m_aPursuitVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumRepelVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumRepelVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getRepelVSUnitCombatChangeType(i, true);
			m_aRepelVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumKnockbackVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumKnockbackVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getKnockbackVSUnitCombatChangeType(i, true);
			m_aKnockbackVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPunctureVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPunctureVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPunctureVSUnitCombatChangeType(i);
			m_aPunctureVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumArmorVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumArmorVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getArmorVSUnitCombatChangeType(i);
			m_aArmorVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumDodgeVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumDodgeVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getDodgeVSUnitCombatChangeType(i);
			m_aDodgeVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPrecisionVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPrecisionVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPrecisionVSUnitCombatChangeType(i);
			m_aPrecisionVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumCriticalVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumCriticalVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getCriticalVSUnitCombatChangeType(i);
			m_aCriticalVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumRoundStunVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumRoundStunVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getRoundStunVSUnitCombatChangeType(i);
			m_aRoundStunVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumTrapDisableUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTrapDisableUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getTrapDisableUnitCombatType(i);
			m_aTrapDisableUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumTrapAvoidanceUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTrapAvoidanceUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getTrapAvoidanceUnitCombatType(i);
			m_aTrapAvoidanceUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumTrapTriggerUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTrapTriggerUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getTrapTriggerUnitCombatType(i);
			m_aTrapTriggerUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumBuildWorkRateModifierChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumBuildWorkRateModifierChangeTypes(); i++)
		{
			BuildTypes eBuild = ((BuildTypes)i);
			int iChange = pClassInfo->getBuildWorkRateModifierChangeType(i);
			m_aBuildWorkRateModifierChangeTypes.push_back(std::make_pair(eBuild, iChange));
		}
	}

	if (getNumVisibilityIntensityChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getVisibilityIntensityChangeType(i);
			m_aVisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumInvisibilityIntensityChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumInvisibilityIntensityChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getInvisibilityIntensityChangeType(i);
			m_aInvisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumVisibilityIntensityRangeChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityRangeChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getVisibilityIntensityRangeChangeType(i);
			m_aVisibilityIntensityRangeChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	// int vectors utilizing struct with delayed resolution
	if (getNumAIWeightbyUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAIWeightbyUnitCombatTypes, pClassInfo->m_aAIWeightbyUnitCombatTypes);
	}

#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (getNumAfflictionFortitudeChangeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAfflictionFortitudeChangeModifiers, pClassInfo->m_aAfflictionFortitudeChangeModifiers);
	}

	if (getNumAfflictOnAttackChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAfflictOnAttackChangeTypes, pClassInfo->m_aAfflictOnAttackChangeTypes);
	}
#endif // OUTBREAKS_AND_AFFLICTIONS
	if (getNumHealUnitCombatChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aHealUnitCombatChangeTypes, pClassInfo->m_aHealUnitCombatChangeTypes);
	}

	if (getNumInvisibleTerrainChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aInvisibleTerrainChanges, pClassInfo->m_aInvisibleTerrainChanges);
	}

	if (getNumInvisibleFeatureChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aInvisibleFeatureChanges, pClassInfo->m_aInvisibleFeatureChanges);
	}

	if (getNumInvisibleImprovementChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aInvisibleImprovementChanges, pClassInfo->m_aInvisibleImprovementChanges);
	}

	if (getNumVisibleTerrainChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleTerrainChanges, pClassInfo->m_aVisibleTerrainChanges);
	}

	if (getNumVisibleFeatureChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleFeatureChanges, pClassInfo->m_aVisibleFeatureChanges);
	}

	if (getNumVisibleImprovementChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleImprovementChanges, pClassInfo->m_aVisibleImprovementChanges);
	}

	if (getNumVisibleTerrainRangeChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleTerrainRangeChanges, pClassInfo->m_aVisibleTerrainRangeChanges);
	}

	if (getNumVisibleFeatureRangeChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleFeatureRangeChanges, pClassInfo->m_aVisibleFeatureRangeChanges);
	}

	if (getNumVisibleImprovementRangeChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleImprovementRangeChanges, pClassInfo->m_aVisibleImprovementRangeChanges);
	}

	if (getNumDistanceAttackCommunicabilityTypeChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aDistanceAttackCommunicabilityTypeChanges, pClassInfo->m_aDistanceAttackCommunicabilityTypeChanges);
	}
	//TB Combat Mods End  TB SubCombat Mods end

	if (isCanMovePeaks() == bDefault) m_bCanMovePeaks = pClassInfo->isCanMovePeaks();
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	if (isCanLeadThroughPeaks() == bDefault) m_bCanLeadThroughPeaks = pClassInfo->isCanLeadThroughPeaks();

	if (getObsoleteTech() == NO_TECH) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getControlPoints() == iDefault) m_iControlPoints = pClassInfo->getControlPoints();
	if (getCommandRange() == iDefault) m_iCommandRange = pClassInfo->getCommandRange();
	if (isZoneOfControl() == bDefault) m_bZoneOfControl = pClassInfo->isZoneOfControl();
	if (getIgnoreTerrainDamage() == NO_TERRAIN) m_iIgnoreTerrainDamage = pClassInfo->getIgnoreTerrainDamage();

	//GC.copyNonDefaultDelayedResolutionVector(m_vPromotionOverwrites, pClassInfo->getPromotionOverwrites());

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

void CvPromotionInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CvInfoUtil(this).checkSum(iSum);

	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iStateReligionPrereq);
	CheckSum(iSum, m_iMinEraType);
	CheckSum(iSum, m_iMaxEraType);
	CheckSum(iSum, m_iVisibilityChange);
	CheckSum(iSum, m_iMovesChange);
	CheckSum(iSum, m_iMoveDiscountChange);
	CheckSum(iSum, m_iAirRangeChange);
	CheckSum(iSum, m_iInterceptChange);
	CheckSum(iSum, m_iEvasionChange);
	CheckSum(iSum, m_iWithdrawalChange);
	CheckSum(iSum, m_iCargoChange);
	CheckSum(iSum, m_iSMCargoChange);
	CheckSum(iSum, m_iSMCargoVolumeChange);
	CheckSum(iSum, m_iSMCargoVolumeModifierChange);
	CheckSum(iSum, m_iCollateralDamageChange);
	CheckSum(iSum, m_iBombardRateChange);
	CheckSum(iSum, m_iFirstStrikesChange);
	CheckSum(iSum, m_iChanceFirstStrikesChange);
	CheckSum(iSum, m_iEnemyHealChange);
	CheckSum(iSum, m_iNeutralHealChange);
	CheckSum(iSum, m_iFriendlyHealChange);
	CheckSum(iSum, m_iSameTileHealChange);
	CheckSum(iSum, m_iAdjacentTileHealChange);
	CheckSum(iSum, m_iCombatPercent);
	CheckSum(iSum, m_iCityAttackPercent);
	CheckSum(iSum, m_iCityDefensePercent);
	CheckSum(iSum, m_iHillsAttackPercent);
	CheckSum(iSum, m_iHillsDefensePercent);
	CheckSum(iSum, m_iHillsWorkPercent);
	CheckSum(iSum, m_iPeaksWorkPercent);
	CheckSum(iSum, m_iWorkRatePercent);
	CheckSum(iSum, m_iCommandType);
	CheckSum(iSum, m_iRevoltProtection);
	CheckSum(iSum, m_iCollateralDamageProtection);
	CheckSum(iSum, m_iPillageChange);
	CheckSum(iSum, m_iUpgradeDiscount);
	CheckSum(iSum, m_iExperiencePercent);
	CheckSum(iSum, m_iKamikazePercent);

	CheckSum(iSum, m_bLeader);
	CheckSum(iSum, m_bBlitz);
	CheckSum(iSum, m_bAmphib);
	CheckSum(iSum, m_bRiver);
	CheckSum(iSum, m_bEnemyRoute);
	CheckSum(iSum, m_bAlwaysHeal);
	CheckSum(iSum, m_bHillsDoubleMove);
	CheckSum(iSum, m_bImmuneToFirstStrikes);

	// Arrays

	CheckSum(iSum, m_piTerrainAttackPercent, GC.getNumTerrainInfos());
	CheckSum(iSum, m_piTerrainDefensePercent, GC.getNumTerrainInfos());
	CheckSum(iSum, m_piFeatureAttackPercent, GC.getNumFeatureInfos());
	CheckSum(iSum, m_piFeatureDefensePercent, GC.getNumFeatureInfos());
	CheckSum(iSum, m_piUnitCombatModifierPercent, GC.getNumUnitCombatInfos());
	CheckSum(iSum, m_piDomainModifierPercent, NUM_DOMAIN_TYPES);
	CheckSum(iSum, m_pbTerrainDoubleMove, GC.getNumTerrainInfos());
	CheckSum(iSum, m_pbFeatureDoubleMove, GC.getNumFeatureInfos());
	CheckSum(iSum, m_pbUnitCombat, GC.getNumUnitCombatInfos());
	CheckSum(iSum, m_piTerrainWorkPercent, GC.getNumTerrainInfos());
	CheckSum(iSum, m_piFeatureWorkPercent, GC.getNumFeatureInfos());
	CheckSum(iSum, m_bCanMovePeaks);
	CheckSum(iSum, m_bCanLeadThroughPeaks);
	//CheckSumC(iSum, m_vPromotionOverwrites);
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iControlPoints);
	CheckSum(iSum, m_iCommandRange);
	CheckSum(iSum, m_iIgnoreTerrainDamage);
	CheckSum(iSum, m_bZoneOfControl);
	m_PropertyManipulators.getCheckSum(iSum);
	CheckSum(iSum, m_bDefensiveVictoryMove);
	CheckSum(iSum, m_bFreeDrop);
	CheckSum(iSum, m_bOffensiveVictoryMove);
	CheckSum(iSum, m_bOneUp);
	CheckSum(iSum, m_bPillageEspionage);
	CheckSum(iSum, m_bPillageMarauder);
	CheckSum(iSum, m_bPillageOnMove);
	CheckSum(iSum, m_bPillageOnVictory);
	CheckSum(iSum, m_bPillageResearch);
	CheckSum(iSum, m_iAirCombatLimitChange);
	CheckSum(iSum, m_iCelebrityHappy);
	CheckSum(iSum, m_iCollateralDamageLimitChange);
	CheckSum(iSum, m_iCollateralDamageMaxUnitsChange);
	CheckSum(iSum, m_iCombatLimitChange);
	CheckSum(iSum, m_iExtraDropRange);
	CheckSum(iSum, m_iSurvivorChance);
	CheckSum(iSum, m_iVictoryAdjacentHeal);
	CheckSum(iSum, m_iVictoryHeal);
	CheckSum(iSum, m_iVictoryStackHeal);

	//TB Combat Mods Begin  TB SubCombat Mod begin
	// Textual References
	CheckSum(iSum, m_ePromotionLine);
	CheckSum(iSum, m_eReplacesUnitCombat);
	CheckSum(iSum, m_eDomainCargoChange);
	CheckSum(iSum, m_eSpecialCargoChange);
	CheckSum(iSum, m_eSpecialCargoPrereq);
	CheckSum(iSum, m_eSMNotSpecialCargoChange);
	CheckSum(iSum, m_eSMNotSpecialCargoPrereq);
	CheckSum(iSum, m_eSetSpecialUnit);
	//integers
	CheckSum(iSum, m_iAttackCombatModifierChange);
	CheckSum(iSum, m_iDefenseCombatModifierChange);
	CheckSum(iSum, m_iPursuitChange);
	CheckSum(iSum, m_iEarlyWithdrawChange);
	CheckSum(iSum, m_iVSBarbsChange);
	CheckSum(iSum, m_iArmorChange);
	CheckSum(iSum, m_iPunctureChange);
	CheckSum(iSum, m_iOverrunChange);
	CheckSum(iSum, m_iRepelChange);
	CheckSum(iSum, m_iFortRepelChange);
	CheckSum(iSum, m_iRepelRetriesChange);
	CheckSum(iSum, m_iUnyieldingChange);
	CheckSum(iSum, m_iKnockbackChange);
	CheckSum(iSum, m_iKnockbackRetriesChange);
#ifdef BATTLEWORN
	CheckSum(iSum, m_iStrAdjperRndChange);
	CheckSum(iSum, m_iStrAdjperAttChange);
	CheckSum(iSum, m_iStrAdjperDefChange);
	CheckSum(iSum, m_iWithdrawAdjperAttChange);
#endif // BATTLEWORN
	CheckSum(iSum, m_iUnnerveChange);
	CheckSum(iSum, m_iEncloseChange);
	CheckSum(iSum, m_iLungeChange);
	CheckSum(iSum, m_iDynamicDefenseChange);
	CheckSum(iSum, m_iStrengthChange);
	CheckSum(iSum, m_iLinePriority);
	CheckSum(iSum, m_iFortitudeChange);
	CheckSum(iSum, m_iDamageperTurn);
	CheckSum(iSum, m_iStrAdjperTurn);
	CheckSum(iSum, m_iWeakenperTurn);
#ifdef STRENGTH_IN_NUMBERS
	CheckSum(iSum, m_iFrontSupportPercentChange);
	CheckSum(iSum, m_iShortRangeSupportPercentChange);
	CheckSum(iSum, m_iMediumRangeSupportPercentChange);
	CheckSum(iSum, m_iLongRangeSupportPercentChange);
	CheckSum(iSum, m_iFlankSupportPercentChange);
#endif
	CheckSum(iSum, m_iDodgeModifierChange);
	CheckSum(iSum, m_iPrecisionModifierChange);
	CheckSum(iSum, m_iPowerShotsChange);
	CheckSum(iSum, m_iPowerShotCombatModifierChange);
	CheckSum(iSum, m_iPowerShotPunctureModifierChange);
	CheckSum(iSum, m_iPowerShotPrecisionModifierChange);
	CheckSum(iSum, m_iPowerShotCriticalModifierChange);
	CheckSum(iSum, m_iCriticalModifierChange);
	CheckSum(iSum, m_iEnduranceChange);
	CheckSum(iSum, m_iRoundStunProbChange);
	CheckSum(iSum, m_iPoisonProbabilityModifierChange);

	CheckSum(iSum, m_iCaptureProbabilityModifierChange);
	CheckSum(iSum, m_iCaptureResistanceModifierChange);

	CheckSum(iSum, m_iBreakdownChanceChange);
	CheckSum(iSum, m_iBreakdownDamageChange);
	CheckSum(iSum, m_iTauntChange);
	CheckSum(iSum, m_iMaxHPChange);
	CheckSum(iSum, m_iStrengthModifier);
	CheckSum(iSum, m_iQualityChange);
	CheckSum(iSum, m_iGroupChange);
	CheckSum(iSum, m_iLevelPrereq);
	CheckSum(iSum, m_iDamageModifierChange);

	CheckSum(iSum, m_iUpkeepModifier);
	CheckSum(iSum, m_iExtraUpkeep100);

	CheckSum(iSum, m_iRBombardDamageChange);
	CheckSum(iSum, m_iRBombardDamageLimitChange);
	CheckSum(iSum, m_iRBombardDamageMaxUnitsChange);
	CheckSum(iSum, m_iDCMBombRangeChange);
	CheckSum(iSum, m_iDCMBombAccuracyChange);
	CheckSum(iSum, m_iCombatModifierPerSizeMoreChange);
	CheckSum(iSum, m_iCombatModifierPerSizeLessChange);
	CheckSum(iSum, m_iCombatModifierPerVolumeMoreChange);
	CheckSum(iSum, m_iCombatModifierPerVolumeLessChange);
	CheckSum(iSum, m_iSelfHealModifier);
	CheckSum(iSum, m_iNumHealSupport);
	CheckSum(iSum, m_iExcileChange);
	CheckSum(iSum, m_iPassageChange);
	CheckSum(iSum, m_iNoNonOwnedCityEntryChange);
	CheckSum(iSum, m_iBarbCoExistChange);
	CheckSum(iSum, m_iBlendIntoCityChange);
	CheckSum(iSum, m_iUpgradeAnywhereChange);
	CheckSum(iSum, m_iInsidiousnessChange);
	CheckSum(iSum, m_iInvestigationChange);
	CheckSum(iSum, m_iAssassinChange);
	CheckSum(iSum, m_iStealthStrikesChange);
	CheckSum(iSum, m_iStealthCombatModifierChange);
	CheckSum(iSum, m_iStealthDefenseChange);
	CheckSum(iSum, m_iDefenseOnlyChange);
	CheckSum(iSum, m_iNoInvisibilityChange);
	CheckSum(iSum, m_iTrapDamageMax);
	CheckSum(iSum, m_iTrapDamageMin);
	CheckSum(iSum, m_iTrapComplexity);
	CheckSum(iSum, m_iNumTriggers);
	CheckSum(iSum, m_iTriggerBeforeAttackChange);
	CheckSum(iSum, m_iHiddenNationalityChange);
	CheckSum(iSum, m_iAnimalIgnoresBordersChange);
	CheckSum(iSum, m_iNoDefensiveBonusChange);
	CheckSum(iSum, m_iGatherHerdChange);
	CheckSum(iSum, m_iReligiousCombatModifierChange);
	//booleans
	CheckSum(iSum, m_bStampedeChange);
	CheckSum(iSum, m_bRemoveStampede);
	CheckSum(iSum, m_bOnslaughtChange);
	CheckSum(iSum, m_bEquipment);
#ifdef OUTBREAKS_AND_AFFLICTIONS
	CheckSum(iSum, m_bAffliction);
#endif
	CheckSum(iSum, m_bParalyze);
	CheckSum(iSum, m_bMakesDamageCold);
	CheckSum(iSum, m_bMakesDamageNotCold);
	CheckSum(iSum, m_bAddsColdImmunity);
	CheckSum(iSum, m_bRemovesColdImmunity);
	CheckSum(iSum, m_bCritical);
	CheckSum(iSum, m_bAttackOnlyCitiesAdd);
	CheckSum(iSum, m_bAttackOnlyCitiesSubtract);
	CheckSum(iSum, m_bIgnoreNoEntryLevelAdd);
	CheckSum(iSum, m_bIgnoreNoEntryLevelSubtract);
	CheckSum(iSum, m_bIgnoreZoneofControlAdd);
	CheckSum(iSum, m_bIgnoreZoneofControlSubtract);
	CheckSum(iSum, m_bFliesToMoveAdd);
	CheckSum(iSum, m_bFliesToMoveSubtract);
	CheckSum(iSum, m_bNoSelfHeal);
	CheckSum(iSum, m_bSetOnHNCapture);
	CheckSum(iSum, m_bSetOnInvestigated);
	CheckSum(iSum, m_bStatus);
	CheckSum(iSum, m_bPrereqNormInvisible);
	CheckSum(iSum, m_bPlotPrereqsKeepAfter);
	CheckSum(iSum, m_bRemoveAfterSet);
	CheckSum(iSum, m_bQuick);
	CheckSum(iSum, m_bStarsign);
	CheckSum(iSum, m_bZeroesXP);
	CheckSum(iSum, m_bForOffset);
	CheckSum(iSum, m_bCargoPrereq);
	CheckSum(iSum, m_bRBombardPrereq);
	//Arrays
	//CheckSum(iSum, m_piAIWeightbyUnitCombatTypes, GC.getNumUnitCombatInfos());
	// bool vectors without delayed resolution
	CheckSumC(iSum, m_aiSubCombatChangeTypes);
	CheckSumC(iSum, m_aiRemovesUnitCombatTypes);
	CheckSumC(iSum, m_aiOnGameOptions);
	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiFreetoUnitCombats);
	CheckSumC(iSum, m_aiNotOnUnitCombatTypes);
	CheckSumC(iSum, m_aiNotOnDomainTypes);
	CheckSumC(iSum, m_aiNoAutoEquiptoCombatClassTypes);
	CheckSumC(iSum, m_aeMapCategoryTypes);
#ifdef OUTBREAKS_AND_AFFLICTIONS
	CheckSumC(iSum, m_aiCureAfflictionChangeTypes);
#endif // OUTBREAKS_AND_AFFLICTIONS
	CheckSumC(iSum, m_aiPrereqBonusTypes);
	CheckSumC(iSum, m_aiAddsBuildTypes);
	CheckSumC(iSum, m_aiNegatesInvisibilityTypes);
	CheckSumC(iSum, m_aiPrereqTerrainTypes);
	CheckSumC(iSum, m_aiPrereqFeatureTypes);
	CheckSumC(iSum, m_aiPrereqImprovementTypes);
	CheckSumC(iSum, m_aiPrereqPlotBonusTypes);
	CheckSumC(iSum, m_aiPrereqLocalBuildingTypes);
	CheckSumC(iSum, m_aiTrapSetWithPromotionTypes);
	CheckSumC(iSum, m_aiTrapImmunityUnitCombatTypes);
	CheckSumC(iSum, m_aiTargetUnitCombatTypes);
	CheckSumC(iSum, m_aiCategories);
	// int vectors utilizing pairing without delayed resolution
	CheckSumC(iSum, m_aFlankingStrengthbyUnitCombatTypeChange);
	CheckSumC(iSum, m_aWithdrawOnTerrainTypesChange);
	CheckSumC(iSum, m_aWithdrawOnFeatureTypesChange);
	CheckSumC(iSum, m_aWithdrawVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aPursuitVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aRepelVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aKnockbackVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aPunctureVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aArmorVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aDodgeVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aPrecisionVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aCriticalVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aRoundStunVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aTrapDisableUnitCombatTypes);
	CheckSumC(iSum, m_aTrapAvoidanceUnitCombatTypes);
	CheckSumC(iSum, m_aTrapTriggerUnitCombatTypes);
#ifdef OUTBREAKS_AND_AFFLICTIONS
	CheckSumC(iSum, m_aAidChanges);
#endif
//Team Project (4)
		//WorkRateMod
	CheckSumC(iSum, m_aBuildWorkRateModifierChangeTypes);
	CheckSumC(iSum, m_aVisibilityIntensityChangeTypes);
	CheckSumC(iSum, m_aInvisibilityIntensityChangeTypes);
	CheckSumC(iSum, m_aVisibilityIntensityRangeChangeTypes);
	// int vectors utilizing struct with delayed resolution
	int iNumElements;

	iNumElements = m_aAIWeightbyUnitCombatTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAIWeightbyUnitCombatTypes[i].eUnitCombat);
		CheckSum(iSum, m_aAIWeightbyUnitCombatTypes[i].iModifier);
	}

#ifdef OUTBREAKS_AND_AFFLICTIONS
	iNumElements = m_aAfflictionFortitudeChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionFortitudeChangeModifiers[i].iModifier);
	}

	iNumElements = m_aAfflictOnAttackChangeTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].eAfflictionLine);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iProbabilityChange);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iMelee);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iDistance);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iImmediate);
	}
#endif
	iNumElements = m_aHealUnitCombatChangeTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aHealUnitCombatChangeTypes[i].eUnitCombat);
		CheckSum(iSum, m_aHealUnitCombatChangeTypes[i].iHeal);
		CheckSum(iSum, m_aHealUnitCombatChangeTypes[i].iAdjacentHeal);
	}

	iNumElements = m_aInvisibleTerrainChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aInvisibleTerrainChanges[i].eInvisible);
		CheckSum(iSum, m_aInvisibleTerrainChanges[i].eTerrain);
		CheckSum(iSum, m_aInvisibleTerrainChanges[i].iIntensity);
	}

	iNumElements = m_aInvisibleFeatureChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aInvisibleFeatureChanges[i].eInvisible);
		CheckSum(iSum, m_aInvisibleFeatureChanges[i].eFeature);
		CheckSum(iSum, m_aInvisibleFeatureChanges[i].iIntensity);
	}

	iNumElements = m_aInvisibleImprovementChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aInvisibleImprovementChanges[i].eInvisible);
		CheckSum(iSum, m_aInvisibleImprovementChanges[i].eImprovement);
		CheckSum(iSum, m_aInvisibleImprovementChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleTerrainChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleTerrainChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleTerrainChanges[i].eTerrain);
		CheckSum(iSum, m_aVisibleTerrainChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleFeatureChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleFeatureChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleFeatureChanges[i].eFeature);
		CheckSum(iSum, m_aVisibleFeatureChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleImprovementChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleImprovementChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleImprovementChanges[i].eImprovement);
		CheckSum(iSum, m_aVisibleImprovementChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleTerrainRangeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleTerrainRangeChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleTerrainRangeChanges[i].eTerrain);
		CheckSum(iSum, m_aVisibleTerrainRangeChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleFeatureRangeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleFeatureRangeChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleFeatureRangeChanges[i].eFeature);
		CheckSum(iSum, m_aVisibleFeatureRangeChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleImprovementRangeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleImprovementRangeChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleImprovementRangeChanges[i].eImprovement);
		CheckSum(iSum, m_aVisibleImprovementRangeChanges[i].iIntensity);
	}

	iNumElements = m_aDistanceAttackCommunicabilityTypeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine);
		CheckSum(iSum, m_aDistanceAttackCommunicabilityTypeChanges[i].iChange);
	}
	//TB Combat Mods End  TB SubCombat Mod end
}

//======================================================================================================
//					CvMissionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvMissionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvMissionInfo::CvMissionInfo() :
m_iTime(0),
m_bSound(false),
m_bTarget(false),
m_bBuild(false),
m_bVisible(false),
m_eEntityEvent(ENTITY_EVENT_NONE)
{
}

CvMissionInfo::CvMissionInfo(const char * szType) : CvHotkeyInfo(),
m_iTime(0),
m_bSound(false),
m_bTarget(false),
m_bBuild(false),
m_bVisible(false),
m_eEntityEvent(ENTITY_EVENT_NONE)
{
	m_szType = szType;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvMissionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvMissionInfo::~CvMissionInfo()
{
}

int CvMissionInfo::getTime() const
{
	return m_iTime;
}

bool CvMissionInfo::isSound() const
{
	return m_bSound;
}

bool CvMissionInfo::isTarget() const
{
	return m_bTarget;
}

bool CvMissionInfo::isBuild() const
{
	return m_bBuild;
}

bool CvMissionInfo::getVisible() const
{
	return m_bVisible;
}

const char* CvMissionInfo::getWaypoint() const
{
	return m_szWaypoint;
}

EntityEventTypes CvMissionInfo::getEntityEvent() const
{
	return m_eEntityEvent;
}

bool CvMissionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTmp;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szWaypoint, L"Waypoint");
	pXML->GetOptionalChildXmlValByName(&m_iTime, L"iTime");
	pXML->GetOptionalChildXmlValByName(&m_bSound, L"bSound");
	pXML->GetOptionalChildXmlValByName(&m_bTarget, L"bTarget");
	pXML->GetOptionalChildXmlValByName(&m_bBuild, L"bBuild");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");

	if ( pXML->GetOptionalChildXmlValByName(szTmp, L"EntityEventType"))
		m_eEntityEvent = (EntityEventTypes)pXML->GetInfoClass(szTmp);

	return true;
}

void CvMissionInfo::copyNonDefaults(const CvMissionInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getWaypoint() == cDefault) m_szWaypoint = pClassInfo->getWaypoint();
	if (getTime() == iDefault) m_iTime = pClassInfo->getTime();

	if (isSound() == bDefault) m_bSound = pClassInfo->isSound();
	if (isTarget() == bDefault) m_bTarget = pClassInfo->isTarget();
	if (isBuild() == bDefault) m_bBuild = pClassInfo->isBuild();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();

	if ( getEntityEvent() == ENTITY_EVENT_NONE ) m_eEntityEvent = pClassInfo->getEntityEvent();
}


//======================================================================================================
//					CvControlInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvControlInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvControlInfo::CvControlInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvControlInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvControlInfo::~CvControlInfo()
{
}

bool CvControlInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	return true;
}
void CvControlInfo::copyNonDefaults(const CvControlInfo* pClassInfo)
{
	CvHotkeyInfo::copyNonDefaults(pClassInfo);
}

//======================================================================================================
//					CvCommandInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCommandInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCommandInfo::CvCommandInfo() :
m_iAutomate(NO_AUTOMATE),
m_bConfirmCommand(false),
m_bVisible(false),
m_bAll(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCommandInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCommandInfo::~CvCommandInfo()
{
}

int CvCommandInfo::getAutomate() const
{
	return m_iAutomate;
}

bool CvCommandInfo::getConfirmCommand() const
{
	return m_bConfirmCommand;
}

bool CvCommandInfo::getVisible() const
{
	return m_bVisible;
}

bool CvCommandInfo::getAll() const
{
	return m_bAll;
}

bool CvCommandInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iAutomate, L"Automate");
	pXML->GetOptionalChildXmlValByName(&m_bConfirmCommand, L"bConfirmCommand");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");
	pXML->GetOptionalChildXmlValByName(&m_bAll, L"bAll");

	return true;
}

void CvCommandInfo::copyNonDefaults(const CvCommandInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getAutomate() == iTextDefault) m_iAutomate = pClassInfo->getAutomate();
	if (getConfirmCommand() == bDefault) m_bConfirmCommand = pClassInfo->getConfirmCommand();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
	if (getAll() == bDefault) m_bAll = pClassInfo->getAll();
}

//======================================================================================================
//					CvAutomateInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAutomateInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAutomateInfo::CvAutomateInfo() :
m_iCommand(NO_COMMAND),
m_iAutomate(NO_AUTOMATE),
m_bConfirmCommand(false),
m_bVisible(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAutomateInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAutomateInfo::~CvAutomateInfo()
{
}

int CvAutomateInfo::getCommand() const
{
	return m_iCommand;
}

int CvAutomateInfo::getAutomate() const
{
	return m_iAutomate;
}

bool CvAutomateInfo::getConfirmCommand() const
{
	return m_bConfirmCommand;
}

bool CvAutomateInfo::getVisible() const
{
	return m_bVisible;
}

bool CvAutomateInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iCommand, L"Command");
	pXML->GetOptionalTypeEnum(m_iAutomate, L"Automate");
	pXML->GetOptionalChildXmlValByName(&m_bConfirmCommand, L"bConfirmCommand");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");

	return true;
}

void CvAutomateInfo::copyNonDefaults(const CvAutomateInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getCommand() == iTextDefault) m_iCommand = pClassInfo->getCommand();
	if (getAutomate() == iTextDefault) m_iAutomate = pClassInfo->getAutomate();

	if (getConfirmCommand() == bDefault) m_bConfirmCommand = pClassInfo->getConfirmCommand();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
}

//======================================================================================================
//					CvActionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvActionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvActionInfo::CvActionInfo() :
m_iOriginalIndex(-1),
m_eSubType(NO_ACTIONSUBTYPE)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvActionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvActionInfo::~CvActionInfo()
{
}

int CvActionInfo::getMissionData() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_BUILD:
		case ACTIONSUBTYPE_RELIGION:
		case ACTIONSUBTYPE_CORPORATION:
		case ACTIONSUBTYPE_SPECIALIST:
		case ACTIONSUBTYPE_BUILDING:
		case ACTIONSUBTYPE_HERITAGE:
			return m_iOriginalIndex;
	}
	return -1;
}

int CvActionInfo::getCommandData() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_PROMOTION:
		case ACTIONSUBTYPE_UNIT:
			return m_iOriginalIndex;

		case ACTIONSUBTYPE_COMMAND:
			return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getAutomate();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getAutomate();
	}
	return -1;
}

int CvActionInfo::getAutomateType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_COMMAND:
			return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getAutomate();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getAutomate();
	}
	return NO_AUTOMATE;
}

int CvActionInfo::getInterfaceModeType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_INTERFACEMODE:
			return m_iOriginalIndex;
	}
	return NO_INTERFACEMODE;
}

int CvActionInfo::getMissionType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_BUILD:
			return GC.getBuildInfo((BuildTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_RELIGION:
			return GC.getReligionInfo((ReligionTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_CORPORATION:
			return GC.getCorporationInfo((CorporationTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_SPECIALIST:
			return GC.getSpecialistInfo((SpecialistTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_BUILDING:
			return GC.getBuildingInfo((BuildingTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_HERITAGE:
			return GC.getHeritageInfo((HeritageTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_MISSION:
			return m_iOriginalIndex;
	}
	return NO_MISSION;
}

int CvActionInfo::getCommandType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_COMMAND:
			return m_iOriginalIndex;

		case ACTIONSUBTYPE_PROMOTION:
			return GC.getPromotionInfo((PromotionTypes)m_iOriginalIndex).getCommandType();

		case ACTIONSUBTYPE_UNIT:
			return GC.getUnitInfo((UnitTypes)m_iOriginalIndex).getCommandType();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getCommand();
	}
	return NO_COMMAND;
}

int CvActionInfo::getControlType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_CONTROL:
			return m_iOriginalIndex;
	}
	return -1;
}

int CvActionInfo::getOriginalIndex() const
{
	return m_iOriginalIndex;
}

bool CvActionInfo::isConfirmCommand() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_COMMAND:
			return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getConfirmCommand();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getConfirmCommand();
	}
	return false;
}

bool CvActionInfo::isVisible() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_CONTROL:
			return false;

		case ACTIONSUBTYPE_COMMAND:
			return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getVisible();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getVisible();

		case ACTIONSUBTYPE_MISSION:
			return GC.getMissionInfo((MissionTypes)m_iOriginalIndex).getVisible();

		case ACTIONSUBTYPE_INTERFACEMODE:
			return GC.getInterfaceModeInfo((InterfaceModeTypes)m_iOriginalIndex).getVisible();
	}
	return true;
}

ActionSubTypes CvActionInfo::getSubType() const
{
	return m_eSubType;
}

const CvHotkeyInfo* CvActionInfo::getHotkeyInfo() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_INTERFACEMODE:
			return &GC.getInterfaceModeInfo((InterfaceModeTypes)getOriginalIndex());

		case ACTIONSUBTYPE_COMMAND:
			return &GC.getCommandInfo((CommandTypes)getOriginalIndex());

		case ACTIONSUBTYPE_BUILD:
			return &GC.getBuildInfo((BuildTypes)getOriginalIndex());

		case ACTIONSUBTYPE_PROMOTION:
			return &GC.getPromotionInfo((PromotionTypes)getOriginalIndex());

		case ACTIONSUBTYPE_UNIT:
			return &GC.getUnitInfo((UnitTypes)getOriginalIndex());

		case ACTIONSUBTYPE_RELIGION:
			return &GC.getReligionInfo((ReligionTypes)getOriginalIndex());

		case ACTIONSUBTYPE_CORPORATION:
			return &GC.getCorporationInfo((CorporationTypes)getOriginalIndex());

		case ACTIONSUBTYPE_SPECIALIST:
			return &GC.getSpecialistInfo((SpecialistTypes)getOriginalIndex());

		case ACTIONSUBTYPE_BUILDING:
			return &GC.getBuildingInfo((BuildingTypes)getOriginalIndex());

		case ACTIONSUBTYPE_HERITAGE:
			return &GC.getHeritageInfo((HeritageTypes)getOriginalIndex());

		case ACTIONSUBTYPE_CONTROL:
			return &GC.getControlInfo((ControlTypes)getOriginalIndex());

		case ACTIONSUBTYPE_AUTOMATE:
			return &GC.getAutomateInfo(getOriginalIndex());

		case ACTIONSUBTYPE_MISSION:
			return &GC.getMissionInfo((MissionTypes)getOriginalIndex());
	}
	FErrorMsg("Unknown Action Subtype in CvActionInfo::getHotkeyInfo");
	return NULL;
}

const char* CvActionInfo::getType() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getType() : NULL;
}

const wchar_t* CvActionInfo::getDescription() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getDescription() : L"";
}

const wchar_t* CvActionInfo::getCivilopedia() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getCivilopedia() : L"";
}

const wchar_t* CvActionInfo::getHelp() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHelp() : L"";
}

const wchar_t* CvActionInfo::getStrategy() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getStrategy() : L"";
}

const char* CvActionInfo::getButton() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getButton() : NULL;
}

const wchar_t* CvActionInfo::getTextKeyWide() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getTextKeyWide() : NULL;
}

int CvActionInfo::getActionInfoIndex() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getActionInfoIndex() : -1;
}

int CvActionInfo::getHotKeyVal() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyVal() : -1;
}

int CvActionInfo::getHotKeyPriority() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyPriority() : -1;
}

int CvActionInfo::getHotKeyValAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyValAlt() : -1;
}

int CvActionInfo::getHotKeyPriorityAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyPriorityAlt() : -1;
}

int CvActionInfo::getOrderPriority() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getOrderPriority() : 0;
}

bool CvActionInfo::isAltDown() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isAltDown() : false;
}

bool CvActionInfo::isShiftDown() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isShiftDown() : false;
}

bool CvActionInfo::isCtrlDown() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isCtrlDown() : false;
}

bool CvActionInfo::isAltDownAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isAltDownAlt() : false;
}

bool CvActionInfo::isShiftDownAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isShiftDownAlt() : false;
}

bool CvActionInfo::isCtrlDownAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isCtrlDownAlt() : false;
}

const char* CvActionInfo::getHotKey() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKey() : NULL;
}

std::wstring CvActionInfo::getHotKeyDescription() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyDescription() : L"";
}

//======================================================================================================
//					CvSpawnInfo
//======================================================================================================
CvSpawnInfo::CvSpawnInfo()
	:
	m_eUnitType(NO_UNIT),
	m_ePrereqTech(NO_TECH),
	m_eObsoleteTechType(NO_TECH),
	m_iPlayerType(-1),
	m_iTurns(-1),
	m_iGlobalTurns(-1),
	m_iMaxLocalDensity(0),
	m_iMinAreaPlotsPerPlayerUnit(0),
	m_iMinAreaPlotsPerUnitType(0),
	m_iStartDate(-200000),
	m_iEndDate(50000),
	m_bTreatAsBarbarian(false),
	m_bNeutralOnly(true),
	m_bNotInView(false),
	m_bNoSpeedNormalization(false),
	m_iRateOverride(100),
	m_bHills(false),
	m_bFlatlands(false),
	m_bPeaks(false),
	m_bFreshWaterOnly(false),
	m_bLatitudeAbs(true),
	m_iMinLatitude(-90),
	m_iMaxLatitude(90),
	m_iMinLongitude(-180),
	m_iMaxLongitude(180),
	m_pExprSpawnCondition(NULL)
{ }

CvSpawnInfo::~CvSpawnInfo()
{
	SAFE_DELETE(m_pExprSpawnCondition);
}

bool CvSpawnInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	CvString szTextVal2;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"UnitType");
	m_eUnitType = (UnitTypes)pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_eObsoleteTechType = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalTypeEnum(m_iPlayerType, L"PlayerType");

	pXML->GetOptionalChildXmlValByName(&m_iTurns, L"iTurns");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalTurns, L"iGlobalTurns", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLocalDensity, L"iMaxLocalDensity");
	pXML->GetOptionalChildXmlValByName(&m_iMinAreaPlotsPerPlayerUnit, L"iMinAreaPlotsPerPlayerUnit");
	pXML->GetOptionalChildXmlValByName(&m_iMinAreaPlotsPerUnitType, L"iMinAreaPlotsPerUnitType");
	pXML->GetOptionalChildXmlValByName(&m_iStartDate, L"iStartDate", -200000);
	pXML->GetOptionalChildXmlValByName(&m_iEndDate, L"iEndDate", 50000);
	pXML->GetOptionalChildXmlValByName(&m_bTreatAsBarbarian, L"bTreatAsBarbarian", false);
	pXML->GetOptionalChildXmlValByName(&m_bNeutralOnly, L"bNeutralOnly", true);
	pXML->GetOptionalChildXmlValByName(&m_bNoSpeedNormalization, L"bNoSpeedNormalization");
	pXML->GetOptionalChildXmlValByName(&m_bNotInView, L"bNotInView");
	pXML->GetOptionalChildXmlValByName(&m_bHills, L"bHills");
	pXML->GetOptionalChildXmlValByName(&m_bFlatlands, L"bFlatlands", true);
	pXML->GetOptionalChildXmlValByName(&m_bPeaks, L"bPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bFreshWaterOnly, L"bFreshWaterOnly");
	pXML->GetOptionalChildXmlValByName(&m_bLatitudeAbs, L"bLatitudeAbs", true);
	pXML->GetOptionalChildXmlValByName(&m_iMinLatitude, L"iMinLatitude", -90);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLatitude, L"iMaxLatitude", 90);
	pXML->GetOptionalChildXmlValByName(&m_iMinLongitude, L"iMinLongitude", -180);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLongitude, L"iMaxLongitude", 180);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"rateOverrideDefineName");
	if ( szTextVal.GetLength() != 0 )
	{
		m_iRateOverride = std::max(GC.getDefineINT(szTextVal, 100),0);
	}

	if(pXML->TryMoveToXmlFirstChild(L"BonusTypes"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"BonusType"))
			{
				do
				{
					int iBonusType;

					pXML->GetXmlVal(szTextVal);
					iBonusType = pXML->GetInfoClass(szTextVal);

					m_bonusTypes.push_back((BonusTypes)iBonusType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TerrainTypes"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainType"))
			{
				do
				{
					int iTerrainType;

					pXML->GetXmlVal(szTextVal);
					iTerrainType = pXML->GetInfoClass(szTextVal);

					m_terrainTypes.push_back((TerrainTypes)iTerrainType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureTypes"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"FeatureType"))
			{
				do
				{
					int iFeatureType;

					pXML->GetXmlVal(szTextVal);
					iFeatureType = pXML->GetInfoClass(szTextVal);

					m_featureTypes.push_back((FeatureTypes)iFeatureType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureTerrainTypes"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainType"))
			{
				do
				{
					int iTerrainType;

					pXML->GetXmlVal(szTextVal);
					iTerrainType = pXML->GetInfoClass(szTextVal);

					m_featureTerrainTypes.push_back((TerrainTypes)iTerrainType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"SpawnGroup"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitType"))
			{
				do
				{
					int iUnitType;

					pXML->GetXmlVal(szTextVal);
					iUnitType = pXML->GetInfoClass(szTextVal);

					m_spawnGroup.push_back((UnitTypes)iUnitType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpawnCondition"))
	{
		m_pExprSpawnCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvSpawnInfo::copyNonDefaults(CvSpawnInfo* pClassInfo)
{
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (!m_pExprSpawnCondition)
	{
		m_pExprSpawnCondition = pClassInfo->m_pExprSpawnCondition;
		pClassInfo->m_pExprSpawnCondition = NULL;
	}

	// not yet implemented
}

const BoolExpr* CvSpawnInfo::getSpawnCondition() const
{
	return m_pExprSpawnCondition;
}

int CvSpawnInfo::getTurnRate() const
{
	return m_iTurns;
}

int CvSpawnInfo::getGlobalTurnRate() const
{
	return m_iGlobalTurns;
}

int CvSpawnInfo::getMaxLocalDensity() const
{
	return m_iMaxLocalDensity;
}

int CvSpawnInfo::getMinAreaPlotsPerPlayerUnit() const
{
	return m_iMinAreaPlotsPerPlayerUnit;
}

int CvSpawnInfo::getMinAreaPlotsPerUnitType() const
{
	return m_iMinAreaPlotsPerUnitType;
}

int CvSpawnInfo::getStartDate() const
{
	return m_iStartDate;
}

int	CvSpawnInfo::getEndDate() const
{
	return m_iEndDate;
}

UnitTypes CvSpawnInfo::getUnitType() const
{
	return m_eUnitType;
}

PlayerTypes	CvSpawnInfo::getPlayer() const
{
	return (PlayerTypes)m_iPlayerType;
}

bool CvSpawnInfo::getTreatAsBarbarian() const
{
	return m_bTreatAsBarbarian;
}

bool CvSpawnInfo::getNeutralOnly() const
{
	if (GC.getUnitInfo(getUnitType()).isWildAnimal() && GC.getGame().isOption(GAMEOPTION_ANIMAL_DANGEROUS))
	{
		return false;
	}
	return m_bNeutralOnly;
}

bool CvSpawnInfo::getNoSpeedNormalization() const
{
	return m_bNoSpeedNormalization;
}

bool CvSpawnInfo::getNotInView() const
{
	return m_bNotInView;
}

bool CvSpawnInfo::getHills() const
{
	return m_bHills;
}

bool CvSpawnInfo::getFlatlands() const
{
	return m_bFlatlands;
}

bool CvSpawnInfo::getPeaks() const
{
	return m_bPeaks;
}

bool CvSpawnInfo::getFreshWaterOnly() const
{
	return m_bFreshWaterOnly;
}

bool CvSpawnInfo::getLatitudeAbs() const
{
	return m_bLatitudeAbs;
}

int CvSpawnInfo::getMinLatitude() const
{
	return m_iMinLatitude;
}

int CvSpawnInfo::getMaxLatitude() const
{
	return m_iMaxLatitude;
}

int CvSpawnInfo::getMinLongitude() const
{
	return m_iMinLongitude;
}

int CvSpawnInfo::getMaxLongitude() const
{
	return m_iMaxLongitude;
}

int CvSpawnInfo::getRateOverride() const
{
	return m_iRateOverride;
}

void CvSpawnInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_eUnitType);
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eObsoleteTechType);
	CheckSum(iSum, m_iPlayerType);
	CheckSum(iSum, m_iTurns);
	CheckSum(iSum, m_iGlobalTurns);
	CheckSum(iSum, m_iMaxLocalDensity);
	CheckSum(iSum, m_iMinAreaPlotsPerPlayerUnit);
	CheckSum(iSum, m_iMinAreaPlotsPerUnitType);
	CheckSum(iSum, m_iStartDate);
	CheckSum(iSum, m_iEndDate);
	CheckSum(iSum, m_bTreatAsBarbarian);
	CheckSum(iSum, m_bNeutralOnly);
	CheckSum(iSum, m_bNotInView);
	CheckSum(iSum, m_bNoSpeedNormalization);
	CheckSum(iSum, m_bHills);
	CheckSum(iSum, m_bFlatlands);
	CheckSum(iSum, m_bPeaks);
	CheckSum(iSum, m_bFreshWaterOnly);

	CheckSum(iSum, m_bLatitudeAbs);
	CheckSum(iSum, m_iMinLatitude);
	CheckSum(iSum, m_iMaxLatitude);
	CheckSum(iSum, m_iMinLongitude);
	CheckSum(iSum, m_iMaxLongitude);

	CheckSum(iSum, m_iRateOverride);
	CheckSumC(iSum, m_bonusTypes);
	CheckSumC(iSum, m_terrainTypes);
	CheckSumC(iSum, m_featureTypes);
	CheckSumC(iSum, m_featureTerrainTypes);
	CheckSumC(iSum, m_spawnGroup);

	if (m_pExprSpawnCondition)
	{
		m_pExprSpawnCondition->getCheckSum(iSum);
	}
}


//======================================================================================================
//					CvUnitFormationInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Default Constructor
//
//------------------------------------------------------------------------------------------------------
CvUnitFormationInfo::CvUnitFormationInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Destructor
//
//------------------------------------------------------------------------------------------------------
CvUnitFormationInfo::~CvUnitFormationInfo()
{
}

const char* CvUnitFormationInfo::getFormationType() const
{
	return m_szFormationType;
}

int CvUnitFormationInfo::getNumEventTypes() const
{
	return m_vctEventTypes.size();
}

const EntityEventTypes &CvUnitFormationInfo::getEventType(int index) const
{
	return m_vctEventTypes[index];
}
const std::vector<EntityEventTypes> & CvUnitFormationInfo::getEventTypes() const
{
	return m_vctEventTypes;
}

int CvUnitFormationInfo::getNumUnitEntries() const
{
	return m_vctUnitEntries.size();
}

const CvUnitEntry &CvUnitFormationInfo::getUnitEntry(int index) const
{
	return m_vctUnitEntries[index];
}

void CvUnitFormationInfo::addUnitEntry(const CvUnitEntry &unitEntry)
{
	m_vctUnitEntries.push_back(unitEntry);
}

int CvUnitFormationInfo::getNumGreatUnitEntries() const
{
	return m_vctGreatUnitEntries.size();
}

const CvUnitEntry &CvUnitFormationInfo::getGreatUnitEntry(int index) const
{
	return m_vctGreatUnitEntries[index];
}

int CvUnitFormationInfo::getNumSiegeUnitEntries() const
{
	return m_vctSiegeUnitEntries.size();
}

const CvUnitEntry &CvUnitFormationInfo::getSiegeUnitEntry(int index) const
{
	return m_vctSiegeUnitEntries[index];
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Reads from XML
//
//------------------------------------------------------------------------------------------------------
bool CvUnitFormationInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	int iIndex;
	bool bNextSibling;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szFormationType, L"FormationType");

	if (pXML->TryMoveToXmlFirstChild(L"EventMaskList" ))
	{
		if ( pXML->TryMoveToXmlFirstChild() )
		{
			pXML->GetXmlVal( szTextVal );
			do
			{
				iIndex = pXML->GetInfoClass(szTextVal);
				if ( iIndex != -1 )
					m_vctEventTypes.push_back( (EntityEventTypes)iIndex );
				bNextSibling = pXML->GetNextXmlVal( szTextVal );
			}
			while( bNextSibling );
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	// Read the entries
	if (pXML->TryMoveToXmlFirstChild(L"UnitEntry" ) )
	{
		do
		{
			CvUnitEntry unitEntry;
			pXML->GetChildXmlValByName(szTextVal, L"UnitEntryType");
			if (pXML->TryMoveToXmlFirstChild(L"Position" ) )
			{
				pXML->GetChildXmlValByName( &unitEntry.m_position.x, L"x");
				pXML->GetChildXmlValByName( &unitEntry.m_position.y, L"y");
				pXML->MoveToXmlParent();
			}
			pXML->GetChildXmlValByName( &unitEntry.m_fRadius, L"PositionRadius");
			pXML->GetChildXmlValByName( &unitEntry.m_fFacingDirection, L"Direction");
			pXML->GetChildXmlValByName( &unitEntry.m_fFacingVariance, L"DirVariation");

			if(szTextVal.CompareNoCase("Unit") == 0)
				m_vctUnitEntries.push_back(unitEntry);
			else if(szTextVal.CompareNoCase("General") == 0)
				m_vctGreatUnitEntries.push_back(unitEntry);
			else if(szTextVal.CompareNoCase("Siege") == 0)
				m_vctSiegeUnitEntries.push_back(unitEntry);
			else
			{
				//FErrorMsg("[Jason] Unknown unit formation entry type.");
			}
		}
		while ( pXML->TryMoveToXmlNextSibling(L"UnitEntry"));
		pXML->MoveToXmlParent();
	}

//	FAssertMsg(m_vctGreatUnitEntries.size() > 0, "[Jason] Formation missing great general entry.");
//	FAssertMsg(m_vctSiegeUnitEntries.size() > 0, "[Jason] Formation missing siege tower entry.");

	return true;
}
void CvUnitFormationInfo::copyNonDefaults(const CvUnitFormationInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getFormationType() == cDefault) m_szFormationType = pClassInfo->getFormationType();

	for ( int i = 0; i < pClassInfo->getNumEventTypes(); i++ )
	{
		m_vctEventTypes.push_back( pClassInfo->getEventType(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumUnitEntries(); i++ )
	{
		m_vctUnitEntries.push_back( pClassInfo->getUnitEntry(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumGreatUnitEntries(); i++ )
	{
		m_vctGreatUnitEntries.push_back( pClassInfo->getGreatUnitEntry(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumSiegeUnitEntries(); i++ )
	{
		m_vctSiegeUnitEntries.push_back( pClassInfo->getSiegeUnitEntry(i) );
	}
}


//======================================================================================================
//					CvSpecialUnitInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialUnitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialUnitInfo::CvSpecialUnitInfo() :
	m_bValid(false),
	m_bCityLoad(false),
	m_bSMLoadSame(false),
	m_iCombatPercent(0),
	m_iWithdrawalChange(0),
	m_iPursuitChange(0),
	m_pbCarrierUnitAITypes(NULL)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialUnitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialUnitInfo::~CvSpecialUnitInfo()
{
	SAFE_DELETE_ARRAY(m_pbCarrierUnitAITypes);
}

bool CvSpecialUnitInfo::isValid() const
{
	return m_bValid;
}

bool CvSpecialUnitInfo::isCityLoad() const
{
	return m_bCityLoad;
}

bool CvSpecialUnitInfo::isSMLoadSame() const
{
	return m_bSMLoadSame;
}

int CvSpecialUnitInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}

int CvSpecialUnitInfo::getWithdrawalChange() const
{
	return m_iWithdrawalChange;
}

int CvSpecialUnitInfo::getPursuitChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuitChange;
}

// Arrays

bool CvSpecialUnitInfo::isCarrierUnitAIType(int i) const
{
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, i);
	return m_pbCarrierUnitAITypes ? m_pbCarrierUnitAITypes[i] : false;
}

bool CvSpecialUnitInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(&m_bValid, L"bValid");
	pXML->GetOptionalChildXmlValByName(&m_bCityLoad, L"bCityLoad");
	pXML->GetOptionalChildXmlValByName(&m_bSMLoadSame, L"bSMLoadSame");
	pXML->GetOptionalChildXmlValByName(&m_iCombatPercent, L"iCombatPercent");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawalChange, L"iWithdrawalChange");
	pXML->GetOptionalChildXmlValByName(&m_iPursuitChange, L"iPursuitChange");

	pXML->SetVariableListTagPair(&m_pbCarrierUnitAITypes, L"CarrierUnitAITypes", NUM_UNITAI_TYPES);

	return true;
}

void CvSpecialUnitInfo::copyNonDefaults(const CvSpecialUnitInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const bool bDefault = false;
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isValid() == bDefault) m_bValid = pClassInfo->isValid();
	if (isCityLoad() == bDefault) m_bCityLoad = pClassInfo->isCityLoad();
	if (isSMLoadSame() == bDefault) m_bSMLoadSame = pClassInfo->isSMLoadSame();
	if (getCombatPercent() == iDefault) m_iCombatPercent = pClassInfo->getCombatPercent();
	if (getWithdrawalChange() == iDefault) m_iWithdrawalChange = pClassInfo->getWithdrawalChange();
	if (m_iPursuitChange == iDefault) m_iPursuitChange = pClassInfo->m_iPursuitChange;

	for ( int i = 0; i < NUM_UNITAI_TYPES; i++ )
	{
		if ( isCarrierUnitAIType(i) == bDefault && pClassInfo->isCarrierUnitAIType(i) != bDefault)
		{
			if ( NULL == m_pbCarrierUnitAITypes )
			{
				CvXMLLoadUtility::InitList(&m_pbCarrierUnitAITypes,NUM_UNITAI_TYPES,bDefault);
			}
			m_pbCarrierUnitAITypes[i] = pClassInfo->isCarrierUnitAIType(i);
		}
	}
}

void CvSpecialUnitInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bValid);
	CheckSum(iSum, m_bSMLoadSame);
	CheckSum(iSum, m_iCombatPercent);
	CheckSum(iSum, m_iWithdrawalChange);
	CheckSum(iSum, m_iPursuitChange);

	CheckSum(iSum, m_pbCarrierUnitAITypes, NUM_UNITAI_TYPES);
}

//======================================================================================================
//					CvCivicOptionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivicOptionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivicOptionInfo::CvCivicOptionInfo() :
	m_bPolicy(false),
	m_pabTraitNoUpkeep(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivicOptionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivicOptionInfo::~CvCivicOptionInfo()
{
	//SAFE_DELETE_ARRAY(m_pabTraitNoUpkeep);
}

bool CvCivicOptionInfo::isPolicy() const
{
	return m_bPolicy;
}
//bool CvCivicOptionInfo::getTraitNoUpkeep(int i) const
//{
//	FASSERT_BOUNDS(0, GC.getNumTraitInfos(), i);
//	return m_pabTraitNoUpkeep ? m_pabTraitNoUpkeep[i] : false;
//}

bool CvCivicOptionInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bPolicy, L"bPolicy");
	//pXML->SetVariableListTagPair(&m_pabTraitNoUpkeep, L"TraitNoUpkeeps", GC.getNumTraitInfos());

	return true;
}

void CvCivicOptionInfo::copyNonDefaults(const CvCivicOptionInfo* pClassInfo)
{
	CvInfoBase::copyNonDefaults(pClassInfo);

	if (!isPolicy()) m_bPolicy = pClassInfo->isPolicy();
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivicInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivicInfo::CvCivicInfo()

	: m_iCivicOptionType(NO_CIVICOPTION)
	, m_iAnarchyLength(0)
	, m_iUpkeep(NO_UPKEEP)
	, m_iAIWeight(0)
	, m_iGreatPeopleRateModifier(0)
	, m_iGreatGeneralRateModifier(0)
	, m_iDomesticGreatGeneralRateModifier(0)
	, m_iStateReligionGreatPeopleRateModifier(0)
	, m_iDistanceMaintenanceModifier(0)
	, m_iNumCitiesMaintenanceModifier(0)
	, m_iHomeAreaMaintenanceModifier(0)
	, m_iOtherAreaMaintenanceModifier(0)
	, m_iCorporationMaintenanceModifier(0)
	, m_iExtraHealth(0)
	, m_iFreeExperience(0)
	, m_iWorkerSpeedModifier(0)
	, m_iImprovementUpgradeRateModifier(0)
	, m_iMilitaryProductionModifier(0)
	, m_iFreeUnitUpkeepCivilian(0)
	, m_iFreeUnitUpkeepMilitary(0)
	, m_iFreeUnitUpkeepCivilianPopPercent(0)
	, m_iFreeUnitUpkeepMilitaryPopPercent(0)

	, m_iCivilianUnitUpkeepMod(0)
	, m_iMilitaryUnitUpkeepMod(0)

	, m_iHappyPerMilitaryUnit(0)
	, m_iLargestCityHappiness(0)
	, m_iWarWearinessModifier(0)
	, m_iFreeSpecialist(0)
	, m_iTradeRoutes(0)
	, m_iTechPrereq(NO_TECH)
	, m_iCivicPercentAnger(0)
	, m_iMaxConscript(0)
	, m_iStateReligionHappiness(0)
	, m_iNonStateReligionHappiness(0)
	, m_iStateReligionUnitProductionModifier(0)
	, m_iStateReligionBuildingProductionModifier(0)
	, m_iStateReligionFreeExperience(0)
	, m_iExpInBorderModifier(0)

	, m_bUpgradeAnywhere(false)
	, m_bAllowInquisitions(false)
	, m_bDisallowInquisitions(false)
	, m_iRevIdxLocal(0)
	, m_iRevIdxNational(0)
	, m_iRevIdxDistanceModifier(0)
	, m_iRevIdxHolyCityGood(0)
	, m_iRevIdxHolyCityBad(0)
	, m_iRevIdxSwitchTo(0)
	, m_fRevIdxNationalityMod(0)
	, m_fRevIdxGoodReligionMod(0)
	, m_fRevIdxBadReligionMod(0)
	, m_fRevViolentMod(0)
	, m_iRevReligiousFreedom(0)
	, m_iRevLaborFreedom(0)
	, m_iRevEnvironmentalProtection(0)
	, m_iRevDemocracyLevel(0)
	, m_bCommunism(false)
	, m_bFreeSpeech(false)
	, m_bCanDoElection(false)

	, m_bMilitaryFoodProduction(false)
	, m_bNoUnhealthyPopulation(false)
	, m_bBuildingOnlyHealthy(false)
	, m_bNoForeignTrade(false)
	, m_bNoCorporations(false)
	, m_bNoForeignCorporations(false)
	, m_bStateReligion(false)
	, m_bNoNonStateReligionSpread(false)
	, m_piYieldModifier(NULL)
	, m_piCapitalYieldModifier(NULL)
	, m_piTradeYieldModifier(NULL)
	, m_piCommerceModifier(NULL)
	, m_piCapitalCommerceModifier(NULL)
	, m_piSpecialistExtraCommerce(NULL)
	, m_paiBuildingHappinessChanges(NULL)
	, m_paiBuildingHealthChanges(NULL)
	, m_paiFeatureHappinessChanges(NULL)
	, m_pabHurry(NULL)
	, m_pabSpecialBuildingNotRequired(NULL)
	, m_pabSpecialistValid(NULL)

	, m_ppiImprovementYieldChanges(NULL)
	, m_bAnyImprovementYieldChange(false)

	, m_iAttitudeShareMod(0)
	, m_iEnslavementChance(0)
	, m_iPopulationgrowthratepercentage(0)
	, m_iReligionSpreadRate(0)
	, m_iCivicHappiness(0)
	, m_iDistantUnitSupportCostModifier(0)
	, m_iExtraCityDefense(0)
	, m_iForeignTradeRouteModifier(0)
	, m_iTaxRateUnhappiness(0)
	, m_iInflationModifier(0)
	, m_iHurryInflationModifier(0)
	, m_iHurryCostModifier(0)
	, m_iSharedCivicTradeRouteModifier(0)
	, m_iLandmarkHappiness(0)
	, m_iCorporationSpreadRate(0)
	, m_iForeignerUnhappyPercent(0)
	, m_iCityLimit(0)
	, m_iCityOverLimitUnhappy(0)
	, m_bFixedBorders(false)
	, m_bNoCapitalUnhappiness(false)
	, m_bNoLandmarkAnger(false)
	, m_piBonusMintedPercent(NULL)
	, m_piImprovementHappinessChanges(NULL)
	, m_piImprovementHealthPercentChanges(NULL)
	, m_ppiSpecialistYieldPercentChanges(NULL)
	, m_ppiSpecialistCommercePercentChanges(NULL)
	, m_piLandmarkYieldChanges(NULL)
	, m_piFreeSpecialistCount(NULL)
	, m_paiUnitCombatProductionModifier(NULL)
	, m_piUnitProductionModifier(NULL)
	, m_ppiTerrainYieldChanges(NULL)
	, m_piFlavorValue(NULL)
	, m_piCivicAttitudeChanges(NULL)
	, m_pszCivicAttitudeReason(NULL)
	, m_ppiBuildingCommerceModifier(NULL)
	, m_ppiBuildingCommerceChange(NULL)
	, m_ppiBonusCommerceModifier(NULL)
	, m_iNationalCaptureProbabilityModifier(0)
	, m_iNationalCaptureResistanceModifier(0)
	, m_iFreedomFighterChange(0)
	, m_bAllReligionsActive(false)
	, m_bBansNonStateReligions(false)
	, m_bFreedomFighter(false)
	, m_bPolicy(false)
	, m_PropertyManipulators()
	{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivicInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivicInfo::~CvCivicInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_piTradeYieldModifier);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	SAFE_DELETE_ARRAY(m_piCapitalCommerceModifier);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_paiBuildingHappinessChanges);
	SAFE_DELETE_ARRAY(m_paiBuildingHealthChanges);
	SAFE_DELETE_ARRAY(m_paiFeatureHappinessChanges);
	SAFE_DELETE_ARRAY(m_pabHurry);
	SAFE_DELETE_ARRAY(m_pabSpecialBuildingNotRequired);
	SAFE_DELETE_ARRAY(m_pabSpecialistValid);
	SAFE_DELETE_ARRAY2(m_ppiImprovementYieldChanges, GC.getNumImprovementInfos());
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piBonusMintedPercent);
	SAFE_DELETE_ARRAY(m_piCivicAttitudeChanges);
	SAFE_DELETE_ARRAY(m_pszCivicAttitudeReason);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitProductionModifier);
	SAFE_DELETE_ARRAY(m_piLandmarkYieldChanges);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piImprovementHappinessChanges);
	SAFE_DELETE_ARRAY(m_piImprovementHealthPercentChanges);
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceChange, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBonusCommerceModifier, GC.getNumBonusInfos());
	SAFE_DELETE_ARRAY2(m_ppiTerrainYieldChanges, GC.getNumTerrainInfos());
	SAFE_DELETE_ARRAY2(m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos());
}

const wchar_t* CvCivicInfo::getWeLoveTheKing() const
{
	return m_szWeLoveTheKingKey;
}

const wchar_t* CvCivicInfo::getWeLoveTheKingKey() const
{
	return m_szWeLoveTheKingKey;
}

int CvCivicInfo::getCivicOptionType() const
{
	return m_iCivicOptionType;
}

int CvCivicInfo::getAnarchyLength() const
{
	return m_iAnarchyLength;
}

int CvCivicInfo::getUpkeep() const
{
	return m_iUpkeep;
}

int CvCivicInfo::getAIWeight() const
{
	return m_iAIWeight;
}

int CvCivicInfo::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}

int CvCivicInfo::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}

int CvCivicInfo::getDomesticGreatGeneralRateModifier() const
{
	return m_iDomesticGreatGeneralRateModifier;
}

int CvCivicInfo::getStateReligionGreatPeopleRateModifier() const
{
	return m_iStateReligionGreatPeopleRateModifier;
}

int CvCivicInfo::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}

int CvCivicInfo::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;
}
//DPII < Maintenance Modifiers >
int CvCivicInfo::getHomeAreaMaintenanceModifier() const
{
	return m_iHomeAreaMaintenanceModifier;
}

int CvCivicInfo::getOtherAreaMaintenanceModifier() const
{
	return m_iOtherAreaMaintenanceModifier;
}
//DPII < Maintenance Modifiers >
int CvCivicInfo::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}

int CvCivicInfo::getExtraHealth() const
{
	return m_iExtraHealth;
}

int CvCivicInfo::getFreeExperience() const
{
	return m_iFreeExperience;
}

int CvCivicInfo::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}

int CvCivicInfo::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}

int CvCivicInfo::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}

int CvCivicInfo::getFreeUnitUpkeepCivilian() const
{
	return m_iFreeUnitUpkeepCivilian;
}

int CvCivicInfo::getFreeUnitUpkeepMilitary() const
{
	return m_iFreeUnitUpkeepMilitary;
}

int CvCivicInfo::getFreeUnitUpkeepCivilianPopPercent() const
{
	return m_iFreeUnitUpkeepCivilianPopPercent;
}

int CvCivicInfo::getFreeUnitUpkeepMilitaryPopPercent() const
{
	return m_iFreeUnitUpkeepMilitaryPopPercent;
}

int CvCivicInfo::getCivilianUnitUpkeepMod() const
{
	return m_iCivilianUnitUpkeepMod;
}

int CvCivicInfo::getMilitaryUnitUpkeepMod() const
{
	return m_iMilitaryUnitUpkeepMod;
}

int CvCivicInfo::getHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}

int CvCivicInfo::getLargestCityHappiness() const
{
	return m_iLargestCityHappiness;
}

int CvCivicInfo::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}

int CvCivicInfo::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}

int CvCivicInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}

int CvCivicInfo::getCivicPercentAnger() const
{
	return m_iCivicPercentAnger;
}

int CvCivicInfo::getMaxConscript() const
{
	return m_iMaxConscript;
}

int CvCivicInfo::getStateReligionHappiness() const
{
	return m_iStateReligionHappiness;
}

int CvCivicInfo::getNonStateReligionHappiness() const
{
	return m_iNonStateReligionHappiness;
}

int CvCivicInfo::getStateReligionUnitProductionModifier() const
{
	return m_iStateReligionUnitProductionModifier;
}

int CvCivicInfo::getStateReligionBuildingProductionModifier() const
{
	return m_iStateReligionBuildingProductionModifier;
}

int CvCivicInfo::getStateReligionFreeExperience() const
{
	return m_iStateReligionFreeExperience;
}

int CvCivicInfo::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}

int CvCivicInfo::getRevIdxLocal() const
{
	return m_iRevIdxLocal;
}

int CvCivicInfo::getRevIdxNational() const
{
	return m_iRevIdxNational;
}

int CvCivicInfo::getRevIdxDistanceModifier() const
{
	return m_iRevIdxDistanceModifier;
}

int CvCivicInfo::getRevIdxHolyCityGood() const
{
	return m_iRevIdxHolyCityGood;
}

int CvCivicInfo::getRevIdxHolyCityBad() const
{
	return m_iRevIdxHolyCityBad;
}

int CvCivicInfo::getRevIdxSwitchTo() const
{
	return m_iRevIdxSwitchTo;
}

int CvCivicInfo::getRevReligiousFreedom() const
{
	return m_iRevReligiousFreedom;
}

int CvCivicInfo::getRevLaborFreedom() const
{
	return m_iRevLaborFreedom;
}

int CvCivicInfo::getRevEnvironmentalProtection() const
{
	return m_iRevEnvironmentalProtection;
}

int CvCivicInfo::getRevDemocracyLevel() const
{
	return m_iRevDemocracyLevel;
}

int CvCivicInfo::getAttitudeShareMod() const
{
	return m_iAttitudeShareMod;
}

int CvCivicInfo::getEnslavementChance() const
{
	return m_iEnslavementChance;
}

int CvCivicInfo::getForeignerUnhappyPercent() const
{
	return m_iForeignerUnhappyPercent;
}

int CvCivicInfo::getCityOverLimitUnhappy() const
{
	return m_iCityOverLimitUnhappy;
}

int CvCivicInfo::getLandmarkHappiness() const
{
	return m_iLandmarkHappiness;
}

int CvCivicInfo::getForeignTradeRouteModifier() const
{
	return m_iForeignTradeRouteModifier;
}

int CvCivicInfo::getTaxRateUnhappiness() const
{
	return m_iTaxRateUnhappiness;
}

int CvCivicInfo::getPopulationgrowthratepercentage() const
{
	return m_iPopulationgrowthratepercentage;
}

int CvCivicInfo::getReligionSpreadRate() const
{
	return m_iReligionSpreadRate;
}

int CvCivicInfo::getCivicHappiness() const
{
	return m_iCivicHappiness;
}

int CvCivicInfo::getDistantUnitSupportCostModifier() const
{
	return m_iDistantUnitSupportCostModifier;
}

int CvCivicInfo::getExtraCityDefense() const
{
	return m_iExtraCityDefense;
}

int CvCivicInfo::getNationalCaptureProbabilityModifier() const
{
	return m_iNationalCaptureProbabilityModifier;
}

int CvCivicInfo::getNationalCaptureResistanceModifier() const
{
	return m_iNationalCaptureResistanceModifier;
}

int CvCivicInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}

int CvCivicInfo::getHurryInflationModifier() const
{
	return m_iHurryInflationModifier;
}

int CvCivicInfo::getHurryCostModifier() const
{
	return m_iHurryCostModifier;
}

int CvCivicInfo::getSharedCivicTradeRouteModifier() const
{
	return m_iSharedCivicTradeRouteModifier;
}

int CvCivicInfo::getCorporationSpreadRate() const
{
	return m_iCorporationSpreadRate;
}

int CvCivicInfo::getFreedomFighterChange() const
{
	return m_iFreedomFighterChange;
}


float CvCivicInfo::getRevIdxNationalityMod() const
{
	return m_fRevIdxNationalityMod;
}

float CvCivicInfo::getRevIdxBadReligionMod() const
{
	return m_fRevIdxBadReligionMod;
}

float CvCivicInfo::getRevIdxGoodReligionMod() const
{
	return m_fRevIdxGoodReligionMod;
}

float CvCivicInfo::getRevViolentMod() const
{
	return m_fRevViolentMod;
}


bool CvCivicInfo::isUpgradeAnywhere() const
{
	return m_bUpgradeAnywhere;
}

bool CvCivicInfo::isAllowInquisitions() const
{
	return m_bAllowInquisitions;
}

bool CvCivicInfo::isDisallowInquisitions() const
{
	return m_bDisallowInquisitions;
}

bool CvCivicInfo::isCommunism() const
{
	return m_bCommunism;
}

bool CvCivicInfo::isFreeSpeech() const
{
	return m_bFreeSpeech;
}

bool CvCivicInfo::isCanDoElection() const
{
	return m_bCanDoElection;
}

bool CvCivicInfo::isMilitaryFoodProduction() const
{
	return m_bMilitaryFoodProduction;
}

bool CvCivicInfo::isNoUnhealthyPopulation() const
{
	return m_bNoUnhealthyPopulation;
}

bool CvCivicInfo::isBuildingOnlyHealthy() const
{
	return m_bBuildingOnlyHealthy;
}

bool CvCivicInfo::isNoForeignTrade() const
{
	return m_bNoForeignTrade;
}

bool CvCivicInfo::isNoCorporations() const
{
	return m_bNoCorporations;
}

bool CvCivicInfo::isNoForeignCorporations() const
{
	return m_bNoForeignCorporations;
}

bool CvCivicInfo::isStateReligion() const
{
	return m_bStateReligion;
}

bool CvCivicInfo::isNoNonStateReligionSpread() const
{
	return m_bNoNonStateReligionSpread;
}

bool CvCivicInfo::IsFixedBorders() const
{
	return m_bFixedBorders;
}

bool CvCivicInfo::isNoCapitalUnhappiness() const
{
	return m_bNoCapitalUnhappiness;
}

bool CvCivicInfo::isNoLandmarkAnger() const
{
	return m_bNoLandmarkAnger;
}

bool CvCivicInfo::isAllReligionsActive() const
{
	return m_bAllReligionsActive;
}

bool CvCivicInfo::isBansNonStateReligions() const
{
	return m_bBansNonStateReligions;
}

bool CvCivicInfo::isFreedomFighter() const
{
	return m_bFreedomFighter;
}

bool CvCivicInfo::isPolicy() const
{
	return m_bPolicy;
}


// Arrays
int CvCivicInfo::getYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldModifier ? m_piYieldModifier[i] : 0;
}

int* CvCivicInfo::getYieldModifierArray() const
{
	return m_piYieldModifier;
}

int CvCivicInfo::getCapitalYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
}

int* CvCivicInfo::getCapitalYieldModifierArray() const
{
	return m_piCapitalYieldModifier;
}

int CvCivicInfo::getTradeYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piTradeYieldModifier ? m_piTradeYieldModifier[i] : 0;
}

int* CvCivicInfo::getTradeYieldModifierArray() const
{
	return m_piTradeYieldModifier;
}

int CvCivicInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int* CvCivicInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}

int CvCivicInfo::getCapitalCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
}

int* CvCivicInfo::getCapitalCommerceModifierArray() const
{
	return m_piCapitalCommerceModifier;
}

int CvCivicInfo::getSpecialistExtraCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
}

int* CvCivicInfo::getSpecialistExtraCommerceArray() const
{
	return m_piSpecialistExtraCommerce;
}

int CvCivicInfo::getCivicAttitudeChange(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), i);
	return m_piCivicAttitudeChanges ? m_piCivicAttitudeChanges[i] : 0;
}

int* CvCivicInfo::getCivicAttitudeChanges() const
{
	return m_piCivicAttitudeChanges;
}

int CvCivicInfo::getLandmarkYieldChanges(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piLandmarkYieldChanges ? m_piLandmarkYieldChanges[i] : 0;
}

int* CvCivicInfo::getLandmarkYieldChangesArray() const
{
	return m_piLandmarkYieldChanges;
}


int CvCivicInfo::getBonusCommerceModifier(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);
	return (m_ppiBonusCommerceModifier && m_ppiBonusCommerceModifier[i]) ? m_ppiBonusCommerceModifier[i][j] : 0;
}

int* CvCivicInfo::getBonusCommerceModifierArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_ppiBonusCommerceModifier ? m_ppiBonusCommerceModifier[i] : NULL;
}


int CvCivicInfo::getBuildingHappinessChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	return m_paiBuildingHappinessChanges ? m_paiBuildingHappinessChanges[i] : 0;
}

int CvCivicInfo::getBuildingHealthChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	return m_paiBuildingHealthChanges ? m_paiBuildingHealthChanges[i] : 0;
}

int CvCivicInfo::getFeatureHappinessChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_paiFeatureHappinessChanges ? m_paiFeatureHappinessChanges[i] : 0;
}

int CvCivicInfo::getBonusMintedPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_piBonusMintedPercent ? m_piBonusMintedPercent[i] : 0;
}

int CvCivicInfo::getFreeSpecialistCount(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}

int CvCivicInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvCivicInfo::getUnitCombatProductionModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return m_paiUnitCombatProductionModifier ? m_paiUnitCombatProductionModifier[i] : 0;
}

int CvCivicInfo::getUnitProductionModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), i);
	return m_piUnitProductionModifier ? m_piUnitProductionModifier[i] : 0;
}

int CvCivicInfo::getBuildingProductionModifier(BuildingTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), e);
	return m_aBuildingProductionModifier.getValue(e);
}

int CvCivicInfo::getImprovementHappinessChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	return m_piImprovementHappinessChanges ? m_piImprovementHappinessChanges[i] : 0;
}

int CvCivicInfo::getImprovementHealthPercentChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	return m_piImprovementHealthPercentChanges ? m_piImprovementHealthPercentChanges[i] : 0;
}


bool CvCivicInfo::isHurry(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumHurryInfos(), i);
	return m_pabHurry ? m_pabHurry[i] : false;
}

bool CvCivicInfo::isSpecialBuildingNotRequired(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), i);
	return m_pabSpecialBuildingNotRequired ? m_pabSpecialBuildingNotRequired[i] : false;
}

bool CvCivicInfo::isSpecialistValid(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	return m_pabSpecialistValid ? m_pabSpecialistValid[i] : false;
}


int CvCivicInfo::getImprovementYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return (m_ppiImprovementYieldChanges && m_ppiImprovementYieldChanges[i]) ? m_ppiImprovementYieldChanges[i][j] : 0;
}

int CvCivicInfo::getTerrainYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return (m_ppiTerrainYieldChanges && m_ppiTerrainYieldChanges[i]) ? m_ppiTerrainYieldChanges[i][j] : 0;
}

int CvCivicInfo::getBuildingCommerceModifier(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);
	return (m_ppiBuildingCommerceModifier && m_ppiBuildingCommerceModifier[i]) ? m_ppiBuildingCommerceModifier[i][j] : 0;
}

int CvCivicInfo::getBuildingCommerceChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);
	return (m_ppiBuildingCommerceChange && m_ppiBuildingCommerceChange[i]) ? m_ppiBuildingCommerceChange[i][j] : 0;
}

int CvCivicInfo::getSpecialistYieldPercentChanges(int i, int j ) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return (m_ppiSpecialistYieldPercentChanges && m_ppiSpecialistYieldPercentChanges[i]) ? m_ppiSpecialistYieldPercentChanges[i][j] : 0;
}

int CvCivicInfo::getSpecialistCommercePercentChanges(int i, int j ) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);
	return (m_ppiSpecialistCommercePercentChanges && m_ppiSpecialistCommercePercentChanges[i]) ? m_ppiSpecialistCommercePercentChanges[i][j] : 0;
}


int CvCivicInfo::getCityLimit(PlayerTypes ePlayer) const
{
	if (ePlayer > NO_PLAYER && GC.getGame().isOption(GAMEOPTION_EXP_OVEREXPANSION_PENALTIES))
	{
		const int iAdaptID = GC.getInfoTypeForString("ADAPT_SCALE_CITY_LIMITS");
		return GET_PLAYER(ePlayer).getGameObject()->adaptValueToGame(iAdaptID , m_iCityLimit);
	}
	return 0;
}


bool CvCivicInfo::isAnyBuildingHappinessChange() const
{
	return (m_paiBuildingHappinessChanges != NULL);
}

bool CvCivicInfo::isAnyBuildingHealthChange() const
{
	return (m_paiBuildingHealthChanges != NULL);
}

bool CvCivicInfo::isAnyFeatureHappinessChange() const
{
	return (m_paiFeatureHappinessChanges != NULL);
}

bool CvCivicInfo::isAnySpecialistValid() const
{
	return (m_pabSpecialistValid != NULL);
}

bool CvCivicInfo::isAnyImprovementYieldChange() const
{
	return m_bAnyImprovementYieldChange;
}

CvString CvCivicInfo::getCivicAttitudeReason(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), i);
	return m_pszCivicAttitudeReason[i];
}

int CvCivicInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvCivicInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvCivicInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

int CvCivicInfo::getCivicAttitudeVectorSize() const						{ return m_aszCivicAttitudeforPass3.size(); }
CvString CvCivicInfo::getCivicAttitudeNamesVectorElement(int i) const	{ return m_aszCivicAttitudeforPass3[i]; }
int CvCivicInfo::getCivicAttitudeValuesVectorElement(int i) const		{ return m_aiCivicAttitudeforPass3[i]; }

int CvCivicInfo::getCivicAttitudeReasonVectorSize() const						{ return m_aszCivicAttitudeReasonforPass3.size(); }
CvString CvCivicInfo::getCivicAttitudeReasonNamesVectorElement(int i) const		{ return m_aszCivicAttitudeReasonforPass3[i]; }
CvString CvCivicInfo::getCivicAttitudeReasonValuesVectorElement(int i) const	{ return m_aszCivicAttitudeReasonValueforPass3[i]; }


void CvCivicInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iCivicOptionType);
	CheckSum(iSum, m_iAnarchyLength);
	CheckSum(iSum, m_iUpkeep);
	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_iGreatPeopleRateModifier);
	CheckSum(iSum, m_iGreatGeneralRateModifier);
	CheckSum(iSum, m_iDomesticGreatGeneralRateModifier);
	CheckSum(iSum, m_iStateReligionGreatPeopleRateModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iHomeAreaMaintenanceModifier);
	CheckSum(iSum, m_iOtherAreaMaintenanceModifier);
	CheckSum(iSum, m_iCorporationMaintenanceModifier);
	CheckSum(iSum, m_iExtraHealth);
	CheckSum(iSum, m_iFreeExperience);
	CheckSum(iSum, m_iWorkerSpeedModifier);
	CheckSum(iSum, m_iImprovementUpgradeRateModifier);
	CheckSum(iSum, m_iMilitaryProductionModifier);
	CheckSum(iSum, m_iFreeUnitUpkeepCivilian);
	CheckSum(iSum, m_iFreeUnitUpkeepMilitary);
	CheckSum(iSum, m_iFreeUnitUpkeepCivilianPopPercent);
	CheckSum(iSum, m_iFreeUnitUpkeepMilitaryPopPercent);
	CheckSum(iSum, m_iCivilianUnitUpkeepMod);
	CheckSum(iSum, m_iMilitaryUnitUpkeepMod);
	CheckSum(iSum, m_iHappyPerMilitaryUnit);
	CheckSum(iSum, m_iLargestCityHappiness);
	CheckSum(iSum, m_iWarWearinessModifier);
	CheckSum(iSum, m_iFreeSpecialist);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iCivicPercentAnger);
	CheckSum(iSum, m_iMaxConscript);
	CheckSum(iSum, m_iStateReligionHappiness);
	CheckSum(iSum, m_iNonStateReligionHappiness);
	CheckSum(iSum, m_iStateReligionUnitProductionModifier);
	CheckSum(iSum, m_iStateReligionBuildingProductionModifier);
	CheckSum(iSum, m_iStateReligionFreeExperience);
	CheckSum(iSum, m_iExpInBorderModifier);
	CheckSum(iSum, m_iRevIdxLocal);
	CheckSum(iSum, m_iRevIdxNational);
	CheckSum(iSum, m_iRevIdxDistanceModifier);
	CheckSum(iSum, m_iRevIdxHolyCityGood);
	CheckSum(iSum, m_iRevIdxHolyCityBad);
	CheckSum(iSum, m_iRevIdxSwitchTo);
	CheckSum(iSum, m_iRevReligiousFreedom);
	CheckSum(iSum, m_iRevLaborFreedom);
	CheckSum(iSum, m_iRevEnvironmentalProtection);
	CheckSum(iSum, m_iRevDemocracyLevel);
	CheckSum(iSum, m_iAttitudeShareMod);
	CheckSum(iSum, m_iEnslavementChance);
	CheckSum(iSum, m_iPopulationgrowthratepercentage);
	CheckSum(iSum, m_iReligionSpreadRate);
	CheckSum(iSum, m_iCivicHappiness);
	CheckSum(iSum, m_iDistantUnitSupportCostModifier);
	CheckSum(iSum, m_iExtraCityDefense);
	CheckSum(iSum, m_iForeignTradeRouteModifier);
	CheckSum(iSum, m_iTaxRateUnhappiness);
	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_iHurryInflationModifier);
	CheckSum(iSum, m_iHurryCostModifier);
	CheckSum(iSum, m_iSharedCivicTradeRouteModifier);
	CheckSum(iSum, m_iLandmarkHappiness);
	CheckSum(iSum, m_iCorporationSpreadRate);
	CheckSum(iSum, m_iForeignerUnhappyPercent);
	CheckSum(iSum, m_iCityLimit);
	CheckSum(iSum, m_iNationalCaptureProbabilityModifier);
	CheckSum(iSum, m_iNationalCaptureResistanceModifier);
	CheckSum(iSum, m_iFreedomFighterChange);

	CheckSum(iSum, m_fRevIdxNationalityMod);
	CheckSum(iSum, m_fRevIdxBadReligionMod);
	CheckSum(iSum, m_fRevIdxGoodReligionMod);
	CheckSum(iSum, m_fRevViolentMod);

	CheckSum(iSum, m_bUpgradeAnywhere);
	CheckSum(iSum, m_bAllowInquisitions);
	CheckSum(iSum, m_bDisallowInquisitions);
	CheckSum(iSum, m_bCommunism);
	CheckSum(iSum, m_bFreeSpeech);
	CheckSum(iSum, m_bCanDoElection);
	CheckSum(iSum, m_bMilitaryFoodProduction);
	CheckSum(iSum, m_bNoUnhealthyPopulation);
	CheckSum(iSum, m_bBuildingOnlyHealthy);
	CheckSum(iSum, m_bNoForeignTrade);
	CheckSum(iSum, m_bNoCorporations);
	CheckSum(iSum, m_bNoForeignCorporations);
	CheckSum(iSum, m_bStateReligion);
	CheckSum(iSum, m_bNoNonStateReligionSpread);
	CheckSum(iSum, m_bFixedBorders);
	CheckSum(iSum, m_bNoCapitalUnhappiness);
	CheckSum(iSum, m_bNoLandmarkAnger);
	CheckSum(iSum, m_bAllReligionsActive);
	CheckSum(iSum, m_bBansNonStateReligions);
	CheckSum(iSum, m_bFreedomFighter);
	CheckSum(iSum, m_bPolicy);

	// Arrays

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piCapitalYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piTradeYieldModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCommerceModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCapitalCommerceModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerce);
	CheckSumI(iSum, GC.getNumBuildingInfos(), m_paiBuildingHappinessChanges);
	CheckSumI(iSum, GC.getNumBuildingInfos(), m_paiBuildingHealthChanges);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_paiFeatureHappinessChanges);
	CheckSumI(iSum, GC.getNumHurryInfos(), m_pabHurry);
	CheckSumI(iSum, GC.getNumSpecialBuildingInfos(), m_pabSpecialBuildingNotRequired);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_pabSpecialistValid);

	int i;
	if (m_ppiImprovementYieldChanges)
		for(i=0;i<GC.getNumImprovementInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiImprovementYieldChanges[i]);
		}

	CheckSumI(iSum, GC.getNumBonusInfos(), m_piBonusMintedPercent);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_paiUnitCombatProductionModifier);
	CheckSumI(iSum, GC.getNumUnitInfos(), m_piUnitProductionModifier);
	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piFlavorValue);
	CheckSumI(iSum, GC.getNumCivicInfos(), m_piCivicAttitudeChanges);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piLandmarkYieldChanges);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_piFreeSpecialistCount);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementHappinessChanges);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementHealthPercentChanges);

	if (m_ppiSpecialistYieldPercentChanges)
	{
		for(i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiSpecialistYieldPercentChanges[i]);
		}
	}

	if (m_ppiSpecialistCommercePercentChanges)
	{
		for(i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiSpecialistCommercePercentChanges[i]);
		}
	}

	if (m_ppiTerrainYieldChanges)
	{
		for(i=0;i<GC.getNumTerrainInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiTerrainYieldChanges[i]);
		}
	}

	if (m_ppiBuildingCommerceModifier)
	{
		for(i=0;i<GC.getNumBuildingInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiBuildingCommerceModifier[i]);
		}
	}

	if (m_ppiBuildingCommerceChange)
	{
		for(i=0;i<GC.getNumBuildingInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiBuildingCommerceChange[i]);
		}
	}

	if (m_ppiBonusCommerceModifier)
	{
		for(i=0;i<GC.getNumBonusInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiBonusCommerceModifier[i]);
		}
	}

	CheckSumC(iSum, m_aBuildingProductionModifier);
	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}

bool CvCivicInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int j;
	int iNumSibs=0;				// the number of siblings the current xml node has
	int iIndex;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivicOptionType");
	m_iCivicOptionType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

	pXML->GetOptionalChildXmlValByName(&m_iAnarchyLength, L"iAnarchyLength");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Upkeep");
	m_iUpkeep = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iAIWeight, L"iAIWeight");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateModifier, L"iGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatGeneralRateModifier, L"iGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, L"iDomesticGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionGreatPeopleRateModifier, L"iStateReligionGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHomeAreaMaintenanceModifier, L"iHomeAreaMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iOtherAreaMaintenanceModifier, L"iOtherAreaMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenanceModifier, L"iCorporationMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iExtraHealth, L"iExtraHealth");
	pXML->GetOptionalChildXmlValByName(&m_iFreeExperience, L"iFreeExperience");
	pXML->GetOptionalChildXmlValByName(&m_iWorkerSpeedModifier, L"iWorkerSpeedModifier");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementUpgradeRateModifier, L"iImprovementUpgradeRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryProductionModifier, L"iMilitaryProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepCivilian, L"iFreeUnitUpkeepCivilian");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepMilitary, L"iFreeUnitUpkeepMilitary");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepCivilianPopPercent, L"iFreeUnitUpkeepCivilianPopPercent");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepMilitaryPopPercent, L"iFreeUnitUpkeepMilitaryPopPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCivilianUnitUpkeepMod, L"iCivilianUnitUpkeepMod");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryUnitUpkeepMod, L"iMilitaryUnitUpkeepMod");
	pXML->GetOptionalChildXmlValByName(&m_iHappyPerMilitaryUnit, L"iHappyPerMilitaryUnit");
	pXML->GetOptionalChildXmlValByName(&m_bMilitaryFoodProduction, L"bMilitaryFoodProduction");
	pXML->GetOptionalChildXmlValByName(&m_iMaxConscript, L"iMaxConscript");
	pXML->GetOptionalChildXmlValByName(&m_bNoUnhealthyPopulation, L"bNoUnhealthyPopulation");
	pXML->GetOptionalChildXmlValByName(&m_bBuildingOnlyHealthy, L"bBuildingOnlyHealthy");
	pXML->GetOptionalChildXmlValByName(&m_iLargestCityHappiness, L"iLargestCityHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessModifier, L"iWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeSpecialist, L"iFreeSpecialist");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_bNoForeignTrade, L"bNoForeignTrade");
	pXML->GetOptionalChildXmlValByName(&m_bNoCorporations, L"bNoCorporations");
	pXML->GetOptionalChildXmlValByName(&m_bNoForeignCorporations, L"bNoForeignCorporations");
	pXML->GetOptionalChildXmlValByName(&m_iCivicPercentAnger, L"iCivicPercentAnger");
	pXML->GetOptionalChildXmlValByName(&m_bStateReligion, L"bStateReligion");
	pXML->GetOptionalChildXmlValByName(&m_bNoNonStateReligionSpread, L"bNoNonStateReligionSpread");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionHappiness, L"iStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iNonStateReligionHappiness, L"iNonStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionUnitProductionModifier, L"iStateReligionUnitProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionBuildingProductionModifier, L"iStateReligionBuildingProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionFreeExperience, L"iStateReligionFreeExperience");
	pXML->GetOptionalChildXmlValByName(&m_iExpInBorderModifier, L"iExpInBorderModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxLocal, L"iRevIdxLocal");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxNational, L"iRevIdxNational");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxDistanceModifier, L"iRevIdxDistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxHolyCityGood, L"iRevIdxHolyCityGood");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxHolyCityBad, L"iRevIdxHolyCityBad");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxSwitchTo, L"iRevIdxSwitchTo");
	pXML->GetOptionalChildXmlValByName(&m_iRevReligiousFreedom, L"iRevReligiousFreedom");
	pXML->GetOptionalChildXmlValByName(&m_iRevLaborFreedom, L"iRevLaborFreedom");
	pXML->GetOptionalChildXmlValByName(&m_iRevEnvironmentalProtection, L"iRevEnvironmentalProtection");
	pXML->GetOptionalChildXmlValByName(&m_iRevDemocracyLevel, L"iRevDemocracyLevel");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeShareMod, L"iAttitudeShareMod");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationgrowthratepercentage, L"iPopulationgrowthratepercentage");
	pXML->GetOptionalChildXmlValByName(&m_iReligionSpreadRate, L"iReligionSpreadRate");
	pXML->GetOptionalChildXmlValByName(&m_iCivicHappiness, L"iCivicHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iDistantUnitSupportCostModifier, L"iDistantUnitSupportCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iExtraCityDefense, L"iExtraCityDefense");
	pXML->GetOptionalChildXmlValByName(&m_iForeignTradeRouteModifier, L"iForeignTradeRouteModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTaxRateUnhappiness, L"iTaxRateUnhappiness");
	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflation");
	pXML->GetOptionalChildXmlValByName(&m_iHurryInflationModifier, L"iHurryInflationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHurryCostModifier, L"iHurryCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iSharedCivicTradeRouteModifier, L"iSharedCivicTradeRouteModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLandmarkHappiness, L"iLandmarkHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iLandmarkHappiness, L"iCorporationSpreadRate");
	pXML->GetOptionalChildXmlValByName(&m_iForeignerUnhappyPercent, L"iForeignerUnhappyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCityLimit, L"iCityLimit");
	pXML->GetOptionalChildXmlValByName(&m_iCityOverLimitUnhappy, L"iCityOverLimitUnhappy");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureProbabilityModifier, L"iNationalCaptureProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureResistanceModifier, L"iNationalCaptureResistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreedomFighterChange, L"iFreedomFighterChange");

	pXML->GetOptionalChildXmlValByName(&m_fRevIdxNationalityMod, L"fRevIdxNationalityMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxBadReligionMod, L"fRevIdxBadReligionMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxGoodReligionMod, L"fRevIdxGoodReligionMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevViolentMod, L"fRevViolentMod");

	pXML->GetOptionalChildXmlValByName(&m_bUpgradeAnywhere, L"bUpgradeAnywhere");
	pXML->GetOptionalChildXmlValByName(&m_bAllowInquisitions, L"bAllowInquisitions");
	pXML->GetOptionalChildXmlValByName(&m_bDisallowInquisitions, L"bDisallowInquisitions");
	pXML->GetOptionalChildXmlValByName(&m_bCommunism, L"bCommunism");
	pXML->GetOptionalChildXmlValByName(&m_bFreeSpeech, L"bFreeSpeech");
	pXML->GetOptionalChildXmlValByName(&m_bCanDoElection, L"bCanDoElection");
	pXML->GetOptionalChildXmlValByName(&m_bFixedBorders, L"bFixedBorders");
	pXML->GetOptionalChildXmlValByName(&m_bNoCapitalUnhappiness, L"bNoCapitalUnhappiness");
	pXML->GetOptionalChildXmlValByName(&m_bNoLandmarkAnger, L"bNoLandmarkAnger");
	pXML->GetOptionalChildXmlValByName(&m_bAllReligionsActive, L"bAllReligionsActive");
	pXML->GetOptionalChildXmlValByName(&m_bBansNonStateReligions, L"bBansNonStateReligions");
	pXML->GetOptionalChildXmlValByName(&m_bFreedomFighter, L"bFreedomFighter");
	pXML->GetOptionalChildXmlValByName(&m_bPolicy, L"bPolicy");


	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalYieldModifiers"))
	{
		pXML->SetYields(&m_piCapitalYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"TradeYieldModifiers"))
	{
		pXML->SetYields(&m_piTradeYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piTradeYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalCommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCapitalCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraCommerces"))
	{
		pXML->SetCommerce(&m_piSpecialistExtraCommerce);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	}

	pXML->SetVariableListTagPair(&m_pabHurry, L"Hurrys", GC.getNumHurryInfos());
	pXML->SetVariableListTagPair(&m_pabSpecialBuildingNotRequired, L"SpecialBuildingNotRequireds",  GC.getNumSpecialBuildingInfos());
	pXML->SetVariableListTagPair(&m_pabSpecialistValid, L"SpecialistValids", GC.getNumSpecialistInfos());
	pXML->SetVariableListTagPair(&m_paiBuildingHappinessChanges, L"BuildingHappinessChanges", GC.getNumBuildingInfos());
	pXML->SetVariableListTagPair(&m_paiBuildingHealthChanges, L"BuildingHealthChanges", GC.getNumBuildingInfos());
	pXML->SetVariableListTagPair(&m_paiFeatureHappinessChanges, L"FeatureHappinessChanges", GC.getNumFeatureInfos());

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	m_bAnyImprovementYieldChange = false;
	if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiImprovementYieldChanges, GC.getNumImprovementInfos(), NUM_YIELD_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiImprovementYieldChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"ImprovementYields"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiImprovementYieldChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
						else
						{
							SAFE_DELETE_ARRAY(m_ppiImprovementYieldChanges[iIndex]);
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				for(int ii=0;(!m_bAnyImprovementYieldChange) && ii<GC.getNumImprovementInfos();ii++)
				{
					for(int ij=0; ij < NUM_YIELD_TYPES; ij++ )
					{
						if( getImprovementYieldChanges(ii, ij) != 0 )
						{
							m_bAnyImprovementYieldChange = true;
							break;
						}
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(m_szWeLoveTheKingKey, L"WeLoveTheKing");

	pXML->SetVariableListTagPair(&m_piBonusMintedPercent, L"BonusMintedPercents", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piImprovementHappinessChanges, L"ImprovementHappinessChanges", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPair(&m_piImprovementHealthPercentChanges, L"ImprovementHealthPercentChanges", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPair(&m_paiUnitCombatProductionModifier, L"UnitCombatProductionModifiers", GC.getNumUnitCombatInfos());

	m_aBuildingProductionModifier.read(pXML, L"BuildingProductionModifiers");

	if (pXML->TryMoveToXmlFirstChild(L"UnitProductionModifiers"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = false;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i = 0; i < iNumSibs; i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszUnitProdModforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiUnitProdModforPass3.push_back(iTemp);
						pXML->MoveToXmlParent();
					}
					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
				pXML->MoveToXmlParent();
			}
		}
		pXML->MoveToXmlParent();
	}

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());
	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, L"FreeSpecialistCounts", GC.getNumSpecialistInfos());
	pXML->GetOptionalChildXmlValByName(&m_iEnslavementChance, L"iEnslavementChance");

	if (pXML->TryMoveToXmlFirstChild(L"LandmarkYieldChanges"))
	{
		pXML->SetYields(&m_piLandmarkYieldChanges);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piLandmarkYieldChanges);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldPercentChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos(), NUM_YIELD_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiSpecialistYieldPercentChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldPercents"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiSpecialistYieldPercentChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommercePercentChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiSpecialistCommercePercentChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommercePercents"))
						{
							// call the function that sets the commerce change variable
							pXML->SetCommerce(&m_ppiSpecialistCommercePercentChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"TerrainYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiTerrainYieldChanges, GC.getNumTerrainInfos(), NUM_YIELD_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiTerrainYieldChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"TerrainYields"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiTerrainYieldChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}
			else
			{
				SAFE_DELETE_ARRAY(m_ppiTerrainYieldChanges);
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"BuildingCommerceModifiers"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiBuildingCommerceModifier[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
						{
							// call the function that sets the yield change variable
							pXML->SetCommerce(&m_ppiBuildingCommerceModifier[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"BuildingCommerceChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiBuildingCommerceChange, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiBuildingCommerceChange[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
						{
							// call the function that sets the yield change variable
							pXML->SetCommerce(&m_ppiBuildingCommerceChange[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"BonusCommerceModifiers"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiBonusCommerceModifier, GC.getNumBonusInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"BonusType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiBonusCommerceModifier[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
						{
							// call the function that sets the yield change variable
							pXML->SetCommerce(&m_ppiBonusCommerceModifier[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"CivicAttitudeChanges"))
	{
			int iTemp = 0;
			CvString szTemp;
			int iNumSibs = pXML->GetXmlChildrenNumber();
			if (pXML->TryMoveToXmlFirstChild())
			{
				if (iNumSibs > 0)
				{
					for (int i=0;i<iNumSibs;i++)
					{
						if (pXML->GetChildXmlVal(szTextVal))
						{
							m_aszCivicAttitudeforPass3.push_back(szTextVal);
							pXML->GetNextXmlVal(&iTemp);
							m_aiCivicAttitudeforPass3.push_back(iTemp);

							m_aszCivicAttitudeReasonforPass3.push_back(szTextVal);
							pXML->GetNextXmlVal(szTemp);
							m_aszCivicAttitudeReasonValueforPass3.push_back(szTemp);

							pXML->MoveToXmlParent();
						}

						if (!pXML->TryMoveToXmlNextSibling())
						{
							break;
						}
					}

					pXML->MoveToXmlParent();
				}
			}

		pXML->MoveToXmlParent();
	}

	m_PropertyManipulators.read(pXML);
	return true;
}

bool CvCivicInfo::readPass3()
{
	PROFILE_EXTRA_FUNC();
	m_piCivicAttitudeChanges = new int[GC.getNumCivicInfos()];
	m_pszCivicAttitudeReason = new CvString[GC.getNumCivicInfos()];
	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		m_piCivicAttitudeChanges[iI] = 0;
		m_pszCivicAttitudeReason[iI] = "";
	}
	if (!m_aiCivicAttitudeforPass3.empty() && !m_aszCivicAttitudeforPass3.empty())
	{
		const int iNumLoad = m_aiCivicAttitudeforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			const int iTempIndex = GC.getInfoTypeForString(m_aszCivicAttitudeforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_piCivicAttitudeChanges[iTempIndex] = m_aiCivicAttitudeforPass3[iI];
		}
		m_aszCivicAttitudeforPass3.clear();
		m_aiCivicAttitudeforPass3.clear();
	}
	if (!m_aszCivicAttitudeReasonValueforPass3.empty() && !m_aszCivicAttitudeReasonforPass3.empty())
	{
		const int iNumLoad = m_aszCivicAttitudeReasonValueforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			const int iTempIndex = GC.getInfoTypeForString(m_aszCivicAttitudeReasonforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_pszCivicAttitudeReason[iTempIndex] = m_aszCivicAttitudeReasonValueforPass3[iI];
		}
		m_aszCivicAttitudeReasonforPass3.clear();
		m_aszCivicAttitudeReasonValueforPass3.clear();
	}

	m_piUnitProductionModifier = new int[GC.getNumUnitInfos()];
    for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		m_piUnitProductionModifier[iI] = 0;
	}
	if (!m_aiUnitProdModforPass3.empty() && !m_aszUnitProdModforPass3.empty())
	{
		int iNumLoad = m_aiUnitProdModforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszUnitProdModforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumUnitInfos())
			{
				m_piUnitProductionModifier[iTempIndex] = m_aiUnitProdModforPass3[iI];
			}
		}
		m_aszUnitProdModforPass3.clear();
		m_aiUnitProdModforPass3.clear();
	}

	return true;
}

void CvCivicInfo::copyNonDefaults(const CvCivicInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getCivicOptionType() == iTextDefault) m_iCivicOptionType = pClassInfo->getCivicOptionType();
	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getAnarchyLength() == iDefault) m_iAnarchyLength = pClassInfo->getAnarchyLength();
	if (getUpkeep() == iTextDefault) m_iUpkeep = pClassInfo->getUpkeep();

	if (getAIWeight() == iDefault) m_iAIWeight = pClassInfo->getAIWeight();
	if (getGreatPeopleRateModifier() == iDefault) m_iGreatPeopleRateModifier = pClassInfo->getGreatPeopleRateModifier();
	if (getGreatGeneralRateModifier() == iDefault) m_iGreatGeneralRateModifier = pClassInfo->getGreatGeneralRateModifier();
	if (getDomesticGreatGeneralRateModifier() == iDefault) m_iDomesticGreatGeneralRateModifier = pClassInfo->getDomesticGreatGeneralRateModifier();
	if (getStateReligionGreatPeopleRateModifier() == iDefault) m_iStateReligionGreatPeopleRateModifier = pClassInfo->getStateReligionGreatPeopleRateModifier();
	if (getDistanceMaintenanceModifier() == iDefault) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (getNumCitiesMaintenanceModifier() == iDefault) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (getCorporationMaintenanceModifier() == iDefault) m_iCorporationMaintenanceModifier = pClassInfo->getCorporationMaintenanceModifier();
	if (getExtraHealth() == iDefault) m_iExtraHealth = pClassInfo->getExtraHealth();
	if (getFreeExperience() == iDefault) m_iFreeExperience = pClassInfo->getFreeExperience();
	if (getWorkerSpeedModifier() == iDefault) m_iWorkerSpeedModifier = pClassInfo->getWorkerSpeedModifier();
	if (getImprovementUpgradeRateModifier() == iDefault) m_iImprovementUpgradeRateModifier = pClassInfo->getImprovementUpgradeRateModifier();
	if (getMilitaryProductionModifier() == iDefault) m_iMilitaryProductionModifier = pClassInfo->getMilitaryProductionModifier();
	if (m_iFreeUnitUpkeepCivilian == iDefault) m_iFreeUnitUpkeepCivilian = pClassInfo->getFreeUnitUpkeepCivilian();
	if (m_iFreeUnitUpkeepMilitary == iDefault) m_iFreeUnitUpkeepMilitary = pClassInfo->getFreeUnitUpkeepMilitary();
	if (m_iFreeUnitUpkeepCivilianPopPercent == iDefault) m_iFreeUnitUpkeepCivilianPopPercent = pClassInfo->getFreeUnitUpkeepCivilianPopPercent();
	if (m_iFreeUnitUpkeepMilitaryPopPercent == iDefault) m_iFreeUnitUpkeepMilitaryPopPercent = pClassInfo->getFreeUnitUpkeepMilitaryPopPercent();
	if (m_iCivilianUnitUpkeepMod == iDefault) m_iCivilianUnitUpkeepMod = pClassInfo->getCivilianUnitUpkeepMod();
	if (m_iMilitaryUnitUpkeepMod == iDefault) m_iMilitaryUnitUpkeepMod = pClassInfo->getMilitaryUnitUpkeepMod();
	if (getHappyPerMilitaryUnit() == iDefault) m_iHappyPerMilitaryUnit = pClassInfo->getHappyPerMilitaryUnit();
	if (isMilitaryFoodProduction() == bDefault) m_bMilitaryFoodProduction = pClassInfo->isMilitaryFoodProduction();
	if (getMaxConscript() == iDefault) m_iMaxConscript = pClassInfo->getMaxConscript();
	if (isNoUnhealthyPopulation() == bDefault) m_bNoUnhealthyPopulation = pClassInfo->isNoUnhealthyPopulation();
	if (isBuildingOnlyHealthy() == bDefault) m_bBuildingOnlyHealthy = pClassInfo->isBuildingOnlyHealthy();
	if (getLargestCityHappiness() == iDefault) m_iLargestCityHappiness = pClassInfo->getLargestCityHappiness();
	if (getWarWearinessModifier() == iDefault) m_iWarWearinessModifier = pClassInfo->getWarWearinessModifier();
	if (getFreeSpecialist() == iDefault) m_iFreeSpecialist = pClassInfo->getFreeSpecialist();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (isNoForeignTrade() == bDefault) m_bNoForeignTrade = pClassInfo->isNoForeignTrade();
	if (isNoCorporations() == bDefault) m_bNoCorporations = pClassInfo->isNoCorporations();
	if (isNoForeignCorporations() == bDefault) m_bNoForeignCorporations = pClassInfo->isNoForeignCorporations();
	if (getCivicPercentAnger() == iDefault) m_iCivicPercentAnger = pClassInfo->getCivicPercentAnger();
	if (isStateReligion() == bDefault) m_bStateReligion = pClassInfo->isStateReligion();
	if (isNoNonStateReligionSpread() == bDefault) m_bNoNonStateReligionSpread = pClassInfo->isNoNonStateReligionSpread();
	if (getStateReligionHappiness() == iDefault) m_iStateReligionHappiness = pClassInfo->getStateReligionHappiness();
	if (getNonStateReligionHappiness() == iDefault) m_iNonStateReligionHappiness = pClassInfo->getNonStateReligionHappiness();
	if (getStateReligionUnitProductionModifier() == iDefault) m_iStateReligionUnitProductionModifier = pClassInfo->getStateReligionUnitProductionModifier();
	if (getStateReligionBuildingProductionModifier() == iDefault) m_iStateReligionBuildingProductionModifier = pClassInfo->getStateReligionBuildingProductionModifier();
	if (getStateReligionFreeExperience() == iDefault) m_iStateReligionFreeExperience = pClassInfo->getStateReligionFreeExperience();
	if (getExpInBorderModifier() == iDefault) m_iExpInBorderModifier = pClassInfo->getExpInBorderModifier();
	if (isUpgradeAnywhere() == bDefault) m_bUpgradeAnywhere = pClassInfo->isUpgradeAnywhere();
	if (isAllowInquisitions() == bDefault) m_bAllowInquisitions = pClassInfo->isAllowInquisitions();
	if (isDisallowInquisitions() == bDefault) m_bDisallowInquisitions = pClassInfo->isDisallowInquisitions();
	if (getRevIdxLocal() == iDefault) m_iRevIdxLocal = pClassInfo->getRevIdxLocal();
	if (getRevIdxNational() == iDefault) m_iRevIdxNational = pClassInfo->getRevIdxNational();
	if (getRevIdxDistanceModifier() == iDefault) m_iRevIdxDistanceModifier = pClassInfo->getRevIdxDistanceModifier();
	if (getRevIdxHolyCityGood() == iDefault) m_iRevIdxHolyCityGood = pClassInfo->getRevIdxHolyCityGood();
	if (getRevIdxHolyCityBad() == iDefault) m_iRevIdxHolyCityBad = pClassInfo->getRevIdxHolyCityBad();
	if (getRevIdxSwitchTo() == iDefault) m_iRevIdxSwitchTo = pClassInfo->getRevIdxSwitchTo();
	if (getRevIdxNationalityMod() == fDefault) m_fRevIdxNationalityMod = pClassInfo->getRevIdxNationalityMod();
	if (getRevIdxBadReligionMod() == fDefault) m_fRevIdxBadReligionMod = pClassInfo->getRevIdxBadReligionMod();
	if (getRevIdxGoodReligionMod() == fDefault) m_fRevIdxGoodReligionMod = pClassInfo->getRevIdxGoodReligionMod();
	if (getRevViolentMod() == fDefault) m_fRevViolentMod = pClassInfo->getRevViolentMod();
	if (getRevReligiousFreedom() == iDefault) m_iRevReligiousFreedom = pClassInfo->getRevReligiousFreedom();
	if (getRevLaborFreedom() == iDefault) m_iRevLaborFreedom = pClassInfo->getRevLaborFreedom();
	if (getRevEnvironmentalProtection() == iDefault) m_iRevEnvironmentalProtection = pClassInfo->getRevEnvironmentalProtection();
	if (getRevDemocracyLevel() == iDefault) m_iRevDemocracyLevel = pClassInfo->getRevDemocracyLevel();
	if (getFreedomFighterChange() == iDefault) m_iFreedomFighterChange = pClassInfo->getFreedomFighterChange();
	if (isCommunism() == bDefault) m_bCommunism = pClassInfo->isCommunism();
	if (isFreeSpeech() == bDefault) m_bFreeSpeech = pClassInfo->isFreeSpeech();
	if (isCanDoElection() == bDefault) m_bCanDoElection = pClassInfo->isCanDoElection();
	if (isFreedomFighter() == bDefault) m_bFreedomFighter = pClassInfo->isFreedomFighter();
	if (isPolicy() == bDefault) m_bPolicy = pClassInfo->isPolicy();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getYieldModifier(i) == iDefault && pClassInfo->getYieldModifier(i) != iDefault)
		{
			if ( NULL == m_piYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piYieldModifier[i] = pClassInfo->getYieldModifier(i);
		}
		if ( getCapitalYieldModifier(i) == iDefault && pClassInfo->getCapitalYieldModifier(i) != iDefault)
		{
			if ( NULL == m_piCapitalYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCapitalYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piCapitalYieldModifier[i] = pClassInfo->getCapitalYieldModifier(i);
		}
		if ( getTradeYieldModifier(i) == iDefault && pClassInfo->getTradeYieldModifier(i) != iDefault)
		{
			if ( NULL == m_piTradeYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piTradeYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piTradeYieldModifier[i] = pClassInfo->getTradeYieldModifier(i);
		}
	}

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( getCommerceModifier(i) == iDefault && pClassInfo->getCommerceModifier(i) != iDefault)
		{
			if ( NULL == m_piCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceModifier[i] = pClassInfo->getCommerceModifier(i);
		}
		if ( getCapitalCommerceModifier(i) == iDefault && pClassInfo->getCapitalCommerceModifier(i) != iDefault)
		{
			if ( NULL == m_piCapitalCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCapitalCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCapitalCommerceModifier[i] = pClassInfo->getCapitalCommerceModifier(i);
		}
		if ( getSpecialistExtraCommerce(i) == iDefault && pClassInfo->getSpecialistExtraCommerce(i) != iDefault)
		{
			if ( NULL == m_piSpecialistExtraCommerce )
			{
				CvXMLLoadUtility::InitList(&m_piSpecialistExtraCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piSpecialistExtraCommerce[i] = pClassInfo->getSpecialistExtraCommerce(i);
		}
	}

	for ( int i = 0; i < GC.getNumHurryInfos(); i++ )
	{
		if ( isHurry(i) == bDefault && pClassInfo->isHurry(i) != bDefault)
		{
			if ( NULL == m_pabHurry )
			{
				CvXMLLoadUtility::InitList(&m_pabHurry,GC.getNumHurryInfos(),bDefault);
			}
			m_pabHurry[i] = pClassInfo->isHurry(i);
		}
	}
	for ( int i = 0; i < GC.getNumSpecialBuildingInfos(); i++ )
	{
		if ( isSpecialBuildingNotRequired(i) == bDefault && pClassInfo->isSpecialBuildingNotRequired(i) != bDefault)
		{
			if ( NULL == m_pabSpecialBuildingNotRequired)
			{
				CvXMLLoadUtility::InitList(&m_pabSpecialBuildingNotRequired,GC.getNumSpecialBuildingInfos(),bDefault);
			}
			m_pabSpecialBuildingNotRequired[i] = pClassInfo->isSpecialBuildingNotRequired(i);
		}
	}
	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++ )
	{
		if ( isSpecialistValid(i) == bDefault && pClassInfo->isSpecialistValid(i) != bDefault)
		{
			if ( NULL == m_pabSpecialistValid )
			{
				CvXMLLoadUtility::InitList(&m_pabSpecialistValid,GC.getNumSpecialistInfos(),bDefault);
			}
			m_pabSpecialistValid[i] = pClassInfo->isSpecialistValid(i);
		}
	}

	for ( int i = 0; i < GC.getNumBuildingInfos(); i++ )
	{
		if ( getBuildingHappinessChanges(i) == iDefault && pClassInfo->getBuildingHappinessChanges(i) != iDefault)
		{
			if ( NULL == m_paiBuildingHappinessChanges )
			{
				CvXMLLoadUtility::InitList(&m_paiBuildingHappinessChanges,GC.getNumBuildingInfos(),iDefault);
			}
			m_paiBuildingHappinessChanges[i] = pClassInfo->getBuildingHappinessChanges(i);
		}
		if ( getBuildingHealthChanges(i) == iDefault && pClassInfo->getBuildingHealthChanges(i) != iDefault)
		{
			if ( NULL == m_paiBuildingHealthChanges )
			{
				CvXMLLoadUtility::InitList(&m_paiBuildingHealthChanges,GC.getNumBuildingInfos(),iDefault);
			}
			m_paiBuildingHealthChanges[i] = pClassInfo->getBuildingHealthChanges(i);
		}
	}
	for ( int i = 0; i < GC.getNumFeatureInfos(); i++ )
	{
		if ( getFeatureHappinessChanges(i) == iDefault && pClassInfo->getFeatureHappinessChanges(i) != iDefault)
		{
			if ( NULL == m_paiFeatureHappinessChanges )
			{
				CvXMLLoadUtility::InitList(&m_paiFeatureHappinessChanges,GC.getNumFeatureInfos(),iDefault);
			}
			m_paiFeatureHappinessChanges[i] = pClassInfo->getFeatureHappinessChanges(i);
		}
	}

	for ( int i = 0; i < GC.getNumImprovementInfos(); i++ )
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			if ( getImprovementYieldChanges(i, j) == iDefault && pClassInfo->getImprovementYieldChanges(i,j) != iDefault)
			{
				if ( NULL == m_ppiImprovementYieldChanges )
				{
					CvXMLLoadUtility::Init2DList(&m_ppiImprovementYieldChanges, GC.getNumImprovementInfos(), NUM_YIELD_TYPES);
				}
				else if ( NULL == m_ppiImprovementYieldChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiImprovementYieldChanges[i],NUM_YIELD_TYPES,iDefault);
				}
				m_ppiImprovementYieldChanges[i][j] = pClassInfo->getImprovementYieldChanges(i,j);
			}
		}
	}

	if (getWeLoveTheKingKey() == wDefault)
	{
		m_szWeLoveTheKingKey = pClassInfo->getWeLoveTheKingKey();
	}

	if (getAttitudeShareMod() == iDefault) m_iAttitudeShareMod = pClassInfo->getAttitudeShareMod();
	if (getEnslavementChance() == iDefault) m_iEnslavementChance = pClassInfo->getEnslavementChance();
	if (getForeignTradeRouteModifier() == iDefault) m_iForeignTradeRouteModifier = pClassInfo->getForeignTradeRouteModifier();
	if (getPopulationgrowthratepercentage() == iDefault) m_iPopulationgrowthratepercentage = pClassInfo->getPopulationgrowthratepercentage();
	if (getReligionSpreadRate() == iDefault) m_iReligionSpreadRate = pClassInfo->getReligionSpreadRate();
	if (getCivicHappiness() == iDefault) m_iCivicHappiness = pClassInfo->getCivicHappiness();
	if (getDistantUnitSupportCostModifier() == iDefault) m_iDistantUnitSupportCostModifier = pClassInfo->getDistantUnitSupportCostModifier();
	if (getExtraCityDefense() == iDefault) m_iExtraCityDefense = pClassInfo->getExtraCityDefense();
	if (getTaxRateUnhappiness() == iDefault) m_iTaxRateUnhappiness = pClassInfo->getTaxRateUnhappiness();
	if (IsFixedBorders() == bDefault) m_bFixedBorders = pClassInfo->IsFixedBorders();
	if (isNoCapitalUnhappiness() == bDefault) m_bNoCapitalUnhappiness = pClassInfo->isNoCapitalUnhappiness();
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();
	if (getHurryInflationModifier() == iDefault) m_iHurryInflationModifier = pClassInfo->getHurryInflationModifier();
	if (getHurryCostModifier() == iDefault) m_iHurryCostModifier = pClassInfo->getHurryCostModifier();
	if (getSharedCivicTradeRouteModifier() == iDefault) m_iSharedCivicTradeRouteModifier = pClassInfo->getSharedCivicTradeRouteModifier();
	if (getLandmarkHappiness() == iDefault) m_iLandmarkHappiness = pClassInfo->getLandmarkHappiness();
	if (getCorporationSpreadRate() == iDefault) m_iCorporationSpreadRate = pClassInfo->getCorporationSpreadRate();
	if (isNoLandmarkAnger() == bDefault) m_bNoLandmarkAnger = pClassInfo->isNoLandmarkAnger();
	if (getCityLimit(NO_PLAYER) == iDefault) m_iCityLimit = pClassInfo->getCityLimit(NO_PLAYER);
	if (getCityOverLimitUnhappy() == iDefault) m_iCityOverLimitUnhappy = pClassInfo->getCityOverLimitUnhappy();
	if (getForeignerUnhappyPercent() == iDefault) m_iForeignerUnhappyPercent = pClassInfo->getForeignerUnhappyPercent();

	if (!m_iHomeAreaMaintenanceModifier) m_iHomeAreaMaintenanceModifier = pClassInfo->getHomeAreaMaintenanceModifier();
	if (!m_iOtherAreaMaintenanceModifier) m_iOtherAreaMaintenanceModifier = pClassInfo->getOtherAreaMaintenanceModifier();

	for ( int i = 0; i < GC.getNumImprovementInfos(); i++ )
	{
		if ( getImprovementHealthPercentChanges(i) == iDefault && pClassInfo->getImprovementHealthPercentChanges(i) != iDefault)
		{
			if ( NULL == m_piImprovementHealthPercentChanges )
			{
				CvXMLLoadUtility::InitList(&m_piImprovementHealthPercentChanges,GC.getNumImprovementInfos(),iDefault);
			}
			m_piImprovementHealthPercentChanges[i] = pClassInfo->getImprovementHealthPercentChanges(i);
		}

		if ( getImprovementHappinessChanges(i) == iDefault && pClassInfo->getImprovementHappinessChanges(i) != iDefault)
		{
			if ( NULL == m_piImprovementHappinessChanges )
			{
				CvXMLLoadUtility::InitList(&m_piImprovementHappinessChanges,GC.getNumImprovementInfos(),iDefault);
			}
			m_piImprovementHappinessChanges[i] = pClassInfo->getImprovementHappinessChanges(i);
		}
	}


	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getLandmarkYieldChanges(i) == iDefault && pClassInfo->getLandmarkYieldChanges(i) != iDefault)
		{
			if ( NULL == m_piLandmarkYieldChanges )
			{
				CvXMLLoadUtility::InitList(&m_piLandmarkYieldChanges,NUM_YIELD_TYPES,iDefault);
			}
			m_piLandmarkYieldChanges[i] = pClassInfo->getLandmarkYieldChanges(i);
		}
	}

	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++ )
	{
		if ( getFreeSpecialistCount(i) == iDefault && pClassInfo->getFreeSpecialistCount(i) != iDefault)
		{
			if ( NULL == m_piFreeSpecialistCount)
			{
				CvXMLLoadUtility::InitList(&m_piFreeSpecialistCount,GC.getNumSpecialistInfos(),iDefault);
			}
			m_piFreeSpecialistCount[i] = pClassInfo->getFreeSpecialistCount(i);
		}

		for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			if ( getSpecialistYieldPercentChanges(i,j) == iDefault && pClassInfo->getSpecialistYieldPercentChanges(i,j) != iDefault)
			{
				if ( NULL == m_ppiSpecialistYieldPercentChanges )
				{
					CvXMLLoadUtility::Init2DList(&m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos(), NUM_YIELD_TYPES);
				}
				else if ( NULL == m_ppiSpecialistYieldPercentChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiSpecialistYieldPercentChanges[i],NUM_YIELD_TYPES,iDefault);
				}
				m_ppiSpecialistYieldPercentChanges[i][j] = pClassInfo->getSpecialistYieldPercentChanges(i,j);
			}
		}

		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getSpecialistCommercePercentChanges(i,j) == iDefault && pClassInfo->getSpecialistCommercePercentChanges(i,j) != iDefault)
			{
				if ( NULL == m_ppiSpecialistCommercePercentChanges )
				{
					CvXMLLoadUtility::Init2DList(&m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiSpecialistCommercePercentChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiSpecialistCommercePercentChanges[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiSpecialistCommercePercentChanges[i][j] = pClassInfo->getSpecialistCommercePercentChanges(i,j);
			}
		}
	}

	for ( int i = 0; i < GC.getNumUnitCombatInfos(); i++ )
	{
		if ( getUnitCombatProductionModifier(i) == iDefault && pClassInfo->getUnitCombatProductionModifier(i) != iDefault)
		{
			if ( NULL == m_paiUnitCombatProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_paiUnitCombatProductionModifier,GC.getNumUnitCombatInfos(),iDefault);
			}
			m_paiUnitCombatProductionModifier[i] = pClassInfo->getUnitCombatProductionModifier(i);
		}
	}

	m_aBuildingProductionModifier.copyNonDefaults(pClassInfo->getBuildingProductionModifiers());

	for ( int i = 0; i < GC.getNumBuildingInfos(); i++ )
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getBuildingCommerceChange(i,j) == iDefault && pClassInfo->getBuildingCommerceChange(i,j) != iDefault)
			{
				if ( NULL == m_ppiBuildingCommerceChange )
				{
					CvXMLLoadUtility::Init2DList(&m_ppiBuildingCommerceChange, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiBuildingCommerceChange[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiBuildingCommerceChange[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiBuildingCommerceChange[i][j] = pClassInfo->getBuildingCommerceChange(i,j);
			}
		}
	}

	for ( int i = 0; i < GC.getNumTerrainInfos(); i++ )
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			if ( getTerrainYieldChanges(i,j) == iDefault && pClassInfo->getTerrainYieldChanges(i,j) != iDefault)
			{
				if ( NULL == m_ppiTerrainYieldChanges)
				{
					CvXMLLoadUtility::Init2DList(&m_ppiTerrainYieldChanges, GC.getNumTerrainInfos(), NUM_YIELD_TYPES);
				}
				else if ( NULL == m_ppiTerrainYieldChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiTerrainYieldChanges[i],NUM_YIELD_TYPES,iDefault);
				}
				m_ppiTerrainYieldChanges[i][j] = pClassInfo->getTerrainYieldChanges(i,j);
			}
		}
	}
	for ( int i = 0; i < GC.getNumBuildingInfos(); i++ )
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getBuildingCommerceModifier(i,j) == iDefault && pClassInfo->getBuildingCommerceModifier(i,j) != iDefault)
			{
				if ( NULL == m_ppiBuildingCommerceModifier)
				{
					CvXMLLoadUtility::Init2DList(&m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiBuildingCommerceModifier[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiBuildingCommerceModifier[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiBuildingCommerceModifier[i][j] = pClassInfo->getBuildingCommerceModifier(i,j);
			}
		}
	}
	for ( int i = 0; i < GC.getNumBonusInfos(); i++ )
	{
		if (getBonusMintedPercent(i) == iDefault && pClassInfo->getBonusMintedPercent(i) != iDefault)
		{
			if ( NULL == m_piBonusMintedPercent )
			{
				CvXMLLoadUtility::InitList(&m_piBonusMintedPercent,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusMintedPercent[i] = pClassInfo->getBonusMintedPercent(i);
		}
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getBonusCommerceModifier(i,j) == iDefault && pClassInfo->getBonusCommerceModifier(i,j) != iDefault)
			{
				if ( NULL == m_ppiBonusCommerceModifier )
				{
					CvXMLLoadUtility::Init2DList(&m_ppiBonusCommerceModifier, GC.getNumBonusInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiBonusCommerceModifier[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiBonusCommerceModifier[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiBonusCommerceModifier[i][j] = pClassInfo->getBonusCommerceModifier(i,j);
			}
		}
	}
	for ( int i = 0; i < pClassInfo->getCivicAttitudeVectorSize(); i++ )
	{
		m_aiCivicAttitudeforPass3.push_back(pClassInfo->getCivicAttitudeValuesVectorElement(i));
		m_aszCivicAttitudeforPass3.push_back(pClassInfo->getCivicAttitudeNamesVectorElement(i));
	}

	//TB Civic Tags
	//Integers
	if (getNationalCaptureProbabilityModifier() == iDefault) m_iNationalCaptureProbabilityModifier = pClassInfo->getNationalCaptureProbabilityModifier();
	if (getNationalCaptureResistanceModifier() == iDefault) m_iNationalCaptureResistanceModifier = pClassInfo->getNationalCaptureResistanceModifier();

	//Booleans
	if (isAllReligionsActive() == bDefault) m_bAllReligionsActive = pClassInfo->isAllReligionsActive();
	if (isBansNonStateReligions() == bDefault) m_bBansNonStateReligions = pClassInfo->isBansNonStateReligions();

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

//======================================================================================================
//					CvSpecialBuildingInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialBuildingInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialBuildingInfo::CvSpecialBuildingInfo() :
	m_iObsoleteTech(NO_TECH),
	m_iTechPrereq(NO_TECH),
	m_iTechPrereqAnyone(NO_TECH),
	m_iMaxPlayerInstances(-1),
	m_bValid(false)
{}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialBuildingInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialBuildingInfo::~CvSpecialBuildingInfo() { }

TechTypes CvSpecialBuildingInfo::getObsoleteTech() const
{
	return m_iObsoleteTech;
}

int CvSpecialBuildingInfo::getTechPrereqAnyone() const
{
	return m_iTechPrereqAnyone;
}

int CvSpecialBuildingInfo::getMaxPlayerInstances() const
{
	return m_iMaxPlayerInstances;
}

bool CvSpecialBuildingInfo::isValid() const
{
	return m_bValid;
}

bool CvSpecialBuildingInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = static_cast<TechTypes>(pXML->GetInfoClass(szTextVal));

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereqAnyone");
	m_iTechPrereqAnyone = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iMaxPlayerInstances, L"iMaxPlayerInstances", -1);

	pXML->GetOptionalChildXmlValByName(&m_bValid, L"bValid");

	return true;
}

void CvSpecialBuildingInfo::copyNonDefaults(const CvSpecialBuildingInfo* pClassInfo)
{
	CvInfoBase::copyNonDefaults(pClassInfo);

	const bool bDefault = false;
	const int iDefault = 0;

	if (getObsoleteTech() == iDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getTechPrereq() == iDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getTechPrereqAnyone() == iDefault) m_iTechPrereqAnyone = pClassInfo->getTechPrereqAnyone();
	if (getMaxPlayerInstances() == -1) m_iMaxPlayerInstances = pClassInfo->getMaxPlayerInstances();

	if (isValid() == bDefault) m_bValid = pClassInfo->isValid();
}

void CvSpecialBuildingInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iTechPrereqAnyone);
	CheckSum(iSum, m_iMaxPlayerInstances);

	CheckSum(iSum, m_bValid);
}

//======================================================================================================
//					CvRiverModelInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRiverModelInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRiverModelInfo::CvRiverModelInfo() :
m_iTextureIndex(0)
{
	m_szDeltaString[0] = '\0';
	m_szConnectString[0] = '\0';
	m_szRotateString[0] = '\0';
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRiverModelInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRiverModelInfo::~CvRiverModelInfo()
{
}

const char* CvRiverModelInfo::getModelFile() const
{
	return m_szModelFile;
}

const char* CvRiverModelInfo::getBorderFile() const
{
	return m_szBorderFile;
}

int CvRiverModelInfo::getTextureIndex() const
{
	return m_iTextureIndex;
}

const char* CvRiverModelInfo::getDeltaString() const
{
	return m_szDeltaString;
}

const char* CvRiverModelInfo::getConnectString() const
{
	return m_szConnectString;
}

const char* CvRiverModelInfo::getRotateString() const
{
	return m_szRotateString;
}

bool CvRiverModelInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szModelFile, L"ModelFile");
	pXML->GetOptionalChildXmlValByName(m_szBorderFile, L"BorderFile");
	pXML->GetOptionalChildXmlValByName(&m_iTextureIndex, L"TextureIndex");
	pXML->GetOptionalChildXmlValByName(m_szDeltaString, L"DeltaType");
	pXML->GetOptionalChildXmlValByName(m_szConnectString, L"Connections");
	pXML->GetOptionalChildXmlValByName(m_szRotateString, L"Rotations");

	return true;
}
void CvRiverModelInfo::copyNonDefaults(const CvRiverModelInfo* pClassInfo)
{
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getModelFile() == cDefault) m_szModelFile = pClassInfo->getModelFile();
	if (getBorderFile() == cDefault) m_szBorderFile = pClassInfo->getBorderFile();

	if (getTextureIndex() == iDefault) m_iTextureIndex = pClassInfo->getTextureIndex();

	if (getDeltaString() == cDefault) strcpy(m_szDeltaString, (const char*) CvString::format("%s", pClassInfo->getDeltaString()));
	if (getConnectString() == cDefault) strcpy(m_szConnectString, (const char*) CvString::format("%s", pClassInfo->getConnectString()));
	if (getRotateString() == cDefault) strcpy(m_szRotateString, (const char*) CvString::format("%s", pClassInfo->getRotateString()));
}

//======================================================================================================
//					CvRouteModelInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRouteModelInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRouteModelInfo::CvRouteModelInfo() :
m_eRouteType(NO_ROUTE),
m_bAnimated(false)
{
	m_szConnectString[0] = '\0';
	m_szModelConnectString[0] = '\0';
	m_szRotateString[0] = '\0';
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRouteModelInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRouteModelInfo::~CvRouteModelInfo()
{
}

RouteTypes CvRouteModelInfo::getRouteType() const		// The route type
{
	return m_eRouteType;
}

const char* CvRouteModelInfo::getModelFile() const
{
	return m_szModelFile;
}

const char* CvRouteModelInfo::getLateModelFile() const
{
	return m_szLateModelFile;
}

const char* CvRouteModelInfo::getModelFileKey() const
{
	return m_szModelFileKey;
}

bool CvRouteModelInfo::isAnimated() const
{
	return m_bAnimated;
}

const char* CvRouteModelInfo::getConnectString() const
{
	return m_szConnectString;
}

const char* CvRouteModelInfo::getModelConnectString() const
{
	return m_szModelConnectString;
}

const char* CvRouteModelInfo::getRotateString() const
{
	return m_szRotateString;
}

bool CvRouteModelInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szModelFile, L"ModelFile");
	pXML->GetChildXmlValByName(m_szLateModelFile, L"LateModelFile");
	pXML->GetChildXmlValByName(m_szModelFileKey, L"ModelFileKey");
	pXML->GetChildXmlValByName(&m_bAnimated, L"Animated");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"RouteType");
	m_eRouteType = (RouteTypes)(pXML->GetInfoClass(szTextVal));
	pXML->GetOptionalChildXmlValByName(m_szConnectString, L"Connections");
	pXML->GetOptionalChildXmlValByName(m_szModelConnectString, L"ModelConnections");
	pXML->GetOptionalChildXmlValByName(m_szRotateString, L"Rotations");

	return true;
}

void CvRouteModelInfo::copyNonDefaults(const CvRouteModelInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getModelFile() == cDefault) m_szModelFile = pClassInfo->getModelFile();
	if (getLateModelFile() == cDefault) m_szLateModelFile = pClassInfo->getLateModelFile();
	if (getModelFileKey() == cDefault) m_szModelFileKey = pClassInfo->getModelFileKey();

	if (isAnimated() == bDefault) m_bAnimated = pClassInfo->isAnimated();

	if (getRouteType() == iTextDefault) m_eRouteType = pClassInfo->getRouteType();

	if (getConnectString() == cDefault) strcpy(m_szConnectString, (const char*) CvString::format("%s", pClassInfo->getConnectString()));
	if (getModelConnectString() == cDefault) strcpy(m_szModelConnectString, (const char*) CvString::format("%s", pClassInfo->getModelConnectString()));
	if (getRotateString() == cDefault) strcpy(m_szRotateString, (const char*) CvString::format("%s", pClassInfo->getRotateString()));
}

//======================================================================================================
//					CvCivilizationInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivilizationInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivilizationInfo::CvCivilizationInfo():
m_iDefaultPlayerColor(NO_PLAYERCOLOR),
m_iArtStyleType(NO_ARTSTYLE),
m_iUnitArtStyleType(NO_UNIT_ARTSTYLE),
m_iNumCityNames(0),
m_iNumLeaders(0),
m_iSelectionSoundScriptId(0),
m_iActionSoundScriptId(0),
m_iDerivativeCiv(NO_CIVILIZATION),
m_bPlayable(false),
m_bAIPlayable(false),
m_piCivilizationInitialCivics(NULL),
m_pbLeaders(NULL),
m_pbCivilizationFreeTechs(NULL),
m_pbCivilizationDisableTechs(NULL),
m_paszCityNames(NULL),
m_iSpawnRateModifier(0),
m_iSpawnRateNPCPeaceModifier(0),
m_bStronglyRestricted(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivilizationInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivilizationInfo::~CvCivilizationInfo()
{
	SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
	SAFE_DELETE_ARRAY(m_pbLeaders);
	SAFE_DELETE_ARRAY(m_pbCivilizationFreeTechs);
	SAFE_DELETE_ARRAY(m_pbCivilizationDisableTechs);
	SAFE_DELETE_ARRAY(m_paszCityNames);
	GC.removeDelayedResolution((int*)&m_iDerivativeCiv);
}

void CvCivilizationInfo::reset()
{
	CvInfoBase::reset();
	m_aszAdjective.clear();
	m_aszShortDescription.clear();
}


int CvCivilizationInfo::getDefaultPlayerColor() const
{
	return m_iDefaultPlayerColor;
}

int CvCivilizationInfo::getArtStyleType() const
{
	return m_iArtStyleType;
}

int CvCivilizationInfo::getUnitArtStyleType() const
{
	return m_iUnitArtStyleType;
}

int CvCivilizationInfo::getNumCityNames() const
{
	return m_iNumCityNames;
}

int CvCivilizationInfo::getNumLeaders() const// the number of leaders the Civ has, this is needed so that random leaders can be generated easily
{
	return m_iNumLeaders;
}

int CvCivilizationInfo::getSelectionSoundScriptId() const
{
	return m_iSelectionSoundScriptId;
}

int CvCivilizationInfo::getActionSoundScriptId() const
{
	return m_iActionSoundScriptId;
}

bool CvCivilizationInfo::isAIPlayable() const
{
	return m_bAIPlayable;
}

bool CvCivilizationInfo::isPlayable() const
{
	return m_bPlayable;
}

const wchar_t* CvCivilizationInfo::getShortDescription(uint uiForm)
{
	PROFILE_EXTRA_FUNC();
	while(m_aszShortDescription.size() <= uiForm)
	{
		m_aszShortDescription.push_back(gDLL->getObjectText(m_szShortDescriptionKey, m_aszShortDescription.size()));
	}

	return m_aszShortDescription[uiForm];
}

const wchar_t* CvCivilizationInfo::getShortDescriptionKey() const
{
	return m_szShortDescriptionKey;
}

const wchar_t* CvCivilizationInfo::getAdjective(uint uiForm)
{
	PROFILE_EXTRA_FUNC();
	while(m_aszAdjective.size() <= uiForm)
	{
		m_aszAdjective.push_back(gDLL->getObjectText(m_szAdjectiveKey, m_aszAdjective.size()));
	}

	return m_aszAdjective[uiForm];
}

const wchar_t* CvCivilizationInfo::getAdjectiveKey() const
{
	return m_szAdjectiveKey;
}

const char* CvCivilizationInfo::getFlagTexture() const
{
	return ARTFILEMGR.getCivilizationArtInfo( getArtDefineTag() )->getPath();
}

const char* CvCivilizationInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}


int CvCivilizationInfo::getCivilizationInitialCivics(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), i);
	return (m_piCivilizationInitialCivics ? m_piCivilizationInitialCivics[i] : -1);
}

void CvCivilizationInfo::setCivilizationInitialCivics(int iCivicOption, int iCivic)
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), iCivicOption);
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), iCivic);

	if ( NULL == m_piCivilizationInitialCivics )
	{
		CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics,GC.getNumCivicOptionInfos(),-1);
	}

	m_piCivilizationInitialCivics[iCivicOption] = iCivic;
}

bool CvCivilizationInfo::isLeaders(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumLeaderHeadInfos(), i);
	return m_pbLeaders ? m_pbLeaders[i] : false;
}

int CvCivilizationInfo::getNumCivilizationBuildings() const
{
	return (int)m_aiCivilizationBuildings.size();
}
int CvCivilizationInfo::getCivilizationBuilding(int i) const
{
	return m_aiCivilizationBuildings[i];
}
bool CvCivilizationInfo::isCivilizationBuilding(int i) const
{
	return algo::any_of_equal(m_aiCivilizationBuildings, i);
}

bool CvCivilizationInfo::isCivilizationFreeTechs(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), i);
	return m_pbCivilizationFreeTechs ? m_pbCivilizationFreeTechs[i] : false;
}

bool CvCivilizationInfo::isCivilizationDisableTechs(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), i);
	return m_pbCivilizationDisableTechs ? m_pbCivilizationDisableTechs[i] : false;
}

const CvArtInfoCivilization* CvCivilizationInfo::getArtInfo() const
{
	return ARTFILEMGR.getCivilizationArtInfo(getArtDefineTag());
}

const char* CvCivilizationInfo::getButton() const
{
	const CvArtInfoCivilization* pArtInfoCivilization = getArtInfo();
	return pArtInfoCivilization ? pArtInfoCivilization->getButton() : NULL;
}

std::string CvCivilizationInfo::getCityNames(int i) const
{
	FASSERT_BOUNDS(0, getNumCityNames(), i);
	return m_paszCityNames[i];
}

//TB Tags

int CvCivilizationInfo::getSpawnRateModifier() const
{
	return m_iSpawnRateModifier;
}

int CvCivilizationInfo::getSpawnRateNPCPeaceModifier() const
{
	return m_iSpawnRateNPCPeaceModifier;
}

bool CvCivilizationInfo::isStronglyRestricted() const
{
	return m_bStronglyRestricted;
}

void CvCivilizationInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iDerivativeCiv);
	CheckSum(iSum, m_bAIPlayable);
	CheckSum(iSum, m_bPlayable);
	CheckSum(iSum, m_iSpawnRateModifier);
	CheckSum(iSum, m_iSpawnRateNPCPeaceModifier);
	CheckSum(iSum, m_bStronglyRestricted);
	CheckSumI(iSum, GC.getNumCivicOptionInfos(), m_piCivilizationInitialCivics);
	CheckSumI(iSum, GC.getNumLeaderHeadInfos(), m_pbLeaders);
	CheckSumI(iSum, GC.getNumTechInfos(), m_pbCivilizationFreeTechs);
	CheckSumI(iSum, GC.getNumTechInfos(), m_pbCivilizationDisableTechs);
	CheckSumC(iSum, m_aiCivilizationBuildings);
}

bool CvCivilizationInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szShortDescriptionKey, L"ShortDescription");
	pXML->GetOptionalChildXmlValByName(m_szAdjectiveKey, L"Adjective");
	pXML->GetOptionalTypeEnum(m_iDefaultPlayerColor, L"DefaultPlayerColor");
	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");
	pXML->GetOptionalTypeEnum(m_iArtStyleType, L"ArtStyleType");
	pXML->GetOptionalTypeEnum(m_iUnitArtStyleType, L"UnitArtStyleType");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationSelectionSound");
	m_iSelectionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationActionSound");
	m_iActionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;

	// set the current xml node to it's next sibling and then
	pXML->GetOptionalChildXmlValByName(&m_bPlayable, L"bPlayable");
	pXML->GetOptionalChildXmlValByName(&m_bAIPlayable, L"bAIPlayable");

	if (pXML->TryMoveToXmlFirstChild(L"Cities"))
	{
		pXML->SetStringList(&m_paszCityNames, &m_iNumCityNames);
		pXML->MoveToXmlParent();
	}

	pXML->SetOptionalVector(&m_aiCivilizationBuildings, L"FreeBuildings");

	pXML->SetVariableListTagPair(&m_pbCivilizationFreeTechs, L"FreeTechs", GC.getNumTechInfos());
	pXML->SetVariableListTagPair(&m_pbCivilizationDisableTechs, L"DisableTechs", GC.getNumTechInfos());

	if (pXML->TryMoveToXmlFirstChild(L"InitialCivics"))
	{
		if (const int iNumSibs = pXML->GetXmlChildrenNumber())
		{
			CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics, GC.getNumCivicOptionInfos());
			if (pXML->GetChildXmlVal(szTextVal))
			{
				for (int j = 0; j < iNumSibs; j++)
				{
					const CivicTypes eCivic = (CivicTypes)pXML->GetInfoClass(szTextVal);//, true);
					if ( eCivic != NO_CIVIC )
					{
						const CivicOptionTypes eCivicOption = (CivicOptionTypes)GC.getCivicInfo(eCivic).getCivicOptionType();

						if ( eCivicOption != NO_CIVICOPTION )
						{
							FAssertMsg((eCivicOption < GC.getNumCivicOptionInfos()),"Bad default civic");
							m_piCivilizationInitialCivics[eCivicOption] = eCivic;
						}
					}

					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
	}

	pXML->SetVariableListTagPair(&m_pbLeaders, L"Leaders", GC.getNumLeaderHeadInfos());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationSelectionSound");

	//TB Tags
	pXML->GetOptionalChildXmlValByName(&m_iSpawnRateModifier, L"iSpawnRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iSpawnRateNPCPeaceModifier, L"iSpawnRateNPCPeaceModifier");
	pXML->GetOptionalChildXmlValByName(&m_bStronglyRestricted, L"bStronglyRestricted");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iDerivativeCiv, L"DerivativeCiv");

	return true;
}

void CvCivilizationInfo::copyNonDefaults(const CvCivilizationInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;
	const int iTextDefault = -1;
	const bool bDefault = false;
	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	// must be before we set the InfoBaseClass else it can't find the button to to corresponding arttag
	if ( getArtDefineTag() == cDefault ) // "ArtDefineTag"
	{
		m_szArtDefineTag = pClassInfo->getArtDefineTag();
	}

	CvInfoBase::copyNonDefaults(pClassInfo);

	if ( isPlayable() == bDefault ) // "bPlayable"
	{
		m_bPlayable = (pClassInfo->isPlayable());
	}
	if ( isAIPlayable() == bDefault ) // "bAIPlayable"
	{
		m_bAIPlayable = (pClassInfo->isAIPlayable());
	}

	if ( getShortDescriptionKey() == wDefault )
	{
		m_szShortDescriptionKey = pClassInfo->getShortDescriptionKey();
	}

	if ( getAdjectiveKey() == wDefault ) // "Adjective"
	{
		m_szAdjectiveKey = pClassInfo->getAdjectiveKey();
	}

	if ( getDefaultPlayerColor() == NO_PLAYERCOLOR ) // "DefaultPlayerColor"
	{
		m_iDefaultPlayerColor = (pClassInfo->getDefaultPlayerColor());
	}
	if ( getArtStyleType() == NO_ARTSTYLE ) // "ArtStyleType"
	{
		m_iArtStyleType = (pClassInfo->getArtStyleType());
	}
	if ( getUnitArtStyleType() == NO_UNIT_ARTSTYLE ) // "UnitArtStyleType"
	{
		m_iUnitArtStyleType = (pClassInfo->getUnitArtStyleType());
	}
	if ( getSelectionSoundScriptId() == AUDIOTAG_NONE ) // "CivilizationSelectionSound"
	{
		m_iSelectionSoundScriptId = (pClassInfo->getSelectionSoundScriptId());
	}
	if ( getActionSoundScriptId() == AUDIOTAG_NONE ) // "CivilizationActionSound"
	{
		m_iActionSoundScriptId = (pClassInfo->getActionSoundScriptId());
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCivilizationBuildings, pClassInfo->m_aiCivilizationBuildings);

	for ( int i = 0; i < GC.getNumTechInfos(); i++)
	{
		if ( isCivilizationFreeTechs(i) == bDefault && pClassInfo->isCivilizationFreeTechs(i) != bDefault)
		{
			if ( NULL == m_pbCivilizationFreeTechs )
			{
				CvXMLLoadUtility::InitList(&m_pbCivilizationFreeTechs,GC.getNumTechInfos(),bDefault);
			}
			m_pbCivilizationFreeTechs[i] = pClassInfo->isCivilizationFreeTechs(i);
		}
		if ( isCivilizationDisableTechs(i) == bDefault && pClassInfo->isCivilizationDisableTechs(i) != bDefault)
		{
			if ( NULL == m_pbCivilizationDisableTechs )
			{
				CvXMLLoadUtility::InitList(&m_pbCivilizationDisableTechs,GC.getNumTechInfos(),bDefault);
			}
			m_pbCivilizationDisableTechs[i] = pClassInfo->isCivilizationDisableTechs(i);
		}
	}

	for ( int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		if ( getCivilizationInitialCivics(i) == -1 && pClassInfo->getCivilizationInitialCivics(i) != -1 )
		{
			if ( NULL == m_pbCivilizationDisableTechs )
			{
				CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics,GC.getNumCivicOptionInfos(),-1);
			}
			m_piCivilizationInitialCivics[i] = pClassInfo->getCivilizationInitialCivics(i);
		}
	}

	for ( int i = 0; i < GC.getNumLeaderHeadInfos(); i++)
	{
		if ( isLeaders(i) == bDefault && pClassInfo->isLeaders(i) != bDefault)
		{
			if ( NULL == m_pbLeaders )
			{
				CvXMLLoadUtility::InitList(&m_pbLeaders,GC.getNumLeaderHeadInfos(),bDefault);
			}
			m_pbLeaders[i] = pClassInfo->isLeaders(i);
		}
	}

	//TB Tags
	//int
	if ( getSpawnRateModifier() == iDefault )
	{
		m_iSpawnRateModifier = (pClassInfo->getSpawnRateModifier());
	}
	if ( getSpawnRateNPCPeaceModifier() == iDefault )
	{
		m_iSpawnRateNPCPeaceModifier = (pClassInfo->getSpawnRateNPCPeaceModifier());
	}
	//bool
	if ( isStronglyRestricted() == bDefault )
	{
		m_bStronglyRestricted = (pClassInfo->isStronglyRestricted());
	}

	// First we check if there are different Unique Names in the Modules(we want to keep all of them)
	// So we have to set the Arraysize properly, knowing the amount of Unique Names
	if ( pClassInfo->getNumCityNames() != 0 )
	{
		CvString* m_paszOldNames = new CvString[pClassInfo->getNumCityNames()];
		for ( int i = 0; i < pClassInfo->getNumCityNames(); i++)
		{
			m_paszOldNames[i] = pClassInfo->getCityNames(i);
		}

		CvXMLLoadUtilityModTools::StringArrayExtend(&m_paszCityNames, &m_iNumCityNames, &m_paszOldNames, pClassInfo->getNumCityNames());
		SAFE_DELETE_ARRAY(m_paszOldNames)
	}

	GC.copyNonDefaultDelayedResolution((int*)&m_iDerivativeCiv, (int*)&pClassInfo->m_iDerivativeCiv);
}

//======================================================================================================
//					CvVictoryInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvVictoryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvVictoryInfo::CvVictoryInfo() :
 m_iPopulationPercentLead(0)
,m_iLandPercent(0)
,m_iMinLandPercent(0)
,m_iReligionPercent(0)
,m_iCityCulture(NO_CULTURELEVEL)
,m_iNumCultureCities(0)
,m_iTotalCultureRatio(0)
,m_iVictoryDelayTurns(0)
,m_bTargetScore(false)
,m_bEndScore(false)
,m_bConquest(false)
,m_bDiploVote(false)
,m_bPermanent(false)
,m_bTotalVictory(false)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvVictoryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvVictoryInfo::~CvVictoryInfo() {}

int CvVictoryInfo::getPopulationPercentLead() const
{
	return m_iPopulationPercentLead;
}

int CvVictoryInfo::getLandPercent() const
{
	return m_iLandPercent;
}

int CvVictoryInfo::getMinLandPercent() const
{
	return m_iMinLandPercent;
}

int CvVictoryInfo::getReligionPercent() const
{
	return m_iReligionPercent;
}

int CvVictoryInfo::getCityCulture() const
{
	return m_iCityCulture;
}

int CvVictoryInfo::getNumCultureCities() const
{
	return m_iNumCultureCities;
}

int CvVictoryInfo::getTotalCultureRatio() const
{
	return m_iTotalCultureRatio;
}

int CvVictoryInfo::getVictoryDelayTurns() const
{
	return m_iVictoryDelayTurns;
}

bool CvVictoryInfo::isTargetScore() const
{
	return m_bTargetScore;
}

bool CvVictoryInfo::isEndScore() const
{
	return m_bEndScore;
}

bool CvVictoryInfo::isConquest() const
{
	return m_bConquest;
}

bool CvVictoryInfo::isDiploVote() const
{
	return m_bDiploVote;
}

bool CvVictoryInfo::isPermanent() const
{
	return m_bPermanent;
}

const char* CvVictoryInfo::getMovie() const
{
	return m_szMovie;
}

bool CvVictoryInfo::isTotalVictory() const
{
	return m_bTotalVictory;
}


//
// read from xml
//
bool CvVictoryInfo::read(CvXMLLoadUtility* pXML)
{

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bTargetScore, L"bTargetScore");
	pXML->GetOptionalChildXmlValByName(&m_bEndScore, L"bEndScore");
	pXML->GetOptionalChildXmlValByName(&m_bConquest, L"bConquest");
	pXML->GetOptionalChildXmlValByName(&m_bDiploVote, L"bDiploVote");
	pXML->GetOptionalChildXmlValByName(&m_bPermanent, L"bPermanent");
	pXML->GetOptionalChildXmlValByName(&m_bTotalVictory, L"bTotalVictory");

	pXML->GetOptionalChildXmlValByName(&m_iPopulationPercentLead, L"iPopulationPercentLead");
	pXML->GetOptionalChildXmlValByName(&m_iLandPercent, L"iLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iMinLandPercent, L"iMinLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iReligionPercent, L"iReligionPercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCultureCities, L"iNumCultureCities");
	pXML->GetOptionalChildXmlValByName(&m_iTotalCultureRatio, L"iTotalCultureRatio");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryDelayTurns, L"iVictoryDelayTurns");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CityCulture");
	m_iCityCulture = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(m_szMovie, L"VictoryMovie");

	return true;
}

void CvVictoryInfo::copyNonDefaults(const CvVictoryInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isTargetScore() == bDefault) m_bTargetScore = pClassInfo->isTargetScore();
	if (isEndScore() == bDefault) m_bEndScore = pClassInfo->isEndScore();
	if (isConquest() == bDefault) m_bConquest = pClassInfo->isConquest();
	if (isDiploVote() == bDefault) m_bDiploVote = pClassInfo->isDiploVote();
	if (isPermanent() == bDefault) m_bPermanent = pClassInfo->isPermanent();

	if (getPopulationPercentLead() == iDefault) m_iPopulationPercentLead = pClassInfo->getPopulationPercentLead();
	if (getLandPercent() == iDefault) m_iLandPercent = pClassInfo->getLandPercent();
	if (getMinLandPercent() == iDefault) m_iMinLandPercent = pClassInfo->getMinLandPercent();
	if (getReligionPercent() == iDefault) m_iReligionPercent = pClassInfo->getReligionPercent();

	if (getCityCulture() == iTextDefault) m_iCityCulture = pClassInfo->getCityCulture();

	if (getNumCultureCities() == iDefault) m_iNumCultureCities = pClassInfo->getNumCultureCities();
	if (getTotalCultureRatio() == iDefault) m_iTotalCultureRatio = pClassInfo->getTotalCultureRatio();
	if (getVictoryDelayTurns() == iDefault) m_iVictoryDelayTurns = pClassInfo->getVictoryDelayTurns();

	if (getMovie() == cDefault) m_szMovie = pClassInfo->getMovie();

	if (isTotalVictory() == bDefault) m_bTotalVictory = pClassInfo->isTotalVictory();
}

void CvVictoryInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iPopulationPercentLead);
	CheckSum(iSum, m_iLandPercent);
	CheckSum(iSum, m_iMinLandPercent);
	CheckSum(iSum, m_iReligionPercent);
	CheckSum(iSum, m_iCityCulture);
	CheckSum(iSum, m_iNumCultureCities);
	CheckSum(iSum, m_iTotalCultureRatio);
	CheckSum(iSum, m_iVictoryDelayTurns);

	CheckSum(iSum, m_bTargetScore);
	CheckSum(iSum, m_bEndScore);
	CheckSum(iSum, m_bConquest);
	CheckSum(iSum, m_bDiploVote);
	CheckSum(iSum, m_bPermanent);
}

//======================================================================================================
//					CvHurryInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHurryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHurryInfo::CvHurryInfo() :
m_iGoldPerProduction(0),
m_iProductionPerPopulation(0),
m_bAnger(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHurryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHurryInfo::~CvHurryInfo()
{
}

int CvHurryInfo::getGoldPerProduction() const
{
	return m_iGoldPerProduction;
}

int CvHurryInfo::getProductionPerPopulation() const
{
	return m_iProductionPerPopulation;
}

bool CvHurryInfo::isAnger() const
{
	return m_bAnger;
}

bool CvHurryInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iGoldPerProduction, L"iGoldPerProduction");
	pXML->GetOptionalChildXmlValByName(&m_iProductionPerPopulation, L"iProductionPerPopulation");

	pXML->GetOptionalChildXmlValByName(&m_bAnger, L"bAnger");

	return true;
}

void CvHurryInfo::copyNonDefaults(const CvHurryInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getGoldPerProduction() == iDefault) m_iGoldPerProduction = pClassInfo->getGoldPerProduction();
	if (getProductionPerPopulation() == iDefault) m_iProductionPerPopulation = pClassInfo->getProductionPerPopulation();

	if (isAnger() == bDefault) m_bAnger = pClassInfo->isAnger();
}

void CvHurryInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iGoldPerProduction);
	CheckSum(iSum, m_iProductionPerPopulation);

	CheckSum(iSum, m_bAnger);
}

//======================================================================================================
//					CvHandicapInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHandicapInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHandicapInfo::CvHandicapInfo() :
m_iFreeWinsVsBarbs(0),
m_iAnimalAttackProb(0),
m_iAdvancedStartPointsMod(0),
m_iStartingGold(0),
m_iUnitUpkeepPercent(0),
m_iDistanceMaintenancePercent(0),
m_iNumCitiesMaintenancePercent(0),
m_iColonyMaintenancePercent(0),
m_iMaxColonyMaintenance(0),
m_iCorporationMaintenancePercent(0),
m_iCivicUpkeepPercent(0),
m_iInflationPercent(0),
m_iHealthBonus(0),
m_iHappyBonus(0),
m_iAttitudeChange(0),
m_iNoTechTradeModifier(0),
m_iTechTradeKnownModifier(0),
m_iUnownedWaterTilesPerBarbarianUnit(0),
m_iUnownedTilesPerBarbarianCity(0),
m_iBarbarianCityCreationTurnsElapsed(0),
m_iBarbarianCityCreationProb(0),
m_iAnimalCombatModifier(0),
m_iBarbarianCombatModifier(0),
m_iAIAnimalCombatModifier(0),
m_iSubdueAnimalBonusAI(0),
m_iAIBarbarianCombatModifier(0),
m_iStartingDefenseUnits(0),
m_iStartingWorkerUnits(0),
m_iStartingExploreUnits(0),
m_iAIStartingDefenseUnits(0),
m_iAIStartingWorkerUnits(0),
m_iAIStartingExploreUnits(0),
m_iBarbarianInitialDefenders(0),
m_iAIDeclareWarProb(0),
m_iAIWorkRateModifier(0),
m_iAIGrowthPercent(0),
m_iAITrainPercent(0),
m_iAIWorldTrainPercent(0),
m_iAIConstructPercent(0),
m_iAIWorldConstructPercent(0),
m_iAICreatePercent(0),
m_iAIResearchPercent(0),
m_iAIWorldCreatePercent(0),
m_iAICivicUpkeepPercent(0),
m_iAIUnitUpkeepPercent(0),
m_iAIUnitSupplyPercent(0),
m_iAIUnitUpgradePercent(0),
m_iAIInflationPercent(0),
m_iAIWarWearinessPercent(0),
m_iAIPerEraModifier(0),
m_iAIAdvancedStartPercent(0),
m_iRevolutionIndexPercent(0)
,m_PropertyManipulators()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHandicapInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHandicapInfo::~CvHandicapInfo()
{
}

int CvHandicapInfo::getFreeWinsVsBarbs() const
{
	return m_iFreeWinsVsBarbs;
}

int CvHandicapInfo::getAnimalAttackProb() const
{
	return m_iAnimalAttackProb;
}

int CvHandicapInfo::getAdvancedStartPointsMod() const
{
	return m_iAdvancedStartPointsMod;
}

int CvHandicapInfo::getStartingGold() const
{
	return m_iStartingGold;
}

int CvHandicapInfo::getUnitUpkeepPercent() const
{
	return m_iUnitUpkeepPercent;
}

int CvHandicapInfo::getDistanceMaintenancePercent() const
{
	return m_iDistanceMaintenancePercent;
}

int CvHandicapInfo::getNumCitiesMaintenancePercent() const
{
	return m_iNumCitiesMaintenancePercent;
}

int CvHandicapInfo::getColonyMaintenancePercent() const
{
	return m_iColonyMaintenancePercent;
}

int CvHandicapInfo::getMaxColonyMaintenance() const
{
	return m_iMaxColonyMaintenance;
}

int CvHandicapInfo::getCorporationMaintenancePercent() const
{
	return m_iCorporationMaintenancePercent;
}

int CvHandicapInfo::getCivicUpkeepPercent() const
{
	return m_iCivicUpkeepPercent;
}

int CvHandicapInfo::getInflationPercent() const
{
	return m_iInflationPercent;
}

int CvHandicapInfo::getHealthBonus() const
{
	return m_iHealthBonus;
}

int CvHandicapInfo::getHappyBonus() const
{
	return m_iHappyBonus;
}

int CvHandicapInfo::getAttitudeChange() const
{
	return m_iAttitudeChange;
}

int CvHandicapInfo::getNoTechTradeModifier() const
{
	return m_iNoTechTradeModifier;
}

int CvHandicapInfo::getTechTradeKnownModifier() const
{
	return m_iTechTradeKnownModifier;
}

int CvHandicapInfo::getUnownedWaterTilesPerBarbarianUnit() const
{
	return m_iUnownedWaterTilesPerBarbarianUnit;
}

int CvHandicapInfo::getUnownedTilesPerBarbarianCity() const
{
	return m_iUnownedTilesPerBarbarianCity;
}

int CvHandicapInfo::getBarbarianCityCreationTurnsElapsed() const
{
	return m_iBarbarianCityCreationTurnsElapsed;
}

int CvHandicapInfo::getBarbarianCityCreationProb() const
{
	return m_iBarbarianCityCreationProb;
}

int CvHandicapInfo::getAnimalCombatModifier() const
{
	return m_iAnimalCombatModifier;
}

int CvHandicapInfo::getBarbarianCombatModifier() const
{
	return m_iBarbarianCombatModifier;
}

int CvHandicapInfo::getAIAnimalCombatModifier() const
{
	return m_iAIAnimalCombatModifier;
}

int CvHandicapInfo::getAIBarbarianCombatModifier() const
{
	return m_iAIBarbarianCombatModifier;
}

int CvHandicapInfo::getStartingDefenseUnits() const
{
	return m_iStartingDefenseUnits;
}

int CvHandicapInfo::getStartingWorkerUnits() const
{
	return m_iStartingWorkerUnits;
}

int CvHandicapInfo::getStartingExploreUnits() const
{
	return m_iStartingExploreUnits;
}

int CvHandicapInfo::getAIStartingDefenseUnits() const
{
	return m_iAIStartingDefenseUnits;
}

int CvHandicapInfo::getAIStartingWorkerUnits() const
{
	return m_iAIStartingWorkerUnits;
}

int CvHandicapInfo::getAIStartingExploreUnits() const
{
	return m_iAIStartingExploreUnits;
}

int CvHandicapInfo::getBarbarianInitialDefenders() const
{
	return m_iBarbarianInitialDefenders;
}

int CvHandicapInfo::getAIDeclareWarProb() const
{
	return m_iAIDeclareWarProb;
}

int CvHandicapInfo::getAIWorkRateModifier() const
{
	return m_iAIWorkRateModifier;
}

int CvHandicapInfo::getAIGrowthPercent() const
{
	return m_iAIGrowthPercent;
}

int CvHandicapInfo::getAITrainPercent() const
{
	return m_iAITrainPercent;
}

int CvHandicapInfo::getAIWorldTrainPercent() const
{
	return m_iAIWorldTrainPercent;
}

int CvHandicapInfo::getAIConstructPercent() const
{
	return m_iAIConstructPercent;
}

int CvHandicapInfo::getAIWorldConstructPercent() const
{
	return m_iAIWorldConstructPercent;
}

int CvHandicapInfo::getAICreatePercent() const
{
	return m_iAICreatePercent;
}

int CvHandicapInfo::getAIResearchPercent() const
{
	return m_iAIResearchPercent;
}

int CvHandicapInfo::getAIWorldCreatePercent() const
{
	return m_iAIWorldCreatePercent;
}

int CvHandicapInfo::getAICivicUpkeepPercent() const
{
	return m_iAICivicUpkeepPercent;
}

int CvHandicapInfo::getAIUnitUpkeepPercent() const
{
	return m_iAIUnitUpkeepPercent;
}

int CvHandicapInfo::getAIUnitSupplyPercent() const
{
	return m_iAIUnitSupplyPercent;
}

int CvHandicapInfo::getAIUnitUpgradePercent() const
{
	return m_iAIUnitUpgradePercent;
}

int CvHandicapInfo::getAIInflationPercent() const
{
	return m_iAIInflationPercent;
}

int CvHandicapInfo::getAIWarWearinessPercent() const
{
	return m_iAIWarWearinessPercent;
}

int CvHandicapInfo::getAIPerEraModifier() const
{
	return m_iAIPerEraModifier;
}

int CvHandicapInfo::getAIAdvancedStartPercent() const
{
	return m_iAIAdvancedStartPercent;
}

int CvHandicapInfo::getNumGoodies() const
{
	return m_piGoodies.size();
}

int CvHandicapInfo::getGoodies(int i) const
{
	FASSERT_BOUNDS(0, getNumGoodies(), i);
	return m_piGoodies[i];
}

int CvHandicapInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}

int CvHandicapInfo::getRevolutionIndexPercent() const
{
	return m_iRevolutionIndexPercent;
}


void CvHandicapInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iFreeWinsVsBarbs);
	CheckSum(iSum, m_iAnimalAttackProb);
	CheckSum(iSum, m_iAdvancedStartPointsMod);
	CheckSum(iSum, m_iStartingGold);
	CheckSum(iSum, m_iUnitUpkeepPercent);
	CheckSum(iSum, m_iDistanceMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesMaintenancePercent);
	CheckSum(iSum, m_iColonyMaintenancePercent);
	CheckSum(iSum, m_iMaxColonyMaintenance);
	CheckSum(iSum, m_iCorporationMaintenancePercent);
	CheckSum(iSum, m_iCivicUpkeepPercent);
	CheckSum(iSum, m_iInflationPercent);
	CheckSum(iSum, m_iHealthBonus);
	CheckSum(iSum, m_iHappyBonus);
	CheckSum(iSum, m_iAttitudeChange);
	CheckSum(iSum, m_iNoTechTradeModifier);
	CheckSum(iSum, m_iTechTradeKnownModifier);
	CheckSum(iSum, m_iUnownedWaterTilesPerBarbarianUnit);
	CheckSum(iSum, m_iUnownedTilesPerBarbarianCity);
	CheckSum(iSum, m_iBarbarianCityCreationTurnsElapsed);
	CheckSum(iSum, m_iBarbarianCityCreationProb);
	CheckSum(iSum, m_iAnimalCombatModifier);
	CheckSum(iSum, m_iBarbarianCombatModifier);
	CheckSum(iSum, m_iAIAnimalCombatModifier);
	CheckSum(iSum, m_iAIBarbarianCombatModifier);

	CheckSum(iSum, m_iStartingDefenseUnits);
	CheckSum(iSum, m_iStartingWorkerUnits);
	CheckSum(iSum, m_iStartingExploreUnits);
	CheckSum(iSum, m_iAIStartingDefenseUnits);
	CheckSum(iSum, m_iAIStartingWorkerUnits);
	CheckSum(iSum, m_iAIStartingExploreUnits);
	CheckSum(iSum, m_iBarbarianInitialDefenders);
	CheckSum(iSum, m_iAIDeclareWarProb);
	CheckSum(iSum, m_iAIWorkRateModifier);
	CheckSum(iSum, m_iAIGrowthPercent);
	CheckSum(iSum, m_iAITrainPercent);
	CheckSum(iSum, m_iAIWorldTrainPercent);
	CheckSum(iSum, m_iAIConstructPercent);
	CheckSum(iSum, m_iAIWorldConstructPercent);
	CheckSum(iSum, m_iAICreatePercent);
	CheckSum(iSum, m_iAIResearchPercent);
	CheckSum(iSum, m_iAIWorldCreatePercent);
	CheckSum(iSum, m_iAICivicUpkeepPercent);
	CheckSum(iSum, m_iAIUnitUpkeepPercent);
	CheckSum(iSum, m_iAIUnitSupplyPercent);
	CheckSum(iSum, m_iAIUnitUpgradePercent);
	CheckSum(iSum, m_iAIInflationPercent);
	CheckSum(iSum, m_iAIWarWearinessPercent);
	CheckSum(iSum, m_iAIPerEraModifier);
	CheckSum(iSum, m_iAIAdvancedStartPercent);
	CheckSumC(iSum, m_Percent);

	m_PropertyManipulators.getCheckSum(iSum);

	CheckSumC(iSum, m_piGoodies);

	CheckSum(iSum, m_iRevolutionIndexPercent);
}

bool CvHandicapInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iFreeWinsVsBarbs, L"iFreeWinsVsBarbs");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalAttackProb, L"iAnimalAttackProb");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartPointsMod, L"iAdvancedStartPointsMod");
	pXML->GetOptionalChildXmlValByName(&m_iStartingGold, L"iGold");
	pXML->GetOptionalChildXmlValByName(&m_iUnitUpkeepPercent, L"iUnitUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenancePercent, L"iDistanceMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenancePercent, L"iNumCitiesMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iColonyMaintenancePercent, L"iColonyMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxColonyMaintenance, L"iMaxColonyMaintenance");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenancePercent, L"iCorporationMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCivicUpkeepPercent, L"iCivicUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iInflationPercent, L"iInflationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iHealthBonus, L"iHealthBonus");
	pXML->GetOptionalChildXmlValByName(&m_iHappyBonus, L"iHappyBonus");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeChange, L"iAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoTechTradeModifier, L"iNoTechTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTechTradeKnownModifier, L"iTechTradeKnownModifier");
	pXML->GetOptionalChildXmlValByName(&m_iUnownedWaterTilesPerBarbarianUnit, L"iUnownedWaterTilesPerBarbarianUnit");
	pXML->GetOptionalChildXmlValByName(&m_iUnownedTilesPerBarbarianCity, L"iUnownedTilesPerBarbarianCity");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCityCreationTurnsElapsed, L"iBarbarianCityCreationTurnsElapsed");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCityCreationProb, L"iBarbarianCityCreationProb");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalCombatModifier, L"iAnimalBonus");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianCombatModifier, L"iBarbarianBonus");
	pXML->GetOptionalChildXmlValByName(&m_iAIAnimalCombatModifier, L"iAIAnimalBonus");
	pXML->GetOptionalChildXmlValByName(&m_iSubdueAnimalBonusAI, L"iSubdueAnimalBonusAI");
	pXML->GetOptionalChildXmlValByName(&m_iAIBarbarianCombatModifier, L"iAIBarbarianBonus");
	pXML->GetOptionalChildXmlValByName(&m_iStartingDefenseUnits, L"iStartingDefenseUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingWorkerUnits, L"iStartingWorkerUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingExploreUnits, L"iStartingExploreUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingDefenseUnits, L"iAIStartingDefenseUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingWorkerUnits, L"iAIStartingWorkerUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAIStartingExploreUnits, L"iAIStartingExploreUnits");
	pXML->GetOptionalChildXmlValByName(&m_iBarbarianInitialDefenders, L"iBarbarianDefenders");
	pXML->GetOptionalChildXmlValByName(&m_iAIDeclareWarProb, L"iAIDeclareWarProb");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorkRateModifier, L"iAIWorkRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAIGrowthPercent, L"iAIGrowthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAITrainPercent, L"iAITrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldTrainPercent, L"iAIWorldTrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIConstructPercent, L"iAIConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldConstructPercent, L"iAIWorldConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAICreatePercent, L"iAICreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIResearchPercent, L"iAIResearchPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWorldCreatePercent, L"iAIWorldCreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAICivicUpkeepPercent, L"iAICivicUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitUpkeepPercent, L"iAIUnitUpkeepPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitSupplyPercent, L"iAIUnitSupplyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIUnitUpgradePercent, L"iAIUnitUpgradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIInflationPercent, L"iAIInflationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIWarWearinessPercent, L"iAIWarWearinessPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAIPerEraModifier, L"iAIPerEraModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAIAdvancedStartPercent, L"iAIAdvancedStartPercent");

	pXML->SetOptionalVector(&m_piGoodies, L"Goodies");

	pXML->GetOptionalChildXmlValByName(&m_iRevolutionIndexPercent, L"iRevolutionIndexPercent");

	m_Percent.read(pXML, L"Percents");

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvHandicapInfo::copyNonDefaults(const CvHandicapInfo* pClassInfo)
{
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getFreeWinsVsBarbs() == iDefault) m_iFreeWinsVsBarbs = pClassInfo->getFreeWinsVsBarbs();
	if (getAnimalAttackProb() == iDefault) m_iAnimalAttackProb = pClassInfo->getAnimalAttackProb();
	if (getAdvancedStartPointsMod() == iDefault) m_iAdvancedStartPointsMod = pClassInfo->getAdvancedStartPointsMod();
	if (getStartingGold() == iDefault) m_iStartingGold = pClassInfo->getStartingGold();
	if (getUnitUpkeepPercent() == iDefault) m_iUnitUpkeepPercent = pClassInfo->getUnitUpkeepPercent();
	if (getDistanceMaintenancePercent() == iDefault) m_iDistanceMaintenancePercent = pClassInfo->getDistanceMaintenancePercent();
	if (getNumCitiesMaintenancePercent() == iDefault) m_iNumCitiesMaintenancePercent = pClassInfo->getNumCitiesMaintenancePercent();
	if (getColonyMaintenancePercent() == iDefault) m_iColonyMaintenancePercent = pClassInfo->getColonyMaintenancePercent();
	if (getMaxColonyMaintenance() == iDefault) m_iMaxColonyMaintenance = pClassInfo->getMaxColonyMaintenance();
	if (getCorporationMaintenancePercent() == iDefault) m_iCorporationMaintenancePercent = pClassInfo->getCorporationMaintenancePercent();
	if (getCivicUpkeepPercent() == iDefault) m_iCivicUpkeepPercent = pClassInfo->getCivicUpkeepPercent();
	if (getInflationPercent() == iDefault) m_iInflationPercent = pClassInfo->getInflationPercent();
	if (getHealthBonus() == iDefault) m_iHealthBonus = pClassInfo->getHealthBonus();
	if (getHappyBonus() == iDefault) m_iHappyBonus = pClassInfo->getHappyBonus();
	if (getAttitudeChange() == iDefault) m_iAttitudeChange = pClassInfo->getAttitudeChange();
	if (getNoTechTradeModifier() == iDefault) m_iNoTechTradeModifier = pClassInfo->getNoTechTradeModifier();
	if (getTechTradeKnownModifier() == iDefault) m_iTechTradeKnownModifier = pClassInfo->getTechTradeKnownModifier();
	if (getUnownedWaterTilesPerBarbarianUnit() == iDefault) m_iUnownedWaterTilesPerBarbarianUnit = pClassInfo->getUnownedWaterTilesPerBarbarianUnit();
	if (getUnownedTilesPerBarbarianCity() == iDefault) m_iUnownedTilesPerBarbarianCity = pClassInfo->getUnownedTilesPerBarbarianCity();
	if (getBarbarianCityCreationTurnsElapsed() == iDefault) m_iBarbarianCityCreationTurnsElapsed = pClassInfo->getBarbarianCityCreationTurnsElapsed();
	if (getBarbarianCityCreationProb() == iDefault) m_iBarbarianCityCreationProb = pClassInfo->getBarbarianCityCreationProb();
	if (getAnimalCombatModifier() == iDefault) m_iAnimalCombatModifier = pClassInfo->getAnimalCombatModifier();
	if (getBarbarianCombatModifier() == iDefault) m_iBarbarianCombatModifier = pClassInfo->getBarbarianCombatModifier();
	if (getAIAnimalCombatModifier() == iDefault) m_iAIAnimalCombatModifier = pClassInfo->getAIAnimalCombatModifier();
	if (m_iSubdueAnimalBonusAI == iDefault) m_iSubdueAnimalBonusAI = pClassInfo->getSubdueAnimalBonusAI();
	if (getAIBarbarianCombatModifier() == iDefault) m_iAIBarbarianCombatModifier = pClassInfo->getAIBarbarianCombatModifier();
	if (getStartingDefenseUnits() == iDefault) m_iStartingDefenseUnits = pClassInfo->getStartingDefenseUnits();
	if (getStartingWorkerUnits() == iDefault) m_iStartingWorkerUnits = pClassInfo->getStartingWorkerUnits();
	if (getStartingExploreUnits() == iDefault) m_iStartingExploreUnits = pClassInfo->getStartingExploreUnits();
	if (getAIStartingDefenseUnits() == iDefault) m_iAIStartingDefenseUnits = pClassInfo->getAIStartingDefenseUnits();
	if (getAIStartingWorkerUnits() == iDefault) m_iAIStartingWorkerUnits = pClassInfo->getAIStartingWorkerUnits();
	if (getAIStartingExploreUnits() == iDefault) m_iAIStartingExploreUnits = pClassInfo->getAIStartingExploreUnits();
	if (getBarbarianInitialDefenders() == iDefault) m_iBarbarianInitialDefenders = pClassInfo->getBarbarianInitialDefenders();
	if (getAIDeclareWarProb() == iDefault) m_iAIDeclareWarProb = pClassInfo->getAIDeclareWarProb();
	if (getAIWorkRateModifier() == iDefault) m_iAIWorkRateModifier = pClassInfo->getAIWorkRateModifier();
	if (getAIGrowthPercent() == iDefault) m_iAIGrowthPercent = pClassInfo->getAIGrowthPercent();
	if (getAITrainPercent() == iDefault) m_iAITrainPercent = pClassInfo->getAITrainPercent();
	if (getAIWorldTrainPercent() == iDefault) m_iAIWorldTrainPercent = pClassInfo->getAIWorldTrainPercent();
	if (getAIConstructPercent() == iDefault) m_iAIConstructPercent = pClassInfo->getAIConstructPercent();
	if (getAIWorldConstructPercent() == iDefault) m_iAIWorldConstructPercent = pClassInfo->getAIWorldConstructPercent();
	if (getAICreatePercent() == iDefault) m_iAICreatePercent = pClassInfo->getAICreatePercent();
	if (getAIResearchPercent() == iDefault) m_iAIResearchPercent = pClassInfo->getAIResearchPercent();
	if (getAIWorldCreatePercent() == iDefault) m_iAIWorldCreatePercent = pClassInfo->getAIWorldCreatePercent();
	if (getAICivicUpkeepPercent() == iDefault) m_iAICivicUpkeepPercent = pClassInfo->getAICivicUpkeepPercent();
	if (getAIUnitUpkeepPercent() == iDefault) m_iAIUnitUpkeepPercent = pClassInfo->getAIUnitUpkeepPercent();
	if (getAIUnitSupplyPercent() == iDefault) m_iAIUnitSupplyPercent = pClassInfo->getAIUnitSupplyPercent();
	if (getAIUnitUpgradePercent() == iDefault) m_iAIUnitUpgradePercent = pClassInfo->getAIUnitUpgradePercent();
	if (getAIInflationPercent() == iDefault) m_iAIInflationPercent = pClassInfo->getAIInflationPercent();
	if (getAIWarWearinessPercent() == iDefault) m_iAIWarWearinessPercent = pClassInfo->getAIWarWearinessPercent();
	if (getAIPerEraModifier() == iDefault) m_iAIPerEraModifier = pClassInfo->getAIPerEraModifier();
	if (getAIAdvancedStartPercent() == iDefault) m_iAIAdvancedStartPercent = pClassInfo->getAIAdvancedStartPercent();
	if (getRevolutionIndexPercent() == iDefault) m_iRevolutionIndexPercent = pClassInfo->getRevolutionIndexPercent();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_piGoodies, pClassInfo->m_piGoodies);

	m_Percent.copyNonDefaults(pClassInfo->m_Percent);

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

//======================================================================================================
//					CvGameSpeedInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvGameSpeedInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvGameSpeedInfo::CvGameSpeedInfo() :
m_iSpeedPercent(0),
m_iNumTurnIncrements(0),
m_pGameTurnInfo(NULL),
m_bEndDatesCalculated(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvGameSpeedInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvGameSpeedInfo::~CvGameSpeedInfo()
{
	SAFE_DELETE_ARRAY(m_pGameTurnInfo);
}

int CvGameSpeedInfo::getSpeedPercent() const
{
	return m_iSpeedPercent;
}

int CvGameSpeedInfo::getHammerCostPercent() const
{
	if (GC.getGame().isOption(GAMEOPTION_EXP_UPSCALED_BUILDING_AND_UNIT_COSTS))
	{
		return getModifiedIntValue(m_iSpeedPercent, GC.getUPSCALED_HAMMER_COST_MODIFIER());
	}
	return m_iSpeedPercent;
}

int CvGameSpeedInfo::getNumTurnIncrements() const
{
	return m_iNumTurnIncrements;
}

const CvDateIncrement& CvGameSpeedInfo::getDateIncrement(int iIndex) const
{
	return m_aIncrements[iIndex];
}

std::vector<CvDateIncrement>& CvGameSpeedInfo::getIncrements()
{
	return m_aIncrements;
}

bool CvGameSpeedInfo::getEndDatesCalculated() const
{
	return m_bEndDatesCalculated;
}

void CvGameSpeedInfo::setEndDatesCalculated(bool bCalculated)
{
	m_bEndDatesCalculated = bCalculated;
}

const GameTurnInfo& CvGameSpeedInfo::getGameTurnInfo(int iIndex) const
{
	return m_pGameTurnInfo[iIndex];
}

void CvGameSpeedInfo::allocateGameTurnInfos(int iSize)
{
	m_pGameTurnInfo = new GameTurnInfo[iSize];
}

int CvGameSpeedInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}

bool CvGameSpeedInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(&m_iSpeedPercent, L"iSpeedPercent");

	if (pXML->TryMoveToXmlFirstChild(L"GameTurnInfos"))
	{
		m_bEndDatesCalculated = false;
		m_iNumTurnIncrements = pXML->GetXmlChildrenNumber(L"GameTurnInfo");
		char szLog[1000];
		sprintf(szLog, ":: %i", m_iNumTurnIncrements);
#ifdef _DEBUG
		OutputDebugString(szLog);
#endif
		if (pXML->TryMoveToXmlFirstChild(L"GameTurnInfo"))
		{
			allocateGameTurnInfos(getNumTurnIncrements());
			int iTempVal;

			// loop through each tag
			for (int j = 0; j < getNumTurnIncrements(); j++)
			{
				CvDateIncrement inc;
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iMonthIncrement");
				m_pGameTurnInfo[j].iMonthIncrement = iTempVal;
				inc.m_iIncrementMonth = iTempVal;
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iTurnsPerIncrement");
				m_pGameTurnInfo[j].iNumGameTurnsPerIncrement = iTempVal;
				inc.m_iendTurn = iTempVal + (m_aIncrements.empty() ? 0 : m_aIncrements[m_aIncrements.size()-1].m_iendTurn);
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iDayIncrement");
				inc.m_iIncrementDay = iTempVal;
				m_aIncrements.push_back(inc);

				// if we cannot set the current xml node to it's next sibling then we will break out of the for loop
				// otherwise we will continue looping
				if (!pXML->TryMoveToXmlNextSibling(L"GameTurnInfo"))
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
	m_Percent.read(pXML, L"Percents");

	return true;
}

void CvGameSpeedInfo::copyNonDefaults(const CvGameSpeedInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (m_iSpeedPercent == iDefault) m_iSpeedPercent = pClassInfo->getSpeedPercent();

	if (getNumTurnIncrements() == iDefault)
	{
		m_iNumTurnIncrements = pClassInfo->getNumTurnIncrements();
		allocateGameTurnInfos(getNumTurnIncrements());
		// loop through each tag
		for (int j = 0; j < getNumTurnIncrements(); j++)
		{
			m_pGameTurnInfo[j] = pClassInfo->getGameTurnInfo(j);
			m_aIncrements[j] = pClassInfo->getDateIncrement(j);
		}
	}

	m_Percent.copyNonDefaults(pClassInfo->m_Percent);
}

void CvGameSpeedInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iSpeedPercent);

	for (int j = 0; j < m_iNumTurnIncrements; j++)
	{
		CheckSum(iSum, m_pGameTurnInfo[j].iMonthIncrement);
		CheckSum(iSum, m_pGameTurnInfo[j].iNumGameTurnsPerIncrement);
		CheckSum(iSum, m_aIncrements[j].m_iIncrementDay);
	}
	CheckSumC(iSum, m_Percent);
}


//======================================================================================================
//					CvTurnTimerInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTurnTimerInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTurnTimerInfo::CvTurnTimerInfo() :
m_iBaseTime(0),
m_iCityBonus(0),
m_iUnitBonus(0),
m_iFirstTurnMultiplier(0)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTurnTimerInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTurnTimerInfo::~CvTurnTimerInfo()
{
}

int CvTurnTimerInfo::getBaseTime() const
{
	return m_iBaseTime;
}

int CvTurnTimerInfo::getCityBonus() const
{
	return m_iCityBonus;
}

int CvTurnTimerInfo::getUnitBonus() const
{
	return m_iUnitBonus;
}

int CvTurnTimerInfo::getFirstTurnMultiplier() const
{
	return m_iFirstTurnMultiplier;
}

bool CvTurnTimerInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iBaseTime, L"iBaseTime");
	pXML->GetOptionalChildXmlValByName(&m_iCityBonus, L"iCityBonus");
	pXML->GetOptionalChildXmlValByName(&m_iUnitBonus, L"iUnitBonus");
	pXML->GetOptionalChildXmlValByName(&m_iFirstTurnMultiplier, L"iFirstTurnMultiplier");

	return true;
}

void CvTurnTimerInfo::copyNonDefaults(const CvTurnTimerInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getBaseTime() == iDefault) m_iBaseTime = pClassInfo->getBaseTime();
	if (getCityBonus() == iDefault) m_iCityBonus = pClassInfo->getCityBonus();
	if (getUnitBonus() == iDefault) m_iUnitBonus = pClassInfo->getUnitBonus();
	if (getFirstTurnMultiplier() == iDefault) m_iFirstTurnMultiplier = pClassInfo->getFirstTurnMultiplier();
}

void CvTurnTimerInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iBaseTime);
	CheckSum(iSum, m_iCityBonus);
	CheckSum(iSum, m_iUnitBonus);
	CheckSum(iSum, m_iFirstTurnMultiplier);
}

//======================================================================================================
//					CvBuildInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBuildInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBuildInfo::CvBuildInfo() :
m_iTime(0),
m_iCost(0),
m_iTechPrereq(NO_TECH),
m_iImprovement(NO_IMPROVEMENT),
m_iRoute(NO_ROUTE),
m_iEntityEvent(ENTITY_EVENT_NONE),
m_iMissionType(NO_MISSION),
m_bKill(false),
m_paiFeatureTech(NULL),
m_paiFeatureTime(NULL),
m_paiFeatureProduction(NULL),
m_pabFeatureRemove(NULL)

,m_iTerrainChange(NO_TERRAIN)
,m_iFeatureChange(NO_FEATURE)
,m_iObsoleteTech(NO_TECH)
,m_bDisabled(false)
{
	CvInfoUtil(this).initDataMembers();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBuildInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBuildInfo::~CvBuildInfo()
{
	PROFILE_EXTRA_FUNC();
	CvInfoUtil(this).uninitDataMembers();

	SAFE_DELETE_ARRAY(m_paiFeatureTech);
	SAFE_DELETE_ARRAY(m_paiFeatureTime);
	SAFE_DELETE_ARRAY(m_paiFeatureProduction);
	SAFE_DELETE_ARRAY(m_pabFeatureRemove);

	for (int i=0; i<(int)m_aiPrereqBonusTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiPrereqBonusTypes[i]));
	}

	for (int i=0; i<(int)m_aTerrainStructs.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aTerrainStructs[i]));
	}

	for (int i=0; i<(int)m_aPlaceBonusTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPlaceBonusTypes[i]));
	}
}

void CvBuildInfo::getDataMembers(CvInfoUtil& util)
{
	util
		//.addEnum(m_iObsoleteTech, L"ObsoleteTech")
		//.add(m_piBonusHealthChanges, L"BonusHealthChanges")
	;
}

int CvBuildInfo::getTime() const
{
	return m_iTime;
}

int CvBuildInfo::getCost() const
{
	return m_iCost;
}

int CvBuildInfo::getRoute() const
{
	return m_iRoute;
}

int CvBuildInfo::getTerrainChange() const
{
	return m_iTerrainChange;
}

int CvBuildInfo::getFeatureChange() const
{
	return m_iFeatureChange;
}

TechTypes CvBuildInfo::getObsoleteTech() const
{
	return m_iObsoleteTech;
}

bool CvBuildInfo::isDisabled() const
{
	return m_bDisabled;
}
void CvBuildInfo::setDisabled(bool bNewVal)
{
	m_bDisabled = bNewVal;
}

int CvBuildInfo::getEntityEvent() const
{
	return m_iEntityEvent;
}

int CvBuildInfo::getMissionType() const
{
	return m_iMissionType;
}

void CvBuildInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}

bool CvBuildInfo::isKill() const
{
	return m_bKill;
}

// Arrays

TechTypes CvBuildInfo::getFeatureTech(FeatureTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), e);
	return m_paiFeatureTech ? (TechTypes)m_paiFeatureTech[e] : NO_TECH;
}

int CvBuildInfo::getFeatureTime(FeatureTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), e);
	return m_paiFeatureTime ? m_paiFeatureTime[e] : 0;
}

int CvBuildInfo::getFeatureProduction(FeatureTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), e);
	return m_paiFeatureProduction ? m_paiFeatureProduction[e] : 0;
}

bool CvBuildInfo::isFeatureRemove(FeatureTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), e);
	return m_pabFeatureRemove ? m_pabFeatureRemove[e] : false;
}

int CvBuildInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvBuildInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvBuildInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

bool CvBuildInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	CvString szTextVal2;
	CvString szTextVal3;
	CvString szTextVal4;
	CvString szTextVal5;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	CvInfoUtil(this).readXml(pXML);

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"PrereqTech");
	pXML->GetOptionalChildXmlValByName(&m_iTime, L"iTime");
	pXML->GetOptionalChildXmlValByName(&m_iCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_bKill, L"bKill");
	pXML->GetOptionalTypeEnum(m_iObsoleteTech, L"ObsoleteTech");
	pXML->GetOptionalTypeEnum(m_iImprovement, L"ImprovementType");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"RouteType");
	m_iRoute = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TerrainChange");
	m_iTerrainChange = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FeatureChange");
	m_iFeatureChange = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EntityEvent");
	m_iEntityEvent = pXML->GetInfoClass(szTextVal);

	pXML->SetFeatureStruct(&m_paiFeatureTech, &m_paiFeatureTime, &m_paiFeatureProduction, &m_pabFeatureRemove);

	pXML->SetOptionalVectorWithDelayedResolution(m_aiPrereqBonusTypes, L"PrereqBonusTypes");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	if(pXML->TryMoveToXmlFirstChild(L"TerrainStructs"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"TerrainStruct" );
		m_aTerrainStructs.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainStruct"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aTerrainStructs[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					m_aTerrainStructs[i].ePrereqTech = (TechTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aTerrainStructs[i].iTime), L"iTime");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TerrainStruct"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PlaceBonusTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"PlaceBonusType" );
		m_aPlaceBonusTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PlaceBonusType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BonusType");
					m_aPlaceBonusTypes[i].eBonus = (BonusTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aPlaceBonusTypes[i].iProbability), L"iProbability");
					pXML->GetChildXmlValByName(&(m_aPlaceBonusTypes[i].bRequiresAccess), L"bRequiresAccess");
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aPlaceBonusTypes[i].ePrereqTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aPlaceBonusTypes[i].ePrereqFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"MapCategoryType");
					m_aPlaceBonusTypes[i].ePrereqMapCategory = (MapCategoryTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"TechType");
					m_aPlaceBonusTypes[i].ePrereqTech = (TechTypes)pXML->GetInfoClass(szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PlaceBonusType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvBuildInfo::copyNonDefaults(const CvBuildInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	CvInfoUtil(this).copyNonDefaults(pClassInfo);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getTime() == iDefault) m_iTime = pClassInfo->getTime();
	if (getCost() == iDefault) m_iCost = pClassInfo->getCost();
	if (isKill() == bDefault) m_bKill = pClassInfo->isKill();
	if (getImprovement() == iTextDefault) m_iImprovement = pClassInfo->getImprovement();
	if (getRoute() == iTextDefault) m_iRoute = pClassInfo->getRoute();
	if (getTerrainChange() == iTextDefault) m_iTerrainChange = pClassInfo->getTerrainChange();
	if (getFeatureChange() == iTextDefault) m_iFeatureChange = pClassInfo->getFeatureChange();
	if (getObsoleteTech() == iTextDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getEntityEvent() == iTextDefault) m_iEntityEvent = pClassInfo->getEntityEvent();

	for ( int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		if ( m_paiFeatureTech[i] == iTextDefault )
		{
			m_paiFeatureTech[i] = pClassInfo->getFeatureTech((FeatureTypes)i);
			m_paiFeatureTime[i] = pClassInfo->getFeatureTime((FeatureTypes)i);
			m_paiFeatureProduction[i] = pClassInfo->getFeatureProduction((FeatureTypes)i);
			m_pabFeatureRemove[i] = pClassInfo->isFeatureRemove((FeatureTypes)i);
		}
	}
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqBonusTypes, pClassInfo->m_aiPrereqBonusTypes);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aTerrainStructs, pClassInfo->m_aTerrainStructs);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aPlaceBonusTypes, pClassInfo->m_aPlaceBonusTypes);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

}

void CvBuildInfo::getCheckSum(uint32_t &iSum) const
{
	PROFILE_EXTRA_FUNC();
	CvInfoUtil(this).checkSum(iSum);

	CheckSum(iSum, m_iTime);
	CheckSum(iSum, m_iCost);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iImprovement);
	CheckSum(iSum, m_iRoute);

	CheckSum(iSum, m_iTerrainChange);
	CheckSum(iSum, m_iFeatureChange);
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_bDisabled);

	CheckSum(iSum, m_iEntityEvent);
	CheckSum(iSum, m_iMissionType);

	CheckSum(iSum, m_bKill);

	// Arrays

	CheckSum(iSum, m_paiFeatureTech,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_paiFeatureTime,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_paiFeatureProduction,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_pabFeatureRemove,  GC.getNumFeatureInfos());

	//Vectors

	CheckSumC(iSum, m_aiPrereqBonusTypes);
	CheckSumC(iSum, m_aeMapCategoryTypes);
	CheckSumC(iSum, m_aiCategories);

	int iNumElements = m_aTerrainStructs.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTerrainStructs[i].eTerrain);
		CheckSum(iSum, m_aTerrainStructs[i].ePrereqTech);
		CheckSum(iSum, m_aTerrainStructs[i].iTime);
	}

	iNumElements = m_aPlaceBonusTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPlaceBonusTypes[i].eBonus);
		CheckSum(iSum, m_aPlaceBonusTypes[i].iProbability);
		CheckSum(iSum, m_aPlaceBonusTypes[i].bRequiresAccess);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqTerrain);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqFeature);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqMapCategory);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqTech);
	}
}

void CvBuildInfo::doPostLoadCaching(uint32_t iThis)
{
}

//======================================================================================================
//					CvGoodyInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvGoodyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvGoodyInfo::CvGoodyInfo() :
m_iGold(0),
m_iGoldRand1(0),
m_iGoldRand2(0),
m_iMapOffset(0),
m_iMapRange(0),
m_iMapProb(0),
m_iExperience(0),
m_iHealing(0),
m_iDamagePrereq(0),
m_iBarbarianUnitProb(0),
m_iMinBarbarians(0),
m_iGoodyUnit(NO_UNIT),
m_iBarbarianUnit(NO_UNIT),
m_iEraType(NO_ERA),
m_iNotEraType(NO_ERA),
m_iResearch(0),
m_bTech(false),
m_bBad(false),
m_bNaval(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvGoodyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvGoodyInfo::~CvGoodyInfo()
{
}

int CvGoodyInfo::getGold() const
{
	return m_iGold;
}

int CvGoodyInfo::getGoldRand1() const
{
	return m_iGoldRand1;
}

int CvGoodyInfo::getGoldRand2() const
{
	return m_iGoldRand2;
}

int CvGoodyInfo::getMapOffset() const
{
	return m_iMapOffset;
}

int CvGoodyInfo::getMapRange() const
{
	return m_iMapRange;
}

int CvGoodyInfo::getMapProb() const
{
	return m_iMapProb;
}

int CvGoodyInfo::getExperience() const
{
	return m_iExperience;
}

int CvGoodyInfo::getHealing() const
{
	return m_iHealing;
}

int CvGoodyInfo::getDamagePrereq() const
{
	return m_iDamagePrereq;
}

int CvGoodyInfo::getBarbarianUnitProb() const
{
	return m_iBarbarianUnitProb;
}

int CvGoodyInfo::getMinBarbarians() const
{
	return m_iMinBarbarians;
}

int CvGoodyInfo::getGoodyUnit() const
{
	return m_iGoodyUnit;
}

int CvGoodyInfo::getBarbarianUnit() const
{
	return m_iBarbarianUnit;
}

int CvGoodyInfo::getEraType() const
{
	return m_iEraType;
}

int CvGoodyInfo::getNotEraType() const
{
	return m_iNotEraType;
}

int CvGoodyInfo::getResearch() const
{
	return m_iResearch;
}

bool CvGoodyInfo::isTech() const
{
	return m_bTech;
}

bool CvGoodyInfo::isBad() const
{
	return m_bBad;
}

bool CvGoodyInfo::isNaval() const
{
	return m_bNaval;
}

const char* CvGoodyInfo::getSound() const
{
	return m_szSound;
}

bool CvGoodyInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");
	pXML->GetOptionalChildXmlValByName(&m_iGold, L"iGold");
	pXML->GetOptionalChildXmlValByName(&m_iGoldRand1, L"iGoldRand1");
	pXML->GetOptionalChildXmlValByName(&m_iGoldRand2, L"iGoldRand2");
	pXML->GetOptionalChildXmlValByName(&m_iMapOffset, L"iMapOffset");
	pXML->GetOptionalChildXmlValByName(&m_iMapRange, L"iMapRange");
	pXML->GetOptionalChildXmlValByName(&m_iMapProb, L"iMapProb");
	pXML->GetOptionalChildXmlValByName(&m_iExperience, L"iExperience");
	pXML->GetOptionalChildXmlValByName(&m_iHealing, L"iHealing");
	pXML->GetOptionalChildXmlValByName(&m_iDamagePrereq, L"iDamagePrereq");
	pXML->GetOptionalChildXmlValByName(&m_iResearch, L"iResearch");
	pXML->GetOptionalChildXmlValByName(&m_bTech, L"bTech");
	pXML->GetOptionalChildXmlValByName(&m_bBad, L"bBad");
	pXML->GetOptionalChildXmlValByName(&m_bNaval, L"bNaval");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnit");
	m_iGoodyUnit = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BarbarianUnit");
	m_iBarbarianUnit = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EraType");
	m_iEraType = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"NotEraType");
	m_iNotEraType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iBarbarianUnitProb, L"iBarbarianUnitProb");
	pXML->GetOptionalChildXmlValByName(&m_iMinBarbarians, L"iMinBarbarians");

	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	return true;
}

void CvGoodyInfo::copyNonDefaults(const CvGoodyInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();
	if (getGold() == iDefault) m_iGold = pClassInfo->getGold();
	if (getGoldRand1() == iDefault) m_iGoldRand1 = pClassInfo->getGoldRand1();
	if (getGoldRand2() == iDefault) m_iGoldRand2 = pClassInfo->getGoldRand2();
	if (getMapOffset() == iDefault) m_iMapOffset = pClassInfo->getMapOffset();
	if (getMapRange() == iDefault) m_iMapRange = pClassInfo->getMapRange();
	if (getMapProb() == iDefault) m_iMapProb = pClassInfo->getMapProb();
	if (getExperience() == iDefault) m_iExperience = pClassInfo->getExperience();
	if (getHealing() == iDefault) m_iHealing = pClassInfo->getHealing();
	if (getDamagePrereq() == iDefault) m_iDamagePrereq = pClassInfo->getDamagePrereq();
	if (isTech() == bDefault) m_bTech = pClassInfo->isTech();
	if (isBad() == bDefault) m_bBad = pClassInfo->isBad();
	if (isNaval() == bDefault) m_bNaval = pClassInfo->isNaval();
	if (m_iGoodyUnit == iTextDefault) m_iGoodyUnit = pClassInfo->getGoodyUnit();
	if (m_iBarbarianUnit == iTextDefault) m_iBarbarianUnit = pClassInfo->getBarbarianUnit();
	if (getEraType() == iTextDefault) m_iEraType = pClassInfo->getEraType();
	if (getNotEraType() == iTextDefault) m_iNotEraType = pClassInfo->getNotEraType();
	if (getResearch() == iDefault) m_iResearch = pClassInfo->getResearch();
	if (getBarbarianUnitProb() == iDefault) m_iBarbarianUnitProb = pClassInfo->getBarbarianUnitProb();
	if (getMinBarbarians() == iDefault) m_iMinBarbarians = pClassInfo->getMinBarbarians();
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
}

void CvGoodyInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iGold);
	CheckSum(iSum, m_iGoldRand1);
	CheckSum(iSum, m_iGoldRand2);
	CheckSum(iSum, m_iMapOffset);
	CheckSum(iSum, m_iMapRange);
	CheckSum(iSum, m_iMapProb);
	CheckSum(iSum, m_iExperience);
	CheckSum(iSum, m_iHealing);
	CheckSum(iSum, m_iDamagePrereq);
	CheckSum(iSum, m_iBarbarianUnitProb);
	CheckSum(iSum, m_iMinBarbarians);
	CheckSum(iSum, m_iGoodyUnit);
	CheckSum(iSum, m_iBarbarianUnit);
	CheckSum(iSum, m_iEraType);
	CheckSum(iSum, m_iNotEraType);
	CheckSum(iSum, m_iResearch);
	CheckSum(iSum, m_bTech);
	CheckSum(iSum, m_bBad);
	CheckSum(iSum, m_bNaval);
	CheckSumC(iSum, m_aeMapCategoryTypes);
}

//======================================================================================================
//					CvRouteInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRouteInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRouteInfo::CvRouteInfo() :
m_iAdvancedStartCost(100),
m_iValue(0),
m_iMovementCost(0),
m_iFlatMovementCost(0),
m_bSeaTunnel(false),
m_iPrereqBonus(NO_BONUS),
m_piYieldChange(NULL),
m_piTechMovementChange(NULL),
m_PropertyManipulators()
{
	m_zobristValue = GC.getGame().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRouteInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRouteInfo::~CvRouteInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piTechMovementChange);
}

int CvRouteInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

int CvRouteInfo::getValue() const
{
	return m_iValue;
}

int CvRouteInfo::getMovementCost() const
{
	return m_iMovementCost;
}

int CvRouteInfo::getFlatMovementCost() const
{
	return m_iFlatMovementCost;
}

bool CvRouteInfo::isSeaTunnel() const
{
	return m_bSeaTunnel;
}

int CvRouteInfo::getPrereqBonus() const
{
	return m_iPrereqBonus;
}

int CvRouteInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvRouteInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}

int CvRouteInfo::getTechMovementChange(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), i);
	return m_piTechMovementChange ? m_piTechMovementChange[i] : 0;
}

int CvRouteInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvRouteInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvRouteInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

const std::vector<BonusTypes>& CvRouteInfo::getPrereqOrBonuses() const
{
	return m_piPrereqOrBonuses;
}

bool CvRouteInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	//shouldHaveType = true;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	if (m_szType.empty())
	{
		OutputDebugStringW(pXML->GetXmlTagName());
		OutputDebugString("\n");
		FErrorMsg("error");
	}
	//shouldHaveType = false;

	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCost, L"iAdvancedStartCost", 100);

	pXML->GetOptionalChildXmlValByName(&m_iValue, L"iValue");
	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iFlatMovementCost, L"iFlatMovement");
	pXML->GetOptionalChildXmlValByName(&m_bSeaTunnel, L"bSeaTunnel");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusType");
	m_iPrereqBonus = pXML->GetInfoClass(szTextVal);

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldChange);
		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	pXML->SetVariableListTagPair(&m_piTechMovementChange, L"TechMovementChanges", GC.getNumTechInfos());

	pXML->SetOptionalVector(&m_piPrereqOrBonuses, L"PrereqOrBonuses");

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvRouteInfo::copyNonDefaults(const CvRouteInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getAdvancedStartCost() == 100) m_iAdvancedStartCost = pClassInfo->getAdvancedStartCost();
	if (getValue() == iDefault) m_iValue = pClassInfo->getValue();
	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getFlatMovementCost() == iDefault) m_iFlatMovementCost = pClassInfo->getFlatMovementCost();
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (!isSeaTunnel()) m_bSeaTunnel = pClassInfo->isSeaTunnel();
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/
	if (getPrereqBonus() == iTextDefault) m_iPrereqBonus = pClassInfo->getPrereqBonus();

	for ( int i = 0;  i < NUM_YIELD_TYPES; i++)
	{
		if (getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
	}

	for ( int i = 0;  i < GC.getNumTechInfos(); i++)
	{
		if (getTechMovementChange(i) == iDefault && pClassInfo->getTechMovementChange(i) != iDefault)
		{
			if ( NULL == m_piTechMovementChange )
			{
				CvXMLLoadUtility::InitList(&m_piTechMovementChange,GC.getNumTechInfos(),iDefault);
			}
			m_piTechMovementChange[i] = pClassInfo->getTechMovementChange(i);
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_piPrereqOrBonuses, pClassInfo->getPrereqOrBonuses());

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

void CvRouteInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iAdvancedStartCost);

	CheckSum(iSum, m_iValue);
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iFlatMovementCost);

	CheckSum(iSum, m_bSeaTunnel);
	CheckSum(iSum, m_iPrereqBonus);

	// Arrays

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piTechMovementChange, GC.getNumTechInfos());
	CheckSumC(iSum, m_piPrereqOrBonuses);
	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}

//======================================================================================================
//					CvImprovementBonusInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvImprovementBonusInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementBonusInfo::CvImprovementBonusInfo() :
m_iDiscoverRand(0),
m_bBonusMakesValid(false),
m_bObsoleteBonusMakesValid(false),
m_bBonusTrade(false),
m_piYieldChange(NULL)
,m_iDepletionRand(0)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvImprovementBonusInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementBonusInfo::~CvImprovementBonusInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
}

int CvImprovementBonusInfo::getDiscoverRand() const
{
	return m_iDiscoverRand;
}

bool CvImprovementBonusInfo::isBonusMakesValid() const
{
	return m_bBonusMakesValid;
}

bool CvImprovementBonusInfo::isObsoleteBonusMakesValid() const
{
	return m_bObsoleteBonusMakesValid;
}

bool CvImprovementBonusInfo::isBonusTrade() const
{
	return m_bBonusTrade;
}

int CvImprovementBonusInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int CvImprovementBonusInfo::getDepletionRand() const
{
	return m_iDepletionRand;
}

void CvImprovementBonusInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iDiscoverRand);

	CheckSum(iSum, m_bBonusMakesValid);
	CheckSum(iSum, m_bObsoleteBonusMakesValid);
	CheckSum(iSum, m_bBonusTrade);

	CheckSum(iSum, m_iDepletionRand);

	// Arrays

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
}

//======================================================================================================
//					CvBonusClassInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBonusClassInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBonusClassInfo::CvBonusClassInfo() :
m_iUniqueRange(0)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBonusClassInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBonusClassInfo::~CvBonusClassInfo()
{
}

int CvBonusClassInfo::getUniqueRange() const
{
	return m_iUniqueRange;
}

bool CvBonusClassInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iUniqueRange, L"iUniqueRange");

	return true;
}

void CvBonusClassInfo::copyNonDefaults(const CvBonusClassInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getUniqueRange() == iDefault) m_iUniqueRange = pClassInfo->getUniqueRange();
}

void CvBonusClassInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iUniqueRange);
}

//======================================================================================================
//					CvBonusInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBonusInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------



//======================================================================================================
//					CvFeatureInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvFeatureInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvFeatureInfo::CvFeatureInfo() :
m_iMovementCost(0),
m_iSeeThroughChange(0),
m_iHealthPercent(0),
m_iAppearanceProbability(0),
m_iDisappearanceProbability(0),
m_iGrowthProbability(0),
m_iDefenseModifier(0),
m_iAdvancedStartRemoveCost(0),
m_iTurnDamage(0),
m_iWarmingDefense(0), //GWMod new xml field M.A.
m_bNoCoast(false),
m_bNoRiver(false),
m_bNoAdjacent(false),
m_bRequiresFlatlands(false),
m_bRequiresRiver(false),
m_bAddsFreshWater(false),
m_bImpassable(false),
m_bNoCity(false),
m_bNoImprovement(false),
m_bNoBonus(false),
m_bVisibleAlways(false),
m_bNukeImmune(false),
m_bCountsAsPeak(false),
m_iWorldSoundscapeScriptId(0),
m_iEffectProbability(0),
m_piYieldChange(NULL),
m_piRiverYieldChange(NULL),
m_pi3DAudioScriptFootstepIndex(NULL),
m_pbTerrain(NULL)
,m_iSpreadProbability(0)
,m_iCultureDistance(0)
,m_iPopDestroys(-1)
,m_bIgnoreTerrainCulture(false)
,m_bCanGrowAnywhere(false)
,m_PropertyManipulators()
{
	m_zobristValue = GC.getGame().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvFeatureInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvFeatureInfo::~CvFeatureInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverYieldChange);
	SAFE_DELETE_ARRAY(m_pi3DAudioScriptFootstepIndex);
	SAFE_DELETE_ARRAY(m_pbTerrain);

}

int CvFeatureInfo::getMovementCost() const
{
	return m_iMovementCost;
}

int CvFeatureInfo::getSeeThroughChange() const
{
	return m_iSeeThroughChange;
}

int CvFeatureInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}

int CvFeatureInfo::getAppearanceProbability() const
{
	return m_iAppearanceProbability;
}

int CvFeatureInfo::getDisappearanceProbability() const
{
	return m_iDisappearanceProbability;
}

int CvFeatureInfo::getGrowthProbability() const
{
	return m_iGrowthProbability;
}

int CvFeatureInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}

int CvFeatureInfo::getAdvancedStartRemoveCost() const
{
	return m_iAdvancedStartRemoveCost;
}

int CvFeatureInfo::getTurnDamage() const
{
	return m_iTurnDamage;
}

int CvFeatureInfo::getWarmingDefense() const			//GWMod new xml field M.A.
{
	return m_iWarmingDefense;
}

int CvFeatureInfo::getPopDestroys() const			//GWMod new xml field M.A.
{
	return m_iPopDestroys;
}

bool CvFeatureInfo::isNoCoast() const
{
	return m_bNoCoast;
}

bool CvFeatureInfo::isNoRiver() const
{
	return m_bNoRiver;
}

bool CvFeatureInfo::isNoAdjacent() const
{
	return m_bNoAdjacent;
}

bool CvFeatureInfo::isRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}

bool CvFeatureInfo::isRequiresRiver() const
{
	return m_bRequiresRiver;
}

bool CvFeatureInfo::isAddsFreshWater() const
{
	return m_bAddsFreshWater;
}

bool CvFeatureInfo::isImpassable() const
{
	return m_bImpassable;
}

bool CvFeatureInfo::isNoCity() const
{
	return m_bNoCity;
}

bool CvFeatureInfo::isNoImprovement() const
{
	return m_bNoImprovement;
}

bool CvFeatureInfo::isVisibleAlways() const
{
	return m_bVisibleAlways;
}

bool CvFeatureInfo::isNukeImmune() const
{
	return m_bNukeImmune;
}

bool CvFeatureInfo::isCountsAsPeak() const
{
	return m_bCountsAsPeak;
}

// BUG - City Plot Status - start
bool CvFeatureInfo::isOnlyBad() const
{
	PROFILE_EXTRA_FUNC();
	if (getHealthPercent() > 0 || isAddsFreshWater())
	{
		return false;
	}
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (getYieldChange(iI) > 0)
		{
			return false;
		}
	}

	return true;
}
// BUG - City Plot Status - end

const char* CvFeatureInfo::getOnUnitChangeTo() const
{
	return m_szOnUnitChangeTo;
}

const char* CvFeatureInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

int CvFeatureInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}

const char* CvFeatureInfo::getEffectType() const
{
	return m_szEffectType;
}

int CvFeatureInfo::getEffectProbability() const
{
	return m_iEffectProbability;
}

// Arrays

int CvFeatureInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int CvFeatureInfo::getRiverYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piRiverYieldChange ? m_piRiverYieldChange[i] : 0;
}

int CvFeatureInfo::get3DAudioScriptFootstepIndex(int i) const
{
	//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pi3DAudioScriptFootstepIndex ? m_pi3DAudioScriptFootstepIndex[i] : -1;
}

bool CvFeatureInfo::isTerrain(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrain ? m_pbTerrain[i] : false;
}

int CvFeatureInfo::getNumVarieties() const
{
	return getArtInfo()->getNumVarieties();
}

const char* CvFeatureInfo::getButton() const
{
	const CvArtInfoFeature* pFeatureArtInfo = getArtInfo();
	return pFeatureArtInfo ? pFeatureArtInfo->getButton() : NULL;
}

const CvArtInfoFeature* CvFeatureInfo::getArtInfo() const
{
	return ARTFILEMGR.getFeatureArtInfo( getArtDefineTag());
}

int CvFeatureInfo::getSpreadProbability() const
{
	return m_iSpreadProbability;
}

int CvFeatureInfo::getCultureDistance() const
{
	return m_iCultureDistance;
}

const char* CvFeatureInfo::getGrowthSound() const
{
	return m_szGrowthSound;
}

bool CvFeatureInfo::isIgnoreTerrainCulture() const
{
	return m_bIgnoreTerrainCulture;
}

bool CvFeatureInfo::isCanGrowAnywhere() const
{
	return m_bCanGrowAnywhere;
}

// AIAndy: Returns true if the feature can be displayed properly as secondary feature
bool CvFeatureInfo::canBeSecondary() const
{
	return !(getArtInfo()->isRiverArt() || (getArtInfo()->getTileArtType() != TILE_ART_TYPE_NONE));
}
#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvFeatureInfo::getNumAfflictionCommunicabilityTypes() const
{
	return (int)m_aAfflictionCommunicabilityTypes.size();
}
#endif
int CvFeatureInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvFeatureInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvFeatureInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}
#ifdef OUTBREAKS_AND_AFFLICTIONS
PromotionLineAfflictionModifier CvFeatureInfo::getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume)
{
	FASSERT_BOUNDS(0, getNumAfflictionCommunicabilityTypes(), iPromotionLine);

	if ((bWorkedTile && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bWorkedTile) ||
		(bVicinity && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bVicinity) ||
		(bAccessVolume && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bAccessVolume))
	{
		PromotionLineAfflictionModifier kMod = m_aAfflictionCommunicabilityTypes[iPromotionLine];
		kMod.iModifier = 0;
		return kMod;
	}
	return m_aAfflictionCommunicabilityTypes[iPromotionLine];
}
#endif

bool CvFeatureInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piYieldChange);

	if (pXML->TryMoveToXmlFirstChild(L"RiverYieldChange"))
	{
		pXML->SetYields(&m_piRiverYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piRiverYieldChange);

	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iSeeThroughChange, L"iSeeThrough");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefense");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartRemoveCost, L"iAdvancedStartRemoveCost");
	pXML->GetOptionalChildXmlValByName(&m_iTurnDamage, L"iTurnDamage");
	pXML->GetOptionalChildXmlValByName(&m_iWarmingDefense, L"iWarmingDefense"); //GWMod new xml field M.A.
	pXML->GetOptionalChildXmlValByName(&m_iAppearanceProbability, L"iAppearance");
	pXML->GetOptionalChildXmlValByName(&m_iDisappearanceProbability, L"iDisappearance");
	pXML->GetOptionalChildXmlValByName(&m_iGrowthProbability, L"iGrowth");
	pXML->GetOptionalChildXmlValByName(&m_iPopDestroys, L"iPopDestroys", -1);
	pXML->GetOptionalChildXmlValByName(&m_bNoCoast, L"bNoCoast");
	pXML->GetOptionalChildXmlValByName(&m_bNoRiver, L"bNoRiver");
	pXML->GetOptionalChildXmlValByName(&m_bNoAdjacent, L"bNoAdjacent");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresFlatlands, L"bRequiresFlatlands");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresRiver, L"bRequiresRiver");
	pXML->GetOptionalChildXmlValByName(&m_bAddsFreshWater, L"bAddsFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bImpassable, L"bImpassable");
	pXML->GetOptionalChildXmlValByName(&m_bNoCity, L"bNoCity");
	pXML->GetOptionalChildXmlValByName(&m_bNoImprovement, L"bNoImprovement");
	pXML->GetOptionalChildXmlValByName(&m_bNoBonus, L"bNoBonus");
	pXML->GetOptionalChildXmlValByName(&m_bVisibleAlways, L"bVisibleAlways");
	pXML->GetOptionalChildXmlValByName(&m_bNukeImmune, L"bNukeImmune");
	pXML->GetOptionalChildXmlValByName(&m_bCountsAsPeak, L"bCountsAsPeak");
	pXML->GetOptionalChildXmlValByName(m_szOnUnitChangeTo, L"OnUnitChangeTo");

	pXML->SetVariableListTagPairForAudioScripts(&m_pi3DAudioScriptFootstepIndex, L"FootstepSounds", GC.getNumFootstepAudioTypes());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WorldSoundscapeAudioScript");
	if ( szTextVal.GetLength() > 0 )
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;

	pXML->GetOptionalChildXmlValByName(m_szEffectType, L"EffectType");
	pXML->GetOptionalChildXmlValByName(&m_iEffectProbability, L"iEffectProbability");

	pXML->SetVariableListTagPair(&m_pbTerrain, L"TerrainBooleans", GC.getNumTerrainInfos());

	pXML->GetOptionalChildXmlValByName(&m_iSpreadProbability, L"iSpread");
	pXML->GetOptionalChildXmlValByName(&m_iCultureDistance, L"iCultureDistance");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreTerrainCulture, L"bIgnoreTerrainCulture");
	pXML->GetOptionalChildXmlValByName(&m_bCanGrowAnywhere, L"bCanGrowAnywhere");
	pXML->GetOptionalChildXmlValByName(m_szGrowthSound, L"GrowthSound");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");
#ifdef OUTBREAKS_AND_AFFLICTIONS
	if(pXML->TryMoveToXmlFirstChild(L"AfflictionCommunicabilityTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictionCommunicabilityType" );
		m_aAfflictionCommunicabilityTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionCommunicabilityType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionCommunicabilityTypes[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].iModifier), L"iCommunicability");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bWorkedTile), L"bWorkedTile");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bVicinity), L"bVicinity");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bAccessVolume), L"bAccessVolume");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PromotionLineType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
#endif

	m_PropertyManipulators.read(pXML);
	return true;
}

void CvFeatureInfo::copyNonDefaults(const CvFeatureInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	if (getArtDefineTag() == cDefault) m_szArtDefineTag = pClassInfo->getArtDefineTag();

	CvInfoBase::copyNonDefaults(pClassInfo);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
		if ( getRiverYieldChange(i) == iDefault && pClassInfo->getRiverYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piRiverYieldChange, NUM_YIELD_TYPES);
			}
			m_piRiverYieldChange[i] = pClassInfo->getRiverYieldChange(i);
		}
	}

	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getSeeThroughChange() == iDefault) m_iSeeThroughChange = pClassInfo->getSeeThroughChange();
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();
	if (getAdvancedStartRemoveCost() == iDefault) m_iAdvancedStartRemoveCost = pClassInfo->getAdvancedStartRemoveCost();
	if (getTurnDamage() == iDefault) m_iTurnDamage = pClassInfo->getTurnDamage();
	if (getAppearanceProbability() == iDefault) m_iAppearanceProbability = pClassInfo->getAppearanceProbability();
	if (getDisappearanceProbability() == iDefault) m_iDisappearanceProbability = pClassInfo->getDisappearanceProbability();
	if (getGrowthProbability() == iDefault) m_iGrowthProbability = pClassInfo->getGrowthProbability();
	if (getPopDestroys() == -1) m_iPopDestroys = pClassInfo->getPopDestroys();
	if (isNoCoast() == bDefault) m_bNoCoast = pClassInfo->isNoCoast();
	if (isNoRiver() == bDefault) m_bNoRiver = pClassInfo->isNoRiver();
	if (isNoAdjacent() == bDefault) m_bNoAdjacent = pClassInfo->isNoAdjacent();
	if (isRequiresFlatlands() == bDefault) m_bRequiresFlatlands = pClassInfo->isRequiresFlatlands();
	if (isRequiresRiver() == bDefault) m_bRequiresRiver = pClassInfo->isRequiresRiver();
	if (isAddsFreshWater() == bDefault) m_bAddsFreshWater = pClassInfo->isAddsFreshWater();
	if (isImpassable() == bDefault) m_bImpassable = pClassInfo->isImpassable();
	if (isNoCity() == bDefault) m_bNoCity = pClassInfo->isNoCity();
	if (isNoImprovement() == bDefault) m_bNoImprovement = pClassInfo->isNoImprovement();
	if (m_bNoBonus == bDefault) m_bNoBonus = pClassInfo->isNoBonus();
	if (isVisibleAlways() == bDefault) m_bVisibleAlways = pClassInfo->isVisibleAlways();
	if (isNukeImmune() == bDefault) m_bNukeImmune = pClassInfo->isNukeImmune();
	if (isCountsAsPeak() == bDefault) m_bCountsAsPeak = pClassInfo->isCountsAsPeak();

	if (getOnUnitChangeTo() == cDefault) m_szOnUnitChangeTo = pClassInfo->getOnUnitChangeTo();

	for ( int i = 0; i < GC.getNumFootstepAudioTypes(); i++ )
	{
		if ( get3DAudioScriptFootstepIndex(i) == iDefault && pClassInfo->get3DAudioScriptFootstepIndex(i) != iDefault)
		{
			if ( NULL == m_pi3DAudioScriptFootstepIndex )
			{
				CvXMLLoadUtility::InitList(&m_pi3DAudioScriptFootstepIndex,GC.getNumFootstepAudioTypes(),iDefault);
			}
			m_pi3DAudioScriptFootstepIndex[i] = pClassInfo->get3DAudioScriptFootstepIndex(i);
		}
	}
	if (getWorldSoundscapeScriptId() == iTextDefault) m_iWorldSoundscapeScriptId = pClassInfo->getWorldSoundscapeScriptId();

	if (getEffectType() == cDefault) m_szEffectType = pClassInfo->getEffectType();
	if (getEffectProbability() == iDefault) m_iEffectProbability = pClassInfo->getEffectProbability();

	for ( int i = 0; i < GC.getNumTerrainInfos(); i++ )
	{
		if ( isTerrain(i) == bDefault && pClassInfo->isTerrain(i) != bDefault )
		{
			if ( m_pbTerrain == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrain, GC.getNumTerrainInfos());
			}

			m_pbTerrain[i] = pClassInfo->isTerrain(i);
		}
	}

	if (getSpreadProbability() == iDefault) m_iSpreadProbability = pClassInfo->getSpreadProbability();
	if (getCultureDistance() == iDefault) m_iCultureDistance = pClassInfo->getCultureDistance();
	if (!isIgnoreTerrainCulture()) m_bIgnoreTerrainCulture = pClassInfo->isIgnoreTerrainCulture();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (getNumAfflictionCommunicabilityTypes() == 0)
	{
		const int iNum = pClassInfo->getNumAfflictionCommunicabilityTypes();
		m_aAfflictionCommunicabilityTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aAfflictionCommunicabilityTypes[i].ePromotionLine = pClassInfo->m_aAfflictionCommunicabilityTypes[i].ePromotionLine;
			m_aAfflictionCommunicabilityTypes[i].iModifier = pClassInfo->m_aAfflictionCommunicabilityTypes[i].iModifier;
			m_aAfflictionCommunicabilityTypes[i].bAccessVolume = pClassInfo->m_aAfflictionCommunicabilityTypes[i].bAccessVolume;
			m_aAfflictionCommunicabilityTypes[i].bVicinity = pClassInfo->m_aAfflictionCommunicabilityTypes[i].bVicinity;
			m_aAfflictionCommunicabilityTypes[i].bWorkedTile = pClassInfo->m_aAfflictionCommunicabilityTypes[i].bWorkedTile;
		}
	}
#endif
	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

void CvFeatureInfo::getCheckSum(uint32_t &iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iSpreadProbability);
	CheckSum(iSum, m_iCultureDistance);
	CheckSum(iSum, m_bIgnoreTerrainCulture);
	CheckSum(iSum, m_bCanGrowAnywhere);
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iSeeThroughChange);
	CheckSum(iSum, m_iHealthPercent);
	CheckSum(iSum, m_iAppearanceProbability);
	CheckSum(iSum, m_iDisappearanceProbability);
	CheckSum(iSum, m_iGrowthProbability);
	CheckSum(iSum, m_iDefenseModifier);
	CheckSum(iSum, m_iAdvancedStartRemoveCost);
	CheckSum(iSum, m_iTurnDamage);
	CheckSum(iSum, m_iWarmingDefense);
	CheckSum(iSum, m_iPopDestroys);

	CheckSum(iSum, m_bNoCoast);
	CheckSum(iSum, m_bNoRiver);
	CheckSum(iSum, m_bNoAdjacent);
	CheckSum(iSum, m_bRequiresFlatlands);
	CheckSum(iSum, m_bRequiresRiver);
	CheckSum(iSum, m_bAddsFreshWater);
	CheckSum(iSum, m_bImpassable);
	CheckSum(iSum, m_bNoCity);
	CheckSum(iSum, m_bNoImprovement);
	CheckSum(iSum, m_bNoBonus);
	CheckSum(iSum, m_bVisibleAlways);
	CheckSum(iSum, m_bNukeImmune);
	CheckSum(iSum, m_bCountsAsPeak);
	CheckSumC(iSum, m_szOnUnitChangeTo);
	CheckSumC(iSum, m_aeMapCategoryTypes);
	CheckSumC(iSum, m_aiCategories);

	// Arrays

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piRiverYieldChange, NUM_YIELD_TYPES);

	CheckSum(iSum, m_pbTerrain, GC.getNumTerrainInfos());
#ifdef OUTBREAKS_AND_AFFLICTIONS
	const int iNumElements = m_aAfflictionCommunicabilityTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].iModifier);
	}

#endif
	m_PropertyManipulators.getCheckSum(iSum);
}

//======================================================================================================
//					CvCommerceInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCommerceInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCommerceInfo::CvCommerceInfo() :
m_iChar(0),
m_iInitialPercent(0),
m_iInitialHappiness(0),
m_iAIWeightPercent(0),
m_bFlexiblePercent(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCommerceInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCommerceInfo::~CvCommerceInfo()
{
}

int CvCommerceInfo::getChar() const
{
	return m_iChar;
}

void CvCommerceInfo::setChar(int i)
{
	m_iChar = i;
}

int CvCommerceInfo::getInitialPercent() const
{
	return m_iInitialPercent;
}

int CvCommerceInfo::getInitialHappiness() const
{
	return m_iInitialHappiness;
}

int CvCommerceInfo::getAIWeightPercent() const
{
	return m_iAIWeightPercent;
}

bool CvCommerceInfo::isFlexiblePercent() const
{
	return m_bFlexiblePercent;
}

bool CvCommerceInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iInitialPercent, L"iInitialPercent");
	pXML->GetChildXmlValByName(&m_iInitialHappiness, L"iInitialHappiness");
	pXML->GetChildXmlValByName(&m_iAIWeightPercent, L"iAIWeightPercent");
	pXML->GetChildXmlValByName(&m_bFlexiblePercent, L"bFlexiblePercent");

	return true;
}

void CvCommerceInfo::copyNonDefaults(const CvCommerceInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getInitialPercent() == iDefault) m_iInitialPercent = pClassInfo->getInitialPercent();
	if (getInitialHappiness() == iDefault) m_iInitialHappiness = pClassInfo->getInitialHappiness();
	if (getAIWeightPercent() == iDefault) m_iAIWeightPercent = pClassInfo->getAIWeightPercent();
	if (isFlexiblePercent() == bDefault) m_bFlexiblePercent = pClassInfo->isFlexiblePercent();

}

void CvCommerceInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iInitialPercent);
	CheckSum(iSum, m_iInitialHappiness);
	CheckSum(iSum, m_iAIWeightPercent);

	CheckSum(iSum, m_bFlexiblePercent);
}

//======================================================================================================
//					CvYieldInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvYieldInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvYieldInfo::CvYieldInfo() :
m_iChar(0),
m_iHillsChange(0),
m_iPeakChange(0),
m_iRiverChange(0),
m_iCityChange(0),
m_iPopulationChangeDivisor(0),
m_iMinCity(0),
m_iTradeModifier(0),
m_iGoldenAgeYield(0),
m_iGoldenAgeYieldThreshold(0),
m_iAIWeightPercent(0),
m_iColorType(NO_COLOR),
m_paszSymbolPath(NULL)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvYieldInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvYieldInfo::~CvYieldInfo()
{
	SAFE_DELETE_ARRAY(m_paszSymbolPath);
}

int CvYieldInfo::getChar() const
{
	return m_iChar;
}

void CvYieldInfo::setChar(int i)
{
	m_iChar = i;
}

int CvYieldInfo::getHillsChange() const
{
	return m_iHillsChange;
}

int CvYieldInfo::getPeakChange() const
{
	return m_iPeakChange;
}

int CvYieldInfo::getRiverChange() const
{
	return m_iRiverChange;
}

int CvYieldInfo::getCityChange() const
{
	return m_iCityChange;
}

int CvYieldInfo::getPopulationChangeDivisor() const
{
	return m_iPopulationChangeDivisor;
}

int CvYieldInfo::getMinCity() const
{
	return m_iMinCity;
}

int CvYieldInfo::getTradeModifier() const
{
	return m_iTradeModifier;
}

int CvYieldInfo::getGoldenAgeYield() const
{
	return m_iGoldenAgeYield;
}

int CvYieldInfo::getGoldenAgeYieldThreshold() const
{
	return m_iGoldenAgeYieldThreshold;
}

int CvYieldInfo::getAIWeightPercent() const
{
	return m_iAIWeightPercent;
}

int CvYieldInfo::getColorType() const
{
	return m_iColorType;
}

// Arrays

const char* CvYieldInfo::getSymbolPath(int i) const
{
	FASSERT_BOUNDS(0, GC.getDefineINT("MAX_YIELD_STACK"), i);
	return m_paszSymbolPath ? m_paszSymbolPath[i] : reinterpret_cast<const char*>(-1);
}

bool CvYieldInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int iNumSibs, j;

	pXML->GetOptionalChildXmlValByName(&m_iHillsChange, L"iHillsChange");
	pXML->GetOptionalChildXmlValByName(&m_iPeakChange, L"iPeakChange");
	pXML->GetOptionalChildXmlValByName(&m_iRiverChange, L"iRiverChange");
	pXML->GetOptionalChildXmlValByName(&m_iCityChange, L"iCityChange");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationChangeDivisor, L"iPopulationChangeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iMinCity, L"iMinCity");
	pXML->GetOptionalChildXmlValByName(&m_iTradeModifier, L"iTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeYield, L"iGoldenAgeYield");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeYieldThreshold, L"iGoldenAgeYieldThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iAIWeightPercent, L"iAIWeightPercent");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ColorType");
	m_iColorType = pXML->GetInfoClass(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"SymbolPaths"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getDefineINT("MAX_YIELD_STACK")) ,"Allocating zero or less memory in SetGlobalYieldInfo");
		m_paszSymbolPath = new CvString[GC.getDefineINT("MAX_YIELD_STACK")];

		if (0 < iNumSibs)
		{
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumSibs <= GC.getDefineINT("MAX_YIELD_STACK")) ,"There are more siblings than memory allocated for them in SetGlobalYieldInfo");
				for (j=0;j<iNumSibs;j++)
				{
					m_paszSymbolPath[j] = szTextVal;
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		CvString cDefault = CvString::format("").GetCString();
		m_paszSymbolPath = new CvString[GC.getDefineINT("MAX_YIELD_STACK")];
		for ( int i = 0; i < GC.getDefineINT("MAX_YIELD_STACK"); i++)
		{
			m_paszSymbolPath[i] = cDefault;
		}
	}

	return true;
}

void CvYieldInfo::copyNonDefaults(const CvYieldInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getHillsChange() == iDefault) m_iHillsChange = pClassInfo->getHillsChange();
	if (getPeakChange() == iDefault) m_iPeakChange = pClassInfo->getPeakChange();
	if (m_iRiverChange == iDefault) m_iRiverChange = pClassInfo->getRiverChange();
	if (getCityChange() == iDefault) m_iCityChange = pClassInfo->getCityChange();
	if (getPopulationChangeDivisor() == iDefault) m_iPopulationChangeDivisor = pClassInfo->getPopulationChangeDivisor();
	if (getMinCity() == iDefault) m_iMinCity = pClassInfo->getMinCity();
	if (getTradeModifier() == iDefault) m_iTradeModifier = pClassInfo->getTradeModifier();
	if (getGoldenAgeYield() == iDefault) m_iGoldenAgeYield = pClassInfo->getGoldenAgeYield();
	if (getGoldenAgeYieldThreshold() == iDefault) m_iGoldenAgeYieldThreshold = pClassInfo->getGoldenAgeYieldThreshold();
	if (getAIWeightPercent() == iDefault) m_iAIWeightPercent = pClassInfo->getAIWeightPercent();
	if (getColorType() == iTextDefault) m_iColorType = pClassInfo->getColorType();

	for ( int i = 0; i < GC.getDefineINT("MAX_YIELD_STACK"); i++)
	{
		if ( m_paszSymbolPath[i] == cDefault)
		{
			m_paszSymbolPath[i] = pClassInfo->getSymbolPath(i);
		}
	}
}

void CvYieldInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iHillsChange);
	CheckSum(iSum, m_iPeakChange);
	CheckSum(iSum, m_iRiverChange);
	CheckSum(iSum, m_iCityChange);
	CheckSum(iSum, m_iPopulationChangeDivisor);
	CheckSum(iSum, m_iMinCity);
	CheckSum(iSum, m_iTradeModifier);
	CheckSum(iSum, m_iGoldenAgeYield);
	CheckSum(iSum, m_iGoldenAgeYieldThreshold);
	CheckSum(iSum, m_iAIWeightPercent);
}

//======================================================================================================
//					CvTerrainInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTerrainInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainInfo::CvTerrainInfo() :
m_iMovementCost(0),
m_iBuildModifier(0),
m_iDefenseModifier(0),
m_bWaterTerrain(false),
m_bImpassable(false),
m_bFound(false),
m_bFoundCoast(false),
m_bFoundFreshWater(false),
m_bFreshWaterTerrain(false),
m_iWorldSoundscapeScriptId(0),
m_piYields(NULL),
m_pi3DAudioScriptFootstepIndex(NULL)
,m_iCultureDistance(0)
,m_iHealthPercent(0)
,m_PropertyManipulators(),
//TB Combat Mod begin
m_bColdDamage(false)
//TB Combat Mod end
{
	m_zobristValue = GC.getGame().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTerrainInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainInfo::~CvTerrainInfo()
{
	SAFE_DELETE_ARRAY(m_piYields);
	SAFE_DELETE_ARRAY(m_pi3DAudioScriptFootstepIndex);

}

int CvTerrainInfo::getMovementCost() const
{
	return m_iMovementCost;
}

int CvTerrainInfo::getBuildModifier() const
{
	return m_iBuildModifier;
}

int CvTerrainInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}

bool CvTerrainInfo::isWaterTerrain() const
{
	return m_bWaterTerrain;
}

bool CvTerrainInfo::isImpassable() const
{
	return m_bImpassable;
}

bool CvTerrainInfo::isFound() const
{
	return m_bFound;
}

bool CvTerrainInfo::isFoundCoast() const
{
	return m_bFoundCoast;
}

bool CvTerrainInfo::isFoundFreshWater() const
{
	return m_bFoundFreshWater;
}

bool CvTerrainInfo::isFreshWaterTerrain() const
{
	return m_bFreshWaterTerrain;
}

const char* CvTerrainInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

int CvTerrainInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}

// Arrays

int CvTerrainInfo::getYield(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYields ? m_piYields[i] : 0;
}

int CvTerrainInfo::get3DAudioScriptFootstepIndex(int i) const
{
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pi3DAudioScriptFootstepIndex ? m_pi3DAudioScriptFootstepIndex[i] : 0;
}

int CvTerrainInfo::getCultureDistance() const
{
	return m_iCultureDistance;
}
int CvTerrainInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}

//TB Combat Mod begin
bool CvTerrainInfo::isColdDamage() const
{
	return m_bColdDamage;
}
//TB Combat Mod end


int CvTerrainInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvTerrainInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvTerrainInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvTerrainInfo::getNumAfflictionCommunicabilityTypes() const
{
	return (int)m_aAfflictionCommunicabilityTypes.size();
}

PromotionLineAfflictionModifier CvTerrainInfo::getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume)
{
	FASSERT_BOUNDS(0, getNumAfflictionCommunicabilityTypes(), iPromotionLine);

	if ((bWorkedTile && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bWorkedTile) ||
		(bVicinity && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bVicinity) ||
		(bAccessVolume && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bAccessVolume))
	{
		PromotionLineAfflictionModifier kMod = m_aAfflictionCommunicabilityTypes[iPromotionLine];
		kMod.iModifier = 0;
		return kMod;
	}

	return m_aAfflictionCommunicabilityTypes[iPromotionLine];
}
#endif // OUTBREAKS_AND_AFFLICTIONS

bool CvTerrainInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");

	if (pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		pXML->SetYields(&m_piYields);
		pXML->MoveToXmlParent();
	}
	else SAFE_DELETE_ARRAY(m_piYields);

	pXML->GetOptionalChildXmlValByName(&m_bWaterTerrain, L"bWaterTerrain");
	pXML->GetOptionalChildXmlValByName(&m_bImpassable, L"bImpassable");
	pXML->GetOptionalChildXmlValByName(&m_bFound, L"bFound");
	pXML->GetOptionalChildXmlValByName(&m_bFoundCoast, L"bFoundCoast");
	pXML->GetOptionalChildXmlValByName(&m_bFoundFreshWater, L"bFoundFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bFreshWaterTerrain, L"bFreshWaterTerrain");

	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iBuildModifier, L"iBuildModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefense");

	pXML->SetVariableListTagPairForAudioScripts(&m_pi3DAudioScriptFootstepIndex, L"FootstepSounds", GC.getNumFootstepAudioTypes());

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	if (pXML->GetOptionalChildXmlValByName(szTextVal, L"WorldSoundscapeAudioScript"))
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;

	pXML->GetOptionalChildXmlValByName(&m_iCultureDistance, L"iCultureDistance");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");

	m_PropertyManipulators.read(pXML);
	//TB Combat Mods begin
	pXML->GetOptionalChildXmlValByName(&m_bColdDamage, L"bColdDamage");
	//TB Combat Mods end
#ifdef OUTBREAKS_AND_AFFLICTIONS
	if(pXML->TryMoveToXmlFirstChild(L"AfflictionCommunicabilityTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictionCommunicabilityType" );
		m_aAfflictionCommunicabilityTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionCommunicabilityType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionCommunicabilityTypes[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].iModifier), L"iCommunicability");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bWorkedTile), L"bWorkedTile");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bVicinity), L"bVicinity");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bAccessVolume), L"bAccessVolume");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PromotionLineType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

#endif
	return true;
}

void CvTerrainInfo::copyNonDefaults(const CvTerrainInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	if (getArtDefineTag() == cDefault) m_szArtDefineTag = pClassInfo->getArtDefineTag();

	CvInfoBase::copyNonDefaults(pClassInfo);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (!m_piYields)
		{
			if (pClassInfo->getYield(i))
			{
				CvXMLLoadUtility::InitList(&m_piYields, NUM_YIELD_TYPES);
			}
		}

		if (m_piYields && (m_piYields[i] == iDefault))
		{
			m_piYields[i] = pClassInfo->getYield(i);
		}
	}
	if (isWaterTerrain() == bDefault) m_bWaterTerrain = pClassInfo->isWaterTerrain();
	if (isImpassable() == bDefault) m_bImpassable = pClassInfo->isImpassable();
	if (isFound() == bDefault) m_bFound = pClassInfo->isFound();
	if (isFoundCoast() == bDefault) m_bFoundCoast = pClassInfo->isFoundCoast();
	if (isFoundFreshWater() == bDefault) m_bFoundFreshWater = pClassInfo->isFoundFreshWater();
	if (isFreshWaterTerrain() == bDefault) m_bFreshWaterTerrain = pClassInfo->isFreshWaterTerrain();
	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getBuildModifier() == iDefault) m_iBuildModifier = pClassInfo->getBuildModifier();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();

	for ( int i = 0; i < GC.getNumFootstepAudioTypes(); i++)
	{
		if (get3DAudioScriptFootstepIndex(i) == -1 && pClassInfo->get3DAudioScriptFootstepIndex(i) != -1)
		{
			if ( NULL == m_pi3DAudioScriptFootstepIndex )
			{
				CvXMLLoadUtility::InitList(&m_pi3DAudioScriptFootstepIndex,GC.getNumFootstepAudioTypes(),-1);
			}
			m_pi3DAudioScriptFootstepIndex[i] = pClassInfo->get3DAudioScriptFootstepIndex(i);
		}
	}

	if (getWorldSoundscapeScriptId() == iTextDefault) m_iWorldSoundscapeScriptId = pClassInfo->getWorldSoundscapeScriptId();

	if (getCultureDistance() == iDefault) m_iCultureDistance = pClassInfo->getCultureDistance();
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);

	//TB Combat Mods begin
	if (isColdDamage() == bDefault) m_bColdDamage = pClassInfo->isColdDamage();
	//TB Combat Mods end
}


void CvTerrainInfo::getCheckSum(uint32_t &iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iBuildModifier);
	CheckSum(iSum, m_iDefenseModifier);

	CheckSum(iSum, m_bWaterTerrain);
	CheckSum(iSum, m_bImpassable);
	CheckSum(iSum, m_bFound);
	CheckSum(iSum, m_bFoundCoast);
	CheckSum(iSum, m_bFoundFreshWater);
	CheckSum(iSum, m_bFreshWaterTerrain);

	// Arrays

	CheckSum(iSum, m_piYields, NUM_YIELD_TYPES);

	CheckSum(iSum, m_iCultureDistance);
	CheckSum(iSum, m_iHealthPercent);

	m_PropertyManipulators.getCheckSum(iSum);
	//TB Combat Mods begin
	CheckSum(iSum, m_bColdDamage);
	CheckSumC(iSum, m_aiCategories);
	CheckSumC(iSum, m_aeMapCategoryTypes);

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int iNumElements = m_aAfflictionCommunicabilityTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].iModifier);
	}
#endif // OUTBREAKS_AND_AFFLICTIONS
	//TB Combat Mods end
}

const char* CvTerrainInfo::getButton() const
{
	const CvArtInfoTerrain* pTerrainArtInfo = getArtInfo();
	return pTerrainArtInfo ? pTerrainArtInfo->getButton() : NULL;
}

const CvArtInfoTerrain* CvTerrainInfo::getArtInfo() const
{
	return ARTFILEMGR.getTerrainArtInfo(getArtDefineTag());
}

//======================================================================================================
//					CvInterfaceModeInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvInterfaceModeInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvInterfaceModeInfo::CvInterfaceModeInfo() :
m_iCursorIndex(NO_CURSOR),
m_iMissionType(NO_MISSION),
m_bVisible(false),
m_bGotoPlot(false),
m_bHighlightPlot(false),
m_bSelectType(false),
m_bSelectAll(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvInterfaceModeInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvInterfaceModeInfo::~CvInterfaceModeInfo()
{
}

int CvInterfaceModeInfo::getCursorIndex() const
{
	return m_iCursorIndex;
}

int CvInterfaceModeInfo::getMissionType() const
{
	return m_iMissionType;
}

bool CvInterfaceModeInfo::getVisible() const
{
	return m_bVisible;
}

bool CvInterfaceModeInfo::getGotoPlot() const
{
	return m_bGotoPlot;
}

bool CvInterfaceModeInfo::getHighlightPlot() const
{
	return m_bHighlightPlot;
}

bool CvInterfaceModeInfo::getSelectType() const
{
	return m_bSelectType;
}

bool CvInterfaceModeInfo::getSelectAll() const
{
	return m_bSelectAll;
}

bool CvInterfaceModeInfo::read(CvXMLLoadUtility* pXML)
{

	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CursorType");
	m_iCursorIndex = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Mission");
	m_iMissionType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");
	pXML->GetOptionalChildXmlValByName(&m_bGotoPlot, L"bGotoPlot");
	pXML->GetOptionalChildXmlValByName(&m_bHighlightPlot, L"bHighlightPlot");
	pXML->GetOptionalChildXmlValByName(&m_bSelectType, L"bSelectType");
	pXML->GetOptionalChildXmlValByName(&m_bSelectAll, L"bSelectAll");

	return true;
}

void CvInterfaceModeInfo::copyNonDefaults(const CvInterfaceModeInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getCursorIndex() == iTextDefault) m_iCursorIndex = pClassInfo->getCursorIndex();
	if (getMissionType() == iTextDefault) m_iMissionType = pClassInfo->getMissionType();

	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
	if (getGotoPlot() == bDefault) m_bGotoPlot = pClassInfo->getGotoPlot();
	if (getHighlightPlot() == bDefault) m_bHighlightPlot = pClassInfo->getHighlightPlot();
	if (getSelectType() == bDefault) m_bSelectType = pClassInfo->getSelectType();
	if (getSelectAll() == bDefault) m_bSelectAll = pClassInfo->getSelectAll();
}

//======================================================================================================
//					CvAdvisorInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAdvisorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAdvisorInfo::CvAdvisorInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAdvisorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAdvisorInfo::~CvAdvisorInfo()
{
}

const char* CvAdvisorInfo::getTexture() const
{
	return m_szTexture;
}

int CvAdvisorInfo::getNumCodes() const
{
	return m_vctEnableDisableCodes.size();
}

int CvAdvisorInfo::getEnableCode(uint32_t uiCode) const
{
	FASSERT_BOUNDS(0, m_vctEnableDisableCodes.size(), uiCode);
	return m_vctEnableDisableCodes[uiCode].first;
}

int CvAdvisorInfo::getDisableCode(uint32_t uiCode) const
{
	FASSERT_BOUNDS(0, m_vctEnableDisableCodes.size(), uiCode);
	return m_vctEnableDisableCodes[uiCode].second;
}

bool CvAdvisorInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szTexture, L"Texture", "");

	if(pXML->TryMoveToXmlFirstChild())
	{
		while (pXML->TryMoveToXmlNextSibling(L"EventCodes"))
		{
			int iEnableCode, iDisableCode;
			pXML->GetChildXmlValByName(&iEnableCode, L"iEnableCode");
			pXML->GetChildXmlValByName(&iDisableCode, L"iDisableCode");
			m_vctEnableDisableCodes.push_back(std::make_pair(iEnableCode, iDisableCode));
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvAdvisorInfo::copyNonDefaults(const CvAdvisorInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getTexture() == cDefault) m_szTexture = pClassInfo->getTexture();

	if ( getNumCodes() == 0 )  //Only copy old values if the new doesn't hold a tag
	{
		for ( int iI = 0; iI < pClassInfo->getNumCodes(); iI++)
		{
			m_vctEnableDisableCodes.push_back(std::make_pair(pClassInfo->getEnableCode(iI), pClassInfo->getDisableCode(iI)));
		}
	}
}

//======================================================================================================
//					CvLeaderHeadInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvLeaderHeadInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvLeaderHeadInfo::CvLeaderHeadInfo() :
m_bNPC(false),
m_iWonderConstructRand(0),
m_iBaseAttitude(0),
m_iBasePeaceWeight(0),
m_iPeaceWeightRand(0),
m_iWarmongerRespect(0),
m_iEspionageWeight(0),
m_iRefuseToTalkWarThreshold(0),
m_iNoTechTradeThreshold(0),
m_iTechTradeKnownPercent(0),
m_iMaxGoldTradePercent(0),
m_iMaxGoldPerTurnTradePercent(0),
m_iCultureVictoryWeight(0),
m_iSpaceVictoryWeight(0),
m_iConquestVictoryWeight(0),
m_iDominationVictoryWeight(0),
m_iDiplomacyVictoryWeight(0),
m_iMaxWarRand(0),
m_iMaxWarNearbyPowerRatio(0),
m_iMaxWarDistantPowerRatio(0),
m_iMaxWarMinAdjacentLandPercent(0),
m_iLimitedWarRand(0),
m_iLimitedWarPowerRatio(0),
m_iDogpileWarRand(0),
m_iMakePeaceRand(0),
m_iDeclareWarTradeRand(0),
m_iDemandRebukedSneakProb(0),
m_iDemandRebukedWarProb(0),
m_iRazeCityProb(0),
m_iBuildUnitProb(0),
m_iBaseAttackOddsChange(0),
m_iAttackOddsChangeRand(0),
m_iWorseRankDifferenceAttitudeChange(0),
m_iBetterRankDifferenceAttitudeChange(0),
m_iCloseBordersAttitudeChange(0),
m_iLostWarAttitudeChange(0),
m_iAtWarAttitudeDivisor(0),
m_iAtWarAttitudeChangeLimit(0),
m_iAtPeaceAttitudeDivisor(0),
m_iAtPeaceAttitudeChangeLimit(0),
m_iSameReligionAttitudeChange(0),
m_iSameReligionAttitudeDivisor(0),
m_iSameReligionAttitudeChangeLimit(0),
m_iDifferentReligionAttitudeChange(0),
m_iDifferentReligionAttitudeDivisor(0),
m_iDifferentReligionAttitudeChangeLimit(0),
m_iBonusTradeAttitudeDivisor(0),
m_iBonusTradeAttitudeChangeLimit(0),
m_iOpenBordersAttitudeDivisor(0),
m_iOpenBordersAttitudeChangeLimit(0),
m_iDefensivePactAttitudeDivisor(0),
m_iDefensivePactAttitudeChangeLimit(0),
m_iShareWarAttitudeChange(0),
m_iShareWarAttitudeDivisor(0),
m_iShareWarAttitudeChangeLimit(0),
m_iFavoriteCivicAttitudeChange(0),
m_iFavoriteCivicAttitudeDivisor(0),
m_iFavoriteCivicAttitudeChangeLimit(0),
m_iDemandTributeAttitudeThreshold(NO_ATTITUDE),
m_iNoGiveHelpAttitudeThreshold(NO_ATTITUDE),
m_iTechRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStrategicBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iHappinessBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iHealthBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iMapRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDeclareWarRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDeclareWarThemRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStopTradingRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStopTradingThemRefuseAttitudeThreshold(NO_ATTITUDE),
m_iAdoptCivicRefuseAttitudeThreshold(NO_ATTITUDE),
m_iConvertReligionRefuseAttitudeThreshold(NO_ATTITUDE),
m_iOpenBordersRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDefensivePactRefuseAttitudeThreshold(NO_ATTITUDE),
m_iPermanentAllianceRefuseAttitudeThreshold(NO_ATTITUDE),
m_iVassalRefuseAttitudeThreshold(NO_ATTITUDE),
m_iVassalPowerModifier(0),
m_iFreedomAppreciation(0),
m_iFavoriteCivic(NO_CIVIC),
m_iFavoriteReligion(NO_RELIGION),
m_pbTraits(NULL),
m_piFlavorValue(NULL),
m_piContactRand(NULL),
m_piContactDelay(NULL),
m_piMemoryDecayRand(NULL),
m_piMemoryAttitudePercent(NULL),
m_piNoWarAttitudeProb(NULL),
m_piUnitAIWeightModifier(NULL),
m_piImprovementWeightModifier(NULL),
m_piDiploPeaceIntroMusicScriptIds(NULL),
m_piDiploPeaceMusicScriptIds(NULL),
m_piDiploWarIntroMusicScriptIds(NULL),
m_piDiploWarMusicScriptIds(NULL)

,m_iMilitaryUnitRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
,m_iWorkerRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
,m_iCorporationRefuseAttitudeThreshold(ATTITUDE_CAUTIOUS)
,m_iSecretaryGeneralVoteRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvLeaderHeadInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvLeaderHeadInfo::~CvLeaderHeadInfo()
{
	SAFE_DELETE_ARRAY(m_pbTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piContactRand);
	SAFE_DELETE_ARRAY(m_piContactDelay);
	SAFE_DELETE_ARRAY(m_piMemoryDecayRand);
	SAFE_DELETE_ARRAY(m_piMemoryAttitudePercent);
	SAFE_DELETE_ARRAY(m_piNoWarAttitudeProb);
	SAFE_DELETE_ARRAY(m_piUnitAIWeightModifier);
	SAFE_DELETE_ARRAY(m_piImprovementWeightModifier);
	SAFE_DELETE_ARRAY(m_piDiploPeaceIntroMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploPeaceMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploWarIntroMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploWarMusicScriptIds);
}

const char* CvLeaderHeadInfo::getButton() const
{
	const CvArtInfoLeaderhead* pLeaderheadArtInfo = getArtInfo();
	return pLeaderheadArtInfo ? pLeaderheadArtInfo->getButton() : NULL;
}

bool CvLeaderHeadInfo::isNPC() const
{
	return m_bNPC;
}

int CvLeaderHeadInfo::getWonderConstructRand() const
{
	return m_iWonderConstructRand;
}

int CvLeaderHeadInfo::getBaseAttitude() const
{
	return m_iBaseAttitude;
}

int CvLeaderHeadInfo::getBasePeaceWeight() const
{
	return m_iBasePeaceWeight;
}

int CvLeaderHeadInfo::getPeaceWeightRand() const
{
	return m_iPeaceWeightRand;
}

int CvLeaderHeadInfo::getWarmongerRespect() const
{
	return m_iWarmongerRespect;
}

int CvLeaderHeadInfo::getEspionageWeight() const
{
	return m_iEspionageWeight;
}

int CvLeaderHeadInfo::getRefuseToTalkWarThreshold() const
{
	return m_iRefuseToTalkWarThreshold;
}

int CvLeaderHeadInfo::getNoTechTradeThreshold() const
{
	return m_iNoTechTradeThreshold;
}

int CvLeaderHeadInfo::getTechTradeKnownPercent() const
{
	return m_iTechTradeKnownPercent;
}

int CvLeaderHeadInfo::getMaxGoldTradePercent() const
{
	return m_iMaxGoldTradePercent;
}

int CvLeaderHeadInfo::getMaxGoldPerTurnTradePercent() const
{
	return m_iMaxGoldPerTurnTradePercent;
}

int CvLeaderHeadInfo::getCultureVictoryWeight() const
{
	return m_iCultureVictoryWeight;
}

int CvLeaderHeadInfo::getSpaceVictoryWeight() const
{
	return m_iSpaceVictoryWeight;
}

int CvLeaderHeadInfo::getConquestVictoryWeight() const
{
	return m_iConquestVictoryWeight;
}

int CvLeaderHeadInfo::getDominationVictoryWeight() const
{
	return m_iDominationVictoryWeight;
}

int CvLeaderHeadInfo::getDiplomacyVictoryWeight() const
{
	return m_iDiplomacyVictoryWeight;
}

int CvLeaderHeadInfo::getMaxWarRand() const
{
	return m_iMaxWarRand;
}

int CvLeaderHeadInfo::getMaxWarNearbyPowerRatio() const
{
	return m_iMaxWarNearbyPowerRatio;
}

int CvLeaderHeadInfo::getMaxWarDistantPowerRatio() const
{
	return m_iMaxWarDistantPowerRatio;
}

int CvLeaderHeadInfo::getMaxWarMinAdjacentLandPercent() const
{
	return m_iMaxWarMinAdjacentLandPercent;
}

int CvLeaderHeadInfo::getLimitedWarRand() const
{
	return m_iLimitedWarRand;
}

int CvLeaderHeadInfo::getLimitedWarPowerRatio() const
{
	return m_iLimitedWarPowerRatio;
}

int CvLeaderHeadInfo::getDogpileWarRand() const
{
	return m_iDogpileWarRand;
}

int CvLeaderHeadInfo::getMakePeaceRand() const
{
	return m_iMakePeaceRand;
}

int CvLeaderHeadInfo::getDeclareWarTradeRand() const
{
	return m_iDeclareWarTradeRand;
}

int CvLeaderHeadInfo::getDemandRebukedSneakProb() const
{
	return m_iDemandRebukedSneakProb;
}

int CvLeaderHeadInfo::getDemandRebukedWarProb() const
{
	return m_iDemandRebukedWarProb;
}

int CvLeaderHeadInfo::getRazeCityProb() const
{
	return m_iRazeCityProb;
}

int CvLeaderHeadInfo::getBuildUnitProb() const
{
	return m_iBuildUnitProb;
}

int CvLeaderHeadInfo::getBaseAttackOddsChange() const
{
	return m_iBaseAttackOddsChange;
}

int CvLeaderHeadInfo::getAttackOddsChangeRand() const
{
	return m_iAttackOddsChangeRand;
}

int CvLeaderHeadInfo::getWorseRankDifferenceAttitudeChange() const
{
	return m_iWorseRankDifferenceAttitudeChange;
}

int CvLeaderHeadInfo::getBetterRankDifferenceAttitudeChange() const
{
	return m_iBetterRankDifferenceAttitudeChange;
}

int CvLeaderHeadInfo::getCloseBordersAttitudeChange() const
{
	return m_iCloseBordersAttitudeChange;
}

int CvLeaderHeadInfo::getLostWarAttitudeChange() const
{
	return m_iLostWarAttitudeChange;
}

int CvLeaderHeadInfo::getAtWarAttitudeDivisor() const
{
	return m_iAtWarAttitudeDivisor;
}

int CvLeaderHeadInfo::getAtWarAttitudeChangeLimit() const
{
	return m_iAtWarAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getAtPeaceAttitudeDivisor() const
{
	return m_iAtPeaceAttitudeDivisor;
}

int CvLeaderHeadInfo::getAtPeaceAttitudeChangeLimit() const
{
	return m_iAtPeaceAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getSameReligionAttitudeChange() const
{
	return m_iSameReligionAttitudeChange;
}

int CvLeaderHeadInfo::getSameReligionAttitudeDivisor() const
{
	return m_iSameReligionAttitudeDivisor;
}

int CvLeaderHeadInfo::getSameReligionAttitudeChangeLimit() const
{
	return m_iSameReligionAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getDifferentReligionAttitudeChange() const
{
	return m_iDifferentReligionAttitudeChange;
}

int CvLeaderHeadInfo::getDifferentReligionAttitudeDivisor() const
{
	return m_iDifferentReligionAttitudeDivisor;
}

int CvLeaderHeadInfo::getDifferentReligionAttitudeChangeLimit() const
{
	return m_iDifferentReligionAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getBonusTradeAttitudeDivisor() const
{
	return m_iBonusTradeAttitudeDivisor;
}

int CvLeaderHeadInfo::getBonusTradeAttitudeChangeLimit() const
{
	return m_iBonusTradeAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getOpenBordersAttitudeDivisor() const
{
	return m_iOpenBordersAttitudeDivisor;
}

int CvLeaderHeadInfo::getOpenBordersAttitudeChangeLimit() const
{
	return m_iOpenBordersAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getDefensivePactAttitudeDivisor() const
{
	return m_iDefensivePactAttitudeDivisor;
}

int CvLeaderHeadInfo::getDefensivePactAttitudeChangeLimit() const
{
	return m_iDefensivePactAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getShareWarAttitudeChange() const
{
	return m_iShareWarAttitudeChange;
}

int CvLeaderHeadInfo::getShareWarAttitudeDivisor() const
{
	return m_iShareWarAttitudeDivisor;
}

int CvLeaderHeadInfo::getShareWarAttitudeChangeLimit() const
{
	return m_iShareWarAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getFavoriteCivicAttitudeChange() const
{
	return m_iFavoriteCivicAttitudeChange;
}

int CvLeaderHeadInfo::getFavoriteCivicAttitudeDivisor() const
{
	return m_iFavoriteCivicAttitudeDivisor;
}

int CvLeaderHeadInfo::getFavoriteCivicAttitudeChangeLimit() const
{
	return m_iFavoriteCivicAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getDemandTributeAttitudeThreshold() const
{
	return m_iDemandTributeAttitudeThreshold;
}

int CvLeaderHeadInfo::getNoGiveHelpAttitudeThreshold() const
{
	return m_iNoGiveHelpAttitudeThreshold;
}

int CvLeaderHeadInfo::getTechRefuseAttitudeThreshold() const
{
	return m_iTechRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getStrategicBonusRefuseAttitudeThreshold() const
{
	return m_iStrategicBonusRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getHappinessBonusRefuseAttitudeThreshold() const
{
	return m_iHappinessBonusRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getHealthBonusRefuseAttitudeThreshold() const
{
	return m_iHealthBonusRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getMapRefuseAttitudeThreshold() const
{
	return m_iMapRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getDeclareWarRefuseAttitudeThreshold() const
{
	return m_iDeclareWarRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getDeclareWarThemRefuseAttitudeThreshold() const
{
	return m_iDeclareWarThemRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getStopTradingRefuseAttitudeThreshold() const
{
	return m_iStopTradingRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getStopTradingThemRefuseAttitudeThreshold() const
{
	return m_iStopTradingThemRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getAdoptCivicRefuseAttitudeThreshold() const
{
	return m_iAdoptCivicRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getConvertReligionRefuseAttitudeThreshold() const
{
	return m_iConvertReligionRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getOpenBordersRefuseAttitudeThreshold() const
{
	return m_iOpenBordersRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getDefensivePactRefuseAttitudeThreshold() const
{
	return m_iDefensivePactRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getPermanentAllianceRefuseAttitudeThreshold() const
{
	return m_iPermanentAllianceRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getVassalRefuseAttitudeThreshold() const
{
	return m_iVassalRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getVassalPowerModifier() const
{
	return m_iVassalPowerModifier;
}

int CvLeaderHeadInfo::getFavoriteCivic() const
{
	return m_iFavoriteCivic;
}

int CvLeaderHeadInfo::getFavoriteReligion() const
{
	return m_iFavoriteReligion;
}

int CvLeaderHeadInfo::getFreedomAppreciation() const
{
	return m_iFreedomAppreciation;
}

const char* CvLeaderHeadInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

// Arrays
//DEFTRAITORIG
bool CvLeaderHeadInfo::hasTrait(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTraitInfos(), i);
	return m_pbTraits ? m_pbTraits[i] : false;
}

int CvLeaderHeadInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvLeaderHeadInfo::getContactRand(int i) const
{
	FASSERT_BOUNDS(0, NUM_CONTACT_TYPES, i);
	return m_piContactRand[i];
}

int CvLeaderHeadInfo::getContactDelay(int i) const
{
	FASSERT_BOUNDS(0, NUM_CONTACT_TYPES, i);
	return m_piContactDelay[i];
}

int CvLeaderHeadInfo::getMemoryDecayRand(int i) const
{
	FASSERT_BOUNDS(0, NUM_MEMORY_TYPES, i);
	return m_piMemoryDecayRand[i];
}

int CvLeaderHeadInfo::getMemoryAttitudePercent(int i) const
{
	FASSERT_BOUNDS(0, NUM_MEMORY_TYPES, i);
	return m_piMemoryAttitudePercent[i];
}

int CvLeaderHeadInfo::getNoWarAttitudeProb(int i) const
{
	FASSERT_BOUNDS(0, NUM_ATTITUDE_TYPES, i);
	return m_piNoWarAttitudeProb ? m_piNoWarAttitudeProb[i] : 0;
}

int CvLeaderHeadInfo::getUnitAIWeightModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, i);
	return m_piUnitAIWeightModifier ? m_piUnitAIWeightModifier[i] : 0;
}

int CvLeaderHeadInfo::getImprovementWeightModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	return m_piImprovementWeightModifier ? m_piImprovementWeightModifier[i] : 0;
}

int CvLeaderHeadInfo::getDiploPeaceIntroMusicScriptIds(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), i);
	return m_piDiploPeaceIntroMusicScriptIds ? m_piDiploPeaceIntroMusicScriptIds[i] : -1;
}

int CvLeaderHeadInfo::getDiploPeaceMusicScriptIds(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), i);
	return m_piDiploPeaceMusicScriptIds ? m_piDiploPeaceMusicScriptIds[i] : -1;
}

int CvLeaderHeadInfo::getDiploWarIntroMusicScriptIds(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), i);
	return m_piDiploWarIntroMusicScriptIds ? m_piDiploWarIntroMusicScriptIds[i] : -1;
}

int CvLeaderHeadInfo::getDiploWarMusicScriptIds(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), i);
	return m_piDiploWarMusicScriptIds ? m_piDiploWarMusicScriptIds[i] : -1;
}

const char* CvLeaderHeadInfo::getLeaderHead() const
{
	return getArtInfo() ? getArtInfo()->getNIF() : NULL;
}

int CvLeaderHeadInfo::getMilitaryUnitRefuseAttitudeThreshold() const
{
	return m_iMilitaryUnitRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getWorkerRefuseAttitudeThreshold() const
{
	return m_iWorkerRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getCorporationRefuseAttitudeThreshold() const
{
	return m_iCorporationRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getSecretaryGeneralVoteRefuseAttitudeThreshold() const
{
	return m_iSecretaryGeneralVoteRefuseAttitudeThreshold;
}

void CvLeaderHeadInfo::setCultureVictoryWeight(int i)
{
	m_iCultureVictoryWeight = i;
}

void CvLeaderHeadInfo::setSpaceVictoryWeight(int i)
{
	m_iSpaceVictoryWeight = i;
}

void CvLeaderHeadInfo::setConquestVictoryWeight(int i)
{
	m_iConquestVictoryWeight = i;
}

void CvLeaderHeadInfo::setDominationVictoryWeight(int i)
{
	m_iDominationVictoryWeight = i;
}

void CvLeaderHeadInfo::setDiplomacyVictoryWeight(int i)
{
	m_iDiplomacyVictoryWeight = i;
}

//Int list Vector without delayed resolution
int CvLeaderHeadInfo::getDefaultTrait(int i) const
{
	return m_aiDefaultTraits[i];
}

int CvLeaderHeadInfo::getNumDefaultTraits() const
{
	return (int)m_aiDefaultTraits.size();
}

bool CvLeaderHeadInfo::isDefaultTrait(int i) const
{
	return algo::any_of_equal(m_aiDefaultTraits, i);
}

int CvLeaderHeadInfo::getDefaultComplexTrait(int i) const
{
	return m_aiDefaultComplexTraits[i];
}

int CvLeaderHeadInfo::getNumDefaultComplexTraits() const
{
	return (int)m_aiDefaultComplexTraits.size();
}

bool CvLeaderHeadInfo::isDefaultComplexTrait(int i) const
{
	return algo::any_of_equal(m_aiDefaultComplexTraits, i);
}


void CvLeaderHeadInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bNPC);
	CheckSum(iSum, m_iWonderConstructRand);
	CheckSum(iSum, m_iBaseAttitude);
	CheckSum(iSum, m_iBasePeaceWeight);
	CheckSum(iSum, m_iPeaceWeightRand);
	CheckSum(iSum, m_iWarmongerRespect);
	CheckSum(iSum, m_iEspionageWeight);
	CheckSum(iSum, m_iRefuseToTalkWarThreshold);
	CheckSum(iSum, m_iNoTechTradeThreshold);
	CheckSum(iSum, m_iTechTradeKnownPercent);
	CheckSum(iSum, m_iMaxGoldTradePercent);
	CheckSum(iSum, m_iMaxGoldPerTurnTradePercent);

	CheckSum(iSum, m_iCultureVictoryWeight);
	CheckSum(iSum, m_iSpaceVictoryWeight);
	CheckSum(iSum, m_iConquestVictoryWeight);
	CheckSum(iSum, m_iDominationVictoryWeight);
	CheckSum(iSum, m_iDiplomacyVictoryWeight);

	CheckSum(iSum, m_iMaxWarRand);
	CheckSum(iSum, m_iMaxWarNearbyPowerRatio);
	CheckSum(iSum, m_iMaxWarDistantPowerRatio);
	CheckSum(iSum, m_iMaxWarMinAdjacentLandPercent);
	CheckSum(iSum, m_iLimitedWarRand);
	CheckSum(iSum, m_iLimitedWarPowerRatio);
	CheckSum(iSum, m_iDogpileWarRand);
	CheckSum(iSum, m_iMakePeaceRand);
	CheckSum(iSum, m_iDeclareWarTradeRand);
	CheckSum(iSum, m_iDemandRebukedSneakProb);
	CheckSum(iSum, m_iDemandRebukedWarProb);
	CheckSum(iSum, m_iRazeCityProb);
	CheckSum(iSum, m_iBuildUnitProb);
	CheckSum(iSum, m_iBaseAttackOddsChange);
	CheckSum(iSum, m_iAttackOddsChangeRand);
	CheckSum(iSum, m_iWorseRankDifferenceAttitudeChange);
	CheckSum(iSum, m_iBetterRankDifferenceAttitudeChange);
	CheckSum(iSum, m_iCloseBordersAttitudeChange);
	CheckSum(iSum, m_iLostWarAttitudeChange);
	CheckSum(iSum, m_iAtWarAttitudeDivisor);
	CheckSum(iSum, m_iAtWarAttitudeChangeLimit);
	CheckSum(iSum, m_iAtPeaceAttitudeDivisor);
	CheckSum(iSum, m_iAtPeaceAttitudeChangeLimit);
	CheckSum(iSum, m_iSameReligionAttitudeChange);
	CheckSum(iSum, m_iSameReligionAttitudeDivisor);
	CheckSum(iSum, m_iSameReligionAttitudeChangeLimit);
	CheckSum(iSum, m_iDifferentReligionAttitudeChange);
	CheckSum(iSum, m_iDifferentReligionAttitudeDivisor);
	CheckSum(iSum, m_iDifferentReligionAttitudeChangeLimit);
	CheckSum(iSum, m_iBonusTradeAttitudeDivisor);
	CheckSum(iSum, m_iBonusTradeAttitudeChangeLimit);
	CheckSum(iSum, m_iOpenBordersAttitudeDivisor);
	CheckSum(iSum, m_iOpenBordersAttitudeChangeLimit);
	CheckSum(iSum, m_iDefensivePactAttitudeDivisor);
	CheckSum(iSum, m_iDefensivePactAttitudeChangeLimit);
	CheckSum(iSum, m_iShareWarAttitudeChange);
	CheckSum(iSum, m_iShareWarAttitudeDivisor);
	CheckSum(iSum, m_iShareWarAttitudeChangeLimit);
	CheckSum(iSum, m_iFavoriteCivicAttitudeChange);
	CheckSum(iSum, m_iFavoriteCivicAttitudeDivisor);
	CheckSum(iSum, m_iFavoriteCivicAttitudeChangeLimit);
	CheckSum(iSum, m_iDemandTributeAttitudeThreshold);
	CheckSum(iSum, m_iNoGiveHelpAttitudeThreshold);
	CheckSum(iSum, m_iTechRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStrategicBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iHappinessBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iHealthBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iMapRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDeclareWarRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDeclareWarThemRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStopTradingRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStopTradingThemRefuseAttitudeThreshold);
	CheckSum(iSum, m_iAdoptCivicRefuseAttitudeThreshold);
	CheckSum(iSum, m_iConvertReligionRefuseAttitudeThreshold);
	CheckSum(iSum, m_iOpenBordersRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDefensivePactRefuseAttitudeThreshold);
	CheckSum(iSum, m_iPermanentAllianceRefuseAttitudeThreshold);
	CheckSum(iSum, m_iVassalRefuseAttitudeThreshold);
	CheckSum(iSum, m_iVassalPowerModifier);
	CheckSum(iSum, m_iFreedomAppreciation);
	CheckSum(iSum, m_iFavoriteCivic);
	CheckSum(iSum, m_iFavoriteReligion);

	// Arrays

	CheckSumI(iSum, GC.getNumTraitInfos(), m_pbTraits);

	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piFlavorValue);
	CheckSumI(iSum, NUM_CONTACT_TYPES, m_piContactRand);
	CheckSumI(iSum, NUM_CONTACT_TYPES, m_piContactDelay);
	CheckSumI(iSum, NUM_MEMORY_TYPES, m_piMemoryDecayRand);
	CheckSumI(iSum, NUM_MEMORY_TYPES, m_piMemoryAttitudePercent);
	CheckSumI(iSum, NUM_ATTITUDE_TYPES, m_piNoWarAttitudeProb);
	CheckSumI(iSum, NUM_UNITAI_TYPES, m_piUnitAIWeightModifier);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementWeightModifier);

	CheckSum(iSum, m_iMilitaryUnitRefuseAttitudeThreshold);
	CheckSum(iSum, m_iWorkerRefuseAttitudeThreshold);
	CheckSum(iSum, m_iCorporationRefuseAttitudeThreshold);
	CheckSum(iSum, m_iSecretaryGeneralVoteRefuseAttitudeThreshold);


	//Int list Vector without delayed resolution

	CheckSumC(iSum, m_aiDefaultTraits);
	CheckSumC(iSum, m_aiDefaultComplexTraits);


}

const CvArtInfoLeaderhead* CvLeaderHeadInfo::getArtInfo() const
{
	return ARTFILEMGR.getLeaderheadArtInfo( getArtDefineTag());
}

bool CvLeaderHeadInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	if (!m_piMemoryDecayRand)
	{
		CvXMLLoadUtility::InitList(&m_piMemoryDecayRand, NUM_MEMORY_TYPES, 0);
		CvXMLLoadUtility::InitList(&m_piMemoryAttitudePercent, NUM_MEMORY_TYPES, 0);
		CvXMLLoadUtility::InitList(&m_piContactRand, NUM_CONTACT_TYPES, 0);
		CvXMLLoadUtility::InitList(&m_piContactDelay, NUM_CONTACT_TYPES, 0);
	}
	CvString szTextVal;

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");
	pXML->GetOptionalChildXmlValByName(&m_bNPC, L"bNPC");
	pXML->GetOptionalChildXmlValByName(&m_iWonderConstructRand, L"iWonderConstructRand");
	pXML->GetOptionalChildXmlValByName(&m_iBaseAttitude, L"iBaseAttitude");
	pXML->GetOptionalChildXmlValByName(&m_iBasePeaceWeight, L"iBasePeaceWeight");
	pXML->GetOptionalChildXmlValByName(&m_iPeaceWeightRand, L"iPeaceWeightRand");
	pXML->GetOptionalChildXmlValByName(&m_iWarmongerRespect, L"iWarmongerRespect");
	pXML->GetOptionalChildXmlValByName(&m_iEspionageWeight, L"iEspionageWeight");
	pXML->GetOptionalChildXmlValByName(&m_iRefuseToTalkWarThreshold, L"iRefuseToTalkWarThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iNoTechTradeThreshold, L"iNoTechTradeThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iTechTradeKnownPercent, L"iTechTradeKnownPercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGoldTradePercent, L"iMaxGoldTradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGoldPerTurnTradePercent, L"iMaxGoldPerTurnTradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCultureVictoryWeight, L"iCultureVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iSpaceVictoryWeight, L"iSpaceVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iConquestVictoryWeight, L"iConquestVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iDominationVictoryWeight, L"iDominationVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iDiplomacyVictoryWeight, L"iDiplomacyVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarRand, L"iMaxWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarNearbyPowerRatio, L"iMaxWarNearbyPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarDistantPowerRatio, L"iMaxWarDistantPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarMinAdjacentLandPercent, L"iMaxWarMinAdjacentLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iLimitedWarRand, L"iLimitedWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iLimitedWarPowerRatio, L"iLimitedWarPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iDogpileWarRand, L"iDogpileWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iMakePeaceRand, L"iMakePeaceRand");
	pXML->GetOptionalChildXmlValByName(&m_iDeclareWarTradeRand, L"iDeclareWarTradeRand");
	pXML->GetOptionalChildXmlValByName(&m_iDemandRebukedSneakProb, L"iDemandRebukedSneakProb");
	pXML->GetOptionalChildXmlValByName(&m_iDemandRebukedWarProb, L"iDemandRebukedWarProb");
	pXML->GetOptionalChildXmlValByName(&m_iRazeCityProb, L"iRazeCityProb");
	pXML->GetOptionalChildXmlValByName(&m_iBuildUnitProb, L"iBuildUnitProb");
	pXML->GetOptionalChildXmlValByName(&m_iBaseAttackOddsChange, L"iBaseAttackOddsChange");
	pXML->GetOptionalChildXmlValByName(&m_iAttackOddsChangeRand, L"iAttackOddsChangeRand");
	pXML->GetOptionalChildXmlValByName(&m_iWorseRankDifferenceAttitudeChange, L"iWorseRankDifferenceAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iBetterRankDifferenceAttitudeChange, L"iBetterRankDifferenceAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iCloseBordersAttitudeChange, L"iCloseBordersAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iLostWarAttitudeChange, L"iLostWarAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iAtWarAttitudeDivisor, L"iAtWarAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iAtWarAttitudeChangeLimit, L"iAtWarAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iAtPeaceAttitudeDivisor, L"iAtPeaceAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iAtPeaceAttitudeChangeLimit, L"iAtPeaceAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeChange, L"iSameReligionAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeDivisor, L"iSameReligionAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeChangeLimit, L"iSameReligionAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeChange, L"iDifferentReligionAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeDivisor, L"iDifferentReligionAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeChangeLimit, L"iDifferentReligionAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iBonusTradeAttitudeDivisor, L"iBonusTradeAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iBonusTradeAttitudeChangeLimit, L"iBonusTradeAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iOpenBordersAttitudeDivisor, L"iOpenBordersAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iOpenBordersAttitudeChangeLimit, L"iOpenBordersAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iDefensivePactAttitudeDivisor, L"iDefensivePactAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iDefensivePactAttitudeChangeLimit, L"iDefensivePactAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeChange, L"iShareWarAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeDivisor, L"iShareWarAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeChangeLimit, L"iShareWarAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeChange, L"iFavoriteCivicAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeDivisor, L"iFavoriteCivicAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeChangeLimit, L"iFavoriteCivicAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iVassalPowerModifier, L"iVassalPowerModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreedomAppreciation, L"iFreedomAppreciation");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DemandTributeAttitudeThreshold");
	m_iDemandTributeAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NoGiveHelpAttitudeThreshold");
	m_iNoGiveHelpAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechRefuseAttitudeThreshold");
	m_iTechRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StrategicBonusRefuseAttitudeThreshold");
	m_iStrategicBonusRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HappinessBonusRefuseAttitudeThreshold");
	m_iHappinessBonusRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HealthBonusRefuseAttitudeThreshold");
	m_iHealthBonusRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MapRefuseAttitudeThreshold");
	m_iMapRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DeclareWarRefuseAttitudeThreshold");
	m_iDeclareWarRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DeclareWarThemRefuseAttitudeThreshold");
	m_iDeclareWarThemRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StopTradingRefuseAttitudeThreshold");
	m_iStopTradingRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StopTradingThemRefuseAttitudeThreshold");
	m_iStopTradingThemRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"AdoptCivicRefuseAttitudeThreshold");
	m_iAdoptCivicRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ConvertReligionRefuseAttitudeThreshold");
	m_iConvertReligionRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"OpenBordersRefuseAttitudeThreshold");
	m_iOpenBordersRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DefensivePactRefuseAttitudeThreshold");
	m_iDefensivePactRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PermanentAllianceRefuseAttitudeThreshold");
	m_iPermanentAllianceRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"VassalRefuseAttitudeThreshold");
	m_iVassalRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FavoriteCivic");
	m_iFavoriteCivic = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FavoriteReligion");
	m_iFavoriteReligion = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_pbTraits, L"Traits", GC.getNumTraitInfos());

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());
	pXML->SetVariableListTagPair(&m_piContactRand, L"ContactRands", NUM_CONTACT_TYPES);
	pXML->SetVariableListTagPair(&m_piContactDelay, L"ContactDelays", NUM_CONTACT_TYPES);
	pXML->SetVariableListTagPair(&m_piMemoryDecayRand, L"MemoryDecays", NUM_MEMORY_TYPES);
	pXML->SetVariableListTagPair(&m_piMemoryAttitudePercent, L"MemoryAttitudePercents", NUM_MEMORY_TYPES);
	pXML->SetVariableListTagPair(&m_piNoWarAttitudeProb, L"NoWarAttitudeProbs", NUM_ATTITUDE_TYPES);
	pXML->SetVariableListTagPair(&m_piUnitAIWeightModifier, L"UnitAIWeightModifiers", NUM_UNITAI_TYPES);
	pXML->SetVariableListTagPair(&m_piImprovementWeightModifier, L"ImprovementWeightModifiers", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploPeaceIntroMusicScriptIds, L"DiplomacyIntroMusicPeace", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploPeaceMusicScriptIds, L"DiplomacyMusicPeace", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploWarIntroMusicScriptIds, L"DiplomacyIntroMusicWar", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploWarMusicScriptIds, L"DiplomacyMusicWar", GC.getNumEraInfos());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MilitaryUnitRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iMilitaryUnitRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WorkerRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iWorkerRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CorporationRefuseAttitudeThreshold", "ATTITUDE_CAUTIOUS");
	m_iCorporationRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SecretaryGeneralVoteRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iSecretaryGeneralVoteRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	//Int list Vector without delayed resolution
	pXML->SetOptionalVector(&m_aiDefaultTraits, L"DefaultTraits");
	pXML->SetOptionalVector(&m_aiDefaultComplexTraits, L"DefaultComplexTraits");

	setDefaultMemoryInfo();
	setDefaultContactInfo();

	return true;
}

void CvLeaderHeadInfo::copyNonDefaults(const CvLeaderHeadInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvString cDefault = CvString::format("").GetCString();

	//Art files must be reread first!
	if (getArtDefineTag() == cDefault) m_szArtDefineTag = pClassInfo->getArtDefineTag();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (!isNPC()) m_bNPC = pClassInfo->isNPC();
	if (getWonderConstructRand() == 0) m_iWonderConstructRand = pClassInfo->getWonderConstructRand();
	if (getBaseAttitude() == 0) m_iBaseAttitude = pClassInfo->getBaseAttitude();
	if (getBasePeaceWeight() == 0) m_iBasePeaceWeight = pClassInfo->getBasePeaceWeight();
	if (getPeaceWeightRand() == 0) m_iPeaceWeightRand = pClassInfo->getPeaceWeightRand();
	if (getWarmongerRespect() == 0) m_iWarmongerRespect = pClassInfo->getWarmongerRespect();
	if (getEspionageWeight() == 0) m_iEspionageWeight = pClassInfo->getEspionageWeight();
	if (getRefuseToTalkWarThreshold() == 0) m_iRefuseToTalkWarThreshold = pClassInfo->getRefuseToTalkWarThreshold();
	if (getNoTechTradeThreshold() == 0) m_iNoTechTradeThreshold = pClassInfo->getNoTechTradeThreshold();
	if (getTechTradeKnownPercent() == 0) m_iTechTradeKnownPercent = pClassInfo->getTechTradeKnownPercent();
	if (getMaxGoldTradePercent() == 0) m_iMaxGoldTradePercent = pClassInfo->getMaxGoldTradePercent();
	if (getMaxGoldPerTurnTradePercent() == 0) m_iMaxGoldPerTurnTradePercent = pClassInfo->getMaxGoldPerTurnTradePercent();
	if (getMaxWarRand() == 0) m_iMaxWarRand = pClassInfo->getMaxWarRand();
	if (getMaxWarNearbyPowerRatio() == 0) m_iMaxWarNearbyPowerRatio = pClassInfo->getMaxWarNearbyPowerRatio();
	if (getMaxWarDistantPowerRatio() == 0) m_iMaxWarDistantPowerRatio = pClassInfo->getMaxWarDistantPowerRatio();
	if (getMaxWarMinAdjacentLandPercent() == 0) m_iMaxWarMinAdjacentLandPercent = pClassInfo->getMaxWarMinAdjacentLandPercent();
	if (getLimitedWarRand() == 0) m_iLimitedWarRand = pClassInfo->getLimitedWarRand();
	if (getLimitedWarPowerRatio() == 0) m_iLimitedWarPowerRatio = pClassInfo->getLimitedWarPowerRatio();
	if (getDogpileWarRand() == 0) m_iDogpileWarRand = pClassInfo->getDogpileWarRand();
	if (getMakePeaceRand() == 0) m_iMakePeaceRand = pClassInfo->getMakePeaceRand();
	if (getDeclareWarTradeRand() == 0) m_iDeclareWarTradeRand = pClassInfo->getDeclareWarTradeRand();
	if (getDemandRebukedSneakProb() == 0) m_iDemandRebukedSneakProb = pClassInfo->getDemandRebukedSneakProb();
	if (getDemandRebukedWarProb() == 0) m_iDemandRebukedWarProb = pClassInfo->getDemandRebukedWarProb();
	if (getRazeCityProb() == 0) m_iRazeCityProb = pClassInfo->getRazeCityProb();
	if (getBuildUnitProb() == 0) m_iBuildUnitProb = pClassInfo->getBuildUnitProb();
	if (getBaseAttackOddsChange() == 0) m_iBaseAttackOddsChange = pClassInfo->getBaseAttackOddsChange();
	if (getAttackOddsChangeRand() == 0) m_iAttackOddsChangeRand = pClassInfo->getAttackOddsChangeRand();
	if (getWorseRankDifferenceAttitudeChange() == 0) m_iWorseRankDifferenceAttitudeChange = pClassInfo->getWorseRankDifferenceAttitudeChange();
	if (getBetterRankDifferenceAttitudeChange() == 0) m_iBetterRankDifferenceAttitudeChange = pClassInfo->getBetterRankDifferenceAttitudeChange();
	if (getCloseBordersAttitudeChange() == 0) m_iCloseBordersAttitudeChange = pClassInfo->getCloseBordersAttitudeChange();
	if (getLostWarAttitudeChange() == 0) m_iLostWarAttitudeChange = pClassInfo->getLostWarAttitudeChange();
	if (getAtWarAttitudeDivisor() == 0) m_iAtWarAttitudeDivisor = pClassInfo->getAtWarAttitudeDivisor();
	if (getAtWarAttitudeChangeLimit() == 0) m_iAtWarAttitudeChangeLimit = pClassInfo->getAtWarAttitudeChangeLimit();
	if (getAtPeaceAttitudeDivisor() == 0) m_iAtPeaceAttitudeDivisor = pClassInfo->getAtPeaceAttitudeDivisor();
	if (getAtPeaceAttitudeChangeLimit() == 0) m_iAtPeaceAttitudeChangeLimit = pClassInfo->getAtPeaceAttitudeChangeLimit();
	if (getSameReligionAttitudeChange() == 0) m_iSameReligionAttitudeChange = pClassInfo->getSameReligionAttitudeChange();
	if (getSameReligionAttitudeDivisor() == 0) m_iSameReligionAttitudeDivisor = pClassInfo->getSameReligionAttitudeDivisor();
	if (getSameReligionAttitudeChangeLimit() == 0) m_iSameReligionAttitudeChangeLimit = pClassInfo->getSameReligionAttitudeChangeLimit();
	if (getDifferentReligionAttitudeChange() == 0) m_iDifferentReligionAttitudeChange = pClassInfo->getDifferentReligionAttitudeChange();
	if (getDifferentReligionAttitudeDivisor() == 0) m_iDifferentReligionAttitudeDivisor = pClassInfo->getDifferentReligionAttitudeDivisor();
	if (getDifferentReligionAttitudeChangeLimit() == 0) m_iDifferentReligionAttitudeChangeLimit = pClassInfo->getDifferentReligionAttitudeChangeLimit();
	if (getBonusTradeAttitudeDivisor() == 0) m_iBonusTradeAttitudeDivisor = pClassInfo->getBonusTradeAttitudeDivisor();
	if (getBonusTradeAttitudeChangeLimit() == 0) m_iBonusTradeAttitudeChangeLimit = pClassInfo->getBonusTradeAttitudeChangeLimit();
	if (getOpenBordersAttitudeDivisor() == 0) m_iOpenBordersAttitudeDivisor = pClassInfo->getOpenBordersAttitudeDivisor();
	if (getOpenBordersAttitudeChangeLimit() == 0) m_iOpenBordersAttitudeChangeLimit = pClassInfo->getOpenBordersAttitudeChangeLimit();
	if (getDefensivePactAttitudeDivisor() == 0) m_iDefensivePactAttitudeDivisor = pClassInfo->getDefensivePactAttitudeDivisor();
	if (getDefensivePactAttitudeChangeLimit() == 0) m_iDefensivePactAttitudeChangeLimit = pClassInfo->getDefensivePactAttitudeChangeLimit();
	if (getShareWarAttitudeChange() == 0) m_iShareWarAttitudeChange = pClassInfo->getShareWarAttitudeChange();
	if (getShareWarAttitudeDivisor() == 0) m_iShareWarAttitudeDivisor = pClassInfo->getShareWarAttitudeDivisor();
	if (getShareWarAttitudeChangeLimit() == 0) m_iShareWarAttitudeChangeLimit = pClassInfo->getShareWarAttitudeChangeLimit();
	if (getFavoriteCivicAttitudeChange() == 0) m_iFavoriteCivicAttitudeChange = pClassInfo->getFavoriteCivicAttitudeChange();
	if (getFavoriteCivicAttitudeDivisor() == 0) m_iFavoriteCivicAttitudeDivisor = pClassInfo->getFavoriteCivicAttitudeDivisor();
	if (getFavoriteCivicAttitudeChangeLimit() == 0) m_iFavoriteCivicAttitudeChangeLimit = pClassInfo->getFavoriteCivicAttitudeChangeLimit();
	if (getVassalPowerModifier() == 0) m_iVassalPowerModifier = pClassInfo->getVassalPowerModifier();
	if (getFreedomAppreciation() == 0) m_iFreedomAppreciation = pClassInfo->getFreedomAppreciation();

	if (getDemandTributeAttitudeThreshold() == -1) m_iDemandTributeAttitudeThreshold = pClassInfo->getDemandTributeAttitudeThreshold();
	if (getNoGiveHelpAttitudeThreshold() == -1) m_iNoGiveHelpAttitudeThreshold = pClassInfo->getNoGiveHelpAttitudeThreshold();
	if (getTechRefuseAttitudeThreshold() == -1) m_iTechRefuseAttitudeThreshold = pClassInfo->getTechRefuseAttitudeThreshold();
	if (getStrategicBonusRefuseAttitudeThreshold() == -1) m_iStrategicBonusRefuseAttitudeThreshold = pClassInfo->getStrategicBonusRefuseAttitudeThreshold();
	if (getHappinessBonusRefuseAttitudeThreshold() == -1) m_iHappinessBonusRefuseAttitudeThreshold = pClassInfo->getHappinessBonusRefuseAttitudeThreshold();
	if (getHealthBonusRefuseAttitudeThreshold() == -1) m_iHealthBonusRefuseAttitudeThreshold = pClassInfo->getHealthBonusRefuseAttitudeThreshold();
	if (getMapRefuseAttitudeThreshold() == -1) m_iMapRefuseAttitudeThreshold = pClassInfo->getMapRefuseAttitudeThreshold();
	if (getDeclareWarRefuseAttitudeThreshold() == -1) m_iDeclareWarRefuseAttitudeThreshold = pClassInfo->getDeclareWarRefuseAttitudeThreshold();
	if (getDeclareWarThemRefuseAttitudeThreshold() == -1) m_iDeclareWarThemRefuseAttitudeThreshold = pClassInfo->getDeclareWarThemRefuseAttitudeThreshold();
	if (getStopTradingRefuseAttitudeThreshold() == -1) m_iStopTradingRefuseAttitudeThreshold = pClassInfo->getStopTradingRefuseAttitudeThreshold();
	if (getStopTradingThemRefuseAttitudeThreshold() == -1) m_iStopTradingThemRefuseAttitudeThreshold = pClassInfo->getStopTradingThemRefuseAttitudeThreshold();
	if (getAdoptCivicRefuseAttitudeThreshold() == -1) m_iAdoptCivicRefuseAttitudeThreshold = pClassInfo->getAdoptCivicRefuseAttitudeThreshold();
	if (getConvertReligionRefuseAttitudeThreshold() == -1) m_iConvertReligionRefuseAttitudeThreshold = pClassInfo->getConvertReligionRefuseAttitudeThreshold();
	if (getOpenBordersRefuseAttitudeThreshold() == -1) m_iOpenBordersRefuseAttitudeThreshold = pClassInfo->getOpenBordersRefuseAttitudeThreshold();
	if (getDefensivePactRefuseAttitudeThreshold() == -1) m_iDefensivePactRefuseAttitudeThreshold = pClassInfo->getDefensivePactRefuseAttitudeThreshold();
	if (getPermanentAllianceRefuseAttitudeThreshold() == -1) m_iPermanentAllianceRefuseAttitudeThreshold = pClassInfo->getPermanentAllianceRefuseAttitudeThreshold();
	if (getVassalRefuseAttitudeThreshold() == -1) m_iVassalRefuseAttitudeThreshold = pClassInfo->getVassalRefuseAttitudeThreshold();
	if (getFavoriteCivic() == -1) m_iFavoriteCivic = pClassInfo->getFavoriteCivic();
	if (getFavoriteReligion() == -1) m_iFavoriteReligion = pClassInfo->getFavoriteReligion();

	for (int j = 0; j < GC.getNumTraitInfos(); j++)
	{
		if (!hasTrait(j) && pClassInfo->hasTrait(j))
		{
			if (!m_pbTraits)
			{
				CvXMLLoadUtility::InitList(&m_pbTraits, GC.getNumTraitInfos(), false);
			}
			m_pbTraits[j] = pClassInfo->hasTrait(j);
		}
	}

	for (int j = 0; j < GC.getNumFlavorTypes(); j++)
	{
		if (getFlavorValue(j) == 0 && pClassInfo->getFlavorValue(j) != 0)
		{
			if (!m_piFlavorValue)
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),0);
			}
			m_piFlavorValue[j] = pClassInfo->getFlavorValue(j);
		}
	}
	for (int j = 0; j < NUM_CONTACT_TYPES; j++)
	{
		if (m_piContactRand[j] == 0 && pClassInfo->getContactRand(j) != 0)
		{
			m_piContactRand[j] = pClassInfo->getContactRand(j);
		}
		if (m_piContactDelay[j] == 0 && pClassInfo->getContactDelay(j) != 0)
		{
			m_piContactDelay[j] = pClassInfo->getContactDelay(j);
		}
	}
	for (int j = 0; j < NUM_MEMORY_TYPES; j++)
	{
		if (m_piMemoryDecayRand[j] == 0 && pClassInfo->getMemoryDecayRand(j) != 0)
		{
			m_piMemoryDecayRand[j] = pClassInfo->getMemoryDecayRand(j);
		}
		if (m_piMemoryAttitudePercent[j] == 0 && pClassInfo->getMemoryAttitudePercent(j) != 0)
		{
			m_piMemoryAttitudePercent[j] = pClassInfo->getMemoryAttitudePercent(j);
		}
	}
	for (int j = 0; j < NUM_ATTITUDE_TYPES; j++)
	{
		if (getNoWarAttitudeProb(j) == 0 && pClassInfo->getNoWarAttitudeProb(j) != 0)
		{
			if (!m_piNoWarAttitudeProb)
			{
				CvXMLLoadUtility::InitList(&m_piNoWarAttitudeProb,NUM_ATTITUDE_TYPES,0);
			}
			m_piNoWarAttitudeProb[j] = pClassInfo->getNoWarAttitudeProb(j);
		}
	}
	for (int j = 0; j < NUM_UNITAI_TYPES; j++)
	{
		if (getUnitAIWeightModifier(j) == 0 && pClassInfo->getUnitAIWeightModifier(j) != 0)
		{
			if (!m_piUnitAIWeightModifier)
			{
				CvXMLLoadUtility::InitList(&m_piUnitAIWeightModifier,NUM_UNITAI_TYPES,0);
			}
			m_piUnitAIWeightModifier[j] = pClassInfo->getUnitAIWeightModifier(j);
		}
	}
	for (int j = 0; j < GC.getNumImprovementInfos(); j++)
	{
		if (getImprovementWeightModifier(j) == 0 && pClassInfo->getImprovementWeightModifier(j) != 0)
		{
			if (!m_piImprovementWeightModifier)
			{
				CvXMLLoadUtility::InitList(&m_piImprovementWeightModifier, GC.getNumImprovementInfos(),0);
			}
			m_piImprovementWeightModifier[j] = pClassInfo->getImprovementWeightModifier(j);
		}
	}

	for (int j = 0; j < GC.getNumEraInfos(); j++)
	{
		if (getDiploPeaceIntroMusicScriptIds(j) == -1 && pClassInfo->getDiploPeaceIntroMusicScriptIds(j) != -1)
		{
			if (!m_piDiploPeaceIntroMusicScriptIds)
			{
				CvXMLLoadUtility::InitList(&m_piDiploPeaceIntroMusicScriptIds, GC.getNumEraInfos(), -1);
			}
			m_piDiploPeaceIntroMusicScriptIds[j] = pClassInfo->getDiploPeaceIntroMusicScriptIds(j);
		}
		if (getDiploPeaceMusicScriptIds(j) == -1 && pClassInfo->getDiploPeaceMusicScriptIds(j) != -1)
		{
			if (!m_piDiploPeaceMusicScriptIds)
			{
				CvXMLLoadUtility::InitList(&m_piDiploPeaceMusicScriptIds, GC.getNumEraInfos(), -1);
			}
			m_piDiploPeaceMusicScriptIds[j] = pClassInfo->getDiploPeaceMusicScriptIds(j);
		}
		if (getDiploWarIntroMusicScriptIds(j) == -1 && pClassInfo->getDiploWarIntroMusicScriptIds(j) != -1)
		{
			if (!m_piDiploWarIntroMusicScriptIds)
			{
				CvXMLLoadUtility::InitList(&m_piDiploWarIntroMusicScriptIds, GC.getNumEraInfos(), -1);
			}
			m_piDiploWarIntroMusicScriptIds[j] = pClassInfo->getDiploWarIntroMusicScriptIds(j);
		}
		if (getDiploWarMusicScriptIds(j) == -1 && pClassInfo->getDiploWarMusicScriptIds(j) != -1)
		{
			if (!m_piDiploWarMusicScriptIds)
			{
				CvXMLLoadUtility::InitList(&m_piDiploWarMusicScriptIds, GC.getNumEraInfos(), -1);
			}
			m_piDiploWarMusicScriptIds[j] = pClassInfo->getDiploWarMusicScriptIds(j);
		}
	}

	if (getMilitaryUnitRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iMilitaryUnitRefuseAttitudeThreshold = pClassInfo->getMilitaryUnitRefuseAttitudeThreshold();
	if (getWorkerRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iWorkerRefuseAttitudeThreshold = pClassInfo->getWorkerRefuseAttitudeThreshold();
	if (getCorporationRefuseAttitudeThreshold() == ATTITUDE_CAUTIOUS) m_iCorporationRefuseAttitudeThreshold = pClassInfo->getCorporationRefuseAttitudeThreshold();
	if (getSecretaryGeneralVoteRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iSecretaryGeneralVoteRefuseAttitudeThreshold = pClassInfo->getSecretaryGeneralVoteRefuseAttitudeThreshold();
	if (getCultureVictoryWeight() == 0) m_iCultureVictoryWeight = pClassInfo->getCultureVictoryWeight();
	if (getSpaceVictoryWeight() == 0) m_iSpaceVictoryWeight = pClassInfo->getSpaceVictoryWeight();
	if (getConquestVictoryWeight() == 0) m_iConquestVictoryWeight = pClassInfo->getConquestVictoryWeight();
	if (getDominationVictoryWeight() == 0) m_iDominationVictoryWeight = pClassInfo->getDominationVictoryWeight();
	if (getDiplomacyVictoryWeight() == 0) m_iDiplomacyVictoryWeight = pClassInfo->getDiplomacyVictoryWeight();

	//Int list Vector without delayed resolution
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiDefaultTraits, pClassInfo->m_aiDefaultTraits);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiDefaultComplexTraits, pClassInfo->m_aiDefaultComplexTraits);
}


//I'm lazy, so sue me. The XML still overrides this, so no worries.
void CvLeaderHeadInfo::setDefaultMemoryInfo()
{
	PROFILE_EXTRA_FUNC();

	for (int i = 0; i < NUM_MEMORY_TYPES; i++)
	{
		if (m_piMemoryDecayRand[i] == 0)
		{
			switch (i)
			{
				case MEMORY_WARMONGER:
				case MEMORY_MADE_PEACE:
				{
					m_piMemoryDecayRand[i] = 1;
					break;
				}
				case MEMORY_RECALLED_AMBASSADOR:
				{
					m_piMemoryDecayRand[i] = 25;
					break;
				}
				case MEMORY_INQUISITION:
				{
					m_piMemoryDecayRand[i] = 75;
					break;
				}
				case MEMORY_ENSLAVED_CITIZENS:
				{
					m_piMemoryDecayRand[i] = 100;
					break;
				}
				case MEMORY_SACKED_CITY:
				{
					m_piMemoryDecayRand[i] = 125;
					break;
				}
				case MEMORY_SACKED_HOLY_CITY:
				{
					m_piMemoryDecayRand[i] = 200;
					break;
				}
				case MEMORY_BACKSTAB:
				case MEMORY_BACKSTAB_FRIEND:
				{
					m_piMemoryDecayRand[i] = 250;
				}
			}
		}
		if (m_piMemoryAttitudePercent[i] == 0)
		{
			switch (i)
			{
				case MEMORY_INQUISITION:
				{
					m_piMemoryAttitudePercent[i] = -100;
					break;
				}
				case MEMORY_BACKSTAB_FRIEND:
				{
					m_piMemoryAttitudePercent[i] = -150;
					break;
				}
				case MEMORY_SACKED_CITY:
				case MEMORY_ENSLAVED_CITIZENS:
				{
					m_piMemoryAttitudePercent[i] = -200;
					break;
				}
				case MEMORY_SACKED_HOLY_CITY:
				case MEMORY_BACKSTAB:
				{
					m_piMemoryAttitudePercent[i] = -400;
				}
			}
		}
	}
}

void CvLeaderHeadInfo::setDefaultContactInfo()
{
	PROFILE_EXTRA_FUNC();

	for (int i = 0; i < NUM_CONTACT_TYPES; i++)
	{
		if (m_piContactRand[i] == 0)
		{
			switch (i)
			{
				case CONTACT_TRADE_JOIN_WAR:
				case CONTACT_TRADE_BUY_WAR:
				{
					m_piContactRand[i] = 10;
					break;
				}
				case CONTACT_TRADE_CONTACTS:
				{
					m_piContactRand[i] = 15;
					break;
				}
				case CONTACT_TRADE_STOP_TRADING:
				case CONTACT_TRADE_MILITARY_UNITS:
				{
					m_piContactRand[i] = 20;
					break;
				}
				case CONTACT_EMBASSY:
				case CONTACT_SECRETARY_GENERAL_VOTE:
				case CONTACT_TRADE_WORKERS:
				{
					m_piContactRand[i] = 25;
					break;
				}
				case CONTACT_TRADE_CORPORATION:
				{
					m_piContactRand[i] = 35;
					break;
				}
				case CONTACT_PEACE_PRESSURE:
				{
					m_piContactRand[i] = 50;
				}
			}
		}
		if (m_piContactDelay[i] == 0)
		{
			switch (i)
			{
				case CONTACT_TRADE_BUY_WAR:
				{
					m_piContactDelay[i] = 10;
					break;
				}
				case CONTACT_EMBASSY:
				case CONTACT_TRADE_JOIN_WAR:
				case CONTACT_TRADE_CONTACTS:
				case CONTACT_TRADE_STOP_TRADING:
				{
					m_piContactDelay[i] = 20;
					break;
				}
				case CONTACT_SECRETARY_GENERAL_VOTE:
				case CONTACT_TRADE_MILITARY_UNITS:
				{
					m_piContactDelay[i] = 25;
					break;
				}
				case CONTACT_PEACE_PRESSURE:
				case CONTACT_TRADE_WORKERS:
				{
					m_piContactDelay[i] = 30;
					break;
				}
				case CONTACT_TRADE_CORPORATION:
				{
					m_piContactDelay[i] = 50;
				}
			}
		}
	}
}


//======================================================================================================
//					CvWorldInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWorldInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWorldInfo::CvWorldInfo() :
 m_iDefaultPlayers(0)
,m_iUnitNameModifier(0)
,m_iTargetNumCities(0)
,m_iBuildingPrereqModifier(0)
,m_iMaxConscriptModifier(0)
,m_iWarWearinessModifier(0)
,m_iGridWidth(0)
,m_iGridHeight(0)
,m_iTerrainGrainChange(0)
,m_iFeatureGrainChange(0)
,m_iTradeProfitPercent(0)
,m_iDistanceMaintenancePercent(0)
,m_iNumCitiesMaintenancePercent(0)
,m_iColonyMaintenancePercent(0)
,m_iCorporationMaintenancePercent(0)
,m_iNumCitiesAnarchyPercent(0)
,m_iAdvancedStartPointsMod(0)
,m_iCommandersLevelThresholdsPercent(0)
,m_iOceanMinAreaSize(0)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWorldInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWorldInfo::~CvWorldInfo()
{
}

int CvWorldInfo::getDefaultPlayers() const
{
	return m_iDefaultPlayers;
}

int CvWorldInfo::getUnitNameModifier() const
{
	return m_iUnitNameModifier;
}

int CvWorldInfo::getTargetNumCities() const
{
	return m_iTargetNumCities;
}

int CvWorldInfo::getBuildingPrereqModifier() const
{
	return m_iBuildingPrereqModifier;
}

int CvWorldInfo::getMaxConscriptModifier() const
{
	return m_iMaxConscriptModifier;
}

int CvWorldInfo::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}

int CvWorldInfo::getGridWidth() const
{
	return m_iGridWidth;
}

int CvWorldInfo::getGridHeight() const
{
	return m_iGridHeight;
}

int CvWorldInfo::getTerrainGrainChange() const
{
	return m_iTerrainGrainChange;
}

int CvWorldInfo::getFeatureGrainChange() const
{
	return m_iFeatureGrainChange;
}

int CvWorldInfo::getTradeProfitPercent() const
{
	return m_iTradeProfitPercent;
}

int CvWorldInfo::getDistanceMaintenancePercent() const
{
	return m_iDistanceMaintenancePercent;
}

int CvWorldInfo::getNumCitiesMaintenancePercent() const
{
	return m_iNumCitiesMaintenancePercent;
}

int CvWorldInfo::getColonyMaintenancePercent() const
{
	return m_iColonyMaintenancePercent;
}

int CvWorldInfo::getCorporationMaintenancePercent() const
{
	return m_iCorporationMaintenancePercent;
}

int CvWorldInfo::getNumCitiesAnarchyPercent() const
{
	return m_iNumCitiesAnarchyPercent;
}

int CvWorldInfo::getAdvancedStartPointsMod() const
{
	return m_iAdvancedStartPointsMod;
}

int CvWorldInfo::getCommandersLevelThresholdsPercent() const
{
	return m_iCommandersLevelThresholdsPercent;
}

int CvWorldInfo::getOceanMinAreaSize() const
{
	return m_iOceanMinAreaSize;
}

int CvWorldInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}

bool CvWorldInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iDefaultPlayers, L"iDefaultPlayers");
	pXML->GetOptionalChildXmlValByName(&m_iUnitNameModifier, L"iUnitNameModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTargetNumCities, L"iTargetNumCities");
	pXML->GetOptionalChildXmlValByName(&m_iBuildingPrereqModifier, L"iBuildingPrereqModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxConscriptModifier, L"iMaxConscriptModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessModifier, L"iWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGridWidth, L"iGridWidth");
	pXML->GetOptionalChildXmlValByName(&m_iGridHeight, L"iGridHeight");
	pXML->GetOptionalChildXmlValByName(&m_iTerrainGrainChange, L"iTerrainGrainChange");
	pXML->GetOptionalChildXmlValByName(&m_iFeatureGrainChange, L"iFeatureGrainChange");
	pXML->GetOptionalChildXmlValByName(&m_iTradeProfitPercent, L"iTradeProfitPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenancePercent, L"iDistanceMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenancePercent, L"iNumCitiesMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iColonyMaintenancePercent, L"iColonyMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenancePercent, L"iCorporationMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesAnarchyPercent, L"iNumCitiesAnarchyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartPointsMod, L"iAdvancedStartPointsMod");
	pXML->GetOptionalChildXmlValByName(&m_iCommandersLevelThresholdsPercent, L"iCommandersLevelThresholdsPercent");
	pXML->GetOptionalChildXmlValByName(&m_iOceanMinAreaSize, L"iOceanMinAreaSize");

	m_Percent.read(pXML, L"Percents");

	return true;
}

void CvWorldInfo::copyNonDefaults(const CvWorldInfo* pClassInfo)
{
	int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefaultPlayers() == iDefault) m_iDefaultPlayers = pClassInfo->getDefaultPlayers();
	if (getUnitNameModifier() == iDefault) m_iUnitNameModifier = pClassInfo->getUnitNameModifier();
	if (getTargetNumCities() == iDefault) m_iTargetNumCities = pClassInfo->getTargetNumCities();
	if (getBuildingPrereqModifier() == iDefault) m_iBuildingPrereqModifier = pClassInfo->getBuildingPrereqModifier();
	if (getMaxConscriptModifier() == iDefault) m_iMaxConscriptModifier = pClassInfo->getMaxConscriptModifier();
	if (getWarWearinessModifier() == iDefault) m_iWarWearinessModifier = pClassInfo->getWarWearinessModifier();
	if (getGridWidth() == iDefault) m_iGridWidth = pClassInfo->getGridWidth();
	if (getGridHeight() == iDefault) m_iGridHeight = pClassInfo->getGridHeight();
	if (getTerrainGrainChange() == iDefault) m_iTerrainGrainChange = pClassInfo->getTerrainGrainChange();
	if (getFeatureGrainChange() == iDefault) m_iFeatureGrainChange = pClassInfo->getFeatureGrainChange();
	if (getTradeProfitPercent() == iDefault) m_iTradeProfitPercent = pClassInfo->getTradeProfitPercent();
	if (getDistanceMaintenancePercent() == iDefault) m_iDistanceMaintenancePercent = pClassInfo->getDistanceMaintenancePercent();
	if (getNumCitiesMaintenancePercent() == iDefault) m_iNumCitiesMaintenancePercent = pClassInfo->getNumCitiesMaintenancePercent();
	if (getColonyMaintenancePercent() == iDefault) m_iColonyMaintenancePercent = pClassInfo->getColonyMaintenancePercent();
	if (getCorporationMaintenancePercent() == iDefault) m_iCorporationMaintenancePercent = pClassInfo->getCorporationMaintenancePercent();
	if (getNumCitiesAnarchyPercent() == iDefault) m_iNumCitiesAnarchyPercent = pClassInfo->getNumCitiesAnarchyPercent();
	if (getAdvancedStartPointsMod() == iDefault) m_iAdvancedStartPointsMod = pClassInfo->getAdvancedStartPointsMod();
	if (getCommandersLevelThresholdsPercent() == iDefault) m_iCommandersLevelThresholdsPercent = pClassInfo->getCommandersLevelThresholdsPercent();
	if (m_iOceanMinAreaSize == iDefault) m_iOceanMinAreaSize = pClassInfo->getOceanMinAreaSize();

	m_Percent.copyNonDefaults(pClassInfo->m_Percent);
}

void CvWorldInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iDefaultPlayers);
	CheckSum(iSum, m_iUnitNameModifier);
	CheckSum(iSum, m_iTargetNumCities);
	CheckSum(iSum, m_iBuildingPrereqModifier);
	CheckSum(iSum, m_iMaxConscriptModifier);
	CheckSum(iSum, m_iWarWearinessModifier);
	CheckSum(iSum, m_iGridWidth);
	CheckSum(iSum, m_iGridHeight);
	CheckSum(iSum, m_iTerrainGrainChange);
	CheckSum(iSum, m_iFeatureGrainChange);
	CheckSum(iSum, m_iTradeProfitPercent);
	CheckSum(iSum, m_iDistanceMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesMaintenancePercent);
	CheckSum(iSum, m_iColonyMaintenancePercent);
	CheckSum(iSum, m_iCorporationMaintenancePercent);
	CheckSum(iSum, m_iNumCitiesAnarchyPercent);
	CheckSum(iSum, m_iAdvancedStartPointsMod);
	CheckSum(iSum, m_iCommandersLevelThresholdsPercent);
	CheckSum(iSum, m_iOceanMinAreaSize);
	CheckSumC(iSum, m_Percent);
}

//======================================================================================================
//					CvMapInfo
//======================================================================================================

CvMapInfo::CvMapInfo()
	: m_iGridWidth(0)
	, m_iGridHeight(0)
	, m_iWrapX(-1)
	, m_iWrapY(-1)
	, m_szInitialWBMap(NULL)
	, m_szMapScript(NULL)
{
}

CvMapInfo::~CvMapInfo()
{
}

bool CvMapInfo::read(CvXMLLoadUtility* pXML)
{
	CvInfoBase::read(pXML);

	pXML->GetOptionalChildXmlValByName(&m_iGridWidth, L"iGridWidth");
	pXML->GetOptionalChildXmlValByName(&m_iGridHeight, L"iGridHeight");
	pXML->GetOptionalChildXmlValByName(&m_iWrapX, L"bWrapX");
	pXML->GetOptionalChildXmlValByName(&m_iWrapY, L"bWrapY");
	pXML->GetOptionalChildXmlValByName(m_szInitialWBMap, L"InitialWBMap");
	pXML->GetOptionalChildXmlValByName(m_szMapScript, L"MapScript");

	return true;
}

//======================================================================================================
//					CvClimateInfo
//======================================================================================================
CvClimateInfo::CvClimateInfo() :
m_iDesertPercentChange(0),
m_iJungleLatitude(0),
m_iHillRange(0),
m_iPeakPercent(0),
m_fSnowLatitudeChange(0.0f),
m_fTundraLatitudeChange(0.0f),
m_fGrassLatitudeChange(0.0f),
m_fDesertBottomLatitudeChange(0.0f),
m_fDesertTopLatitudeChange(0.0f),
m_fIceLatitude(0.0f),
m_fRandIceLatitude(0.0f)
{
}

CvClimateInfo::~CvClimateInfo()
{
}

int CvClimateInfo::getDesertPercentChange() const
{
	return m_iDesertPercentChange;
}

int CvClimateInfo::getJungleLatitude() const
{
	return m_iJungleLatitude;
}

int CvClimateInfo::getHillRange() const
{
	return m_iHillRange;
}

int CvClimateInfo::getPeakPercent() const
{
	return m_iPeakPercent;
}

float CvClimateInfo::getSnowLatitudeChange() const
{
	return m_fSnowLatitudeChange;
}

float CvClimateInfo::getTundraLatitudeChange() const
{
	return m_fTundraLatitudeChange;
}

float CvClimateInfo::getGrassLatitudeChange() const
{
	return m_fGrassLatitudeChange;
}

float CvClimateInfo::getDesertBottomLatitudeChange() const
{
	return m_fDesertBottomLatitudeChange;
}

float CvClimateInfo::getDesertTopLatitudeChange() const
{
	return m_fDesertTopLatitudeChange;
}

float CvClimateInfo::getIceLatitude() const
{
	return m_fIceLatitude;
}

float CvClimateInfo::getRandIceLatitude() const
{
	return m_fRandIceLatitude;
}

bool CvClimateInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iDesertPercentChange, L"iDesertPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iJungleLatitude, L"iJungleLatitude");
	pXML->GetOptionalChildXmlValByName(&m_iHillRange, L"iHillRange");
	pXML->GetOptionalChildXmlValByName(&m_iPeakPercent, L"iPeakPercent");

	pXML->GetOptionalChildXmlValByName(&m_fSnowLatitudeChange, L"fSnowLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fTundraLatitudeChange, L"fTundraLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fGrassLatitudeChange, L"fGrassLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fDesertBottomLatitudeChange, L"fDesertBottomLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fDesertTopLatitudeChange, L"fDesertTopLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fIceLatitude, L"fIceLatitude");
	pXML->GetOptionalChildXmlValByName(&m_fRandIceLatitude, L"fRandIceLatitude");

	return true;
}

void CvClimateInfo::copyNonDefaults(const CvClimateInfo* pClassInfo)
{
	int iDefault = 0;
	float fDefault = 0.0f;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDesertPercentChange() == iDefault) m_iDesertPercentChange = pClassInfo->getDesertPercentChange();
	if (getJungleLatitude() == iDefault) m_iJungleLatitude = pClassInfo->getJungleLatitude();
	if (getHillRange() == iDefault) m_iHillRange = pClassInfo->getHillRange();
	if (getPeakPercent() == iDefault) m_iPeakPercent = pClassInfo->getPeakPercent();

	if (getSnowLatitudeChange() == fDefault) m_fSnowLatitudeChange = pClassInfo->getSnowLatitudeChange();
	if (getTundraLatitudeChange() == fDefault) m_fTundraLatitudeChange = pClassInfo->getTundraLatitudeChange();
	if (getGrassLatitudeChange() == fDefault) m_fGrassLatitudeChange = pClassInfo->getGrassLatitudeChange();
	if (getDesertBottomLatitudeChange() == fDefault) m_fDesertBottomLatitudeChange = pClassInfo->getDesertBottomLatitudeChange();
	if (getDesertTopLatitudeChange() == fDefault) m_fDesertTopLatitudeChange = pClassInfo->getDesertTopLatitudeChange();
	if (getIceLatitude() == fDefault) m_fIceLatitude = pClassInfo->getIceLatitude();
	if (getRandIceLatitude() == fDefault) m_fRandIceLatitude = pClassInfo->getRandIceLatitude();
}

//======================================================================================================
//					CvSeaLevelInfo
//======================================================================================================
CvSeaLevelInfo::CvSeaLevelInfo() :
m_iSeaLevelChange(0)
{
}

CvSeaLevelInfo::~CvSeaLevelInfo()
{
}

int CvSeaLevelInfo::getSeaLevelChange() const
{
	return m_iSeaLevelChange;
}

bool CvSeaLevelInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iSeaLevelChange, L"iSeaLevelChange");

	return true;
}

void CvSeaLevelInfo::copyNonDefaults(const CvSeaLevelInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getSeaLevelChange() == iDefault) m_iSeaLevelChange = pClassInfo->getSeaLevelChange();
}

//======================================================================================================
//					CvProcessInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvProcessInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvProcessInfo::CvProcessInfo() :
m_iTechPrereq(NO_TECH),
m_paiProductionToCommerceModifier(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvProcessInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvProcessInfo::~CvProcessInfo()
{
	SAFE_DELETE_ARRAY(m_paiProductionToCommerceModifier);
}

int CvProcessInfo::getProductionToCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiProductionToCommerceModifier ? m_paiProductionToCommerceModifier[i] : 0;
}

bool CvProcessInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

	if (pXML->TryMoveToXmlFirstChild(L"ProductionToCommerceModifiers"))
	{
		pXML->SetCommerce(&m_paiProductionToCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiProductionToCommerceModifier);
	}

	return true;
}

void CvProcessInfo::copyNonDefaults(const CvProcessInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();

	if (!m_paiProductionToCommerceModifier) CvXMLLoadUtility::InitList(&m_paiProductionToCommerceModifier, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_paiProductionToCommerceModifier[i] == iDefault )
		{
			m_paiProductionToCommerceModifier[i] = pClassInfo->getProductionToCommerceModifier(i);
		}
	}
}

void CvProcessInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iTechPrereq);

	// Arrays

	CheckSum(iSum, m_paiProductionToCommerceModifier, NUM_COMMERCE_TYPES);
}

//======================================================================================================
//					CvVoteInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvVoteInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvVoteInfo::CvVoteInfo() :
m_iPopulationThreshold(0),
m_iStateReligionVotePercent(0),
m_iTradeRoutes(0),
m_iMinVoters(0),
m_bSecretaryGeneral(false),
m_bVictory(false),
m_bFreeTrade(false),
m_bNoNukes(false),
m_bCityVoting(false),
m_bCivVoting(false),
m_bDefensivePact(false),
m_bOpenBorders(false),
m_bForcePeace(false),
m_bForceNoTrade(false),
m_bForceWar(false),
m_bAssignCity(false),
m_pbForceCivic(NULL),
m_abVoteSourceTypes(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvVoteInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvVoteInfo::~CvVoteInfo()
{
	SAFE_DELETE_ARRAY(m_pbForceCivic);
	SAFE_DELETE_ARRAY(m_abVoteSourceTypes);
}

int CvVoteInfo::getPopulationThreshold() const
{
	return m_iPopulationThreshold;
}

int CvVoteInfo::getStateReligionVotePercent() const
{
	return m_iStateReligionVotePercent;
}

int CvVoteInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}

int CvVoteInfo::getMinVoters() const
{
	return m_iMinVoters;
}

bool CvVoteInfo::isSecretaryGeneral() const
{
	return m_bSecretaryGeneral;
}

bool CvVoteInfo::isVictory() const
{
	return m_bVictory;
}

bool CvVoteInfo::isFreeTrade() const
{
	return m_bFreeTrade;
}

bool CvVoteInfo::isNoNukes() const
{
	return m_bNoNukes;
}

bool CvVoteInfo::isCityVoting() const
{
	return m_bCityVoting;
}

bool CvVoteInfo::isCivVoting() const
{
	return m_bCivVoting;
}

bool CvVoteInfo::isDefensivePact() const
{
	return m_bDefensivePact;
}

bool CvVoteInfo::isOpenBorders() const
{
	return m_bOpenBorders;
}

bool CvVoteInfo::isForcePeace() const
{
	return m_bForcePeace;
}

bool CvVoteInfo::isForceNoTrade() const
{
	return m_bForceNoTrade;
}

bool CvVoteInfo::isForceWar() const
{
	return m_bForceWar;
}

bool CvVoteInfo::isAssignCity() const
{
	return m_bAssignCity;
}

bool CvVoteInfo::isForceCivic(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), i);
	return m_pbForceCivic ? m_pbForceCivic[i] : false;
}

bool CvVoteInfo::isVoteSourceType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), i);
	return m_abVoteSourceTypes ? m_abVoteSourceTypes[i] : false;
}

bool CvVoteInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iPopulationThreshold, L"iPopulationThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionVotePercent, L"iStateReligionVotePercent");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iMinVoters, L"iMinVoters");

	pXML->GetOptionalChildXmlValByName(&m_bSecretaryGeneral, L"bSecretaryGeneral");
	pXML->GetOptionalChildXmlValByName(&m_bVictory, L"bVictory");
	pXML->GetOptionalChildXmlValByName(&m_bFreeTrade, L"bFreeTrade");
	pXML->GetOptionalChildXmlValByName(&m_bNoNukes, L"bNoNukes");
	pXML->GetOptionalChildXmlValByName(&m_bCityVoting, L"bCityVoting");
	pXML->GetOptionalChildXmlValByName(&m_bCivVoting, L"bCivVoting");
	pXML->GetOptionalChildXmlValByName(&m_bDefensivePact, L"bDefensivePact");
	pXML->GetOptionalChildXmlValByName(&m_bOpenBorders, L"bOpenBorders");
	pXML->GetOptionalChildXmlValByName(&m_bForcePeace, L"bForcePeace");
	pXML->GetOptionalChildXmlValByName(&m_bForceNoTrade, L"bForceNoTrade");
	pXML->GetOptionalChildXmlValByName(&m_bForceWar, L"bForceWar");
	pXML->GetOptionalChildXmlValByName(&m_bAssignCity, L"bAssignCity");

	pXML->SetVariableListTagPair(&m_pbForceCivic, L"ForceCivics",  GC.getNumCivicInfos());
	pXML->SetVariableListTagPair(&m_abVoteSourceTypes, L"DiploVotes",  GC.getNumVoteSourceInfos());

	return true;
}

void CvVoteInfo::copyNonDefaults(const CvVoteInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPopulationThreshold() == iDefault) m_iPopulationThreshold = pClassInfo->getPopulationThreshold();
	if (getStateReligionVotePercent() == iDefault) m_iStateReligionVotePercent = pClassInfo->getStateReligionVotePercent();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getMinVoters() == iDefault) m_iMinVoters = pClassInfo->getMinVoters();
	if (isSecretaryGeneral() == bDefault) m_bSecretaryGeneral = pClassInfo->isSecretaryGeneral();
	if (isVictory() == bDefault) m_bVictory = pClassInfo->isVictory();
	if (isFreeTrade() == bDefault) m_bFreeTrade = pClassInfo->isFreeTrade();
	if (isNoNukes() == bDefault) m_bNoNukes = pClassInfo->isNoNukes();
	if (isCityVoting() == bDefault) m_bCityVoting = pClassInfo->isCityVoting();
	if (isCivVoting() == bDefault) m_bCivVoting = pClassInfo->isCivVoting();
	if (isDefensivePact() == bDefault) m_bDefensivePact = pClassInfo->isDefensivePact();
	if (isOpenBorders() == bDefault) m_bOpenBorders = pClassInfo->isOpenBorders();
	if (isForcePeace() == bDefault) m_bForcePeace = pClassInfo->isForcePeace();
	if (isForceNoTrade() == bDefault) m_bForceNoTrade = pClassInfo->isForceNoTrade();
	if (isForceWar() == bDefault) m_bForceWar = pClassInfo->isForceWar();
	if (isAssignCity() == bDefault) m_bAssignCity = pClassInfo->isAssignCity();

	for ( int i = 0; i < GC.getNumCivicInfos(); i++ )
	{
		if ( isForceCivic(i) == bDefault && pClassInfo->isForceCivic(i) != bDefault)
		{
			if ( NULL == m_pbForceCivic )
			{
				CvXMLLoadUtility::InitList(&m_pbForceCivic,GC.getNumCivicInfos(),bDefault);
			}
			m_pbForceCivic[i] = pClassInfo->isForceCivic(i);
		}
	}

	for ( int i = 0; i < GC.getNumVoteSourceInfos(); i++ )
	{
		if ( isVoteSourceType(i) == bDefault && pClassInfo->isVoteSourceType(i) != bDefault)
		{
			if ( NULL == m_abVoteSourceTypes )
			{
				CvXMLLoadUtility::InitList(&m_abVoteSourceTypes,GC.getNumVoteSourceInfos(),bDefault);
			}
			m_abVoteSourceTypes[i] = pClassInfo->isVoteSourceType(i);
		}
	}
}

void CvVoteInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iPopulationThreshold);
	CheckSum(iSum, m_iStateReligionVotePercent);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iMinVoters);

	CheckSum(iSum, m_bSecretaryGeneral);
	CheckSum(iSum, m_bVictory);
	CheckSum(iSum, m_bFreeTrade);
	CheckSum(iSum, m_bNoNukes);
	CheckSum(iSum, m_bCityVoting);
	CheckSum(iSum, m_bCivVoting);
	CheckSum(iSum, m_bDefensivePact);
	CheckSum(iSum, m_bOpenBorders);
	CheckSum(iSum, m_bForcePeace);
	CheckSum(iSum, m_bForceNoTrade);
	CheckSum(iSum, m_bForceWar);
	CheckSum(iSum, m_bAssignCity);

	// Arrays

	CheckSum(iSum, m_pbForceCivic, GC.getNumCivicInfos());
	CheckSum(iSum, m_abVoteSourceTypes, GC.getNumVoteSourceInfos());
}

//======================================================================================================
//					CvProjectInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvProjectInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvProjectInfo::CvProjectInfo() :
	m_iVictoryPrereq(NO_VICTORY),
	m_iTechPrereq(NO_TECH),
	m_iAnyoneProjectPrereq(NO_PROJECT),
	m_iMaxGlobalInstances(-1),
	m_iMaxTeamInstances(-1),
	m_iProductionCost(0),
	m_iNukeInterception(0),
	m_iTechShare(0),

	m_iGlobalMaintenanceModifier(0),
	m_iDistanceMaintenanceModifier(0),
	m_iNumCitiesMaintenanceModifier(0),
	m_iConnectedCityMaintenanceModifier(0),

	m_iEveryoneSpecialUnit(NO_SPECIALUNIT),
	m_iEveryoneSpecialBuilding(NO_SPECIALBUILDING),
	m_iVictoryDelayPercent(0),
	m_iSuccessRate(0),
	m_bSpaceship(false),
	m_bAllowsNukes(false),
	m_piBonusProductionModifier(NULL),
	m_piVictoryThreshold(NULL),
	m_piVictoryMinThreshold(NULL),
	m_piProjectsNeeded(NULL)

	,m_iWorldHappiness(0)
	,m_iGlobalHappiness(0)
	,m_iWorldHealth(0)
	,m_iGlobalHealth(0)
	,m_iWorldTradeRoutes(0)
	,m_iInflationModifier(0)
	,m_bTechShareWithHalfCivs(false)
	,m_piCommerceModifier(NULL)
{}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvProjectInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvProjectInfo::~CvProjectInfo()
{
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
	SAFE_DELETE_ARRAY(m_piVictoryMinThreshold);
	SAFE_DELETE_ARRAY(m_piProjectsNeeded);

}

int CvProjectInfo::getVictoryPrereq() const
{
	return m_iVictoryPrereq;
}

int CvProjectInfo::getAnyoneProjectPrereq() const
{
	return m_iAnyoneProjectPrereq;
}

void CvProjectInfo::setAnyoneProjectPrereq(int i)
{
	m_iAnyoneProjectPrereq = i;
}

int CvProjectInfo::getMaxGlobalInstances() const
{
	return m_iMaxGlobalInstances;
}

int CvProjectInfo::getMaxTeamInstances() const
{
	return m_iMaxTeamInstances;
}

int CvProjectInfo::getProductionCost() const
{
	return m_iProductionCost;
}

int CvProjectInfo::getNukeInterception() const
{
	return m_iNukeInterception;
}

int CvProjectInfo::getTechShare() const
{
	return m_iTechShare;
}

//DPII < Maintenance Modifiers >
int CvProjectInfo::getGlobalMaintenanceModifier() const
{
	return m_iGlobalMaintenanceModifier;
}

int CvProjectInfo::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}

int CvProjectInfo::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;

}

int CvProjectInfo::getConnectedCityMaintenanceModifier() const
{
	return m_iConnectedCityMaintenanceModifier;
}
//DPII < Maintenance Modifiers >

int CvProjectInfo::getEveryoneSpecialUnit() const
{
	return m_iEveryoneSpecialUnit;
}

int CvProjectInfo::getEveryoneSpecialBuilding() const
{
	return m_iEveryoneSpecialBuilding;
}

int CvProjectInfo::getVictoryDelayPercent() const
{
	return m_iVictoryDelayPercent;
}

int CvProjectInfo::getSuccessRate() const
{
	return m_iSuccessRate;
}

bool CvProjectInfo::isSpaceship() const
{
	return m_bSpaceship;
}

bool CvProjectInfo::isAllowsNukes() const
{
	return m_bAllowsNukes;
}

const char* CvProjectInfo::getMovieArtDef() const
{
	return m_szMovieArtDef;
}

const char* CvProjectInfo::getCreateSound() const
{
	return m_szCreateSound;
}

void CvProjectInfo::setCreateSound(const char* szVal)
{
	m_szCreateSound = szVal;
}

// Arrays

int CvProjectInfo::getBonusProductionModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_piBonusProductionModifier ? m_piBonusProductionModifier[i] : 0;
}

int CvProjectInfo::getVictoryThreshold(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), i);
	return m_piVictoryThreshold ? m_piVictoryThreshold[i] : 0;
}

int CvProjectInfo::getVictoryMinThreshold(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), i);

	if (m_piVictoryMinThreshold && m_piVictoryMinThreshold[i] != 0)
	{
		return m_piVictoryMinThreshold[i];
	}
	return getVictoryThreshold(i);
}

int CvProjectInfo::getProjectsNeeded(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), i);
	return m_piProjectsNeeded ? m_piProjectsNeeded[i] : false;
}

int CvProjectInfo::getWorldHappiness() const
{
	return m_iWorldHappiness;
}

int CvProjectInfo::getGlobalHappiness() const
{
	return m_iGlobalHappiness;
}

int CvProjectInfo::getWorldHealth() const
{
	return m_iWorldHealth;
}

int CvProjectInfo::getGlobalHealth() const
{
	return m_iGlobalHealth;
}

int CvProjectInfo::getWorldTradeRoutes() const
{
	return m_iWorldTradeRoutes;
}

int CvProjectInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}

bool CvProjectInfo::isTechShareWithHalfCivs() const
{
	return m_bTechShareWithHalfCivs;
}

int CvProjectInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int* CvProjectInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}

int CvProjectInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvProjectInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvProjectInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

int CvProjectInfo::getProjectsNeededVectorSize() const						{ return m_aszProjectsNeededforPass3.size(); }
CvString CvProjectInfo::getProjectsNeededNamesVectorElement(int i) const	{ return m_aszProjectsNeededforPass3[i]; }
int CvProjectInfo::getProjectsNeededValuesVectorElement(int i) const		{ return m_aiProjectsNeededforPass3[i]; }


bool CvProjectInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"VictoryPrereq");
	m_iVictoryPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGlobalInstances, L"iMaxGlobalInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxTeamInstances, L"iMaxTeamInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iProductionCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_iNukeInterception, L"iNukeInterception");
	pXML->GetOptionalChildXmlValByName(&m_iTechShare, L"iTechShare");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalMaintenanceModifier, L"iGlobalMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iConnectedCityMaintenanceModifier, L"iConnectedCityMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"EveryoneSpecialUnit");
	m_iEveryoneSpecialUnit = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EveryoneSpecialBuilding");
	m_iEveryoneSpecialBuilding = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bSpaceship, L"bSpaceship");
	pXML->GetOptionalChildXmlValByName(&m_bAllowsNukes, L"bAllowsNukes");
	pXML->GetOptionalChildXmlValByName(m_szMovieArtDef, L"MovieDefineTag");

	pXML->SetVariableListTagPair(&m_piBonusProductionModifier, L"BonusProductionModifiers", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piVictoryThreshold, L"VictoryThresholds",  GC.getNumVictoryInfos());
	pXML->SetVariableListTagPair(&m_piVictoryMinThreshold, L"VictoryMinThresholds",  GC.getNumVictoryInfos());
	pXML->GetOptionalChildXmlValByName(&m_iVictoryDelayPercent, L"iVictoryDelayPercent");
	pXML->GetOptionalChildXmlValByName(&m_iSuccessRate, L"iSuccessRate");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CreateSound");
	setCreateSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iWorldHappiness, L"iWorldHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHappiness, L"iGlobalHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iWorldHealth, L"iWorldHealth");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHealth, L"iGlobalHealth");
	pXML->GetOptionalChildXmlValByName(&m_iWorldTradeRoutes, L"iWorldTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_bTechShareWithHalfCivs, L"bTechShareWithHalfCivs");
	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflationModifier");

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"PrereqProjects"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszProjectsNeededforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiProjectsNeededforPass3.push_back(iTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"AnyonePrereqProject");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	return true;
}


void CvProjectInfo::copyNonDefaults(const CvProjectInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getVictoryPrereq() == iTextDefault) m_iVictoryPrereq = pClassInfo->getVictoryPrereq();
	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getEveryoneSpecialUnit() == iTextDefault) m_iEveryoneSpecialUnit = pClassInfo->getEveryoneSpecialUnit();
	if (getEveryoneSpecialBuilding() == iTextDefault) m_iEveryoneSpecialBuilding = pClassInfo->getEveryoneSpecialBuilding();

	if (getMaxGlobalInstances() == -1) m_iMaxGlobalInstances = pClassInfo->getMaxGlobalInstances();
	if (getMaxTeamInstances() == -1) m_iMaxTeamInstances = pClassInfo->getMaxTeamInstances();
	if (getProductionCost() == iDefault) m_iProductionCost = pClassInfo->getProductionCost();
	if (getNukeInterception() == iDefault) m_iNukeInterception = pClassInfo->getNukeInterception();
	if (getTechShare() == iDefault) m_iTechShare = pClassInfo->getTechShare();

	if (isSpaceship() == bDefault) m_bSpaceship = pClassInfo->isSpaceship();
	if (isAllowsNukes() == bDefault) m_bAllowsNukes = pClassInfo->isAllowsNukes();

	if (getMovieArtDef() == cDefault) m_szMovieArtDef = pClassInfo->getMovieArtDef();

	for ( int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		if (getBonusProductionModifier(i) == iDefault && pClassInfo->getBonusProductionModifier(i) != iDefault)
		{
			if ( NULL == m_piBonusProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_piBonusProductionModifier,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusProductionModifier[i] = pClassInfo->getBonusProductionModifier(i);
		}
	}
	for ( int i = 0; i < GC.getNumVictoryInfos(); i++)
	{
		if (getVictoryThreshold(i) == iDefault && pClassInfo->getVictoryThreshold(i) != iDefault)
		{
			if ( NULL == m_piVictoryThreshold )
			{
				CvXMLLoadUtility::InitList(&m_piVictoryThreshold,GC.getNumVictoryInfos(),iDefault);
			}
			m_piVictoryThreshold[i] = pClassInfo->getVictoryThreshold(i);
		}
		if (getVictoryMinThreshold(i) == iDefault && pClassInfo->getVictoryMinThreshold(i) != iDefault)
		{
			if ( NULL == m_piVictoryMinThreshold )
			{
				CvXMLLoadUtility::InitList(&m_piVictoryMinThreshold,GC.getNumVictoryInfos(),iDefault);
			}
			m_piVictoryMinThreshold[i] = pClassInfo->getVictoryMinThreshold(i);
		}
	}
	if (getVictoryDelayPercent() == iDefault) m_iVictoryDelayPercent = pClassInfo->getVictoryDelayPercent();
	if (getSuccessRate() == iDefault) m_iSuccessRate = pClassInfo->getSuccessRate();

	if (getCreateSound() == cDefault) setCreateSound(pClassInfo->getCreateSound());


	if (m_iAnyoneProjectPrereq == iTextDefault) m_iAnyoneProjectPrereq = pClassInfo->getAnyoneProjectPrereq();

	if (getWorldHappiness() == iDefault) m_iWorldHappiness = pClassInfo->getWorldHappiness();
	if (getGlobalHappiness() == iDefault) m_iGlobalHappiness= pClassInfo->getGlobalHappiness();
	if (getWorldHealth() == iDefault) m_iWorldHealth = pClassInfo->getWorldHealth();
	if (getGlobalHealth() == iDefault) m_iGlobalHealth = pClassInfo->getGlobalHealth();
	if (isTechShareWithHalfCivs() == bDefault) m_bTechShareWithHalfCivs = pClassInfo->isTechShareWithHalfCivs();
	if (getWorldTradeRoutes() == iDefault) m_iWorldTradeRoutes = pClassInfo->getWorldTradeRoutes();
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();

	if (!m_iGlobalMaintenanceModifier) m_iGlobalMaintenanceModifier = pClassInfo->getGlobalMaintenanceModifier();
	if (!m_iDistanceMaintenanceModifier) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (!m_iNumCitiesMaintenanceModifier) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (!m_iConnectedCityMaintenanceModifier) m_iConnectedCityMaintenanceModifier = pClassInfo->getConnectedCityMaintenanceModifier();

	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( m_piCommerceModifier[j] == iDefault )
		{
			m_piCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
	}

	for ( int i = 0; i < pClassInfo->getProjectsNeededVectorSize(); i++ )
	{
		m_aiProjectsNeededforPass3.push_back(pClassInfo->getProjectsNeededValuesVectorElement(i));
		m_aszProjectsNeededforPass3.push_back(pClassInfo->getProjectsNeededNamesVectorElement(i));
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
}

bool CvProjectInfo::readPass3()
{
	PROFILE_EXTRA_FUNC();
	m_piProjectsNeeded = new int[GC.getNumProjectInfos()];
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		m_piProjectsNeeded[iI] = 0;
	}
	if (!m_aiProjectsNeededforPass3.empty() && !m_aszProjectsNeededforPass3.empty())
	{
		int iNumLoad = m_aiProjectsNeededforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszProjectsNeededforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piProjectsNeeded[iTempIndex] = m_aiProjectsNeededforPass3[iI];
		}
		m_aszProjectsNeededforPass3.clear();
		m_aiProjectsNeededforPass3.clear();
	}

	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FErrorMsg("error");
		return false;
	}
	m_iAnyoneProjectPrereq = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);

	return true;
}


void CvProjectInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iWorldHappiness);
	CheckSum(iSum, m_iGlobalHappiness);
	CheckSum(iSum, m_iWorldHealth);
	CheckSum(iSum, m_iGlobalHealth);
	CheckSum(iSum, m_iWorldTradeRoutes);
	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_bTechShareWithHalfCivs);

	CheckSum(iSum, m_piCommerceModifier, NUM_COMMERCE_TYPES);

	CheckSum(iSum, m_iVictoryPrereq);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iAnyoneProjectPrereq);
	CheckSum(iSum, m_iMaxGlobalInstances);
	CheckSum(iSum, m_iMaxTeamInstances);
	CheckSum(iSum, m_iProductionCost);
	CheckSum(iSum, m_iNukeInterception);
	CheckSum(iSum, m_iTechShare);

	CheckSum(iSum, m_iGlobalMaintenanceModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iConnectedCityMaintenanceModifier);

	CheckSum(iSum, m_iEveryoneSpecialUnit);
	CheckSum(iSum, m_iEveryoneSpecialBuilding);
	CheckSum(iSum, m_iVictoryDelayPercent);
	CheckSum(iSum, m_iSuccessRate);

	CheckSum(iSum, m_bSpaceship);
	CheckSum(iSum, m_bAllowsNukes);

	// Arrays

	CheckSum(iSum, m_piBonusProductionModifier, GC.getNumBonusInfos());
	CheckSum(iSum, m_piVictoryThreshold, GC.getNumVictoryInfos());
	CheckSum(iSum, m_piVictoryMinThreshold, GC.getNumVictoryInfos());
	CheckSum(iSum, m_piProjectsNeeded, GC.getNumProjectInfos());

	// Vectors

	CheckSumC(iSum, m_aiCategories);
	CheckSumC(iSum, m_aeMapCategoryTypes);
}

//======================================================================================================
//					CvReligionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvReligionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvReligionInfo::CvReligionInfo() :
m_iChar(0),
m_iTGAIndex(-1),
m_iHolyCityChar(0),
m_iTechPrereq(NO_TECH),
m_iFreeUnit(NO_UNIT),
m_iNumFreeUnits(0),
m_iSpreadFactor(0),
m_iMissionType(NO_MISSION),
m_paiGlobalReligionCommerce(NULL),
m_paiHolyCityCommerce(NULL),
m_paiStateReligionCommerce(NULL),
m_piFlavorValue(NULL),
m_PropertyManipulators()
{
	reset();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvReligionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvReligionInfo::~CvReligionInfo()
{
	SAFE_DELETE_ARRAY(m_paiGlobalReligionCommerce);
	SAFE_DELETE_ARRAY(m_paiHolyCityCommerce);
	SAFE_DELETE_ARRAY(m_paiStateReligionCommerce);
	SAFE_DELETE_ARRAY(m_piFlavorValue);

	GC.removeDelayedResolution((int*)&m_iFreeUnit);
}

int CvReligionInfo::getChar() const
{
	return m_iChar;
}

int CvReligionInfo::getTGAIndex() const
{
	return m_iTGAIndex;
}

void CvReligionInfo::setChar(int i)
{
	m_iChar = 8550 + m_iTGAIndex * 2;
}

int CvReligionInfo::getHolyCityChar() const
{
	return m_iHolyCityChar;
}

void CvReligionInfo::setHolyCityChar(int i)
{
	m_iHolyCityChar = 8551 + m_iTGAIndex * 2;
}

int CvReligionInfo::getFreeUnit() const
{
	return m_iFreeUnit;
}

int CvReligionInfo::getNumFreeUnits() const
{
	return m_iNumFreeUnits;
}

int CvReligionInfo::getSpreadFactor() const
{
	return m_iSpreadFactor;
}

int CvReligionInfo::getMissionType() const
{
	return m_iMissionType;
}

void CvReligionInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}

const char* CvReligionInfo::getTechButton() const
{
	return m_szTechButton;
}

const char* CvReligionInfo::getGenericTechButton() const
{
	return m_szGenericTechButton;
}

const char* CvReligionInfo::getMovieFile() const
{
	return m_szMovieFile;
}

const char* CvReligionInfo::getMovieSound() const
{
	return m_szMovieSound;
}

const char* CvReligionInfo::getButtonDisabled() const
{
	static char szDisabled[512];

	szDisabled[0] = '\0';

	if ( getButton() && strlen(getButton()) > 4 )
	{
		strncpy( szDisabled, getButton(), strlen(getButton()) - 4 );
		szDisabled[strlen(getButton()) - 4] = '\0';
		strcat( szDisabled, "_D.dds" );
	}

	return szDisabled;
}

const char* CvReligionInfo::getSound() const
{
	return m_szSound;
}

void CvReligionInfo::setAdjectiveKey(const char* szVal)
{
	m_szAdjectiveKey = szVal;
}

const wchar_t* CvReligionInfo::getAdjectiveKey() const
{
	return m_szAdjectiveKey;
}

// Arrays

int CvReligionInfo::getGlobalReligionCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiGlobalReligionCommerce ? m_paiGlobalReligionCommerce[i] : 0;
}

int* CvReligionInfo::getGlobalReligionCommerceArray() const
{
	return m_paiGlobalReligionCommerce;
}

int CvReligionInfo::getHolyCityCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiHolyCityCommerce ? m_paiHolyCityCommerce[i] : 0;
}

int* CvReligionInfo::getHolyCityCommerceArray() const
{
	return m_paiHolyCityCommerce;
}

int CvReligionInfo::getStateReligionCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiStateReligionCommerce ? m_paiStateReligionCommerce[i] : 0;
}

int* CvReligionInfo::getStateReligionCommerceArray() const
{
	return m_paiStateReligionCommerce;
}

int CvReligionInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvReligionInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvReligionInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvReligionInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

//
// read from xml
//
bool CvReligionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnit");
	GC.addDelayedResolution((int*)&m_iFreeUnit, szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iNumFreeUnits, L"iFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_iSpreadFactor, L"iSpreadFactor");
	pXML->GetOptionalChildXmlValByName(&m_iTGAIndex, L"iTGAIndex");

	if (pXML->TryMoveToXmlFirstChild(L"GlobalReligionCommerces"))
	{
		pXML->SetCommerce(&m_paiGlobalReligionCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiGlobalReligionCommerce);

	if (pXML->TryMoveToXmlFirstChild(L"HolyCityCommerces"))
	{
		pXML->SetCommerce(&m_paiHolyCityCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiHolyCityCommerce);

	if (pXML->TryMoveToXmlFirstChild(L"StateReligionCommerces"))
	{
		pXML->SetCommerce(&m_paiStateReligionCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiStateReligionCommerce);

	pXML->GetChildXmlValByName(m_szTechButton, L"TechButton");
	pXML->GetOptionalChildXmlValByName(m_szGenericTechButton, L"GenericTechButton");
	pXML->GetOptionalChildXmlValByName(m_szMovieFile, L"MovieFile");
	pXML->GetOptionalChildXmlValByName(m_szMovieSound, L"MovieSound");
	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Adjective");
	setAdjectiveKey(szTextVal);

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvReligionInfo::copyNonDefaults(const CvReligionInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	GC.copyNonDefaultDelayedResolution((int*)&m_iFreeUnit, (int*)&pClassInfo->m_iFreeUnit);

	if (getNumFreeUnits() == iDefault) m_iNumFreeUnits = pClassInfo->getNumFreeUnits();
	if (getSpreadFactor() == iDefault) m_iSpreadFactor = pClassInfo->getSpreadFactor();

	if (getTGAIndex() == iDefault) m_iTGAIndex = pClassInfo->getTGAIndex();

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++)
	{
		if ( getGlobalReligionCommerce(i) == iDefault && pClassInfo->getGlobalReligionCommerce(i) != iDefault)
		{
			if ( NULL == m_paiGlobalReligionCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiGlobalReligionCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiGlobalReligionCommerce[i] = pClassInfo->getGlobalReligionCommerce(i);
		}

		if ( getHolyCityCommerce(i) == iDefault && pClassInfo->getHolyCityCommerce(i) != iDefault)
		{
			if ( NULL == m_paiHolyCityCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiHolyCityCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiHolyCityCommerce[i] = pClassInfo->getHolyCityCommerce(i);
		}

		if ( getStateReligionCommerce(i) == iDefault && pClassInfo->getStateReligionCommerce(i) != iDefault)
		{
			if ( NULL == m_paiStateReligionCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiStateReligionCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiStateReligionCommerce[i] = pClassInfo->getStateReligionCommerce(i);
		}
	}
	if (getTechButton() == cDefault) m_szTechButton = pClassInfo->getTechButton();
	if (getGenericTechButton() == cDefault) m_szGenericTechButton = pClassInfo->getGenericTechButton();
	if (getMovieFile() == cDefault) m_szMovieFile = pClassInfo->getMovieFile();
	if (getMovieSound() == cDefault) m_szMovieSound = pClassInfo->getMovieSound();
	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();
	if (getAdjectiveKey() == wDefault) setAdjectiveKey(CvString::format("%s",pClassInfo->getAdjectiveKey()).GetCString());

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++ )
	{
		if ( getFlavorValue(i) == iDefault && pClassInfo->getFlavorValue(i) != iDefault)
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[i] = pClassInfo->getFlavorValue(i);
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

void CvReligionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iFreeUnit);
	CheckSum(iSum, m_iNumFreeUnits);
	CheckSum(iSum, m_iSpreadFactor);
	CheckSum(iSum, m_iMissionType);

	// Arrays

	CheckSum(iSum, m_paiGlobalReligionCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiHolyCityCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiStateReligionCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());

	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}

//======================================================================================================
//					CvCorporationInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCorporationInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCorporationInfo::CvCorporationInfo() :
 m_iChar(0)
,m_iTGAIndex(-1)
,m_iHeadquarterChar(0)
,m_iTechPrereq(NO_TECH)
,m_iFreeUnit(NO_UNIT)
,m_iSpreadFactor(0)
,m_iSpreadCost(0)
,m_iMaintenance(0)
,m_iMissionType(NO_MISSION)
,m_iBonusProduced(NO_BONUS)
,m_paiHeadquarterCommerce(NULL)
,m_paiCommerceProduced(NULL)
,m_paiYieldProduced(NULL)
,m_iObsoleteTech(NO_TECH)
,m_iPrereqGameOption(NO_GAMEOPTION)
,m_iHealth(0)
,m_iHappiness(0)
,m_iMilitaryProductionModifier(0)
,m_iFreeXP(0)
,m_iSpread(0)
,m_paiPrereqBuilding(NULL)
,m_pabCompetingCorporation(NULL)
,m_piYieldChange(NULL)
,m_piCommerceChange(NULL)
,m_PropertyManipulators()
{
	reset();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvReligionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCorporationInfo::~CvCorporationInfo()
{
	SAFE_DELETE_ARRAY(m_paiHeadquarterCommerce);
	SAFE_DELETE_ARRAY(m_paiCommerceProduced);
	SAFE_DELETE_ARRAY(m_paiYieldProduced);
	SAFE_DELETE_ARRAY(m_paiPrereqBuilding);
	SAFE_DELETE_ARRAY(m_pabCompetingCorporation);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piCommerceChange);

}

int CvCorporationInfo::getChar() const
{
	return m_iChar;
}


int CvCorporationInfo::getTGAIndex() const
{
	return m_iTGAIndex;
}

void CvCorporationInfo::setChar(int i)
{
	m_iChar = 8550 + (GC.getGAMEFONT_TGA_RELIGIONS() + m_iTGAIndex) * 2;
}

int CvCorporationInfo::getHeadquarterChar() const
{
	return m_iHeadquarterChar;
}

void CvCorporationInfo::setHeadquarterChar(int i)
{
	m_iHeadquarterChar = 8551 + (GC.getGAMEFONT_TGA_RELIGIONS() + m_iTGAIndex) * 2;
}

int CvCorporationInfo::getFreeUnit() const
{
	return m_iFreeUnit;
}

int CvCorporationInfo::getSpreadFactor() const
{
	return m_iSpreadFactor;
}

int CvCorporationInfo::getSpreadCost() const
{
	return m_iSpreadCost;
}

int CvCorporationInfo::getMaintenance() const
{
	return m_iMaintenance;
}

int CvCorporationInfo::getMissionType() const
{
	return m_iMissionType;
}

void CvCorporationInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}

int CvCorporationInfo::getBonusProduced() const
{
	return m_iBonusProduced;
}

const char* CvCorporationInfo::getMovieFile() const
{
	return m_szMovieFile;
}

const char* CvCorporationInfo::getMovieSound() const
{
	return m_szMovieSound;
}

const char* CvCorporationInfo::getSound() const
{
	return m_szSound;
}

TechTypes CvCorporationInfo::getObsoleteTech() const
{
	return m_iObsoleteTech;
}

int CvCorporationInfo::getSpread() const
{
	return m_iSpread;
}

int CvCorporationInfo::getHealth() const
{
	return m_iHealth;
}

int CvCorporationInfo::getHappiness() const
{
	return m_iHappiness;
}

int CvCorporationInfo::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}

int CvCorporationInfo::getFreeXP() const
{
	return m_iFreeXP;
}

int CvCorporationInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}

int CvCorporationInfo::getPrereqBuilding(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	return m_paiPrereqBuilding ? m_paiPrereqBuilding[i] : false;
}

int CvCorporationInfo::getPrereqBuildingVectorSize() const					{return m_aszPrereqBuildingforPass3.size();}
CvString CvCorporationInfo::getPrereqBuildingNamesVectorElement(const int i) const	{return m_aszPrereqBuildingforPass3[i];}
int CvCorporationInfo::getPrereqBuildingValuesVectorElement(const int i) const		{return m_aiPrereqBuildingforPass3[i];}

bool CvCorporationInfo::isCompetingCorporation(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), i);
	return m_pabCompetingCorporation ? m_pabCompetingCorporation[i] : false;
}

int CvCorporationInfo::getCompetingCorporationVectorSize() const					{return m_aszCompetingCorporationforPass3.size();}
CvString CvCorporationInfo::getCompetingCorporationNamesVectorElement(const int i) const	{return m_aszCompetingCorporationforPass3[i];}
bool CvCorporationInfo::getCompetingCorporationValuesVectorElement(const int i) const		{return m_abCompetingCorporationforPass3[i];}

int CvCorporationInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvCorporationInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}

int CvCorporationInfo::getCommerceChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceChange ? m_piCommerceChange[i] : 0;
}

int* CvCorporationInfo::getCommerceChangeArray() const
{
	return m_piCommerceChange;
}

const std::vector<BonusTypes>& CvCorporationInfo::getPrereqBonuses() const
{
	return m_vPrereqBonuses;
}

int CvCorporationInfo::getHeadquarterCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiHeadquarterCommerce ? m_paiHeadquarterCommerce[i] : 0;
}

int* CvCorporationInfo::getHeadquarterCommerceArray() const
{
	return m_paiHeadquarterCommerce;
}

int CvCorporationInfo::getCommerceProduced(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiCommerceProduced ? m_paiCommerceProduced[i] : 0;
}

int* CvCorporationInfo::getCommerceProducedArray() const
{
	return m_paiCommerceProduced;
}

int CvCorporationInfo::getYieldProduced(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_paiYieldProduced ? m_paiYieldProduced[i] : 0;
}

int* CvCorporationInfo::getYieldProducedArray() const
{
	return m_paiYieldProduced;
}

int CvCorporationInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvCorporationInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvCorporationInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

//
// read from xml
//
bool CvCorporationInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnit");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iSpreadFactor, L"iSpreadFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSpreadCost, L"iSpreadCost");
	pXML->GetOptionalChildXmlValByName(&m_iMaintenance, L"iMaintenance");
	pXML->GetOptionalChildXmlValByName(&m_iTGAIndex, L"iTGAIndex");

	if (pXML->TryMoveToXmlFirstChild(L"HeadquarterCommerces"))
	{
		pXML->SetCommerce(&m_paiHeadquarterCommerce);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiHeadquarterCommerce);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommercesProduced"))
	{
		pXML->SetCommerce(&m_paiCommerceProduced);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceProduced);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldsProduced"))
	{
		pXML->SetYields(&m_paiYieldProduced);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiYieldProduced);
	}

	pXML->SetOptionalVector(&m_vPrereqBonuses, L"PrereqBonuses");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusProduced");
	m_iBonusProduced = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(m_szMovieFile, L"MovieFile");
	pXML->GetOptionalChildXmlValByName(m_szMovieSound, L"MovieSound");
	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = static_cast<TechTypes>(pXML->GetInfoClass(szTextVal));

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iSpread, L"iSpread");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryProductionModifier, L"iMilitaryProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeXP, L"iFreeXP");

	if (pXML->TryMoveToXmlFirstChild(L"PrereqBuildings"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszPrereqBuildingforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiPrereqBuildingforPass3.push_back(iTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"CompetingCorporations"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		bool bTemp = false;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszCompetingCorporationforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&bTemp);
						m_abCompetingCorporationforPass3.push_back(bTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
	{
		pXML->SetCommerce(&m_piCommerceChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceChange);
	}
	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		pXML->CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
	}


	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvCorporationInfo::copyNonDefaults(const CvCorporationInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (m_iFreeUnit == iTextDefault) m_iFreeUnit = pClassInfo->getFreeUnit();
	if (getSpreadFactor() == iDefault) m_iSpreadFactor = pClassInfo->getSpreadFactor();
	if (getSpreadCost() == iDefault) m_iSpreadCost = pClassInfo->getSpreadCost();
	if (getMaintenance() == iDefault) m_iMaintenance = pClassInfo->getMaintenance();

	if (getTGAIndex() == iDefault) m_iTGAIndex = pClassInfo->getTGAIndex();

	if (!m_paiHeadquarterCommerce) CvXMLLoadUtility::InitList(&m_paiHeadquarterCommerce, NUM_COMMERCE_TYPES);
	if (!m_paiCommerceProduced) CvXMLLoadUtility::InitList(&m_paiCommerceProduced, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_paiHeadquarterCommerce[i] == iDefault )
		{
			m_paiHeadquarterCommerce[i] = pClassInfo->getHeadquarterCommerce(i);
		}

		if ( m_paiCommerceProduced[i] == iDefault )
		{
			m_paiCommerceProduced[i] = pClassInfo->getCommerceProduced(i);
		}
	}

	if (!m_paiYieldProduced) CvXMLLoadUtility::InitList(&m_paiYieldProduced, NUM_YIELD_TYPES);
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( m_paiYieldProduced[i] == iDefault )
		{
			m_paiYieldProduced[i] = pClassInfo->getYieldProduced(i);
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_vPrereqBonuses, pClassInfo->m_vPrereqBonuses);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	if (getBonusProduced() == iTextDefault) m_iBonusProduced = pClassInfo->getBonusProduced();

	if (getMovieFile() == cDefault) m_szMovieFile = pClassInfo->getMovieFile();
	if (getMovieSound() == cDefault) m_szMovieSound = pClassInfo->getMovieSound();
	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();

	for ( int i = 0; i < pClassInfo->getPrereqBuildingVectorSize(); i++ )
	{
		m_aiPrereqBuildingforPass3.push_back(pClassInfo->getPrereqBuildingValuesVectorElement(i));
		m_aszPrereqBuildingforPass3.push_back(pClassInfo->getPrereqBuildingNamesVectorElement(i));
	}
	if (getObsoleteTech() == iTextDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getSpread() == iDefault) m_iSpread = pClassInfo->getSpread();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getMilitaryProductionModifier() == iDefault) m_iMilitaryProductionModifier = pClassInfo->getMilitaryProductionModifier();
	if (getFreeXP() == iDefault) m_iFreeXP = pClassInfo->getFreeXP();
	if (getPrereqGameOption() == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();

	for ( int i = 0; i < pClassInfo->getCompetingCorporationVectorSize(); i++ )
	{
		m_abCompetingCorporationforPass3.push_back(pClassInfo->getCompetingCorporationValuesVectorElement(i));
		m_aszCompetingCorporationforPass3.push_back(pClassInfo->getCompetingCorporationNamesVectorElement(i));
	}

	if (!m_piYieldChange) CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
	for ( int j = 0; j < NUM_YIELD_TYPES; j++)
	{
		if ( getYieldChange(j) == iDefault && pClassInfo->getYieldChange(j) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[j] = pClassInfo->getYieldChange(j);
		}
	}
	if (!m_piCommerceChange) CvXMLLoadUtility::InitList(&m_piCommerceChange, NUM_COMMERCE_TYPES);
	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( m_piCommerceChange[j] == iDefault )
		{
			m_piCommerceChange[j] = pClassInfo->getCommerceChange(j);
		}
	}
	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

void CvCorporationInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iFreeUnit);
	CheckSum(iSum, m_iSpreadFactor);
	CheckSum(iSum, m_iSpreadCost);
	CheckSum(iSum, m_iMaintenance);
	CheckSum(iSum, m_iMissionType);
	CheckSum(iSum, m_iBonusProduced);

	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iSpread);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iFreeXP);
	CheckSum(iSum, m_iMilitaryProductionModifier);
	CheckSum(iSum, m_iPrereqGameOption);

	CheckSum(iSum, m_paiPrereqBuilding, GC.getNumBuildingInfos());

	CheckSum(iSum, m_pabCompetingCorporation, GC.getNumCorporationInfos());

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piCommerceChange, NUM_COMMERCE_TYPES);

	// Arrays

	CheckSumC(iSum, m_vPrereqBonuses);
	CheckSum(iSum, m_paiHeadquarterCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiCommerceProduced, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiYieldProduced, NUM_YIELD_TYPES);

	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}


bool CvCorporationInfo::readPass3()
{
	PROFILE_EXTRA_FUNC();
	m_paiPrereqBuilding = new int[GC.getNumBuildingInfos()];
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_paiPrereqBuilding[iI] = 0;
	}
	if (!m_aiPrereqBuildingforPass3.empty() && !m_aszPrereqBuildingforPass3.empty())
	{
		const int iNumLoad = m_aiPrereqBuildingforPass3.size();
		for (int iI = 0; iI < iNumLoad; iI++)
		{
			const int iTempIndex = GC.getInfoTypeForString(m_aszPrereqBuildingforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumBuildingInfos())
				m_paiPrereqBuilding[iTempIndex] = m_aiPrereqBuildingforPass3[iI];
		}
		m_aszPrereqBuildingforPass3.clear();
		m_aiPrereqBuildingforPass3.clear();
	}

	m_pabCompetingCorporation = new bool[GC.getNumCorporationInfos()];
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		m_pabCompetingCorporation[iI] = 0;
	}
	if (!m_abCompetingCorporationforPass3.empty() && !m_aszCompetingCorporationforPass3.empty())
	{
		const int iNumLoad = m_abCompetingCorporationforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			const int iTempIndex = GC.getInfoTypeForString(m_aszCompetingCorporationforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCorporationInfos())
				m_pabCompetingCorporation[iTempIndex] = m_abCompetingCorporationforPass3[iI];
		}
		m_aszCompetingCorporationforPass3.clear();
		m_abCompetingCorporationforPass3.clear();
	}

	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FErrorMsg("error");
		return false;
	}

	m_iFreeUnit = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);

	m_aszExtraXMLforPass3.clear();

	return true;
}


//======================================================================================================
//					CvCursorInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCursorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCursorInfo::CvCursorInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCursorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCursorInfo::~CvCursorInfo()
{
}

const char* CvCursorInfo::getPath()
{
	return m_szPath;
}

bool CvCursorInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"CursorPath");
	m_szPath = szTextVal;

	return true;
}

void CvCursorInfo::copyNonDefaults(const CvCursorInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) m_szPath = pClassInfo->m_szPath;
}

//======================================================================================================
//					CvThroneRoomCamera
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomCamera()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomCamera::CvThroneRoomCamera()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomCamera()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomCamera::~CvThroneRoomCamera()
{
}

const char* CvThroneRoomCamera::getFileName()
{
	return m_szFileName;
}

bool CvThroneRoomCamera::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szFileName, L"FileName");

	return true;
}

void CvThroneRoomCamera::copyNonDefaults(const CvThroneRoomCamera* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getFileName() == cDefault) m_szFileName = pClassInfo->m_szFileName;
}

//======================================================================================================
//					CvThroneRoomInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomInfo::CvThroneRoomInfo() :
m_iFromState(0),
m_iToState(0),
m_iAnimation(0)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomInfo::~CvThroneRoomInfo()
{
}

const char* CvThroneRoomInfo::getEvent()
{
	return m_szEvent;
}

const char* CvThroneRoomInfo::getNodeName()
{
	return m_szNodeName;
}

int CvThroneRoomInfo::getFromState()
{
	return m_iFromState;
}

int CvThroneRoomInfo::getToState()
{
	return m_iToState;
}

int CvThroneRoomInfo::getAnimation()
{
	return m_iAnimation;
}

bool CvThroneRoomInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szEvent, L"Event");
	pXML->GetOptionalChildXmlValByName(&m_iFromState, L"iFromState" );
	pXML->GetOptionalChildXmlValByName(&m_iToState, L"iToState" );
	pXML->GetOptionalChildXmlValByName(m_szNodeName, L"NodeName");
	pXML->GetOptionalChildXmlValByName(&m_iAnimation, L"iAnimation" );

	return true;
}

void CvThroneRoomInfo::copyNonDefaults(CvThroneRoomInfo* pClassInfo)
{
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getEvent() == cDefault) m_szEvent = pClassInfo->getEvent();
	if (getFromState() == iDefault) m_iFromState = pClassInfo->getFromState();
	if (getToState() == iDefault) m_iToState = pClassInfo->getToState();
	if (getNodeName() == cDefault) m_szNodeName = pClassInfo->getNodeName();
	if (getAnimation() == iDefault) m_iAnimation = pClassInfo->getAnimation();
}

//======================================================================================================
//					CvThroneRoomStyleInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomStyleInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomStyleInfo::CvThroneRoomStyleInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomStyleInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomStyleInfo::~CvThroneRoomStyleInfo()
{
}

bool CvThroneRoomStyleInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(m_szArtStyleType, L"ArtStyleType");
	pXML->GetOptionalChildXmlValByName(m_szEraType, L"EraType");
	pXML->GetOptionalChildXmlValByName(m_szFileName, L"FileName");

	//node names
	if(pXML->TryMoveToXmlFirstChild())
	{
		while(pXML->TryMoveToXmlNextSibling(L"NodeName"))
		{
			pXML->GetXmlVal(szTextVal);
			m_aNodeNames.push_back(szTextVal);
		}
		pXML->MoveToXmlParent();
	}

	//texture names
	if(pXML->TryMoveToXmlFirstChild())
	{
		while(pXML->TryMoveToXmlNextSibling(L"TextureName"))
		{
			pXML->GetXmlVal(szTextVal);
			m_aTextureNames.push_back(szTextVal);
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvThroneRoomStyleInfo::copyNonDefaults(const CvThroneRoomStyleInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getArtStyleType() == cDefault) m_szArtStyleType = pClassInfo->m_szArtStyleType;
	if (getEraType() == cDefault) m_szEraType = pClassInfo->m_szEraType;
	if (getFileName() == cDefault) m_szFileName = pClassInfo->m_szFileName;

	/*
	m_aNodeNames and m_aTextureNames don't seem to be used?
	since i hardly doubt anyone ever touches those XML's anyway, i just leave them out
	FOR NOW! */
}

//======================================================================================================
//					CvSlideShowInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSlideShowInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowInfo::CvSlideShowInfo() :
m_fStartTime(0.0f)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSlideShowInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowInfo::~CvSlideShowInfo()
{
}

bool CvSlideShowInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szPath, L"Path");
	pXML->GetChildXmlValByName(m_szTransitionType, L"TransitionType");
	pXML->GetChildXmlValByName(&m_fStartTime, L"fStartTime");

	return true;
}
void CvSlideShowInfo::copyNonDefaults(const CvSlideShowInfo* pClassInfo)
{
	const float fDefault = 0.0f;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) m_szPath = pClassInfo->m_szPath;
	if (getTransitionType() == cDefault) m_szTransitionType = pClassInfo->m_szTransitionType;
	if (getStartTime() == fDefault) m_fStartTime = pClassInfo->m_fStartTime;
}

//======================================================================================================
//					CvSlideShowRandomInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSlideShowRandomInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowRandomInfo::CvSlideShowRandomInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSlideShowRandomInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowRandomInfo::~CvSlideShowRandomInfo()
{
}

const char* CvSlideShowRandomInfo::getPath()
{
	return m_szPath;
}

bool CvSlideShowRandomInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szPath, L"Path");

	return true;
}

void CvSlideShowRandomInfo::copyNonDefaults(const CvSlideShowRandomInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) m_szPath = pClassInfo->m_szPath;
}

//======================================================================================================
//					CvWorldPickerInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWorldPickerInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWorldPickerInfo::CvWorldPickerInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWorldPickerInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWorldPickerInfo::~CvWorldPickerInfo()
{
}

const char* CvWorldPickerInfo::getMapName()
{
	return m_szMapName;
}

const char* CvWorldPickerInfo::getModelFile()
{
	return m_szModelFile;
}

int CvWorldPickerInfo::getNumSizes()
{
	return m_aSizes.size();
}

float CvWorldPickerInfo::getSize(int index)
{
	return m_aSizes[index];
}

int CvWorldPickerInfo::getNumClimates()
{
	return m_aClimates.size();
}

const char* CvWorldPickerInfo::getClimatePath(int index)
{
	return m_aClimates[index];
}

int CvWorldPickerInfo::getNumWaterLevelDecals()
{
	return m_aWaterLevelDecals.size();
}

const char* CvWorldPickerInfo::getWaterLevelDecalPath(int index)
{
	return m_aWaterLevelDecals[index];
}

int CvWorldPickerInfo::getNumWaterLevelGloss()
{
	return m_aWaterLevelGloss.size();
}

const char* CvWorldPickerInfo::getWaterLevelGlossPath(int index)
{
	return m_aWaterLevelGloss[index];
}

bool CvWorldPickerInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	float fVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szMapName, L"MapName");
	pXML->GetChildXmlValByName(m_szModelFile, L"ModelFile");

	//sizes
	if(pXML->TryMoveToXmlFirstChild(L"Sizes"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"Size"))
		{
			do
			{
				pXML->GetXmlVal(&fVal);
				m_aSizes.push_back(fVal);
			} while(pXML->TryMoveToXmlNextSibling(L"Size"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//climates
	if(pXML->TryMoveToXmlFirstChild(L"Climates"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"ClimatePath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aClimates.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"ClimatePath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//water level decals
	if(pXML->TryMoveToXmlFirstChild(L"WaterLevelDecals"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"WaterLevelDecalPath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aWaterLevelDecals.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"WaterLevelDecalPath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//water level gloss
	if(pXML->TryMoveToXmlFirstChild(L"WaterLevelGloss"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"WaterLevelGlossPath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aWaterLevelGloss.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"WaterLevelGlossPath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}
void CvWorldPickerInfo::copyNonDefaults(CvWorldPickerInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getMapName() == cDefault) m_szMapName = pClassInfo->m_szMapName;
	if (getModelFile() == cDefault) m_szModelFile = pClassInfo->getModelFile();

	if ( getNumSizes() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumSizes(); i++ )
		{
			m_aSizes.push_back(pClassInfo->getSize(i));
		}
	}

	if ( getNumClimates() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumClimates(); i++ )
		{
			m_aClimates.push_back(pClassInfo->getClimatePath(i));
		}
	}

	if ( getNumWaterLevelDecals() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumWaterLevelDecals(); i++ )
		{
			m_aWaterLevelDecals.push_back(pClassInfo->getWaterLevelDecalPath(i));
		}
	}

	if ( getNumWaterLevelGloss() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumWaterLevelGloss(); i++ )
		{
			m_aWaterLevelDecals.push_back(pClassInfo->getWaterLevelGlossPath(i));
		}
	}
}

//======================================================================================================
//					CvSpaceShipInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpaceShipInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpaceShipInfo::CvSpaceShipInfo() :
m_eSpaceShipInfoType(SPACE_SHIP_INFO_TYPE_NONE),
m_iPartNumber(-1),
m_iArtType(-1),
m_iEventCode(-1),
m_eProjectType(NO_PROJECT),
m_eCameraUpAxis(AXIS_X)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpaceShipInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpaceShipInfo::~CvSpaceShipInfo()
{
}

const char* CvSpaceShipInfo::getNodeName()
{
	return m_szNodeName;
}

const char* CvSpaceShipInfo::getProjectName()
{
	return m_szProjectName;
}

void CvSpaceShipInfo::setProjectName(const char* szVal)
{
	m_szProjectName = szVal;
	m_eProjectType = (ProjectTypes) GC.getInfoTypeForString(m_szProjectName, true);
}

ProjectTypes CvSpaceShipInfo::getProjectType()
{
	return m_eProjectType;
}

AxisTypes CvSpaceShipInfo::getCameraUpAxis()
{
	return m_eCameraUpAxis;
}

SpaceShipInfoTypes CvSpaceShipInfo::getSpaceShipInfoType()
{
	return m_eSpaceShipInfoType;
}

int CvSpaceShipInfo::getPartNumber()
{
	return m_iPartNumber;
}

int CvSpaceShipInfo::getArtType()
{
	return m_iArtType;
}

int CvSpaceShipInfo::getEventCode()
{
	return m_iEventCode;
}

bool CvSpaceShipInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;
	pXML->GetChildXmlValByName(m_szNodeName, L"NodeName");
	pXML->GetChildXmlValByName(szTextVal, L"ProjectName");
	setProjectName(szTextVal);

	//up axis
	pXML->GetChildXmlValByName(szTextVal, L"CameraUpAxis");
	if(szTextVal.CompareNoCase("AXIS_X") == 0)
		m_eCameraUpAxis = AXIS_X;
	else if(szTextVal.CompareNoCase("AXIS_Y") == 0)
		m_eCameraUpAxis = AXIS_Y;
	else if(szTextVal.CompareNoCase("AXIS_Z") == 0)
		m_eCameraUpAxis = AXIS_Z;
	else
	{
		FErrorMsg("[Jason] Unknown Axis Type.");
	}

	pXML->GetOptionalChildXmlValByName(&m_iPartNumber, L"PartNumber");
	pXML->GetOptionalChildXmlValByName(&m_iArtType, L"ArtType");
	pXML->GetOptionalChildXmlValByName(&m_iEventCode, L"EventCode");

	//type
	pXML->GetChildXmlValByName(szTextVal, L"InfoType");
	if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_FILENAME") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_FILENAME;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ALPHA_CENTAURI") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ALPHA_CENTAURI;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LAUNCH") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LAUNCH;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LAUNCHED") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LAUNCHED;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ZOOM_IN") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ZOOM_IN;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ZOOM_MOVE") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ZOOM_MOVE;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_OFF") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_OFF;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_APPEAR") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_APPEAR;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_PREVIEW") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_PREVIEW;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LIGHT_OFF") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LIGHT_OFF;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_GANTRY_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_GANTRY_SMOKE_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_IN_SPACE_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_IN_SPACE_SMOKE_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_IN_GAME_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_IN_GAME_SMOKE_ON;
	else
	{
		FErrorMsg("[Jason] Unknown SpaceShipInfoType.");
	}

	return true;
}

void CvSpaceShipInfo::copyNonDefaults(CvSpaceShipInfo* pClassInfo)
{
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getNodeName() == cDefault) m_szNodeName = pClassInfo->m_szNodeName;
	if (getProjectName() == cDefault) setProjectName(pClassInfo->getProjectName());

	if (getPartNumber() == iDefault) m_iPartNumber = pClassInfo->m_iPartNumber;
	if (getArtType() == iDefault) m_iArtType = pClassInfo->m_iArtType;
	if (getEventCode() == iDefault) m_iEventCode = pClassInfo->m_iEventCode;

//	if (getCameraUpAxis() == cDefault) m_eCameraUpAxis = pClassInfo->getCameraUpAxis();
//	if (getSpaceShipInfoType() == cDefault) m_eSpaceShipInfoType = pClassInfo->getSpaceShipInfoType();
}

//======================================================================================================
//					CvAnimationPathInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAnimationPathInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationPathInfo::CvAnimationPathInfo() :
	m_bMissionPath(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAnimationPathInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationPathInfo::~CvAnimationPathInfo()
{
}

int CvAnimationPathInfo::getPathCategory( int i )
{
	return (int)m_vctPathDefinition.size() > i ? m_vctPathDefinition[i].first : -1;
}

float CvAnimationPathInfo::getPathParameter( int i )
{
	return (int)m_vctPathDefinition.size() > i ? m_vctPathDefinition[i].second : -1;
}

int CvAnimationPathInfo::getNumPathDefinitions()
{
	return m_vctPathDefinition.size();
}

CvAnimationPathDefinition * CvAnimationPathInfo::getPath( )
{
	return &m_vctPathDefinition;
}

bool CvAnimationPathInfo::isMissionPath() const
{
	return m_bMissionPath;
}

//------------------------------------------------------------------------------------------------
// FUNCTION:	CvAnimationPathInfo::read
//! \brief	  Reads in a CvAnimationPathInfo definition from XML
//! \param	  pXML Pointer to the XML loading object
//! \retval	 true if the definition was read successfully, false otherwise
//------------------------------------------------------------------------------------------------
bool CvAnimationPathInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bMissionPath, L"bMissionPath");

	CvString animPathName;
	if (!pXML->GetChildXmlValByName(animPathName, L"Type"))
	{
		FErrorMsg("No animation path name found");
	}
	if (pXML->TryMoveToXmlFirstChild(L"PathEntry"))
	{
		do
		{
			int		iCurrentCategory;	// The current category information we are building
			float	fParameter;			// Temporary
			std::string	szTempString;

			if (pXML->GetOptionalChildXmlValByName(szTempString, L"Category"))
			{
				iCurrentCategory = pXML->GetInfoClass(szTempString.c_str());
				fParameter = 0.0f;
			}
			else
			{
				pXML->GetChildXmlValByName(szTempString, L"Operator");
				iCurrentCategory = GC.getInfoTypeForString(szTempString.c_str());
				iCurrentCategory += (int)ANIMOP_FIRST;
				if (!pXML->GetChildXmlValByName(&fParameter, L"Parameter"))
				{
					fParameter = 0.0f;
				}
			}
			m_vctPathDefinition.push_back(std::make_pair(iCurrentCategory, fParameter));
		} while (pXML->TryMoveToXmlNextSibling());
	}
	else
	{
		FErrorMsg(CvString::format("Animation path %s XML structure is invalid", animPathName).c_str());
		return false;
	}
	pXML->MoveToXmlParent();

	return true;
}

void CvAnimationPathInfo::copyNonDefaults(CvAnimationPathInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isMissionPath() == bDefault) m_bMissionPath = pClassInfo->isMissionPath();

	int		iCurrentCategory;
	float	fParameter;

	if ( m_vctPathDefinition.empty())
	{
		for ( int i = 0; i < (int)pClassInfo->m_vctPathDefinition.size(); i++ )
		{
			iCurrentCategory = pClassInfo->getPathCategory(i);
			fParameter = pClassInfo->getPathParameter(i);
			m_vctPathDefinition.push_back( std::make_pair(iCurrentCategory, fParameter ));
		}
	}
}

//======================================================================================================
//					CvAnimationCategoryInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAnimationCategoryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationCategoryInfo::CvAnimationCategoryInfo()
{
	m_kCategory.second = -7540; // invalid.
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAnimationCategoryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationCategoryInfo::~CvAnimationCategoryInfo()
{
}

int CvAnimationCategoryInfo::getCategoryBaseID( )
{
	return m_kCategory.first;
}

int CvAnimationCategoryInfo::getCategoryDefaultTo( )
{
	if ( m_kCategory.second < -1 )
	{
		// CvXMLLoadUtility *pXML = new CvXMLLoadUtility();
		m_kCategory.second = CvXMLLoadUtility::GetInfoClass( m_szDefaultTo);
	}
	return (int)m_kCategory.second;
}

bool CvAnimationCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int		iBaseID;						// Temporary
	pXML->GetChildXmlValByName( m_szDefaultTo, L"DefaultTo");
	pXML->GetChildXmlValByName( &iBaseID, L"BaseID");
	m_kCategory.first = iBaseID;
	return true;
}

void CvAnimationCategoryInfo::copyNonDefaults(const CvAnimationCategoryInfo* pClassInfo)
{
	const int iDefault = 0;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getCategoryBaseID() == iDefault) m_kCategory.first = pClassInfo->m_kCategory.first;
	if (getCategoryDefaultTo() == iTextDefault) m_kCategory.second = pClassInfo->m_kCategory.second;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvEntityEventInfo
/////////////////////////////////////////////////////////////////////////////////////////////

CvEntityEventInfo::CvEntityEventInfo() :
m_bUpdateFormation(true)
{
}

CvEntityEventInfo::~CvEntityEventInfo()
{
}

bool CvEntityEventInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTmp, szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int iNumSibs, i;

	if (pXML->TryMoveToXmlFirstChild(L"AnimationPathTypes"))
	{
		// get the total number of children the current xml node has
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (iNumSibs > 0)
		{
			// if the call to the function that sets the current xml node to it's first non-comment
			// child and sets the parameter with the new node's value succeeds
			if (pXML->GetChildXmlVal(szTmp))
			{
				AnimationPathTypes eAnimationPath = (AnimationPathTypes)CvXMLLoadUtility::GetInfoClass( szTmp);
				if ( eAnimationPath > ANIMATIONPATH_NONE )
					m_vctAnimationPathType.push_back( eAnimationPath );

				// loop through all the siblings, we start at 1 since we already have the first value
				for (i=1;i<iNumSibs;i++)
				{
					if (!pXML->GetNextXmlVal(szTmp))
					{
						break;
					}
					AnimationPathTypes eAnimationPath = (AnimationPathTypes)CvXMLLoadUtility::GetInfoClass( szTmp);
					if ( eAnimationPath > ANIMATIONPATH_NONE )
						m_vctAnimationPathType.push_back( eAnimationPath );

				}
				pXML->MoveToXmlParent();
			}
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"EffectTypes"))
	{
		// get the total number of children the current xml node has
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (iNumSibs > 0)
		{
			// if the call to the function that sets the current xml node to it's first non-comment
			// child and sets the parameter with the new node's value succeeds
			if (pXML->GetChildXmlVal(szTmp))
			{
				EffectTypes eEffectType = (EffectTypes)CvXMLLoadUtility::GetInfoClass( szTmp);
				if ( eEffectType > NO_EFFECT )
					m_vctEffectTypes.push_back( eEffectType );

				// loop through all the siblings, we start at 1 since we already have the first value
				for (i=1;i<iNumSibs;i++)
				{
					if (!pXML->GetNextXmlVal(szTmp))
					{
						break;
					}
					EffectTypes eEffectType = (EffectTypes)CvXMLLoadUtility::GetInfoClass( szTmp);
					if ( eEffectType > NO_EFFECT )
						m_vctEffectTypes.push_back( eEffectType );
				}
				pXML->MoveToXmlParent();
			}
		}
		pXML->MoveToXmlParent();
	}

	pXML->GetChildXmlValByName( &m_bUpdateFormation, L"bUpdateFormation" );

	return true;
}

void CvEntityEventInfo::copyNonDefaults(const CvEntityEventInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	for ( int i = 0; i < pClassInfo->getAnimationPathCount(); i++ )
	{
		bool bPushBack = true;
		for ( int j = 0; j < getAnimationPathCount(); j++)
		{
			if ( getAnimationPathType(j) == pClassInfo->getAnimationPathType(i) )
			{
				bPushBack = false;
			}
		}
		if ( bPushBack )
		{
			m_vctAnimationPathType.push_back( pClassInfo->getAnimationPathType(i));
		}
	}

	for ( int i = 0; i < pClassInfo->getEffectTypeCount(); i++ )
	{
		bool bPushBack = true;
		for ( int j = 0; j < getEffectTypeCount(); j++)
		{
			if ( getEffectType(j) == pClassInfo->getEffectType(i) )
			{
				bPushBack = false;
			}
		}
		if ( bPushBack )
		{
			m_vctEffectTypes.push_back( pClassInfo->getEffectType(i));
		}
	}

	if (getUpdateFormation() == bDefault) m_bUpdateFormation = pClassInfo->getUpdateFormation();
}

AnimationPathTypes CvEntityEventInfo::getAnimationPathType(int iIndex) const
{
	return iIndex >= (int)m_vctAnimationPathType.size() ? ANIMATIONPATH_NONE : m_vctAnimationPathType[iIndex];
}

EffectTypes CvEntityEventInfo::getEffectType(int iIndex) const
{
	return iIndex >= (int)m_vctEffectTypes.size() ? NO_EFFECT : m_vctEffectTypes[iIndex];
}

int CvEntityEventInfo::getAnimationPathCount() const
{
	return m_vctAnimationPathType.size();
}

int CvEntityEventInfo::getEffectTypeCount() const
{
	return m_vctEffectTypes.size();
}

bool CvEntityEventInfo::getUpdateFormation() const
{
	return m_bUpdateFormation;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvAssetInfoBase
/////////////////////////////////////////////////////////////////////////////////////////////

const char* CvAssetInfoBase::getTag() const
{
	return getType();
}

void CvAssetInfoBase::setTag(const char* szDesc)
{
	m_szType = szDesc;
}

const char* CvAssetInfoBase::getPath() const
{
	return m_szPath;
}

void CvAssetInfoBase::setPath(const char* szDesc)
{
	m_szPath = szDesc;
}

bool CvAssetInfoBase::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))		// 'tag' is the same as 'type'
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Path");
	setPath(szTextVal);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoAsset
/////////////////////////////////////////////////////////////////////////////////////////////

const char* CvArtInfoAsset::getNIF() const
{
	return m_szNIF;
}

const char* CvArtInfoAsset::getKFM() const
{
	return m_szKFM;
}

void CvArtInfoAsset::setNIF(const char* szDesc)
{
	m_szNIF = szDesc;
}

void CvArtInfoAsset::setKFM(const char* szDesc)
{
	m_szKFM = szDesc;
}

bool CvArtInfoAsset::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvAssetInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NIF"); // TO DO : maybe we should do something different if there is no NIF tag?
	setNIF(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"KFM");// TO DO : maybe we should do something different if there is no KFM tag?
	setKFM(szTextVal);


	return true;
}
void CvArtInfoAsset::copyNonDefaults(const CvArtInfoAsset* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoMovie
/////////////////////////////////////////////////////////////////////////////////////////////

bool CvArtInfoMovie::read(CvXMLLoadUtility* pXML)
{
	return CvArtInfoAsset::read(pXML);
}

void CvArtInfoMovie::copyNonDefaults(const CvArtInfoMovie* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoBonus
/////////////////////////////////////////////////////////////////////////////////////////////

bool CvArtInfoBonus::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}
	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"SHADERNIF");
	setShaderNIF(szTextVal);

	pXML->GetChildXmlValByName(&m_iFontButtonIndex, L"FontButtonIndex");

	return true;
}

void CvArtInfoBonus::copyNonDefaults(const CvArtInfoBonus* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

CvArtInfoBonus::CvArtInfoBonus()
{
	m_iFontButtonIndex = 0;
}

int CvArtInfoBonus::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}

const char* CvArtInfoBonus::getShaderNIF() const
{
	return m_szShaderNIF;
}
void CvArtInfoBonus::setShaderNIF(const char* szDesc)
{
	m_szShaderNIF = szDesc;
}

const CvArtInfoBonus* CvBonusInfo::getArtInfo() const
{
	return ARTFILEMGR.getBonusArtInfo( getArtDefineTag());
}


/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoUnit
/////////////////////////////////////////////////////////////////////////////////////////////

CvArtInfoUnit::CvArtInfoUnit() :
m_fShadowScale(0.0f),
m_iDamageStates(0),
m_bActAsRanged(false),
m_bActAsLand(false),
m_bActAsAir(false),
m_bCombatExempt(false),
m_fTrailWidth(0.0f),
m_fTrailLength(0.0f),
m_fTrailTaper(0.0f),
m_fTrailFadeStartTime(0.0f),
m_fTrailFadeFalloff(0.0f),
m_fBattleDistance(0.0f),
m_fRangedDeathTime(0.0f),
m_fExchangeAngle(0.0f),
m_bSmoothMove(false),
m_fAngleInterRate(FLT_MAX),
m_fBankRate(0),
m_iRunLoopSoundTag(0),
m_iRunEndSoundTag(0),
m_iSelectionSoundScriptId(0),
m_iActionSoundScriptId(0),
m_iPatrolSoundTag(0)
{
}

CvArtInfoUnit::~CvArtInfoUnit()
{
}

bool CvArtInfoUnit::getActAsRanged() const
{
	return m_bActAsRanged;
}

bool CvArtInfoUnit::getActAsLand() const
{
	return m_bActAsLand;
}

bool CvArtInfoUnit::getActAsAir() const
{
	return m_bActAsAir;
}

const char* CvArtInfoUnit::getShaderNIF() const
{
	return m_szShaderNIF;
}

void CvArtInfoUnit::setShaderNIF(const char* szDesc)
{
	m_szShaderNIF = szDesc;
}

const char* CvArtInfoUnit::getShadowNIF() const
{
	return m_szShadowNIF;
}

float CvArtInfoUnit::getShadowScale() const
{
	return m_fShadowScale;
}

const char* CvArtInfoUnit::getShadowAttachNode() const
{
	return m_szShadowAttach;
}

int CvArtInfoUnit::getDamageStates() const
{
	return m_iDamageStates;
}


const char* CvArtInfoUnit::getTrailTexture() const
{
	return m_szTrailTexture;
}

float CvArtInfoUnit::getTrailWidth() const
{
	return m_fTrailWidth;
}

float CvArtInfoUnit::getTrailLength() const
{
	return m_fTrailLength;
}

float CvArtInfoUnit::getTrailTaper() const
{
	return m_fTrailTaper;
}

float CvArtInfoUnit::getTrailFadeStarTime() const
{
	return m_fTrailFadeStartTime;
}

float CvArtInfoUnit::getTrailFadeFalloff() const
{
	return m_fTrailFadeFalloff;
}

float CvArtInfoUnit::getBattleDistance() const
{
	return m_fBattleDistance;
}

float CvArtInfoUnit::getRangedDeathTime() const
{
	return m_fRangedDeathTime;
}

float CvArtInfoUnit::getExchangeAngle() const
{
	return m_fExchangeAngle;
}

bool CvArtInfoUnit::getCombatExempt() const
{
	return m_bCombatExempt;
}

bool CvArtInfoUnit::getSmoothMove() const
{
	return m_bSmoothMove;
}

float CvArtInfoUnit::getAngleInterpRate() const
{
	return m_fAngleInterRate;
}

float CvArtInfoUnit::getBankRate() const
{
	return m_fBankRate;
}

bool CvArtInfoUnit::read(CvXMLLoadUtility* pXML)
{

	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	CvString szTextVal;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ActionSound");
	m_iActionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"SelectionSound");
	m_iSelectionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PatrolSound");
	m_iPatrolSoundTag = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TrainSound");
	setTrainSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bActAsRanged, L"bActAsRanged" );
	pXML->GetOptionalChildXmlValByName(&m_bActAsLand,   L"bActAsLand" );
	pXML->GetOptionalChildXmlValByName(&m_bActAsAir,	L"bActAsAir" );
	pXML->GetOptionalChildXmlValByName(&m_bCombatExempt,   L"bCombatExempt");
	pXML->GetOptionalChildXmlValByName(&m_fExchangeAngle,  L"fExchangeAngle");
	pXML->GetOptionalChildXmlValByName(&m_bSmoothMove,	 L"bSmoothMove");
	pXML->GetOptionalChildXmlValByName(&m_fAngleInterRate, L"fAngleInterpRate", FLT_MAX );
	pXML->GetOptionalChildXmlValByName(&m_fBankRate,	   L"fBankRate");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SHADERNIF");
	setShaderNIF(szTextVal);

	if ( pXML->TryMoveToXmlFirstChild(L"ShadowDef" ))
	{
		pXML->GetOptionalChildXmlValByName( m_szShadowAttach, L"ShadowAttachNode" );
		pXML->GetOptionalChildXmlValByName(m_szShadowNIF, L"ShadowNIF");
		pXML->GetOptionalChildXmlValByName(&m_fShadowScale, L"fShadowScale");
		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(&m_iDamageStates,	L"iDamageStates");
	pXML->GetOptionalChildXmlValByName(&m_fBattleDistance,  L"fBattleDistance");
	pXML->GetOptionalChildXmlValByName(&m_fRangedDeathTime, L"fRangedDeathTime");

	m_fTrailWidth = -1.0f; // invalid.
	if (pXML->TryMoveToXmlFirstChild(L"TrailDefinition"))
	{
		pXML->GetChildXmlValByName(m_szTrailTexture, L"Texture" );
		pXML->GetOptionalChildXmlValByName(&m_fTrailWidth,  L"fWidth", -1.0f);
		pXML->GetOptionalChildXmlValByName(&m_fTrailLength, L"fLength" );
		pXML->GetOptionalChildXmlValByName(&m_fTrailTaper,  L"fTaper" );
		pXML->GetOptionalChildXmlValByName(&m_fTrailFadeStartTime, L"fFadeStartTime" );
		pXML->GetOptionalChildXmlValByName(&m_fTrailFadeFalloff,   L"fFadeFalloff" );
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"AudioRunSounds"))
	{
		pXML->GetOptionalChildXmlValByName(szTextVal, L"AudioRunTypeLoop");
		m_iRunLoopSoundTag = GC.getFootstepAudioTypeByTag(szTextVal);
		pXML->GetOptionalChildXmlValByName(szTextVal, L"AudioRunTypeEnd");
		m_iRunEndSoundTag = GC.getFootstepAudioTypeByTag(szTextVal);
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvArtInfoUnit::copyNonDefaults(const CvArtInfoUnit* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

const char* CvArtInfoUnit::getTrainSound() const
{
	return m_szTrainSound;
}

void CvArtInfoUnit::setTrainSound(const char* szVal)
{
	m_szTrainSound = szVal;
}

int CvArtInfoUnit::getRunLoopSoundTag() const
{
	return m_iRunLoopSoundTag;
}

int CvArtInfoUnit::getRunEndSoundTag() const
{
	return m_iRunEndSoundTag;
}

int CvArtInfoUnit::getPatrolSoundTag() const
{
	return m_iPatrolSoundTag;
}

int CvArtInfoUnit::getSelectionSoundScriptId() const
{
	return m_iSelectionSoundScriptId;
}

int CvArtInfoUnit::getActionSoundScriptId() const
{
	return m_iActionSoundScriptId;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoBuilding
/////////////////////////////////////////////////////////////////////////////////////////////

CvArtInfoBuilding::CvArtInfoBuilding() :
m_bAnimated(false)
{
}

CvArtInfoBuilding::~CvArtInfoBuilding()
{
}

bool CvArtInfoBuilding::isAnimated() const
{
	return m_bAnimated;
}

const char* CvArtInfoBuilding::getLSystemName() const
{
	return m_szLSystemName;
}

bool CvArtInfoBuilding::read(CvXMLLoadUtility* pXML)
{

	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szLSystemName, L"LSystem");

	pXML->GetOptionalChildXmlValByName(&m_bAnimated, L"bAnimated");
	return true;
}
void CvArtInfoBuilding::copyNonDefaults(const CvArtInfoBuilding* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoCivilization
/////////////////////////////////////////////////////////////////////////////////////////////

CvArtInfoCivilization::CvArtInfoCivilization() :
m_bWhiteFlag(false)
{
}

CvArtInfoCivilization::~CvArtInfoCivilization()
{
}

bool CvArtInfoCivilization::isWhiteFlag() const
{
	return m_bWhiteFlag;
}

bool CvArtInfoCivilization::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bWhiteFlag, L"bWhiteFlag");
	return true;
}

void CvArtInfoCivilization::copyNonDefaults(const CvArtInfoCivilization* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoLeaderhead
/////////////////////////////////////////////////////////////////////////////////////////////

const char* CvArtInfoLeaderhead::getNoShaderNIF() const
{
	return m_szNoShaderNIF;
}

void CvArtInfoLeaderhead::setNoShaderNIF(const char* szNIF)
{
	m_szNoShaderNIF = szNIF;
}

const char* CvArtInfoLeaderhead::getBackgroundKFM() const
{
	return m_szBackgroundKFM;
}

void CvArtInfoLeaderhead::setBackgroundKFM(const char* szKFM)
{
	m_szBackgroundKFM = szKFM;
}

bool CvArtInfoLeaderhead::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NoShaderNIF");
	setNoShaderNIF(szTextVal);

	if (pXML->GetOptionalChildXmlValByName(szTextVal, L"BackgroundKFM"))
	{
		setBackgroundKFM(szTextVal);
	}
	else
		setBackgroundKFM("");

	return true;
}

void CvArtInfoLeaderhead::copyNonDefaults(const CvArtInfoLeaderhead* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoScalableAsset
/////////////////////////////////////////////////////////////////////////////////////////////

bool CvArtInfoScalableAsset::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	return CvScalableInfo::read(pXML);
}

void CvArtInfoScalableAsset::copyNonDefaults(const CvArtInfoScalableAsset* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

//////////////////////////////////////////////////////////////////////////
// CvArtInfoImprovement
//////////////////////////////////////////////////////////////////////////

CvArtInfoImprovement::CvArtInfoImprovement() :
m_bExtraAnimations(false)
{
}

CvArtInfoImprovement::~CvArtInfoImprovement()
{
}

bool CvArtInfoImprovement::isExtraAnimations() const
{
	return m_bExtraAnimations;
}

bool CvArtInfoImprovement::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bExtraAnimations, L"bExtraAnimations");

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"SHADERNIF", "");
	setShaderNIF(szTextVal);

	return true;
}

void CvArtInfoImprovement::copyNonDefaults(const CvArtInfoImprovement* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

//////////////////////////////////////////////////////////////////////////
// CvArtInfoTerrain
//////////////////////////////////////////////////////////////////////////

CvArtInfoTerrain::CvArtInfoTerrain()
	: m_iLayerOrder(0)
	, m_bAlphaShader(false)
{
}

const char* CvArtInfoTerrain::getBaseTexture()
{
	return getPath();
}

void CvArtInfoTerrain::setBaseTexture(const char* szTmp)
{
	setPath(szTmp);
}

const char* CvArtInfoTerrain::getGridTexture()
{
	return m_szGridTexture;
}

void CvArtInfoTerrain::setGridTexture(const char* szTmp)
{
	m_szGridTexture = szTmp;
}

const char* CvArtInfoTerrain::getDetailTexture()
{
	return m_szDetailTexture;
}

void CvArtInfoTerrain::setDetailTexture(const char* szTmp)
{
	m_szDetailTexture = szTmp;
}

int CvArtInfoTerrain::getLayerOrder()
{
	return m_iLayerOrder;
}

bool CvArtInfoTerrain::useAlphaShader()
{
	return m_bAlphaShader;
}

CvTextureBlendSlotList& CvArtInfoTerrain::getBlendList(int blendMask)
{
	FAssertMsg(blendMask >= 1 && blendMask < NUM_TEXTURE_BLENDS, "blendMask index must be in valid range");
	return m_pTextureSlots[blendMask];
}

void BuildSlotList( CvTextureBlendSlotList &list, CvString &numlist)
{
	PROFILE_EXTRA_FUNC();
	//convert string to
	char seps[]   = " ,\t\n";
	char *token;
	const char *numstring = numlist;
	// AIAndy: This is really, really dirty as strtok does change the string so it will alter the string representation in CvString which is not supposed to be changed from outside CvString
	// Given the black terrain issue I change that to copying the string first
	//token = strtok( const_cast<char *>(numstring), seps);
	char* szList = new char[numlist.length()+1];
	strcpy(szList, numstring);
	token = strtok( szList, seps);
	while( token != NULL )
	{
		int slot = atoi(token);
		token = strtok( NULL, seps);
		int rotation = atoi(token);
		list.push_back(std::make_pair( slot, rotation));
		token = strtok( NULL, seps);
	}
	delete[] szList;
}

bool CvArtInfoTerrain::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"Grid");
	setGridTexture(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"Detail");
	setDetailTexture(szTextVal);

	pXML->GetChildXmlValByName(&m_iLayerOrder, L"LayerOrder");
	pXML->GetChildXmlValByName(&m_bAlphaShader, L"AlphaShader");

	// Parse texture slots for blend tile lists
	wchar_t xmlName[] = L"TextureBlend00";
	for (int i = 1; i < NUM_TEXTURE_BLENDS; i++)
	{
		swprintf(xmlName + (wcslen(xmlName) - 2), L"%02d", i);
		pXML->GetChildXmlValByName(szTextVal, xmlName);

		m_pSlotNames[i] = szTextVal;
		BuildSlotList(m_pTextureSlots[i], szTextVal);
	}

	// AIAndy: Reading CvArtInfoAsset again is bad
	// return CvArtInfoAsset::read(pXML);
	return true;
}

void CvArtInfoTerrain::copyNonDefaults(const CvArtInfoTerrain* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

//////////////////////////////////////////////////////////////////////////
// CvArtInfoFeature
//////////////////////////////////////////////////////////////////////////

CvArtInfoFeature::CvArtInfoFeature() :
m_bAnimated(false),
m_bRiverArt(false),
m_eTileArtType(TILE_ART_TYPE_NONE),
m_eLightType(LIGHT_TYPE_NONE)
{
}

CvArtInfoFeature::~CvArtInfoFeature()
{
}

bool CvArtInfoFeature::isAnimated() const
{
	return m_bAnimated;
}

bool CvArtInfoFeature::isRiverArt() const
{
	return m_bRiverArt;
}

TileArtTypes CvArtInfoFeature::getTileArtType() const
{
	return m_eTileArtType;
}

LightTypes CvArtInfoFeature::getLightType() const
{
	return m_eLightType;
}

bool CvArtInfoFeature::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bAnimated, L"bAnimated");
	pXML->GetOptionalChildXmlValByName(&m_bRiverArt, L"bRiverArt");

	CvString szTemp;
	pXML->GetOptionalChildXmlValByName(szTemp, L"TileArtType", "TILE_ART_TYPE_NONE");
	if(szTemp.CompareNoCase("TILE_ART_TYPE_NONE") == 0)
		m_eTileArtType = TILE_ART_TYPE_NONE;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_TREES") == 0)
		m_eTileArtType = TILE_ART_TYPE_TREES;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_HALF_TILING") == 0)
		m_eTileArtType = TILE_ART_TYPE_HALF_TILING;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_PLOT_TILING") == 0)
		m_eTileArtType = TILE_ART_TYPE_PLOT_TILING;
	else
	{
		FErrorMsg("[Jason] Unknown TileArtType.");
	}

	pXML->GetOptionalChildXmlValByName(szTemp, L"LightType", "LIGHT_TYPE_NONE");
	if(szTemp.CompareNoCase("LIGHT_TYPE_NONE") == 0)
		m_eLightType = LIGHT_TYPE_NONE;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_SUN") == 0)
		m_eLightType = LIGHT_TYPE_SUN;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_TERRAIN") == 0)
		m_eLightType = LIGHT_TYPE_TERRAIN;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_UNIT") == 0)
		m_eLightType = LIGHT_TYPE_UNIT;
	else
	{
		FErrorMsg("[Jason] Unknown LightType.");
	}

	//feature varieties
	if(pXML->TryMoveToXmlFirstChild(L"FeatureVariety"))
	{
		do
		{
			m_aFeatureVarieties.push_back(FeatureVariety());
			FeatureVariety &featureVariety = m_aFeatureVarieties.back();

			//generate rotations
			bool bGenerateRotations = false;
			pXML->GetOptionalChildXmlValByName(&bGenerateRotations, L"bGenerateRotations");

			//feature art pieces
			if(pXML->TryMoveToXmlFirstChild(L"FeatureArtPieces"))
			{
				if(pXML->TryMoveToXmlFirstChild(L"FeatureArtPiece"))
				{
					do
					{
						//connection mask
						pXML->GetOptionalChildXmlValByName(szTemp, L"Connections");
						int connectionMask = getConnectionMaskFromString(szTemp);

						//model files
						if(pXML->TryMoveToXmlFirstChild(L"ModelFile"))
						{
							do
							{
								pXML->GetXmlVal(szTemp);

								if(!bGenerateRotations)
								{
									FeatureArtPiece &featureArtPiece = featureVariety.createFeatureArtPieceFromConnectionMask(connectionMask);
									featureArtPiece.m_aArtModels.push_back(FeatureArtModel(szTemp, ROTATE_NONE));
								}
								else
								{
									for(int i=0;i<NUM_ROTATION_TYPES;i++)
									{
										const int newConnectionMask = getRotatedConnectionMask(connectionMask, (RotationTypes) i);
										FeatureArtPiece &featureArtPiece = featureVariety.createFeatureArtPieceFromConnectionMask(newConnectionMask);
										featureArtPiece.m_aArtModels.push_back(FeatureArtModel(szTemp, (RotationTypes) i));
									}
								}
							} while(pXML->TryMoveToXmlNextSibling(L"ModelFile"));

							pXML->MoveToXmlParent();
						}
					} while(pXML->TryMoveToXmlNextSibling(L"FeatureArtPiece"));

					pXML->MoveToXmlParent();
				}

				pXML->MoveToXmlParent();
			}

			//feature art pieces
			if(pXML->TryMoveToXmlFirstChild(L"FeatureDummyNodes"))
			{
				if(pXML->TryMoveToXmlFirstChild(L"FeatureDummyNode"))
				{
					do
					{
						CvString tagName;
						CvString nodeName;

						//tag name
						pXML->GetChildXmlValByName(tagName, L"Tag");

						//node name
						pXML->GetChildXmlValByName(nodeName, L"Name");

						featureVariety.createFeatureDummyNode(tagName, nodeName);
					} while(pXML->TryMoveToXmlNextSibling(L"FeatureDummyNode"));

					pXML->MoveToXmlParent();
				}

				pXML->MoveToXmlParent();
			}

			//variety button
			pXML->GetChildXmlValByName(featureVariety.m_szVarietyButton, L"VarietyButton");
		} while(pXML->TryMoveToXmlNextSibling(L"FeatureVariety"));

		pXML->MoveToXmlParent();
	}

	return true;
}

void CvArtInfoFeature::dump()
{
	PROFILE_EXTRA_FUNC();
	OutputDebugString(CvString::format("CvArtInfoFeature: %s:\n", m_szType.c_str()).c_str());
	int iNum = m_aFeatureVarieties.size();
	OutputDebugString(CvString::format("\t%d varieties:\n", iNum).c_str());
	for(int i = 0; i < iNum; i++)
	{
		FeatureVariety &featureVariety = m_aFeatureVarieties[i];

		//feature art pieces
		int iNumArtPieces = featureVariety.m_aFeatureArtPieces.size();
		OutputDebugString(CvString::format("\t\t%d art pieces:\n", iNumArtPieces).c_str());
		for(int j = 0; j < iNumArtPieces; j++)
		{
			CvArtInfoFeature::FeatureArtPiece& kArtPiece = featureVariety.m_aFeatureArtPieces[j];
			//connection mask
			OutputDebugString(CvString::format("\t\t\tConnection mask: %d\n", kArtPiece.getConnectionMask()).c_str());

			//model files
			int iNumModelFiles = kArtPiece.m_aArtModels.size();
			OutputDebugString(CvString::format("\t\t\t%d art models:\n", iNumModelFiles).c_str());
			for(int k = 0; k < iNumModelFiles; k++)
			{
				FeatureArtModel kArtModel = kArtPiece.m_aArtModels[k];

				OutputDebugString(CvString::format("\t\t\t\t%Rotation %d, file: %s\n", kArtModel.getRotation(), kArtModel.getModelFile().c_str()).c_str());
			}
		}

		//feature dummy nodes
		int iNumDummyNodes = featureVariety.m_aFeatureDummyNodes.size();
		OutputDebugString(CvString::format("\t\t%d dummy nodes:\n", iNumDummyNodes).c_str());
		for(int j = 0; j < iNumDummyNodes; j++)
		{
			FeatureDummyNode&	kNode = featureVariety.m_aFeatureDummyNodes[j];

			OutputDebugString(CvString::format("\t\t\tTag %s, name %s\n", kNode.getTagName().c_str(), kNode.getNodeName().c_str()).c_str());
		}

		//variety button
		OutputDebugString(CvString::format("\t\t%Variety button: %s\n", featureVariety.m_szVarietyButton.c_str()).c_str());
	}
}

void CvArtInfoFeature::copyNonDefaults(const CvArtInfoFeature* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

const CvArtInfoFeature::FeatureVariety &CvArtInfoFeature::getVariety(int index) const
{
	//FAssertMsg((index >= 0) && (index < (int) m_aFeatureVarieties.size()), "[Jason] Feature Variety index out of range.");
	//AIAndy: limit to valid range
	const int iIndex = range(index, 0, getNumVarieties() - 1);
	return m_aFeatureVarieties[iIndex];
}

int CvArtInfoFeature::getNumVarieties() const
{
	return m_aFeatureVarieties.size();
}

const std::string CvArtInfoFeature::getFeatureDummyNodeName(int variety, const std::string tagName) const
{
	return getVariety(variety).getFeatureDummyNodeName(tagName);
}

int CvArtInfoFeature::getConnectionMaskFromString(const CvString& connectionString) const
{
	PROFILE_EXTRA_FUNC();
	if(connectionString.IsEmpty())
		return 0;
	else
	{
		std::vector<CvString> tokens;
		connectionString.getTokens(" \t\n", tokens);

		int connectionMask = 0;
		for(int i=0;i<(int)tokens.size();i++)
		{
			// found a token, parse it.
			const CvString& token = tokens[i];
			if(token.CompareNoCase("NW") == 0)
				connectionMask |= DIRECTION_NORTHWEST_MASK;
			else if(token.CompareNoCase("N") == 0)
				connectionMask |= DIRECTION_NORTH_MASK;
			else if(token.CompareNoCase("NE") == 0)
				connectionMask |= DIRECTION_NORTHEAST_MASK;
			else if(token.CompareNoCase("E") == 0)
				connectionMask |= DIRECTION_EAST_MASK;
			else if(token.CompareNoCase("SE") == 0)
				connectionMask |= DIRECTION_SOUTHEAST_MASK;
			else if(token.CompareNoCase("S") == 0)
				connectionMask |= DIRECTION_SOUTH_MASK;
			else if(token.CompareNoCase("SW") == 0)
				connectionMask |= DIRECTION_SOUTHWEST_MASK;
			else if(token.CompareNoCase("W") == 0)
				connectionMask |= DIRECTION_WEST_MASK;
			else
			{
				FErrorMsg("[Jason] Invalid connection direction.");
			}
		}

		FAssertMsg(connectionMask > 0, "[Jason] Did not find feature connection mask.");
		return connectionMask;
	}
}

int CvArtInfoFeature::getRotatedConnectionMask(int connectionMask, RotationTypes rotation) const
{
	if(rotation == ROTATE_90CW)
		connectionMask = connectionMask << 2; //rotate two directions CW
	else if(rotation == ROTATE_180CW)
		connectionMask = connectionMask << 4; //rotate four directions CW
	else if(rotation == ROTATE_270CW)
		connectionMask = connectionMask << 6; //rotate six directions CW

	//renormalize directions that wrapped around
	connectionMask = connectionMask | (connectionMask >> 8);
	connectionMask = connectionMask & 255;
	return connectionMask;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvEmphasizeInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvEmphasizeInfo::CvEmphasizeInfo() :
 m_bAvoidGrowth(false)
,m_bGreatPeople(false)
,m_piYieldModifiers(NULL)
,m_piCommerceModifiers(NULL)
,m_bAvoidAngryCitizens(false)
,m_bAvoidUnhealthyCitizens(false)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvEmphasizeInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvEmphasizeInfo::~CvEmphasizeInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldModifiers);
	SAFE_DELETE_ARRAY(m_piCommerceModifiers);
}

bool CvEmphasizeInfo::isAvoidGrowth() const
{
	return m_bAvoidGrowth;
}

bool CvEmphasizeInfo::isGreatPeople() const
{
	return m_bGreatPeople;
}

bool CvEmphasizeInfo::isAvoidAngryCitizens() const
{
	return m_bAvoidAngryCitizens;
}

bool CvEmphasizeInfo::isAvoidUnhealthyCitizens() const
{
	return m_bAvoidUnhealthyCitizens;
}


// Arrays

int CvEmphasizeInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldModifiers ? m_piYieldModifiers[i] : 0;
}

int CvEmphasizeInfo::getCommerceChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifiers ? m_piCommerceModifiers[i] : 0;
}

//
// read from XML
//
bool CvEmphasizeInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bAvoidGrowth, L"bAvoidGrowth");
	pXML->GetChildXmlValByName(&m_bGreatPeople, L"bGreatPeople");

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifiers);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifiers);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifiers);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifiers);
	}
	pXML->GetChildXmlValByName(&m_bAvoidAngryCitizens, L"bAvoidAngryCitizens");
	pXML->GetChildXmlValByName(&m_bAvoidUnhealthyCitizens, L"bAvoidUnhealthyCitizens");

	return true;
}
void CvEmphasizeInfo::copyNonDefaults(const CvEmphasizeInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isAvoidGrowth() == bDefault) m_bAvoidGrowth = pClassInfo->isAvoidGrowth();
	if (isGreatPeople() == bDefault) m_bGreatPeople = pClassInfo->isGreatPeople();

	if (!m_piYieldModifiers) CvXMLLoadUtility::InitList(&m_piYieldModifiers, NUM_YIELD_TYPES);
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( m_piYieldModifiers[i] == iDefault )
		{
			m_piYieldModifiers[i] = pClassInfo->getYieldChange(i);
		}
	}

	if (!m_piCommerceModifiers) CvXMLLoadUtility::InitList(&m_piCommerceModifiers, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_piCommerceModifiers[i] == iDefault )
		{
			m_piCommerceModifiers[i] = pClassInfo->getCommerceChange(i);
		}
	}
	if (isAvoidAngryCitizens() == false) m_bAvoidAngryCitizens = pClassInfo->isAvoidAngryCitizens();
	if (isAvoidUnhealthyCitizens() == false) m_bAvoidUnhealthyCitizens = pClassInfo->isAvoidUnhealthyCitizens();
}

//------------------------------------------------------------------------------------------------------
//
//  CvUpkeepInfo
//

CvUpkeepInfo::CvUpkeepInfo() :
m_iPopulationPercent(0),
m_iCityPercent(0)
{
}

CvUpkeepInfo::~CvUpkeepInfo()
{
}

int CvUpkeepInfo::getPopulationPercent() const
{
	return m_iPopulationPercent;
}

int CvUpkeepInfo::getCityPercent() const
{
	return m_iCityPercent;
}

bool CvUpkeepInfo::read(CvXMLLoadUtility* pXml)
{
	if (!CvInfoBase::read(pXml))
	{
		return false;
	}

	pXml->GetChildXmlValByName(&m_iPopulationPercent, L"iPopulationPercent");
	pXml->GetChildXmlValByName(&m_iCityPercent, L"iCityPercent");

	return true;
}

void CvUpkeepInfo::copyNonDefaults(const CvUpkeepInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPopulationPercent() == iDefault) m_iPopulationPercent = pClassInfo->getPopulationPercent();
	if (getCityPercent() == iDefault) m_iCityPercent = pClassInfo->getCityPercent();
}

void CvUpkeepInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iPopulationPercent);
	CheckSum(iSum, m_iCityPercent);
}

//------------------------------------------------------------------------------------------------------
//
//  CvCultureLevelInfo
//

CvCultureLevelInfo::CvCultureLevelInfo() :
m_iLevel(-1),
m_iCityDefenseModifier(0),

m_iCityRadius(1),
m_iMaxWorldWonders(1),
m_iMaxTeamWonders(1),
m_iMaxNationalWonders(1),
m_iMaxNationalWondersOCC(1),
m_iPrereqGameOption(NO_GAMEOPTION),

m_paiSpeedThreshold(NULL)
{
}

CvCultureLevelInfo::~CvCultureLevelInfo()
{
	SAFE_DELETE_ARRAY(m_paiSpeedThreshold);
}

int CvCultureLevelInfo::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}

int CvCultureLevelInfo::getSpeedThreshold(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameSpeedInfos(), i);
	return m_paiSpeedThreshold ? m_paiSpeedThreshold[i] : 0;
}


int CvCultureLevelInfo::getCityRadius() const
{
	return m_iCityRadius;
}

int CvCultureLevelInfo::getMaxWorldWonders() const
{
	return m_iMaxWorldWonders;
}

int CvCultureLevelInfo::getMaxTeamWonders() const
{
	return m_iMaxTeamWonders;
}

int CvCultureLevelInfo::getMaxNationalWonders() const
{
	return m_iMaxNationalWonders;
}

int CvCultureLevelInfo::getMaxNationalWondersOCC() const
{
	return m_iMaxNationalWondersOCC;
}

int CvCultureLevelInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}


bool CvCultureLevelInfo::read(CvXMLLoadUtility* pXml)
{
	if (!CvInfoBase::read(pXml))
	{
		return false;
	}

	pXml->GetOptionalChildXmlValByName(&m_iCityDefenseModifier, L"iCityDefenseModifier");

	pXml->GetOptionalChildXmlValByName(&m_iCityRadius, L"iCityRadius", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxWorldWonders, L"iMaxWorldWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxTeamWonders, L"iMaxTeamWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxNationalWonders, L"iMaxNationalWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxNationalWondersOCC, L"iMaxNationalWondersOCC", 1);
	CvString szTextVal;
	pXml->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXml->GetInfoClass(szTextVal);

	pXml->SetVariableListTagPair(&m_paiSpeedThreshold, L"SpeedThresholds", GC.getNumGameSpeedInfos());

	return true;
}

void CvCultureLevelInfo::copyNonDefaults(const CvCultureLevelInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getCityDefenseModifier() == iDefault) m_iCityDefenseModifier = pClassInfo->getCityDefenseModifier();
	if (getCityRadius() == 1) m_iCityRadius = pClassInfo->getCityRadius();
	if (getMaxWorldWonders() == 1) m_iMaxWorldWonders = pClassInfo->getMaxWorldWonders();
	if (getMaxTeamWonders() == 1) m_iMaxTeamWonders = pClassInfo->getMaxTeamWonders();
	if (getMaxNationalWonders() == 1) m_iMaxNationalWonders = pClassInfo->getMaxNationalWonders();
	if (getMaxNationalWondersOCC() == 1) m_iMaxNationalWondersOCC = pClassInfo->getMaxNationalWondersOCC();
	if (getPrereqGameOption() == NO_GAMEOPTION) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();

	for ( int i = 0; i < GC.getNumGameSpeedInfos(); i++ )
	{
		if ( getSpeedThreshold(i) == iDefault && pClassInfo->getSpeedThreshold(i) != iDefault)
		{
			if ( NULL == m_paiSpeedThreshold )
			{
				CvXMLLoadUtility::InitList(&m_paiSpeedThreshold,GC.getNumGameSpeedInfos(),iDefault);
			}
			m_paiSpeedThreshold[i] = pClassInfo->getSpeedThreshold(i);
		}
	}
}

void CvCultureLevelInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iCityDefenseModifier);
	CheckSum(iSum, m_iCityRadius);
	CheckSum(iSum, m_iMaxWorldWonders);
	CheckSum(iSum, m_iMaxTeamWonders);
	CheckSum(iSum, m_iMaxNationalWonders);
	CheckSum(iSum, m_iMaxNationalWondersOCC);
	CheckSum(iSum, m_iPrereqGameOption);

	CheckSum(iSum, m_paiSpeedThreshold, GC.getNumGameSpeedInfos());
}

//------------------------------------------------------------------------------------------------------
//
//  CvEraInfo
//

CvEraInfo::CvEraInfo() :
m_iStartingUnitMultiplier(0),
m_iStartingDefenseUnits(0),
m_iStartingWorkerUnits(0),
m_iStartingExploreUnits(0),
m_iAdvancedStartPoints(0),
m_iStartingGold(0),
m_iFreePopulation(0),
m_iStartPercent(0),
m_iGrowthPercent(0),
m_iTrainPercent(0),
m_iConstructPercent(0),
m_iCreatePercent(0),
m_iResearchPercent(0),
m_iBuildPercent(0),
m_iImprovementPercent(0),
m_iGreatPeoplePercent(0),
m_iAnarchyPercent(0),
m_iEventChancePerTurn(0),
m_iSoundtrackSpace(0),
m_iNumSoundtracks(0),
m_iBeelineStingsTechCostModifier(0),
m_bNoGoodies(false),
m_bNoAnimals(false),
m_bNoBarbUnits(false),
m_bNoBarbCities(false),
m_bFirstSoundtrackFirst(false),
m_paiCitySoundscapeSciptIds(NULL),
m_paiSoundtracks(NULL)
,m_iInitialCityMaintenancePercent(0)
{ }

CvEraInfo::~CvEraInfo()
{
	SAFE_DELETE_ARRAY(m_paiCitySoundscapeSciptIds);
	SAFE_DELETE_ARRAY(m_paiSoundtracks);
}

int CvEraInfo::getStartingUnitMultiplier() const
{
	return m_iStartingUnitMultiplier;
}

int CvEraInfo::getStartingDefenseUnits() const
{
	return m_iStartingDefenseUnits;
}

int CvEraInfo::getStartingWorkerUnits() const
{
	return m_iStartingWorkerUnits;
}

int CvEraInfo::getStartingExploreUnits() const
{
	return m_iStartingExploreUnits;
}

int CvEraInfo::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}

int CvEraInfo::getStartingGold() const
{
	return m_iStartingGold;
}

int CvEraInfo::getFreePopulation() const
{
	return m_iFreePopulation;
}

int CvEraInfo::getStartPercent() const
{
	return m_iStartPercent;
}

int CvEraInfo::getGrowthPercent() const
{
	return m_iGrowthPercent;
}

int CvEraInfo::getTrainPercent() const
{
	return m_iTrainPercent;
}

int CvEraInfo::getConstructPercent() const
{
	return m_iConstructPercent;
}

int CvEraInfo::getCreatePercent() const
{
	return m_iCreatePercent;
}

int CvEraInfo::getResearchPercent() const
{
	return m_iResearchPercent;
}

int CvEraInfo::getBuildPercent() const
{
	return m_iBuildPercent;
}

int CvEraInfo::getImprovementPercent() const
{
	return m_iImprovementPercent;
}

int CvEraInfo::getGreatPeoplePercent() const
{
	return m_iGreatPeoplePercent;
}

int CvEraInfo::getAnarchyPercent() const
{
	return m_iAnarchyPercent;
}

int CvEraInfo::getEventChancePerTurn() const
{
	return m_iEventChancePerTurn;
}

int CvEraInfo::getSoundtrackSpace() const
{
	return m_iSoundtrackSpace;
}

bool CvEraInfo::isFirstSoundtrackFirst() const
{
	return m_bFirstSoundtrackFirst;
}

int CvEraInfo::getNumSoundtracks() const
{
	return m_iNumSoundtracks;
}

int CvEraInfo::getBeelineStingsTechCostModifier() const
{
	return m_iBeelineStingsTechCostModifier;
}

const char* CvEraInfo::getAudioUnitVictoryScript() const
{
	return m_szAudioUnitVictoryScript;
}

const char* CvEraInfo::getAudioUnitDefeatScript() const
{
	return m_szAudioUnitDefeatScript;
}

bool CvEraInfo::isNoGoodies() const
{
	return m_bNoGoodies;
}

bool CvEraInfo::isNoAnimals() const
{
	return m_bNoAnimals;
}

bool CvEraInfo::isNoBarbUnits() const
{
	return m_bNoBarbUnits;
}

bool CvEraInfo::isNoBarbCities() const
{
	return m_bNoBarbCities;
}

int CvEraInfo::getInitialCityMaintenancePercent() const
{
	return m_iInitialCityMaintenancePercent;
}


// Arrays

int CvEraInfo::getSoundtracks(int i) const
{
	FASSERT_BOUNDS(0, getNumSoundtracks(), i);
	return m_paiSoundtracks ? m_paiSoundtracks[i] : -1;
}

int CvEraInfo::getCitySoundscapeSciptId(int i) const
{
//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCitySoundscapeSciptIds ? m_paiCitySoundscapeSciptIds[i] : -1;
}

bool CvEraInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bNoGoodies, L"bNoGoodies");
	pXML->GetOptionalChildXmlValByName(&m_bNoAnimals, L"bNoAnimals");
	pXML->GetOptionalChildXmlValByName(&m_bNoBarbUnits, L"bNoBarbUnits");
	pXML->GetOptionalChildXmlValByName(&m_bNoBarbCities, L"bNoBarbCities");
	pXML->GetOptionalChildXmlValByName(&m_iStartingUnitMultiplier, L"iStartingUnitMultiplier");
	pXML->GetOptionalChildXmlValByName(&m_iStartingDefenseUnits, L"iStartingDefenseUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingWorkerUnits, L"iStartingWorkerUnits");
	pXML->GetOptionalChildXmlValByName(&m_iStartingExploreUnits, L"iStartingExploreUnits");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartPoints, L"iAdvancedStartPoints");
	pXML->GetOptionalChildXmlValByName(&m_iStartingGold, L"iStartingGold");
	pXML->GetOptionalChildXmlValByName(&m_iFreePopulation, L"iFreePopulation");
	pXML->GetOptionalChildXmlValByName(&m_iStartPercent, L"iStartPercent");
	pXML->GetOptionalChildXmlValByName(&m_iGrowthPercent, L"iGrowthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTrainPercent, L"iTrainPercent");
	pXML->GetOptionalChildXmlValByName(&m_iConstructPercent, L"iConstructPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCreatePercent, L"iCreatePercent");
	pXML->GetOptionalChildXmlValByName(&m_iResearchPercent, L"iResearchPercent");
	pXML->GetOptionalChildXmlValByName(&m_iBuildPercent, L"iBuildPercent");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementPercent, L"iImprovementPercent");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeoplePercent, L"iGreatPeoplePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAnarchyPercent, L"iAnarchyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iEventChancePerTurn, L"iEventChancePerTurn");
	pXML->GetOptionalChildXmlValByName(&m_iSoundtrackSpace, L"iSoundtrackSpace");
	pXML->GetOptionalChildXmlValByName(&m_bFirstSoundtrackFirst, L"bFirstSoundtrackFirst");
	pXML->GetOptionalChildXmlValByName(m_szAudioUnitVictoryScript, L"AudioUnitVictoryScript");
	pXML->GetOptionalChildXmlValByName(m_szAudioUnitDefeatScript, L"AudioUnitDefeatScript");
	pXML->GetOptionalChildXmlValByName(&m_iInitialCityMaintenancePercent, L"iInitialCityMaintenancePercent");
	pXML->GetOptionalChildXmlValByName(&m_iBeelineStingsTechCostModifier, L"iBeelineStingsTechCostModifier");

	if (m_iInitialCityMaintenancePercent < 0)
	{
		m_iInitialCityMaintenancePercent = 0;
	}

	if (pXML->TryMoveToXmlFirstChild(L"EraInfoSoundtracks"))
	{
		CvString* pszSoundTrackNames = NULL;
		pXML->SetStringList(&pszSoundTrackNames, &m_iNumSoundtracks);

		if (m_iNumSoundtracks > 0)
		{
			m_paiSoundtracks = new int[m_iNumSoundtracks];

			int j;
			for (j=0;j<m_iNumSoundtracks;j++)
			{
				m_paiSoundtracks[j] = ((!gDLL->getAudioDisabled()) ? gDLL->getAudioTagIndex(pszSoundTrackNames[j], AUDIOTAG_2DSCRIPT) : -1);
			}
		}
		else
		{
			m_paiSoundtracks = NULL;
		}

		pXML->MoveToXmlParent();

		SAFE_DELETE_ARRAY(pszSoundTrackNames);
	}

	pXML->SetVariableListTagPairForAudioScripts(&m_paiCitySoundscapeSciptIds, L"CitySoundscapes", GC.getNumCitySizeTypes());

	return true;
}

void CvEraInfo::copyNonDefaults(const CvEraInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iAudioDefault = -1;  //all audio is default -1
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isNoGoodies() == bDefault) m_bNoGoodies = pClassInfo->isNoGoodies();
	if (isNoAnimals() == bDefault) m_bNoAnimals = pClassInfo->isNoAnimals();
	if (isNoBarbUnits() == bDefault) m_bNoBarbUnits = pClassInfo->isNoBarbUnits();
	if (isNoBarbCities() == bDefault) m_bNoBarbCities = pClassInfo->isNoBarbCities();
	if (getStartingUnitMultiplier() == iDefault) m_iStartingUnitMultiplier = pClassInfo->getStartingUnitMultiplier();
	if (getStartingDefenseUnits() == iDefault) m_iStartingDefenseUnits = pClassInfo->getStartingDefenseUnits();
	if (getStartingWorkerUnits() == iDefault) m_iStartingWorkerUnits = pClassInfo->getStartingWorkerUnits();
	if (getStartingExploreUnits() == iDefault) m_iStartingExploreUnits = pClassInfo->getStartingExploreUnits();
	if (getAdvancedStartPoints() == iDefault) m_iAdvancedStartPoints = pClassInfo->getAdvancedStartPoints();
	if (getStartingGold() == iDefault) m_iStartingGold = pClassInfo->getStartingGold();
	if (getFreePopulation() == iDefault) m_iFreePopulation = pClassInfo->getFreePopulation();
	if (getStartPercent() == iDefault) m_iStartPercent = pClassInfo->getStartPercent();
	if (getGrowthPercent() == iDefault) m_iGrowthPercent = pClassInfo->getGrowthPercent();
	if (getTrainPercent() == iDefault) m_iTrainPercent = pClassInfo->getTrainPercent();
	if (getConstructPercent() == iDefault) m_iConstructPercent = pClassInfo->getConstructPercent();
	if (getCreatePercent() == iDefault) m_iCreatePercent = pClassInfo->getCreatePercent();
	if (getResearchPercent() == iDefault) m_iResearchPercent = pClassInfo->getResearchPercent();
	if (getBuildPercent() == iDefault) m_iBuildPercent = pClassInfo->getBuildPercent();
	if (getImprovementPercent() == iDefault) m_iImprovementPercent = pClassInfo->getImprovementPercent();
	if (getGreatPeoplePercent() == iDefault) m_iGreatPeoplePercent = pClassInfo->getGreatPeoplePercent();
	if (getAnarchyPercent() == iDefault) m_iAnarchyPercent = pClassInfo->getAnarchyPercent();
	if (getBeelineStingsTechCostModifier() == iDefault) m_iBeelineStingsTechCostModifier = pClassInfo->getBeelineStingsTechCostModifier();
	if (getEventChancePerTurn() == iDefault) m_iEventChancePerTurn = pClassInfo->getEventChancePerTurn();
	if (getSoundtrackSpace() == iDefault) m_iSoundtrackSpace = pClassInfo->getSoundtrackSpace();
	if (isFirstSoundtrackFirst() == bDefault) m_bFirstSoundtrackFirst = pClassInfo->isFirstSoundtrackFirst();
	if (getAudioUnitVictoryScript() == cDefault) m_szAudioUnitVictoryScript = pClassInfo->getAudioUnitVictoryScript();
	if (getAudioUnitDefeatScript() == cDefault) m_szAudioUnitDefeatScript = pClassInfo->getAudioUnitDefeatScript();
	if (m_iInitialCityMaintenancePercent == iDefault) m_iInitialCityMaintenancePercent = pClassInfo->getInitialCityMaintenancePercent();

	if (m_iInitialCityMaintenancePercent < 0)
	{
		m_iInitialCityMaintenancePercent = 0;
	}

	if ( pClassInfo->getNumSoundtracks() != 0 )
	{
		int iNumSoundTracks = getNumSoundtracks() + pClassInfo->getNumSoundtracks();
		int* m_paiSoundtracksTemp = new int[iNumSoundTracks];
		for ( int i = 0; i < iNumSoundTracks; i++)
		{
			if ( i < getNumSoundtracks() )
			{
				m_paiSoundtracksTemp[i] = getSoundtracks(i);
			}
			else
			{
				m_paiSoundtracksTemp[i] = pClassInfo->getSoundtracks(i - getNumSoundtracks());
			}
		}
		SAFE_DELETE_ARRAY(m_paiSoundtracks);
		m_paiSoundtracks = new int[iNumSoundTracks];
		for ( int i = 0; i < iNumSoundTracks; i++)
		{
			m_paiSoundtracks[i] = m_paiSoundtracksTemp[i];
		}
		SAFE_DELETE_ARRAY(m_paiSoundtracksTemp);
	}

	for ( int i = 0; i < GC.getNumCitySizeTypes(); i++)
	{
		if ( getCitySoundscapeSciptId(i) == iAudioDefault && pClassInfo->getCitySoundscapeSciptId(i) != iAudioDefault)
		{
			if ( NULL == m_paiCitySoundscapeSciptIds )
			{
				CvXMLLoadUtility::InitList(&m_paiCitySoundscapeSciptIds,GC.getNumCitySizeTypes(),iAudioDefault);
			}
			m_paiCitySoundscapeSciptIds[i] = pClassInfo->getCitySoundscapeSciptId(i);
		}
	}
}

void CvEraInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iStartingUnitMultiplier);
	CheckSum(iSum, m_iStartingDefenseUnits);
	CheckSum(iSum, m_iStartingWorkerUnits);
	CheckSum(iSum, m_iStartingExploreUnits);
	CheckSum(iSum, m_iAdvancedStartPoints);
	CheckSum(iSum, m_iStartingGold);
	CheckSum(iSum, m_iFreePopulation);
	CheckSum(iSum, m_iStartPercent);
	CheckSum(iSum, m_iGrowthPercent);
	CheckSum(iSum, m_iTrainPercent);
	CheckSum(iSum, m_iConstructPercent);
	CheckSum(iSum, m_iCreatePercent);
	CheckSum(iSum, m_iResearchPercent);
	CheckSum(iSum, m_iBuildPercent);
	CheckSum(iSum, m_iImprovementPercent);
	CheckSum(iSum, m_iGreatPeoplePercent);
	CheckSum(iSum, m_iAnarchyPercent);
	CheckSum(iSum, m_iEventChancePerTurn);
	CheckSum(iSum, m_iBeelineStingsTechCostModifier);

	CheckSum(iSum, m_bNoGoodies);
	CheckSum(iSum, m_bNoAnimals);
	CheckSum(iSum, m_bNoBarbUnits);
	CheckSum(iSum, m_bNoBarbCities);
}

//======================================================================================================
//					CvColorInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvColorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvColorInfo::CvColorInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvColorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvColorInfo::~CvColorInfo()
{
}

const NiColorA& CvColorInfo::getColor() const
{
	return m_Color;
}

bool CvColorInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	float afColorVals[4];	// array to hold the 4 color values, red, green, blue, and alpha

	pXML->GetChildXmlValByName(&afColorVals[0], L"fRed");
	pXML->GetChildXmlValByName(&afColorVals[1], L"fGreen");
	pXML->GetChildXmlValByName(&afColorVals[2], L"fBlue");
	pXML->GetChildXmlValByName(&afColorVals[3], L"fAlpha");

	m_Color = NiColorA(afColorVals[0], afColorVals[1], afColorVals[2], afColorVals[3]);

	return true;
}

void CvColorInfo::copyNonDefaults(const CvColorInfo* pClassInfo)
{
	//const float fDefault = 0.0f;

	CvInfoBase::copyNonDefaults(pClassInfo);

//	if (afColorVals[0] == fDefault && afColorVals[1] == fDefault && afColorVals[2] == fDefault
//		 && afColorVals[3] == fDefault) m_Color = pClassInfo->getColor();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvPlayerColorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvPlayerColorInfo::CvPlayerColorInfo() :
m_iColorTypePrimary(NO_COLOR),
m_iColorTypeSecondary(NO_COLOR),
m_iTextColorType(NO_COLOR)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvPlayerColorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvPlayerColorInfo::~CvPlayerColorInfo()
{
}

int CvPlayerColorInfo::getColorTypePrimary() const
{
	return m_iColorTypePrimary;
}

int CvPlayerColorInfo::getColorTypeSecondary() const
{
	return m_iColorTypeSecondary;
}

int CvPlayerColorInfo::getTextColorType() const
{
	return m_iTextColorType;
}

bool CvPlayerColorInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"ColorTypePrimary");
	m_iColorTypePrimary = pXML->GetInfoClass( szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"ColorTypeSecondary");
	m_iColorTypeSecondary = pXML->GetInfoClass( szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"TextColorType");
	m_iTextColorType = pXML->GetInfoClass( szTextVal);

	return true;
}

void CvPlayerColorInfo::copyNonDefaults(const CvPlayerColorInfo* pClassInfo)
{
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getColorTypePrimary() == iTextDefault) m_iColorTypePrimary = pClassInfo->getColorTypePrimary();
	if (getColorTypeSecondary() == iTextDefault) m_iColorTypeSecondary = pClassInfo->getColorTypeSecondary();
	if (getTextColorType() == iTextDefault) m_iTextColorType = pClassInfo->getTextColorType();
}

//------------------------------------------------------------------------------------------------------
//
//	CvLandscapeInfo
//
//
CvLandscapeInfo::CvLandscapeInfo() :
m_iFogR(0),
m_iFogG(0),
m_iFogB(0),
m_iHorizontalGameCell(0),
m_iVerticalGameCell(0),
m_iPlotsPerCellX(0),
m_iPlotsPerCellY(0),
m_iHorizontalVertCnt(0),
m_iVerticalVertCnt(0),
m_iWaterHeight(0),
m_fTextureScaleX(0.0f),
m_fTextureScaleY(0.0f),
m_fZScale(0.0f),
m_fPeakScale(0.0f),
m_fHillScale(0.0f),
m_bUseTerrainShader(false),
m_bUseLightmap(false),
m_bRandomMap(false)
{
}

int CvLandscapeInfo::getFogR() const
{
	return m_iFogR;
}

int CvLandscapeInfo::getFogG() const
{
	return m_iFogG;
}

int CvLandscapeInfo::getFogB() const
{
	return m_iFogB;
}

int CvLandscapeInfo::getHorizontalGameCell() const
{
	return m_iHorizontalGameCell;
}

int CvLandscapeInfo::getVerticalGameCell() const
{
	return m_iVerticalGameCell;
}

int CvLandscapeInfo::getPlotsPerCellX() const
{
	return m_iPlotsPerCellX;
}

int CvLandscapeInfo::getPlotsPerCellY() const
{
	return m_iPlotsPerCellY;
}

int CvLandscapeInfo::getHorizontalVertCnt() const
{
	return m_iHorizontalVertCnt;
}

int CvLandscapeInfo::getVerticalVertCnt() const
{
	return m_iVerticalVertCnt;
}

int CvLandscapeInfo::getWaterHeight() const
{
	return m_iWaterHeight;
}

float CvLandscapeInfo::getTextureScaleX() const
{
	return m_fTextureScaleX;
}

float CvLandscapeInfo::getTextureScaleY() const
{
	return m_fTextureScaleY;
}

float CvLandscapeInfo::getZScale() const
{
	return m_fZScale;
}

bool CvLandscapeInfo::isUseTerrainShader() const
{
	return m_bUseTerrainShader;
}

bool CvLandscapeInfo::isUseLightmap() const
{
	return m_bUseLightmap;
}
float CvLandscapeInfo::getPeakScale() const
{
	return 	m_fPeakScale;
}

float CvLandscapeInfo::getHillScale() const
{
	return 	m_fHillScale;
}

bool CvLandscapeInfo::isRandomMap() const
{
	return m_bRandomMap;
}

//
// read from xml
//
bool CvLandscapeInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iWaterHeight, L"iWaterHeight");
	pXML->GetChildXmlValByName(&m_bRandomMap, L"bRandomMap");

	pXML->GetOptionalChildXmlValByName(m_szHeightMap, L"HeightMap");
	pXML->GetOptionalChildXmlValByName(m_szTerrainMap, L"TerrainMap");
	pXML->GetOptionalChildXmlValByName(m_szNormalMap, L"NormalMap");
	pXML->GetOptionalChildXmlValByName(m_szBlendMap, L"BlendMap");
	pXML->GetOptionalChildXmlValByName(m_szSkyArt, L"SkyArt");

	pXML->GetChildXmlValByName(&m_iFogR, L"iFogR");
	pXML->GetChildXmlValByName(&m_iFogG, L"iFogG");
	pXML->GetChildXmlValByName(&m_iFogB, L"iFogB");

	pXML->GetChildXmlValByName(&m_fTextureScaleX, L"fTextureScaleX");
	pXML->GetChildXmlValByName(&m_fTextureScaleY, L"fTextureScaleY");

	pXML->GetChildXmlValByName(&m_iHorizontalGameCell, L"iGameCellSizeX");
	pXML->GetChildXmlValByName(&m_iVerticalGameCell, L"iGameCellSizeY");

	pXML->GetChildXmlValByName(&m_iPlotsPerCellX, L"iPlotsPerCellX");
	pXML->GetChildXmlValByName(&m_iPlotsPerCellY, L"iPlotsPerCellY");

	m_iHorizontalVertCnt = m_iPlotsPerCellX * m_iHorizontalGameCell - (m_iPlotsPerCellX - 1);
	m_iVerticalVertCnt   = m_iPlotsPerCellY * m_iVerticalGameCell - (m_iPlotsPerCellY - 1);

	pXML->GetChildXmlValByName(&m_fZScale, L"fZScale");
	pXML->GetChildXmlValByName(&m_bUseTerrainShader, L"bTerrainShader");
	pXML->GetChildXmlValByName(&m_bUseLightmap, L"bUseLightmap");
	pXML->GetChildXmlValByName(&m_fPeakScale, L"fPeakScale");
	pXML->GetChildXmlValByName(&m_fHillScale, L"fHillScale");


	return true;
}

void CvLandscapeInfo::copyNonDefaults(const CvLandscapeInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getWaterHeight() == iDefault) m_iWaterHeight = pClassInfo->getWaterHeight();
	if (isRandomMap() == bDefault) m_bRandomMap = pClassInfo->isRandomMap();
	if (getHeightMap() == cDefault) m_szHeightMap = pClassInfo->getHeightMap();
	if (getTerrainMap() == cDefault) m_szTerrainMap = pClassInfo->getTerrainMap();
	if (getNormalMap() == cDefault) m_szNormalMap = pClassInfo->getNormalMap();
	if (getBlendMap() == cDefault) m_szBlendMap = pClassInfo->getBlendMap();
	if (getSkyArt() == cDefault) m_szSkyArt = pClassInfo->getSkyArt();
	if (getFogR() == iDefault) m_iFogR = pClassInfo->getFogR();
	if (getFogG() == iDefault) m_iFogG = pClassInfo->getFogG();
	if (getFogB() == iDefault) m_iFogB = pClassInfo->getFogB();
	if (getTextureScaleX() == fDefault) m_fTextureScaleX = pClassInfo->getTextureScaleX();
	if (getTextureScaleY() == fDefault) m_fTextureScaleY = pClassInfo->getTextureScaleY();
	if (getHorizontalGameCell() == iDefault) m_iHorizontalGameCell = pClassInfo->getHorizontalGameCell();
	if (getVerticalGameCell() == iDefault) m_iVerticalGameCell = pClassInfo->getVerticalGameCell();
	if (getPlotsPerCellX() == iDefault) m_iPlotsPerCellX = pClassInfo->getPlotsPerCellX();
	if (getPlotsPerCellY() == iDefault) m_iPlotsPerCellY = pClassInfo->getPlotsPerCellY();
	m_iHorizontalVertCnt = m_iPlotsPerCellX * m_iHorizontalGameCell - (m_iPlotsPerCellX - 1);
	m_iVerticalVertCnt   = m_iPlotsPerCellY * m_iVerticalGameCell - (m_iPlotsPerCellY - 1);

	if (getZScale() == fDefault) m_fZScale = pClassInfo->getZScale();
	if (isUseTerrainShader() == bDefault) m_bUseTerrainShader = pClassInfo->isUseTerrainShader();
	if (isUseLightmap() == bDefault) m_bUseLightmap = pClassInfo->isUseLightmap();
	if (getPeakScale() == fDefault) m_fPeakScale = pClassInfo->getPeakScale();
	if (getHillScale() == fDefault) m_fHillScale = pClassInfo->getHillScale();
}

//////////////////////////////////////////////////////////////////////////
// CvGameText
//////////////////////////////////////////////////////////////////////////

// static
int CvGameText::NUM_LANGUAGES = 7;
CvWString CvGameText::m_szLanguage = L"";

CvGameText::CvGameText() :
	m_szGender("N"),
	m_szPlural("false")
{
}

bool CvGameText::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	//pXML->TryMoveToXmlFirstChild(); // Move down to Child level
	pXML->GetChildXmlValByName(m_szType, L"Tag");
	//pXML->GetXmlVal(m_szType);		// TAG

	int iFallback = 0;

	CvWString wszTextVal;

	while (true)
	{
		switch (iFallback)
		{
			case 0:
				iFallback++;
				if (pXML->TryMoveToXmlFirstChild(m_szLanguage))
				{
					break;
				}

			case 1:
				// fallback to English
				iFallback++;
				if (pXML->TryMoveToXmlFirstChild(L"English"))
				{
					break;
				}

			case 2:
				// fallback to first tag after Tag
				iFallback++;
				if (!pXML->TryMoveToXmlFirstChild())
				{
					return false;
				}
				else if (!pXML->TryMoveToXmlNextSibling())
				{
					pXML->MoveToXmlParent();
					return false;
				}
				break;

			default:
				// no success, discard the text
				return false;
		}

		//CvString szTextVal;

		// we are now on the chosen language tag (or a fallback)
		if (!pXML->GetOptionalChildXmlValByName(wszTextVal, L"Text", L""))
		{
			pXML->GetXmlVal(wszTextVal);

		}

		// if there is content, we succeeded and break the loop
		if (!wszTextVal.empty())
		{
			m_szText = wszTextVal;
			break;
		}
		else
		{
			// next fallback, but undo the SetToChild first
			pXML->MoveToXmlParent();
		}
	}

	// GENDER
	if (pXML->GetOptionalChildXmlValByName(wszTextVal, L"Gender", L""))
	{
		m_szGender = wszTextVal;
	}

	// PLURAL
	if (pXML->GetOptionalChildXmlValByName(wszTextVal, L"Plural", L""))
	{
		m_szPlural = wszTextVal;
	}

	// old code
	/*int j=0;
	for (j = 0; j < iNumLanguages; j++)
	{
		pXML->SkipToNextVal();	// skip comments

		if (!pXML->TryMoveToXmlNextSibling(); || j == iMaxNumLanguages)
		{
			NUM_LANGUAGES = j;
			break;
		}
		if (j == GAMETEXT.getCurrentLanguage()) // Only add appropriate language Text
		{
			// TEXT
			if (pXML->GetChildXmlValByName(wszTextVal, L"Text"))
			{
				m_szText = wszTextVal;
			}
			else
			{
				pXML->GetXmlVal(wszTextVal);
				setText(wszTextVal);
				if (NUM_LANGUAGES > 0)
				{
					break;
				}
			}

			// GENDER
			if (pXML->GetChildXmlValByName(wszTextVal, L"Gender"))
			{
				m_szGender = wszTextVal;
			}

			// PLURAL
			if (pXML->GetChildXmlValByName(wszTextVal, L"Plural"))
			{
				m_szPlural = wszTextVal;
			}
			if (NUM_LANGUAGES > 0)
			{
				break;
			}
		}
	}*/

	pXML->MoveToXmlParent(); // Move back up to Parent

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	CvDiplomacyTextInfo
//
//

CvDiplomacyTextInfo::CvDiplomacyTextInfo() :
m_iNumResponses(0),
m_pResponses(NULL)
{
}

// note - Response member vars allocated by CvXmlLoadUtility
void CvDiplomacyTextInfo::init(int iNum)
{
	uninit();
	m_pResponses = new Response[iNum];
	m_iNumResponses=iNum;
}

void CvDiplomacyTextInfo::uninit()
{
	SAFE_DELETE_ARRAY(m_pResponses);
}

int CvDiplomacyTextInfo::getNumResponses() const
{
	return m_iNumResponses;
}

bool CvDiplomacyTextInfo::getCivilizationTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, GC.getNumCivilizationInfos(), j);
	return m_pResponses[i].m_pbCivilizationTypes ? m_pResponses[i].m_pbCivilizationTypes[j] : false;
}

bool CvDiplomacyTextInfo::getLeaderHeadTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, GC.getNumLeaderHeadInfos(), j);
	return m_pResponses[i].m_pbLeaderHeadTypes ? m_pResponses[i].m_pbLeaderHeadTypes[j] : false;
}

bool CvDiplomacyTextInfo::getAttitudeTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, NUM_ATTITUDE_TYPES, j);
	return m_pResponses[i].m_pbAttitudeTypes ? m_pResponses[i].m_pbAttitudeTypes[j] : false;
}

bool CvDiplomacyTextInfo::getDiplomacyPowerTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, NUM_DIPLOMACYPOWER_TYPES, j);
	return m_pResponses[i].m_pbDiplomacyPowerTypes ? m_pResponses[i].m_pbDiplomacyPowerTypes[j] : false;
}

int CvDiplomacyTextInfo::getNumDiplomacyText(int i) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	return m_pResponses[i].m_iNumDiplomacyText;
}

const char* CvDiplomacyTextInfo::getDiplomacyText(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, getNumDiplomacyText(i), j);
	return m_pResponses[i].m_paszDiplomacyText[j];
}

bool CvDiplomacyTextInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"Type");

	if ( pXML->TryMoveToXmlFirstChild(L"Responses") )
	{
		const int iIndexVal = pXML->GetXmlChildrenNumber(L"Response");
		init(iIndexVal);

		if (iIndexVal > 0 && pXML->TryMoveToXmlFirstChild())
		{
			for (int j = 0; j < iIndexVal; j++)
			{
				pXML->SetVariableListTagPair(&m_pResponses[j].m_pbCivilizationTypes, L"Civilizations", GC.getNumCivilizationInfos());
				pXML->SetVariableListTagPair(&m_pResponses[j].m_pbLeaderHeadTypes, L"Leaders", GC.getNumLeaderHeadInfos());
				pXML->SetVariableListTagPair(&m_pResponses[j].m_pbAttitudeTypes, L"Attitudes", NUM_ATTITUDE_TYPES);
				pXML->SetVariableListTagPair(&m_pResponses[j].m_pbDiplomacyPowerTypes, L"DiplomacyPowers", NUM_DIPLOMACYPOWER_TYPES);

				if (pXML->TryMoveToXmlFirstChild(L"DiplomacyText"))
				{
					pXML->SetStringList(&m_pResponses[j].m_paszDiplomacyText, &m_pResponses[j].m_iNumDiplomacyText);
					pXML->MoveToXmlParent();
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			pXML->MoveToXmlParent();
		}
	}
	pXML->MoveToXmlParent();

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//	CvEffectInfo			Misc\CIV4EffectInfos.xml
//
//

CvEffectInfo::CvEffectInfo() :
 m_fUpdateRate(0.0f)
,m_bProjectile(false)
,m_bSticky(false)
,m_fProjectileSpeed(0.0f)
,m_fProjectileArc(0.0f)
{ }

CvEffectInfo::~CvEffectInfo() {}

bool CvEffectInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvScalableInfo::read(pXML);

	pXML->GetChildXmlValByName(szTextVal, L"Path");
	setPath(szTextVal);

	pXML->GetChildXmlValByName(&m_fUpdateRate, L"fUpdateRate" );

	int iTemporary;
	pXML->GetOptionalChildXmlValByName(&iTemporary, L"bIsProjectile" );
	m_bProjectile = iTemporary != 0;

	pXML->GetOptionalChildXmlValByName(&m_fProjectileSpeed, L"fSpeed");
	pXML->GetOptionalChildXmlValByName(&m_fProjectileArc, L"fArcValue");
	pXML->GetOptionalChildXmlValByName(&m_bSticky, L"bSticky");

	return true;
}

void CvEffectInfo::copyNonDefaults(const CvEffectInfo* pClassInfo)
{
	const bool bDefault = false;
	const float fDefault = 0.0f;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);
	CvScalableInfo::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) setPath(pClassInfo->getPath());
	if (getUpdateRate() == fDefault) m_fUpdateRate = pClassInfo->getUpdateRate();

	if (isProjectile() == bDefault) m_bProjectile = pClassInfo->isProjectile();

	if (getProjectileSpeed() == fDefault) m_fProjectileSpeed = pClassInfo->getProjectileSpeed();
	if (getProjectileArc() == fDefault) m_fProjectileArc = pClassInfo->getProjectileArc();

	if (isSticky() == bDefault) m_bSticky = pClassInfo->isSticky();
}


//////////////////////////////////////////////////////////////////////////
//
//	CvAttachableInfo			Misc\CIV4AttachableInfos.xml
//
//
CvAttachableInfo::CvAttachableInfo() :
m_fUpdateRate(0.0f)
{
}

CvAttachableInfo::~CvAttachableInfo()
{
}

bool CvAttachableInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvScalableInfo::read(pXML);

	pXML->GetChildXmlValByName(szTextVal, L"Path");
	setPath(szTextVal);

	return true;
}

void CvAttachableInfo::copyNonDefaults(const CvAttachableInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);
	CvScalableInfo::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) setPath(pClassInfo->getPath());
}

//////////////////////////////////////////////////////////////////////////
//
//	CvGameOptionInfo
//	Game options and their default values
//
//
CvGameOptionInfo::CvGameOptionInfo() :
m_bDefault(false),
m_bVisible(true)
{
}

CvGameOptionInfo::~CvGameOptionInfo()
{
	GC.removeDelayedResolutionVector(m_aEnforcesGameOptionOnTypes);
	GC.removeDelayedResolutionVector(m_aEnforcesGameOptionOffTypes);
}

bool CvGameOptionInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(&m_bDefault, L"bDefault");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible", true);

	pXML->SetOptionalVectorWithDelayedResolution(m_aEnforcesGameOptionOnTypes, L"EnforcesGameOptionOnTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aEnforcesGameOptionOffTypes, L"EnforcesGameOptionOffTypes");

	return true;
}

void CvGameOptionInfo::copyNonDefaults(const CvGameOptionInfo* pClassInfo)
{
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
	if (getVisible()) m_bVisible = pClassInfo->getVisible();

	//TB's Tags
	GC.copyNonDefaultDelayedResolutionVector(m_aEnforcesGameOptionOnTypes, pClassInfo->m_aEnforcesGameOptionOnTypes);
	GC.copyNonDefaultDelayedResolutionVector(m_aEnforcesGameOptionOffTypes, pClassInfo->m_aEnforcesGameOptionOffTypes);
}

void CvGameOptionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bDefault);
	CheckSum(iSum, m_bVisible);

	//TB's Tags
	CheckSumC(iSum, m_aEnforcesGameOptionOnTypes);
	CheckSumC(iSum, m_aEnforcesGameOptionOffTypes);
}

//////////////////////////////////////////////////////////////////////////
//
//	CvMPOptionInfo
//	Multiplayer options and their default values
//
//
CvMPOptionInfo::CvMPOptionInfo() :
m_bDefault(false)
{
}

CvMPOptionInfo::~CvMPOptionInfo()
{
}

bool CvMPOptionInfo::getDefault() const
{
	return m_bDefault;
}

bool CvMPOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}

void CvMPOptionInfo::copyNonDefaults(const CvMPOptionInfo* pClassInfo)
{
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}

void CvMPOptionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bDefault);
}

//////////////////////////////////////////////////////////////////////////
//
//	CvForceControlInfo
//	Forced Controls and their default values
//
//
CvForceControlInfo::CvForceControlInfo() :
m_bDefault(false)
{
}

CvForceControlInfo::~CvForceControlInfo()
{
}

bool CvForceControlInfo::getDefault() const
{
	return m_bDefault;
}

bool CvForceControlInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}

void CvForceControlInfo::copyNonDefaults(const CvForceControlInfo* pClassInfo)
{
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}

void CvForceControlInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bDefault);
}

//////////////////////////////////////////////////////////////////////////
//
//	CvPlayerOptionInfo
//	Player options and their default values
//
//

//////////////////////////////////////////////////////////////////////////
//
//	CvGraphicOptionInfo
//	Graphic options and their default values
//
//
CvGraphicOptionInfo::CvGraphicOptionInfo() :
m_bDefault(false)
{
}

CvGraphicOptionInfo::~CvGraphicOptionInfo()
{
}

bool CvGraphicOptionInfo::getDefault() const
{
	return m_bDefault;
}

bool CvGraphicOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}

void CvGraphicOptionInfo::copyNonDefaults(const CvGraphicOptionInfo* pClassInfo)
{
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}

void CvGraphicOptionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bDefault);
}

//////////////////////////////////////////////////////////////////////////
//
//	CvEventTriggerInfo
//	Event triggers
//
//
CvEventTriggerInfo::CvEventTriggerInfo() :
	m_iPercentGamesActive(0),
	m_iProbability(0),
	m_iNumUnits(0),
	m_iNumBuildings(0),
	m_iNumUnitsGlobal(0),
	m_iNumBuildingsGlobal(0),
	m_iNumPlotsRequired(0),
	m_iPlotType(NO_PLOT),
	m_iNumReligions(0),
	m_iNumCorporations(0),
	m_iOtherPlayerShareBorders(0),
	m_iOtherPlayerHasTech(NO_TECH),
	m_iCivic(NO_CIVIC),
	m_iMinPopulation(0),
	m_iMaxPopulation(0),
	m_iMinMapLandmass(0),
	m_iMinOurLandmass(0),
	m_iMaxOurLandmass(-1),
	m_iMinDifficulty(NO_HANDICAP),
	m_iAngry(0),
	m_iUnhealthy(0),
	m_iUnitDamagedWeight(0),
	m_iUnitDistanceWeight(0),
	m_iUnitExperienceWeight(0),
	m_iMinTreasury(0),
	m_bSinglePlayer(false),
	m_bTeam(false),
	m_bRecurring(false),
	m_bGlobal(false),
	m_bPickPlayer(false),
	m_bOtherPlayerWar(false),
	m_bOtherPlayerHasReligion(false),
	m_bOtherPlayerHasOtherReligion(false),
	m_bOtherPlayerAI(false),
	m_bPickCity(false),
	m_bPickOtherPlayerCity(false),
	m_bShowPlot(true),
	m_iCityFoodWeight(0),
	m_bUnitsOnPlot(false),
	m_bOwnPlot(false),
	m_bPickReligion(false),
	m_bStateReligion(false),
	m_bHolyCity(false),
	m_bPickCorporation(false),
	m_bHeadquarters(false),
	m_bProbabilityUnitMultiply(false),
	m_bProbabilityBuildingMultiply(false),
	m_bPrereqEventCity(false)
{
}

CvEventTriggerInfo::~CvEventTriggerInfo()
{
}

int CvEventTriggerInfo::getPercentGamesActive() const
{
	return m_iPercentGamesActive;
}

int CvEventTriggerInfo::getProbability() const
{
	return m_iProbability;
}

int CvEventTriggerInfo::getUnitRequired(int i) const
{
	return m_aiUnitsRequired[i];
}

int CvEventTriggerInfo::getNumUnitsRequired() const
{
	return (int)m_aiUnitsRequired.size();
}

int CvEventTriggerInfo::getBuildingRequired(int i) const
{
	return m_aiBuildingsRequired[i];
}

int CvEventTriggerInfo::getNumBuildingsRequired() const
{
	return (int)m_aiBuildingsRequired.size();
}

int CvEventTriggerInfo::getNumUnits() const
{
	return m_iNumUnits;
}

int CvEventTriggerInfo::getNumBuildings() const
{
	return m_iNumBuildings;
}

int CvEventTriggerInfo::getNumUnitsGlobal() const
{
	return m_iNumUnitsGlobal;
}

int CvEventTriggerInfo::getNumBuildingsGlobal() const
{
	return m_iNumBuildingsGlobal;
}

int CvEventTriggerInfo::getNumPlotsRequired() const
{
	return m_iNumPlotsRequired;
}

int CvEventTriggerInfo::getPlotType() const
{
	return m_iPlotType;
}

int CvEventTriggerInfo::getNumReligions() const
{
	return m_iNumReligions;
}

int CvEventTriggerInfo::getNumCorporations() const
{
	return m_iNumCorporations;
}

int CvEventTriggerInfo::getOtherPlayerShareBorders() const
{
	return m_iOtherPlayerShareBorders;
}

int CvEventTriggerInfo::getOtherPlayerHasTech() const
{
	return m_iOtherPlayerHasTech;
}

int CvEventTriggerInfo::getCivic() const
{
	return m_iCivic;
}

int CvEventTriggerInfo::getMinPopulation() const
{
	return m_iMinPopulation;
}

int CvEventTriggerInfo::getMaxPopulation() const
{
	return m_iMaxPopulation;
}

int CvEventTriggerInfo::getMinMapLandmass() const
{
	return m_iMinMapLandmass;
}

int CvEventTriggerInfo::getMinOurLandmass() const
{
	return m_iMinOurLandmass;
}

int CvEventTriggerInfo::getMaxOurLandmass() const
{
	return m_iMaxOurLandmass;
}

int CvEventTriggerInfo::getMinDifficulty() const
{
	return m_iMinDifficulty;
}

int CvEventTriggerInfo::getAngry() const
{
	return m_iAngry;
}

int CvEventTriggerInfo::getUnhealthy() const
{
	return m_iUnhealthy;
}

int CvEventTriggerInfo::getUnitDamagedWeight() const
{
	return m_iUnitDamagedWeight;
}

int CvEventTriggerInfo::getUnitDistanceWeight() const
{
	return m_iUnitDistanceWeight;
}

int CvEventTriggerInfo::getUnitExperienceWeight() const
{
	return m_iUnitExperienceWeight;
}

int CvEventTriggerInfo::getMinTreasury() const
{
	return m_iMinTreasury;
}

int CvEventTriggerInfo::getEvent(int i) const
{
	return m_aiEvents[i];
}

int CvEventTriggerInfo::getNumEvents() const
{
	return (int)m_aiEvents.size();
}

int CvEventTriggerInfo::getPrereqEvent(int i) const
{
	return m_aiPrereqEvents[i];
}

int CvEventTriggerInfo::getNumPrereqEvents() const
{
	return (int)m_aiPrereqEvents.size();
}

int CvEventTriggerInfo::getPrereqOrTechs(int i) const
{
	return m_aiPrereqOrTechs[i];
}

int CvEventTriggerInfo::getNumPrereqOrTechs() const
{
	return (int)m_aiPrereqOrTechs.size();
}

int CvEventTriggerInfo::getPrereqAndTechs(int i) const
{
	return m_aiPrereqAndTechs[i];
}

int CvEventTriggerInfo::getNumPrereqAndTechs() const
{
	return (int)m_aiPrereqAndTechs.size();
}

int CvEventTriggerInfo::getObsoleteTech(int i) const
{
	return m_aiObsoleteTechs[i];
}

int CvEventTriggerInfo::getNumObsoleteTechs() const
{
	return (int)m_aiObsoleteTechs.size();
}

int CvEventTriggerInfo::getFeatureRequired(int i) const
{
	return m_aiFeaturesRequired[i];
}

int CvEventTriggerInfo::getNumFeaturesRequired() const
{
	return (int)m_aiFeaturesRequired.size();
}

int CvEventTriggerInfo::getTerrainRequired(int i) const
{
	return m_aiTerrainsRequired[i];
}

int CvEventTriggerInfo::getNumTerrainsRequired() const
{
	return (int)m_aiTerrainsRequired.size();
}

int CvEventTriggerInfo::getImprovementRequired(int i) const
{
	return m_aiImprovementsRequired[i];
}

int CvEventTriggerInfo::getNumImprovementsRequired() const
{
	return (int)m_aiImprovementsRequired.size();
}

int CvEventTriggerInfo::getBonusRequired(int i) const
{
	return m_aiBonusesRequired[i];
}

int CvEventTriggerInfo::getNumBonusesRequired() const
{
	return (int)m_aiBonusesRequired.size();
}

int CvEventTriggerInfo::getRouteRequired(int i) const
{
	return m_aiRoutesRequired[i];
}

int CvEventTriggerInfo::getNumRoutesRequired() const
{
	return (int)m_aiRoutesRequired.size();
}

int CvEventTriggerInfo::getReligionRequired(int i) const
{
	return m_aiReligionsRequired[i];
}

int CvEventTriggerInfo::getNumReligionsRequired() const
{
	return (int)m_aiReligionsRequired.size();
}

int CvEventTriggerInfo::getCorporationRequired(int i) const
{
	return m_aiCorporationsRequired[i];
}

int CvEventTriggerInfo::getNumCorporationsRequired() const
{
	return (int)m_aiCorporationsRequired.size();
}

// Begin EmperorFool: Events with Images
const char* CvEventTriggerInfo::getEventArt() const
{
	if (m_szEventArt.empty())
	{
		return NULL;
	}

	return m_szEventArt;
}
// End EmperorFool: Events with Images

bool CvEventTriggerInfo::isSinglePlayer() const
{
	return m_bSinglePlayer;
}

bool CvEventTriggerInfo::isTeam() const
{
	return m_bTeam;
}

const CvWString& CvEventTriggerInfo::getText(int i) const
{
	FASSERT_BOUNDS(0, (int)m_aszText.size(), i);
	return m_aszText[i];
}

int CvEventTriggerInfo::getTextEra(int i) const
{
	FASSERT_BOUNDS(0, (int)m_aiTextEra.size(), i);
	return m_aiTextEra[i];
}

int CvEventTriggerInfo::getNumTexts() const
{
	FAssert(m_aiTextEra.size() == m_aszText.size());
	return m_aszText.size();
}

const CvWString& CvEventTriggerInfo::getWorldNews(int i) const
{
	FASSERT_BOUNDS(0, getNumWorldNews(), i);
	return m_aszWorldNews[i];
}

int CvEventTriggerInfo::getNumWorldNews() const
{
	return m_aszWorldNews.size();
}

bool CvEventTriggerInfo::isRecurring() const
{
	return m_bRecurring;
}

bool CvEventTriggerInfo::isGlobal() const
{
	return m_bGlobal;
}

bool CvEventTriggerInfo::isPickPlayer() const
{
	return m_bPickPlayer;
}

bool CvEventTriggerInfo::isOtherPlayerWar() const
{
	return m_bOtherPlayerWar;
}

bool CvEventTriggerInfo::isOtherPlayerHasReligion() const
{
	return m_bOtherPlayerHasReligion;
}

bool CvEventTriggerInfo::isOtherPlayerHasOtherReligion() const
{
	return m_bOtherPlayerHasOtherReligion;
}

bool CvEventTriggerInfo::isOtherPlayerAI() const
{
	return m_bOtherPlayerAI;
}

bool CvEventTriggerInfo::isPickCity() const
{
	return m_bPickCity;
}

bool CvEventTriggerInfo::isPickOtherPlayerCity() const
{
	return m_bPickOtherPlayerCity;
}

bool CvEventTriggerInfo::isShowPlot() const
{
	return m_bShowPlot;
}

int CvEventTriggerInfo::getCityFoodWeight() const
{
	return m_iCityFoodWeight;
}

bool CvEventTriggerInfo::isUnitsOnPlot() const
{
	return m_bUnitsOnPlot;
}

bool CvEventTriggerInfo::isOwnPlot() const
{
	return m_bOwnPlot;
}

bool CvEventTriggerInfo::isPickReligion() const
{
	return m_bPickReligion;
}

bool CvEventTriggerInfo::isStateReligion() const
{
	return m_bStateReligion;
}

bool CvEventTriggerInfo::isHolyCity() const
{
	return m_bHolyCity;
}

bool CvEventTriggerInfo::isPickCorporation() const
{
	return m_bPickCorporation;
}

bool CvEventTriggerInfo::isHeadquarters() const
{
	return m_bHeadquarters;
}

bool CvEventTriggerInfo::isProbabilityUnitMultiply() const
{
	return m_bProbabilityUnitMultiply;
}

bool CvEventTriggerInfo::isProbabilityBuildingMultiply() const
{
	return m_bProbabilityBuildingMultiply;
}

bool CvEventTriggerInfo::isPrereqEventCity() const
{
	return m_bPrereqEventCity;
}

const CvProperties* CvEventTriggerInfo::getPrereqMinProperties() const
{
	return &m_PrereqMinProperties;
}

const CvProperties* CvEventTriggerInfo::getPrereqMaxProperties() const
{
	return &m_PrereqMaxProperties;
}

const CvProperties* CvEventTriggerInfo::getPrereqPlayerMinProperties() const
{
	return &m_PrereqPlayerMinProperties;
}

const CvProperties* CvEventTriggerInfo::getPrereqPlayerMaxProperties() const
{
	return &m_PrereqPlayerMaxProperties;
}

const char* CvEventTriggerInfo::getPythonCallback() const
{
	return m_szPythonCallback;
}

const char* CvEventTriggerInfo::getPythonCanDo() const
{
	return m_szPythonCanDo;
}

const char* CvEventTriggerInfo::getPythonCanDoCity() const
{
	return m_szPythonCanDoCity;
}

const char* CvEventTriggerInfo::getPythonCanDoUnit() const
{
	return m_szPythonCanDoUnit;
}

// bool vector without delayed resolution
int CvEventTriggerInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvEventTriggerInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvEventTriggerInfo::isNotOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiNotOnGameOptions, i);
}

int CvEventTriggerInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvEventTriggerInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvEventTriggerInfo::isOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiOnGameOptions, i);
}

void CvEventTriggerInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iPercentGamesActive);
	CheckSum(iSum, m_iProbability);
	CheckSum(iSum, m_iNumUnits);
	CheckSum(iSum, m_iNumBuildings);
	CheckSum(iSum, m_iNumUnitsGlobal);
	CheckSum(iSum, m_iNumBuildingsGlobal);
	CheckSum(iSum, m_iNumPlotsRequired);
	CheckSum(iSum, m_iPlotType);
	CheckSum(iSum, m_iNumReligions);
	CheckSum(iSum, m_iNumCorporations);
	CheckSum(iSum, m_iOtherPlayerShareBorders);
	CheckSum(iSum, m_iOtherPlayerHasTech);
	CheckSum(iSum, m_iCivic);
	CheckSum(iSum, m_iMinPopulation);
	CheckSum(iSum, m_iMaxPopulation);
	CheckSum(iSum, m_iMinMapLandmass);
	CheckSum(iSum, m_iMinOurLandmass);
	CheckSum(iSum, m_iMaxOurLandmass);
	CheckSum(iSum, m_iMinDifficulty);
	CheckSum(iSum, m_iAngry);
	CheckSum(iSum, m_iUnhealthy);
	CheckSum(iSum, m_iUnitDamagedWeight);
	CheckSum(iSum, m_iUnitDistanceWeight);
	CheckSum(iSum, m_iUnitExperienceWeight);
	CheckSum(iSum, m_iMinTreasury);

	CheckSumC(iSum, m_aiUnitsRequired);
	CheckSumC(iSum, m_aiBuildingsRequired);
	CheckSumC(iSum, m_aiPrereqOrTechs);
	CheckSumC(iSum, m_aiPrereqAndTechs);
	CheckSumC(iSum, m_aiObsoleteTechs);
	CheckSumC(iSum, m_aiEvents);
	CheckSumC(iSum, m_aiPrereqEvents);
	CheckSumC(iSum, m_aiFeaturesRequired);
	CheckSumC(iSum, m_aiTerrainsRequired);
	CheckSumC(iSum, m_aiImprovementsRequired);
	CheckSumC(iSum, m_aiBonusesRequired);
	CheckSumC(iSum, m_aiRoutesRequired);
	CheckSumC(iSum, m_aiReligionsRequired);
	CheckSumC(iSum, m_aiCorporationsRequired);

	CheckSum(iSum, m_bSinglePlayer);
	CheckSum(iSum, m_bTeam);
	CheckSum(iSum, m_bRecurring);
	CheckSum(iSum, m_bGlobal);
	CheckSum(iSum, m_bPickPlayer);
	CheckSum(iSum, m_bOtherPlayerWar);
	CheckSum(iSum, m_bOtherPlayerHasReligion);
	CheckSum(iSum, m_bOtherPlayerHasOtherReligion);
	CheckSum(iSum, m_bOtherPlayerAI);
	CheckSum(iSum, m_bPickCity);
	CheckSum(iSum, m_bPickOtherPlayerCity);
	CheckSum(iSum, m_bShowPlot);
	CheckSum(iSum, m_iCityFoodWeight);
	CheckSum(iSum, m_bUnitsOnPlot);
	CheckSum(iSum, m_bOwnPlot);
	CheckSum(iSum, m_bPickReligion);
	CheckSum(iSum, m_bStateReligion);
	CheckSum(iSum, m_bHolyCity);
	CheckSum(iSum, m_bPickCorporation);
	CheckSum(iSum, m_bHeadquarters);
	CheckSum(iSum, m_bProbabilityUnitMultiply);
	CheckSum(iSum, m_bProbabilityBuildingMultiply);
	CheckSum(iSum, m_bPrereqEventCity);

	m_PrereqMinProperties.getCheckSum(iSum);
	m_PrereqMaxProperties.getCheckSum(iSum);
	m_PrereqPlayerMinProperties.getCheckSum(iSum);
	m_PrereqPlayerMaxProperties.getCheckSum(iSum);

	CheckSumC(iSum, m_szPythonCallback);
	CheckSumC(iSum, m_szPythonCanDo);
	CheckSumC(iSum, m_szPythonCanDoCity);
	CheckSumC(iSum, m_szPythonCanDoUnit);

	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiOnGameOptions);
}

bool CvEventTriggerInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iPercentGamesActive, L"iPercentGamesActive");
	pXML->GetOptionalChildXmlValByName(&m_iProbability, L"iWeight");
	pXML->GetOptionalChildXmlValByName(&m_iNumUnits, L"iNumUnits");
	pXML->GetOptionalChildXmlValByName(&m_iNumBuildings, L"iNumBuildings");
	pXML->GetOptionalChildXmlValByName(&m_iNumUnitsGlobal, L"iNumUnitsGlobal");
	pXML->GetOptionalChildXmlValByName(&m_iNumBuildingsGlobal, L"iNumBuildingsGlobal");
	pXML->GetOptionalChildXmlValByName(&m_iNumPlotsRequired, L"iNumPlotsRequired");
	pXML->GetOptionalChildXmlValByName(&m_iPlotType, L"iPlotType", NO_PLOT);
	pXML->GetOptionalChildXmlValByName(&m_iNumReligions, L"iNumReligions");
	pXML->GetOptionalChildXmlValByName(&m_iNumCorporations, L"iNumCorporations");
	pXML->GetOptionalChildXmlValByName(&m_iOtherPlayerShareBorders, L"iOtherPlayerShareBorders");
	pXML->GetOptionalChildXmlValByName(&m_iMinPopulation, L"iMinPopulation");
	pXML->GetOptionalChildXmlValByName(&m_iMaxPopulation, L"iMaxPopulation");
	pXML->GetOptionalChildXmlValByName(&m_iMinMapLandmass, L"iMinMapLandmass");
	pXML->GetOptionalChildXmlValByName(&m_iMinOurLandmass, L"iMinOurLandmass");
	pXML->GetOptionalChildXmlValByName(&m_iMaxOurLandmass, L"iMaxOurLandmass", -1);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MinDifficulty");
	m_iMinDifficulty = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iAngry, L"iAngry");
	pXML->GetOptionalChildXmlValByName(&m_iUnhealthy, L"iUnhealthy");
	pXML->GetOptionalChildXmlValByName(&m_iUnitDamagedWeight, L"iUnitDamagedWeight");
	pXML->GetOptionalChildXmlValByName(&m_iUnitDistanceWeight, L"iUnitDistanceWeight");
	pXML->GetOptionalChildXmlValByName(&m_iUnitExperienceWeight, L"iUnitExperienceWeight");
	pXML->GetOptionalChildXmlValByName(&m_iMinTreasury, L"iMinTreasury");
	pXML->SetOptionalVector(&m_aiUnitsRequired, L"UnitsRequired");
	pXML->SetOptionalVector(&m_aiBuildingsRequired, L"BuildingsRequired");
	pXML->SetOptionalVector(&m_aiPrereqOrTechs, L"OrPreReqs");
	pXML->SetOptionalVector(&m_aiPrereqAndTechs, L"AndPreReqs");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"OtherPlayerHasTech");
	m_iOtherPlayerHasTech = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Civic");
	m_iCivic = pXML->GetInfoClass(szTextVal);

	pXML->SetOptionalVector(&m_aiObsoleteTechs, L"ObsoleteTechs");
	pXML->SetOptionalVector(&m_aiEvents, L"Events");
	pXML->SetOptionalVector(&m_aiPrereqEvents, L"PrereqEvents");
	pXML->SetOptionalVector(&m_aiFeaturesRequired, L"FeaturesRequired");
	pXML->SetOptionalVector(&m_aiTerrainsRequired, L"TerrainsRequired");
	pXML->SetOptionalVector(&m_aiImprovementsRequired, L"ImprovementsRequired");
	pXML->SetOptionalVector(&m_aiBonusesRequired, L"BonusesRequired");
	pXML->SetOptionalVector(&m_aiRoutesRequired, L"RoutesRequired");
	pXML->SetOptionalVector(&m_aiReligionsRequired, L"ReligionsRequired");
	pXML->SetOptionalVector(&m_aiCorporationsRequired, L"CorporationsRequired");

	m_aszText.clear();
	m_aiTextEra.clear();
	if (pXML->TryMoveToXmlFirstChild(L"TriggerTexts"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszText.push_back(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						m_aiTextEra.push_back(pXML->GetInfoClass(szTextVal));

						pXML->MoveToXmlParent();

					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}

				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	m_aszWorldNews.clear();
	if (pXML->TryMoveToXmlFirstChild(L"WorldNewsTexts"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->GetChildXmlVal(szTextVal))
			{
				for (int j=0; j<iNumSibs; ++j)
				{
					m_aszWorldNews.push_back(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

// Begin EmperorFool: Events with Images
	pXML->GetOptionalChildXmlValByName(m_szEventArt, L"EventArt");
// End EmperorFool: Events with Images
	pXML->GetOptionalChildXmlValByName(&m_bSinglePlayer, L"bSinglePlayer");
	pXML->GetOptionalChildXmlValByName(&m_bTeam, L"bTeam");
	pXML->GetOptionalChildXmlValByName(&m_bRecurring, L"bRecurring");
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	pXML->GetOptionalChildXmlValByName(&m_bPickPlayer, L"bPickPlayer");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerWar, L"bOtherPlayerWar");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerHasReligion, L"bOtherPlayerHasReligion");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerHasOtherReligion, L"bOtherPlayerHasOtherReligion");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerAI, L"bOtherPlayerAI");
	pXML->GetOptionalChildXmlValByName(&m_bPickCity, L"bPickCity");
	pXML->GetOptionalChildXmlValByName(&m_bPickOtherPlayerCity, L"bPickOtherPlayerCity");
	pXML->GetOptionalChildXmlValByName(&m_bShowPlot, L"bShowPlot", true);
	pXML->GetOptionalChildXmlValByName(&m_iCityFoodWeight, L"iCityFoodWeight");
	pXML->GetOptionalChildXmlValByName(&m_bUnitsOnPlot, L"bUnitsOnPlot");
	pXML->GetOptionalChildXmlValByName(&m_bOwnPlot, L"bOwnPlot");
	pXML->GetOptionalChildXmlValByName(&m_bPickReligion, L"bPickReligion");
	pXML->GetOptionalChildXmlValByName(&m_bStateReligion, L"bStateReligion");
	pXML->GetOptionalChildXmlValByName(&m_bHolyCity, L"bHolyCity");
	pXML->GetOptionalChildXmlValByName(&m_bPickCorporation, L"bPickCorporation");
	pXML->GetOptionalChildXmlValByName(&m_bHeadquarters, L"bHeadquarters");
	pXML->GetOptionalChildXmlValByName(&m_bProbabilityUnitMultiply, L"bProbabilityUnitMultiply");
	pXML->GetOptionalChildXmlValByName(&m_bProbabilityBuildingMultiply, L"bProbabilityBuildingMultiply");
	pXML->GetOptionalChildXmlValByName(&m_bPrereqEventCity, L"bPrereqEventPlot");

	m_PrereqMinProperties.read(pXML, L"PrereqMinProperties");
	m_PrereqMaxProperties.read(pXML, L"PrereqMaxProperties");
	m_PrereqPlayerMinProperties.read(pXML, L"PrereqPlayerMinProperties");
	m_PrereqPlayerMaxProperties.read(pXML, L"PrereqPlayerMaxProperties");

	pXML->GetOptionalChildXmlValByName(m_szPythonCallback, L"PythonCallback");
	pXML->GetOptionalChildXmlValByName(m_szPythonCanDo, L"PythonCanDo");
	pXML->GetOptionalChildXmlValByName(m_szPythonCanDoCity, L"PythonCanDoCity");
	pXML->GetOptionalChildXmlValByName(m_szPythonCanDoUnit, L"PythonCanDoUnit");

	// bool vector without delayed resolution
	pXML->SetOptionalVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	return true;
}

void CvEventTriggerInfo::copyNonDefaults(const CvEventTriggerInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPercentGamesActive() == iDefault) m_iPercentGamesActive = pClassInfo->getPercentGamesActive();
	if (getProbability() == iDefault) m_iProbability = pClassInfo->getProbability();
	if (getNumUnits() == iDefault) m_iNumUnits = pClassInfo->getNumUnits();
	if (getNumBuildings() == iDefault) m_iNumBuildings = pClassInfo->getNumBuildings();
	if (getNumUnitsGlobal() == iDefault) m_iNumUnitsGlobal = pClassInfo->getNumUnitsGlobal();
	if (getNumBuildingsGlobal() == iDefault) m_iNumBuildingsGlobal = pClassInfo->getNumBuildingsGlobal();
	if (getNumPlotsRequired() == iDefault) m_iNumPlotsRequired = pClassInfo->getNumPlotsRequired();
	if (getPlotType() == NO_PLOT) m_iPlotType = pClassInfo->getPlotType();
	if (getNumReligions() == iDefault) m_iNumReligions = pClassInfo->getNumReligions();
	if (getNumCorporations() == iDefault) m_iNumCorporations = pClassInfo->getNumCorporations();
	if (getOtherPlayerShareBorders() == iDefault) m_iOtherPlayerShareBorders = pClassInfo->getOtherPlayerShareBorders();
	if (getMinPopulation() == iDefault) m_iMinPopulation = pClassInfo->getMinPopulation();
	if (getMaxPopulation() == iDefault) m_iMaxPopulation = pClassInfo->getMaxPopulation();
	if (getMinMapLandmass() == iDefault) m_iMinMapLandmass = pClassInfo->getMinMapLandmass();
	if (getMinOurLandmass() == iDefault) m_iMinOurLandmass = pClassInfo->getMinOurLandmass();
	if (getMaxOurLandmass() == -1) m_iMaxOurLandmass = pClassInfo->getMaxOurLandmass();
	if (getMinDifficulty() == iTextDefault) m_iMinDifficulty = pClassInfo->getMinDifficulty();
	if (getAngry() == iDefault) m_iAngry = pClassInfo->getAngry();
	if (getUnhealthy() == iDefault) m_iUnhealthy = pClassInfo->getUnhealthy();
	if (getUnitDamagedWeight() == iDefault) m_iUnitDamagedWeight = pClassInfo->getUnitDamagedWeight();
	if (getUnitDistanceWeight() == iDefault) m_iUnitDistanceWeight = pClassInfo->getUnitDistanceWeight();
	if (getUnitExperienceWeight() == iDefault) m_iUnitExperienceWeight = pClassInfo->getUnitExperienceWeight();
	if (getMinTreasury() == iDefault) m_iMinTreasury = pClassInfo->getMinTreasury();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiUnitsRequired, pClassInfo->m_aiUnitsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiBuildingsRequired, pClassInfo->m_aiBuildingsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqOrTechs, pClassInfo->m_aiPrereqOrTechs);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqAndTechs, pClassInfo->m_aiPrereqAndTechs);

	if (getOtherPlayerHasTech() == iTextDefault) m_iOtherPlayerHasTech = pClassInfo->getOtherPlayerHasTech();
	if (getCivic() == iTextDefault) m_iCivic = pClassInfo->getCivic();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiObsoleteTechs, pClassInfo->m_aiObsoleteTechs);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiEvents, pClassInfo->m_aiEvents);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqEvents, pClassInfo->m_aiPrereqEvents);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiFeaturesRequired, pClassInfo->m_aiFeaturesRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTerrainsRequired, pClassInfo->m_aiTerrainsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiImprovementsRequired, pClassInfo->m_aiImprovementsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiBonusesRequired, pClassInfo->m_aiBonusesRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiRoutesRequired, pClassInfo->m_aiRoutesRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiReligionsRequired, pClassInfo->m_aiReligionsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCorporationsRequired, pClassInfo->m_aiCorporationsRequired);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTextEra, pClassInfo->m_aiTextEra);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aszText, pClassInfo->m_aszText);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aszWorldNews, pClassInfo->m_aszWorldNews);

	if (isSinglePlayer() == bDefault) m_bSinglePlayer = pClassInfo->isSinglePlayer();
	if (isTeam() == bDefault) m_bTeam = pClassInfo->isTeam();
	if (isRecurring() == bDefault) m_bRecurring = pClassInfo->isRecurring();
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();
	if (isPickPlayer() == bDefault) m_bPickPlayer = pClassInfo->isPickPlayer();
	if (isOtherPlayerWar() == bDefault) m_bOtherPlayerWar = pClassInfo->isOtherPlayerWar();
	if (isOtherPlayerHasReligion() == bDefault) m_bOtherPlayerHasReligion = pClassInfo->isOtherPlayerHasReligion();
	if (isOtherPlayerHasOtherReligion() == bDefault) m_bOtherPlayerHasOtherReligion = pClassInfo->isOtherPlayerHasOtherReligion();
	if (isOtherPlayerAI() == bDefault) m_bOtherPlayerAI = pClassInfo->isOtherPlayerAI();
	if (isPickCity() == bDefault) m_bPickCity = pClassInfo->isPickCity();
	if (isPickOtherPlayerCity() == bDefault) m_bPickOtherPlayerCity = pClassInfo->isPickOtherPlayerCity();
	if (isShowPlot()) m_bShowPlot = pClassInfo->isShowPlot();
	if (getCityFoodWeight() == iDefault) m_iCityFoodWeight = pClassInfo->getCityFoodWeight();
	if (isUnitsOnPlot() == bDefault) m_bUnitsOnPlot = pClassInfo->isUnitsOnPlot();
	if (isOwnPlot() == bDefault) m_bOwnPlot = pClassInfo->isOwnPlot();
	if (isPickReligion() == bDefault) m_bPickReligion = pClassInfo->isPickReligion();
	if (isStateReligion() == bDefault) m_bStateReligion = pClassInfo->isStateReligion();
	if (isHolyCity() == bDefault) m_bHolyCity = pClassInfo->isHolyCity();
	if (isPickCorporation() == bDefault) m_bPickCorporation = pClassInfo->isPickCorporation();
	if (isHeadquarters() == bDefault) m_bHeadquarters = pClassInfo->isHeadquarters();
	if (isProbabilityUnitMultiply() == bDefault) m_bProbabilityUnitMultiply = pClassInfo->isProbabilityUnitMultiply();
	if (isProbabilityBuildingMultiply() == bDefault) m_bProbabilityBuildingMultiply = pClassInfo->isProbabilityBuildingMultiply();
	if (isPrereqEventCity() == bDefault) m_bPrereqEventCity = pClassInfo->isPrereqEventCity();

	m_PrereqMinProperties.copyNonDefaults(pClassInfo->getPrereqMinProperties());
	m_PrereqMaxProperties.copyNonDefaults(pClassInfo->getPrereqMaxProperties());
	m_PrereqPlayerMinProperties.copyNonDefaults(pClassInfo->getPrereqPlayerMinProperties());
	m_PrereqPlayerMaxProperties.copyNonDefaults(pClassInfo->getPrereqPlayerMaxProperties());

	if (getPythonCallback() == cDefault) m_szPythonCallback = pClassInfo->getPythonCallback();
	if (getPythonCanDo() == cDefault) m_szPythonCanDo = pClassInfo->getPythonCanDo();
	if (getPythonCanDoCity() == cDefault) m_szPythonCanDoCity = pClassInfo->getPythonCanDoCity();
	if (getPythonCanDoUnit() == cDefault) m_szPythonCanDoUnit = pClassInfo->getPythonCanDoUnit();
	// bool vector without delayed resolution
	if (getNumNotOnGameOptions() == 0)
	{
		m_aiNotOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnGameOptions(); i++)
		{
			m_aiNotOnGameOptions.push_back(pClassInfo->getNotOnGameOption(i));
		}
	}
	if (getNumOnGameOptions() == 0)
	{
		m_aiOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumOnGameOptions(); i++)
		{
			m_aiOnGameOptions.push_back(pClassInfo->getOnGameOption(i));
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
//	CvEventInfo
//	Events
//
//
CvEventInfo::CvEventInfo() :
	m_bQuest(false),
	m_bGlobal(false),
	m_bTeam(false),
	m_bCityEffect(false),
	m_bOtherPlayerCityEffect(false),
	m_bGoldToPlayer(false),
	m_bGoldenAge(false),
	m_bDeclareWar(false),
	m_bDisbandUnit(false),
	m_bGameSpeedScale(false),
	m_iGold(0),
	m_iRandomGold(0),
	m_iCulture(0),
	m_iEspionagePoints(0),
	m_iTech(NO_TECH),
	m_iTechPercent(0),
	m_iTechCostPercent(0),
	m_iTechMinTurnsLeft(0),
	m_iPrereqTech(NO_TECH),
	m_iFreeUnit(NO_UNIT),
	m_iNumUnits(0),
	m_iUnitExperience(0),
	m_iUnitImmobileTurns(0),
	m_iBuilding(NO_BUILDING),
	m_iBuildingChange(0),
	m_iHappy(0),
	m_iHealth(0),
	m_iHurryAnger(0),
	m_iHappyTurns(0),
	m_iFood(0),
	m_iFoodPercent(0),
	m_iFeature(NO_FEATURE),
	m_iFeatureChange(0),
	m_iImprovement(NO_IMPROVEMENT),
	m_iImprovementChange(0),
	m_iBonus(NO_BONUS),
	m_iBonusChange(0),
	m_iRoute(NO_ROUTE),
	m_iRouteChange(0),
	m_iBonusRevealed(NO_BONUS),
	m_iBonusGift(NO_BONUS),
	m_iConvertOwnCities(0),
	m_iConvertOtherCities(0),
	m_iMaxNumReligions(-1),
	m_iOurAttitudeModifier(0),
	m_iAttitudeModifier(0),
	m_iTheirEnemyAttitudeModifier(0),
	m_iPopulationChange(0),
	m_iRevoltTurns(0),
	m_iMinPillage(0),
	m_iMaxPillage(0),
	m_iFreeUnitSupport(0),
	m_iInflationModifier(0),
	m_iSpaceProductionModifier(0),
	m_iAIValue(0),
	m_piTechFlavorValue(NULL),
	m_piPlotExtraYields(NULL),
	m_piFreeSpecialistCount(NULL),
	m_piAdditionalEventChance(NULL),
	m_piAdditionalEventTime(NULL),
	m_piClearEventChance(NULL),
	m_piUnitCombatPromotions(NULL),
	m_piUnitPromotions(NULL),
	m_piCommerceModifier(NULL),
	m_piYieldModifier(NULL),
	m_iPrereqGameOption(NO_GAMEOPTION),
	m_iRevolutionIndexChange(0)
{ }

CvEventInfo::~CvEventInfo()
{
	SAFE_DELETE_ARRAY(m_piTechFlavorValue);
	SAFE_DELETE_ARRAY(m_piPlotExtraYields);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piAdditionalEventChance);
	SAFE_DELETE_ARRAY(m_piAdditionalEventTime);
	SAFE_DELETE_ARRAY(m_piClearEventChance);
	SAFE_DELETE_ARRAY(m_piUnitCombatPromotions);
	SAFE_DELETE_ARRAY(m_piUnitPromotions);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
}

bool CvEventInfo::isGlobal() const
{
	return m_bGlobal;
}

bool CvEventInfo::isQuest() const
{
	return m_bQuest;
}

bool CvEventInfo::isTeam() const
{
	return m_bTeam;
}

bool CvEventInfo::isCityEffect() const
{
	return m_bCityEffect;
}

bool CvEventInfo::isOtherPlayerCityEffect() const
{
	return m_bOtherPlayerCityEffect;
}

bool CvEventInfo::isGoldToPlayer() const
{
	return m_bGoldToPlayer;
}

bool CvEventInfo::isGoldenAge() const
{
	return m_bGoldenAge;
}

bool CvEventInfo::isDeclareWar() const
{
	return m_bDeclareWar;
}

bool CvEventInfo::isDisbandUnit() const
{
	return m_bDisbandUnit;
}

int CvEventInfo::getGold() const
{
	return m_iGold;
}

int CvEventInfo::getRandomGold() const
{
	return m_iRandomGold;
}

int CvEventInfo::getCulture() const
{
	return m_iCulture;
}

int CvEventInfo::getEspionagePoints() const
{
	return m_iEspionagePoints;
}

int CvEventInfo::getTech() const
{
	return m_iTech;
}

int CvEventInfo::getTechPercent() const
{
	return m_iTechPercent;
}

int CvEventInfo::getTechCostPercent() const
{
	return m_iTechCostPercent;
}

int CvEventInfo::getTechMinTurnsLeft() const
{
	return m_iTechMinTurnsLeft;
}

int CvEventInfo::getFreeUnit() const
{
	return m_iFreeUnit;
}

int CvEventInfo::getNumUnits() const
{
	return m_iNumUnits;
}

int CvEventInfo::getUnitExperience() const
{
	return m_iUnitExperience;
}

int CvEventInfo::getUnitImmobileTurns() const
{
	return m_iUnitImmobileTurns;
}

int CvEventInfo::getBuilding() const
{
	return m_iBuilding;
}

int CvEventInfo::getBuildingChange() const
{
	return m_iBuildingChange;
}

int CvEventInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int CvEventInfo::getYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldModifier ? m_piYieldModifier[i] : 0;
}

int CvEventInfo::getHappy() const
{
	return m_iHappy;
}

int CvEventInfo::getHealth() const
{
	return m_iHealth;
}

int CvEventInfo::getHurryAnger() const
{
	return m_iHurryAnger;
}

int CvEventInfo::getHappyTurns() const
{
	return m_iHappyTurns;
}

int CvEventInfo::getFood() const
{
	return m_iFood;
}

int CvEventInfo::getFoodPercent() const
{
	return m_iFoodPercent;
}

int CvEventInfo::getFeature() const
{
	return m_iFeature;
}

int CvEventInfo::getFeatureChange() const
{
	return m_iFeatureChange;
}

int CvEventInfo::getImprovementChange() const
{
	return m_iImprovementChange;
}

int CvEventInfo::getBonus() const
{
	return m_iBonus;
}

int CvEventInfo::getBonusChange() const
{
	return m_iBonusChange;
}

int CvEventInfo::getRoute() const
{
	return m_iRoute;
}

int CvEventInfo::getRouteChange() const
{
	return m_iRouteChange;
}

int CvEventInfo::getBonusRevealed() const
{
	return m_iBonusRevealed;
}

int CvEventInfo::getBonusGift() const
{
	return m_iBonusGift;
}

int CvEventInfo::getConvertOwnCities() const
{
	return m_iConvertOwnCities;
}

int CvEventInfo::getConvertOtherCities() const
{
	return m_iConvertOtherCities;
}

int CvEventInfo::getMaxNumReligions() const
{
	return m_iMaxNumReligions;
}

int CvEventInfo::getOurAttitudeModifier() const
{
	return m_iOurAttitudeModifier;
}

int CvEventInfo::getAttitudeModifier() const
{
	return m_iAttitudeModifier;
}

int CvEventInfo::getTheirEnemyAttitudeModifier() const
{
	return m_iTheirEnemyAttitudeModifier;
}

int CvEventInfo::getPopulationChange() const
{
	return m_iPopulationChange;
}

int CvEventInfo::getRevoltTurns() const
{
	return m_iRevoltTurns;
}

int CvEventInfo::getMinPillage() const
{
	return m_iMinPillage;
}

int CvEventInfo::getMaxPillage() const
{
	return m_iMaxPillage;
}

int CvEventInfo::getFreeUnitSupport() const
{
	return m_iFreeUnitSupport;
}

int CvEventInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}

int CvEventInfo::getSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}

int CvEventInfo::getAIValue() const
{
	return m_iAIValue;
}

int CvEventInfo::getAdditionalEventChance(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), i);
	return m_piAdditionalEventChance ? m_piAdditionalEventChance[i] : 0;
}

int CvEventInfo::getAdditionalEventTime(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), i);
	return m_piAdditionalEventTime ? m_piAdditionalEventTime[i] : 0;
}

int CvEventInfo::getClearEventChance(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), i);
	return m_piClearEventChance ? m_piClearEventChance[i] : 0;
}

int CvEventInfo::getTechFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piTechFlavorValue ? m_piTechFlavorValue[i] : 0;
}

int CvEventInfo::getPlotExtraYield(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piPlotExtraYields ? m_piPlotExtraYields[i] : 0;
}

int CvEventInfo::getFreeSpecialistCount(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}

int CvEventInfo::getUnitCombatPromotion(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return m_piUnitCombatPromotions ? m_piUnitCombatPromotions[i] : -1;
}

int CvEventInfo::getUnitPromotion(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), i);
	return m_piUnitPromotions ? m_piUnitPromotions[i] : -1;
}

const CvWString& CvEventInfo::getWorldNews(int i) const
{
	FASSERT_BOUNDS(0, getNumWorldNews(), i);
	return m_aszWorldNews[i];
}

int CvEventInfo::getNumWorldNews() const
{
	return m_aszWorldNews.size();
}

int CvEventInfo::getBuildingYieldChange(int iBuilding, int iYield) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingYieldChange& it, m_aBuildingYieldChanges)
	{
		if (it.eBuilding == (BuildingTypes)iBuilding && it.eYield == (YieldTypes)iYield)
		{
			return it.iChange;
		}
	}

	return 0;
}

int CvEventInfo::getNumBuildingYieldChanges() const
{
	return m_aBuildingYieldChanges.size();
}

int CvEventInfo::getBuildingCommerceChange(int iBuilding, int iCommerce) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceChanges)
	{
		if (it.eBuilding == (BuildingTypes)iBuilding && it.eCommerce == (CommerceTypes)iCommerce)
		{
			return it.iChange;
		}
	}

	return 0;
}

int CvEventInfo::getNumBuildingCommerceChanges() const
{
	return m_aBuildingCommerceChanges.size();
}

int CvEventInfo::getBuildingCommerceModifier(int iBuilding, int iCommerce) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceModifiers)
	{
		if (it.eBuilding == (BuildingTypes)iBuilding && it.eCommerce == (CommerceTypes)iCommerce)
		{
			return it.iChange;
		}
	}

	return 0;
}

int CvEventInfo::getNumBuildingCommerceModifiers() const
{
	return m_aBuildingCommerceModifiers.size();
}

int CvEventInfo::getNumBuildingHappyChanges() const
{
	return m_aBuildingHappyChanges.size();
}

int CvEventInfo::getBuildingHappyChange(int iBuilding) const
{
	PROFILE_EXTRA_FUNC();
	for (BuildingChangeArray::const_iterator it = m_aBuildingHappyChanges.begin(); it != m_aBuildingHappyChanges.end(); ++it)
	{
		if ((*it).first == (BuildingTypes)iBuilding)
		{
			return (*it).second;
		}
	}

	return 0;
}

int CvEventInfo::getNumBuildingHealthChanges() const
{
	return m_aBuildingHealthChanges.size();
}

int CvEventInfo::getBuildingHealthChange(int iBuilding) const
{
	PROFILE_EXTRA_FUNC();
	for (BuildingChangeArray::const_iterator it = m_aBuildingHealthChanges.begin(); it != m_aBuildingHealthChanges.end(); ++it)
	{
		if ((*it).first == (BuildingTypes)iBuilding)
		{
			return (*it).second;
		}
	}

	return 0;
}

const CvProperties* CvEventInfo::getProperties() const
{
	return &m_Properties;
}

const CvProperties* CvEventInfo::getPropertiesAllCities() const
{
	return &m_PropertiesAllCities;
}

const char* CvEventInfo::getPythonCallback() const
{
	return m_szPythonCallback;
}

const char* CvEventInfo::getPythonExpireCheck() const
{
	return m_szPythonExpireCheck;
}

const char* CvEventInfo::getPythonCanDo() const
{
	return m_szPythonCanDo;
}

const char* CvEventInfo::getPythonHelp() const
{
	return m_szPythonHelp;
}

const wchar_t* CvEventInfo::getUnitNameKey() const
{
	return m_szUnitName;
}

const wchar_t* CvEventInfo::getQuestFailTextKey() const
{
	return m_szQuestFailText;
}

const wchar_t* CvEventInfo::getLocalInfoTextKey() const
{
	return m_szLocalInfoText;
}

const wchar_t* CvEventInfo::getOtherPlayerPopup() const
{
	return m_szOtherPlayerPopup;
}

int CvEventInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}
int CvEventInfo::getRevolutionIndexChange() const
{
	return m_iRevolutionIndexChange;
}

int CvEventInfo::getAdditionalEventChanceVectorSize() const						{ return m_aszAdditionalEventChanceforPass3.size(); }
CvString CvEventInfo::getAdditionalEventChanceNamesVectorElement(int i) const	{ return m_aszAdditionalEventChanceforPass3[i]; }
int CvEventInfo::getAdditionalEventChanceValuesVectorElement(int i) const		{ return m_aiAdditionalEventChanceforPass3[i]; }

int CvEventInfo::getAdditionalEventTimeVectorSize() const					{ return m_aszAdditionalEventTimeforPass3.size(); }
CvString CvEventInfo::getAdditionalEventTimeNamesVectorElement(int i) const	{ return m_aszAdditionalEventTimeforPass3[i]; }
int CvEventInfo::getAdditionalEventTimeValuesVectorElement(int i) const		{ return m_aiAdditionalEventTimeforPass3[i]; }

int CvEventInfo::getClearEventChanceVectorSize() const						{ return m_aszClearEventChanceforPass3.size(); }
CvString CvEventInfo::getClearEventChanceNamesVectorElement(int i) const	{ return m_aszClearEventChanceforPass3[i]; }
int CvEventInfo::getClearEventChanceValuesVectorElement(int i) const		{ return m_aiClearEventChanceforPass3[i]; }


void CvEventInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_bQuest);
	CheckSum(iSum, m_bGlobal);
	CheckSum(iSum, m_bTeam);
	CheckSum(iSum, m_bCityEffect);
	CheckSum(iSum, m_bOtherPlayerCityEffect);
	CheckSum(iSum, m_bGoldToPlayer);
	CheckSum(iSum, m_bGoldenAge);
	CheckSum(iSum, m_bDeclareWar);
	CheckSum(iSum, m_bDisbandUnit);
	CheckSum(iSum, m_bGameSpeedScale);
	CheckSum(iSum, m_iGold);
	CheckSum(iSum, m_iRandomGold);
	CheckSum(iSum, m_iCulture);
	CheckSum(iSum, m_iEspionagePoints);
	CheckSum(iSum, m_iTech);
	CheckSum(iSum, m_iTechPercent);
	CheckSum(iSum, m_iTechCostPercent);
	CheckSum(iSum, m_iTechMinTurnsLeft);
	CheckSum(iSum, m_iPrereqTech);
	CheckSum(iSum, m_iFreeUnit);
	CheckSum(iSum, m_iNumUnits);
	CheckSum(iSum, m_iUnitExperience);
	CheckSum(iSum, m_iUnitImmobileTurns);
	CheckSum(iSum, m_iBuilding);
	CheckSum(iSum, m_iBuildingChange);
	CheckSum(iSum, m_iHappy);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHurryAnger);
	CheckSum(iSum, m_iHappyTurns);
	CheckSum(iSum, m_iFood);
	CheckSum(iSum, m_iFoodPercent);
	CheckSum(iSum, m_iFeature);
	CheckSum(iSum, m_iFeatureChange);
	CheckSum(iSum, m_iImprovement);
	CheckSum(iSum, m_iImprovementChange);
	CheckSum(iSum, m_iBonus);
	CheckSum(iSum, m_iBonusChange);
	CheckSum(iSum, m_iRoute);
	CheckSum(iSum, m_iRouteChange);
	CheckSum(iSum, m_iBonusRevealed);
	CheckSum(iSum, m_iBonusGift);
	CheckSum(iSum, m_iConvertOwnCities);
	CheckSum(iSum, m_iConvertOtherCities);
	CheckSum(iSum, m_iMaxNumReligions);
	CheckSum(iSum, m_iOurAttitudeModifier);
	CheckSum(iSum, m_iAttitudeModifier);
	CheckSum(iSum, m_iTheirEnemyAttitudeModifier);
	CheckSum(iSum, m_iPopulationChange);
	CheckSum(iSum, m_iRevoltTurns);
	CheckSum(iSum, m_iMinPillage);
	CheckSum(iSum, m_iMaxPillage);
	CheckSum(iSum, m_iFreeUnitSupport);
	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_iSpaceProductionModifier);
	CheckSum(iSum, m_iAIValue);

	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piTechFlavorValue);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piPlotExtraYields);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_piFreeSpecialistCount);
	CheckSumI(iSum, GC.getNumEventInfos(), m_piAdditionalEventChance);
	CheckSumI(iSum, GC.getNumEventInfos(), m_piAdditionalEventTime);
	CheckSumI(iSum, GC.getNumEventInfos(), m_piClearEventChance);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_piUnitCombatPromotions);
	CheckSumI(iSum, GC.getNumUnitInfos(), m_piUnitPromotions);

	foreach_(const BuildingYieldChange& it, m_aBuildingYieldChanges)
	{
		CheckSum(iSum, it.eBuilding);
		CheckSum(iSum, it.eYield);
		CheckSum(iSum, it.iChange);
	}

	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceChanges)
	{
		CheckSum(iSum, it.eBuilding);
		CheckSum(iSum, it.eCommerce);
		CheckSum(iSum, it.iChange);
	}

	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceModifiers)
	{
		CheckSum(iSum, it.eBuilding);
		CheckSum(iSum, it.eCommerce);
		CheckSum(iSum, it.iChange);
	}

	CheckSumC(iSum, m_aBuildingHappyChanges);
	CheckSumC(iSum, m_aBuildingHealthChanges);

	//CheckSumC(iSum, m_szUnitName);
	//CheckSumC(iSum, m_szOtherPlayerPopup);

	m_Properties.getCheckSum(iSum);
	m_PropertiesAllCities.getCheckSum(iSum);

	CheckSumC(iSum, m_szPythonCallback);
	CheckSumC(iSum, m_szPythonExpireCheck);
	CheckSumC(iSum, m_szPythonCanDo);
	CheckSumC(iSum, m_szPythonHelp);

	CheckSum(iSum, m_iPrereqGameOption);
	CheckSum(iSum, m_iRevolutionIndexChange);
}

bool CvEventInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bQuest, L"bQuest");
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	pXML->GetOptionalChildXmlValByName(&m_bTeam, L"bTeam");
	pXML->GetOptionalChildXmlValByName(&m_bCityEffect, L"bPickCity");
	pXML->GetOptionalChildXmlValByName(&m_bOtherPlayerCityEffect, L"bPickOtherPlayerCity");
	pXML->GetOptionalChildXmlValByName(&m_bGoldToPlayer, L"bGoldToPlayer");
	pXML->GetOptionalChildXmlValByName(&m_bGoldenAge, L"bGoldenAge");
	pXML->GetOptionalChildXmlValByName(&m_bDeclareWar, L"bDeclareWar");
	pXML->GetOptionalChildXmlValByName(&m_iGold, L"iGold");
	pXML->GetOptionalChildXmlValByName(&m_iRandomGold, L"iRandomGold");
	pXML->GetOptionalChildXmlValByName(&m_iCulture, L"iCulture");
	pXML->GetOptionalChildXmlValByName(&m_iEspionagePoints, L"iEspionagePoints");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"Tech");
	m_iTech = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iTechPercent, L"iTechPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTechCostPercent, L"iTechCostPercent");
	pXML->GetOptionalChildXmlValByName(&m_iTechMinTurnsLeft, L"iTechMinTurnsLeft");
	pXML->GetOptionalTypeEnum(m_iPrereqTech, L"PrereqTech");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnit");
	m_iFreeUnit = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iNumUnits, L"iNumFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_bDisbandUnit, L"bDisbandUnit");
	pXML->GetOptionalChildXmlValByName(&m_bGameSpeedScale, L"bGameSpeedScale");
	pXML->GetOptionalChildXmlValByName(&m_iUnitExperience, L"iUnitExperience");
	pXML->GetOptionalChildXmlValByName(&m_iUnitImmobileTurns, L"iUnitImmobileTurns");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"Building");
	m_iBuilding = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iBuildingChange, L"iBuildingChange");

	pXML->GetOptionalChildXmlValByName(&m_iHappy, L"iHappy");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHurryAnger, L"iHurryAnger");
	pXML->GetOptionalChildXmlValByName(&m_iHappyTurns, L"iHappyTurns");
	pXML->GetOptionalChildXmlValByName(&m_iFood, L"iFood");
	pXML->GetOptionalChildXmlValByName(&m_iFoodPercent, L"iFoodPercent");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FeatureType");
	m_iFeature = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iFeatureChange, L"iFeatureChange");

	pXML->GetOptionalTypeEnum(m_iImprovement, L"ImprovementType");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementChange, L"iImprovementChange");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusType");
	m_iBonus = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iBonusChange, L"iBonusChange");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"RouteType");
	m_iRoute = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iRouteChange, L"iRouteChange");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusRevealed");
	m_iBonusRevealed = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusGift");
	m_iBonusGift = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_piTechFlavorValue, L"TechFlavors", GC.getNumFlavorTypes());
	pXML->SetVariableListTagPair(&m_piPlotExtraYields, L"PlotExtraYields", NUM_YIELD_TYPES, 0);
	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, L"FreeSpecialistCounts", GC.getNumSpecialistInfos());

	pXML->GetOptionalChildXmlValByName(&m_iConvertOwnCities, L"iConvertOwnCities");
	pXML->GetOptionalChildXmlValByName(&m_iConvertOtherCities, L"iConvertOtherCities");
	pXML->GetOptionalChildXmlValByName(&m_iMaxNumReligions, L"iMaxNumReligions", -1);
	pXML->GetOptionalChildXmlValByName(&m_iOurAttitudeModifier, L"iOurAttitudeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeModifier, L"iAttitudeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTheirEnemyAttitudeModifier, L"iTheirEnemyAttitudeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationChange, L"iPopulationChange");
	pXML->GetOptionalChildXmlValByName(&m_iRevoltTurns, L"iRevoltTurns");
	pXML->GetOptionalChildXmlValByName(&m_iMinPillage, L"iMinPillage");
	pXML->GetOptionalChildXmlValByName(&m_iMaxPillage, L"iMaxPillage");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitSupport, L"iFreeUnitSupport");
	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflationMod");
	pXML->GetOptionalChildXmlValByName(&m_iSpaceProductionModifier, L"iSpaceProductionMod");
	pXML->GetOptionalChildXmlValByName(&m_iAIValue, L"iAIValue");

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifier);
	}

	CvString* pszPromotions = NULL;
	FAssertMsg(NULL == m_piUnitCombatPromotions, "Memory leak");
	CvXMLLoadUtility::InitList(&m_piUnitCombatPromotions,GC.getNumUnitCombatInfos(),-1);
	pXML->SetVariableListTagPair(&pszPromotions, L"UnitCombatPromotions", GC.getNumUnitCombatInfos(), "NONE");
	if ( pszPromotions != NULL )
	{
		for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
		{
			m_piUnitCombatPromotions[i] = pXML->GetInfoClass(pszPromotions[i]);
		}
		SAFE_DELETE_ARRAY(pszPromotions);
	}

	FAssertMsg(NULL == m_piUnitPromotions, "Memory leak");
	CvXMLLoadUtility::InitList(&m_piUnitPromotions, GC.getNumUnitInfos(), -1);
	pXML->SetVariableListTagPair(&pszPromotions, L"PromoteUnits", GC.getNumUnitInfos(), "NONE");
	if (pszPromotions != NULL)
	{
		for (int i = 0; i < GC.getNumUnitInfos(); ++i)
		{
			m_piUnitPromotions[i] = pXML->GetInfoClass(pszPromotions[i]);
		}
		SAFE_DELETE_ARRAY(pszPromotions);
	}

	pXML->GetOptionalChildXmlValByName(m_szUnitName, L"UnitName");
	pXML->GetOptionalChildXmlValByName(m_szOtherPlayerPopup, L"OtherPlayerPopup");
	pXML->GetOptionalChildXmlValByName(m_szQuestFailText, L"QuestFailText");
	pXML->GetOptionalChildXmlValByName(m_szLocalInfoText, L"LocalInfoText");

	m_Properties.read(pXML);
	m_PropertiesAllCities.read(pXML, L"PropertiesAllCities");

	pXML->GetOptionalChildXmlValByName(m_szPythonCallback, L"PythonCallback");
	pXML->GetOptionalChildXmlValByName(m_szPythonExpireCheck, L"PythonExpireCheck");
	pXML->GetOptionalChildXmlValByName(m_szPythonCanDo, L"PythonCanDo");
	pXML->GetOptionalChildXmlValByName(m_szPythonHelp, L"PythonHelp");

	m_aszWorldNews.clear();
	if (pXML->TryMoveToXmlFirstChild(L"WorldNewsTexts"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->GetChildXmlVal(szTextVal))
			{
				for (int j=0; j<iNumSibs; ++j)
				{
					m_aszWorldNews.push_back(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	m_aBuildingYieldChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraYields"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingYieldChange kChange;
						kChange.eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						kChange.eYield = (YieldTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(&kChange.iChange);
						m_aBuildingYieldChanges.push_back(kChange);

						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	m_aBuildingCommerceChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraCommerces"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingCommerceChange kChange;
						kChange.eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						kChange.eCommerce = (CommerceTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(&kChange.iChange);
						m_aBuildingCommerceChanges.push_back(kChange);

						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	m_aBuildingCommerceModifiers.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingCommerceModifiers"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingCommerceChange kChange;
						kChange.eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						kChange.eCommerce = (CommerceTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(&kChange.iChange);
						m_aBuildingCommerceModifiers.push_back(kChange);

						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}

				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	m_aBuildingHappyChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraHappies"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						const BuildingTypes eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aBuildingHappyChanges.push_back(std::make_pair(eBuilding, iChange));

						pXML->MoveToXmlParent();

					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}

				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	m_aBuildingHealthChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"BuildingExtraHealths"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						const BuildingTypes eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aBuildingHealthChanges.push_back(std::make_pair(eBuilding, iChange));

						pXML->MoveToXmlParent();

					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}

				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iRevolutionIndexChange, L"iRevolutionIndexChange");

	if (pXML->TryMoveToXmlFirstChild(L"AdditionalEvents"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszAdditionalEventChanceforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiAdditionalEventChanceforPass3.push_back(iTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}
	if (pXML->TryMoveToXmlFirstChild(L"EventTimes"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszAdditionalEventTimeforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiAdditionalEventTimeforPass3.push_back(iTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}
	if (pXML->TryMoveToXmlFirstChild(L"ClearEvents"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszClearEventChanceforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiClearEventChanceforPass3.push_back(iTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
				pXML->MoveToXmlParent();
			}
		}
		pXML->MoveToXmlParent();
	}
	return true;
}

void CvEventInfo::copyNonDefaults(const CvEventInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isQuest() == bDefault) m_bQuest = pClassInfo->isQuest();
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();
	if (isTeam() == bDefault) m_bTeam = pClassInfo->isTeam();
	if (isCityEffect() == bDefault) m_bCityEffect = pClassInfo->isCityEffect();
	if (isOtherPlayerCityEffect() == bDefault) m_bOtherPlayerCityEffect = pClassInfo->isOtherPlayerCityEffect();
	if (isGoldToPlayer() == bDefault) m_bGoldToPlayer = pClassInfo->isGoldToPlayer();
	if (isGoldenAge() == bDefault) m_bGoldenAge = pClassInfo->isGoldenAge();
	if (isDeclareWar() == bDefault) m_bDeclareWar = pClassInfo->isDeclareWar();

	if (getGold() == iDefault) m_iGold = pClassInfo->getGold();
	if (getRandomGold() == iDefault) m_iRandomGold = pClassInfo->getRandomGold();
	if (getCulture() == iDefault) m_iCulture = pClassInfo->getCulture();
	if (getEspionagePoints() == iDefault) m_iEspionagePoints = pClassInfo->getEspionagePoints();
	if (getTech() == iTextDefault) m_iTech = pClassInfo->getTech();
	if (getTechPercent() == iDefault) m_iTechPercent = pClassInfo->getTechPercent();
	if (getTechCostPercent() == iDefault) m_iTechCostPercent = pClassInfo->getTechCostPercent();
	if (getTechMinTurnsLeft() == iDefault) m_iTechMinTurnsLeft = pClassInfo->getTechMinTurnsLeft();
	if (getPrereqTech() == iTextDefault) m_iPrereqTech = pClassInfo->getPrereqTech();
	if (getFreeUnit() == iTextDefault) m_iFreeUnit = pClassInfo->getFreeUnit();
	if (getNumUnits() == iDefault) m_iNumUnits = pClassInfo->getNumUnits();
	if (m_bDisbandUnit == bDefault) m_bDisbandUnit = pClassInfo->isDisbandUnit();
	if (m_bGameSpeedScale == bDefault) m_bGameSpeedScale = pClassInfo->isGameSpeedScale();
	if (getUnitExperience() == iDefault) m_iUnitExperience = pClassInfo->getUnitExperience();
	if (getUnitImmobileTurns() == iDefault) m_iUnitImmobileTurns = pClassInfo->getUnitImmobileTurns();
	if (getBuilding() == iTextDefault) m_iBuilding = pClassInfo->getBuilding();
	if (getBuildingChange() == iDefault) m_iBuildingChange = pClassInfo->getBuildingChange();
	if (getHappy() == iDefault) m_iHappy = pClassInfo->getHappy();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHurryAnger() == iDefault) m_iHurryAnger = pClassInfo->getHurryAnger();
	if (getHappyTurns() == iDefault) m_iHappyTurns = pClassInfo->getHappyTurns();
	if (getFood() == iDefault) m_iFood = pClassInfo->getFood();
	if (getFoodPercent() == iDefault) m_iFoodPercent = pClassInfo->getFoodPercent();
	if (getFeature() == iTextDefault) m_iFeature = pClassInfo->getFeature();
	if (getFeatureChange() == iDefault) m_iFeatureChange = pClassInfo->getFeatureChange();
	if (getImprovement() == iTextDefault) m_iImprovement = pClassInfo->getImprovement();
	if (getImprovementChange() == iDefault) m_iImprovementChange = pClassInfo->getImprovementChange();
	if (getBonus() == iTextDefault) m_iBonus = pClassInfo->getBonus();
	if (getBonusChange() == iDefault) m_iBonusChange = pClassInfo->getBonusChange();
	if (getRouteChange() == iDefault) m_iRouteChange = pClassInfo->getRouteChange();
	if (getRoute() == iTextDefault) m_iRoute = pClassInfo->getRoute();
	if (getBonusRevealed() == iTextDefault) m_iBonusRevealed = pClassInfo->getBonusRevealed();
	if (getBonusGift() == iTextDefault) m_iBonusGift = pClassInfo->getBonusGift();

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		if (getTechFlavorValue(i) == iDefault && pClassInfo->getTechFlavorValue(i) != iDefault)
		{
			if ( NULL == m_piTechFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piTechFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piTechFlavorValue[i] = pClassInfo->getTechFlavorValue(i);
		}
	}
	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( getCommerceModifier(j) == iDefault && pClassInfo->getCommerceModifier(j) != iDefault)
		{
			if ( NULL == m_piCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
	}
	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if ( getYieldModifier(i) == iDefault && pClassInfo->getYieldModifier(i) != iDefault)
		{
			if ( NULL == m_piYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piYieldModifier[i] = pClassInfo->getYieldModifier(i);
		}
		if (getPlotExtraYield(i) == iDefault && pClassInfo->getPlotExtraYield(i) != iDefault)
		{
			if ( NULL == m_piPlotExtraYields )
			{
				CvXMLLoadUtility::InitList(&m_piPlotExtraYields,NUM_YIELD_TYPES,iDefault);
			}
			m_piPlotExtraYields[i] = pClassInfo->getPlotExtraYield(i);
		}
	}
	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		if (getFreeSpecialistCount(i) == iDefault && pClassInfo->getFreeSpecialistCount(i) != iDefault)
		{
			if ( NULL == m_piFreeSpecialistCount )
			{
				CvXMLLoadUtility::InitList(&m_piFreeSpecialistCount,GC.getNumSpecialistInfos(),iDefault);
			}
			m_piFreeSpecialistCount[i] = pClassInfo->getFreeSpecialistCount(i);
		}
	}

	if (getConvertOwnCities() == iDefault) m_iConvertOwnCities = pClassInfo->getConvertOwnCities();
	if (getConvertOtherCities() == iDefault) m_iConvertOtherCities = pClassInfo->getConvertOtherCities();
	if (getMaxNumReligions() == -1) m_iMaxNumReligions = pClassInfo->getMaxNumReligions();
	if (getOurAttitudeModifier() == iDefault) m_iOurAttitudeModifier = pClassInfo->getOurAttitudeModifier();
	if (getAttitudeModifier() == iDefault) m_iAttitudeModifier = pClassInfo->getAttitudeModifier();
	if (getTheirEnemyAttitudeModifier() == iDefault) m_iTheirEnemyAttitudeModifier = pClassInfo->getTheirEnemyAttitudeModifier();
	if (getPopulationChange() == iDefault) m_iPopulationChange = pClassInfo->getPopulationChange();
	if (getRevoltTurns() == iDefault) m_iRevoltTurns = pClassInfo->getRevoltTurns();
	if (getMinPillage() == iDefault) m_iMinPillage = pClassInfo->getMinPillage();
	if (getMaxPillage() == iDefault) m_iMaxPillage = pClassInfo->getMaxPillage();
	if (getFreeUnitSupport() == iDefault) m_iFreeUnitSupport = pClassInfo->getFreeUnitSupport();
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();
	if (getSpaceProductionModifier() == iDefault) m_iSpaceProductionModifier = pClassInfo->getSpaceProductionModifier();
	if (getAIValue() == iDefault) m_iAIValue = pClassInfo->getAIValue();

	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		if (m_piUnitCombatPromotions[i] == iTextDefault)
		{
			m_piUnitCombatPromotions[i] = pClassInfo->getUnitCombatPromotion(i);
		}
	}

	for (int i = 0; i < GC.getNumUnitInfos(); ++i)
	{
		if (m_piUnitPromotions[i] == iTextDefault)
		{
			m_piUnitPromotions[i] = pClassInfo->getUnitPromotion(i);
		}
	}

	if (getUnitNameKey() == wDefault) m_szUnitName = pClassInfo->getUnitNameKey();
	if (getOtherPlayerPopup() == wDefault) m_szOtherPlayerPopup = pClassInfo->getOtherPlayerPopup();
	if (getQuestFailTextKey() == wDefault) m_szQuestFailText = pClassInfo->getQuestFailTextKey();
	if (getLocalInfoTextKey() == wDefault) m_szLocalInfoText = pClassInfo->getLocalInfoTextKey();

	m_Properties.copyNonDefaults(pClassInfo->getProperties());
	m_PropertiesAllCities.copyNonDefaults(pClassInfo->getPropertiesAllCities());

	if (getPythonCallback() == cDefault) m_szPythonCallback = pClassInfo->getPythonCallback();
	if (getPythonExpireCheck() == cDefault) m_szPythonExpireCheck = pClassInfo->getPythonExpireCheck();
	if (getPythonCanDo() == cDefault) m_szPythonCanDo = pClassInfo->getPythonCanDo();
	if (getPythonHelp() == cDefault) m_szPythonHelp = pClassInfo->getPythonHelp();

	CvWString szwTextVal;
	for ( int i = 0; i < pClassInfo->getNumWorldNews(); i++)
	{
		szwTextVal = pClassInfo->getWorldNews(i);
		m_aszWorldNews.push_back(szwTextVal);
	}

	bool bNoDuplicate = true;
	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		foreach_(const BuildingYieldChange& it, m_aBuildingYieldChanges)
		{
			if (it.eBuilding == (BuildingTypes)iBuilding)
			{
				//obviously some modder already set this Building to some value
				//we don't want to overwrite his settings with the older(assuming he added
				//his tag on purpose)
				bNoDuplicate = false;
				break;
			}
		}
		if ( bNoDuplicate )
		{
			for ( int iYield = 0; iYield < NUM_YIELD_TYPES; iYield++)
			{
				if ( pClassInfo->getBuildingYieldChange(iBuilding, iYield) != 0)
				{
					BuildingYieldChange kChange;
					kChange.eBuilding = (BuildingTypes)iBuilding;
					kChange.eYield = (YieldTypes)iYield;
					kChange.iChange = pClassInfo->getBuildingYieldChange(iBuilding, iYield);
					m_aBuildingYieldChanges.push_back(kChange);
				}
			}
		}
	}

	bNoDuplicate = true;
	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceChanges)
		{
			if (it.eBuilding == (BuildingTypes)iBuilding)
			{
				//obviously some modder already set this Building to some value
				//we don't want to overwrite his settings with the older(assuming he added
				//his tag on purpose)
				bNoDuplicate = false;
				break;
			}
		}
		if ( bNoDuplicate )
		{
			for ( int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; iCommerce++)
			{
				if ( pClassInfo->getBuildingCommerceChange(iBuilding, iCommerce) != 0)
				{
					BuildingCommerceChange kChange;
					kChange.eBuilding = (BuildingTypes)iBuilding;
					kChange.eCommerce = (CommerceTypes)iCommerce;
					kChange.iChange = pClassInfo->getBuildingCommerceChange(iBuilding, iCommerce);
					m_aBuildingCommerceChanges.push_back(kChange);
				}
			}
		}
	}

	bNoDuplicate = true;
	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceModifiers)
		{
			if (it.eBuilding == (BuildingTypes)iBuilding)
			{
				//obviously some modder already set this Building to some value
				//we don't want to overwrite his settings with the older(assuming he added
				//his tag on purpose)
				bNoDuplicate = false;
				break;
			}
		}
		if ( bNoDuplicate )
		{
			for ( int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; iCommerce++)
			{
				if ( pClassInfo->getBuildingCommerceModifier(iBuilding, iCommerce) != 0)
				{
					BuildingCommerceChange kChange;
					kChange.eBuilding = (BuildingTypes)iBuilding;
					kChange.eCommerce = (CommerceTypes)iCommerce;
					kChange.iChange = pClassInfo->getBuildingCommerceModifier(iBuilding, iCommerce);
					m_aBuildingCommerceModifiers.push_back(kChange);
				}
			}
		}
	}

	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		if ( pClassInfo->getBuildingHappyChange(iBuilding) != 0)
		{
			//if the new value is set, we assume the modder did this on purpose
			//so we only add the old value, if the new isn't 0(as with all the new loading system)
			if ( getBuildingHappyChange(iBuilding) == 0 )
			{
				const BuildingTypes eBuilding = (BuildingTypes)iBuilding;
				const int iChange = pClassInfo->getBuildingHappyChange(iBuilding);
				m_aBuildingHappyChanges.push_back(std::make_pair(eBuilding, iChange));
			}
		}
	}

	for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
	{
		if ( pClassInfo->getBuildingHealthChange(iBuilding) != 0)
		{
			//if the new value is set, we assume the modder did this on purpose
			//so we only add the old value, if the new isn't 0(as with all the new loading system)
			if ( getBuildingHealthChange(iBuilding) == 0 )
			{
				const BuildingTypes eBuilding = (BuildingTypes)iBuilding;
				const int iChange = pClassInfo->getBuildingHealthChange(iBuilding);
				m_aBuildingHealthChanges.push_back(std::make_pair(eBuilding, iChange));
			}
		}
	}

	if (getPrereqGameOption() == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();
	if (getRevolutionIndexChange() == iDefault) m_iRevolutionIndexChange = pClassInfo->getRevolutionIndexChange();

	for ( int i = 0; i < pClassInfo->getAdditionalEventChanceVectorSize(); i++ )
	{
		m_aiAdditionalEventChanceforPass3.push_back(pClassInfo->getAdditionalEventChanceValuesVectorElement(i));
		m_aszAdditionalEventChanceforPass3.push_back(pClassInfo->getAdditionalEventChanceNamesVectorElement(i));
	}
	for ( int i = 0; i < pClassInfo->getAdditionalEventTimeVectorSize(); i++ )
	{
		m_aiAdditionalEventTimeforPass3.push_back(pClassInfo->getAdditionalEventTimeValuesVectorElement(i));
		m_aszAdditionalEventTimeforPass3.push_back(pClassInfo->getAdditionalEventTimeNamesVectorElement(i));
	}
	for ( int i = 0; i < pClassInfo->getClearEventChanceVectorSize(); i++ )
	{
		m_aiClearEventChanceforPass3.push_back(pClassInfo->getClearEventChanceValuesVectorElement(i));
		m_aszClearEventChanceforPass3.push_back(pClassInfo->getClearEventChanceNamesVectorElement(i));
	}
}


bool CvEventInfo::readPass3()
{
	PROFILE_EXTRA_FUNC();
	m_piAdditionalEventChance = new int[GC.getNumEventInfos()];
	m_piAdditionalEventTime = new int[GC.getNumEventInfos()];
	m_piClearEventChance = new int[GC.getNumEventInfos()];
	for (int iI = 0; iI < GC.getNumEventInfos(); iI++)
	{
		m_piAdditionalEventChance[iI] = 0;
		m_piAdditionalEventTime[iI] = 0;
		m_piClearEventChance[iI] = 0;
	}
	if (!m_aiAdditionalEventChanceforPass3.empty() && !m_aszAdditionalEventChanceforPass3.empty())
	{
		int iNumLoad = m_aiAdditionalEventChanceforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszAdditionalEventChanceforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piAdditionalEventChance[iTempIndex] = m_aiAdditionalEventChanceforPass3[iI];
		}
		m_aszAdditionalEventChanceforPass3.clear();
		m_aiAdditionalEventChanceforPass3.clear();
	}
	if (!m_aiAdditionalEventTimeforPass3.empty() && !m_aszAdditionalEventTimeforPass3.empty())
	{
		int iNumLoad = m_aiAdditionalEventTimeforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszAdditionalEventTimeforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piAdditionalEventTime[iTempIndex] = m_aiAdditionalEventTimeforPass3[iI];
		}
		m_aszAdditionalEventTimeforPass3.clear();
		m_aiAdditionalEventTimeforPass3.clear();
	}
	if (!m_aiClearEventChanceforPass3.empty() && !m_aszClearEventChanceforPass3.empty())
	{
		int iNumLoad = m_aiClearEventChanceforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszClearEventChanceforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumEventInfos())
				m_piClearEventChance[iTempIndex] = m_aiClearEventChanceforPass3[iI];
		}
		m_aszClearEventChanceforPass3.clear();
		m_aiClearEventChanceforPass3.clear();
	}
	return true;
}


//======================================================================================================
//					CvEspionageMissionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvEspionageMissionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvEspionageMissionInfo::CvEspionageMissionInfo() :
m_bNuke(false),
m_bRevolt(false),
m_bDisablePower(false),
m_iWarWearinessCounter(0),
m_iPrereqGameOption(NO_GAMEOPTION),
m_iSabatogeResearchCostFactor(0),
m_iRemoveReligionsCostFactor(0),
m_iRemoveCorporationsCostFactor(0),
m_iCost(0),
m_bIsPassive(false),
m_bIsTwoPhases(false),
m_bTargetsCity(false),
m_bSelectPlot(false),
m_iTechPrereq(NO_TECH),
m_iVisibilityLevel(0),
m_bInvestigateCity(false),
m_bSeeDemographics(false),
m_bNoActiveMissions(false),
m_bSeeResearch(false),
m_bDestroyImprovement(false),
m_iDestroyBuildingCostFactor(0),
m_iDestroyUnitCostFactor(0),
m_iDestroyProjectCostFactor(0),
m_iDestroyProductionCostFactor(0),
m_iBuyUnitCostFactor(0),
m_iBuyCityCostFactor(0),
m_iStealTreasuryTypes(0),
m_iCityInsertCultureAmountFactor(0),
m_iCityInsertCultureCostFactor(0),
m_iCityPoisonWaterCounter(0),
m_iCityUnhappinessCounter(0),
m_iCityRevoltCounter(0),
m_iBuyTechCostFactor(0),
m_iSwitchCivicCostFactor(0),
m_iSwitchReligionCostFactor(0),
m_iPlayerAnarchyCounter(0),
m_iCounterespionageNumTurns(0),
m_iCounterespionageMod(0),
m_iDifficultyMod(0)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvEspionageMissionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvEspionageMissionInfo::~CvEspionageMissionInfo()
{
}

int CvEspionageMissionInfo::getCost() const
{
	return m_iCost;
}

bool CvEspionageMissionInfo::isPassive() const
{
	return m_bIsPassive;
}

bool CvEspionageMissionInfo::isTwoPhases() const
{
	return m_bIsTwoPhases;
}

bool CvEspionageMissionInfo::isTargetsCity() const
{
	return m_bTargetsCity;
}

bool CvEspionageMissionInfo::isSelectPlot() const
{
	return m_bSelectPlot;
}

int CvEspionageMissionInfo::getVisibilityLevel() const
{
	return m_iVisibilityLevel;
}

bool CvEspionageMissionInfo::isInvestigateCity() const
{
	return m_bInvestigateCity;
}

bool CvEspionageMissionInfo::isSeeDemographics() const
{
	return m_bSeeDemographics;
}

bool CvEspionageMissionInfo::isNoActiveMissions() const
{
	return m_bNoActiveMissions;
}

bool CvEspionageMissionInfo::isSeeResearch() const
{
	return m_bSeeResearch;
}

bool CvEspionageMissionInfo::isDestroyImprovement() const
{
	return m_bDestroyImprovement;
}

int CvEspionageMissionInfo::getDestroyBuildingCostFactor() const
{
	return m_iDestroyBuildingCostFactor;
}

int CvEspionageMissionInfo::getDestroyUnitCostFactor() const
{
	return m_iDestroyUnitCostFactor;
}

int CvEspionageMissionInfo::getDestroyProjectCostFactor() const
{
	return m_iDestroyProjectCostFactor;
}

int CvEspionageMissionInfo::getDestroyProductionCostFactor() const
{
	return m_iDestroyProductionCostFactor;
}

int CvEspionageMissionInfo::getBuyUnitCostFactor() const
{
	return m_iBuyUnitCostFactor;
}

int CvEspionageMissionInfo::getBuyCityCostFactor() const
{
	return m_iBuyCityCostFactor;
}

int CvEspionageMissionInfo::getStealTreasuryTypes() const
{
	return m_iStealTreasuryTypes;
}

int CvEspionageMissionInfo::getCityInsertCultureAmountFactor() const
{
	return m_iCityInsertCultureAmountFactor;
}

int CvEspionageMissionInfo::getCityInsertCultureCostFactor() const
{
	return m_iCityInsertCultureCostFactor;
}

int CvEspionageMissionInfo::getCityPoisonWaterCounter() const
{
	return m_iCityPoisonWaterCounter;
}

int CvEspionageMissionInfo::getCityUnhappinessCounter() const
{
	return m_iCityUnhappinessCounter;
}

int CvEspionageMissionInfo::getCityRevoltCounter() const
{
	return m_iCityRevoltCounter;
}

int CvEspionageMissionInfo::getBuyTechCostFactor() const
{
	return m_iBuyTechCostFactor;
}

int CvEspionageMissionInfo::getSwitchCivicCostFactor() const
{
	return m_iSwitchCivicCostFactor;
}

int CvEspionageMissionInfo::getSwitchReligionCostFactor() const
{
	return m_iSwitchReligionCostFactor;
}

int CvEspionageMissionInfo::getPlayerAnarchyCounter() const
{
	return m_iPlayerAnarchyCounter;
}

int CvEspionageMissionInfo::getCounterespionageNumTurns() const
{
	return m_iCounterespionageNumTurns;
}

int CvEspionageMissionInfo::getCounterespionageMod() const
{
	return m_iCounterespionageMod;
}

int CvEspionageMissionInfo::getDifficultyMod() const
{
	return m_iDifficultyMod;
}

bool CvEspionageMissionInfo::isNuke() const
{
	return m_bNuke;
}

bool CvEspionageMissionInfo::isRevolt() const
{
	return m_bRevolt;
}

bool CvEspionageMissionInfo::isDisablePower() const
{
	return m_bDisablePower;
}

int CvEspionageMissionInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}

int CvEspionageMissionInfo::getWarWearinessCounter() const
{
	return m_iWarWearinessCounter;
}

int CvEspionageMissionInfo::getSabatogeResearchCostFactor() const
{
	return m_iSabatogeResearchCostFactor;
}

int CvEspionageMissionInfo::getRemoveReligionsCostFactor() const
{
	return m_iRemoveReligionsCostFactor;
}

int CvEspionageMissionInfo::getRemoveCorporationsCostFactor() const
{
	return m_iRemoveCorporationsCostFactor;
}

bool CvEspionageMissionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_bIsPassive, L"bIsPassive");
	pXML->GetOptionalChildXmlValByName(&m_bIsTwoPhases, L"bIsTwoPhases");
	pXML->GetOptionalChildXmlValByName(&m_bTargetsCity, L"bTargetsCity");
	pXML->GetOptionalChildXmlValByName(&m_bSelectPlot, L"bSelectPlot");
	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iVisibilityLevel, L"iVisibilityLevel");
	pXML->GetOptionalChildXmlValByName(&m_bInvestigateCity, L"bInvestigateCity");
	pXML->GetOptionalChildXmlValByName(&m_bSeeDemographics, L"bSeeDemographics");
	pXML->GetOptionalChildXmlValByName(&m_bNoActiveMissions, L"bNoActiveMissions");
	pXML->GetOptionalChildXmlValByName(&m_bSeeResearch, L"bSeeResearch");
	pXML->GetOptionalChildXmlValByName(&m_bDestroyImprovement, L"bDestroyImprovement");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyBuildingCostFactor, L"iDestroyBuildingCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyUnitCostFactor, L"iDestroyUnitCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyProjectCostFactor, L"iDestroyProjectCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iDestroyProductionCostFactor, L"iDestroyProductionCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iBuyUnitCostFactor, L"iBuyUnitCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iBuyCityCostFactor, L"iBuyCityCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iStealTreasuryTypes, L"iStealTreasuryTypes");
	pXML->GetOptionalChildXmlValByName(&m_iCityInsertCultureAmountFactor, L"iCityInsertCultureAmountFactor");
	pXML->GetOptionalChildXmlValByName(&m_iCityInsertCultureCostFactor, L"iCityInsertCultureCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iCityPoisonWaterCounter, L"iCityPoisonWaterCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCityUnhappinessCounter, L"iCityUnhappinessCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCityRevoltCounter, L"iCityRevoltCounter");
	pXML->GetOptionalChildXmlValByName(&m_iBuyTechCostFactor, L"iBuyTechCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSwitchCivicCostFactor, L"iSwitchCivicCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSwitchReligionCostFactor, L"iSwitchReligionCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iPlayerAnarchyCounter, L"iPlayerAnarchyCounter");
	pXML->GetOptionalChildXmlValByName(&m_iCounterespionageNumTurns, L"iCounterespionageNumTurns");
	pXML->GetOptionalChildXmlValByName(&m_iCounterespionageMod, L"iCounterespionageMod");
	pXML->GetOptionalChildXmlValByName(&m_iDifficultyMod, L"iDifficultyMod");
	pXML->GetOptionalChildXmlValByName(&m_bNuke, L"bNuke");
	pXML->GetOptionalChildXmlValByName(&m_bRevolt, L"bRevolt");
	pXML->GetOptionalChildXmlValByName(&m_bDisablePower, L"bDisablePower");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessCounter, L"iWarWearinessCounter");
	pXML->GetOptionalChildXmlValByName(&m_iSabatogeResearchCostFactor, L"iSabatogeResearchCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iRemoveReligionsCostFactor, L"iRemoveReligionsCostFactor");
	pXML->GetOptionalChildXmlValByName(&m_iRemoveCorporationsCostFactor, L"iRemoveCorporationsCostFactor");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	return true;
}

void CvEspionageMissionInfo::copyNonDefaults(const CvEspionageMissionInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getCost() == iDefault) m_iCost = pClassInfo->getCost();
	if (isPassive() == bDefault) m_bIsPassive = pClassInfo->isPassive();
	if (isTwoPhases() == bDefault) m_bIsTwoPhases = pClassInfo->isTwoPhases();
	if (isTargetsCity() == bDefault) m_bTargetsCity = pClassInfo->isTargetsCity();
	if (isSelectPlot() == bDefault) m_bSelectPlot = pClassInfo->isSelectPlot();

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();

	if (getVisibilityLevel() == iDefault) m_iVisibilityLevel = pClassInfo->getVisibilityLevel();
	if (isInvestigateCity() == bDefault) m_bInvestigateCity = pClassInfo->isInvestigateCity();
	if (isSeeDemographics() == bDefault) m_bSeeDemographics = pClassInfo->isSeeDemographics();
	if (isNoActiveMissions() == bDefault) m_bNoActiveMissions = pClassInfo->isNoActiveMissions();
	if (isSeeResearch() == bDefault) m_bSeeResearch = pClassInfo->isSeeResearch();

	if (isDestroyImprovement() == bDefault) m_bDestroyImprovement = pClassInfo->isDestroyImprovement();
	if (getDestroyBuildingCostFactor() == iDefault) m_iDestroyBuildingCostFactor = pClassInfo->getDestroyBuildingCostFactor();
	if (getDestroyUnitCostFactor() == iDefault) m_iDestroyUnitCostFactor = pClassInfo->getDestroyUnitCostFactor();
	if (getDestroyProjectCostFactor() == iDefault) m_iDestroyProjectCostFactor = pClassInfo->getDestroyProjectCostFactor();
	if (getDestroyProductionCostFactor() == iDefault) m_iDestroyProductionCostFactor = pClassInfo->getDestroyProductionCostFactor();
	if (getBuyUnitCostFactor() == iDefault) m_iBuyUnitCostFactor = pClassInfo->getBuyUnitCostFactor();
	if (getBuyCityCostFactor() == iDefault) m_iBuyCityCostFactor = pClassInfo->getBuyCityCostFactor();
	if (getStealTreasuryTypes() == iDefault) m_iStealTreasuryTypes = pClassInfo->getStealTreasuryTypes();
	if (getCityInsertCultureAmountFactor() == iDefault) m_iCityInsertCultureAmountFactor = pClassInfo->getCityInsertCultureAmountFactor();
	if (getCityInsertCultureCostFactor() == iDefault) m_iCityInsertCultureCostFactor = pClassInfo->getCityInsertCultureCostFactor();
	if (getCityPoisonWaterCounter() == iDefault) m_iCityPoisonWaterCounter = pClassInfo->getCityPoisonWaterCounter();
	if (getCityUnhappinessCounter() == iDefault) m_iCityUnhappinessCounter = pClassInfo->getCityUnhappinessCounter();
	if (getCityRevoltCounter() == iDefault) m_iCityRevoltCounter = pClassInfo->getCityRevoltCounter();
	if (getBuyTechCostFactor() == iDefault) m_iBuyTechCostFactor = pClassInfo->getBuyTechCostFactor();
	if (getSwitchCivicCostFactor() == iDefault) m_iSwitchCivicCostFactor = pClassInfo->getSwitchCivicCostFactor();
	if (getSwitchReligionCostFactor() == iDefault) m_iSwitchReligionCostFactor = pClassInfo->getSwitchReligionCostFactor();
	if (getPlayerAnarchyCounter() == iDefault) m_iPlayerAnarchyCounter = pClassInfo->getPlayerAnarchyCounter();
	if (getCounterespionageNumTurns() == iDefault) m_iCounterespionageNumTurns = pClassInfo->getCounterespionageNumTurns();
	if (getCounterespionageMod() == iDefault) m_iCounterespionageMod = pClassInfo->getCounterespionageMod();
	if (getDifficultyMod() == iDefault) m_iDifficultyMod = pClassInfo->getDifficultyMod();
	if (isNuke() == bDefault) m_bNuke = pClassInfo->isNuke();
	if (isRevolt() == bDefault) m_bRevolt = pClassInfo->isRevolt();
	if (isDisablePower() == bDefault) m_bDisablePower = pClassInfo->isDisablePower();
	if (getPrereqGameOption() == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();
	if (getWarWearinessCounter() == iDefault) m_iWarWearinessCounter = pClassInfo->getWarWearinessCounter();
	if (getSabatogeResearchCostFactor() == iDefault) m_iSabatogeResearchCostFactor = pClassInfo->getSabatogeResearchCostFactor();
	if (getRemoveReligionsCostFactor() == iDefault) m_iRemoveReligionsCostFactor = pClassInfo->getRemoveReligionsCostFactor();
	if (getRemoveCorporationsCostFactor() == iDefault) m_iRemoveCorporationsCostFactor = pClassInfo->getRemoveCorporationsCostFactor();
}

void CvEspionageMissionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iCost);
	CheckSum(iSum, m_bIsPassive);
	CheckSum(iSum, m_bIsTwoPhases);
	CheckSum(iSum, m_bTargetsCity);
	CheckSum(iSum, m_bSelectPlot);

	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iVisibilityLevel);
	CheckSum(iSum, m_bInvestigateCity);
	CheckSum(iSum, m_bSeeDemographics);
	CheckSum(iSum, m_bNoActiveMissions);
	CheckSum(iSum, m_bSeeResearch);

	CheckSum(iSum, m_bDestroyImprovement);
	CheckSum(iSum, m_iDestroyBuildingCostFactor);
	CheckSum(iSum, m_iDestroyUnitCostFactor);
	CheckSum(iSum, m_iDestroyProjectCostFactor);
	CheckSum(iSum, m_iDestroyProductionCostFactor);
	CheckSum(iSum, m_iBuyUnitCostFactor);
	CheckSum(iSum, m_iBuyCityCostFactor);
	CheckSum(iSum, m_iStealTreasuryTypes);
	CheckSum(iSum, m_iCityInsertCultureAmountFactor);
	CheckSum(iSum, m_iCityInsertCultureCostFactor);
	CheckSum(iSum, m_iCityPoisonWaterCounter);
	CheckSum(iSum, m_iCityUnhappinessCounter);
	CheckSum(iSum, m_iCityRevoltCounter);
	CheckSum(iSum, m_iBuyTechCostFactor);
	CheckSum(iSum, m_iSwitchCivicCostFactor);
	CheckSum(iSum, m_iSwitchReligionCostFactor);
	CheckSum(iSum, m_iPlayerAnarchyCounter);
	CheckSum(iSum, m_iCounterespionageNumTurns);
	CheckSum(iSum, m_iCounterespionageMod);
	CheckSum(iSum, m_iDifficultyMod);
}

//////////////////////////////////////////////////////////////////////////
//
//	CvUnitArtStyleTypeInfo
//  Graphic Flavor Units packages by Impaler[WrG]
//

CvUnitArtStyleTypeInfo::CvUnitArtStyleTypeInfo()
{
}

CvUnitArtStyleTypeInfo::~CvUnitArtStyleTypeInfo()
{
}

const char* CvUnitArtStyleTypeInfo::getEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azEarlyArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azEarlyArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azEarlyArtDefineTags.push_back(kTag);
}

const char* CvUnitArtStyleTypeInfo::getLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azLateArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azLateArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azLateArtDefineTags.push_back(kTag);
}

const char* CvUnitArtStyleTypeInfo::getMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azMiddleArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azMiddleArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azMiddleArtDefineTags.push_back(kTag);
}


const char* CvUnitArtStyleTypeInfo::getClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azClassicalArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azClassicalArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azClassicalArtDefineTags.push_back(kTag);
}

const char* CvUnitArtStyleTypeInfo::getRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azRennArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azRennArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azRennArtDefineTags.push_back(kTag);
}

const char* CvUnitArtStyleTypeInfo::getIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azIndustrialArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azIndustrialArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azIndustrialArtDefineTags.push_back(kTag);
}

const char* CvUnitArtStyleTypeInfo::getFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azFutureArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azFutureArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azFutureArtDefineTags.push_back(kTag);
}


bool CvUnitArtStyleTypeInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	int j, i;
	int iNumSibs;
	int iIndex; // UnitIndex
	int iMesh;  // Mesh Index


	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	if (pXML->TryMoveToXmlFirstChild(L"StyleUnits"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				for (i = 0; i < iNumSibs; i++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						iMesh = GC.getUnitInfo((UnitTypes) iIndex).getGroupDefinitions();

						if (pXML->TryMoveToXmlFirstChild(L"UnitMeshGroup"))
						{
							// Overwrite with the Style Art
							for (j = 0; j < iMesh; j++)
							{
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"EarlyArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setEarlyArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"ClassicalArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setClassicalArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"MiddleArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setMiddleArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"RennArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setRennArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"IndustrialArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setIndustrialArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"LateArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setLateArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"FutureArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setFutureArtDefineTag(j, iIndex, szTextVal);

								if (!pXML->TryMoveToXmlNextSibling())
								{
									break;
								}
							}
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}
			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	return true;
}

void CvUnitArtStyleTypeInfo::copyNonDefaults(const CvUnitArtStyleTypeInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	for ( int iUnitIndex = 0; iUnitIndex < GC.getNumUnitInfos(); iUnitIndex++)
	{
		for ( int iUnitMesh = 0; iUnitMesh < GC.getUnitInfo((UnitTypes) iUnitIndex).getGroupDefinitions(); iUnitMesh++ )
		{
			if ( getEarlyArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getEarlyArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setEarlyArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getEarlyArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}

			if ( getLateArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getLateArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setLateArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getLateArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}

			if ( getMiddleArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getMiddleArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setMiddleArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getMiddleArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}

			if ( getClassicalArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getClassicalArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setClassicalArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getClassicalArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}
			if ( getRennArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getRennArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setRennArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getRennArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}
			if ( getIndustrialArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getIndustrialArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setIndustrialArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getIndustrialArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}
			if ( getFutureArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getFutureArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setFutureArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getFutureArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}
		}
	}
}

CvVoteSourceInfo::CvVoteSourceInfo() :
	m_iVoteInterval(0),
	m_iFreeSpecialist(NO_SPECIALIST),
	m_iCivic(NO_CIVIC),
	m_aiReligionYields(NULL),
	m_aiReligionCommerces(NULL)
{
}

CvVoteSourceInfo::~CvVoteSourceInfo()
{
	SAFE_DELETE_ARRAY(m_aiReligionYields);
	SAFE_DELETE_ARRAY(m_aiReligionCommerces);

	GC.removeDelayedResolution((int*)&m_iCivic);
}

int CvVoteSourceInfo::getVoteInterval() const
{
	return m_iVoteInterval;
}

int CvVoteSourceInfo::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}

int CvVoteSourceInfo::getCivic() const
{
	return m_iCivic;
}

int CvVoteSourceInfo::getReligionYield(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_aiReligionYields[i];
}

int CvVoteSourceInfo::getReligionCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_aiReligionCommerces[i];
}

const CvWString CvVoteSourceInfo::getPopupText() const
{
	return gDLL->getText(m_szPopupText);
}

const CvWString CvVoteSourceInfo::getSecretaryGeneralText() const
{
	return gDLL->getText(m_szSecretaryGeneralText);
}

bool CvVoteSourceInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iVoteInterval, L"iVoteInterval");
	pXML->GetOptionalChildXmlValByName(m_szPopupText, L"PopupText");
	pXML->GetOptionalChildXmlValByName(m_szSecretaryGeneralText, L"SecretaryGeneralText");

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeSpecialist");
	m_iFreeSpecialist = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Civic");
	GC.addDelayedResolution((int*)&m_iCivic, szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"ReligionYields"))
	{
		pXML->SetYields(&m_aiReligionYields);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_aiReligionYields);
	}

	if (pXML->TryMoveToXmlFirstChild(L"ReligionCommerces"))
	{
		pXML->SetCommerce(&m_aiReligionCommerces);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_aiReligionCommerces);
	}

	return true;
}

void CvVoteSourceInfo::copyNonDefaults(const CvVoteSourceInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getVoteInterval() == iDefault) m_iVoteInterval = pClassInfo->getVoteInterval();
	if (getPopupText() == wDefault) m_szPopupText = pClassInfo->m_szPopupText;
	if (getSecretaryGeneralText() == wDefault) m_szSecretaryGeneralText = pClassInfo->m_szSecretaryGeneralText;
	if (getFreeSpecialist() == iTextDefault) m_iFreeSpecialist = pClassInfo->getFreeSpecialist();
	GC.copyNonDefaultDelayedResolution((int*)&m_iCivic, (int*)&pClassInfo->m_iCivic);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if ( getReligionYield(i) == iDefault )
		{
			m_aiReligionYields[i] = pClassInfo->getReligionYield(i);
		}
	}

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++)
	{
		if ( getReligionCommerce(i) == iDefault )
		{
			m_aiReligionCommerces[i] = pClassInfo->getReligionCommerce(i);
		}
	}
}

void CvVoteSourceInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iVoteInterval);
	CheckSum(iSum, m_iFreeSpecialist);
	CheckSum(iSum, m_iCivic);

	CheckSum(iSum, m_aiReligionYields, NUM_YIELD_TYPES);
	CheckSum(iSum, m_aiReligionCommerces, NUM_COMMERCE_TYPES);

	CheckSumC(iSum, m_szPopupText);
	CheckSumC(iSum, m_szSecretaryGeneralText);
}

CvMainMenuInfo::CvMainMenuInfo()
{
}

CvMainMenuInfo::~CvMainMenuInfo()
{
}

std::string CvMainMenuInfo::getScene() const
{
	return m_szScene;
}

std::string CvMainMenuInfo::getSceneNoShader() const
{
	return m_szSceneNoShader;
}

std::string CvMainMenuInfo::getSoundtrack() const
{
	return m_szSoundtrack;
}

std::string CvMainMenuInfo::getLoading() const
{
	return m_szLoading;
}

std::string CvMainMenuInfo::getLoadingSlideshow() const
{
	return m_szLoadingSlideshow;
}

bool CvMainMenuInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szScene, L"Scene");
	pXML->GetChildXmlValByName(m_szSceneNoShader, L"SceneNoShader");
	pXML->GetChildXmlValByName(m_szSoundtrack, L"Soundtrack");
	pXML->GetChildXmlValByName(m_szLoading, L"Loading");
	pXML->GetChildXmlValByName(m_szLoadingSlideshow, L"LoadingSlideshow");

	return true;
}

void CvMainMenuInfo::copyNonDefaults(const CvMainMenuInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getScene() == cDefault) m_szScene = pClassInfo->getScene();
	if (getSceneNoShader() == cDefault) m_szSceneNoShader = pClassInfo->getSceneNoShader();
	if (getSoundtrack() == cDefault) m_szSoundtrack = pClassInfo->getSoundtrack();
	if (getLoading() == cDefault) m_szLoading = pClassInfo->getLoading();
	if (getLoadingSlideshow() == cDefault) m_szLoadingSlideshow = pClassInfo->getLoadingSlideshow();
}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 10/24/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
// MLF loading
CvModLoadControlInfo::CvModLoadControlInfo():
m_bLoad(NULL),
m_iDirDepth(0),
m_iNumModules(0),
m_paszModuleFolder(NULL)
{
}

CvModLoadControlInfo::~CvModLoadControlInfo()
{
	SAFE_DELETE_ARRAY(m_bLoad);
	SAFE_DELETE_ARRAY(m_paszModuleFolder);
}

bool CvModLoadControlInfo::isLoad(int i) const
{
	FASSERT_BOUNDS(0, getNumModules(), i);
	return m_bLoad[i];
}

void CvModLoadControlInfo::setLoad(int i, bool bLoad)
{
	FASSERT_BOUNDS(0, getNumModules(), i);
	m_bLoad[i] = bLoad;
}

int CvModLoadControlInfo::getNumModules() const
{
	return m_iNumModules;
}

int CvModLoadControlInfo::getDirDepth() const
{
	return m_iDirDepth;
}

const std::string CvModLoadControlInfo::getModuleFolder(int i) const
{
	FASSERT_BOUNDS(0, getNumModules(), i);
	return m_paszModuleFolder[i];
}

const std::string CvModLoadControlInfo::getParentFolder() const
{
	return m_paszParentFolder;
}

bool CvModLoadControlInfo::read(CvXMLLoadUtility* pXML, CvString szDirDepth, int iDirDepth)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	//Class variables
	m_iDirDepth = iDirDepth;
	m_paszParentFolder = szDirDepth; //verify previous level in CvXMLLoadUtility::getModLoadControlDirectory
	//method variables
	CvString szTextVal;
	std::string m_szDirDepth;
	bool bLoad;

	if (pXML->TryMoveToXmlFirstChild(L"Modules"))
	{
		m_iNumModules = pXML->GetXmlChildrenNumber();
		if (0 < m_iNumModules)
		{
			m_paszModuleFolder = new std::string[m_iNumModules];
			m_bLoad = new bool[m_iNumModules];
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int iIndex = 0; iIndex < m_iNumModules; iIndex++)
				{
					m_bLoad[iIndex] = false;	// by default bLoad is false
					if (pXML->GetChildXmlVal(szTextVal))
					{
						pXML->GetNextXmlVal(&bLoad, true);
						if (bLoad)
						{
							m_bLoad[iIndex] = bLoad;	//this Module needs to be loaded
							m_szDirDepth = szDirDepth;
							m_szDirDepth.append(szTextVal);
							m_paszModuleFolder[iIndex] = m_szDirDepth;
							GC.setTotalNumModules();  //we need this for looping in the XMLLoad class
						}

						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}
	return true;
}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 END												  */
/************************************************************************************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPropertyInfo
//
//  DESC:   Contains info about generic properties which can be added to buildings
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvPropertyInfo::CvPropertyInfo() :
								m_iChar(0),
								m_iFontButtonIndex(0),
								m_bSourceDrain(false),
								m_bOAType(false),
								m_iAIWeight(0),
								m_iOperationalRangeMin(-500),
								m_iOperationalRangeMax(500),
								m_iTargetLevel(0),
								m_iTrainReluctance(1),
								m_eAIScaleType(AISCALE_NONE),
								m_PropertyManipulators()
{
	PROFILE_EXTRA_FUNC();
	for (int i=0; i < NUM_GAMEOBJECTS; i++)
	{
		for (int j=0; j < NUM_GAMEOBJECTS; j++)
		{
			m_aaiChangePropagator[i][j] = 0;
		}
	}
}

CvPropertyInfo::~CvPropertyInfo()
{
	PROFILE_EXTRA_FUNC();
	foreach_(const PropertyBuilding& propBuilding, m_aPropertyBuildings)
	{
		GC.removeDelayedResolution((int*)&propBuilding.eBuilding);
	}

	foreach_(const PropertyPromotion& propPromotion, m_aPropertyPromotions)
	{
		GC.removeDelayedResolution((int*)&propPromotion.ePromotion);
	}
}

bool CvPropertyInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szValueDisplayText, L"ValueDisplayText");
	pXML->GetOptionalChildXmlValByName(m_szChangeDisplayText, L"ChangeDisplayText");
	pXML->GetOptionalChildXmlValByName(m_szChangeAllCitiesDisplayText, L"ChangeAllCitiesDisplayText");
	pXML->GetOptionalChildXmlValByName(m_szPrereqMinDisplayText, L"PrereqMinDisplayText");
	pXML->GetOptionalChildXmlValByName(m_szPrereqMaxDisplayText, L"PrereqMaxDisplayText");
	pXML->GetOptionalChildXmlValByName(&m_iAIWeight, L"iAIWeight");
	pXML->GetOptionalChildXmlValByName(&m_bSourceDrain, L"bSourceDrain");
	pXML->GetOptionalChildXmlValByName(&m_bOAType, L"bOAType");
	pXML->GetOptionalChildXmlValByName(&m_iOperationalRangeMin, L"iOperationalRangeMin", -500);
	pXML->GetOptionalChildXmlValByName(&m_iOperationalRangeMax, L"iOperationalRangeMax", 500);
	pXML->GetOptionalChildXmlValByName(&m_iTargetLevel, L"iTargetLevel", 0);
	pXML->GetOptionalChildXmlValByName(&m_iTrainReluctance, L"iTrainReluctance", 1);
	pXML->GetOptionalChildXmlValByName(&m_iFontButtonIndex, L"FontButtonIndex");
	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"AIScaleType");
	m_eAIScaleType = (AIScaleTypes) pXML->GetInfoClass(szTextVal);

	if(pXML->TryMoveToXmlFirstChild(L"ChangePropagators"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"ChangePropagator"))
			{
				do
				{
					int iChangePercent;
					CvString from;
					pXML->GetChildXmlValByName(from, L"GameObjectTypeFrom");
					int eFrom = pXML->GetInfoClass(from);
					CvString to;
					pXML->GetChildXmlValByName(to, L"GameObjectTypeTo");
					int eTo = pXML->GetInfoClass(to);
					pXML->GetChildXmlValByName(&iChangePercent, L"iChangePercent");
					if (eFrom == -1 || eTo == -1)
					{
						CvXMLLoadUtility::showXMLError("ChangePropagator From (%s) and To (%s) must both be valid", from.c_str(), to.c_str());
					}
					else
					{
						m_aaiChangePropagator[eFrom][eTo] = iChangePercent;
					}
				}
				while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PropertyBuildings"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"PropertyBuilding" );
		m_aPropertyBuildings.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertyBuilding"))
			{
				do
				{
					pXML->GetChildXmlValByName(&(m_aPropertyBuildings[i].iMinValue), L"iMinValue");
					pXML->GetChildXmlValByName(&(m_aPropertyBuildings[i].iMaxValue), L"iMaxValue");
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					GC.addDelayedResolution((int*)&(m_aPropertyBuildings[i].eBuilding), szTextVal);
					i++;
				}
				while(pXML->TryMoveToXmlNextSibling(L"PropertyBuilding"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PropertyPromotions"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"PropertyPromotion");
		m_aPropertyPromotions.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PropertyPromotion"))
			{
				do
				{
					pXML->GetChildXmlValByName(&(m_aPropertyPromotions[i].iMinValue), L"iMinValue");
					pXML->GetChildXmlValByName(&(m_aPropertyPromotions[i].iMaxValue), L"iMaxValue");
					pXML->GetChildXmlValByName(szTextVal, L"PromotionType");
					GC.addDelayedResolution((int*)&(m_aPropertyPromotions[i].ePromotion), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PropertyPromotion"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	m_aTargetLevelbyEraTypes.clear();
	if (pXML->TryMoveToXmlFirstChild(L"TargetLevelbyEraTypes"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						EraTypes eEra = (EraTypes)pXML->GetInfoClass(szTextVal);
						int iLevel;
						pXML->GetNextXmlVal(&iLevel);
						m_aTargetLevelbyEraTypes.push_back(std::make_pair(eEra, iLevel));

						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}

				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvPropertyInfo::copyNonDefaults(const CvPropertyInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getAIWeight() == iDefault) m_iAIWeight = pClassInfo->getAIWeight();
	if (isSourceDrain() == false) m_bSourceDrain = pClassInfo->isSourceDrain();
	if (isOAType() == false) m_bOAType = pClassInfo->isOAType();
	if (getOperationalRangeMin() == -500) m_iOperationalRangeMin = pClassInfo->getOperationalRangeMin();
	if (getOperationalRangeMax() == 500) m_iOperationalRangeMax = pClassInfo->getOperationalRangeMax();
	if (getTargetLevel() == 0) m_iTargetLevel = pClassInfo->getTargetLevel();
	if (getTrainReluctance() == 1) m_iTrainReluctance = pClassInfo->getTrainReluctance();
	if (getAIScaleType() == AISCALE_NONE) m_eAIScaleType = pClassInfo->getAIScaleType();
	if (getValueDisplayText() == NULL || getValueDisplayText() == wDefault) m_szValueDisplayText = pClassInfo->getValueDisplayText();
	if (getChangeDisplayText() == NULL || getChangeDisplayText() == wDefault) m_szChangeDisplayText = pClassInfo->getChangeDisplayText();
	if (getChangeAllCitiesDisplayText() == NULL || getChangeAllCitiesDisplayText() == wDefault) m_szChangeAllCitiesDisplayText = pClassInfo->getChangeAllCitiesDisplayText();
	if (getPrereqMinDisplayText() == NULL || getPrereqMinDisplayText() == wDefault) m_szPrereqMinDisplayText = pClassInfo->getPrereqMinDisplayText();
	if (getPrereqMaxDisplayText() == NULL || getPrereqMaxDisplayText() == wDefault) m_szPrereqMaxDisplayText = pClassInfo->getPrereqMaxDisplayText();
	if (getFontButtonIndex() == 0) m_iFontButtonIndex = pClassInfo->getFontButtonIndex();

	for (int i=0; i < NUM_GAMEOBJECTS; i++)
	{
		for (int j=0; j < NUM_GAMEOBJECTS; j++)
		{
			if (m_aaiChangePropagator[i][j] == 0)
			{
				m_aaiChangePropagator[i][j] = pClassInfo->getChangePropagator((GameObjectTypes)i,(GameObjectTypes)j);
			}
		}
	}

	if (getNumPropertyBuildings() == 0)
	{
		const int iNum = pClassInfo->getNumPropertyBuildings();
		m_aPropertyBuildings.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPropertyBuildings[i] = pClassInfo->getPropertyBuilding(i);
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPropertyBuildings[i].eBuilding), (int*)&(pClassInfo->getPropertyBuilding(i).eBuilding));
		}
	}

	if (getNumPropertyPromotions() == 0)
	{
		const int iNum = pClassInfo->getNumPropertyPromotions();
		m_aPropertyPromotions.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPropertyPromotions[i] = pClassInfo->getPropertyPromotion(i);
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPropertyPromotions[i].ePromotion), (int*)&(pClassInfo->getPropertyPromotion(i).ePromotion));
		}
	}

	if (getNumTargetLevelbyEraTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTargetLevelbyEraTypes(); i++)
		{
			m_aTargetLevelbyEraTypes.push_back(std::make_pair((EraTypes)i, pClassInfo->getTargetLevelbyEraType(i)));
		}
	}

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

void CvPropertyInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_bSourceDrain);
	CheckSum(iSum, m_bOAType);
	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_eAIScaleType);
	CheckSum(iSum, m_iOperationalRangeMin);
	CheckSum(iSum, m_iOperationalRangeMax);
	CheckSum(iSum, m_iTargetLevel);
	CheckSum(iSum, m_iTrainReluctance);
	CheckSumC(iSum, m_aTargetLevelbyEraTypes);

	for (int i=0; i < NUM_GAMEOBJECTS; i++)
	{
		for (int j=0; j < NUM_GAMEOBJECTS; j++)
		{
			CheckSum(iSum, m_aaiChangePropagator[i][j]);
		}
	}

	foreach_(const PropertyBuilding& propBuilding, m_aPropertyBuildings)
	{
		CheckSum(iSum, propBuilding.iMinValue);
		CheckSum(iSum, propBuilding.iMaxValue);
		CheckSum(iSum, (int)propBuilding.eBuilding);
	}

	foreach_(const PropertyPromotion& propPromotion, m_aPropertyPromotions)
	{
		CheckSum(iSum, propPromotion.iMinValue);
		CheckSum(iSum, propPromotion.iMaxValue);
		CheckSum(iSum, (int)propPromotion.ePromotion);
	}

	m_PropertyManipulators.getCheckSum(iSum);
}

int CvPropertyInfo::getChar() const
{
	return m_iChar;
}

void CvPropertyInfo::setChar(int i)
{
	m_iChar = i;
}

int CvPropertyInfo::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}

bool CvPropertyInfo::isSourceDrain() const
{
	return m_bSourceDrain;
}

bool CvPropertyInfo::isOAType() const
{
	return m_bOAType;
}

int	CvPropertyInfo::getAIWeight() const
{
	return m_iAIWeight;
}

int	CvPropertyInfo::getOperationalRangeMin() const
{
	return m_iOperationalRangeMin;
}

int	CvPropertyInfo::getOperationalRangeMax() const
{
	return m_iOperationalRangeMax;
}

int	CvPropertyInfo::getTargetLevel() const
{
	return m_iTargetLevel;
}

int	CvPropertyInfo::getTrainReluctance() const
{
	return m_iTrainReluctance;
}

AIScaleTypes CvPropertyInfo::getAIScaleType() const
{
	return m_eAIScaleType;
}

CvWString CvPropertyInfo::getValueDisplayText() const
{
	return m_szValueDisplayText;
}

CvWString CvPropertyInfo::getChangeDisplayText() const
{
	return m_szChangeDisplayText;
}

CvWString CvPropertyInfo::getChangeAllCitiesDisplayText() const
{
	return m_szChangeAllCitiesDisplayText;
}

CvWString CvPropertyInfo::getPrereqMinDisplayText() const
{
	return m_szPrereqMinDisplayText;
}

CvWString CvPropertyInfo::getPrereqMaxDisplayText() const
{
	return m_szPrereqMaxDisplayText;
}

int CvPropertyInfo::getChangePropagator(const GameObjectTypes eFrom, const GameObjectTypes eTo) const
{
	FASSERT_BOUNDS(0, NUM_GAMEOBJECTS, eFrom);
	FASSERT_BOUNDS(0, NUM_GAMEOBJECTS, eTo);
	return m_aaiChangePropagator[eFrom][eTo];
}

const PropertyBuilding& CvPropertyInfo::getPropertyBuilding(int index) const
{
	FASSERT_BOUNDS(0, getNumPropertyBuildings(), index);
	return m_aPropertyBuildings[index];
}

int CvPropertyInfo::getNumPropertyBuildings() const
{
	return (int)m_aPropertyBuildings.size();
}

const PropertyPromotion& CvPropertyInfo::getPropertyPromotion(int index) const
{
	FASSERT_BOUNDS(0, getNumPropertyPromotions(), index);
	return m_aPropertyPromotions[index];
}

int CvPropertyInfo::getNumPropertyPromotions() const
{
	return (int)m_aPropertyPromotions.size();
}

int CvPropertyInfo::getNumTargetLevelbyEraTypes() const
{
	return m_aTargetLevelbyEraTypes.size();
}

int CvPropertyInfo::getTargetLevelbyEraType(int iIndex) const
{
	PROFILE_EXTRA_FUNC();
	for (EraArray::const_iterator it = m_aTargetLevelbyEraTypes.begin(); it != m_aTargetLevelbyEraTypes.end(); ++it)
	{
		if ((*it).first == (EraTypes)iIndex)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPropertyInfo::isTargetLevelbyEraType(int iIndex) const
{
	PROFILE_EXTRA_FUNC();
	for (EraArray::const_iterator it = m_aTargetLevelbyEraTypes.begin(); it != m_aTargetLevelbyEraTypes.end(); ++it)
	{
		if ((*it).first == (EraTypes)iIndex)
		{
			return true;
		}
	}
	return false;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvOutcomeInfo
//
//  DESC:   Contains info about outcome types which can be the result of a kill or of actions
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvOutcomeInfo::CvOutcomeInfo() :
									m_ePrereqTech(NO_TECH),
									m_eObsoleteTech(NO_TECH),
									m_bToCoastalCity(false),
									m_bFriendlyTerritory(true),
									m_bNeutralTerritory(true),
									m_bHostileTerritory(true),
									m_bBarbarianTerritory(false),
									m_bCity(false),
									m_bNotCity(false),
									m_bCapture(false),
									m_ePrereqCivic(NO_CIVIC)
{
}

CvOutcomeInfo::~CvOutcomeInfo()
{
	GC.removeDelayedResolutionVector(m_aeReplaceOutcomes);
}

bool CvOutcomeInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szMessageText, L"Message");
	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_eObsoleteTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqCivic");
	m_ePrereqCivic = (CivicTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bToCoastalCity, L"bToCoastalCity");
	pXML->GetOptionalChildXmlValByName(&m_bFriendlyTerritory, L"bFriendlyTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bNeutralTerritory, L"bNeutralTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bHostileTerritory, L"bHostileTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bBarbarianTerritory, L"bBarbarianTerritory");
	pXML->GetOptionalChildXmlValByName(&m_bCity, L"bCity");
	pXML->GetOptionalChildXmlValByName(&m_bNotCity, L"bNotCity");
	pXML->GetOptionalChildXmlValByName(&m_bCapture, L"bCapture");

	if(pXML->TryMoveToXmlFirstChild(L"ExtraChancePromotions"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"ExtraChancePromotion"))
			{
				do
				{
					int iExtraChance;
					pXML->GetChildXmlValByName(szTextVal, L"PromotionType");
					PromotionTypes ePromotion = (PromotionTypes) pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&iExtraChance, L"iExtraChance");
					m_aeiExtraChancePromotions.push_back(std::pair<PromotionTypes,int>(ePromotion, iExtraChance));
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	pXML->SetOptionalVector(&m_aePrereqBuildings, L"PrereqBuildings");
	pXML->SetOptionalVectorWithDelayedResolution(m_aeReplaceOutcomes, L"ReplaceOutcomes");

	return true;
}

void CvOutcomeInfo::copyNonDefaults(const CvOutcomeInfo* pClassInfo)
{
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getMessageText() == wDefault) m_szMessageText = pClassInfo->getMessageText();
	if (getPrereqTech() == NO_TECH) m_ePrereqTech = pClassInfo->getPrereqTech();
	if (getObsoleteTech() == NO_TECH) m_eObsoleteTech = pClassInfo->getObsoleteTech();
	if (!getToCoastalCity()) m_bToCoastalCity = pClassInfo->getToCoastalCity();
	if (getFriendlyTerritory()) m_bFriendlyTerritory = pClassInfo->getFriendlyTerritory();
	if (getNeutralTerritory()) m_bNeutralTerritory = pClassInfo->getNeutralTerritory();
	if (getHostileTerritory()) m_bHostileTerritory = pClassInfo->getHostileTerritory();
	if (!getBarbarianTerritory()) m_bBarbarianTerritory = pClassInfo->getBarbarianTerritory();
	if (!getCity()) m_bCity = pClassInfo->getCity();
	if (!getNotCity()) m_bNotCity = pClassInfo->getNotCity();
	if (!isCapture()) m_bCapture = pClassInfo->isCapture();
	if (m_aePrereqBuildings.empty()) m_aePrereqBuildings = pClassInfo->m_aePrereqBuildings;
	if (getNumExtraChancePromotions() == 0) m_aeiExtraChancePromotions = pClassInfo->m_aeiExtraChancePromotions;
	if (getPrereqCivic() == NO_CIVIC) m_ePrereqCivic = pClassInfo->getPrereqCivic();

	GC.copyNonDefaultDelayedResolutionVector(m_aeReplaceOutcomes, pClassInfo->getReplaceOutcomes());
}

void CvOutcomeInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eObsoleteTech);
	CheckSumC(iSum, m_aeiExtraChancePromotions);
	CheckSumC(iSum, m_aePrereqBuildings);
	CheckSum(iSum, m_bToCoastalCity);
	CheckSum(iSum, m_bFriendlyTerritory);
	CheckSum(iSum, m_bNeutralTerritory);
	CheckSum(iSum, m_bHostileTerritory);
	CheckSum(iSum, m_bBarbarianTerritory);
	CheckSum(iSum, m_bCity);
	CheckSum(iSum, m_bNotCity);
	CheckSum(iSum, m_bCapture);
	CheckSumC(iSum, m_aeReplaceOutcomes);
	CheckSum(iSum, m_ePrereqCivic);
}

CvWString CvOutcomeInfo::getMessageText() const
{
	return m_szMessageText;
}

bool CvOutcomeInfo::getToCoastalCity() const
{
	return m_bToCoastalCity;
}

bool CvOutcomeInfo::getFriendlyTerritory() const
{
	return m_bFriendlyTerritory;
}

bool CvOutcomeInfo::getNeutralTerritory() const
{
	return m_bNeutralTerritory;
}

bool CvOutcomeInfo::getHostileTerritory() const
{
	return m_bHostileTerritory;
}

bool CvOutcomeInfo::getBarbarianTerritory() const
{
	return m_bBarbarianTerritory;
}

bool CvOutcomeInfo::getCity() const
{
	return m_bCity;
}

bool CvOutcomeInfo::getNotCity() const
{
	return m_bNotCity;
}

bool CvOutcomeInfo::isCapture() const
{
	return m_bCapture;
}

TechTypes CvOutcomeInfo::getObsoleteTech() const
{
	return m_eObsoleteTech;
}

CivicTypes CvOutcomeInfo::getPrereqCivic() const
{
	return m_ePrereqCivic;
}

int CvOutcomeInfo::getNumExtraChancePromotions() const
{
	return m_aeiExtraChancePromotions.size();
}

PromotionTypes CvOutcomeInfo::getExtraChancePromotion(int i) const
{
	FASSERT_BOUNDS(0, getNumExtraChancePromotions(), i);
	return m_aeiExtraChancePromotions[i].first;
}

int CvOutcomeInfo::getExtraChancePromotionChance(int i) const
{
	FASSERT_BOUNDS(0, getNumExtraChancePromotions(), i);
	return m_aeiExtraChancePromotions[i].second;
}

//TB Promotion Line Info
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionLineInfo
//
//  DESC:   Contains info about Promotion Lines
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvPromotionLineInfo::CvPromotionLineInfo() :
									m_ePrereqTech(NO_TECH),
									m_eObsoleteTech(NO_TECH),
									m_ePropertyType(NO_PROPERTY),
#ifdef OUTBREAKS_AND_AFFLICTIONS
									m_iToleranceBuildup(0),
									m_iToleranceDecay(0),
									m_iCommunicability(0),
									m_iWorseningProbabilityIncrementModifier(0),
									m_iWorsenedCommunicabilityIncrementModifier(0),
									m_iWorsenedOvercomeIncrementModifier(0),
									m_iOvercomeProbability(0),
									m_iOvercomeAdjperTurn(0),
									m_iOutbreakModifier(100),
									m_iOvercomeModifier(100),
									m_bAffliction(false),
#endif
									m_bEquipment(false),
									m_bCritical(false),
									m_bNoSpreadonBattle(false),
									m_bNoSpreadUnitProximity(false),
									m_bNoSpreadUnittoCity(false),
									m_bNoSpreadCitytoUnit(false),
									m_bBuildUp(false),
									m_bPoison(false)
{
}

CvPromotionLineInfo::~CvPromotionLineInfo()
{
}

bool CvPromotionLineInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_eObsoleteTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PropertyType");
	m_ePropertyType = (PropertyTypes) pXML->GetInfoClass(szTextVal);

#ifdef OUTBREAKS_AND_AFFLICTIONS
	pXML->GetOptionalChildXmlValByName(&m_iToleranceBuildup, L"iToleranceBuildup");
	pXML->GetOptionalChildXmlValByName(&m_iToleranceDecay, L"iToleranceDecay");
	pXML->GetOptionalChildXmlValByName(&m_iCommunicability, L"iCommunicability");
	pXML->GetOptionalChildXmlValByName(&m_iWorseningProbabilityIncrementModifier, L"iWorseningProbabilityIncrementModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorsenedCommunicabilityIncrementModifier, L"iWorsenedCommunicabilityIncrementModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorsenedOvercomeIncrementModifier, L"iWorsenedOvercomeIncrementModifier");
	pXML->GetOptionalChildXmlValByName(&m_iOvercomeProbability, L"iOvercomeProbability");
	pXML->GetOptionalChildXmlValByName(&m_iOvercomeAdjperTurn, L"iOvercomeAdjperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iOutbreakModifier, L"iOutbreakModifier", 100);
	pXML->GetOptionalChildXmlValByName(&m_iOvercomeModifier, L"iOvercomeModifier", 100);
	pXML->GetOptionalChildXmlValByName(&m_bAffliction, L"bAffliction");
#endif
	pXML->GetOptionalChildXmlValByName(&m_bEquipment, L"bEquipment");
	pXML->GetOptionalChildXmlValByName(&m_bCritical, L"bCritical");
	pXML->GetOptionalChildXmlValByName(&m_bNoSpreadonBattle, L"bNoSpreadonBattle");
	pXML->GetOptionalChildXmlValByName(&m_bNoSpreadUnitProximity, L"bNoSpreadUnitProximity");
	pXML->GetOptionalChildXmlValByName(&m_bNoSpreadUnittoCity, L"bNoSpreadUnittoCity");
	pXML->GetOptionalChildXmlValByName(&m_bNoSpreadCitytoUnit, L"bNoSpreadCitytoUnit");
	pXML->GetOptionalChildXmlValByName(&m_bBuildUp, L"bBuildUp");
	pXML->GetOptionalChildXmlValByName(&m_bPoison, L"bPoison");

	// bool vector without delayed resolution
	pXML->SetOptionalVector(&m_aiUnitCombatPrereqTypes, L"UnitCombatPrereqTypes");
	pXML->SetOptionalVector(&m_aiNotOnUnitCombatTypes, L"NotOnUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiNotOnDomainTypes, L"NotOnDomainTypes");
	pXML->SetOptionalVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	pXML->SetOptionalVector(&m_aiCriticalOriginCombatClassTypes, L"CriticalOriginCombatClassTypes");

	// int vector utilizing pairing without delayed resolution
	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatContractChanceChanges, L"UnitCombatContractChanceChanges");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatOvercomeChanges, L"UnitCombatOvercomeChanges");

	pXML->SetOptionalPairVector<TechModifierArray, TechTypes, int>(&m_aTechContractChanceChanges, L"TechContractChanceChanges");

	pXML->SetOptionalPairVector<TechModifierArray, TechTypes, int>(&m_aTechOvercomeChanges, L"TechOvercomeChanges");

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	return true;
}

void CvPromotionLineInfo::copyNonDefaults(const CvPromotionLineInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const bool bDefault = false;
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPrereqTech() == NO_TECH) m_ePrereqTech = pClassInfo->getPrereqTech();
	if (getObsoleteTech() == NO_TECH) m_eObsoleteTech = pClassInfo->getObsoleteTech();

#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (getToleranceBuildup() == iDefault) m_iToleranceBuildup = pClassInfo->getToleranceBuildup();
	if (getToleranceDecay() == iDefault) m_iToleranceDecay = pClassInfo->getToleranceDecay();
	if (getCommunicability() == iDefault) m_iCommunicability = pClassInfo->getCommunicability();
	if (getWorseningProbabilityIncrementModifier() == iDefault) m_iWorseningProbabilityIncrementModifier = pClassInfo->getWorseningProbabilityIncrementModifier();
	if (getWorsenedCommunicabilityIncrementModifier() == iDefault) m_iWorsenedCommunicabilityIncrementModifier = pClassInfo->getWorsenedCommunicabilityIncrementModifier();
	if (getWorsenedOvercomeIncrementModifier() == iDefault) m_iWorsenedOvercomeIncrementModifier = pClassInfo->getWorsenedOvercomeIncrementModifier();
	if (getOvercomeProbability() == iDefault) m_iOvercomeProbability = pClassInfo->getOvercomeProbability();
	if (getOvercomeAdjperTurn() == iDefault) m_iOvercomeAdjperTurn = pClassInfo->getOvercomeAdjperTurn();
	if (getOutbreakModifier() == iDefault) m_iOutbreakModifier = pClassInfo->getOutbreakModifier();
	if (getOvercomeModifier() == iDefault) m_iOvercomeModifier = pClassInfo->getOvercomeModifier();
	if (isAffliction() == bDefault) m_bAffliction = pClassInfo->isAffliction();
#endif
	if (isEquipment() == bDefault) m_bEquipment = pClassInfo->isEquipment();
	if (isCritical() == bDefault) m_bCritical = pClassInfo->isCritical();
	if (isNoSpreadonBattle() == bDefault) m_bNoSpreadonBattle = pClassInfo->isNoSpreadonBattle();
	if (isNoSpreadUnitProximity() == bDefault) m_bNoSpreadUnitProximity = pClassInfo->isNoSpreadUnitProximity();
	if (isNoSpreadUnittoCity() == bDefault) m_bNoSpreadUnittoCity = pClassInfo->isNoSpreadUnittoCity();
	if (isNoSpreadCitytoUnit() == bDefault) m_bNoSpreadCitytoUnit = pClassInfo->isNoSpreadCitytoUnit();
	if (isBuildUp() == bDefault) m_bBuildUp = pClassInfo->isBuildUp();
	if (isPoison() == bDefault) m_bPoison = pClassInfo->isPoison();

	if (getPropertyType() != NO_PROPERTY) m_ePropertyType = pClassInfo->getPropertyType();

	// bool vectors without delayed resolution
	if (getNumUnitCombatPrereqTypes() == 0)
	{
		m_aiUnitCombatPrereqTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumUnitCombatPrereqTypes(); i++)
		{
			m_aiUnitCombatPrereqTypes.push_back(pClassInfo->getUnitCombatPrereqType(i));
		}
	}

	if (getNumNotOnUnitCombatTypes() == 0)
	{
		m_aiNotOnUnitCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnUnitCombatTypes(); i++)
		{
			m_aiNotOnUnitCombatTypes.push_back(pClassInfo->getNotOnUnitCombatType(i));
		}
	}

	if (getNumNotOnDomainTypes() == 0)
	{
		m_aiNotOnDomainTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnDomainTypes(); i++)
		{
			m_aiNotOnDomainTypes.push_back(pClassInfo->getNotOnDomainType(i));
		}
	}

	if (getNumOnGameOptions() == 0)
	{
		m_aiOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumOnGameOptions(); i++)
		{
			m_aiOnGameOptions.push_back(pClassInfo->getOnGameOption(i));
		}
	}

	if (getNumNotOnGameOptions() == 0)
	{
		m_aiNotOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnGameOptions(); i++)
		{
			m_aiNotOnGameOptions.push_back(pClassInfo->getNotOnGameOption(i));
		}
	}

	if (getNumCriticalOriginCombatClassTypes() == 0)
	{
		m_aiCriticalOriginCombatClassTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumCriticalOriginCombatClassTypes(); i++)
		{
			m_aiCriticalOriginCombatClassTypes.push_back(pClassInfo->getCriticalOriginCombatClassType(i));
		}
	}

	// int vector utilizing pairing without delayed resolution
	if (getNumUnitCombatContractChanceChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatContractChanceChanges(); i++)
		{
			const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			const int iChange = pClassInfo->getUnitCombatContractChanceChange(i);
			m_aUnitCombatContractChanceChanges.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumUnitCombatOvercomeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatOvercomeChanges(); i++)
		{
			const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			const int iChange = pClassInfo->getUnitCombatOvercomeChange(i);
			m_aUnitCombatOvercomeChanges.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumTechContractChanceChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumTechContractChanceChanges(); i++)
		{
			const TechTypes eTech = ((TechTypes)i);
			const int iChange = pClassInfo->getTechContractChanceChange(i);
			m_aTechContractChanceChanges.push_back(std::make_pair(eTech, iChange));
		}
	}

	if (getNumTechOvercomeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumTechOvercomeChanges(); i++)
		{
			const TechTypes eTech = ((TechTypes)i);
			const int iChange = pClassInfo->getTechOvercomeChange(i);
			m_aTechOvercomeChanges.push_back(std::make_pair(eTech, iChange));
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

}

void CvPromotionLineInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eObsoleteTech);
#ifdef OUTBREAKS_AND_AFFLICTIONS
	//integers
	CheckSum(iSum, m_iToleranceBuildup);
	CheckSum(iSum, m_iToleranceDecay);
	CheckSum(iSum, m_iCommunicability);
	CheckSum(iSum, m_iWorseningProbabilityIncrementModifier);
	CheckSum(iSum, m_iWorsenedCommunicabilityIncrementModifier);
	CheckSum(iSum, m_iWorsenedOvercomeIncrementModifier);
	CheckSum(iSum, m_iOvercomeProbability);
	CheckSum(iSum, m_iOvercomeAdjperTurn);
	CheckSum(iSum, m_iOutbreakModifier);
	CheckSum(iSum, m_iOvercomeModifier);
	//booleans
	CheckSum(iSum, m_bAffliction);
#endif // OUTBREAKS_AND_AFFLICTIONS
	CheckSum(iSum, m_bEquipment);
	CheckSum(iSum, m_bCritical);
	CheckSum(iSum, m_bNoSpreadonBattle);
	CheckSum(iSum, m_bNoSpreadUnitProximity);
	CheckSum(iSum, m_bNoSpreadUnittoCity);
	CheckSum(iSum, m_bNoSpreadCitytoUnit);
	CheckSum(iSum, m_bBuildUp);
	CheckSum(iSum, m_bPoison);

	CheckSum(iSum, (int)m_ePropertyType);
	// bool vectors without delayed resolution
	CheckSumC(iSum, m_aiUnitCombatPrereqTypes);
	CheckSumC(iSum, m_aiNotOnUnitCombatTypes);
	CheckSumC(iSum, m_aiNotOnDomainTypes);
	CheckSumC(iSum, m_aiOnGameOptions);
	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiCriticalOriginCombatClassTypes);
	// int vector utilizing pairing without delayed resolution
	CheckSumC(iSum, m_aUnitCombatContractChanceChanges);
	CheckSumC(iSum, m_aUnitCombatOvercomeChanges);
	CheckSumC(iSum, m_aTechContractChanceChanges);
	CheckSumC(iSum, m_aTechOvercomeChanges);
}

void CvPromotionLineInfo::doPostLoadCaching(uint32_t iThis)
{
	PROFILE_EXTRA_FUNC();
	//Establish speedy promotion & Building reference by line
	m_aiPromotions.clear();
	m_aiBuildings.clear();

	for ( int i = 0; i < GC.getNumPromotionInfos(); i++)
	{
		if (GC.getPromotionInfo((PromotionTypes)i).getPromotionLine() == iThis)
		{
			m_aiPromotions.push_back(i);
		}
	}
	for ( int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		if (GC.getBuildingInfo((BuildingTypes)i).getPromotionLineType() == iThis)
		{
			m_aiBuildings.push_back(i);
		}
	}
}

TechTypes CvPromotionLineInfo::getObsoleteTech() const
{
	return m_eObsoleteTech;
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvPromotionLineInfo::getToleranceBuildup() const
{
	return m_iToleranceBuildup;
}

int CvPromotionLineInfo::getToleranceDecay() const
{
	return m_iToleranceDecay;
}

int CvPromotionLineInfo::getCommunicability() const
{
	return m_iCommunicability;
}

int CvPromotionLineInfo::getWorseningProbabilityIncrementModifier() const
{
	return m_iWorseningProbabilityIncrementModifier;
}

int CvPromotionLineInfo::getWorsenedCommunicabilityIncrementModifier() const
{
	return m_iWorsenedCommunicabilityIncrementModifier;
}

int CvPromotionLineInfo::getWorsenedOvercomeIncrementModifier() const
{
	return m_iWorsenedOvercomeIncrementModifier;
}

int CvPromotionLineInfo::getOvercomeProbability() const
{
	return m_iOvercomeProbability;
}

int CvPromotionLineInfo::getOvercomeAdjperTurn() const
{
	return m_iOvercomeAdjperTurn;
}

int CvPromotionLineInfo::getOutbreakModifier() const
{
	return m_iOutbreakModifier;
}

int CvPromotionLineInfo::getOvercomeModifier() const
{
	return m_iOvercomeModifier;
}
#endif // OUTBREAKS_AND_AFFLICTIONS

bool CvPromotionLineInfo::isEquipment() const
{
	return m_bEquipment;
}

bool CvPromotionLineInfo::isCritical() const
{
	return m_bCritical;
}

bool CvPromotionLineInfo::isNoSpreadonBattle() const
{
	return m_bNoSpreadonBattle;
}

bool CvPromotionLineInfo::isNoSpreadUnitProximity() const
{
	return m_bNoSpreadUnitProximity;
}

bool CvPromotionLineInfo::isNoSpreadUnittoCity() const
{
	return m_bNoSpreadUnittoCity;
}

bool CvPromotionLineInfo::isNoSpreadCitytoUnit() const
{
	return m_bNoSpreadCitytoUnit;
}

bool CvPromotionLineInfo::isBuildUp() const
{
	return m_bBuildUp;
}

bool CvPromotionLineInfo::isPoison() const
{
	return m_bPoison;
}

PropertyTypes CvPromotionLineInfo::getPropertyType() const
{
	return m_ePropertyType;
}

int CvPromotionLineInfo::getUnitCombatPrereqType(int i) const
{
	return m_aiUnitCombatPrereqTypes[i];
}

int CvPromotionLineInfo::getNumUnitCombatPrereqTypes() const
{
	return (int)m_aiUnitCombatPrereqTypes.size();
}

int CvPromotionLineInfo::getNotOnUnitCombatType(int i) const
{
	return m_aiNotOnUnitCombatTypes[i];
}

int CvPromotionLineInfo::getNumNotOnUnitCombatTypes() const
{
	return (int)m_aiNotOnUnitCombatTypes.size();
}

bool CvPromotionLineInfo::isNotOnUnitCombatType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiNotOnUnitCombatTypes, i);
}

int CvPromotionLineInfo::getNotOnDomainType(int i) const
{
	return m_aiNotOnDomainTypes[i];
}

int CvPromotionLineInfo::getNumNotOnDomainTypes() const
{
	return (int)m_aiNotOnDomainTypes.size();
}

bool CvPromotionLineInfo::isNotOnDomainType(int i) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, i);
	return algo::any_of_equal(m_aiNotOnDomainTypes, i);
}

int CvPromotionLineInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvPromotionLineInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvPromotionLineInfo::isOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiOnGameOptions, i);
}

int CvPromotionLineInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvPromotionLineInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvPromotionLineInfo::isNotOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiNotOnGameOptions, i);
}

int CvPromotionLineInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvPromotionLineInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvPromotionLineInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

int CvPromotionLineInfo::getCriticalOriginCombatClassType(int i) const
{
	return m_aiCriticalOriginCombatClassTypes[i];
}

int CvPromotionLineInfo::getNumCriticalOriginCombatClassTypes() const
{
	return (int)m_aiCriticalOriginCombatClassTypes.size();
}

bool CvPromotionLineInfo::isCriticalOriginCombatClassType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiCriticalOriginCombatClassTypes, i);
}

// int vector utilizing pairing without delayed resolution
int CvPromotionLineInfo::getNumUnitCombatContractChanceChanges() const
{
	return m_aUnitCombatContractChanceChanges.size();
}

int CvPromotionLineInfo::getUnitCombatContractChanceChange(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatContractChanceChanges.begin(); it != m_aUnitCombatContractChanceChanges.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionLineInfo::isUnitCombatContractChanceChange(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatContractChanceChanges.begin(); it != m_aUnitCombatContractChanceChanges.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionLineInfo::getNumUnitCombatOvercomeChanges() const
{
	return m_aUnitCombatOvercomeChanges.size();
}

int CvPromotionLineInfo::getUnitCombatOvercomeChange(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatOvercomeChanges.begin(); it != m_aUnitCombatOvercomeChanges.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionLineInfo::isUnitCombatOvercomeChange(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatOvercomeChanges.begin(); it != m_aUnitCombatOvercomeChanges.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionLineInfo::getNumTechContractChanceChanges() const
{
	return m_aTechContractChanceChanges.size();
}

int CvPromotionLineInfo::getTechContractChanceChange(int iTech) const
{
	PROFILE_EXTRA_FUNC();
	for (TechModifierArray::const_iterator it = m_aTechContractChanceChanges.begin(); it != m_aTechContractChanceChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionLineInfo::isTechContractChanceChange(int iTech) const
{
	PROFILE_EXTRA_FUNC();
	for (TechModifierArray::const_iterator it = m_aTechContractChanceChanges.begin(); it != m_aTechContractChanceChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionLineInfo::getNumTechOvercomeChanges() const
{
	return m_aTechOvercomeChanges.size();
}

int CvPromotionLineInfo::getTechOvercomeChange(int iTech) const
{
	PROFILE_EXTRA_FUNC();
	for (TechModifierArray::const_iterator it = m_aTechOvercomeChanges.begin(); it != m_aTechOvercomeChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionLineInfo::isTechOvercomeChange(int iTech) const
{
	PROFILE_EXTRA_FUNC();
	for (TechModifierArray::const_iterator it = m_aTechOvercomeChanges.begin(); it != m_aTechOvercomeChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionLineInfo::getPromotion(int i) const
{
	return m_aiPromotions[i];
}

int CvPromotionLineInfo::getNumPromotions() const
{
	return (int)m_aiPromotions.size();
}

bool CvPromotionLineInfo::isPromotion(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionInfos(), i);
	return algo::any_of_equal(m_aiPromotions, i);
}

int CvPromotionLineInfo::getBuilding(int i) const
{
	return m_aiBuildings[i];
}

int CvPromotionLineInfo::getNumBuildings() const
{
	return (int)m_aiBuildings.size();
}

bool CvPromotionLineInfo::isBuilding(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	return algo::any_of_equal(m_aiBuildings, i);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMapCategoryInfo
//
//  DESC:   Contains info about Map Categories
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvMapCategoryInfo::CvMapCategoryInfo()
{
}

CvMapCategoryInfo::~CvMapCategoryInfo()
{
}

bool CvMapCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	CvInfoBase::read(pXML);

	return true;
}

void CvMapCategoryInfo::copyNonDefaults(const CvMapCategoryInfo* pClassInfo)
{
}

void CvMapCategoryInfo::getCheckSum(uint32_t& iSum) const
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaClassInfo
//
//  DESC:   Contains info about Idea Classes
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvIdeaClassInfo::CvIdeaClassInfo() :
m_bInitialized(true)
{
}

CvIdeaClassInfo::~CvIdeaClassInfo()
{
}

bool CvIdeaClassInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	return true;
}

void CvIdeaClassInfo::copyNonDefaults(const CvIdeaClassInfo* pClassInfo)
{
	//CvInfoBase::copyNonDefaults(pClassInfo);
}

void CvIdeaClassInfo::getCheckSum(uint32_t& iSum) const
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaInfo
//
//  DESC:   Contains info about Ideas
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvIdeaInfo::CvIdeaInfo() :
m_eIdeaClass(NO_IDEACLASS)
{
}

CvIdeaInfo::~CvIdeaInfo()
{
}

IdeaClassTypes CvIdeaInfo::getIdeaClass() const
{
	return m_eIdeaClass;
}

bool CvIdeaInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;

	if ( pXML->GetOptionalChildXmlValByName(szTextVal, L"IdeaClassType") )
		m_eIdeaClass = (IdeaClassTypes)pXML->GetInfoClass(szTextVal);
	else
		m_eIdeaClass = NO_IDEACLASS;

	return true;
}

void CvIdeaInfo::copyNonDefaults(const CvIdeaInfo* pClassInfo)
{
	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getIdeaClass() == NO_IDEACLASS) m_eIdeaClass = pClassInfo->getIdeaClass();
}

void CvIdeaInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_eIdeaClass);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInvisibleInfo
//
//  DESC:   Contains info about Invisibles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvInvisibleInfo::CvInvisibleInfo() :
 m_iChar(0)
,m_iFontButtonIndex(0)
,m_bIntrinsic(false)
{
}

CvInvisibleInfo::~CvInvisibleInfo()
{
}

bool CvInvisibleInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iFontButtonIndex, L"FontButtonIndex");
	pXML->GetOptionalChildXmlValByName(&m_bIntrinsic, L"bIntrinsic");

	return true;
}

void CvInvisibleInfo::copyNonDefaults(const CvInvisibleInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (m_iFontButtonIndex == iDefault) m_iFontButtonIndex = pClassInfo->getFontButtonIndex();
	if (m_bIntrinsic == iDefault) m_bIntrinsic = pClassInfo->isIntrinsic();
}

void CvInvisibleInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iChar);
	CheckSum(iSum, m_iFontButtonIndex);
	CheckSum(iSum, m_bIntrinsic);
}

void CvInvisibleInfo::setChar(int i)
{
	m_iChar = i;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCategoryInfo
//
//  DESC:   Contains info about Categories
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvCategoryInfo::CvCategoryInfo() :
	m_bInitialized(true)
{
}

CvCategoryInfo::~CvCategoryInfo()
{

	GC.removeDelayedResolutionVector(m_aiCategories);
}

bool CvCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->SetOptionalVectorWithDelayedResolution(m_aiCategories, L"Categories");

	return true;
}

void CvCategoryInfo::copyNonDefaults(const CvCategoryInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	GC.copyNonDefaultDelayedResolutionVector(m_aiCategories, pClassInfo->m_aiCategories);
}

void CvCategoryInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSumC(iSum, m_aiCategories);
}

int CvCategoryInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}
int CvCategoryInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}
bool CvCategoryInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}
