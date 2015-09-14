// (C) Copyright Gert-Jan de Vos and Jan Wilmans 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)

// Repository at: https://github.com/djeedjay/DebugViewPP/

#pragma once

#include <boost/utility.hpp>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>

#include "LogSource.h"

namespace fusion {
namespace debugviewpp {

class ILineBuffer;

struct PollLine
{
	PollLine(DWORD pid, const std::string& processName, const std::string& message, std::shared_ptr<LogSource> logsource);
	DWORD pid;
	std::string processName;
	std::string message;
	std::shared_ptr<LogSource> logsource;
};

class PassiveLogSource : public LogSource
{
public:
	explicit PassiveLogSource(Timer& timer, SourceType::type sourceType, ILineBuffer& linebuffer, long pollFrequency);
	virtual ~PassiveLogSource();
	
	virtual HANDLE GetHandle() const;
	virtual void Notify();
	virtual void Poll() {}
	virtual void Abort();

	// in contrast to the LogSource::Add methdods, these methods are de-coupled so they 
	// can be used to add messages from any thread. The typical use-case are messages from the UI thread.
	void AddMessage(DWORD pid, const char* processName, const char* message);
	void AddMessage(const std::string& message);
	void Signal();
	void StartThread();

	long GetMicrosecondInterval() const;
private:
	void Loop();

	std::vector<PollLine> m_lines;
	std::vector<PollLine> m_backBuffer;
	Handle m_handle;
	boost::mutex m_mutex;
	long m_microsecondInterval;
	boost::thread m_thread;
};

} // namespace debugviewpp 
} // namespace fusion