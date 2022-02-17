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
			if num > table.getn(SpecificPoint) then--防止溢出
				return ;
			end
			if SpecificPoint[num][5] == false then
				if char.getWorkInt(_talkerindex,"组队") > 0 then
					char.TalkToCli(_talkerindex, _meindex, "不可组队传送！", 6);
					return;
				end
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
	SpecificPoint = {{"英雄岛前传", 5500, 10, 18, false}
					,{"英雄岛前传", 5510, 50, 17, false}
					,{"黑蛙任务", 30602, 47, 24, false}
					,{"黑蛙任务", 30604, 20, 55, false}
					,{"天空任务", 30619, 19,23, false}
					,{"天空任务", 30689, 44, 29, false}
					,{"PC老大", 5582, 58, 21, false}
					,{"席格房间", 60044, 6,7, false}
					,{"神秘老人", 60033, 14, 12, false}
					,{"大海怪前", 60208, 18, 13, false}
					,{"师傅的房间", 3008, 24, 14, false}
					,{"猜谜12楼", 850, 23, 15, false}
					,{"蒙达村", 7350, 33, 28, false}
					,{"蒙达山", 7352, 70, 61, false}
					,{"小猪森林", 703, 72, 189, false}
					,{"火山道场", 7301, 47, 28, false}
					,{"火山道场21层", 60342, 9, 11, false}
					,{"梦幻洞", 42, 33, 44, false}
					,{"地精灵洞", 60101, 7, 33, false}
					,{"海底洞窟前", 817, 188, 513, false}
					,{"天空任务", 30691, 85,60, false}
					,{"转生抓守护兽", 32012, 30,11, false}
	                };
	npcdata = {"任务传送",16048,777,42,13,4,"TransferD"};								
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

