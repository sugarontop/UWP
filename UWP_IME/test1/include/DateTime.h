#pragma once

namespace V4 {

class DateTime
{
	public :
		int sec;  // 0-59
		int min;  // 0-59
		int hour; // 0-23
		int mday; // 1-31
		int mon;  // 1-12
		int year; // 2000-
		int wday; // 0-6
	
	enum TYP { YMD, YMDHMS, RFC1123,ISO8601 };

	public :
		DateTime();		
		DateTime(const FILETIME& ft);
		FString Format( TYP typ );

		static DateTime Now();
		static DateTime GNow();
};


};


