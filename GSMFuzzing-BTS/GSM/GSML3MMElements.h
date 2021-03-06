/**@file
	@brief Elements for Mobility Management messages, GSM 04.08 9.2.
*/

/*
* Copyright 2008, 2009, 2010 Free Software Foundation, Inc.
*
* This software is distributed under the terms of the GNU Affero Public License.
* See the COPYING file in the main directory for details.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



#ifndef GSML3MMELEMENTS_H
#define GSML3MMELEMENTS_H

#include "GSML3Message.h"
#include <Globals.h>

namespace GSM {

/** CM Service Type, GSM 04.08 10.5.3.3 */
class L3CMServiceType : public L3ProtocolElement {

	public:

	enum TypeCode {
		UndefinedType=0,
		MobileOriginatedCall=1,
		EmergencyCall=2,
		ShortMessage=4,							///< specifically, MO-SMS
		SupplementaryService=8,
		VoiceCallGroup=9,
		VoiceBroadcast=10,
		LocationService=11,
		MobileTerminatedCall=100,				///< non-standard code
		MobileTerminatedShortMessage=101,		///< non-standard code
		TestCall=102,			///< non-standard code

		MobileTerminatedShortMessage2=103,		///< non-standard code

		
	};
		
	private:

	TypeCode mType;

	public:

	L3CMServiceType(TypeCode wType=UndefinedType)
		:L3ProtocolElement(),mType(wType)
	{}

	TypeCode type() const { return mType; }
	//void type(TypeCode wType) {mType=wType;}

	bool operator==(const L3CMServiceType& other) const
		{ return mType == other.mType; }
	
	size_t lengthV() const { return 0; }	
	void writeV(L3Frame&, size_t&) const { assert(0); }
	void parseV(const L3Frame &src, size_t &rp);
	void parseV(const L3Frame&, size_t&, size_t) { assert(0); }
	void text(std::ostream&) const;

};


std::ostream& operator<<(std::ostream& os, L3CMServiceType::TypeCode code);


/** RejectCause, GSM 04.08 10.5.3.6 */
class L3RejectCause : public L3ProtocolElement {

private:

	int mRejectCause;

public:
	
	L3RejectCause( const int wRejectCause=0 )
		:L3ProtocolElement(),mRejectCause(wRejectCause)
	{}

	size_t lengthV() const { return 1; }	
	void writeV( L3Frame& dest, size_t &wp ) const;
	void parseV(const L3Frame&, size_t&) {}//{ assert(0); }
	void parseV(const L3Frame&, size_t& , size_t) {}//{ assert(0); }
	void text(std::ostream&) const;
};




/**
	Network Name, GSM 04.08 10.5.3.5a
	This class supports UCS2 and 7-bit (default) encodings.
*/
class L3NetworkName : public L3ProtocolElement {


private:

	static const unsigned maxLen=93;
	GSMAlphabet mAlphabet;		///< Alphabet to use for encoding
	char mName[maxLen+1];		///< network name as a C string
	int mCI;		///< CI (Country Initials) bit value

public:

	/** Set the network name, taking the default from gConfig. */
	L3NetworkName(const char* wName,
	              GSMAlphabet alphabet=ALPHABET_7BIT,
	              int wCI=gConfig.defines("GSM.ShowCountry"))
		:L3ProtocolElement(), mAlphabet(alphabet), mCI(wCI)
	{ strncpy(mName,wName,maxLen); mName[maxLen] = '\0'; }

	size_t lengthV() const
	{
		if (mAlphabet == ALPHABET_UCS2)
			return 1+strlen(mName)*2;
		else
			return 1+(strlen(mName)*7+7)/8;
	}
	void writeV(L3Frame& dest, size_t &wp) const;
	void parseV(const L3Frame&, size_t&) { assert(0); }
	void parseV(const L3Frame&, size_t& , size_t) { assert(0); }
	void text(std::ostream&) const;
};


	
	
/**
	Network Name, GSM 04.08 10.5.3.5a
	This class supports UCS2 and 7-bit (default) encodings.
*/
class FuzzingL3NetworkName : public L3ProtocolElement {
	
	
	private:
	
		static const unsigned maxLen=256;
		GSMAlphabet mAlphabet;		///< Alphabet to use for encoding
		char mName[maxLen+1];		///< network name as a C string
		int mCI;		///< CI (Country Initials) bit value
	
	public:
	
		/** Set the network name, taking the default from gConfig. */
		FuzzingL3NetworkName(const char* wName,
					  GSMAlphabet alphabet=ALPHABET_7BIT,
					  int wCI=gConfig.defines("GSM.ShowCountry"))
			:L3ProtocolElement(), mAlphabet(alphabet), mCI(wCI)
		{ strncpy(mName,wName,maxLen); mName[maxLen] = '\0'; }
	
