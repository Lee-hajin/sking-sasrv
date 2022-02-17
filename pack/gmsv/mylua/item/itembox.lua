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
	print("开始统计达标奖励(".._Rand..")...");
	local TmItemList = {};
	for i=1,table.getn(_TableData) do
		if _Rand <= _TableData[i][6] then
			if table.getn(_TableData[i][3]) == 2 then
				for k = _TableData[i][3][1],_TableData[i][3][2] do
					local TmTable = {-1,"",-1,"","",false};
					TmTable[1] = _TableData[i][1];
					TmTable[2] = _TableData[i][2];
					TmTable[3] = k;
					TmTable[4] = _TableData[i][4];
					TmTable[5] = _TableData[i][5];
					TmTable[6] = _TableData[i][7];
					table.insert(TmItemList,TmTable);
				end
			elseif table.getn(_TableData[i][3]) > 2 then
				for k=1,table.getn(_TableData[i][3]) do
					local TmTable = {-1,"",-1,"","",false};
					TmTable[1] = _TableData[i][1];
					TmTable[2] = _TableData[i][2];
					TmTable[3] = _TableData[i][3][k];
					print("加入".._TableData[i][3][k])
					TmTable[4] = _TableData[i][4];
					TmTable[5] = _TableData[i][5];
					TmTable[6] = _TableData[i][7];
					table.insert(TmItemList,TmTable);
				end
			else
				local TmTable = {-1,"",-1,"","",false};
				TmTable[1] = _TableData[i][1];
				TmTable[2] = _TableData[i][2];
				TmTable[3] = _TableData[i][3][1];
				TmTable[4] = _TableData[i][4];
				TmTable[5] = _TableData[i][5];
				TmTable[6] = _TableData[i][7];
				table.insert(TmItemList,TmTable);
			end
			
		end
	end
	print("共达标奖励数="..table.getn(TmItemList));
	return TmItemList;
end

--真·打乱排序
function RandItemList(itemlist)
	local TmItemList={};
	while table.getn(itemlist) > 0 do
		local n = math.random(1,table.getn(itemlist));
		if itemlist[n] ~= nil then
			table.insert(TmItemList,itemlist[n]);
			table.remove(itemlist,n);
		end
	end
	return TmItemList;
end

