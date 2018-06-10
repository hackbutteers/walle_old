#include <walle/config/base.h>
#include <walle/strings/string.h>
#include <walle/format/numeric_string.h>
#include <walle/format/detail/bignum_dtoa.h>
#include <walle/format/detail/bignum.h>
#include <walle/format/detail/cached_powers.h>
#include <walle/format/detail/simple_fp.h>
#include <walle/format/detail/double_conversion.h>
#include <walle/format/detail/fast_dtoa.h>
#include <walle/format/detail/fixed_dtoa.h>
#include <walle/format/detail/ieee.h>
#include <walle/format/detail/strtod.h>
#include <cmath>
#include <memory>


namespace walle {
namespace detail {


void pad(std::string& str, size_t precision, size_t width, char prefix = ' ', char decSep = '.')
{
	WALLE_ASSERT(precision > 0,"");
	WALLE_ASSERT(str.length(),"");

	std::string::size_type decSepPos = str.find(decSep);
	if (decSepPos == std::string::npos) {
		str.append(1, '.');
		decSepPos = str.size() - 1;
	}

	std::string::size_type frac = str.length() - decSepPos - 1;

	std::string::size_type ePos = str.find_first_of("eE");
	std::unique_ptr<std::string> eStr;
	if (ePos != std::string::npos) {
		eStr.reset(new std::string(str.substr(ePos, std::string::npos)));
		frac -= eStr->length();
		str = str.substr(0, str.length() - eStr->length());
	}

	if (frac != precision) {
		if (frac < precision) {
			str.append(precision - frac, '0');
		} else if ((frac > precision) && (decSepPos != std::string::npos)) {
			int pos = decSepPos + 1 + precision;
			if (str[pos] >= '5') {
				char carry = 0;
				if (str[--pos] == '9') {
					str[pos] = '0';
					carry = 1;
				} else {
					++str[pos];
					carry = 0;
				}

				while (--pos >= 0) {
					if(str[pos] == decSep) {
						continue;
					}
					if(carry) {
						if((str[pos] + carry) <= '9') {
							++str[pos];
							carry = 0;
						} else {
							str[pos] = '0';
							carry = 1;
						}
					}
				}
				if (carry) {
					str.insert(str.begin(), 1, '1');
				}
			}
			str = str.substr(0, decSepPos + 1 + precision);
		}
	}

	if (eStr.get()) {
		str += *eStr;
	}

	if (width && (str.length() < width)) {
		str.insert(str.begin(), width - str.length(), prefix);
	}
}


void insert_thousand_sep(std::string& str, char thSep, char decSep = '.')
{
	WALLE_ASSERT(decSep != thSep, "");
	if (str.size() == 0) {
		return;
	}

	std::string::size_type exPos = str.find('e');
	if (exPos == std::string::npos) exPos = str.find('E');
	std::string::size_type decPos = str.find(decSep);
	// there's no rinsert, using forward iterator to go backwards
	std::string::iterator it = str.end();
	if (exPos != std::string::npos) {
		it -= str.size() - exPos;
	}
	if (decPos != std::string::npos) {
		while (it != str.begin()) {
			--it;
			if (*it == decSep) {
				break;
			}
		}
	}
	int thCount = 0;
	if (it == str.end()) --it;
	for (; it != str.begin();) {
		std::string::iterator pos = it;
		std::string::value_type chr = *it;
		std::string::value_type prevChr = *--it;

		if (!std::isdigit(chr)) {
			continue;
		}

		if (++thCount == 3 && std::isdigit(prevChr)) {
			it = str.insert(pos, thSep);
		}

		if (thCount == 3) {
			thCount = 0;
		}
	}
}


} // namespace detail 



void float_to_str(StringBuilder &result, float value, int lowDec, int highDec)
{
	using namespace walle::format_detail;

	int flags = DoubleToStringConverter::eUniqueZero |
		DoubleToStringConverter::eEmitPositiveExponentSign;
	DoubleToStringConverter dc(flags, kFltInf, kFltNan, kFltExp, lowDec, highDec, 0, 0);
	dc.to_shortest_single(value, &result);
}


void float_to_fixed_str(StringBuilder &result, float value, int precision)
{
	using namespace walle::format_detail;

	int flags = DoubleToStringConverter::eUniqueZero |
		DoubleToStringConverter::eEmitPositiveExponentSign;
	DoubleToStringConverter dc(flags, kFltInf, kFltNan, kFltExp, -std::numeric_limits<float>::digits10, std::numeric_limits<float>::digits10, 0, 0);
	dc.to_fixed(value, precision, &result);
}


std::string& float_to_str(std::string& str, float value, int precision, int width, char thSep, char decSep)
{
	if (!decSep) decSep = '.';
	if (precision == 0) value = std::floor(value);

	StdStringBuilderHelper<kMaxFLTStringLen> helper(str);
	float_to_str(helper.builder(), value);
	
	if (decSep && (decSep != '.') && (str.find('.') != std::string::npos))
		replace_in_place(str, '.', decSep);

	if (thSep) {
		detail::insert_thousand_sep(str, thSep, decSep);
	}
	if (precision > 0 || width) {
		walle::detail::pad(str, precision, width, ' ', decSep ? decSep : '.');
	}
	return str;
}


std::string& float_to_fixed_str(std::string& str, float value, int precision, int width, char thSep, char decSep)
{
	if (!decSep) decSep = '.';
	if (precision == 0) value = std::floor(value);

	StdStringBuilderHelper<kMaxFLTStringLen> helper(str);
	float_to_fixed_str(helper.builder(), value, precision);
	
	if (decSep && (decSep != '.') && (str.find('.') != std::string::npos)) {
		replace_in_place(str, '.', decSep);
	}

	if (thSep) {
		detail::insert_thousand_sep(str, thSep, decSep);
	}
	if (precision > 0 || width) {
		walle::detail::pad(str, precision, width, ' ', decSep ? decSep : '.');
	}
	return str;
}


void double_to_str(StringBuilder &result, double value, int lowDec, int highDec)
{
	using namespace walle::format_detail;

	int flags = DoubleToStringConverter::eUniqueZero |
		DoubleToStringConverter::eEmitPositiveExponentSign;
	DoubleToStringConverter dc(flags, kFltInf, kFltNan, kFltExp, lowDec, highDec, 0, 0);
	dc.to_shortest(value, &result);
}


void double_to_fixed_str(StringBuilder &result, double value, int precision)
{
	using namespace walle::format_detail;

	int flags = DoubleToStringConverter::eUniqueZero |
		DoubleToStringConverter::eEmitPositiveExponentSign;
	DoubleToStringConverter dc(flags, kFltInf, kFltNan, kFltExp, -std::numeric_limits<double>::digits10, std::numeric_limits<double>::digits10, 0, 0);
	dc.to_fixed(value, precision, &result);
}


std::string& double_to_str(std::string& str, double value, int precision, int width, char thSep, char decSep)
{
	if (!decSep) {
		decSep = '.';
	}
	if (precision == 0) {
		value = std::floor(value);
	}

	StdStringBuilderHelper<kMaxFLTStringLen> helper(str);
	double_to_str(helper.builder(), value);
		
	
	if (decSep && (decSep != '.') && (str.find('.') != std::string::npos)) {
		replace_in_place(str, '.', decSep);
	}

	if (thSep) {
		detail::insert_thousand_sep(str, thSep, decSep);
	}
	if (precision > 0 || width) {
		walle::detail::pad(str, precision, width, ' ', decSep ? decSep : '.');
	}
	return str;
}


std::string& double_to_fixed_str(std::string& str, double value, int precision, int width, char thSep, char decSep)
{
	if (!decSep) {
		decSep = '.';
	}
	if (precision == 0) {
		value = std::floor(value);
	}
	StdStringBuilderHelper<kMaxFLTStringLen> helper(str);
	double_to_fixed_str(helper.builder(), value, precision);
		
	if (decSep && (decSep != '.') && (str.find('.') != std::string::npos)) {
		replace_in_place(str, '.', decSep);
	}

	if (thSep) {
		detail::insert_thousand_sep(str, thSep, decSep);
	}
	if (precision > 0 || width) {
		walle::detail::pad(str, precision, width, ' ', decSep ? decSep : '.');
	}
	return str;
}


float str_to_float(StringRef ref)
{
	using namespace walle::format_detail;

	int processed;
	int flags = StringToDoubleConverter::eAllowLeadingSpace |
		StringToDoubleConverter::eAllowTrailingSpace;
	StringToDoubleConverter converter(flags, 0.0, Single::nan(), kFltInf, kFltNan);
	float result = converter.string_to_float(ref.data(), static_cast<int>(ref.size()), &processed);
	return result;
}

float str_to_float(StringRef ref, int &proccessed)
{
	using namespace walle::format_detail;

	int flags = StringToDoubleConverter::eAllowLeadingSpace |
		StringToDoubleConverter::eAllowTrailingSpace;
	StringToDoubleConverter converter(flags, 0.0, Single::nan(), kFltInf, kFltNan);
	float result = converter.string_to_float(ref.data(), static_cast<int>(ref.size()), &proccessed);
	return result;	
}

double str_to_double(StringRef ref)
{
	using namespace walle::format_detail;
	int processed;
	int flags = StringToDoubleConverter::eAllowLeadingSpace |
		StringToDoubleConverter::eAllowTrailingSpace;
	StringToDoubleConverter converter(flags, 0.0, Double::nan(), kFltInf, kFltNan);
	double result = converter.string_to_double(ref.data(), static_cast<int>(ref.size()), &processed);
	return result;
}

double str_to_double(StringRef ref, int &processed)
{
	using namespace walle::format_detail;
	int flags = StringToDoubleConverter::eAllowLeadingSpace |
		StringToDoubleConverter::eAllowTrailingSpace;
	StringToDoubleConverter converter(flags, 0.0, Double::nan(), kFltInf, kFltNan);
	double result = converter.string_to_double(ref.data(), static_cast<int>(ref.size()), &processed);
	return result;
}

bool str_to_float(const std::string& str, float& result, char decSep, char thSep)
{
	using namespace walle::format_detail;

	std::string tmp(str);
	trim_in_place(tmp);
	remove_in_place(tmp, thSep);
	remove_in_place(tmp, 'f');
	replace_in_place(tmp, decSep, '.');
	result = str_to_float(StringRef(tmp));
	return !std::isinf(result) &&
		!std::isnan(result);
}


bool str_to_double(const std::string& str, double& result, char decSep, char thSep)
{
	if (str.empty()) return false;

	using namespace walle::format_detail;

	std::string tmp(str);
	trim_in_place(tmp);
	remove_in_place(tmp, thSep);
	replace_in_place(tmp, decSep, '.');
	remove_in_place(tmp, 'f');
	result = str_to_double(StringRef(tmp));
	return !std::isinf(result) &&
		!std::isnan(result);
}


} //namespace walle
