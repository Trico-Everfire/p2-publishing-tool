#pragma once

#include "KeyValue.h"

#include <QString>

class CElementList
{
private:
	inline static bool initialised = false;
	inline static KeyValueRoot *elementList;
	inline static const QString defaultElementList = "\"entities\" {\n"
													 "    \"logic_relay\" {\n"
													 "        \"disable_pti_audio\" \"1\"\n"
													 "        \"tag\" \"Custom Story\"\n"
													 "    }\n"
													 "    \"npc_portal_turret_floor\" {\n"
													 "        \"tag\" \"Turret\"\n"
													 "    }\n"
													 "    \"paint_sphere\" {\n"
													 "        \"painttype\" \"0\"\n"
													 "        \"tag\" \"Bounce Gel\"\n"
													 "    }\n"
													 "    \"prop_paint_bomb\" {\n"
													 "        \"painttype\" \"0\"\n"
													 "        \"tag\" \"Bounce Gel\"\n"
													 "    }\n"
													 "    \"info_paint_sprayer\" {\n"
													 "        \"painttype\" \"0\"\n"
													 "        \"tag\" \"Bounce Gel\"\n"
													 "    }\n"
													 "    \"paint_sphere\" {\n"
													 "        \"painttype\" \"3\"\n"
													 "        \"tag\" \"Conversion Gel\"\n"
													 "    }\n"
													 "    \"prop_paint_bomb\" {\n"
													 "        \"painttype\" \"3\"\n"
													 "        \"tag\" \"Conversion Gel\"\n"
													 "    }\n"
													 "    \"info_paint_sprayer\" {\n"
													 "        \"painttype\" \"3\"\n"
													 "        \"tag\" \"Conversion Gel\"\n"
													 "    }\n"
													 "    \"paint_sphere\" {\n"
													 "        \"painttype\" \"2\"\n"
													 "        \"tag\" \"Speed Gel\"\n"
													 "    }\n"
													 "    \"prop_paint_bomb\" {\n"
													 "        \"painttype\" \"2\"\n"
													 "        \"tag\" \"Speed Gel\"\n"
													 "    }\n"
													 "    \"info_paint_sprayer\" {\n"
													 "        \"painttype\" \"2\"\n"
													 "        \"tag\" \"Speed Gel\"\n"
													 "    }\n"
													 "    \"prop_tractor_beam\" {\n"
													 "        \"tag\" \"Tractor Beam\"\n"
													 "    }\n"
													 "    \"trigger_catapult\" {\n"
													 "        \"tag\" \"Faith Plate\"\n"
													 "    }\n"
													 "    \"prop_laser_relay\" {\n"
													 "        \"tag\" \"Laser Relay\"\n"
													 "    }\n"
													 "    \"env_portal_laser\" {\n"
													 "        \"tag\" \"Laser Emitter\"\n"
													 "    }\n"
													 "    \"prop_wall_projector\" {\n"
													 "        \"tag\" \"Light Bridge\"\n"
													 "    }\n"
													 "}";

public:
	enum class ListInitResponse
	{
		FILENOTFOUND = 0,
		FILEINVALID,
		FILEINITIALISED
	};
	static ListInitResponse initialiseElementList();
	static KeyValueRoot *getElementList();
	static bool isInitialised();
};
