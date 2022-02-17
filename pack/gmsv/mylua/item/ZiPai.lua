function StringSplit(_data, _splitint)-- 分割处理函数（返回数组）
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

function ZiPiaItemCall(itemindex, charaindex, toindex, haveitemindex)
	--1字牌标识
	--2字牌类型(1为道具，2为宠物)
	--3字牌内物件ID
	--4字牌需的字符集合
	ZiPaiList = {{"白虎",2,{1610},{"A","B","C","D"}}
				,{"水暴",2,{1647},{"A","B","C","D"}}
				};
				
	local strdata = item.getChar(itemindex, "字段");--获取道具字段
	--if strdata == "" then--如果字段为空，进行抽取
		--char.TalkToCli(charaindex, -1, "道具参数有误！", 6);
		--return ;
	--end
	--print(strdata);
	local ItemData = other.getString(strdata, "|", 1);--取出道具字段的类型
	local ItemType = other.getString(strdata, "|", 2);--取出道具字段的数值
	if ItemData == "" then--如果字段为空，进行抽取
		char.TalkToCli(charaindex, -1, "道具参数有误！", 6);
		return ;
	end
	if ItemType == "" then--如果字段为空，进行抽取
		char.TalkToCli(charaindex, -1, "道具参数有误！", 6);
		return ;
	end
	local setid = -1;
	for i=1,table.getn(ZiPaiList) do
		if ZiPaiList[i][1] == ItemData then
			setid = i;
			break ;
		end
	end
	local TypeData = {-1,-1,-1,-1};--初始化
	--搜索字牌
	for i=9,23 do
		local tmitemindex = char.getItemIndex(charaindex,i);
		if tmitemindex ~= -1 then
			local tmstrdata = item.getChar(tmitemindex, "字段");
			local tmname = other.getString(tmstrdata, "|", 1);
			local tmtype = other.getString(tmstrdata, "|", 2);
			if tmname == ItemData then--字牌识别名字一致
				for j=1,table.getn(ZiPaiList[setid][4]) do
					if tmtype == ZiPaiList[setid][4][j] then
						if TypeData[j] == -1 then
							TypeData[j] = i;
							break ;
						end
					end
				end
			end
		end
	end
	
	for i=1,table.getn(ZiPaiList[setid][4]) do
		if TypeData[i] == -1 then
			char.TalkToCli(charaindex, -1, "缺乏 "..ItemData.."字牌"..ZiPaiList[setid][4][i].."，使用失败！", 6);
			return ;
		end
	end
	--扣除碎片
	for i=1,table.getn(TypeData) do
		char.DelItem(charaindex, TypeData[i]);	
	end
	local gettype = ZiPaiList[setid][2];
	local randnum = math.random(1,table.getn(ZiPaiList[setid][3]));
	if gettype == 1 then
		local ItemNum = char.GetEmptyItemNum(charaindex);--取得道具栏数量
		if ItemNum < 1 then
			char.TalkToCli(charaindex, -1, "使用该字牌需空出一个道具栏空位！", 6);
			return ;
		end
		local getid = ZiPaiList[setid][3][randnum];
		local itemindex = char.Additem( charaindex, getid);
		char.TalkToCli(charaindex, -1, "获得道具[".. item.getChar(itemindex,"名称") .."]！", 4);
	elseif gettype == 2 then
		local PetNum = char.GetEmptyPetNum(charaindex);--取得宠物栏数量
		if PetNum < 1 then
			char.TalkToCli(charaindex, -1, "使用该字牌需空出一个宠物栏空位！", 6);
			return ;
		end
		local getid = ZiPaiList[setid][3][randnum];
		local petindex = char.AddPet( charaindex, getid,1);
		char.TalkToCli(charaindex, -1, "获得宠物[".. char.getChar(petindex,"名字") .."]！", 4);
	end
end

function main()
	item.addLUAListFunction( "ITEM_ItemZiPai", "ZiPiaItemCall", "");
end
