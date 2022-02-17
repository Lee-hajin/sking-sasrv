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

function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end

    if math.ceil(x) == x then
       x = math.ceil(x);
    else
       x = math.ceil(x) - 1;
    end
    return x;
end

function SetPlayerPoint(itemindex, charaindex, toindex, haveitemindex)
	local itemdata = item.getChar(itemindex, "字段");
	local field = other.getString(itemdata, "|", 1);
	local point = other.atoi(other.getString(itemdata, "|", 2));
	local setpoint = point;
	local mypoint = char.getInt(charaindex, "技能点");
	local vital = getIntPart(char.getInt(charaindex, "体力")  / 100);
	local str = getIntPart(char.getInt(charaindex, "腕力") / 100);
	local tgh = getIntPart(char.getInt(charaindex, "耐力") / 100);
	local dex = getIntPart(char.getInt(charaindex, "速度") / 100);
	local sum = vital + str + tgh + dex + mypoint;
	if field == "血" then
		if vital <= 5 then
			char.TalkToCli(charaindex, -1, "您的体力点数小于5，无需重置!", 6);
			return ;
		end
		if setpoint <= 0 then
			setpoint = vital;
		elseif setpoint > vital then
			setpoint = vital;
		end
		
		local tmsum = vital + str + tgh + dex -setpoint;
		if tmsum <= 0 then
			setpoint = setpoint - 5;
		end
		char.setInt(charaindex, "体力",(vital-setpoint) * 100);
		char.setInt(charaindex, "技能点",mypoint+setpoint);
		char.TalkToCli(charaindex, -1, "体力发生变化!", 4);
	elseif field == "攻" then
		if str <= 5 then
			char.TalkToCli(charaindex, -1, "您的腕力点数小于5，无需重置!", 6);
			return ;
		end
		
		if setpoint <= 0 then
			setpoint = str;
		elseif setpoint > str then
			setpoint = str;
		end
		local tmsum = vital + str + tgh + dex -setpoint;
		if tmsum <= 0 then
			setpoint = setpoint - 5;
		end
		
		char.setInt(charaindex, "腕力",(str-setpoint) * 100);
		char.setInt(charaindex, "技能点",mypoint+setpoint);
		char.TalkToCli(charaindex, -1, "腕力发生变化!", 4);
		
	elseif field == "防" then
		if tgh <= 5 then
			char.TalkToCli(charaindex, -1, "您的耐力点数小于5，无需重置!", 6);
			return ;
		end
		if setpoint <= 0 then
			setpoint = tgh;
		elseif setpoint > tgh then
			setpoint = tgh;
		end
		
		local tmsum = vital + str + tgh + dex -setpoint;
		if tmsum <= 0 then
			setpoint = setpoint - 5;
		end
		char.setInt(charaindex, "耐力",(tgh-setpoint) * 100);
		char.setInt(charaindex, "技能点",mypoint+setpoint);
		char.TalkToCli(charaindex, -1, "耐力发生变化!", 4);
	elseif field == "敏" then
		if dex <= 5 then
			char.TalkToCli(charaindex, -1, "您的速度点数小于5，无需重置!", 6);
			return ;
		end
		if setpoint <= 0 then
			setpoint = dex;
		elseif setpoint > dex then
			setpoint = dex;
		end
		local tmsum = vital + str + tgh + dex -setpoint;
		if tmsum <= 0 then
			setpoint = setpoint - 5;
		end
		char.setInt(charaindex, "速度",(dex-setpoint) * 100);
		char.setInt(charaindex, "技能点",mypoint+setpoint);
		char.TalkToCli(charaindex, -1, "速度发生变化!", 4);
	elseif field == "全" then
		char.setInt(charaindex, "体力",1000);
		char.setInt(charaindex, "腕力",0);
		char.setInt(charaindex, "耐力",0);
		char.setInt(charaindex, "速度",0);
		char.setInt(charaindex, "技能点",sum-10);
		char.TalkToCli(charaindex, -1, "全部点数重置完成!", 4);
	end
	char.complianceParameter(charaindex);
	char.sendStatusString(charaindex, "P");
	char.Skillupsend(charaindex);
	char.DelItem(charaindex, haveitemindex);
end

function main()
	item.addLUAListFunction( "ITEM_SetPlayerPoint", "SetPlayerPoint", "");
end
