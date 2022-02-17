function ShowMapdlg(meindex, talkerindex,page)
	token = "1\n请选择要观战的地图：\n";
	for i=1,7 do
		if (i + (page-1) * 7) > table.getn(floorid) then
			break ;
		end
		local playerlist = map.getFloorPlayer(floorid[i + (page-1) * 7][2]);
		local battlelist = map.getBattleIndex(floorid[i + (page-1) * 7][2]);
		local playernum = 0;
		local battlenum = 0;
		if type(playerlist)=="table" then
			playernum = table.getn(playerlist);
		end
		if type(battlelist)=="table" then
			battlenum = table.getn(battlelist);
		end
		token = token..floorid[i + (page-1) * 7][1].."  [style c=5](玩家数:"..playernum.." 战斗数:"..battlenum..")[/style]\n";
	end
		
	button = 8;
	local minpage = page-1;
	local maxpage = math.ceil(table.getn(floorid)/7);
	if minpage > 0 and (minpage+1) >= maxpage then--如果当前页数大于0并大于或等于总页数，返回按钮取消和上一页
		button = 8+16;
	elseif minpage > 0 and (minpage+1) < maxpage then--如果当前页数大于0并小于总页数，返回按钮取消、上一页和下一页
		button = 8+16+32;
	elseif minpage == 0 and (minpage+1) < maxpage then--如果当前页数等于0并小于总页数，返回按钮取消和下一页
		button = 8+32;
	end
		
	lssproto.windows(talkerindex, 2, button, page, char.getWorkInt( meindex, "对象"), token);
end

function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then
		ShowMapdlg(meindex, talkerindex,1);
	end
end

function WindowTalked ( meindex, talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(meindex, talkerindex) == 1 then
		if _seqno >= 1 and _seqno < 100  then
			if _select == 16 then
				ShowMapdlg(meindex, talkerindex,_seqno-1);
			elseif _select == 32 then
				ShowMapdlg(meindex, talkerindex,_seqno+1);
			else
				local setid = other.atoi(_data) + (_seqno - 1) * 7;
				if setid < 0 and setid > table.getn(floorid) then
					return ;
				end
				token = "1\n请选择要观战的队伍：\n";
				local battlelist = map.getBattleIndex(floorid[setid][2]);
				local battlenum = 0;
				if type(battlelist) == "table" then
					battlenum = table.getn(battlelist);
				end
				if battlenum <= 0 then
					char.TalkToCli(talkerindex, meindex, "该地图没有任何战斗！", 6);
					return ;
				end
				for i=1,7 do
					if i > table.getn(battlelist) then
						break ;
					end
					local playerindex1 = battle.getInt(battlelist[i],"上方队长");
					local playerindex2 = battle.getInt(battlelist[i],"下方队长");
					if char.check(playerindex1) == 1 and char.check(playerindex2) == 1 then
						local playername1 = char.getChar(playerindex1,"名字");
						local playername2 = char.getChar(playerindex2,"名字");
						token = token.."[style c=1]"..playername2.."[/style] [style c=4]VS[/style] [style c=5]"..playername2.."[/style]\n";
					end
				end
				lssproto.windows(talkerindex, 2, 8, 1000, char.getWorkInt( meindex, "对象"), token);
				char.setWorkInt(talkerindex,"临时整数1",setid);
			end
		elseif _seqno == 1000 then
			local setid = char.getWorkInt(talkerindex,"临时整数1");
			local setid2 = other.atoi(_data);
			local battlelist = map.getBattleIndex(floorid[setid][2]);
			local battlenum = 0;
			if type(battlelist) == "table" then
				battlenum = table.getn(battlelist);
			end
			if battlenum <= 0 then
				char.TalkToCli(talkerindex, meindex, "战斗已经结束！", 6);
				return ;
			end
			if setid2 > battlenum then
				char.TalkToCli(talkerindex, meindex, "战斗已经结束！", 6);
				return ;
			end
			
			local playerindex1 = battle.getInt(battlelist[setid2],"上方队长");
			battle.WatchEntry(talkerindex, playerindex1);
		end
	end
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function reload(charaindex,_data)
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

function mydata()
	floorid = {{"萨姆吉尔庄园",1042}
			  ,{"玛丽娜丝庄园",2032}
			  ,{"加    加庄园",3032}
			  ,{"卡鲁它那庄园",4032}
			  --,{"伊    甸庄园",5032}
			  --,{"塔 尔 塔庄园",6032}
			  --,{"尼 克 斯庄园",7032}
			  --,{"弗 烈 顿庄园",8032}
			  --,{"亚 伊 欧庄园",9032}
			  --,{"瑞尔亚斯庄园",7532}
			  --,{"玛丽娜丝渔村",2000}
			  };
	npcdata = {"族战观战使者",51339,777,47,49,6,"LookFmBattle"};
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 1, "["..npcdata[7].."]");
end

