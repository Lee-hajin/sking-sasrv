function ShowDlg(_meindex, _talkerindex, _page)
	_maxpage = math.ceil(table.getn(SpecificPoint)/7);
	token = "1 请问你要传送到哪里？\n"
	for i = 1, 7 do
		if (i + (_page-1) * 7) > table.getn(SpecificPoint) then
			break;
		end
		token = token .. string.format("  %-18s%s\n", SpecificPoint[i + (_page-1) * 7][1], map.getFloorName(SpecificPoint[i + (_page-1) * 7][2]));
	end
	if _maxpage == 1 then
		button = 8;
	elseif _page == 1 and _page < _maxpage then
		button = 8+32
	elseif _page > 1 and _page < _maxpage then
		button = 8+16+32;
	elseif _page == _maxpage then
		button = 8+16;
	end
	lssproto.windows(_talkerindex, "选择框", button, _page, char.getWorkInt( _meindex, "对象"), token);
end

--NPC对话事件(NPC索引)
function Talked(_meindex, _talkerindex, _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		ShowDlg(_meindex, _talkerindex, 1);
	end
end

--NPC窗口事件(NPC索引)
function WindowTalked (_meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		if _select == 0 then
			num = other.atoi(_data) + (_seqno - 1) * 7;
			if char.getWorkInt(_talkerindex,"组队") == 2 then
				return;
			end
			char.AllWarpToSpecificPoint(_talkerindex, SpecificPoint[num][2], SpecificPoint[num][3], SpecificPoint[num][4]);
		elseif _select == 16 then
			ShowDlg(_meindex, _talkerindex, _seqno - 1);
		elseif _select == 32 then
			ShowDlg(_meindex, _talkerindex, _seqno + 1);
		end
	end
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function mydata()
	SpecificPoint = {{"眠玫饭骇 15", 10001, 40, 3,-1}
					,{"眠玫饭骇 20", 31401, 92, 27,-1}
					,{"眠玫饭骇 50", 31705, 26, 29,-1}
					,{"眠玫饭骇 65", 300, 81, 270,-1}
					,{"眠玫饭骇 100", 32018, 12, 34,-1}
					,{"120级雷　龙", 7402, 155,75,-1}
					,{"140级地雷区", 60050, 15, 18,-1}
					,{"140级金暴区", 60054, 42, 50,-1}
					,{"160级泪之海", 817, 24,116,-1}
					,{"阿布洞挂技能", 10007, 29, 38,-1}
				    ,{"120级英雄岛", 500, 212, 342,-1}
	                };
	npcdata = {"练级传送",51229,777,49,33,6,"TransferB"};								
end


function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	end
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end

