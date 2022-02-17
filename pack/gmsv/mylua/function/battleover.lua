function FreeBattleOver(charaindex,alive,battletime,battleturn,battletype)
	--print("调试信息：",charaindex,alive,battletime,battleturn,battletype);
	--alive 0为战斗失败 1为战斗胜利
	--battletime  当场战斗的时间
	--battleturn  当场战斗回合数
	--battletype  1为PVE，2为PVP，3为EVE，4为观战
	--战后掉落
	--掉落数据(地图，是否需要携带道具,掉落表,几率百分比,几率范围)
	local itemdata = {{100,2241,{2242,2243,2244},3,100}--2/100的几率掉落MAX证书
					 ,{60050,2241,{2245},3,100}--雷龙的骨头
					 ,{60051,2241,{2245},3,100}--雷龙的骨头
					 ,{60051,2241,{2245},3,100}--雷龙的骨头
					 };
	local mymap = char.getInt(charaindex,"地图号");
	if alive == 1 then--战斗胜利才触发
		if battletype == 1 then--如果为PVE才触发
			for i=1,table.getn(itemdata) do
				if mymap == itemdata[i][1] then
					local itemcf = true;
					if itemdata[i][2] >= 0 then--判断是否携带道具
						if char.Finditem(charaindex,itemdata[i][2]) == -1 then
							itemcf = false;
						end
					end
					if itemcf == true then
						local myrand = math.random(1,itemdata[i][5]);
						--print(myrand,itemdata[i][4]);
						if myrand <= itemdata[i][4] then--判断几率
							local randid = math.random(1,table.getn(itemdata[i][3]));
							local itemindex = char.Additem( charaindex, itemdata[i][3][randid]);
							char.TalkToCli(charaindex, -1, "得到物品: ["..item.getChar(itemindex,"名称").."]", 4);
						end
					end
				end
			end
		end
	end
	--战后恢复
	for i=1,table.getn(healmap) do
		if mymap == healmap[i] then
			char.HealAll(charaindex);
			char.TalkToCli(charaindex, -1, "[温馨提示]当前地图设置有战后自动治疗功能 满血满状态原地恢复哦！o(∩_∩)o", 4);
		end
	end
	return 1;
end



function mydata()
	--完全恢复地图
	healmap = {2005};
end

function reload(charaindex, data)
	mydata();
end

function main()
	mydata();
	magic.addLUAListFunction("重读战后事件", "reload", "", 3, "[重读战后事件]");
end
