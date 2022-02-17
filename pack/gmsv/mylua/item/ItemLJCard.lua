--真·打乱排序
function RandTableList(TableList)
	local TmTableList={};
	while table.getn(TableList) > 0 do
		local n = math.random(1,table.getn(TableList));
		if TableList[n] ~= nil then
			table.insert(TmTableList,TableList[n]);
			table.remove(TableList,n);
		end
	end
	return TmTableList;
end

function GetTableRadn(_TableData, _Rand)--取概率数加入奖品，然后再返回概率列表
	local TmItemList = {};
	for i=1,table.getn(_TableData) do
		if _Rand <= _TableData[i][3] then
			local TmTableList = {-1,-1};
			TmTableList[1] = _TableData[i][1];
			TmTableList[2] = _TableData[i][2];
			table.insert(TmItemList,TmTableList);
		end
	end
	TmItemList = RandTableList(TmItemList);
	return TmItemList;
end

function ItemLJCard(itemindex, charaindex, toindex, haveitemindex)
	local ItemStr = item.getChar(itemindex, "字段");
	--print(ItemStr);
	local ItemType = other.getString(ItemStr, "|", 1);
	local ItemLv = other.atoi(other.getString(ItemStr, "|", 2));
	
	local MyItemNum = char.GetEmptyItemNum(charaindex);--取得道具栏数量
	local MyPetNum = char.GetEmptyPetNum(charaindex);--取得宠物栏数量
	if ItemType == "来吉卡第一弹" then
		if MyPetNum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出一个宠物栏位！", 6);
			return ;
		end
		--1类型(1道具,2宠物),2编号,3概率
		local PetList = {{2,1979,600}--地魔兽
						,{2,1980,600}--火魔兽
						,{2,2057,200}--水魔兽
						,{2,2058,800}--风魔兽
						,{2,1975,800}--地狼人
						,{2,1976,800}--水狼人
						,{2,1977,600}--火狼人
						,{2,1978,600}--风狼人
						,{2,1804,1000}--小GP
						,{2,1926,1000}--大GP
						,{2,2066,1000}--地龙蜥
						,{2,1947,1000}--蒙奇
						,{2,2212,1000}--波比
						,{2,1647,800}--水暴
						,{2,1642,300}--黑鸡
						,{2,2560,600}--地水蜜蜂
						,{2,2559,300}--火风蜜蜂
						,{2,2490,500}--狂暴火牛
						,{2,2558,500}--布里萨尔
						,{2,2571,300}--火牛人
						,{2,5025,300}--黑飞暴
						,{2,2572,600}--地牛人
						,{2,2561,300}--巴雅蝴蝶
						,{2,2562,300}--巴特蝴蝶
						,{1,3305,800}--黑飞暴骑证
						};
		local RandNum = math.random(1,1000);
		local TmPetList = GetTableRadn(PetList, RandNum);
		local RandId = math.random(1,table.getn(TmPetList));
		local GetRandType = TmPetList[RandId][1];
		local GetRandId = TmPetList[RandId][2];
		
		local DelName = item.getChar(itemindex,"名称");
		local GetName = "";
		char.DelItem(charaindex, haveitemindex);
		if GetRandType == 1 then--给道具
			local GetIndex = char.Additem( charaindex, GetRandId);
			GetName = item.getChar(GetIndex,"名称");
		else--给宠物
			local GetIndex = char.AddPet( charaindex, GetRandId,1);
			GetName = char.getChar(GetIndex,"名字");
		end
		char.TalkToCli(charaindex, -1, "交出 ".. DelName .."！", 4);
		char.TalkToCli(charaindex, -1, "获得 ".. GetName .."！", 4);
		char.talkToAllServer("<"..ItemType..">恭喜玩家 "
				.. char.getChar(charaindex, "名字") .. " 打开 "..DelName.." 幸运获得["
				.. GetName .. "]！",4);
	else
		char.TalkToCli(charaindex, -1, "该礼包暂未开放！", 6);
		return ;
	end
	
end

function mydata()
end

function main()
	item.addLUAListFunction( "ITEM_LJCard", "ItemLJCard", "");
end
