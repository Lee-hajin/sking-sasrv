------------------------------- 杂类模块 -------------------------------
function StringSplit(_data, _splitint)--分割处理函数（返回数组）
	local StartIndex = 1;
	local SplitIndex = 1;
	local SplitArray = {};
	while true do
		local FindLastIndex = string.find(_data, _splitint, StartIndex);
		if not FindLastIndex then
			SplitArray[SplitIndex] = string.sub(_data, StartIndex, string.len(_data));
			break ;
		end
		SplitArray[SplitIndex] = string.sub(_data, StartIndex, FindLastIndex - 1);
		StartIndex = FindLastIndex + string.len(_splitint);
		SplitIndex = SplitIndex + 1;
	end
	return SplitArray;
end

------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if _select == 8 then
		return ;
	end
	if char.getInt(_talkerindex,"转数") <= 0 then
		char.TalkToCli(_talkerindex, _meindex, "请1转后再来吧！", 6);
		return ;
	end
	if (_seqno == 0) then
		local setid = other.atoi(_data);
		if setid == 1 then
			local token = "                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n春节来了~\n大魔王为各位玩家安排了一系列勇者试练呢~\n你是否要开启春节活动之旅？\n[style c=6](任务石每天只可领取一次哦，丢弃概不补给)[/style]";
			lssproto.windows(_talkerindex, 0, 12, 1, char.getWorkInt( _meindex, "对象"), token);
		elseif setid == 2 then--兑换1
			local token = "                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n集齐[style c=6]猪年[/style]两个字牌，可跟我兑换[style c=5]猪年吉祥礼包[/style]哦！";
			lssproto.windows(_talkerindex, 0, 12, 2, char.getWorkInt( _meindex, "对象"), token);
		elseif setid == 3 then--兑换2
			local token = "                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n集齐[style c=6]猪年大吉[/style]四个字牌，可跟我兑换[style c=5]猪年幸运礼包[/style]哦！";
			lssproto.windows(_talkerindex, 0, 12, 3, char.getWorkInt( _meindex, "对象"), token);
		elseif setid == 4 then--说明
			local token = "                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]\n"
					.."\n“猪”字牌获取请到[style c=10]柯奥村(89.84)[/style]处NPC了解"
					.."\n“年”字牌获取请到[style c=10]加加村(79.55)[/style]处了解"
					.."\n“大”字牌获取请到[style c=10]多多村(72.54)[/style]处了解"
					.."\n“吉”字牌获取请到[style c=10]达那村医院(14.2)[/style]处了解";
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
		end
	elseif (_seqno == 1) then--接受任务
		if char.Finditem(_talkerindex, eventitem) >= 0 then
			char.TalkToCli(_talkerindex, _meindex, "你身上已经有任务石了！", 6);
			return ;
		end
		local mytime = char.getInt(_talkerindex,"春节任务时");
		if os.date("%Y-%m-%d",mytime) == os.date("%Y-%m-%d",os.time()) then
			char.TalkToCli(_talkerindex, _meindex, "你今天已经接受过任务了！", 6);
			return ;
		end
		local itemnum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
		if itemnum < 1 then
			char.TalkToCli(_talkerindex, _meindex, "身上必须有一个道具栏空位", 6);
			return ;
		end
		
		local itemindex = char.Additem( _talkerindex, eventitem);
		item.setInt(itemindex,"绑定",1);
		char.setInt(_talkerindex,"春节任务时",os.time());
		char.TalkToCli(_talkerindex, -1, "获得 "..item.getChar(itemindex,"名称")..",接受任务成功！", 4);
	elseif (_seqno == 2) then--兑换1
		---12950
		if npc.Free(_meindex, _talkerindex, "ITEM=23013") ~= 1 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有“新春任务石”无法兑换！", 6);
			return ;
		end
		if npc.Free(_meindex, _talkerindex, "ITEM=23014") ~= 1 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有“猪字牌”无法兑换！", 6);
			return ;
		end
		if npc.Free(_meindex, _talkerindex, "ITEM=23015") ~= 1 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有“年字牌”无法兑换！", 6);
			return ;
		end
		npc.DelItem(_talkerindex, "23013");
		npc.DelItem(_talkerindex, "23014");
		npc.DelItem(_talkerindex, "23015");
		local itemindex = char.Additem( _talkerindex, 12950);
		item.setInt(itemindex,"绑定",1);
		char.TalkToCli(_talkerindex, _meindex, "兑换成功！获得 "..item.getChar(itemindex,"显示名").." !", 4);
	elseif (_seqno == 3) then--兑换2
		---12951
		if npc.Free(_meindex, _talkerindex, "ITEM=23013") ~= 1 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有“新春任务石”无法兑换！", 6);
			return ;
		end
		if npc.Free(_meindex, _talkerindex, "ITEM=23014") ~= 1 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有“猪字牌”无法兑换！", 6);
			return ;
		end
		if npc.Free(_meindex, _talkerindex, "ITEM=23015") ~= 1 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有“年字牌”无法兑换！", 6);
			return ;
		end
		if npc.Free(_meindex, _talkerindex, "ITEM=23016") ~= 1 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有“大字牌”无法兑换！", 6);
			return ;
		end
		if npc.Free(_meindex, _talkerindex, "ITEM=23017") ~= 1 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有“吉字牌”无法兑换！", 6);
			return ;
		end
		npc.DelItem(_talkerindex, "23013");
		npc.DelItem(_talkerindex, "23014");
		npc.DelItem(_talkerindex, "23015");
		npc.DelItem(_talkerindex, "23016");
		npc.DelItem(_talkerindex, "23017");
		local itemindex = char.Additem( _talkerindex, 12951);
		item.setInt(itemindex,"绑定",1);
		char.TalkToCli(_talkerindex, _meindex, "兑换成功！获得 "..item.getChar(itemindex,"显示名").." !", 4);
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		if char.getInt(_talkerindex,"转数") <= 0 then
			char.TalkToCli(_talkerindex, _meindex, "请1转后再来吧！", 6);
			return ;
		end
		local mytime = char.getInt(_talkerindex,"春节任务时");
		local token = "4\n                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n你好，每日只可完成一次任务哦！！"
					.."\n[style c=1]活动结束时间："..os.date("%Y-%m-%d %X",endtime).."[/style]\n"
					.."\n                   接受新春任务"
					.."\n                兑换 猪年吉祥礼包"
					.."\n                兑换 猪年幸运礼包"
					.."\n                   字牌获取途径";
		lssproto.windows(_talkerindex, 2, 8, 0, char.getWorkInt( _meindex, "对象"), token);
	end
end

------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
	char.setInt(npcindex,"现称号图像",120001);
end

------------------------------- GM命令函数 -------------------------------
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

------------------------------- 常量设置函数 -------------------------------
function mydata()
	eventitem = 23013;
	endtime = 1549814399;--活动结束时间戳
	npcdata = {"春节活动",51251,777,42,23,6,"XinNian"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