		size_t lengthV() const
		{
			if (mAlphabet == ALPHABET_UCS2)
				return 1+strlen(mName)*2;
			else
				return 1+(strlen(mName)*7+7)/8;
		}
		void writeV(L3Frame& dest, size_t &wp) const;
		void parseV(const L3Frame&, size_t&) { assert(0); }
		void parseV(const L3Frame&, size_t& , size_t) { assert(0); }
		void text(std::ostream&) const;
	};

/**
	Time & Time Zone, GSM 04.08 10.5.3.9, GSM 03.40 9.2.3.11.
	This class is also used in SMS.
*/
class L3TimeZoneAndTime : public L3ProtocolElement {
public:
	enum TimeType {
		LOCAL_TIME, ///< Used in SMS. Time is sent as local time. In this case
		            ///< timezone seems to be ignored by handsets (tested with
		            ///< Nokia DCT3, Siemens and Windows Mobile 6), but we still
		            ///< send it.
		UTC_TIME    ///< Used in MM Info message. Time is sent as UTC time. In
		            ///< this case phones seem to regard timezone information.
	};

protected:

	Timeval mTime;
	TimeType mType;

public:

	/** Defaults from the current time. */
	L3TimeZoneAndTime(const Timeval& wTime = Timeval(), TimeType type = LOCAL_TIME)
		:L3ProtocolElement(),
		mTime(wTime),
		mType(type)
	{}

	const Timeval& time() const { return mTime; }
	void time(const Timeval& wTime) { mTime=wTime; }

	TimeType type() const { return mType; }
	void type(TimeType type) { mType=type; }

	size_t lengthV() const { return 7; }
	void writeV(L3Frame&, size_t&) const;
	void parseV(const L3Frame& src, size_t &rp);
	void parseV(const L3Frame&, size_t& , size_t) { assert(0); }
	void text(std::ostream&) const;
};


/**
	Time & Time Zone, GSM 04.08 10.5.3.9, GSM 03.40 9.2.3.11.
	This class is also used in SMS.
*/
class FuzzingL3TimeZoneAndTime : public L3ProtocolElement {
private:
	int mYear;
	int mMonth;
	int mDay;
	int mHour;
	int mMinute;
	int mSecond;
	int mTime_zone;


public:

	/** Defaults from the current time. */
	FuzzingL3TimeZoneAndTime(int wYear=0, int wMonth=0,int wDay=0, int wHour=0, int wMinute=0, int wSecond=0, int wTime_zone=0)
		:L3ProtocolElement(),
		mYear(wYear), mMonth(wMonth), mDay(wDay), mHour(wHour), mMinute(wMinute), mSecond(wSecond), mTime_zone(wTime_zone)
	{}

	size_t lengthV() const { return 7; }
	void writeV(L3Frame&, size_t&) const;
	void parseV(const L3Frame& src, size_t &rp) {assert(0);}
	void parseV(const L3Frame&, size_t& , size_t) { assert(0); }
	void text(std::ostream&) const;
};


/*-----------------------------------------------
GSM04.08 10.5.3.5
LUT (octet 1)
	0 0 Normal location updating
	0 1 Periodic updating 
	1 0 IMSI attach 
	1 1 Reserved 
	
FOR (octet 1) 
	The Follow-On Request bit (FOR) is coded as follows:
		0 No follow-on request pending
		1 Follow-on request pending
-----------------------------------------------*/

class L3LocationUpdatingType : public L3ProtocolElement {

	public:

	enum TypeCode {
		Normal=0,
		Periodic=1,
		IMSIAttach=2,
		Reserved=3
	};
		
	private:

		TypeCode mType;
		int FOR;

	public:

	L3LocationUpdatingType(TypeCode wType=Normal)
		:L3ProtocolElement(),mType(wType)
	{}
	
	TypeCode type() const { return mType; }
	int getFOR() const {return FOR;}

	bool operator==(const L3LocationUpdatingType& other) const
		{ return mType == other.mType && FOR == other.FOR; }
	
	size_t lengthV() const { return 0; }	
	void writeV(L3Frame&, size_t&) const { assert(0); }
	void parseV(const L3Frame &src, size_t &rp);
	void parseV(const L3Frame&, size_t&, size_t) { assert(0); }
	void text(std::ostream&) const;
};

std::ostream& operator<<(std::ostream& os, L3LocationUpdatingType::TypeCode code);

} // namespace GSM

#endif

// vim: ts=4 sw=4
