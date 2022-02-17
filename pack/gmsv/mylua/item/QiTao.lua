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

function ItemQiTao(itemindex, charaindex, toindex, haveitemindex)
	local itemdata = item.getChar(itemindex, "字段");
	local strdata = StringSplit(itemdata,"|");
	if table.getn(strdata) < 2 then
		char.TalkToCli(charaindex, -1, "道具参数有误！", 6);
		return ;
	end
	local itemnum = char.GetEmptyItemNum(charaindex);--取得道具栏数量
	local petnum = char.GetEmptyPetNum(charaindex);--取得宠物栏数量
	if itemnum < 1 then
		char.TalkToCli(charaindex, -1, "请先空出1个道具栏位！", 6);
		return ;
	end
	if petnum < 1 then
		char.TalkToCli(charaindex, -1, "请先空出1个宠物栏位！", 6);
		return ;
	end
	local getpet = char.AddPet( charaindex, strdata[1],1);
	char.TalkToCli(charaindex, -1, "获得 ".. char.getChar(getpet, "名字") .." !", "黄色");
	local getitem = char.Additem(charaindex,strdata[2]);
	char.TalkToCli(charaindex, -1, "获得[".. item.getChar(getitem,"名称") .."]!!!", "黄色");
	char.DelItem(charaindex, haveitemindex);
end

function main()
	item.addLUAListFunction( "ITEM_QiTao", "ItemQiTao", "");
end
