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
					.."\n你要接受每日任务么？"
					.."\n接受每日任务需提交[style c=1]200声望[/style]"
					.."\n接受任务之后可获得一张[style c=4]遗失的证明[/style]"
					.."\n带着[style c=4]遗失的证明[/style]可在以下地方掉落任务物品："
					.."\n[style c=6]库塔伊洞窟地下二楼[/style]  掉落  [style c=5]雷龙的骨头[/style]"
					.."\n[style c=6]萨伊纳斯[/style]  掉落  [style c=5]M证书[/style]、[style c=5]A证书[/style]、[style c=5]X证书[/style]"
					.."\n获得以上4个物品之后即可回来找我提交任务！";
			lssproto.windows(_talkerindex, 0, 12, 1, char.getWorkInt( _meindex, "对象"), token);
		elseif setid == 2 then--提交任务
			if char.Finditem(_talkerindex, eventitem) < 0 then
				char.TalkToCli(_talkerindex, _meindex, "你身上没有遗失的证明，无法提交任务！", 6);
				return ;
			end
			if char.Finditem(_talkerindex, 2242) < 0 then
				char.TalkToCli(_talkerindex, _meindex, "你身上没有M证明，无法提交任务！", 6);
				return ;
			end
			if char.Finditem(_talkerindex, 2243) < 0 then
				char.TalkToCli(_talkerindex, _meindex, "你身上没有A证明，无法提交任务！", 6);
				return ;
			end
			if char.Finditem(_talkerindex, 2244) < 0 then
				char.TalkToCli(_talkerindex, _meindex, "你身上没有X证明，无法提交任务！", 6);
				return ;
			end
			if char.Finditem(_talkerindex, 2245) < 0 then
				char.TalkToCli(_talkerindex, _meindex, "你身上没有雷龙的骨头，无法提交任务！", 6);
				return ;
			end
			npc.DelItem(_talkerindex, eventitem.."*15");
			npc.DelItem(_talkerindex, "2242*15");
			npc.DelItem(_talkerindex, "2243*15");
			npc.DelItem(_talkerindex, "2244*15");
			npc.DelItem(_talkerindex, "2245*15");
			local itemindex = char.Additem( _talkerindex, 2246);
			item.setInt(itemindex,"绑定",1);
			char.setInt(_talkerindex,"每日任务时",os.time());
			char.TalkToCli(_talkerindex, -1, "提交任务完成，获得 "..item.getChar(itemindex,"名称"), 4);
		elseif setid == 3 then--兑换B级宠物蛋
			local token = "                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n兑换【B级奖励证明】需要"
					.."\n        [style c=4]200声望[/style]"
					.."\n        [style c=4]奖励领取证明[/style]"
					.."\n"
					.."\n你确定要兑换吗？！";
			lssproto.windows(_talkerindex, 0, 12, 2, char.getWorkInt( _meindex, "对象"), token);
		elseif setid == 4 then--兑换A级宠物蛋
			local token = "                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n兑换【A级奖励证明】需要"
					.."\n        [style c=4]遗忘的心[/style]"
					.."\n        [style c=4]奖励领取证明[/style]"
					.."\n"
					.."\n你确定要兑换吗？！";
			lssproto.windows(_talkerindex, 0, 12, 3, char.getWorkInt( _meindex, "对象"), token);
			
		end
	elseif (_seqno == 1) then--接受任务
		if char.Finditem(_talkerindex, eventitem) >= 0 then
			char.TalkToCli(_talkerindex, _meindex, "你身上已经有遗失的证明了！", 6);
			return ;
		end
		local mytime = char.getInt(_talkerindex,"每日任务时");
		if os.date("%Y-%m-%d",mytime) == os.date("%Y-%m-%d",os.time()) then
			char.TalkToCli(_talkerindex, _meindex, "你今天已经完成过任务了！", 6);
			return ;
		end
		if char.getInt(_talkerindex,"声望") < 20000 then
			char.TalkToCli(_talkerindex, _meindex, "接受任务需要200声望！", 6);
			return ;
		end
		local itemnum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
		if itemnum < 1 then
			char.TalkToCli(_talkerindex, _meindex, "身上必须有一个道具栏空位", 6);
			return ;
		end
		
		local itemindex = char.Additem( _talkerindex, eventitem);
		item.setInt(itemindex,"绑定",1);
		char.setInt(_talkerindex,"声望",char.getInt(_talkerindex,"声望")-20000);
		char.TalkToCli(_talkerindex, -1, "扣除200声望，获得 "..item.getChar(itemindex,"名称"), 6);
		char.TalkToCli(_talkerindex, _meindex, "接受任务成功！", 4);
	elseif (_seqno == 2) then--兑换B级蛋
		if char.getInt(_talkerindex,"声望") < 20000 then
			char.TalkToCli(_talkerindex, _meindex, "兑换【B级奖励证明】需要200声望！", 6);
			return ;
		end
		if char.Finditem(_talkerindex, 2246) < 0 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有【奖励领取证明】，无法兑换！", 6);
			return ;
		end
		char.setInt(_talkerindex,"声望",char.getInt(_talkerindex,"声望")-20000);
		npc.DelItem(_talkerindex, "2246*1");
		local itemindex = char.Additem( _talkerindex, 2247);
		char.TalkToCli(_talkerindex, -1, "扣除200声望，获得 "..item.getChar(itemindex,"名称"), 4);
	elseif (_seqno == 3) then--兑换A级蛋
		if char.Finditem(_talkerindex, 2650) < 0 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有【遗忘的心】，无法兑换！", 6);
			return ;
		end
		if char.Finditem(_talkerindex, 2246) < 0 then
			char.TalkToCli(_talkerindex, _meindex, "你身上没有【奖励领取证明】，无法兑换！", 6);
			return ;
		end
		npc.DelItem(_talkerindex, "2650*1");
		npc.DelItem(_talkerindex, "2246*1");
		local itemindex = char.Additem( _talkerindex, 2254);
		char.TalkToCli(_talkerindex, -1, "获得 "..item.getChar(itemindex,"名称"), 4);
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		if char.getInt(_talkerindex,"转数") <= 0 then
			char.TalkToCli(_talkerindex, _meindex, "请1转后再来吧！", 6);
			return ;
		end
		local mytime = char.getInt(_talkerindex,"每日任务时");
		local token = "4\n                   [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n你好，每日只可完成一次任务哦！！"
					.."\n[style c=1]上次完成任务时间："..os.date("%Y-%m-%d %X",mytime).."[/style]\n"
					.."\n                   接受每日任务"
					.."\n                   提交每日任务"
					.."\n                 兑换Ｂ级奖励证明"
					.."\n                 兑换Ａ级奖励证明";
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
	eventitem = 2241;
	npcdata = {"每日任务",51296,777,47,43,4,"OneDayEvent"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
