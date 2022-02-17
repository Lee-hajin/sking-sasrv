function ShowList(meindex, talkerindex)
	local token = "3\n" .. char.getChar(meindex, "名字") .. "\n         购买道具"
	local point = char.getInt(talkerindex, "积分")
	token = "0|1|0|" .. char.getChar(meindex, "名字") .. "|sdfsdf|本商店是积分商店，使用货币为积分\n你当前积分币："
	.. point .."|请输入你想购买的数量|您是否确定要买该物品？|您是否确定要买该物品？|您的道具栏已满|"
	for i = 1, table.getn(ItemList) do
		if ItemList[i][1] > -1 then
			token = token .. string.format( "%-18s %4d积分", item.getNameFromNumber(ItemList[i][1]), ItemList[i][2]);
			if point < ItemList[i][2] then
				token = token .. "|1|"
			else
				token = token .. "|0|"
			end
			token = token .. "0|0|" .. item.getgraNoFromITEMtabl(ItemList[i][1]) .. "|" .. item.getItemInfoFromNumber(ItemList[i][1]) .. "|-1|";
		end
	end

	lssproto.windows(talkerindex, "买道具框", "确定", 1, char.getWorkInt( meindex, "对象"), token)
end

function BuyItem(meindex, talkerindex, id, num)
	if num < 1 then
		return;
	end
	--对话框中选择确定
	local cost = ItemList[id][2];
	local mycost = char.getInt(talkerindex, "积分");
	if mycost >= cost * num then
		local icost = 0;
		local inum = 0;
		for i = 1, num do
			itemindex = char.Additem(talkerindex, ItemList[id][1]);
			if itemindex > -1 then
				item.UpdataItemOne(talkerindex, itemindex);
				char.setInt(talkerindex, "积分", mycost - cost);
				icost = icost + cost;
				inum = inum + 1;
			else
				char.TalkToCli(talkerindex, -1, "身上的道具已满了!!!", "黄色")
				break;
			end
		end
		char.TalkToCli(talkerindex, -1, "成功购买" .. inum .. "个 " .. item.getChar(itemindex, "名称") .. " 并扣除了" .. icost .. "积分!!!", "黄色");
		if icost > 0 then
			local logstr = os.date("%Y-%m-%d %X",os.time())
							.."|"..char.getChar(talkerindex, "账号")
							.."|"..char.getChar(talkerindex, "名字")
							.."|"..item.getNameFromNumber(ItemList[id][1]).."("..ItemList[id][1]..")"
							.."|"..inum--数量
							.."|"..icost--总价
							.."|"..tostring(mycost-cost)--剩余
							.."\n";
			local f = assert(io.open("./mylua/log/JFShop/"..os.date("%Y%m%d",os.time())..".log", "a"));
			f:write( logstr);
			f:close();
		end
	end
	char.Updata(talkerindex, "石币")
end

--NPC对话事件(NPC索引)
function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then 
		ShowList(meindex, talkerindex);
	end
end

--NPC窗口事件(NPC索引)
function WindowTalked ( _meindex, _talkerindex, _seqno, _select, _data)
	if _select == 2 then
		return
	end
	if _seqno == 1 then
		local id = other.atoi(other.getString(_data, "|", 1));
		local num = other.atoi(other.getString(_data, "|", 2));
		BuyItem(_meindex, _talkerindex, id, num)
	end
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function mydata()
	ItemList = {{3316,1000}
				,{3317,1000}
				,{3318,1000}
				,{3319,1000}
				};
	
	npcdata = {"积分商店",106020,777,47,29,4,"JFShop1"};
end

function reload(_charaindex, _data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	end
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
