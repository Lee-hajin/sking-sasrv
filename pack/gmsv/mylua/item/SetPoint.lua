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

function SetPoint(itemindex, charaindex, toindex, haveitemindex)
	local itemdata = item.getChar(itemindex, "字段");
	local field = other.getString(itemdata, "|", 1);
	local pointmin = other.atoi(other.getString(itemdata, "|", 2));
	local pointmax = other.atoi(other.getString(itemdata, "|", 3));
	local getpoint = 0;
	if pointmax == 0 then
		getpoint = pointmin;
	else
		getpoint = math.random(pointmin,pointmax);
	end
	if field == "金币" then
		local mypoint = sasql.getVipPoint(charaindex);
		sasql.setVipPoint(charaindex,mypoint+getpoint);
		char.TalkToCli(charaindex, -1, "恭喜你获得" .. getpoint.."点金币！", "黄色");
	elseif field == "声望" then
		local mypoint = char.getInt(charaindex, "声望");
		char.setInt(charaindex, "声望",mypoint+(getpoint*100));
		char.TalkToCli(charaindex, -1, "恭喜你获得" .. getpoint.."点声望！", "黄色");
	else
		return ;
	end
	
	char.DelItem(charaindex, haveitemindex);
end

function main()
	item.addLUAListFunction( "ITEM_SetPoint", "SetPoint", "");
end
