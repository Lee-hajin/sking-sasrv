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

function ItemOverlapFunction(charindex,fromitemindex,toitemindex,fromid,toid)
	local ItemUse = item.getChar(fromitemindex,"使用函数名");--取得强化石使用函数名
	if ItemUse == "ITEM_UPITEM" then
		local parameter = {charindex,fromitemindex,toitemindex,fromid,toid};
		other.CallFunction("ItemUpLevelFunction", "mylua/item/ItemUpLevel.lua", parameter);
		return 1;
	elseif ItemUse == "ITEM_RandBoxLua" then
		local parameter = {charindex,fromitemindex,toitemindex,fromid,toid};
		other.CallFunction("RandBoxFunction", "mylua/item/RandBox.lua", parameter);
		return 1;
	end
	return 0;
end



function mydata()
	
end

function main()
	mydata();
end

