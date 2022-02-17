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
	if _select == 8 then
		return;
	end
	if (_seqno == 0) then
		if npc.Free(_meindex, _talkerindex, "ITEM=2647") == 0 then
			char.TalkToCli(_talkerindex, _meindex, "很抱歉，您身上没有宝箱钥匙！", 6);
			return;
		end
		if char.getInt(_talkerindex,"声望") < 10000 then
			char.TalkToCli(_talkerindex, _meindex, "开启宝箱需要100声望，您的声望不足！", 6);
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
		npc.DelItem(_talkerindex,"2647*1");
		char.setInt(_talkerindex,"声望",char.getInt(_talkerindex,"声望")-10000);--扣除5声望
		
		char.TalkToCli(_talkerindex, -1, "交出100声望！", 4);
		
		local TmItemList = GetTableRadn(ItemList, math.random(1,1000));
		local RandId = math.random(1,table.getn(TmItemList));
		local ItemType = TmItemList[RandId][1];--取出奖品类型(1为道具，2为宠物，3为点数，4位字牌)
		local ItemName = TmItemList[RandId][2];
		--print(ItemType);
		if ItemType == 1 then
			local itemindex = char.Additem( _talkerindex, TmItemList[RandId][3]);
			char.TalkToCli(_talkerindex, -1, "获得[".. item.getChar(itemindex,"名称") .."]！", 4);
		elseif ItemType == 2 then
			local petindex = char.AddPet( _talkerindex, TmItemList[RandId][3],1);
			char.TalkToCli(_talkerindex, -1, "获得[".. char.getChar(petindex,"名字") .."]！", 4);
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
		end
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "                [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n你好，石器勇士！"
					.."\n想要开宝箱吗？请准备好钥匙..."
					.."\n我这就来帮您开启宝箱！"
					.."\n\n\n每次开宝箱扣除一把[style c=5]宝箱钥匙[/style]及[style c=5]100点声望[/style]！";
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
	TypeName = {{"A",24597}
				,{"B",24598}
				,{"C",24599}
				,{"D",24600}
				};
	ItemList = {{1,"珊瑚装备箱Lv3",2960,"","",800}
			   ,{1,"黑飞暴骑乘同意书",3305,"","",500}
			   ,{1,"水虎龙骑乘同意书",3308,"","",300}
			   ,{1,"地虎龙骑乘同意书",3309,"","",300}
			   ,{1,"火虎龙骑乘同意书",3310,"","",300}
			   ,{1,"风虎龙骑乘同意书",3311,"","",300}
			   ,{1,"机暴骑乘证",20296,"","",700}
			   ,{2,"水暴",1647,"","",500}
			   ,{2,"机暴",353,"","",600}
			   ,{2,"黑飞暴",5025,"","",300}
			   ,{1,"流星手套",2056,"","",500}
			   ,{1,"驯兽手环",19549,"","",500}
			   ,{1,"合成手环10",21651,"","",1000}
			   ,{1,"合成手环10",21652,"","",1000}
			   ,{1,"合成手环10",21653,"","",1000}
			   ,{1,"200声望券",22908,"","",1000}
			   ,{1,"300声望券",22909,"","",700}
			   ,{1,"剧毒罐头",22858,"","",600}
			   ,{4,"白虎字牌",2648,"白虎","凑齐白虎A、B、C、D四块字牌，双击使用即可获得一只Lv1的佩露夏！",400}
				};
	npcdata = {"Max宝箱",51011,777,47,39,6,"RandBox1"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
