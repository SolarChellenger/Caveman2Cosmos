#include "CvErrorHandling.h"

#include "FAssertDialog.h"
#include <string>
#include <exception>

void CvErrorHandling::assertFail(const FAssertInfo& info)
{
#ifdef FASSERT_LOGGING
	logging::logMsg("Asserts.log", "%s %s (%d): %s,  %s\n%s\n%s",
		info.szFile ? info.szFile : "",
		info.szFunction ? info.szFunction : "",
		info.line,
		info.szExpr ? info.szExpr : "",
		info.szMsg ? info.szMsg : "",
		info.pyTrace.c_str(),
		info.dllTrace.c_str()
	);

	picojson::value::object obj;

	if (szFile) obj["file"] = picojson::value(szFile);
	if (szFunction) obj["function"] = picojson::value(szFunction);
	obj["line"] = picojson::value(static_cast<double>(line));
	if (szExpr) obj["expr"] = picojson::value(szExpr);
	if (szMsg) obj["msg"] = picojson::value(szMsg);
	if (!pyTrace.empty()) obj["py_trace"] = picojson::value(pyTrace);
	if (!dllTrace.empty()) obj["dll_trace"] = picojson::value(dllTrace);
	obj["assert_key"] = picojson::value(CvString::format(
		"%s %s (%d): %s",
		szFile ? szFile : "nofile",
		szFunction ? szFunction : "nofunc",
		line,
		szExpr ? szExpr : "noexpr"
	).c_str());
	obj["callstack_key"] = picojson::value(CvString::format("%u", stdext::hash_value(pyTrace + dllTrace)).c_str());
	logging::logMsg("AssertsJson.log", picojson::value(obj).serialize().c_str());

	return false;
#else

	FAssertDialog::Result result = FAssertDialog::display(info);

	switch (result)
	{
	case FAssertDialog::ASSERT_DLG_DEBUG:
		_asm int 3
		break;

	case FAssertDialog::ASSERT_DLG_IGNORE:
		break;

	case FAssertDialog::ASSERT_DLG_IGNOREALWAYS:
		*info.bIgnoreAlways = true;
		break;

	case FAssertDialog::ASSERT_DLG_EXIT:
		exit(0);
		break;
	}
#endif
}

void CvErrorHandling::ensureFail(const FAssertInfo& info)
{
	assertFail(info);
	throw std::exception(info.szExpression.c_str());
}

void CvErrorHandling::error(const FAssertInfo& info)
{
	assertFail(info);
}

void CvErrorHandling::fatal(const std::exception& ex)
{
	throw new std::exception("for future implementation");
}