function ItemBox(itemindex, charaindex, toindex, haveitemindex)
	local strdata = item.getChar(itemindex, "字段");
	local itemnum = char.GetEmptyItemNum(charaindex);--取得道具栏数量
	local petnum = char.GetEmptyPetNum(charaindex);--取得宠物栏数量
	if strdata == "珊瑚装备1" then
		if itemnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏位！", 6);
			return ;
		end
		local ItemList = {1877,1878,1879,1880,1881--苍绿1
						,1877,1878,1879,1880,1881--苍绿1
						--,1888,1889,1890,1891,1892--湛蓝1
						--,1893,1894,1895,1896,1897--焰红1
						,1898,1899,1906,1907,1908--岚黄1
						};
		local TmItemList = RandItemList(ItemList);--打乱排序
		local getid = TmItemList[math.random(1,table.getn(TmItemList))];
		local getindex = char.Additem( charaindex, getid);
		char.TalkToCli(charaindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "珊瑚装备2" then
		if itemnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏位！", 6);
			return ;
		end
		local ItemList = {1909,1910,1911,1912,1913--苍绿2
						,1909,1910,1911,1912,1913--苍绿2
						--,1914,1915,1916,1917,1918--湛蓝2
						--,1919,1920,1921,1922,1923--焰红2
						,1924,1925,1926,1927,1928--岚黄2
						};
		local TmItemList = RandItemList(ItemList);--打乱排序
		local getid = TmItemList[math.random(1,table.getn(TmItemList))];
		local getindex = char.Additem( charaindex, getid);
		char.TalkToCli(charaindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "珊瑚装备3" then
		if itemnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏位！", 6);
			return ;
		end
		local ItemList = {1929,1942,1943,1944,1945--苍绿3
						,1929,1942,1943,1944,1945--苍绿3
						,1946,1947,1948,1949,1950--湛蓝3
						,1951,1952,1953,1954,1955--焰红3
						,1956,1957,1958,1959,1960--岚黄3
						};
		local TmItemList = RandItemList(ItemList);--打乱排序
		local getid = TmItemList[math.random(1,table.getn(TmItemList))];
		local getindex = char.Additem( charaindex, getid);
		char.TalkToCli(charaindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "智慧果包" then
		if itemnum < 12 then
			char.TalkToCli(charaindex, -1, "请先空出12个道具栏位！", 6);
			return ;
		end
		for i=1,12 do
			local getindex = char.Additem( charaindex, 20579);
			item.setInt(getindex,"绑定",1);
			item.UpdataItemOne(charaindex, getindex);
		end
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "随机珊瑚包" then
		local ItemList = {1877,1878,1879,1880,1881--苍绿1
						,1888,1889,1890,1891,1892--湛蓝1
						,1877,1878,1879,1880,1881--苍绿1
						,1888,1889,1890,1891,1892--湛蓝1
						,1877,1878,1879,1880,1881--苍绿1
						,1888,1889,1890,1891,1892--湛蓝1
						,1909,1910,1911,1912,1913--苍绿2
						,1914,1915,1916,1917,1918--湛蓝2
						,1909,1910,1911,1912,1913--苍绿2
						,1914,1915,1916,1917,1918--湛蓝2
						,1929,1942,1943,1944,1945--苍绿3
						,1946,1947,1948,1949,1950--湛蓝3
						};
		local TmItemList = RandItemList(ItemList);--打乱排序
		local getid = TmItemList[math.random(1,table.getn(TmItemList))];
		local getindex = char.Additem( charaindex, getid);
		item.setInt(getindex,"绑定",1);
		item.UpdataItemOne(charaindex, getindex);
		char.TalkToCli(charaindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "练级冲级礼包" then
		if itemnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏位！", 6);
			return ;
		end
		--local getindex = char.Additem( charaindex, 3402);
		--item.setInt(getindex,"绑定",1);
		--item.UpdataItemOne(charaindex, getindex);
		local ItemList = {2961--来吉卡
						,20296--机暴证
						,20224,20225,20226,20227,20228,21592,21593
						};
		local TmItemList = RandItemList(ItemList);--打乱排序
		local getid = TmItemList[math.random(1,table.getn(TmItemList))];
		getindex = char.Additem( charaindex, getid);
		--item.setInt(getindex,"绑定",1);
		--item.UpdataItemOne(charaindex, getindex);
		char.TalkToCli(charaindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "幸运星礼包" then
		if itemnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏位！", 6);
			return ;
		end		
		local ItemList = {3403--称号
						,3308,3309,3310,3311--机暴证
						};
		local getid = ItemList[math.random(1,table.getn(ItemList))];
		local getindex = char.Additem( charaindex, getid);
		char.TalkToCli(charaindex, -1, "获得[".. item.getChar(getindex,"名称") .."]!!!", "黄色");
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "充值福袋" then
		--骰子模版 12764
		local TmMotoList = {24298,24299,24300,24301,24302,24303};
		local getindex = char.Additem( charaindex, 12764);
		item.setInt(getindex,"绑定",1);
		local RandNum = math.random(1,6);
		
		item.setChar(getindex,"名称","骰子"..RandNum);
		item.setChar(getindex,"显示名","骰子"..RandNum);
		item.setChar(getindex,"说明","点数为"..RandNum.."点的骰子！");
		item.setInt(getindex,"图号",TmMotoList[RandNum]);
		item.UpdataItemOne(charaindex, getindex);
		char.TalkToCli(charaindex, -1, "获得点数为[".. RandNum .."]点的骰子！", "黄色");
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "B级奖励证明" then
		if itemnum < 1 then
			char.TalkToCli(charaindex,-1,"请先空出1个道具栏和1个宠物栏！",6);
			return ;
		end
		if petnum < 1 then
			char.TalkToCli(charaindex,-1,"请先空出1个道具栏和1个宠物栏！",6);
			return ;
		end
		local DatList = {{1,20272,0}--铜铠
						,{1,20275,0}--铜兜
						,{1,20278,0}--铁斧
						,{1,20281,0}--铁棍
						,{1,20284,0}--铁枪
						,{1,2650,0}--遗忘的心
						,{1,18076,0}--环9
						,{1,18077,0}--环9
						,{1,18078,0}--环9
						,{1,18080,0}--环9
						,{1,18084,0}--环9
						,{1,18085,0}--环9
						,{1,18090,0}--环9
						};
		local RandNum = math.random(1,table.getn(DatList));
		local GetType = DatList[RandNum][1];
		local GetId = DatList[RandNum][2];
		local GetLock = DatList[RandNum][3];
		local GetName = "";
		if GetType == 1 then--给道具
			local GetIndex = char.Additem( charaindex, GetId);
			if GetLock == 1 then
				item.setInt(GetIndex,"绑定",1);
			end
			GetName = item.getChar(GetIndex,"名称");
		else--给宠物
			local GetIndex = char.AddPet( charaindex, GetId,1);
			if GetLock == 1 then
				char.setInt(GetIndex,"绑定",1);
			end
			GetName = char.getChar(GetIndex,"名字");
		end
		char.TalkToCli(charaindex, -1, "获得 ".. GetName .."！", 4);				
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "A级奖励证明" then
		if itemnum < 1 then
			char.TalkToCli(charaindex,-1,"请先空出1个道具栏和1个宠物栏！",6);
			return ;
		end
		if petnum < 1 then
			char.TalkToCli(charaindex,-1,"请先空出1个道具栏和1个宠物栏！",6);
			return ;
		end
		local DatList = {{2,1558,0}--扑满乌力
						,{1,2647,0}--宝箱钥匙
						,{2,1978,0}--风人狼
						,{2,1976,0}--水人狼
						,{2,1975,0}--地人狼
						,{2,1977,0}--火人狼
						,{2,2245,0}--地蝎子
						,{2,2246,0}--水蝎子
						,{2,2247,0}--火蝎子
						,{2,2248,0}--风蝎子
						,{2,5025,0}--黑飞暴
						,{2,5016,0}--虎龙
						,{2,5017,0}--虎龙
						,{2,5018,0}--虎龙
						,{2,5019,0}--虎龙
						,{2,1804,0}--小GP
						,{2,1926,0}--大GP
						,{2,1645,0}--金暴
						};
		local RandNum = math.random(1,table.getn(DatList));
		local GetType = DatList[RandNum][1];
		local GetId = DatList[RandNum][2];
		local GetLock = DatList[RandNum][3];
		local GetName = "";
		if GetType == 1 then--给道具
			local GetIndex = char.Additem( charaindex, GetId);
			if GetLock == 1 then
				item.setInt(GetIndex,"绑定",1);
			end
			GetName = item.getChar(GetIndex,"名称");
		else--给宠物
			local GetIndex = char.AddPet( charaindex, GetId,1);
			if GetLock == 1 then
				char.setInt(GetIndex,"绑定",1);
			end
			GetName = char.getChar(GetIndex,"名字");
		end
		char.TalkToCli(charaindex, -1, "获得 ".. GetName .."！", 4);				
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "猪年吉祥" then
		local ItemList = {{1,"扬奇洛斯骑证",{20226},"","",100,false}
						,{1,"卡卡金宝骑证",{20227},"","",100,false}
						,{1,"邦浦洛斯骑证",{3301},"","",100,false}
						,{1,"邦奇诺骑证",{3302},"","",100,false}
						,{1,"利则诺顿骑证",{3303},"","",100,false}
						,{1,"布鲁顿骑证",{3304},"","",100,false}
						,{1,"朵拉比斯骑证",{20228},"","",100,false}
						,{1,"拉奇鲁哥骑证",{20224},"","",100,false}
						,{1,"卡达鲁卡斯骑证",{20225},"","",100,false}
						,{1,"史卡鲁骑证",{21592},"","",100,false}
						,{1,"尚青啤酒",{20599},"","",80,false}
						,{1,"鸡尾酒",{20600},"","",80,false}
						,{1,"合成手环 9",{18076,18090},"","",70,false}
						,{1,"合成手环 10",{18091,18105},"","",40,true}
						,{1,"珊瑚1",{1877,1878,1879,1880,1881,1888,1889,1890,1891,1892,1893,1894,1895,1896,1897,1898,1899,1906,1907,1908},"","",60,false}
						,{1,"1级MM",{2955,2956},"","",50,false}
						,{1,"魔兽蛋蛋",{3902},"","",50,true}
						,{1,"月光散件",{20595,20597},"","",50,false}
						};
		local TmItemList = GetTableRand(ItemList,math.random(100));
		local RandId = math.random(1,table.getn(TmItemList));
		local ItemType = TmItemList[RandId][1];--取出奖品类型(1为道具，2为宠物，3位字牌)
		if itemnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏和1个宠物栏！", 6);
			return ;
		end
		if petnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏和1个宠物栏！", 6);
			return ;
		end
		local GiveName = "";
		if ItemType == 1 then
			local Tmitemindex = char.Additem( charaindex, TmItemList[RandId][3]);
			GiveName = item.getChar(Tmitemindex,"名称");
			char.TalkToCli(charaindex, -1, "获得[".. item.getChar(Tmitemindex,"名称") .."]！", 4);
		elseif ItemType == 2 then
			local petindex = char.AddPet( charaindex, TmItemList[RandId][3],1);
			GiveName = char.getChar(petindex,"名字");
			char.TalkToCli(charaindex, -1, "获得[".. char.getChar(petindex,"名字") .."]！", 4);
		elseif ItemType == 3 then
			local Tmitemindex = char.Additem( charaindex, TmItemList[RandId][3]);--给予字牌
			local ZiPaiType = math.random(1,4);
			item.setChar(Tmitemindex,"名称",TmItemList[RandId][2]..TypeName[ZiPaiType][1]);
			item.setChar(Tmitemindex,"显示名",TmItemList[RandId][2]..TypeName[ZiPaiType][1]);
			item.setChar(Tmitemindex,"说明",TmItemList[RandId][5]);
			item.setInt(Tmitemindex,"图号",TypeName[ZiPaiType][2]);
			item.setChar(Tmitemindex,"字段",TmItemList[RandId][4].."|"..TypeName[ZiPaiType][1]);
			char.sendStatusString(charaindex,"I");
			GiveName = item.getChar(Tmitemindex,"名称");
			char.TalkToCli(charaindex, -1, "获得[".. item.getChar(Tmitemindex,"名称") .."]！", 4);
		end
		if TmItemList[RandId][6] == true then
			char.talkToAllServer("<新春礼包>恭喜玩家 "
				.. char.getChar(charaindex, "名字") .. " 打开 猪年吉祥礼包 获得["
				.. GiveName .. "]！",4);
		end
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "猪年幸运" then
		local ItemList = {{1,"驯兽手环",{19549},"","",100,true}
						,{1,"神霄手环",{23019},"","",100,true}
						,{1,"云阶手环",{23020},"","",100,true}
						,{1,"金刚手环",{23021},"","",100,true}
						,{1,"随机声望100~300",{23022},"","",100,false}
						,{1,"愤怒罐头",{21787},"","",100,true}
						,{1,"背3罐头",{22658},"","",100,true}
						,{1,"2D人龙",{12952},"","",5,true}
						,{2,"乌金贝得",{5029},"","",10,true}
						};
		local TmItemList = GetTableRand(ItemList,math.random(100));
		local RandId = math.random(1,table.getn(TmItemList));
		local ItemType = TmItemList[RandId][1];--取出奖品类型(1为道具，2为宠物，3位字牌,4货币)
		if itemnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏和1个宠物栏！", 6);
			return ;
		end
		if petnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个道具栏和1个宠物栏！", 6);
			return ;
		end
		local GiveName = "";
		if ItemType == 1 then
			local Tmitemindex = char.Additem( charaindex, TmItemList[RandId][3]);
			GiveName = item.getChar(Tmitemindex,"名称");
			char.TalkToCli(charaindex, -1, "获得[".. item.getChar(Tmitemindex,"名称") .."]！", 4);
		elseif ItemType == 2 then
			local petindex = char.AddPet( charaindex, TmItemList[RandId][3],1);
			GiveName = char.getChar(petindex,"名字");
			char.TalkToCli(charaindex, -1, "获得[".. char.getChar(petindex,"名字") .."]！", 4);
		elseif ItemType == 3 then
			local Tmitemindex = char.Additem( charaindex, TmItemList[RandId][3]);--给予字牌
			local ZiPaiType = math.random(1,4);
			item.setChar(Tmitemindex,"名称",TmItemList[RandId][2]..TypeName[ZiPaiType][1]);
			item.setChar(Tmitemindex,"显示名",TmItemList[RandId][2]..TypeName[ZiPaiType][1]);
			item.setChar(Tmitemindex,"说明",TmItemList[RandId][5]);
			item.setInt(Tmitemindex,"图号",TypeName[ZiPaiType][2]);
			item.setChar(Tmitemindex,"字段",TmItemList[RandId][4].."|"..TypeName[ZiPaiType][1]);
			char.sendStatusString(charaindex,"I");
			GiveName = item.getChar(Tmitemindex,"名称");
			char.TalkToCli(charaindex, -1, "获得[".. item.getChar(Tmitemindex,"名称") .."]！", 4);
		end
		if TmItemList[RandId][6] == true then
			char.talkToAllServer("<新春礼包>恭喜玩家 "
				.. char.getChar(charaindex, "名字") .. " 打开 猪年幸运礼包 获得["
				.. GiveName .. "]！",4);
		end
		char.DelItem(charaindex, haveitemindex);
	elseif strdata == "2D人龙" then
		if petnum < 1 then
			char.TalkToCli(charaindex, -1, "请先空出1个宠物栏！", 6);
			return ;
		end
		local petlist = {5020,5021,5022,5023,5024};
		local RandId = math.random(1,table.getn(petlist));
		local Tmpetindex = char.AddPet( charaindex, petlist[RandId],1);
		char.setInt(Tmpetindex,"转数",2);
		for i = 0, 4 do
			if Tmpetindex == char.getCharPet(charaindex, i) then
				char.sendStatusString(charaindex, "K" .. i);
				break;
			end
		end
		char.TalkToCli(charaindex, -1, "获得[".. char.getChar(Tmpetindex,"名字") .."]！", 4);
		char.DelItem(charaindex, haveitemindex);
	else
		char.TalkToCli(charaindex, -1, "该礼包暂未开放！", 6);
		return ;
	end
end

function data()
end

function main()
	item.addLUAListFunction( "ITEM_BOX", "ItemBox", "");
end
