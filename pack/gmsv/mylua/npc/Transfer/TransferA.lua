function ShowDlg(_meindex, _talkerindex, _page)
	_maxpage = math.ceil(table.getn(SpecificPoint)/7);
	--char.TalkToCli(_talkerindex, _meindex, _maxpage, "黄色");
	token = "1 请问你要传送到哪里？\n";
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
			local warpmap =  SpecificPoint[num][2];
			local warpx =  SpecificPoint[num][3];
			local warpy =  SpecificPoint[num][4];
			if char.getWorkInt(_talkerindex,"组队") == 2 then
				return;
			end
			char.AllWarpToSpecificPoint(_talkerindex, warpmap, warpx, warpy);
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
	SpecificPoint = {{"伊甸园", 	7000, 35,128,5000}
					,{"塔耳塔地城", 7112, 18,  6,5000}
					,{"尼克斯水城", 7200, 37, 33,5000}
					,{"弗烈顿火城", 7300, 87,101,5000}
					,{"亚伊欧风城", 7400, 99, 90,5000}
					,{"瑞尔亚斯", 7500, 46, 52,5000}
					,{"萨姆吉尔庄园", 1040,46,29,0}
					,{"玛丽娜丝庄园", 2030,59,41,0}
					,{"加  加  庄园", 3030,59,36,0}
					,{"卡鲁它那庄园", 4030,29,18,0}
					--,{"伊  甸  庄园", 5030,57,25,0}
					--,{"塔 尔 塔庄园", 6030,53,9,0}
					--,{"尼 克 斯庄园", 7030,32,36,0}
					--,{"弗 列 顿庄园", 8030,14,56,0}
					--,{"亚 伊 欧庄园", 9030,56,50,0}
					--,{"瑞尔亚斯庄园", 7530,44,43,0}
	                };
	npcdata = {"庄园传送",51229,777,41,19,6,"TransferA"};								
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

