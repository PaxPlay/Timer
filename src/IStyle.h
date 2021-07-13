#ifndef TIMER_ISTYLE_H
#define TIMER_ISTYLE_H

class CTimerClient;
class CUserCmd;
class IMoveHelper;

class IStyle
{
public:
	virtual ~IStyle() {};

public:
	virtual std::string GetName() const = 0;
	virtual std::string GetShortName() const = 0;

	virtual bool SetOption(const char* option, const char* value) = 0;
	virtual bool SetSubOption(const char* option, const char* sub_option, const char* value) = 0;
	
	virtual bool ShouldSaveReplays() const = 0;
	virtual bool ShouldSaveSegmentedReplays() const = 0;
	
	virtual bool ShouldAllowTas() const = 0;
	virtual bool ShouldSaveTasReplays() const = 0;

public:
	virtual void PlayerRunCmd(CTimerClient* client, CUserCmd* pCmd, IMoveHelper* movehelper) = 0;
};

#endif // !TIMER_ISTYLE_H
