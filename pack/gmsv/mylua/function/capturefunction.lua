function CaptureCheckFunction( attackindex, defindex )
	local floorid = char.getInt(attackindex, "地图号");
	if floorid == 2015 then
		return 1;
	end
	for i=1, table.getn(NeedEnemy) do
		if NeedEnemy[i][1] == char.getInt(defindex, "宠ID") then
			for j=1, table.getn(NeedEnemy[i][2]) do
				if NeedEnemy[i][2][j] > -1 then
					if npc.Free(-1, attackindex, "ITEM=" .. NeedEnemy[i][2][j]) == 0 then
						char.TalkToCli(attackindex, -1, "【温馨提示】由于您缺少[" .. item.getNameFromNumber(NeedEnemy[i][2][j]) .. "]导致无法捕捉成功[" .. char.getChar(defindex, "名字") .. "]。", "黄色")
						return 0;
					end
				end
			end
		end
	end
	return 1;
end

function CaptureOkFunction( attackindex, defindex )
	local floorid = char.getInt(attackindex, "地图号");
	local CapFlg = false;
	
	for i=1, table.getn(NeedEnemy) do
		if NeedEnemy[i][1] == char.getInt(defindex, "宠ID") then
			for j=1, table.getn(NeedEnemy[i][2]) do
				if NeedEnemy[i][2][j] > -1 then
					
					npc.DelItem(attackindex, NeedEnemy[i][2][j] .. "*1");
				end
			end
			return;
		end
	end
end


function mydata()
	--地图,道具,抓宠所需道具, 宠物列表
	rarepetid1 = {{125, 19641, {{-1, -1}}}
				 ,{126, 19642, {{-1,-1}}}
				 ,{127, 19643, {{1794,23102}, {1795,23103}, {1796,23104}, {1797,23105}}}
				 ,{128, 19644, {{1794,23102}, {1795,23103}, {1796,23104}, {1797,23105}}}
				 };
	--1宠物ID,2抓宠物删除道具编号,3不限的物品扣除地图
	NeedEnemy = {{	524,{2456},{}}--卡特力奴
				,{  961,{20219},{}}--艾蜜/玛丽娜丝庄园旅游指南
				,{  953,{20223},{}}--雷诛/布伊比的牙
				,{  962,{20222},{}}--讦谯龙/收据
				,{  777,{20253},{}}--佩露夏/魔法钻戒[水LV3-1]
				,{  796,{20247},{}}--嘎吱拉/魔法钻戒[地LV3-1]
				,{  812,{20259},{}}--斑尼迪克/魔法钻戒[火LV3-1]
				,{  1105,{1690,1691,1692},{}} --夏普德
				,{  8,{1810},{}}--巨大玛恩摩
				,{  145,{2236},{2015}}--双头狼
				,{  146,{2236},{2015}}--双头狼
				,{ 5041,{23025},{}}--帖拉鲁卡
				,{ 5042,{23026},{}}--帖拉多斯
				,{ 5043,{23027},{}}--帖拉尼索
				,{ 5044,{23028},{}}--帖拉拉普
				};

end

function reload()
	print("重读捕抓限制完成！");
	mydata();
end

function main()
	mydata();
	magic.addLUAListFunction("重读捕抓限制", "reload", "", 1, "[重读捕抓限制]");
end


