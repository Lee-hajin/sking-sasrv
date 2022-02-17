function lua_string_split(str, split_char)
    local sub_str_tab = {};
    while (true) do
        local pos = string.find(str, split_char);
        if (not pos) then
            sub_str_tab[#sub_str_tab + 1] = str;
            break;
        end
        local sub_str = string.sub(str, 1, pos - 1);
        sub_str_tab[#sub_str_tab + 1] = sub_str;
        str = string.sub(str, pos + 1, #str);
    end

    return sub_str_tab;
end

function GetTableRand(_TableData, _Rand)--取概率数加入奖品，然后再返回概率列表
	local TmItemList = {};
	for i=1,table.getn(_TableData) do
		if _Rand <= _TableData[i][3] then
			local TmTable = _TableData[i];
			table.insert(TmItemList,TmTable);
		end
	end
	return TmItemList;
end


function RandBoxFunction(charindex,fromitemindex,toitemindex,fromid,toid)
	local itemnum = char.GetEmptyItemNum(charindex);--取得道具栏数量
	local petnum = char.GetEmptyPetNum(charindex);--取得宠物栏数量
	
	local ItemUse = item.getChar(fromitemindex,"使用函数名");--取得强化石使用函数名
	if ItemUse ~= "ITEM_RandBoxLua" then
		char.TalkToCli(charindex, -1,"道具不是宝箱或者宝箱钥匙，无法使用!", 6);
		return ;
	end
	local ItemUse = item.getChar(toitemindex,"使用函数名");--取得强化石使用函数名
	if ItemUse ~= "ITEM_RandBoxLua" then
		char.TalkToCli(charindex, -1,"道具不是宝箱或者宝箱钥匙，无法使用!", 6);
		return ;
	end
	
	local strdata = item.getChar(toitemindex, "字段");
	if strdata == "测试宝箱" then
		if itemnum < 1 then
			char.TalkToCli(charindex, -1, "请先空出1个道具栏和1个宠物栏！", 6);
			return ;
		end
		if petnum < 1 then
			char.TalkToCli(charindex, -1, "请先空出1个道具栏和1个宠物栏！", 6);
			return ;
		end
		--奖品列表(1奖品类型(1道具,2宠物),2奖品编号,3奖品概率,4是否公告,5是否绑定)
		local ItemList = {{1,1,100,true,false}
						 ,{1,2,100,false,true}
						 ,{2,1,100,false,true}
						 };
		local TmItemList = GetTableRand(ItemList,math.random(100));
		local RandId = math.random(1,#TmItemList);
		
		local ItemType = TmItemList[RandId][1];--取出奖品类型(1为道具，2为宠物)
		local GiveName = "";
		if ItemType == 1 then
			local Tmindex = char.Additem( charindex, TmItemList[RandId][2]);
			if TmItemList[RandId][5] == true then
				item.setInt(Tmindex,"绑定",1);
				item.UpdataItemOne(charindex, Tmindex);
			end
			GiveName = item.getChar(Tmindex,"名称");
			char.TalkToCli(charindex, -1, "获得[".. item.getChar(Tmindex,"名称") .."]！", 4);
		elseif ItemType == 2 then
			local Tmindex = char.AddPet( charindex, TmItemList[RandId][2],1);
			if TmItemList[RandId][5] == true then
				char.setInt(Tmindex,"绑定",1);
				for i=0,4 do
					char.sendStatusString(charindex, "K" .. i);
				end
			end
			GiveName = char.getChar(Tmindex,"名字");
			char.TalkToCli(charindex, -1, "获得[".. char.getChar(Tmindex,"名字") .."]！", 4);
		end
		if TmItemList[RandId][4] == true then
			char.talkToAllServer("<测试宝箱>恭喜玩家 "
				.. char.getChar(charindex, "名字") .. " 打开 测试宝箱 获得["
				.. GiveName .. "]！",4);
		end
		char.DelItem(charindex, fromid);
		char.DelItem(charindex, toid);
	end
end

function mydata()
	
end

function main()
	mydata();
end
