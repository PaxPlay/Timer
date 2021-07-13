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
	virtual std::string getName() const = 0;
	virtual std::string getShortName() const = 0;

	virtual bool setOption(const char* option, const char* value) = 0;
	virtual bool setSubOption(const char* option, const char* sub_option, const char* value) = 0;
	
	virtual bool shouldSaveReplays() const = 0;
	virtual bool shouldSaveSegmentedReplays() const = 0;
	
	virtual bool shouldAllowTas() const = 0;
	virtual bool shouldSaveTasReplays() const = 0;

public:
	virtual void PlayerRunCmd(CTimerClient* client, CUserCmd* pCmd, IMoveHelper* movehelper) = 0;
};

#endif // !TIMER_ISTYLE_H
