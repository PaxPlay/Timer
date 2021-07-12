#ifndef TIMER_CTEMPENTS_H
#define TIMER_CTEMPENTS_H

#include <extension.h>

class CTempEnts
{
public:
	static void Beam(IRecipientFilter& filter, float delay,
		const Vector& pos1, const Vector& pos2, int modelindex, int haloindex, int startframe, int framerate,
		float life, float width, float endWidth, float fadelength, float amplitude, int r, int g, int b, int a, int speed, int flags = 0)
	{
		auto tempentmanager = sdktools->GetTempEntityManager();
		if (!tempentmanager->IsAvailable())
		{
			rootconsole->ConsolePrint("Tempentmanager is not available sadge");
		}
		auto tempent = tempentmanager->GetTempEntityInfo("BeamPoints");
		
		tempent->TE_SetEntDataVector("m_vecStartPoint", static_cast<const float*>(pos1.Base()));
		tempent->TE_SetEntDataVector("m_vecEndPoint", static_cast<const float*>(pos2.Base()));
		tempent->TE_SetEntData("m_nModelIndex", modelindex);
		tempent->TE_SetEntData("m_nHaloIndex", haloindex);
		tempent->TE_SetEntData("m_nStartFrame", startframe);
		tempent->TE_SetEntData("m_nFrameRate", framerate);
		tempent->TE_SetEntDataFloat("m_fLife", life);
		tempent->TE_SetEntDataFloat("m_fWidth", width);
		tempent->TE_SetEntDataFloat("m_fEndWidth", endWidth);
		tempent->TE_SetEntDataFloat("m_fAmplitude", amplitude);
		tempent->TE_SetEntData("r", r);
		tempent->TE_SetEntData("g", g);
		tempent->TE_SetEntData("b", b);
		tempent->TE_SetEntData("a", a);
		tempent->TE_SetEntData("m_nSpeed", speed);
		tempent->TE_SetEntData("m_nFadeLength", fadelength);
		tempent->TE_Send(filter, delay);
	}
};

#endif // !TIMER_CTEMPENTS_H
