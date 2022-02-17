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

function GetTableRadn(_TableData, _Rand)--取概率数加入奖品，然后再返回概率列表
	local TmItemList = {};
	for i=1,table.getn(_TableData) do
		if _Rand <= _TableData[i][6] then
			local TmTable = {-1,"",-1,"",""};
			TmTable[1] = _TableData[i][1];
			TmTable[2] = _TableData[i][2];
			TmTable[3] = _TableData[i][3];
			TmTable[4] = _TableData[i][4];
			TmTable[5] = _TableData[i][5];
			table.insert(TmItemList,TmTable);
		end
	end
	return TmItemList;
end

------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if (_seqno == 0) then
		if char.getInt(_talkerindex,"积分") < 200 then
			char.TalkToCli(_talkerindex, _meindex, "开启宝箱需要200积分，您的积分不足！", 6);
			return;
		end
		local ItemNum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
		local PetNum = char.GetEmptyPetNum(_talkerindex);--取得宠物栏数量
		if ItemNum < 1 then
			char.TalkToCli(_talkerindex, _meindex, "打开宝箱必须空出一个道具栏位和一个宠物栏位！", 6);
			return ;
		end
		if PetNum < 1 then
			char.TalkToCli(_talkerindex, _meindex, "打开宝箱必须空出一个道具栏位和一个宠物栏位！", 6);
			return ;
		end
		char.setInt(_talkerindex,"积分",char.getInt(_talkerindex,"积分")-200);
		char.TalkToCli(_talkerindex, -1, "交出200积分！", 4);
		
		local TmItemList = GetTableRadn(ItemList, math.random(1,1000));
		
		local RandId = math.random(1,table.getn(TmItemList));
		local ItemType = TmItemList[RandId][1];--取出奖品类型(1为道具，2为宠物，3为点数，4位字牌)
		local ItemName = TmItemList[RandId][2];
		--print(ItemType);
		if ItemType == 1 then
			local itemindex = char.Additem( _talkerindex, TmItemList[RandId][3]);
			char.TalkToCli(_talkerindex, -1, "获得[".. item.getChar(itemindex,"名称") .."]！", 4);
			char.talkToAllServer("<积分宝箱>恭喜玩家 "
				.. char.getChar(_talkerindex, "名字") .. " 打开积分宝箱获得["
				.. item.getChar(itemindex, "名称") .. "]！",5);
		elseif ItemType == 2 then
			local petindex = char.AddPet( _talkerindex, TmItemList[RandId][3],1);
			char.TalkToCli(_talkerindex, -1, "获得[".. char.getChar(petindex,"名字") .."]！", 4);
			char.talkToAllServer("<积分宝箱>恭喜玩家 "
				.. char.getChar(_talkerindex, "名字") .. " 打开积分宝箱获得["
				.. char.getChar(petindex, "名字") .. "]！",4);
		elseif ItemType == 3 then
		
		elseif ItemType == 4 then
			local itemindex = char.Additem( _talkerindex, TmItemList[RandId][3]);--给予字牌
			local ZiPaiType = math.random(1,4);
			item.setChar(itemindex,"名称",TmItemList[RandId][2]..TypeName[ZiPaiType][1]);
			item.setChar(itemindex,"显示名",TmItemList[RandId][2]..TypeName[ZiPaiType][1]);
			item.setChar(itemindex,"说明",TmItemList[RandId][5]);
			item.setInt(itemindex,"图号",TypeName[ZiPaiType][2]);
			item.setChar(itemindex,"字段",TmItemList[RandId][4].."|"..TypeName[ZiPaiType][1]);
			char.sendStatusString(_talkerindex,"I");
			char.TalkToCli(_talkerindex, -1, "获得[".. item.getChar(itemindex,"名称") .."]！", 4);
			char.talkToAllServer("<积分宝箱>恭喜玩家 "
				.. char.getChar(_talkerindex, "名字") .. " 打开积分宝箱获得["
				.. item.getChar(itemindex, "名称") .. "]！",4);
		end
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "                [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n你好，石器勇士！"
					.."\n"
					.."\n我这就来帮您开启宝箱！"
					.."\n\n\n每次开宝箱扣除[style c=5]200积分[/style]！";
		lssproto.windows(_talkerindex, 0, 12, 0, char.getWorkInt( _meindex, "对象"), token);
	end
end

------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
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
	--1奖品类型(1为道具，2为宠物，3为点数，4位字牌)
	--2奖品名字
	--3奖品编号
	--4参数1(如为点数类型，该字段记录点数类型，如字牌类型，则记录字牌数据索引)
	--5参数2(如为字牌类型，该字段记录字牌说明)
	--6概率(1000内随机数)
	ItemList = {{1,"珊瑚装备箱Lv3",2960,"","",800}
			   ,{2,"机豹",1946,"","",800}
			   ,{1,"机暴骑乘证",20296,"","",1000}
			   ,{1,"500声望券",22911,"","",1000}
			   ,{1,"饲养丛林门票",2728,"","",1000}
			   ,{4,"白虎字牌",2648,"白虎","凑齐白虎A、B、C、D四块字牌，双击使用即可获得一只Lv1的佩露夏！",800}
			   ,{1,"黑飞暴骑乘同意书",3305,"","",600}
			   --,{2,"风人狼",1978,"","",400}
			   --,{2,"水人狼",1976,"","",100}
			   --,{2,"地人狼",1975,"","",400}
			   --,{2,"火人狼",1977,"","",100}
			   --,{1,"快速就职门票",2744,"","",600}
			   --,{1,"月光装备套",2746,"","",500}
			   --,{2,"粉红人龙",1588,"","",600}
			   --,{2,"粉蓝人龙",1589,"","",600}
			   --,{2,"虎鲸-夏克",2703,"","",100}
			   --,{2,"虎鲸-夏基",2704,"","",100}
			   };
	TypeName = {{"A",24597}
				,{"B",24598}
				,{"C",24599}
				,{"D",24600}
				};
	npcdata = {"Max积分宝箱",51014,777,47,37,4,"RandBox2"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
