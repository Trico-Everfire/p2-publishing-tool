//
// Created by trico on 19-6-22.
//

#ifndef P2_MAP_PUBLISHER_P2ELEMENTPARSER_H
#define P2_MAP_PUBLISHER_P2ELEMENTPARSER_H

#include "KeyValue.h"

#include <QString>

enum class ListInitResponse {
	FILENOTFOUND,
	FILEINVALID,
	FILEINITIALISED
};

class P2ElementParser {
private:
	inline static bool initialised = false;
	inline static KeyValueRoot* elementList;
	inline static const QString defaultElementList = "{\n"
													 "    \"logic_relay\" {\n"
													 "        \"disable_pti_audio\" \"1\"\n"
													 "        \"tag\" \"Custom Story\"\n"
													 "    }\n"
													 "    \"npc_portal_turret_floor\" {\n"
													 "        \"tag\" \"Turret\"\n"
													 "    }\n"
													 "    \"paint_sphere|prop_paint_bomb|info_paint_sprayer\" {\n"
													 "        \"PaintType\" \"0\"\n"
													 "        \"tag\"       \"Bounce Gel\"\n"
													 "    }\n"
													 "    \"paint_sphere|prop_paint_bomb|info_paint_sprayer\" {\n"
													 "        \"PaintType\" \"3\"\n"
													 "        \"tag\"       \"Conversion Gel\"\n"
													 "    }\n"
													 "    \"paint_sphere|prop_paint_bomb|info_paint_sprayer\" {\n"
													 "        \"PaintType\" \"2\"\n"
													 "        \"tag\"       \"Speed Gel\"\n"
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
	static ListInitResponse initialiseElementList();
	static KeyValueRoot* getElementList();
	static bool isInitialised();
};

#endif // P2_MAP_PUBLISHER_P2ELEMENTPARSER_H
