function checkEmptItemNum(charaindex)
	EmptyItemNum = 0;
	for i = 9, 23 do
		if char.getItemIndex(charaindex, i) == -1 then
			EmptyItemNum = EmptyItemNum + 1;
		end
	end
	return EmptyItemNum;
end

--此lua是庄园骑证和邀请函时回调(kind为1时制作邀请函，kind为2时制作骑证 flg为庄园ID(1~10))
function FreeFameFeatures( meindex, kind, flg)
	print(meindex, kind, flg);
	local ridefame = 0;--制作骑证需要的声望
	local hanfame = 0;--制作邀请函需要的声望
	--char.TalkToCli(meindex, -1, "暂未开放!", "红色")
	if kind == 1 then
		if char.getInt(meindex, "声望") < 2000 then
			char.TalkToCli(meindex, -1, "你的声望小于".. 20 ..",所以无法制作骑证!", "黄色");
			return ;
		end
		if checkEmptItemNum(meindex) == 0 then
			char.TalkToCli(meindex, -1, "物品已满，请道具栏留有足够的空位！", "黄色");
			return ;
		end
		local familymap = char.getWorkInt( meindex, "家族地图");
		if familymap == 1041 then
			npc.AddItem(meindex, "19001");
		elseif familymap == 2031 then
			npc.AddItem(meindex, "19002");
		elseif familymap == 3031 then
			npc.AddItem(meindex, "19003");
		elseif familymap == 4031 then
			npc.AddItem(meindex, "19004");
		end
		char.setInt(meindex, "声望", char.getInt(meindex, "声望") - 2000);
		char.TalkToCli(meindex, -1, "制作成功，扣除"..ridefame.."声望!", "黄色");
		--char.TalkToCli(meindex, -1, "暂未开放!", "红色");
	elseif kind == 2 then
		if char.getInt(meindex, "声望") < ridefame*100 then
			char.TalkToCli(meindex, -1, "你的声望小于".. ridefame ..",所以无法制作骑证!", "黄色")
			return
		end
		if checkEmptItemNum(meindex) == 0 then
			char.TalkToCli(meindex, -1, "物品已满，请道具栏留有足够的空位！", "黄色")
			return
		end
		
		local familymap = char.getWorkInt( meindex, "家族地图");
		if char.getInt( meindex, "家族类型") == 0 then--光明
			if familymap == 1041 then
				npc.AddItem(meindex, "19005");
			elseif familymap == 2031 then
				npc.AddItem(meindex, "19006");
			elseif familymap == 3031 then
				npc.AddItem(meindex, "19007");
			elseif familymap == 4031 then
				npc.AddItem(meindex, "19008");
			elseif familymap == 5031 then
				npc.AddItem(meindex, "20224");
			elseif familymap == 6031 then
				npc.AddItem(meindex, "20225");
			elseif familymap == 7031 then
				npc.AddItem(meindex, "20226");
			elseif familymap == 8031 then
				npc.AddItem(meindex, "20227");
			elseif familymap == 9031 then
				npc.AddItem(meindex, "20228");
			elseif familymap == 7531 then
				npc.AddItem(meindex, "20766");
			end
		elseif char.getInt( meindex, "家族类型") == 1 then--黑暗
			if familymap == 1041 then
				npc.AddItem(meindex, "19005");
			elseif familymap == 2031 then
				npc.AddItem(meindex, "19006");
			elseif familymap == 3031 then
				npc.AddItem(meindex, "19007");
			elseif familymap == 4031 then
				npc.AddItem(meindex, "19008");
			elseif familymap == 5031 then
				npc.AddItem(meindex, "20224");
			elseif familymap == 6031 then
				npc.AddItem(meindex, "20225");
			elseif familymap == 7031 then
				npc.AddItem(meindex, "20226");
			elseif familymap == 8031 then
				npc.AddItem(meindex, "20227");
			elseif familymap == 9031 then
				npc.AddItem(meindex, "20228");
			elseif familymap == 7531 then
				npc.AddItem(meindex, "20766");
			end
		end
		
		if ridefame > 0 then
			char.setInt(meindex, "声望", char.getInt(meindex, "声望") - (ridefame*100));
			char.TalkToCli(meindex, -1, "制作成功，扣除"..ridefame.."声望!", "黄色");
		end
	end
	
end
