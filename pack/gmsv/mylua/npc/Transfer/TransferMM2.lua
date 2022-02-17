function ShowDlg(_meindex, _talkerindex, _page)
	_maxpage = math.ceil(table.getn(SpecificPoint)/7);
	token = "1\n二代MM风景区传送点：\n"
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
	SpecificPoint = {{"10级风景区", 3000, 93, 65,false}
					,{"15级风景区", 3400, 49, 61,false}
					,{"20级风景区", 1200, 52, 47,false}
					,{"25级风景区", 5577, 36, 22,false}
					,{"30级风景区", 4000, 64, 55,false}
					,{"35级风景区",20301,276,129,false}
					,{"40级风景区", 3300, 59, 50,false}
					,{"45级风景区",21201, 59, 55,false}
					,{"50级风景区",20105, 11, 21,false}
					,{"55级风景区", 6000, 32, 76,false}
					,{"60级风景区",31901,  8, 43,false}
					,{"65级风景区",30703, 40, 33,false}
					,{"70级风景区",31201, 23, 40,false}
					,{"75级风景区",   40,  9,  4,false}
					,{"79级风景区",32020, 20, 10,false}
	                };
	npcdata = {"二代MM风景区",100451,777,42,17,6,"TransferMM2"};								
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

